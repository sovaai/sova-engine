/**
 * @file   LPMorphStrictReal.cpp
 * @author swizard <me@swizard.info>
 * @date   Sat Jul 19 21:01:39 2008
 * 
 * @brief  Strict morphology for LingProc (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_string.h>
#include <_include/_inttype.h>
#include <_include/ntoh.h>

#include <LingProc4/LanguageInfo.h>
#include <LingProc4/WordNormalizer.h>

#include <LingProc4/LPStrict.h>

#include <LingProc4/StrictMorphGram/StrictDictInfo.h>

#include "LPMorphStrict.h"

/* Set strict morphology constants */
const uint32_t STRICTMORPH_MAX_WORD_LEN                = lpMaxWordLength * 4 + 4;
const uint32_t STRICTMORPH_MIN_STEM_LEN                = 1;
const uint32_t STRICTMORPH_VERSION                     = 1;
const uint32_t STRICTMORPH_VARIANT                     = 1;

const size_t   STRICTMORPH_FS_HEADER_SIZE              = (sizeof(uint32_t) * 12);
const uint32_t STRICTMORPH_FS_MAGIC                    = 0xC7C1715CU;
const uint32_t STRICTMORPH_FS_WORK_FORMAT_VERSION      = 2;
const uint32_t STRICTMORPH_FS_WORK_FORMAT_VARIANT      = 2;

const int      STRICTMORPH_FS_SEC_DATA                 = 1;

/* Implementation */

LPMorphStrict::LPMorphStrict() :
    LPMorphInterface(),
    morph(),
    lexPool(128, 2),
    chainsPool(32)
{
    initData();
    strict_morph_data_fs = 0;
}

LPMorphStrict::~LPMorphStrict()
{
    close();
}

LingProcErrors LPMorphStrict::initializeStrictMorph()
{
    /* Check if a file with data is provided */
    if ( unlikely(strict_morph_data_fs == 0) )
        return LP_ERROR_STRICT_DATA;

    /* Check if already opened */
    if ( morph.isCreated() )
        return LP_OK;

    /* Obtain the section offset for the language */
    const LanguageInfo *langFullInfo = LanguageInfo::getLanguageInfo( _info.lang );
    if ( unlikely(langFullInfo == 0) )
        return LP_ERROR_UNSUPPORTED_LANG;

    /* Map parameters */
    StrictMorphParams params;
    params.language       = _info.lang;
    params.internalNumber = _info.internalNumber;
    params.minStemLength  = _info.minStemLength;
    params.minCompoundStemLength = _info.minCompoundStemLength;

    /* Map flags */
    params.flags          = StrictMorphParams::FlagNone;

    if ( _info.creationFlags & MORPH_FLAG_CASE_INSENSITIVE )
        params.flags |= StrictMorphParams::FlagCaseInsensitive;
    if ( _info.creationFlags & MORPH_FLAG_LWR_ACCENT )
        params.flags |= StrictMorphParams::FlagLwrAccent;
    if ( _info.creationFlags & MORPH_FLAG_LWR_PUNCT )
        params.flags |= StrictMorphParams::FlagLwrPunct;
    if ( _info.creationFlags & MORPH_FLAG_REM_IGNORE )
        params.flags |= StrictMorphParams::FlagRemIgnore;
    if ( _info.creationFlags & MORPH_FLAG_NATIVE_ONLY )
        params.flags |= StrictMorphParams::FlagNativeOnly;
    if ( _info.creationFlags & MORPH_FLAG_PROCESS_COMPOUNDS )
        params.flags |= StrictMorphParams::FlagProcessCompounds;
    if ( _info.creationFlags & MORPH_FLAG_ARABIC_SEQ_COLLAPSE )
        params.flags |= StrictMorphParams::FlagArabicSeqCollapse;

    if ( _info.creationFlags & MORPH_FLAG_USE_DEFAULT )
        params.flags |= StrictMorphParams::FlagUseDefault;
        
    /* Open the morphology */
    int underlyingError = 0;
    StrictMorphErrors status = morph.load( _charset,
                                           params,
                                           strict_morph_data_fs,
                                           LanguageInfo::getLPSectionStrict( langFullInfo->fsectionOffset ),
                                           underlyingError );
    if ( unlikely(status != STRICT_OK) )
        return LP_ERROR_STRICT;
        
    return LP_OK;
}


LingProcErrors LPMorphStrict::create( const UCharSet *charset, const LPMorphCreateParams *params )
{
    /* Flush first the data if any */
    close();

    /* Check parameters */
    if ( unlikely(params == 0 || charset == 0) )
        return LP_ERROR_EINVAL;

    if ( unlikely(params->type != MORPH_TYPE_STRICT) )
        return LP_ERROR_UNSUPPORTED_MORPH;

    _charset             = charset;

    /* Fill the LPMorphDictInfo info */
    _info.type           = params->type;
    _info.version        = STRICTMORPH_VERSION;
    _info.variant        = STRICTMORPH_VARIANT;
    _info.lang           = params->lang;
    _info.globalId       = (_info.type << 24) | (_info.lang);
    _info.internalNumber = params->internalNumber;

    _info.minStemLength = ( params->minStemLength == 0 ? STRICTMORPH_MIN_STEM_LEN : params->minStemLength );
    _info.minCompoundStemLength = ( params->minCompoundStemLength == 0 ?
                                    _info.minStemLength :
                                    params->minCompoundStemLength );
        
    if (params->flags & MORPH_FLAG_USE_DEFAULT)
        return LP_ERROR_MORPH_FLAGS;
        
    _info.creationFlags = params->flags;
        
    /* Clear all except only meaning flags */
    unsigned int storeFlags     = 
        MORPH_FLAG_CASE_INSENSITIVE    |
        MORPH_FLAG_LWR_PUNCT           |
        MORPH_FLAG_REM_IGNORE          |
        MORPH_FLAG_NATIVE_ONLY         |
        MORPH_FLAG_PROCESS_COMPOUNDS   |
        MORPH_FLAG_ARABIC_SEQ_COLLAPSE;
    _info.creationFlags &= storeFlags;

    /* Initialize the charset */
    LingProcErrors ret = initCharSet(_charset);
    if ( unlikely(ret != LP_OK) )
    {
        close();
        return ret;
    }

    ret = initializeStrictMorph();
    if ( unlikely(ret != LP_OK) )
        return ret;
        
    return LP_OK;
}

LingProcErrors LPMorphStrict::FSAttachStrictMorph( fstorage *fs )
{
    if ( unlikely(fs == 0) )
        return LP_ERROR_EINVAL;
    strict_morph_data_fs = fs;
    return LP_OK;
}

LingProcErrors LPMorphStrict::close()
{
    initData();

    return LP_OK;
}

int LPMorphStrict::lemmatize( LemmatizeResultFunctor &functor,
			      const char             *word,
			      size_t                  wordLength,
			      unsigned int            /* flags */)
{
    if ( strict_morph_data_fs == 0 )
        return 0;
        
    bool forceCompounds = false;
    if ( _info.creationFlags & MORPH_FLAG_PROCESS_COMPOUNDS )
    {
        /* Clear the pools */
        lexPool.clear();
        chainsPool.clear();
        forceCompounds = true;
    }

    bool forceDictResolve = ( _info.creationFlags & MORPH_FLAG_NO_DICT_INFO ? false : true );
        
    FunctorsMapper mapper( morph, functor, lexPool, formsSetsPool, chainsPool, forceCompounds, forceDictResolve );
        
    StrictMorphErrors status = morph.lemmatize( mapper, word, wordLength );
        
    /* Process lemmatization results */
    if ( unlikely(status != STRICT_OK) )
    {
        if ( status == STRICT_ERROR_NOT_NATIVE_SYMBOL )
            return 0;
                
        if ( unlikely(mapper.getStatus() != LP_OK) )
            return mapper.getStatus();
    }

    if ( forceCompounds )
    {
        /* Finish the process for compounds */
        mapper.finish();

        formsSetsPool.clear();
        
        if ( unlikely(mapper.getStatus() != LP_OK) )
            return mapper.getStatus();
    }
        
    return mapper.getHomoCount();
}

int LPMorphStrict::stemmatize( StemmatizeResultFunctor &functor,
                               const char*              word,
                               size_t                   wordLength,
                               unsigned int             flags )
{
    class ResultsMapper : public LemmatizeResultFunctor
    {
    public:
        ResultsMapper( StemmatizeResultFunctor &_functor, StrictMorph &_morph, const LPMorphDictInfo &_info ) :
            LemmatizeResultFunctor(),
            functor( _functor ),
            morph( _morph ),
            info( _info )
        {
        }
                
        ~ResultsMapper() { }

    public: 
        LingProcErrors reset() { return LP_OK; }
                
        LingProcErrors nextLemma(LEXID lexid, size_t compoundNumber, const LPStrict::Info *info)
        {
            char *buffer = functor.getStemBuffer( 8 /* lexNo */ + 1 /* $ */ + 2 /* morphNo */ + 1 /* '\0' */ );
            if ( unlikely(buffer == 0) )
                return LP_ERROR_ENOMEM;
                        
            char *p = dumpHex( realMorphNumber< LEXID >( this->info ), 2, buffer );
            *p++    = '$';
            p       = dumpHex( LexId(lexid), 8, p );
            *p      = '\0';
                        
            return functor.nextStem( lexid, compoundNumber, buffer, static_cast<size_t>(p - buffer), info );
        }

    private:
        StemmatizeResultFunctor &functor;
        StrictMorph             &morph;
        const LPMorphDictInfo   &info;
    } mapper( functor, morph, _info );
        
    return lemmatize( mapper, word, wordLength, flags );
}

int LPMorphStrict::getLexText( LEXID lex, char *buffer, size_t bufferSize, const BinaryData *additionalData )
{
    if ( unlikely(strict_morph_data_fs == 0) )
        return 0;

    class TextGetter : public StrictLex::TextFunctor, public StrictMorph::UnpackFunctor
    {
    public:
        TextGetter( const StrictMorph &_morph, char *_buffer, size_t _bufferSize ) :
            morph( _morph ),
            buffer( _buffer ),
            bufferSize( _bufferSize ),
            requiredSize( 0 )
        {
        }

        StrictMorphErrors apply(const StrictLex &lex)
        {
            return lex.restoreText( *this );
        }

        char *allocateBuffer(size_t size)
        {
            requiredSize = size;
            if ( unlikely(size > bufferSize) )
                return 0;
            return buffer;
        }
                
        StrictMorphErrors apply(const char * /* text */) { return STRICT_ERROR_ENUMERATION_STOPPED; }
        size_t            getRequiredSize()       { return ( requiredSize == 0 ? 0 : requiredSize - 1 ); }
    private:
        const StrictMorph &morph;
        char              *buffer;
        size_t             bufferSize;
        size_t             requiredSize;
    } getter( morph, buffer, bufferSize );

    StrictMorphErrors status = morph.callWithUnpack< LEXID >( lex, getter, additionalData );
    if ( unlikely(status != STRICT_OK && status != STRICT_ERROR_ENUMERATION_STOPPED) )
        return LP_ERROR_EFAULT;
        
    return (int)getter.getRequiredSize();
}

int LPMorphStrict::getLexText(LEXID partialLex, const char * /* stem */, char *buffer, size_t bufferSize, const BinaryData *additionalData)
{
    return getLexText( partialLex, buffer, bufferSize, additionalData );
}

LingProcErrors LPMorphStrict::enumerate( MorphEnumerator &enumerator)
{
    class LocalEnumWrapper : public StrictLex::LemmatizeFunctor
    {
    public:
        LocalEnumWrapper( const StrictMorph &_morph, MorphEnumerator &_enumerator ) :
            StrictLex::LemmatizeFunctor(),
            morph( _morph ),
            enumerator( _enumerator ),
            lastError( LP_OK ) { }

        int apply(const StrictLex &lex)
        {
            LEXID             lexid  = static_cast<LEXID>(0);
            StrictMorphErrors result = morph.pack<LEXID>(lex, lexid);
            if ( unlikely(result != STRICT_OK) )
            {
                lastError = LP_ERROR_EFAULT;
                return 1;
            }
                        
            lastError = enumerator.nextLexid( lexid );
            if ( unlikely(lastError != LP_OK) )
                return 1;
            return 0;
        }
                
        LingProcErrors getLastError() const { return lastError; }
                
    private:
        const StrictMorph &morph;
        MorphEnumerator   &enumerator;
        LingProcErrors     lastError;
    } enumWrapper( morph, enumerator );

    StrictMorphErrors status = morph.enumerate( enumWrapper );
    if ( unlikely(status != STRICT_OK) )
    {
        if ( status == STRICT_ERROR_ENUMERATION_STOPPED )
            return LP_ENUM_STOP;
        if ( enumWrapper.getLastError() != LP_OK )
            return enumWrapper.getLastError();
        return LP_ERROR_STRICT;
    }
    return LP_OK;
}

LingProcErrors LPMorphStrict::FSCreateFromTemplate( const UCharSet      *charset,
						    fstorage            *templateFs,
						    fstorage_section_id  sectionId )
{
    return FSOpenRW(charset, templateFs, sectionId);
}

LingProcErrors LPMorphStrict::FSOpenRW( const UCharSet      *charset,
					fstorage            *fs,
					fstorage_section_id  sectionId )
{
    LingProcErrors result;
    result = openCommon(charset, fs, sectionId);
    if ( unlikely(result != LP_OK) )
        return result;

    return LP_OK;
}

LingProcErrors LPMorphStrict::FSOpenRO( const UCharSet      *charset,
					fstorage            *fs,
					fstorage_section_id  sectionId )
{
    LingProcErrors result;
    result = openCommon(charset, fs, sectionId);
    if ( unlikely(result != LP_OK) )
        return result;

    return LP_OK;
}

LingProcErrors LPMorphStrict::FSOpenLimited( const UCharSet      *charset,
					     fstorage            *fs,
					     fstorage_section_id  sectionId )
{
    LingProcErrors result;
    result = openCommon(charset, fs, sectionId);
    if ( unlikely(result != LP_OK) )
        return result;

    return LP_OK;
}

LingProcErrors LPMorphStrict::FSSave( fstorage            *fs,
				      fstorage_section_id  sectionId,
				      bool                 transport )
{
    if ( unlikely(fs == 0) )
        return LP_ERROR_EINVAL;

    if ( unlikely(!isValid() || fstorage_is_shared(fs)) ) // do not save if openLimited()
        return LP_ERROR_INVALID_MODE;

    if ( transport )
        return LP_ERROR_ENOTIMPL;
        
    /* Initialize the header */
    memset(&headerData, 0, sizeof(HeaderData));
    headerData.magic                    = STRICTMORPH_FS_MAGIC;
    headerData.work_format_version      = STRICTMORPH_FS_WORK_FORMAT_VERSION;
    headerData.work_format_variant      = STRICTMORPH_FS_WORK_FORMAT_VARIANT;

    /* Store the header */
    LingProcErrors result;
    result = serializeHeader(fs, sectionId);
    if ( unlikely(result != LP_OK) )
        return result;

    return LP_OK;
}

void LPMorphStrict::initData()
{
    _charset             = 0;
    memset(&headerData, 0, sizeof(HeaderData));
    memset(&_info,      0, sizeof(LPMorphDictInfo));
}

LingProcErrors LPMorphStrict::deserializeHeader( fstorage *fs, fstorage_section_id sectionId )
{
    /* Find the section */
    fstorage_section *section = fstorage_find_section(fs, sectionId);
    if ( unlikely(section == 0) )
        return LP_ERROR_FS_FAILED;

    /* Obtain the pointer to the section */
    uint32_t *sectionPointer = (uint32_t *)fstorage_section_get_all_data(section);
    if ( unlikely(sectionPointer == 0) )
        return LP_ERROR_FS_FAILED;

    size_t obligatoryHeaderSize = static_cast<size_t>( STRICTMORPH_FS_HEADER_SIZE ) - sizeof( uint32_t );
    
    /* Verify the section size */
    if ( unlikely(fstorage_section_get_size(section) < obligatoryHeaderSize) )
        return LP_ERROR_FS_FAILED;

    /* Load the data */
    uint32_t *pt = sectionPointer;

    memset(&headerData, 0, sizeof(HeaderData));
    headerData.magic                    = be32toh(*pt++);
    
    /* Check if the versions match */
    if ( unlikely(headerData.magic != STRICTMORPH_FS_MAGIC) )
        return LP_ERROR_INVALID_FORMAT;

    headerData.work_format_version      = be32toh(*pt++);

    if ( unlikely(headerData.work_format_version != STRICTMORPH_FS_WORK_FORMAT_VERSION) )
        return LP_ERROR_INVALID_FORMAT;
    
    headerData.work_format_variant      = be32toh(*pt++);

    size_t expectedHeaderSize = ( headerData.work_format_variant < STRICTMORPH_FS_WORK_FORMAT_VARIANT ?
                                  obligatoryHeaderSize : STRICTMORPH_FS_HEADER_SIZE );

    /* Verify the section size once more */
    if ( unlikely(fstorage_section_get_size(section) < expectedHeaderSize) )
        return LP_ERROR_FS_FAILED;
    
    memset(&_info, 0, sizeof(LPMorphDictInfo));
    _info.type           = static_cast<MorphTypes>(be32toh(*pt++));
    _info.version        = be32toh(*pt++);
    _info.variant        = be32toh(*pt++);
    _info.lang           = static_cast<LangCodes>(be32toh(*pt++));
    _info.globalId       = be32toh(*pt++);
    _info.internalNumber = be32toh(*pt++);
    _info.minStemLength  = be32toh(*pt++);
    _info.creationFlags  = be32toh(*pt++);
    if ( headerData.work_format_variant >= STRICTMORPH_FS_WORK_FORMAT_VARIANT )
        _info.minCompoundStemLength = be32toh(*pt++);
    else
        _info.minCompoundStemLength = _info.minStemLength;

    /* Verify the data amount read */
    if ( unlikely(static_cast<size_t>(reinterpret_cast<char *>(pt) -
                                      reinterpret_cast<char *>(sectionPointer)) != expectedHeaderSize) )
        return LP_ERROR_EFAULT; // fatal error in program

    if ( unlikely(_info.version != STRICTMORPH_VERSION) )
        return LP_ERROR_INVALID_FORMAT;
  
    return LP_OK;
}

LingProcErrors LPMorphStrict::serializeHeader( fstorage *fs, fstorage_section_id sectionId )
{
    /* Obtain the section */
    fstorage_section *section = fstorage_get_section(fs, sectionId);
    if ( unlikely(section == 0) )
        return LP_ERROR_FS_FAILED;

    /* Allocate the memory */
    if ( fstorage_section_get_size(section) != STRICTMORPH_FS_HEADER_SIZE )
    {
        if ( unlikely(fstorage_section_realloc(section, STRICTMORPH_FS_HEADER_SIZE) != FSTORAGE_OK) )
            return LP_ERROR_FS_FAILED;
    }

    /* Obtain the pointer */
    uint32_t *sectionPointer = (uint32_t *)fstorage_section_get_all_data(section);
    if ( unlikely(sectionPointer == 0) )
        return LP_ERROR_FS_FAILED;

    /* Store the data */
    memset(sectionPointer, 0, STRICTMORPH_FS_HEADER_SIZE);

    uint32_t *pt = sectionPointer;
    *pt++ = htobe32(static_cast<uint32_t>(headerData.magic));
    *pt++ = htobe32(static_cast<uint32_t>(headerData.work_format_version));
    *pt++ = htobe32(static_cast<uint32_t>(headerData.work_format_variant));

    *pt++ = htobe32(static_cast<uint32_t>(_info.type));
    *pt++ = htobe32(static_cast<uint32_t>(_info.version));
    *pt++ = htobe32(static_cast<uint32_t>(_info.variant));
    *pt++ = htobe32(static_cast<uint32_t>(_info.lang));
    *pt++ = htobe32(static_cast<uint32_t>(_info.globalId));
    *pt++ = htobe32(static_cast<uint32_t>(_info.internalNumber));
    *pt++ = htobe32(static_cast<uint32_t>(_info.minStemLength));
    *pt++ = htobe32(static_cast<uint32_t>(_info.creationFlags));
    *pt++ = htobe32(static_cast<uint32_t>(_info.minCompoundStemLength));

    /* Check the data amount written */
    if ( unlikely(static_cast<size_t>(reinterpret_cast<char *>(pt) -
                                      reinterpret_cast<char *>(sectionPointer)) != STRICTMORPH_FS_HEADER_SIZE) )
        return LP_ERROR_EFAULT; // fatal error in program

    return LP_OK;
}

LingProcErrors LPMorphStrict::initCharSet( const UCharSet *charset )
{
    if ( unlikely(charset == 0 || !charset->IsValid()) )
        return LP_ERROR_EINVAL;
    if ( unlikely(getLang() != charset->GetLang()) )
        return LP_ERROR_UNSUPPORTED_LANG;
    _charset = charset;

    return LP_OK;
}

LingProcErrors LPMorphStrict::openCommon( const UCharSet *charset, fstorage *fs, fstorage_section_id sectionId )
{
    /* Common part of FSOpenRO() and FSOpenRW() */

    close();

    if ( unlikely(fs == 0 || charset == 0) )
        return LP_ERROR_EINVAL;

    LingProcErrors result;

    /* Load the header and dictionary information */
    result = deserializeHeader(fs, sectionId);
    if ( unlikely(result != LP_OK) )
    {
        close();
        return result;
    }

    /* Initialize the charset */
    result = initCharSet(charset);
    if ( unlikely(result != LP_OK) )
    {
        close();
        return result;
    }

    LingProcErrors ret = initializeStrictMorph();
    if ( unlikely(ret != LP_OK) )
        return ret;
        
    return LP_OK;
}

LingProcErrors LPMorphStrict::resolveLPStrict( LEXID lex, LPStrict::Resolver &resolver ) const
{
    if ( unlikely(strict_morph_data_fs == 0) )
        return LP_ERROR_STRICT;

    class StrictResolver : public StrictMorph::UnpackFunctor,
                           public LPStrict::FormsSetsRetriever
    {
    public:
        StrictResolver( const StrictMorph &_morph, LPStrict::Resolver &_resolver ) :
            morph( _morph ),
            resolver( _resolver ),
            currentLex( 0 )
        {
        }

        StrictMorphErrors apply(const StrictLex &lex)
        {
            currentLex = &lex;
            LPStrict::Info info( lex, *this );
                        
            return ( resolver.apply( info ) == 0 ?
                     STRICT_OK : STRICT_ERROR_ENUMERATION_STOPPED );
        }

        StrictMorphErrors retrieveFormsSets( StrictLex::FormsSetFunctor &functor ) const
        {
            if ( unlikely(currentLex == 0) )
                return STRICT_ERROR_INTERNAL_ERROR;

            return currentLex->queryFormsSets( functor );
        }        
                                
    private:
        const StrictMorph  &morph;
        LPStrict::Resolver &resolver;
        const StrictLex    *currentLex;
    } getter( morph, resolver );

    StrictMorphErrors status = morph.callWithUnpack<LEXID>( lex, getter );
    if ( unlikely(status != STRICT_OK && status != STRICT_ERROR_ENUMERATION_STOPPED) )
        return LP_ERROR_EFAULT;
        
    return LP_OK;
}

const StrictDictInfo *LPMorphStrict::getStrictDictInfo() const
{
    if ( unlikely(strict_morph_data_fs == 0) )
        return 0;

    return morph.getDictInfo();
}


/* LPMorphStrict::FunctorsMapper */

LPMorphStrict::FunctorsMapper::FunctorsMapper( const StrictMorph      &_morph,
                                               LemmatizeResultFunctor &_functor,
                                               CompoundLexPool        &_lexPool,
                                               FormsSetsPool          &_formsSetsPool,
                                               DecompositionChainPool &_chainsPool,
                                               bool                    _forceCompounds,
                                               bool                    _forceDictResolve ) :
    morph( _morph ),
    functor( _functor ),
    lexPool( _lexPool ),
    formsSetsPool( _formsSetsPool ),
    chainsPool( _chainsPool ),
    forceCompounds( _forceCompounds ),
    forceDictResolve( _forceDictResolve ),
    status( LP_OK ),
    homoCount( 0 )
{
}

LPMorphStrict::FunctorsMapper::~FunctorsMapper()
{
}

int LPMorphStrict::FunctorsMapper::recurseCompound( const StrictLex *lex, size_t depth, size_t offset )
{
    /* Recursion termination conditions */
    if ( lex == 0 )
    {
        lexPool.resize( offset + depth );
        if ( unlikely(lexPool.no_memory()) )
        {
            status = LP_ERROR_ENOMEM;
            return -1;
        }
        return (int)depth;
    }

    /* Sink into recursion, then install a chain */
    int length = recurseCompound( lex->getLeftCompound(), depth + 1, offset );
    if ( unlikely(length < 0) )
        return length;
        
    CompoundLex &cLex = lexPool[ offset + length - depth - 1];
        
    LEXID             lexid  = static_cast<LEXID>(0);
    StrictMorphErrors result = morph.pack<LEXID>(*lex, lexid, &cLex.additionalData);
    if ( unlikely(result != STRICT_OK) )
    {
        status = LP_ERROR_EFAULT;
        return -1;
    }
        
    cLex.lex             = lexid;
    cLex.bound           = lex->getCompoundBound();
    cLex.formsSetsOffset = formsSetsPool.size();

    class LocalFunctor : public StrictLex::FormsSetFunctor
    {
    public:
        LocalFunctor( FormsSetsPool &_target ) : target( _target ) { }

        StrictMorphErrors apply( const StrictWordFormsSet &set )
        {
            target.push_back( set );
            if ( unlikely(target.no_memory()) )
                return STRICT_ERROR_ALLOCATION_ERROR;
                
            return STRICT_OK;
        }
            
    private:
        FormsSetsPool &target;
    } functor( formsSetsPool );
        
    StrictMorphErrors sstatus = lex->queryFormsSets( functor );
    if ( unlikely(sstatus != STRICT_OK) )
    {
        status = LP_ERROR_EFAULT;
        return -1;
    }

    cLex.formsSetsCount = formsSetsPool.size() - cLex.formsSetsOffset;
    
    return length;
}

int LPMorphStrict::FunctorsMapper::apply( const StrictLex &lex )
{
    if ( forceCompounds )
    {
        /* Convert the lex units chain to the flat chain of LEXIDs with decomposition bounds */
        size_t offset   = lexPool.size();
        int chainLength = recurseCompound( &lex, 0, offset );
        if ( unlikely(chainLength < 0) )
            return 1;
                     
        /* Check the index for this decomposition variant */
        DecompositionChain   decChain( &lexPool, offset, chainLength );
        DecompositionChain  *targetChain = 0;
                
        for ( size_t chainIdx = 0; chainIdx < chainsPool.size(); chainIdx++ )
        {
            if ( static_cast<size_t>(chainLength) != chainsPool[chainIdx].chainLength )
                continue;
                        
            bool found = true;
            for ( register int i = 0; i < chainLength; i++ )
                if ( (*decChain.pool)[  decChain.chainOffset + i  ].bound !=
                     (*chainsPool[chainIdx].pool)[ chainsPool[chainIdx].chainOffset + i ].bound )
                {
                    found = false;
                    break;
                }
            if ( found )
            {
                targetChain = &chainsPool[chainIdx];
                break;
            }
        }
                
        if ( targetChain != 0 )
        {
            /* A decomposition variant is found, mix there a chain created */

            for (size_t position = 0; position < decChain.chainLength; position++ )
            {
                LEXID lex       = lexPool[ decChain.chainOffset + position ].lex;
                bool  duplicate = false;
                int   lastIdx   = 0;
                for ( int curIdx = (int)(targetChain->chainOffset + position);
                      curIdx >= 0;
                      lastIdx = curIdx, curIdx = lexPool[curIdx].nextHomoIdx )
                    if ( lexPool[curIdx].lex == lex )
                    {
                        duplicate = true;
                        break;
                    }
                if ( !duplicate )
                    lexPool[lastIdx].nextHomoIdx = (int)(decChain.chainOffset + position);
            }
        }
        else 
        {
            /* The new decomposition variant is coming: add it */
            targetChain = chainsPool.grow();
            if ( unlikely(chainsPool.no_memory()) )
            {
                status = LP_ERROR_ENOMEM;
                return 1;
            }
            *targetChain = decChain;
        }
    }
    else
    {
        class FormsRetriever : public LPStrict::FormsSetsRetriever
        {
        public:
            FormsRetriever( const StrictLex &_lex ) : lex( _lex ) { }

            StrictMorphErrors retrieveFormsSets( StrictLex::FormsSetFunctor &functor ) const
            {
                return lex.queryFormsSets( functor );
            }        
            
        private:
            const StrictLex &lex;
        } retriever( lex );
        
        LPStrict::Info info( lex, retriever );

        LEXID             lexid  = static_cast<LEXID>( 0 );
        StrictMorphErrors result = morph.pack<LEXID>( lex, lexid, &info.additionalData );
        if ( unlikely(result != STRICT_OK) )
        {
            status = LP_ERROR_EFAULT;
            return 1;
        }

        homoCount++;
        status = functor.nextLemma( lexid, 0, ( forceDictResolve ? &info : 0 ) );
        if ( unlikely(status != LP_OK) )
            return 1;
    }
                        
    return 0;
}

int LPMorphStrict::FunctorsMapper::finish()
{
    if ( !forceCompounds )
        return 0;
        
    /* Iterate over the results obtained */
    for ( size_t chainIdx = 0; chainIdx < chainsPool.size(); chainIdx++ )
    {
        DecompositionChain &chain = chainsPool[chainIdx];

        for ( size_t position = 0; position < chain.chainLength; position++ )
        {
            for ( int curIdx = (int)(chain.chainOffset + position);
                  curIdx >= 0;
                  curIdx = lexPool[curIdx].nextHomoIdx )
            {
                /* At last invoke the functor */
                homoCount++;

                class LocalBuilder : public StrictMorph::UnpackFunctor,
                                     public LPStrict::FormsSetsRetriever
                {
                public:
                    LocalBuilder( LemmatizeResultFunctor &_functor,
                                  const CompoundLex      &_clex,
                                  size_t                  _position,
                                  DecompositionChain     &_chain,
                                  bool                    _forceDictResolve,
                                  FormsSetsPool          &_formsSetsPool ) :
                        functor( _functor ),
                        status( LP_OK ),
                        clex( _clex ),
                        position( _position ),
                        chain( _chain ),
                        forceDictResolve( _forceDictResolve ),
                        formsSetsPool( _formsSetsPool )
                    {
                    }
                    
                    StrictMorphErrors apply( const StrictLex &lex )
                    {
                        LPStrict::Info info( lex, *this );
                        info.additionalData     = clex.additionalData;
                        
                        status = functor.nextLemma( clex.lex,
                                                    position + (chain.chainLength > 1 ? 1 : 0),
                                                    ( forceDictResolve ? &info : 0 ) );
                        if ( unlikely(status != LP_OK) )
                            return STRICT_ERROR_ENUMERATION_STOPPED;

                        return STRICT_ERROR_ENUMERATION_STOPPED;
                    }

                    StrictMorphErrors retrieveFormsSets( StrictLex::FormsSetFunctor &functor ) const
                    {
                        for ( size_t i = 0; i < clex.formsSetsCount; i++ )
                        {
                            StrictMorphErrors status = functor.apply( formsSetsPool[ clex.formsSetsOffset + i ] );
                            if ( unlikely(status != STRICT_OK) )
                                return status;
                        }

                        return STRICT_OK;
                    }

                    LingProcErrors lastStatus() const { return status; }

                private:
                    LemmatizeResultFunctor &functor;
                    LingProcErrors          status;
                    const CompoundLex      &clex;
                    size_t                  position;
                    DecompositionChain     &chain;
                    bool                    forceDictResolve;
                    FormsSetsPool          &formsSetsPool;
                } builder( functor, lexPool[ curIdx ], position, chain, forceDictResolve, formsSetsPool );

                StrictMorphErrors sstatus =
                    morph.callWithUnpack< LEXID >( lexPool[ curIdx ].lex, builder, &lexPool[ curIdx ].additionalData );
                if ( unlikely(builder.lastStatus() != LP_OK) )
                    return builder.lastStatus();
                if ( unlikely(sstatus != STRICT_OK && sstatus != STRICT_ERROR_ENUMERATION_STOPPED) )
                    return LP_ERROR_STRICT;
            }
        }
    }
        
    return 0;
}

