/**
 * @file   StrictMorphShadow.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed May 14 19:29:09 2008
 * 
 * @brief  Strict (dictionnary) morphology class (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_inttype.h>
#include <_include/_string.h>
#include <_include/_utf8.h>

#include <lib/libpts2/PatriciaTree.h>
#include <lib/libpts2/PatriciaStorageHandler.h>
#include "LingProc4/UCharSet/UCharSet.h"
#include "LingProc4/LexID64.h"

#include "LexUnit.h"
#include "Lexeme.h"
#include "StrictMorphShadow.h"
#include "StrictMorphParamsMap.h"
#include "PriorityQueue.h"
#include "DecompositionProcessor.h"

/* StrictMorphShadow implementation */

StrictMorphShadow::StrictMorphShadow() :
	ready( false ),
	header(),
	dictInfo(),
	charset( 0 ),
        normalizer( 0 ),
	paradigms( 0 ),
	paradigmForms( 0 ),
	wordForms( 0 ),
	lexemes( 0 ),
	lexStemsList( 0 ),
        lexGramDescsIdx( 0 ),
        lexGramDescs( 0 ),
	stemsTree( 0 ),
	flexDistribs(),
	flexions(),
	lexOrthVariants(),
        excludeIndex(),
        excludeChains()
{
}

StrictMorphShadow::~StrictMorphShadow()
{
	if ( likely(stemsTree != 0) )
		delete stemsTree;
        if ( likely(normalizer != 0) )
                delete normalizer;
}

StrictMorphErrors StrictMorphShadow::create( const UCharSet			*charset,
					     const StrictMorphParams		&params,
					     const StrictMorphMemoryHandler	&handler,
					     int				&underlyingError )
{
	StrictMorphErrors status;
	underlyingError = STRICT_OK;
	
        if ( unlikely(charset == 0) )
		return STRICT_ERROR_INVALID_CHARSET;
	this->charset = charset;
	
	/* Create the common part first */
	status = createCommon(handler, params, underlyingError);
	if ( unlikely(status != STRICT_OK) )
		return status;

	/* Create the stems tree */
	status = createStemsTree(handler, underlyingError);
	if ( unlikely(status != STRICT_OK) )
		return status;
	
	return STRICT_OK;
}

PatriciaTree *StrictMorphShadow::createPatriciaTree()
{
	return new PatriciaTree();
}

StrictMorphErrors StrictMorphShadow::createStemsTree( const StrictMorphMemoryHandler &handler,
						      int			     &underlyingError )
{
	underlyingError = STRICT_OK;
	
	PatriciaStorageHandler patriciaHandler;

	/* Copy the StrictMorphMemoryHandler data to PatriciaStorageHandler */

	/* Header */
	patriciaHandler.chunks[PATRICIA_CHUNK_HEADER].buffer =
		reinterpret_cast<const uint8_t *>(handler.stemsTreeHeader.buffer);
	patriciaHandler.chunks[PATRICIA_CHUNK_HEADER].size   =
		handler.stemsTreeHeader.size;

	/* Data */
	patriciaHandler.chunks[PATRICIA_CHUNK_DATA].buffer =
		reinterpret_cast<const uint8_t *>(handler.stemsTreeData.buffer);
	patriciaHandler.chunks[PATRICIA_CHUNK_DATA].size   =
		handler.stemsTreeData.size;

	/* Crosslinks */
	patriciaHandler.chunks[PATRICIA_CHUNK_CROSSLINKS].buffer =
		reinterpret_cast<const uint8_t *>(handler.stemsTreeCrosslinks.buffer);
	patriciaHandler.chunks[PATRICIA_CHUNK_CROSSLINKS].size   =
		handler.stemsTreeCrosslinks.size;

	stemsTree = createPatriciaTree();

	PatriciaTreeCore::ResultStatus status =	
		stemsTree->setStorageHandler( StrictMorphShadow::patriciaCreateFlags, patriciaHandler );
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
	{
		underlyingError = static_cast<int>(status);
		return STRICT_ERROR_STEMS_LOAD_FAILURE;
	}
	
	return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::createCommon( const StrictMorphMemoryHandler &handler,
						   const StrictMorphParams        &params,
						   int                            &underlyingError )
{
	underlyingError = STRICT_OK;
        
	/* Check if already created */
	if ( unlikely( ready ) )
		return STRICT_ERROR_ALREADY_CREATED;

        avector<uint8_t>        versionHackBuf;
        StrictMorphStorageChunk versionHackChunk;
        
	/* Load the header information */
	StrictMorphStorageChunk::Status status = header.load(handler.header);
	if ( unlikely(status != StrictMorphStorageChunk::SUCCESS) )
        {
                /* Lex gram descriptions hack for backward compatibility:
                 * Assume major version 1 and zero size gram descriptions section for success */
                if ( status == StrictMorphStorageChunk::ERROR_VERSION_MISMATCH )
                {
                        if ( header.workFormatMajor       == 0 &&
                             strictMorphWFMajor           == 2 &&
                             handler.lexGramDescList.size == 0 &&
                             handler.dictInfo.size        < sizeof(uint32_t) * DictionnaryInfo::infoFieldsCount )
                        {
                                versionHackBuf.resize( sizeof(uint32_t) * DictionnaryInfo::infoFieldsCount );
                                memset( versionHackBuf.get_buffer(), 0, versionHackBuf.size() );
                                memcpy( versionHackBuf.get_buffer(), handler.dictInfo.buffer, handler.dictInfo.size );
                                versionHackChunk.buffer = versionHackBuf.get_buffer();
                                versionHackChunk.size   = versionHackBuf.size();
                        }
                        else
                                return STRICT_ERROR_HEADER_LOAD_FAILURE;
                }
                else 
                        return STRICT_ERROR_HEADER_LOAD_FAILURE;
        }
	
	/* Verify and update the header */
	if ( unlikely(params.language != header.dict.lang) )
		return STRICT_ERROR_INVALID_LANGUAGE;
	
	StrictMorphErrors mapStatus;
	unsigned int flags = MORPH_FLAG_USE_DEFAULT;
	mapStatus = mapStrictMorphParamsFlags(params.flags, flags);
	if ( unlikely(mapStatus != STRICT_OK) )
		return mapStatus;
	if ( flags == MORPH_FLAG_USE_DEFAULT )
		flags = MORPH_FLAG_LWR_PUNCT | MORPH_FLAG_REM_IGNORE;
	header.dict.creationFlags = flags;
	header.dict.minStemLength = params.minStemLength;
        header.dict.minCompoundStemLength = params.minCompoundStemLength;
        
        /* Create the normalizer */
        normalizer = WordNormalizer::factory( charset->GetLang(), MORPH_TYPE_STRICT );
        if ( unlikely(normalizer == 0) )
                return STRICT_ERROR_INTERNAL_ERROR;
        
        WordNormalizerResult normalizerResult = normalizer->create( charset, header.dict.creationFlags & ~MORPH_FLAG_CASE_INSENSITIVE );
        if ( unlikely(normalizerResult != NORMALIZE_SUCCESS) )
                return STRICT_ERROR_INTERNAL_ERROR;
        
	if ( params.internalNumber != 0 )
		header.dict.internalNumber = params.internalNumber;
	
	/* Load the dictionary information */
        
	status = dictInfo.load( (versionHackBuf.size() > 0 ? versionHackChunk : handler.dictInfo) );
	if ( unlikely(status != StrictMorphStorageChunk::SUCCESS) )
		return STRICT_ERROR_DICT_INFO_LOAD_FAILURE;

	/* Load paradigms */
	if ( unlikely(handler.paradigms.size != dictInfo.paradigmsCount * sizeof(Paradigm)) )
		return STRICT_ERROR_PARADIGMS_LOAD_FAILURE;
	paradigms = reinterpret_cast<const Paradigm *>(handler.paradigms.buffer);

	/* Load paradigm forms */
	if ( unlikely(handler.paradigmForms.size != dictInfo.paradigmFormsCount * sizeof(ParadigmForm)) )
		return STRICT_ERROR_PARADIGM_FORMS_LOAD_FAILURE;
	paradigmForms = reinterpret_cast<const ParadigmForm *>(handler.paradigmForms.buffer);
	
	/* Load word forms */
	if ( unlikely(handler.wordForms.size != dictInfo.wordFormsCount * sizeof(WordForm)) )
		return STRICT_ERROR_WORD_FORMS_LOAD_FAILURE;
	wordForms = reinterpret_cast<const WordForm *>(handler.wordForms.buffer);

	/* Load flexions distributions */
	if ( unlikely(handler.flexDistribs.size !=
		      dictInfo.flexCount * dictInfo.flexDistribCount * sizeof(FlexDistrib)) )
		return STRICT_ERROR_FLEX_DISTRIB_LOAD_FAILURE;
	flexDistribs.assign( reinterpret_cast<const FlexDistrib *>(handler.flexDistribs.buffer),
			     dictInfo.flexCount );

	/* Load flexions */
	if ( unlikely(handler.flexions.size != dictInfo.flexCount * FlexionsArray::maxFlexLength) )
		return STRICT_ERROR_FLEXIONS_LOAD_FAILURE;
	if ( unlikely(flexions.assign( reinterpret_cast<const char *>(handler.flexions.buffer),
				       dictInfo.flexCount ) != 0) )
		return STRICT_ERROR_ALLOCATION_ERROR;

	/* Load lexemes */
	if ( unlikely(handler.lexemes.size != dictInfo.lexemesCount * sizeof(Lexeme)) )
		return STRICT_ERROR_LEXEMES_LOAD_FAILURE;
	lexemes = reinterpret_cast<const Lexeme *>(handler.lexemes.buffer);

	/* Load lexemes grammatical descriptions list */
	if ( unlikely(handler.lexGramDescList.size != dictInfo.lexGramDescListLength) )
		return STRICT_ERROR_LEX_GDESC_LIST_LOAD_FAILURE;
        if ( handler.lexGramDescList.size > 0 )
        {
                lexGramDescsIdx = reinterpret_cast<const CommonReference<uint32_t> *>(handler.lexGramDescList.buffer);
                lexGramDescs    = reinterpret_cast<const LexGramDescShadow *>( lexGramDescsIdx + (lexGramDescsIdx->getReference() + 1) );
        }

	/* Load lexemes stems backlinks list */
	if ( unlikely(handler.lexStemsList.size != dictInfo.lexStemsListLength * sizeof(StemId)) )
		return STRICT_ERROR_LEX_STEMS_LIST_LOAD_FAILURE;
	lexStemsList = reinterpret_cast<const StemId *>(handler.lexStemsList.buffer);
	
	/* Load lexemes orthographic variants list */
	if ( unlikely(handler.lexOrthVariants.size != dictInfo.lexOrthVarsListLength * sizeof(LexemeId)) )
		return STRICT_ERROR_LEX_ORTH_LIST_LOAD_FAILURE;
	if ( unlikely(lexOrthVariants.assign( reinterpret_cast<const LexemeId *>(handler.lexOrthVariants.buffer),
					      handler.lexOrthVariants.size / sizeof(LexemeId) ) != 0) )
		return STRICT_ERROR_ALLOCATION_ERROR;
        
        /* Load compounds exclude dictionary chains */
	if ( unlikely(handler.excludeChains.size != dictInfo.excludeChainsLength * sizeof(uint64_t)) )
		return STRICT_ERROR_EXCLUDE_CHAINS_LOAD_FAILURE;
        excludeChains = reinterpret_cast<ExcludeChain>(handler.excludeChains.buffer);
        
        /* Load compounds exclude dictionary index */
        if ( handler.excludeIndex.size  > 0 && handler.excludeIndex.buffer != 0  )
                excludeIndex.setBuffer( const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(handler.excludeIndex.buffer)) );
        
        ready = true;
	return STRICT_OK;
}

const ParadigmForm *StrictMorphShadow::getParadigmForm( ParadigmFormId id ) const
{
	if ( unlikely(id >= dictInfo.paradigmFormsCount) )
		return 0;
	return paradigmForms + id;
}

const Paradigm *StrictMorphShadow::getParadigm(ParadigmType type) const
{
	if ( unlikely(type >= dictInfo.paradigmsCount) )
		return 0;
	return paradigms + type;
}

StrictMorphErrors StrictMorphShadow::enumerate( StrictLex::LemmatizeFunctor &enumerator ) const
{
	/* Cycle through all lexemes */
	size_t limit = dictInfo.lexemesCount;
	for (size_t i = 0; i < limit; i++)
	{
		const Lexeme *lex = lexemes + i;
                
                /* Mess with orth variant: skip them */
                if ( !lex->isMainOrthVariant() && !lex->hasOrthVariants() )
                        continue;
                
                LexUnit unit( *this,
                              static_cast<LexemeId>(i),
                              0,
                              StrictLex::CAPS_NO_CASE,
                              header.dict.internalNumber,
                              PARADIGM_FORM_EMPTY,
                              0 );
                
		if ( likely(!lex->isDummy()) )
                {
                        StrictLex strictLex;
                        strictLex.assignShadow( &unit );

                        if ( unlikely( enumerator.apply(strictLex) != 0 ) )
                                return STRICT_ERROR_ENUMERATION_STOPPED;
                }

                class OrthEnumerator : public StrictLex::OrthEnumFunctor
                {
                public:
                        OrthEnumerator( const StrictMorphShadow     &_morph,
                                        LexemeId                     _lexId,
                                        unsigned int                 _morphNumber,
                                        StrictLex::LemmatizeFunctor &_enumerator ) :
                                enumerator( _enumerator ),
                                unit( _morph, _lexId, 0, StrictLex::CAPS_NO_CASE, _morphNumber, PARADIGM_FORM_EMPTY, 0 )
                        {
                        }
                        
                        StrictMorphErrors apply( const StrictLexOrthVariant &var )
                        {
                                unit.lexOrthVariant = var.orthVariant;
                                
                                StrictLex strictLex;
                                strictLex.assignShadow( &unit );
                                
                                int status = enumerator.apply( strictLex );
                                if ( status != 0 )
                                        return STRICT_ERROR_ENUMERATION_STOPPED;
                                return STRICT_OK;
                        }

                private:
                        StrictLex::LemmatizeFunctor &enumerator;
                        LexUnit                      unit;
                } orthFunctor( *this, static_cast<LexemeId>(i), header.dict.internalNumber, enumerator );
                
                if ( lex->hasOrthVariants() )
                {
                        StrictMorphErrors status = queryOrthVariants( orthFunctor, unit );
                        if ( unlikely(status != STRICT_OK) )
                                return status;
                }
	}
	
	return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::normalizeWord( WordNormalizerFunctor    &functor,
                                                    const char               *word,
                                                    size_t                    wordLength,
                                                    unsigned int              flags ) const
{
        unsigned int localFlags = header.dict.creationFlags;
        if ( flags != StrictMorphParams::FlagUseDefault )
        {
                StrictMorphErrors status = mapStrictMorphParamsFlags( flags, localFlags );
                if ( unlikely(status != STRICT_OK) )
                        return status;
        }
        
        WordNormalizerResult status = normalizer->normalizeWord( functor,
                                                                 static_cast<uint32_t>(localFlags & ~MORPH_FLAG_CASE_INSENSITIVE),
                                                                 word,
                                                                 wordLength );
        switch ( status )
        {
        case NORMALIZE_SUCCESS:
                break;
        
        case NORMALIZE_ERROR_PARAMETER:                
        case NORMALIZE_ERROR_FLAGS:
        case NORMALIZE_ERROR_INTERNAL:
                return STRICT_ERROR_INTERNAL_ERROR;
        
        case NORMALIZE_ERROR_BUFFER:
                return STRICT_ERROR_ALLOCATION_ERROR;
        
        case NORMALIZE_ERROR_NOT_NATIVE:
                return STRICT_ERROR_NOT_NATIVE_SYMBOL;
                
        case NORMALIZE_ERROR_FUNCTOR:
                return STRICT_ERROR_ENUMERATION_FUNCTOR;
        };
        
        return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::lemmatize( StrictLex::LemmatizeFunctor &functor,
						const char                  *word,
						size_t                       wordLength,
                                                unsigned int                 flags ) const
{
        unsigned int localFlags = header.dict.creationFlags;
        if ( flags != StrictMorphParams::FlagUseDefault )
        {
                StrictMorphErrors status = mapStrictMorphParamsFlags( flags, localFlags );
                if ( unlikely(status != STRICT_OK) )
                        return status;
        }
        
        return lemmatizeCompound( functor, word, wordLength, localFlags, 0, 0 );
}

inline int getCapsWeigth( StrictLex::DictCaps caps )
{
        switch (caps)
        {
        case StrictLex::CAPS_NO_CASE:    return 0;
        case StrictLex::CAPS_LOWER_CASE: return 1;
        case StrictLex::CAPS_TITLE_CASE: return 2;
        case StrictLex::CAPS_UPPER_CASE: return 3;
        default:                         return -1;
        };
}


StrictMorphErrors StrictMorphShadow::lemmatizeCompound( StrictLex::LemmatizeFunctor &functor,
                                                        const char                  *word,
                                                        size_t                       wordLength,
                                                        unsigned int                 localFlags,
                                                        const StrictLex             *leftCompound,
                                                        size_t                       compoundBound ) const
{ 
	char buf[ StrictMorphShadow::maxLemmatizeWordLength ];
        
        WordNormalizerTarget  f( reinterpret_cast<char *>(buf), StrictMorphShadow::maxLemmatizeWordLength );
        WordNormalizerResult  status = normalizer->normalizeWord( f,
                                                                  static_cast<uint32_t>(localFlags & ~MORPH_FLAG_CASE_INSENSITIVE),
                                                                  word,
                                                                  wordLength );
        switch ( status )
        {
        case NORMALIZE_SUCCESS:
                break;
        
        case NORMALIZE_ERROR_PARAMETER:                
        case NORMALIZE_ERROR_FLAGS:
        case NORMALIZE_ERROR_INTERNAL:
                return STRICT_ERROR_INTERNAL_ERROR;
        
        case NORMALIZE_ERROR_BUFFER:
		return STRICT_OK;
        
        case NORMALIZE_ERROR_NOT_NATIVE:
                return STRICT_ERROR_NOT_NATIVE_SYMBOL;
                
        case NORMALIZE_ERROR_FUNCTOR:
                return STRICT_ERROR_ENUMERATION_FUNCTOR;
        };
        
	wordLength = f.resultSize;
	
	/* If empty word */
	if (wordLength == 0)
		return STRICT_OK;
        
        if ( wordLength < StrictMorphShadow::maxLemmatizeWordLength )
                buf[wordLength] = '\0';
        
	/* Setup capitalization scheme */
        StrictLex::DictCaps currentCaps = StrictLex::CAPS_NO_CASE;
        switch (f.caps)
        {
        case NORMALIZE_CAPS_NO_CASE:    currentCaps = StrictLex::CAPS_NO_CASE;    break;
        case NORMALIZE_CAPS_LOWER_CASE: currentCaps = StrictLex::CAPS_LOWER_CASE; break;
        case NORMALIZE_CAPS_TITLE_CASE: currentCaps = StrictLex::CAPS_TITLE_CASE; break;
        case NORMALIZE_CAPS_UPPER_CASE: currentCaps = StrictLex::CAPS_UPPER_CASE; break;
        };

        return lemmatizeCompound( functor,
                                  reinterpret_cast<const uint8_t *>(buf),
                                  currentCaps,
                                  wordLength,
                                  localFlags,
                                  leftCompound,
                                  compoundBound,
                                  0,
                                  0 );
}
        
StrictMorphErrors StrictMorphShadow::lemmatizeCompound( StrictLex::LemmatizeFunctor &functor,
                                                        const uint8_t               *key,
                                                        StrictLex::DictCaps          currentCaps,
                                                        size_t                       wordLength,
                                                        unsigned int                 localFlags,
                                                        const StrictLex             *leftCompound,
                                                        size_t                       compoundBound,
                                                        const uint64_t              *excludeFilters,
                                                        size_t                       currentDepth  ) const
{
        if ( currentDepth >= maxCompoundRecurseDepth )
                return STRICT_OK;
        
        /* Create word parts accumulator */
        PriorityQueue queue( *stemsTree );
        
        /* Define and create patricia tree result accumulating functor */
        class PartsCollector : public CrossPathFunctor
        {
        public:
                PartsCollector( PriorityQueue &_queue ) :
                        queue( _queue ),
                        status( STRICT_OK )
                {
                }
                ~PartsCollector() { }

        public:
                int nextPart(const uint8_t* /* part */, const uint8_t *data, size_t dataSize, bool isExactMatch)
                {
                        /* Create accessor on the data */
                        StemRecordsArray accessor( data, dataSize );

                        /* Iterate and collect the stems */
                        for ( size_t stemIdx = 0; stemIdx < accessor.getRecordsCount(); stemIdx++ )
                        {
                                status = queue.push( data, stemIdx, isExactMatch );
                                if ( unlikely(status != STRICT_OK ) )
                                        return 1;
                        }
                        
                        return 0;
                }

                StrictMorphErrors getStatus() const { return status; }

        private:
                PriorityQueue     &queue;
                StrictMorphErrors  status;
        } collector( queue );

        /* Apply the accumulating functor for the tree lookup */
        PatriciaTreeCore::ResultStatus lookupStatus = stemsTree->lookupAll( key, collector );
        if ( unlikely(lookupStatus != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
                return STRICT_ERROR_INTERNAL_ERROR;
        if ( unlikely(collector.getStatus() != STRICT_OK) )
                return collector.getStatus();
        
        /* Define and create current decomposition processor */
        DecompositionProcessor processor( functor );
        
	LexemeId excludeFilterLexId = 0;
        if ( excludeFilters != 0 && *excludeFilters != 0 )
                excludeFilterLexId = static_cast<LexemeId>( LexId(*excludeFilters) );
        
        /* Having the filled queue with the word decomposition (from left to right) iterate through it */
        for ( PriorityQueue::Iterator it = queue.first(); !queue.isEnd(it); it = queue.next(it) )
        {
                /* Retrieve the left and right parts of the word */
                const uint8_t       *data        = queue.getData(it);
                size_t               stemIdx     = queue.getStemIdx(it);
                uint32_t             nodeLink    = queue.getNodeLink(it);
                
                /* Initialize stem data accessor */
                LexUnit   lex( *this, data, stemsTree->getDataSize(data) );
                StrictLex strictLex;
                strictLex.assignShadow( &lex );
                                
                const char          *leftWord    = reinterpret_cast<const char *>(queue.getKey(it));
                size_t               leftLength  = queue.getKeyLength(it);
                const char          *rightWord   = reinterpret_cast<const char *>(key + leftLength);
                size_t               rightLength = wordLength - leftLength;

                /* Fill the lex information known at the current moment */
                lex.leftCompound     = leftCompound;
                lex.lexCaps          = currentCaps;
                lex.morphNumber      = header.dict.internalNumber;
                lex.stemText         = leftWord;
                lex.lexId            = lex.getLexId( stemIdx );

                if ( localFlags & MORPH_FLAG_PROCESS_COMPOUNDS )
                {
                        /* Check if current stem cannot be a part of a compound word */
                        if ( lex.checkRestrictBit() && leftCompound != 0 )
                                continue;

                        /* Check if current stem has a special decomposition rule */
                        if ( excludeFilterLexId == 0 && lex.checkExcludeBit() )
                        {
                                /* Perform exclude dictionary lookup */
                                uint32_t *poolOffset = 0;
                                ExcludeIndex::ResultStatus lookupStatus = excludeIndex.get( nodeLink, &poolOffset );
                                if ( lookupStatus == ExcludeIndex::RESULT_STATUS_SUCCESS )
                                {
                                        /* Lookup succeeded, setup filters and proceed */
                                        excludeFilters     = excludeChains + (*poolOffset);
                                        excludeFilterLexId = static_cast<LexemeId>( LexId(*excludeFilters) );
                                }
                        }
                }

                /* Follow exclude lexid filtering if active */
                if ( excludeFilterLexId != 0 && lex.lexId != excludeFilterLexId )
                        continue;
                
                /* Check the case if need to */
                StrictLex::DictCaps dictCaps = lex.getLexCaps( stemIdx );
                if ( !(localFlags & MORPH_FLAG_CASE_INSENSITIVE) &&
                     getCapsWeigth(currentCaps) < getCapsWeigth(dictCaps) )
                        continue;

                /* Retrieve the flexions distribution */
                FlexDistribId distribId = lex.getFlexDistribId( stemIdx );
                
                /* The standard lemmatization part */
                bool lemmatizationPerformed = false;
                processor.clearInvokeFlag();
                
                for (;;)
                {
                        /* Perform normal lemmatization */
                        size_t      stemLength    = leftLength;
                        const char *flexion       = rightWord;
                        size_t      flexionLength = rightLength;

                        /* Check if a part found is smaller than minimum stem length allowed or
                         * a flexion is greater than maximum allowed */
                        size_t leftLengthSymbols = countUtf8( leftWord, stemLength );
                        if ( ( leftLengthSymbols < header.dict.minStemLength ) ||
                             ( countUtf8( flexion, flexionLength ) > dictInfo.maxFlexLength ) ||
                             ( ( leftCompound != 0 ) && ( leftLengthSymbols < header.dict.minCompoundStemLength ) ) )
                                break;

                        /* Obtain a flexion index */
                        FlexId flexIdx = flexions.getIndex(flexion);
                        if ( flexIdx == static_cast<FlexId>(-1) )
                                break;
                
                        /* Check if the current combination is OK */
                        WordFormListIdx formsListIdx = flexDistribs.getWordFormListIdx(distribId, flexIdx);
                        if ( likely(formsListIdx == static_cast<WordFormListIdx>(-1)) )
                                break;
                
                        /* Fill the lex unit */
                        lex.compoundBound  = compoundBound + leftLength + flexionLength;
                        lex.lexOrthVariant = lex.getLexOrthNumber( stemIdx );
                        lex.stemIndex      = stemIdx;
                        lex.listIdx        = formsListIdx;
                        lex.flexId         = flexIdx;
                
                        /* Invoke functor */
                        int result = processor.apply( strictLex );
                        if ( unlikely(result != 0) )
                                return STRICT_ERROR_ENUMERATION_STOPPED;
                        
                        lemmatizationPerformed = true;
                        break;
                }

                if ( lemmatizationPerformed )
                        continue;
                
                /* Standart lemmatization failed: continue with the compounds processing part */
                processor.clearInvokeFlag();

                while ( ( localFlags & MORPH_FLAG_PROCESS_COMPOUNDS ) &&
                        ( countUtf8( leftWord ) >= header.dict.minCompoundStemLength ) )
                {
                        /* Compounds are on, try to decompose the word first */
                                                
                        /* Check if current stem cannot be a part of a compound word */
                        if ( lex.checkRestrictBit() )
                                break;
                        
                        /* Check if the left part has a compound flexion */
                        if ( ! lex.checkCompositeBit(stemIdx) )
                                break;

                        /* Prepare the buffer for compound flexion */
                        const size_t compoundFlexionBufferSize = 32;
                        char compoundFlexion[ compoundFlexionBufferSize ];

                        /* Loop among compound flexions */
                        size_t limit = flexions.getMaxCompoundLength();
                        if ( limit > rightLength )
                                limit = rightLength;
                        if ( limit > compoundFlexionBufferSize - 1 )
                                limit = compoundFlexionBufferSize - 1;

                        size_t i, j, k;
                        for ( i = j = k = 0; i <= limit; i = j )
                        {
                                /* Do not process one-symbol words */
                                if ( j >= rightLength )
                                        break;
                                
                                for ( ;; )
                                {
                                        compoundFlexion[ k ] = '\0';

                                        /* Receive a compound flexion id */
                                        FlexId compoundFlexIdx = flexions.getCompoundIndex( compoundFlexion );
                                        if ( compoundFlexIdx == static_cast<FlexId>(-1) )
                                                break;

                                        /* Check if the current combination is OK */
                                        WordFormListIdx formsListIdx = flexDistribs.getWordFormListIdx(distribId, compoundFlexIdx);
                                        if ( formsListIdx == static_cast<WordFormListIdx>(-1) )
                                                break;

                                        /* Construct the lex unit */
                                        lex.compoundBound  = compoundBound + leftLength + i;
                                        lex.lexOrthVariant = lex.getLexOrthNumber( stemIdx );
                                        lex.stemIndex      = stemIdx;
                                        lex.listIdx        = formsListIdx;
                                        lex.flexId         = compoundFlexIdx;
                                
                                        /* Transfer capitalization flag */
                                        StrictLex::DictCaps compoundCaps;
                                        switch (currentCaps)
                                        {
                                        case StrictLex::CAPS_LOWER_CASE:
                                        case StrictLex::CAPS_TITLE_CASE: compoundCaps = StrictLex::CAPS_LOWER_CASE; break;
                                        case StrictLex::CAPS_UPPER_CASE: compoundCaps = StrictLex::CAPS_UPPER_CASE; break;
                                        default:                         compoundCaps = StrictLex::CAPS_NO_CASE;
                                        };
                                
                                        /* Recurse to the right */
                                        StrictMorphErrors status =
                                                lemmatizeCompound( processor,
                                                                   reinterpret_cast<const uint8_t *>(rightWord + i),
                                                                   compoundCaps,
                                                                   rightLength - i,
                                                                   localFlags,
                                                                   &strictLex,
                                                                   lex.compoundBound,
                                                                   ( excludeFilters == 0 ? 0 : excludeFilters + 2 ),
                                                                   currentDepth + 1 );
                                        if ( unlikely(processor.getStatus() != STRICT_OK) )
                                                return processor.getStatus();
                                        if ( unlikely(status != STRICT_OK) )
                                                return status;
                                        break;
                                }

                                if ( unlikely(k >= compoundFlexionBufferSize) )
                                        break;
                                
                                uint32_t ch;
                                U8_NEXT_UNSAFE( rightWord, j, ch );
                                U8_APPEND_UNSAFE( compoundFlexion, k, ch );
                        }

                        break;
                }
        }
	
	return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::requestLexeme( LexemeId lexId, const Lexeme *&lexeme ) const
{
	if ( unlikely(lexId >= dictInfo.lexemesCount) )
		return STRICT_ERROR_INVALID_LEX_UNIT_LEXID;
	
	/* Position to the lexeme */
	lexeme = lexemes + lexId;

	/* Check if lexeme is blank */
	if ( unlikely(lexeme->isDummy()) )
		return STRICT_ERROR_INVALID_LEX_UNIT_DUMMY;

	return STRICT_OK;
}



StrictMorphErrors StrictMorphShadow::requestTreeNode( StemId          stemId,
						      const uint8_t *&nodeData,
						      size_t         &nodeDataSize ) const
{
	StemNodeRef    nodeRef    = getStemNodeRef(stemId);

	/* Access the data within a node in patricia tree */
	nodeData = stemsTree->linkToDataAddress( static_cast<uint32_t>(nodeRef) );
	if ( unlikely(nodeData == 0) )
		return STRICT_ERROR_TREE_NODE_ACCESS_FAILED;

	nodeDataSize = stemsTree->getDataSize( nodeData );
	if ( unlikely(nodeDataSize == static_cast<size_t>(-1)) )
		return STRICT_ERROR_TREE_NODE_ACCESS_FAILED;

	return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::hasOrthVariants( LexemeId lexNum, bool &hasMore ) const
{
	/* Position to the lexeme */
	const Lexeme *lexeme = 0;
	StrictMorphErrors status = requestLexeme(lexNum, lexeme);
	if ( unlikely(status != STRICT_OK) )
		if ( status != STRICT_ERROR_INVALID_LEX_UNIT_DUMMY )
			return status;
        
        if ( status != STRICT_ERROR_INVALID_LEX_UNIT_DUMMY )
        {
                if ( lexeme->isMainOrthVariant() )
                {
                        hasMore = false;
                        return STRICT_OK;
                }
        
                if ( lexeme->hasOrthVariants() )
                {
                        hasMore = true;
                        return STRICT_OK;
                }
        
                return STRICT_ERROR_SUPPLEMENT_VARIANT_PROVIDED;
        }

	/* Search for the variant */
        hasMore = true;
	const LexemeId **orthListPtr = 0;
        OrthVariantList::ResultStatus searchResult = lexOrthVariants.get( lexNum, &orthListPtr );
	if ( searchResult == OrthVariantList::RESULT_STATUS_NOT_FOUND )
                hasMore = false;

        return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::getOrthVariantsList( LexemeId &lexNum, size_t orthVar, const LexemeId *&orthList ) const
{
	/* Position to the lexeme */
	const Lexeme *lexeme = 0;
	StrictMorphErrors status = requestLexeme(lexNum, lexeme);
	if ( unlikely(status != STRICT_OK) )
		if ( status != STRICT_ERROR_INVALID_LEX_UNIT_DUMMY )
			return status;

	/* Check if the given lexeme has no variants */
	if ( status != STRICT_ERROR_INVALID_LEX_UNIT_DUMMY &&
	     !lexeme->hasOrthVariants() )
	{
		if ( orthVar == 0 )
			return STRICT_OK;
		if ( lexeme->isMainOrthVariant() )
			return STRICT_ERROR_INVALID_ORTH_VARIANT;
		return STRICT_ERROR_SUPPLEMENT_VARIANT_PROVIDED;
	}

	/* Search for the variant */
	const LexemeId **orthListPtr = 0;
        OrthVariantList::ResultStatus searchResult = lexOrthVariants.get( lexNum, &orthListPtr );
	if ( unlikely(searchResult == OrthVariantList::RESULT_STATUS_NOT_FOUND) )
		return ( status == STRICT_ERROR_INVALID_LEX_UNIT_DUMMY ?
			 STRICT_ERROR_ERROR_NO_SUCH_VARIANT : 
			 STRICT_ERROR_INTERNAL_ERROR );
        if ( unlikely(searchResult != OrthVariantList::RESULT_STATUS_SUCCESS) )
                return STRICT_ERROR_INTERNAL_ERROR;
        
	orthList = *orthListPtr;

        return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::getOrthVariant( LexemeId &lexNum, size_t orthVar ) const
{
	const LexemeId *orthList;
	StrictMorphErrors status = getOrthVariantsList( lexNum, orthVar, orthList );
	if ( unlikely(status != STRICT_OK) )
                return status;
        
	/* Calculate the list length */
	size_t listLength = 0;
	for ( ; ; listLength++ )
	{
		const CommonReference<LexemeId> *lexRef =
			reinterpret_cast<const CommonReference<LexemeId> *>(orthList + listLength);
		if ( lexRef->isListReference() )
			break;
	}

	/* Check the orth variant requested */
	if ( unlikely(orthVar > listLength) )
		return STRICT_ERROR_INVALID_ORTH_VARIANT;

	if ( unlikely( (orthList[orthVar] & dummyLexeme) == dummyLexeme) )
		return STRICT_ERROR_ERROR_NO_SUCH_VARIANT;

	const CommonReference<LexemeId> *lexRef =
		reinterpret_cast<const CommonReference<LexemeId> *>(orthList + orthVar);
	lexNum = lexRef->getReference();
	
	return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::queryOrthVariants( StrictLex::OrthEnumFunctor &functor, const LexUnit &lex ) const
{
        LexemeId lexNo = lex.lexId;
        
	const LexemeId *orthList;
	StrictMorphErrors status = getOrthVariantsList( lexNo, 0, orthList );
	if ( unlikely(status != STRICT_OK) )
                return status;
        
	/* Invoke the functor over the list */
	for ( size_t i = 1; ; i++ )
	{
		const CommonReference<LexemeId> *lexRef =
			reinterpret_cast<const CommonReference<LexemeId> *>(orthList + i);
                
                if ( likely((orthList[i] & dummyLexeme) != dummyLexeme) )
                {
                        StrictLexOrthVariant currentVar;
                        
                        currentVar.type         = StrictLexOrthVariant::SUPPLEMENT;
                        currentVar.lexNo        = static_cast<size_t>( lexRef->getReference() );
                        currentVar.orthVariant  = i;
                        currentVar.usageAttribs = getUsageAttribs( static_cast<LexemeId>(currentVar.lexNo),
                                                                   currentVar.orthVariant );

                        status = functor.apply( currentVar );
                        if ( unlikely(status != STRICT_OK) )
                                return status;
                }
                
		if ( lexRef->isListReference() )
			break;
	}
        
        return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::getGramDesc( StrictLex::GramEnumFunctor &functor, LexemeId lexId, size_t lexOrthVariant ) const
{
	/* Obtain the lexeme */
	const Lexeme *lexeme = NULL;
	StrictMorphErrors status = getLexemeOrth(lexId, lexOrthVariant, lexeme);
	if ( unlikely(status != STRICT_OK) )
		return status;
        
        LexGram gram;
        
        if ( dictInfo.lexGramDescListLength == 0 ||
             lexeme->getGramDescrId()->getReference() == CommonReference<LexGramDescrId>::dummy() )
        {
                return STRICT_OK;
        }
        else if ( lexeme->getGramDescrId()->isListReference() )
        {
                const CommonReference<uint32_t> *list =
                        lexGramDescsIdx + lexeme->getGramDescrId()->getReference() + 1;
                for ( ; ; list++ )
                {
                        const LexGramDescShadow *gramShadow = lexGramDescs + list->getReference();

                        gram.assignShadow( gramShadow );
                        
                        StrictMorphErrors status = functor.apply( gram );
                        if ( unlikely(status != STRICT_OK) )
                                return status;
                        
                        if ( list->isListReference() )
                                break;
                }
        }
        else
        {
                const LexGramDescShadow *gramShadow = lexGramDescs + lexeme->getGramDescrId()->getReference();

                gram.assignShadow( gramShadow );
                        
                StrictMorphErrors status = functor.apply( gram );
                if ( unlikely(status != STRICT_OK) )
                        return status;
        }
        
        return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::getLexText( StrictLex::TextFunctor &functor,
						 const LexUnit          &unit ) const
{
	/* Obtain the lexeme */
	const Lexeme *lexeme = NULL;
	StrictMorphErrors status = getLexemeOrth( unit.lexId, unit.lexOrthVariant, lexeme );
	if ( unlikely(status != STRICT_OK) )
		return status;
        
        class LocalFunctor : public LexUnit::PositionFunctor
        {
        public:
                LocalFunctor( StrictLex::TextFunctor &_functor,
                              const FlexionsArray    &_flexions,
                              StrictLex::DictCaps    _caps,
                              const UCharSet         *_charset ) :
                        functor( _functor ),
                        flexions( _flexions ),
                        caps( _caps ),
                        charset( _charset ) { }
                
                StrictMorphErrors apply( const MorphPosition &position )
                {
                        const char *stemText    = position.stemText;
                        const char *flexionText = flexions.getFlexion( position.flexId );
                        
                        StrictLex::DictCaps dictCaps = position.getLexCaps( position.stemIndex );

                        size_t stemLength = strlen( stemText );
                        size_t flexLength = strlen( flexionText );

                        char *word = functor.allocateBuffer( stemLength + flexLength + 1 );
                        if ( unlikely(word == 0) )
                                return STRICT_ERROR_ALLOCATION_ERROR;

                        /* If no capitalization given, restore the dictionary capitalization */
                        if (caps == StrictLex::CAPS_NO_CASE)
                                caps = dictCaps;
                        
                        /* Fill output buffer */
                        const char *p           = stemText;
                        size_t      i           = 0;
                        size_t      j           = 0;
                        bool        firstUpper  = false;
                        for ( ;; )
                        {
                                uint32_t ch;
                                U8_NEXT_UNSAFE( p, i, ch );
                                if ( ch == 0 )
                                {
                                        if ( p != stemText )
                                                break;
                                        p = flexionText;
                                        i = 0;
                                        continue;
                                }

                                /* Restore capitalization if any */
                                switch (caps)
                                {
                                case StrictLex::CAPS_UPPER_CASE:
                                        ch = charset->ToUpper( ch );
                                        break;

                                case StrictLex::CAPS_TITLE_CASE:
                                        if ( !firstUpper && charset->IsLower( ch ) )
                                        {
                                                ch = charset->ToUpper( ch );
                                                firstUpper = true;
                                        }
                                        break;

                                default: break;
                                };

                                U8_APPEND_UNSAFE( word, j, ch );
                        }
                        word[ j ] = '\0';
                        
                        return functor.apply( word );
                }
                
        private:
                StrictLex::TextFunctor &functor;
                const FlexionsArray    &flexions;
                StrictLex::DictCaps    caps;
                const UCharSet         *charset;
        } localFunctor( functor, flexions, unit.lexCaps, charset );
        
	return unit.callWithMorphPosition( localFunctor );
}

StrictLexOrthVariant::UsageAttribs StrictMorphShadow::getUsageAttribs( LexemeId lexNo, size_t orthVar ) const
{
	/* Obtain the lexeme */
	const Lexeme *lexeme = NULL;
	StrictMorphErrors status = getLexemeOrth( lexNo, orthVar, lexeme );
	if ( unlikely(status != STRICT_OK) )
                return static_cast<StrictLexOrthVariant::UsageAttribs>(StrictLexOrthVariant::USAGE_ATTR_LAST);
        
        return lexeme->getUsageAttributes();
}

const StrictDictInfo *StrictMorphShadow::getDictInfo() const
{
        return &dictInfo;
}

unsigned int StrictMorphShadow::getFlags() const
{
        unsigned int flags = 0;
        if ( likely(mapStrictMorphParamsFlagsBack( header.dict.creationFlags, flags ) == STRICT_OK) )
                return flags;
        return StrictMorphParams::FlagInvalid;
}

unsigned int StrictMorphShadow::getMorphNumber() const
{
        return header.dict.internalNumber;
}

unsigned long StrictMorphShadow::getDataSerial() const
{
        return static_cast<unsigned long>(dictInfo.datasetBuildTime);
}

unsigned long StrictMorphShadow::getDataMajor() const
{
        return static_cast<unsigned long>(header.workFormatMajor);
}

unsigned long StrictMorphShadow::getDataMinor() const
{
        return static_cast<unsigned long>(header.workFormatMinor);
}

