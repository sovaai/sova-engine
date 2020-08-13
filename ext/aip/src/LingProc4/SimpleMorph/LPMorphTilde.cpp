/**
* @file   LPMorphTilde.cpp
* @author swizard <me@swizard.info>
* @date   Mon Sep  1 19:15:09 2008
* 
* @brief  A fuzzymorph-like morphology with special behaviour.
* 
* 
*/

#include <_include/cc_compat.h>
#include <_include/_string.h>
#include <_include/_inttype.h>
#include <_include/ntoh.h>

#include "LPMorphTilde.h"
#include "LPMorphTildeRev.h"

/* Implementation */

template< typename TildeCustom >
LPMorphTildeImpl<TildeCustom>::LPMorphTildeImpl() :
    LPMorphInterface(),
    normalizer( 0 ),
    tree( 0 )
{
    initData();
}

template< typename TildeCustom >
LPMorphTildeImpl<TildeCustom>::~LPMorphTildeImpl()
{
    close();
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::create(const UCharSet *charset, const LPMorphCreateParams *params)
{
    /* Flush first the data if any */
    close();

    /* Check parameters */
    if ( unlikely(params == 0 || charset == 0) )
        return LP_ERROR_EINVAL;

    if ( unlikely(params->type != TildeCustom::MORPH_TYPE) )
        return LP_ERROR_UNSUPPORTED_MORPH;

    /* Fill the LPMorphDictInfo info */
    _info.type           = params->type;
    _info.version        = TildeCustom::TILDEMORPH_VERSION;
    _info.variant        = TildeCustom::TILDEMORPH_VARIANT;
    _info.lang           = params->lang;
    _info.globalId       = (_info.type << 24) | (_info.lang);
    _info.internalNumber = params->internalNumber;

    _info.minStemLength = TildeCustom::TILDEMORPH_MIN_STEM_LEN;
    
    if (params->flags & (MORPH_FLAG_USE_DEFAULT | MORPH_FLAG_INVALID))
        return LP_ERROR_MORPH_FLAGS;
        
    _info.creationFlags = params->flags;
        
    /* Clear all except only meaning flags */
    unsigned int storeFlags     = 
        MORPH_FLAG_CASE_INSENSITIVE    |
        MORPH_FLAG_LWR_ACCENT          |
        MORPH_FLAG_LWR_PUNCT           |
        MORPH_FLAG_REM_IGNORE          |
        MORPH_FLAG_EXPAND_LIGATURES    |
        MORPH_FLAG_NATIVE_ONLY         |
        MORPH_FLAG_ARABIC_SEQ_COLLAPSE;
    _info.creationFlags &= storeFlags;
    
    /* Initialize the charset */
    LingProcErrors ret = initCharSet(charset);
    if ( unlikely(ret != LP_OK) )
    {
        close();
        return ret;
    }

    /* Initialize stem tree */
    tree = new PatriciaTreeFS();
    if ( unlikely(tree == NULL) )
    {
        close();
        return LP_ERROR_ENOMEM;
    }

    PatriciaTreeCore::ResultStatus status = tree->create( treeFlags );
    if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
        return LP_ERROR_EFAULT;

    return LP_OK;
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::flush()
{
    if ( likely(tree != 0) )
    {
        PatriciaTreeCore::ResultStatus status = tree->cleanup();
        if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS &&
                            status != PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED) )
            return LP_ERROR_EFAULT;
    }
    return LP_OK;
}

/* !!! When saving close() must be called AFTER fstorage_close() !!! */
template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::close()
{
    if ( likely(tree != 0) )
        delete tree;
    if ( likely(normalizer != 0) )
        delete normalizer;
    
    initData();

    return LP_OK;
}

template< typename TildeCustom >
int LPMorphTildeImpl<TildeCustom>::lemmatize( LemmatizeResultFunctor &functor,
                             const char             *word,
                             size_t                  wordLength,
                             unsigned int            flags )
{
    LEXID         lexid = 0;
    unsigned char lemmatizeBuffer[TildeCustom::TILDEMORPH_MAX_WORD_LEN + 1];

    /* Check if some parameters are invalid */
    if ( !isValid() )
        return LP_ERROR_INVALID_MODE;

    word = TildeCustom::preprocessWord( word, wordLength );
    
    WordNormalizerTarget  f( reinterpret_cast<char *>(lemmatizeBuffer), TildeCustom::TILDEMORPH_MAX_WORD_LEN );
    WordNormalizerResult  status = normalizer->normalizeWord( f, word, wordLength );
    switch ( status )
    {
    case NORMALIZE_SUCCESS:
        break;
        
    case NORMALIZE_ERROR_PARAMETER:                
        return LP_ERROR_EINVAL;
        
    case NORMALIZE_ERROR_FLAGS:
        return LP_ERROR_MORPH_FLAGS;
        
    case NORMALIZE_ERROR_BUFFER:
    case NORMALIZE_ERROR_NOT_NATIVE:
        return 0;
        
    case NORMALIZE_ERROR_FUNCTOR:
    case NORMALIZE_ERROR_INTERNAL:
        return LP_ERROR_EFAULT;
    };
                
    size_t  lemmatizeLength = f.resultSize;
    LEXID   caps            = 0;
    switch (f.caps)
    {
    case NORMALIZE_CAPS_NO_CASE:    caps = CAP_NOCASE;    break;
    case NORMALIZE_CAPS_LOWER_CASE: caps = CAP_LOWERCASE; break;
    case NORMALIZE_CAPS_TITLE_CASE: caps = CAP_TITLECASE; break;
    case NORMALIZE_CAPS_UPPER_CASE: caps = CAP_UPPERCASE; break;
    };
    
    /* If empty word */
    if ( unlikely(lemmatizeLength == 0) )
        return 0;
    
    if ( lemmatizeLength < TildeCustom::TILDEMORPH_MAX_WORD_LEN )
        lemmatizeBuffer[lemmatizeLength] = '\0';
    
    lexid |= caps;

    /* Install the morphology number */
    _LexSetNoMrph(&lexid, _info.internalNumber);

    /* Search the stem */
    LEXID lex = 0;

    if ( flags & MORPH_FLAG_PROCESS_TILDE )
    {
        /* cut trailing tilde */
        if ( unlikely(lemmatizeBuffer[lemmatizeLength - 1] != '~') )
            return 0;
        lemmatizeBuffer[lemmatizeLength - 1] = '\0';
        lemmatizeLength--;

        /* If empty word */
        if ( unlikely(lemmatizeLength == 0) )
            return 0;

        /* Try to add the new stem */
        uint32_t id = 0;

        PatriciaTreeCore::ResultStatus status = tree->indexedAppend(lemmatizeBuffer, &id);

        if ( status == PatriciaTreeCore::RESULT_STATUS_DUPLICATE_KEY ||
                            status == PatriciaTreeCore::RESULT_STATUS_SUCCESS )
            lex = static_cast<LEXID>( id );
        else if ( status == PatriciaTreeCore::RESULT_STATUS_ERROR_ALLOC ||
                            status == PatriciaTreeCore::RESULT_STATUS_ERROR_ALLOC_MAX_SIZE )
            return LP_ERROR_ENOMEM;
        else
            return LP_ERROR_EFAULT;

        /* Merge lexids */
        lexid |= lex;

        /* Invoke the functor */
        LingProcErrors result = functor.nextLemma(lexid, 0, 0);
        if (result != LP_OK)
            return result;
        else
            return 1;
    }
    else
    {
        /* Declare a functor mapper */
        class FunctorMapper : public CrossPathFunctor
        {
        public:
            FunctorMapper( LemmatizeResultFunctor &_functor,
                           LEXID                   _partLex ) :
                CrossPathFunctor(),
                functor( _functor ),
                partLex( _partLex ),
                status( LP_OK ),
                invoke_counter( 0 )
            {
            }

            ~FunctorMapper() { }
        private:
            FunctorMapper& operator = (const FunctorMapper&) { return *this; }

        public:
            int nextPart(const uint8_t* /*part*/ , const uint8_t *data, size_t /* dataSize */, bool /* isExactMatch */)
            {
                LEXID lex = static_cast<LEXID>( *(reinterpret_cast<const uint32_t *>(data)) );
                lex |= partLex;

                /* Invoke the functor */
                invoke_counter++;
                status = functor.nextLemma(lex, 0, 0);
                if (status != LP_OK)
                    return 1;
                return 0;
            }

            LingProcErrors getStatus()  const { return status;     }
            size_t           Invoked()  const { return invoke_counter; }

        private:
            LemmatizeResultFunctor &functor;
            LEXID                   partLex;
            LingProcErrors          status;
            size_t                  invoke_counter;
        } mapper( functor, lexid );

        /* Try to locate all stems */
        PatriciaTreeCore::ResultStatus status = tree->lookupAll(lemmatizeBuffer, mapper);
        if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
            return LP_ERROR_EFAULT;
        if ( unlikely(mapper.getStatus() != LP_OK) )
            return mapper.getStatus();
        return (int)mapper.Invoked();
    }
}

template< typename TildeCustom >
int LPMorphTildeImpl<TildeCustom>::getLexText(LEXID lex, char *buffer, size_t bufferSize, const BinaryData * /* additionalData */)
{
    if ( unlikely(!isValid()) )
        return LP_ERROR_INVALID_MODE;

    if ( unlikely(LexNoMrph(lex) != _info.internalNumber) )
        return 0;

    /* Obtain the stem */
    uint32_t    link = 0;

    PatriciaTreeCore::ResultStatus status = tree->getFromIndex( static_cast<uint32_t>(LexIdNorm(lex)), &link );
    if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS &&
                status != PatriciaTreeCore::RESULT_STATUS_NOT_FOUND) )
        return LP_ERROR_EFAULT;

    if ( status == PatriciaTreeCore::RESULT_STATUS_NOT_FOUND )
        return 0;

    const char *stem = reinterpret_cast<const char *>( tree->linkToKeyAddress(link) );
    if ( unlikely(stem == 0) )
        return LP_ERROR_EFAULT;

    /* Fill output buffer */
    size_t stem_len  = strlen(stem);
    size_t total_len = stem_len + 1;

    if (buffer == 0 || bufferSize < total_len)
        return static_cast<int>(total_len);

    memcpy(buffer, stem, stem_len);
    buffer[stem_len] = '~';
    if (bufferSize > total_len)
        buffer[total_len] = '\0';
    
    TildeCustom::postprocessWord( buffer, total_len + 1 );

    char tmp[ TildeCustom::TILDEMORPH_MAX_WORD_LEN + 1 ];
    memcpy( tmp, buffer, total_len );
    
    /* Restore capitalization if any */
    size_t i = 0;
    size_t j = 0;
    bool   firstUpper = false;
    while ( i < total_len )
    {
        uint32_t ch;
        U8_NEXT_UNSAFE( tmp, i, ch );
        if ( LexIsTitle( lex ) && !firstUpper && _charset->IsLower( ch ) )
        {
            ch = _charset->ToUpper( ch );
            firstUpper = true;
        }
        else if ( LexIsUpper( lex ) )
            ch = _charset->ToUpper( ch );

        U8_APPEND_UNSAFE( buffer, j, ch );
    }

    return static_cast<int>(total_len);
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::enumerate(MorphEnumerator &enumerator)
{
    size_t morphSize = size();

    for (size_t n = 0; n < morphSize; n++)
    {
        LEXID lex = LexSetNoMrph(static_cast<LEXID>(n+1), _info.internalNumber);
        LingProcErrors result = enumerator.nextLexid(lex);
        if ( unlikely(result != LP_OK) )
            return result;
    }

    return LP_OK;
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::FSCreateFromTemplate(const UCharSet      *charset,
                                                  fstorage            *templateFs,
                                                  fstorage_section_id  sectionId)
{
    return FSOpenRW(charset, templateFs, sectionId);
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::FSOpenRW(const UCharSet      *charset,
                                      fstorage            *fs,
                                      fstorage_section_id  sectionId)
{
    LingProcErrors result;
    result = openCommon(charset, fs, sectionId);
    if (result != LP_OK)
        return result;

    /* Create the stems tree */
    tree = new PatriciaTreeFS();
    if ( unlikely(tree == NULL) )
    {
        close();
        return LP_ERROR_ENOMEM;
    }

    /* Load the stem tree */
    PatriciaTreeCore::ResultStatus status = tree->load( treeFlags, fs, sectionId + TildeCustom::TILDEMORPH_FS_SEC_STEMS );
    if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
    {
        close();
        return LP_ERROR_FS_FAILED;
    }

    return LP_OK;
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::FSOpenRO(const UCharSet      *charset,
                                      fstorage            *fs,
                                      fstorage_section_id  sectionId)
{
    LingProcErrors result;
    result = openCommon(charset, fs, sectionId);
    if (result != LP_OK)
        return result;

    /* Create the stems tree */
    tree = new PatriciaTreeFS();
    if ( unlikely(tree == NULL) )
    {
        close();
        return LP_ERROR_ENOMEM;
    }

    /* Load the stem tree */
    PatriciaTreeCore::ResultStatus status = tree->load( treeFlags | PatriciaTree::TREE_READ_ONLY,
                                                        fs,
                                                        sectionId + TildeCustom::TILDEMORPH_FS_SEC_STEMS );
    if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
    {
        close();
        return LP_ERROR_FS_FAILED;
    }

    return LP_OK;
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::FSOpenLimited(const UCharSet      *charset,
                                           fstorage            *fs,
                                           fstorage_section_id  sectionId)
{
    return FSOpenRO( charset, fs, sectionId );
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::FSSave(fstorage            *fs,
                                    fstorage_section_id  sectionId,
                                    bool                 transport)
{
    if ( unlikely(fs == 0) )
        return LP_ERROR_EINVAL;

    if ( unlikely(!isValid() || fstorage_is_shared(fs)) ) // do not save if openLimited()
        return LP_ERROR_INVALID_MODE;

    /* TODO: libpts2 transport mode */
    if ( transport )
        return LP_ERROR_ENOTIMPL;

    /* Initialize the header */
    memset(&headerData, 0, sizeof(HeaderData));
    headerData.magic                    = TildeCustom::TILDEMORPH_FS_MAGIC;
    headerData.work_format_version      = TildeCustom::TILDEMORPH_FS_WORK_FORMAT_VERSION;
    headerData.work_format_variant      = TildeCustom::TILDEMORPH_FS_WORK_FORMAT_VARIANT;

    /* Store the header */
    LingProcErrors result;
    result = serializeHeader(fs, sectionId);
    if ( unlikely(result != LP_OK) )
        return result;

    /* Store stems */
    PatriciaTreeCore::ResultStatus status = tree->save( fs, sectionId + TildeCustom::TILDEMORPH_FS_SEC_STEMS );
    if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
        return LP_ERROR_FS_FAILED;

    return LP_OK;
}

template< typename TildeCustom >
void LPMorphTildeImpl<TildeCustom>::initData()
{
    _charset  = 0;
    tree      = 0;
    memset(&headerData, 0, sizeof(HeaderData));
    memset(&_info,      0, sizeof(LPMorphDictInfo));
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::deserializeHeader(fstorage *fs, fstorage_section_id sectionId)
{
    /* Find the section */
    fstorage_section *section = fstorage_find_section(fs, sectionId);
    if ( unlikely(section == 0) )
        return LP_ERROR_FS_FAILED;

    /* Obtain the pointer to the section */
    uint32_t *sectionPointer = (uint32_t *)fstorage_section_get_all_data(section);
    if ( unlikely(sectionPointer == 0) )
        return LP_ERROR_FS_FAILED;

    /* Verify the section size */
    if ( unlikely(fstorage_section_get_size(section) < TildeCustom::TILDEMORPH_FS_HEADER_SIZE) )
        return LP_ERROR_FS_FAILED;

    /* Load the data */
    uint32_t *pt = sectionPointer;

    memset(&headerData, 0, sizeof(HeaderData));
    headerData.magic                    = be32toh(*pt++);
    headerData.work_format_version      = be32toh(*pt++);
    headerData.work_format_variant      = be32toh(*pt++);

    memset(&_info, 0, sizeof(LPMorphDictInfo));
    _info.type           = static_cast<MorphTypes>(be32toh(*pt++));
    _info.version        = be32toh(*pt++);
    _info.variant        = be32toh(*pt++);
    _info.lang           = static_cast<LangCodes>(be32toh(*pt++));
    _info.globalId       = be32toh(*pt++);
    _info.internalNumber = be32toh(*pt++);
    _info.minStemLength  = be32toh(*pt++);
    _info.creationFlags  = be32toh(*pt++);

    /* Verify the data amount read */
    if ( unlikely( static_cast<size_t>(reinterpret_cast<char *>(pt) -
                    reinterpret_cast<char *>(sectionPointer)) != TildeCustom::TILDEMORPH_FS_HEADER_SIZE) )
        return LP_ERROR_EFAULT; // fatal error in program

    /* Check if the versions match */
    if ( unlikely(headerData.magic != TildeCustom::TILDEMORPH_FS_MAGIC) )
        return LP_ERROR_INVALID_FORMAT;

    if ( unlikely(headerData.work_format_version != TildeCustom::TILDEMORPH_FS_WORK_FORMAT_VERSION) )
        return LP_ERROR_INVALID_FORMAT;

    if ( unlikely(_info.version != TildeCustom::TILDEMORPH_VERSION) )
        return LP_ERROR_INVALID_FORMAT;

    return LP_OK;
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::serializeHeader(fstorage *fs, fstorage_section_id sectionId)
{
    /* Obtain the section */
    fstorage_section *section = fstorage_get_section(fs, sectionId);
    if ( unlikely(section == 0) )
        return LP_ERROR_FS_FAILED;

    /* Allocate the memory */
    if ( unlikely(fstorage_section_get_size(section) != TildeCustom::TILDEMORPH_FS_HEADER_SIZE) )
    {
        if ( unlikely(fstorage_section_realloc(section, TildeCustom::TILDEMORPH_FS_HEADER_SIZE) != FSTORAGE_OK) )
            return LP_ERROR_FS_FAILED;
    }

    /* Obtain the pointer */
    uint32_t *sectionPointer = (uint32_t *)fstorage_section_get_all_data(section);
    if ( unlikely(sectionPointer == 0) )
        return LP_ERROR_FS_FAILED;

    /* Store the data */
    memset(sectionPointer, 0, TildeCustom::TILDEMORPH_FS_HEADER_SIZE);

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

    /* Check the data amount written */
    if ( unlikely( static_cast<size_t>(reinterpret_cast<char *>(pt) -
                reinterpret_cast<char *>(sectionPointer)) != TildeCustom::TILDEMORPH_FS_HEADER_SIZE) )
        return LP_ERROR_EFAULT; // fatal error in program

    return LP_OK;
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::initCharSet(const UCharSet *charset)
{
    if ( unlikely(charset == 0 || !charset->IsValid()) )
        return LP_ERROR_EINVAL;
    if ( unlikely(getLang() != charset->GetLang()) )
        return LP_ERROR_EINVAL;

    _charset = charset;
    
    /* Initialize word normalizer */
    normalizer = WordNormalizer::factory( getLang(), _info.type );
    if ( unlikely(normalizer == 0) )
        return LP_ERROR_EFAULT;
    
    WordNormalizerResult status = normalizer->create( _charset, _info.creationFlags );
    if ( unlikely(status != NORMALIZE_SUCCESS) )
        return LP_ERROR_EFAULT;
    
    return LP_OK;
}

template< typename TildeCustom >
LingProcErrors LPMorphTildeImpl<TildeCustom>::openCommon(const UCharSet *charset, fstorage *fs, fstorage_section_id sectionId)
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
        return LP_ERROR_EINVAL;
    }

    return LP_OK;
}

/* Force instantiation of the objects needed */

template class LPMorphTildeImpl< TildeCustomDirect >;
template class LPMorphTildeImpl< TildeCustomReversed >;

