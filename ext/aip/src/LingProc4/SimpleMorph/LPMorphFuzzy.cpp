/**
 * @file   LPMorphFuzzy.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed Apr  9 15:25:44 2008
 * 
 * @brief  Fuzzy morphology for LingProc (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_string.h>
#include <_include/_inttype.h>
#include <_include/ntoh.h>
#include <lib/libpts/ptsfs.h>

#include "LPMorphFuzzy.h"
#include "../data/morph_tables.h"

/* Set fuzzy morphologies constants */
const uint32_t FUZZYMORPH_MAX_WORD_LEN                = lpMaxWordLength * 4 + 4;
const uint32_t FUZZYMORPH_MIN_STEM_LEN                = 2;
const uint32_t FUZZYMORPH_VERSION                     = 2;
const uint32_t FUZZYMORPH_VARIANT                     = 1;
const int      FUZZYMORPH_SPEC_SUFFIX                 = 255;

const size_t   FUZZYMORPH_FS_HEADER_SIZE              = (sizeof(uint32_t) * 11);
const uint32_t FUZZYMORPH_FS_MAGIC                    = 0x05095950U;
const uint32_t FUZZYMORPH_FS_WORK_FORMAT_VERSION      = 2;
const uint32_t FUZZYMORPH_FS_WORK_FORMAT_VARIANT      = 1;

const int      FUZZYMORPH_FS_SEC_SUFFIXES             = 1;
const int      FUZZYMORPH_FS_SEC_STOPWORDS            = 2;
const int      FUZZYMORPH_FS_SEC_STEMS                = 3;

/* Implementation */

LPMorphFuzzy::LPMorphFuzzy() : LPMorphInterface()
{
    initData();
}

LPMorphFuzzy::~LPMorphFuzzy()
{
    close();
}

LingProcErrors LPMorphFuzzy::create(const UCharSet *charset, const LPMorphCreateParams *params)
{
    /* Flush first the data if any */
    close();

    /* Check parameters */
    if ( unlikely(params == 0 || charset == 0) )
        return LP_ERROR_EINVAL;

    if ( unlikely(params->type != MORPH_TYPE_FUZZY &&
                  params->type != MORPH_TYPE_HIER) ) 
        return LP_ERROR_UNSUPPORTED_MORPH;

    _charset             = charset;

    /* Fill the LPMorphDictInfo info */
    _info.type           = params->type;
    _info.version        = FUZZYMORPH_VERSION;
    _info.variant        = FUZZYMORPH_VARIANT;
    _info.lang           = params->lang;
    _info.globalId       = (_info.type << 24) | (_info.lang);
    _info.internalNumber = params->internalNumber;

    _info.minStemLength = ( params->minStemLength == 0 ? FUZZYMORPH_MIN_STEM_LEN : params->minStemLength );
        
    if ( unlikely(params->flags & (MORPH_FLAG_USE_DEFAULT | MORPH_FLAG_INVALID)) )
        return LP_ERROR_MORPH_FLAGS;
        
    _info.creationFlags = params->flags;
        
    /* Clear all except only meaning flags */
    unsigned int storeFlags     = 
        MORPH_FLAG_CASE_INSENSITIVE    |
        MORPH_FLAG_LWR_ACCENT          |
        MORPH_FLAG_LWR_PUNCT           |
        MORPH_FLAG_REM_IGNORE          |
        MORPH_FLAG_EXPAND_LIGATURES    |
        ( (_info.type == MORPH_TYPE_FUZZY || params->flags & MORPH_FLAG_HIER_NATIVE_ONLY) ? MORPH_FLAG_NATIVE_ONLY : 0 ) |
        MORPH_FLAG_HIER_NATIVE_ONLY    |
        MORPH_FLAG_ARABIC_SEQ_COLLAPSE;
                
    _info.creationFlags &= storeFlags;
       
    /* Initialize the charset */
    LingProcErrors ret = initCharSet( _charset );
    if ( unlikely(ret != LP_OK) )
    {
        close();
        return ret;
    }

    /* Initialize the suffix and stop tables */
    const char * const *suffTable = NULL;
    const char * const *stopTable = NULL;

    if (_info.type == MORPH_TYPE_FUZZY)
    {
        switch ( params->lang )
        {
        case LNG_UKRAINIAN:
        case LNG_RUSSIAN:
            suffTable = morph_table_suff_rus;
            stopTable = morph_table_stop_rus;
            break;
        case LNG_ENGLISH:
            suffTable = morph_table_suff_eng;
            stopTable = morph_table_stop_eng;
            break;
        case LNG_EURO:
            suffTable = morph_table_suff_eur;
            stopTable = morph_table_stop_eur;
            break;
        case LNG_GERMAN:
            suffTable = morph_table_suff_deu;
            stopTable = morph_table_stop_deu;
            break;
        case LNG_FRENCH:
            suffTable = morph_table_suff_fra;
            stopTable = morph_table_stop_fra;
            break;
        case LNG_SPANISH:
            suffTable = morph_table_suff_esp;
            stopTable = morph_table_stop_esp;
            break;
        case LNG_VIETNAMESE:
            suffTable = morph_table_suff_vie; // These are hardcoded empty tables: 
            stopTable = morph_table_stop_vie; // there is no fuzzy morph for vietnamese language!
            break;
        case LNG_ARABIC:
            suffTable = morph_table_suff_ara; // These are hardcoded empty tables: 
            stopTable = morph_table_stop_ara; // there is no fuzzy morph for vietnamese language!
            break;
        case LNG_CYRILLIC:
            suffTable = morph_table_suff_cyr;
            stopTable = morph_table_stop_cyr;
            break;
        default:
            close();
            return LP_ERROR_UNSUPPORTED_LANG;
        }
    }

    /* Initialize suffixes */
    if ( suffTable )
    {
        const char * const *next = suffTable;
        for ( size_t index = 1; *next != NULL; next++, index++ )
        {
            FlexionsHash::ResultStatus status = suffixes.access( *next, index );
            if ( unlikely(status != FlexionsHash::RESULT_STATUS_SUCCESS) )
            {
                close();
                return LP_ERROR_ENOMEM;
            }

            suffixesBackRef.push_back( *next );
            if ( unlikely(suffixesBackRef.no_memory()) )
            {
                close();
                return LP_ERROR_ENOMEM;
            }
        }
    }

    /* Initialize stop words */
    if ( stopTable )
    {
        stopWords = pat_tree_create();
        if ( unlikely(stopWords == NULL) )
        {
            close();
            return LP_ERROR_ENOMEM;
        }

        const char * const *next = stopTable;
        while ( *next != NULL )
        {
            if ( unlikely(pat_tree_append( stopWords,
                                           reinterpret_cast<const pat_char *>(*next++) ) == PAT_ID_NONE) )
            {
                close();
                return LP_ERROR_ENOMEM;
            }
        }
    }

    /* Initialize stem tree */
    stems = pts_create();
    if ( unlikely(stems == NULL) )
    {
        close();
        return LP_ERROR_ENOMEM;
    }
        
    return LP_OK;
}

LingProcErrors LPMorphFuzzy::flush()
{
	if ( likely(stems != 0) )
                pts_cleanup( stems );
        return LP_OK;
}

/* !!! When saving close() must be called AFTER fstorage_close() !!! */
LingProcErrors LPMorphFuzzy::close()
{
    if ( suffixesBackRef.size() > 0 )
    {
        suffixes.clear();
        suffixesBackRef.clear();
    }

    if ( stopWords )
        pat_tree_destroy( stopWords );

    if ( stems != 0 )
        pts_destroy( stems );

    if ( normalizer != 0 )
        delete normalizer;

    initData();

    return LP_OK;
}

int LPMorphFuzzy::cutSuffix( unsigned char *outBuffer,
			     LEXID         &lexid,
			     const char    *word,
			     size_t         wordLength )
{
    /* Check if some parameters are invalid */
    if ( unlikely(!isValid()) ) 
        return LP_ERROR_INVALID_MODE;
        
    WordNormalizerTarget f( reinterpret_cast<char *>(outBuffer), FUZZYMORPH_MAX_WORD_LEN );

    /* Determine which normalizer to use */
    WordNormalizerResult status = normalizer->normalizeWord( f, word, wordLength );
        
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
    switch ( f.caps )
    {
    case NORMALIZE_CAPS_NO_CASE:    caps = CAP_NOCASE;    break;
    case NORMALIZE_CAPS_LOWER_CASE: caps = CAP_LOWERCASE; break;
    case NORMALIZE_CAPS_TITLE_CASE: caps = CAP_TITLECASE; break;
    case NORMALIZE_CAPS_UPPER_CASE: caps = CAP_UPPERCASE; break;
    };
        
    /* If empty word */
    if ( unlikely(lemmatizeLength == 0) )
        return 0;
        
    if ( lemmatizeLength < FUZZYMORPH_MAX_WORD_LEN )
        outBuffer[ lemmatizeLength ] = '\0';
        
    /* Search the suffix */
    size_t suffixId = 0;
    if ( suffixesBackRef.size() > 0 )
    {
        uint32_t ch;
        int32_t  i = lemmatizeLength;
        U8_PREV( outBuffer, 0, i, ch );
        
        if ( ch == static_cast<uint32_t>('$') )
        {
            suffixId                      = FUZZYMORPH_SPEC_SUFFIX;
            lemmatizeLength              -= 1;
            outBuffer[ lemmatizeLength ]  = '\0';
        }
        else 
        {
            size_t j = 0;
            for ( i = 0; j < (int32_t)_info.minStemLength; j++ )
            {
                U8_NEXT_UNSAFE( outBuffer, i, ch );
                if ( ch == static_cast<uint32_t>('\0') )
                    break;
            }
                
            if ( i < (int32_t)lemmatizeLength )
            {
                /* Check the stop word first */
                if ( pat_tree_lookup( stopWords, outBuffer, PAT_LOOKUP_EXACT_MATCH ) == PAT_ID_NONE )
                {
                    for ( ;; )
                    {                    
                        size_t *id = 0;
                        if ( suffixes.get( reinterpret_cast<const char *>(outBuffer) + i, &id ) == FlexionsHash::RESULT_STATUS_SUCCESS )
                        {
                            suffixId                      = *id;
                            lemmatizeLength               =  i;
                            outBuffer[ lemmatizeLength ]  = '\0';
                            break;
                        }
                    
                        U8_NEXT_UNSAFE( outBuffer, i, ch );
                        if ( ch == 0 )
                            break;
                    }
                }
            }
        }
    }

    // Check and build LEXID
    if ( suffixId != 0 )
        _LexSetForm( &lexid, static_cast<unsigned int>(suffixId) );
    lexid |= caps;
    _LexSetNoMrph( &lexid, _info.internalNumber );

    /* Return the length of the result in the out buffer */
    return (unsigned int)lemmatizeLength;
}

int LPMorphFuzzy::lemmatize( LemmatizeResultFunctor &functor,
			     const char             *word,
			     size_t                  wordLength,
			     unsigned int            flags )
{
    LEXID         lexid = 0;
    unsigned char lemmatizeBuffer[ FUZZYMORPH_MAX_WORD_LEN + 1 ];

    int result = cutSuffix( lemmatizeBuffer, lexid, word, wordLength );
    if (result <= 0)
        return result;
        
    /* Searching the stem */
    LEXID lex = PAT_ID_NONE;
	
    if (flags & (MORPH_FLAG_NO_APPEND | MORPH_FLAG_FORCE_UNKNOWN))
        lex = pts_lookup(stems, lemmatizeBuffer);
    else if ( (uint64_t)(pts_items_count(stems) + 1) >= (uint64_t)(_info.type == MORPH_TYPE_HIER ?
                                                                   LEX_IDHIER - 1 : LEX_IDNORM) )
        /* Maximum capacity has been reached */
        lex = pts_lookup( stems, lemmatizeBuffer );
    else
        /* Add new stem */
        lex = pts_append( stems, lemmatizeBuffer );

    if ( lex == PAT_ID_NONE )
    {
        if ( !(flags & MORPH_FLAG_FORCE_UNKNOWN) )
            return 0;
        lex = LEX_UNKNOWN;
    }

    /* Merge lexids */
    lexid |= lex;

    /* Invoke the functor */
    LingProcErrors status = functor.nextLemma( lexid, 0, 0 );
    if ( unlikely(status != LP_OK) )
        return status;
	
    return 1;
}

int LPMorphFuzzy::stemmatize( StemmatizeResultFunctor &functor,
			      const char              *word,
			      size_t                   wordLength,
			      unsigned int             /* flags */ )
{
    /* Reserve the memory */
    size_t  requiredSize = wordLength + 4;
    char   *stem         = functor.getStemBuffer( requiredSize );
    if ( unlikely(stem == 0) )
        return LP_ERROR_ENOMEM;
        
    char *p = stem;
    if ( _info.type != MORPH_TYPE_HIER )
    {
        p    = dumpHex( realMorphNumber< LEXID >( _info ), 2, p );
        *p++ = '$';
    }
    int resultAdd = (int)( p - stem );
        
    LEXID lexid  = 0;
    int   result = cutSuffix( reinterpret_cast<unsigned char *>(p), lexid, word, wordLength );
    if ( result <= 0 )
        return result;
        
    /* Invoke the functor */
    LingProcErrors status = functor.nextStem( lexid, 0, stem, static_cast<size_t>(result + resultAdd), 0 );
    if ( unlikely(status != LP_OK) )
        return status;
	
    return 1;
}

int LPMorphFuzzy::normalize(NormalizeResultFunctor &functor,
                            const char             *word,
                            size_t                  wordLength,
                            unsigned int            /* flags */)
{
    class FunctorAdapter : public WordNormalizerFunctor
    {
    public:
        FunctorAdapter( NormalizeResultFunctor &_functor, unsigned int _morphNumber ) :
            functor( _functor ), morphNumber( _morphNumber ), status( LP_OK ) { }
        ~FunctorAdapter() { }
                
    public:
        char *getResultBuffer( size_t requestSize ) { return functor.getResultBuffer(requestSize); }
                
        WordNormalizerResult nextResult( const char         *result,
                                         size_t              resultSize,
                                         WordNormalizerCaps  caps )
        {
            LEXID lex = 0;
            switch ( caps )
            {
            case NORMALIZE_CAPS_NO_CASE:    lex = CAP_NOCASE;    break;
            case NORMALIZE_CAPS_LOWER_CASE: lex = CAP_LOWERCASE; break;
            case NORMALIZE_CAPS_TITLE_CASE: lex = CAP_TITLECASE; break;
            case NORMALIZE_CAPS_UPPER_CASE: lex = CAP_UPPERCASE; break;
            };

            _LexSetNoMrph( &lex, morphNumber );

            status = functor.nextResult( lex, result, resultSize );
            if ( unlikely(status != LP_OK) )
                return NORMALIZE_ERROR_FUNCTOR;
            return NORMALIZE_SUCCESS;
        }

        LingProcErrors getStatus() const { return status; }
                
    private:
        NormalizeResultFunctor &functor;
        unsigned int            morphNumber;
        LingProcErrors          status;
    } adapter( functor, _info.internalNumber );

    WordNormalizerResult status = normalizer->normalizeWord( adapter, word, wordLength );
    switch ( status )
    {
    case NORMALIZE_SUCCESS:
        break;
                
    case NORMALIZE_ERROR_PARAMETER:                
        return LP_ERROR_EINVAL;
        
    case NORMALIZE_ERROR_FLAGS:
        return LP_ERROR_MORPH_FLAGS;
                                
    case NORMALIZE_ERROR_BUFFER:
        return LP_ERROR_ENOMEM;
                
    case NORMALIZE_ERROR_NOT_NATIVE:
        return 0;

    case NORMALIZE_ERROR_INTERNAL:
        return LP_ERROR_EFAULT;
        
    case NORMALIZE_ERROR_FUNCTOR:
        return adapter.getStatus();
    };
        
    return 1;
}

int LPMorphFuzzy::getLexText( LEXID lex, char *buffer, size_t bufferSize, const BinaryData * /* additionalData */ )
{
    if ( unlikely(!isValid() ) )
        return LP_ERROR_INVALID_MODE;

    if ( unlikely(LexNoMrph(lex) != _info.internalNumber ) )
        return 0;
        
                       
    /* Obtain the stem */
    const char *stem = 0;
        
    /* Check for LEX_UNKNOWN */
    if ( LexIdHier(lex) == LEX_UNKNOWN )
        stem = "##";
    else if (_info.type == MORPH_TYPE_HIER)
        stem = reinterpret_cast<const char *>(pts_find_id( stems, static_cast<unsigned int>(LexIdHier(lex))) );
    else
        stem = reinterpret_cast<const char *>(pts_find_id( stems, static_cast<unsigned int>(LexIdNorm(lex))) );
    
    if ( unlikely(stem == 0) )
        return 0;

    return getLexText( lex, stem, buffer, bufferSize );
}

int LPMorphFuzzy::getLexText( LEXID partialLex, const char *stem, char *buffer, size_t bufferSize, const BinaryData * /* additionalData */ )
{
    if ( unlikely(!isValid() ) )
        return LP_ERROR_INVALID_MODE;

    if ( unlikely(LexNoMrph(partialLex) != _info.internalNumber ) )
        return 0;
        
    /* Obtain the suffix */
    const char *suff = 0;
        
    /* Check for LEX_UNKNOWN */
    if ( LexIdHier(partialLex) == LEX_UNKNOWN )
        suff = "";
    else if (_info.type == MORPH_TYPE_HIER)
        suff = "";
    else
    {
        int suffixId = LexIdForm(partialLex);
        if (suffixId == 0)
            suff = "";
        else if (suffixId == FUZZYMORPH_SPEC_SUFFIX)
            suff = "$";
        else if (suffixId > 0 && static_cast<size_t>(suffixId - 1) < suffixesBackRef.size())
            suff = suffixesBackRef[suffixId - 1];
        else
            suff = 0;
    }
    
    if ( unlikely(stem == 0 || suff == 0) )
        return 0;

    /* Fix stem to cut off '$' morph part */
    if ( _info.type != MORPH_TYPE_HIER )
    {
        uint32_t ch;
        size_t   i = 0;
        for ( ;; )
        {
            U8_NEXT_UNSAFE( stem, i, ch );
            if ( ch == 0 || ch == static_cast<uint32_t>('$') )
                break;
        }
        if ( ch == static_cast<uint32_t>('$') )
            stem += i;
    }
        
    /* Fill output buffer */
    const char *p           = stem;
    size_t      i           = 0;
    size_t      j           = 0;
    size_t      totalLength = 0;
    bool        firstUpper  = false;
    for ( ;; )
    {
        uint32_t ch;
        U8_NEXT_UNSAFE( p, i, ch );
        if ( ch == 0 )
        {
            if ( p != stem )
                break;
            p = suff;
            i = 0;
            continue;
        }

        totalLength += U8_LENGTH( ch );
        if ( buffer != 0 && totalLength <= bufferSize )
        {
            /* Restore capitalization if any */
            if ( LexIsTitle( partialLex ) && !firstUpper && _charset->IsLower( ch ) )
            {
                ch = _charset->ToUpper( ch );
                firstUpper = true;
            }
            else if ( LexIsUpper( partialLex ) )
                ch = _charset->ToUpper( ch );
            
            U8_APPEND_UNSAFE( buffer, j, ch );
        }
    }   

    if ( bufferSize > totalLength )
        buffer[ totalLength ] = '\0';

    return static_cast<int>(totalLength);
}

LingProcErrors LPMorphFuzzy::enumerate(MorphEnumerator &enumerator)
{
    size_t morphSize = size();
	
    for ( size_t n = 0; n < morphSize; n++ )
    {
        LEXID lex = LexSetNoMrph( static_cast<LEXID>(n+1), _info.internalNumber );
        LingProcErrors result = enumerator.nextLexid( lex );
        if ( unlikely(result != LP_OK) )
            return result;
    }

    return LP_OK;
}

LingProcErrors LPMorphFuzzy::FSCreateFromTemplate( const UCharSet      *charset,
                                                   fstorage            *templateFs,
                                                   fstorage_section_id  sectionId )
{
    return FSOpenRW( charset, templateFs, sectionId );
}

LingProcErrors LPMorphFuzzy::FSOpenRW( const UCharSet      *charset,
                                       fstorage            *fs,
                                       fstorage_section_id  sectionId )
{
    LingProcErrors result;
    result = openCommon( charset, fs, sectionId );
    if (result != LP_OK)
        return result;

    /* Load the stop words */
    if ( !isHier() )
    {
        stopWords = pat_tree_fs_open_rw( fs, sectionId + FUZZYMORPH_FS_SEC_STOPWORDS );
        if ( stopWords == 0 )
        {
            close();
            return LP_ERROR_FS_FAILED;
        }
    }

    /* Load the stem tree */
    stems = pts_fs_open_rw( fs, sectionId + FUZZYMORPH_FS_SEC_STEMS );
    if ( stems == 0 )
    {
        close();
        return LP_ERROR_FS_FAILED;
    }

    return LP_OK;
}

LingProcErrors LPMorphFuzzy::FSOpenRO( const UCharSet      *charset,
                                       fstorage            *fs,
                                       fstorage_section_id  sectionId )
{
    LingProcErrors result;
    result = openCommon( charset, fs, sectionId );
    if (result != LP_OK)
        return result;

    /* Load the stop words */
    if ( !isHier() )
    {
        stopWords = pat_tree_fs_open_ro( fs, sectionId + FUZZYMORPH_FS_SEC_STOPWORDS );
        if ( stopWords == 0 )
        {
            close();
            return LP_ERROR_FS_FAILED;
        }
    }

    /* Load the stem tree */
    stems = pts_fs_open_ro( fs, sectionId + FUZZYMORPH_FS_SEC_STEMS );
    if (stems == 0)
    {
        close();
        return LP_ERROR_FS_FAILED;
    }

    return LP_OK;
}

LingProcErrors LPMorphFuzzy::FSOpenLimited( const UCharSet      *charset,
                                            fstorage            *fs,
                                            fstorage_section_id  sectionId )
{
    LingProcErrors result;
    result = openCommon( charset, fs, sectionId );
    if ( result != LP_OK )
        return result;

    /* Load the stop words -- no need in this mode */

    /* Load the stem tree */
    stems = pts_fs_open_limited( fs, sectionId + FUZZYMORPH_FS_SEC_STEMS );
    if ( stems == 0 )
    {
        close();
        return LP_ERROR_FS_FAILED;
    }

    return LP_OK;
}

LingProcErrors LPMorphFuzzy::FSSave( fstorage            *fs,
                                     fstorage_section_id  sectionId,
                                     bool                 transport )
{
    if ( fs == 0 )
        return LP_ERROR_EINVAL;

    if ( !isValid() || fstorage_is_shared(fs) ) // do not save if openLimited()
        return LP_ERROR_INVALID_MODE;

    if ( transport )
        return LP_ERROR_ENOTIMPL;

    /* Initialize the header */
    memset(&headerData, 0, sizeof(HeaderData));
    headerData.magic                    = FUZZYMORPH_FS_MAGIC;
    headerData.work_format_version      = FUZZYMORPH_FS_WORK_FORMAT_VERSION;
    headerData.work_format_variant      = FUZZYMORPH_FS_WORK_FORMAT_VARIANT;

    /* Store the header */
    LingProcErrors result;
    result = serializeHeader( fs, sectionId );
    if ( unlikely(result != LP_OK) )
        return result;

    /* Store suffixes */
    if ( suffixesBackRef.size() > 0 )
    {
        /* Calculate the size needed */
        size_t      suffSize = 0;
        int         n;
        const char *suff;
		
        for ( n = 0; static_cast<size_t>(n) < suffixesBackRef.size(); n++ )
            suffSize += strlen( suffixesBackRef[n] ) + 1;
        suffSize++;

        /* Obtain the section */
        fstorage_section *section = fstorage_get_section( fs, sectionId + FUZZYMORPH_FS_SEC_SUFFIXES );
        if (section == 0)
            return LP_ERROR_FS_FAILED;

        /* Resize the section if need to */
        if ( fstorage_section_get_size(section) != suffSize )
        {
            if ( fstorage_section_realloc(section, suffSize ) != FSTORAGE_OK )
                return LP_ERROR_FS_FAILED;
        }

        /* Obtain the pointer to the section */
        char *pt = static_cast<char *>( fstorage_section_get_all_data(section) );
        if ( pt == 0 )
            return LP_ERROR_FS_FAILED;

        /* Store the data */
        size_t wr = 0;
        for (n = 0; static_cast<size_t>(n) < suffixesBackRef.size(); n++)
        {
            suff = suffixesBackRef[n];
            size_t size = strlen(suff) + 1;
            if (wr + size > suffSize)
                return LP_ERROR_EFAULT;
            memcpy( pt + wr, suff, size );
            wr += size;
        }
        if ( wr != suffSize - 1 )
            return LP_ERROR_EFAULT;
        pt[wr] = '\0';
    }

    unsigned int ptsFlags = PTS_SAVE_DATA_FORMAT;

    /* Store stop words */
    if ( stopWords != 0 )
    {
        if ( pat_tree_fs_save( stopWords, fs, sectionId + FUZZYMORPH_FS_SEC_STOPWORDS, ptsFlags ) != 0 )
            return LP_ERROR_FS_FAILED;
    }

    /* Store stems */
    if ( pts_fs_save(stems, ptsFlags, fs, sectionId + FUZZYMORPH_FS_SEC_STEMS) != 0 )
        return LP_ERROR_FS_FAILED;

    return LP_OK;
}

void LPMorphFuzzy::initData()
{
    _charset   = 0;
    stopWords  = 0;
    stems      = 0;
    normalizer = 0;
    memset( &headerData, 0, sizeof(HeaderData) );
    memset( &_info,      0, sizeof(LPMorphDictInfo) );
}

LingProcErrors LPMorphFuzzy::deserializeHeader(fstorage *fs, fstorage_section_id sectionId)
{
    /* Find the section */
    fstorage_section *section = fstorage_find_section(fs, sectionId);
    if (section == 0)
        return LP_ERROR_FS_FAILED;

    /* Obtain the pointer to the section */
    uint32_t *sectionPointer = (uint32_t *)fstorage_section_get_all_data(section);
    if (sectionPointer == 0)
        return LP_ERROR_FS_FAILED;

    /* Verify the section size */
    if (fstorage_section_get_size(section) < FUZZYMORPH_FS_HEADER_SIZE)
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
    if ( static_cast<size_t>(reinterpret_cast<char *>(pt) - reinterpret_cast<char *>(sectionPointer)) != FUZZYMORPH_FS_HEADER_SIZE)
        return LP_ERROR_EFAULT; // fatal error in program

    /* Check if the versions match */
    if (headerData.magic != FUZZYMORPH_FS_MAGIC)
        return LP_ERROR_INVALID_FORMAT;

    if (headerData.work_format_version != FUZZYMORPH_FS_WORK_FORMAT_VERSION)
        return LP_ERROR_INVALID_FORMAT;

    if (_info.version != FUZZYMORPH_VERSION)
        return LP_ERROR_INVALID_FORMAT;
  
    return LP_OK;
}

LingProcErrors LPMorphFuzzy::serializeHeader(fstorage *fs, fstorage_section_id sectionId)
{
    /* Obtain the section */
    fstorage_section *section = fstorage_get_section(fs, sectionId);
    if (section == 0)
        return LP_ERROR_FS_FAILED;

    /* Allocate the memory */
    if (fstorage_section_get_size(section) != FUZZYMORPH_FS_HEADER_SIZE)
    {
        if (fstorage_section_realloc(section, FUZZYMORPH_FS_HEADER_SIZE) != FSTORAGE_OK)
            return LP_ERROR_FS_FAILED;
    }

    /* Obtain the pointer */
    uint32_t *sectionPointer = (uint32_t *)fstorage_section_get_all_data(section);
    if (sectionPointer == 0)
        return LP_ERROR_FS_FAILED;

    /* Store the data */
    memset(sectionPointer, 0, FUZZYMORPH_FS_HEADER_SIZE);

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
    if ( static_cast<size_t>(reinterpret_cast<char *>(pt) - reinterpret_cast<char *>(sectionPointer)) != FUZZYMORPH_FS_HEADER_SIZE)
        return LP_ERROR_EFAULT; // fatal error in program

    return LP_OK;
}

LingProcErrors LPMorphFuzzy::initCharSet(const UCharSet *charset)
{
    if ( charset == 0 || !charset->IsValid() )
        return LP_ERROR_EINVAL;
    if ( getLang() != charset->GetLang() )
        return LP_ERROR_UNSUPPORTED_LANG;
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

LingProcErrors LPMorphFuzzy::openCommon( const UCharSet *charset, fstorage *fs, fstorage_section_id sectionId )
{
    /* Common part of FSOpenRO() and FSOpenRW() */

    close();

    if (fs == 0 || charset == 0)
        return LP_ERROR_EINVAL;

    LingProcErrors result;

    /* Load the header and dictionary information */
    result = deserializeHeader( fs, sectionId );
    if (result != LP_OK)
    {
        close();
        return result;
    }

    /* Initialize the charset */
    result = initCharSet( charset );
    if (result != LP_OK)
    {
        close();
        return result;
    }

    /* Load the suffixes */
    if ( !isHier() )
    {
        result = loadSuffixes( fs, sectionId + FUZZYMORPH_FS_SEC_SUFFIXES );
        if ( unlikely(result != LP_OK) )
        {
            close();
            return result;
        }
    }

    return LP_OK;
}

LingProcErrors LPMorphFuzzy::loadSuffixes( fstorage *fs, fstorage_section_id sectionId )
{
    /* Obtain the section */
    fstorage_section *section = fstorage_find_section( fs, sectionId );
    if ( section == 0 )
        return LP_OK; /* No suffixes at all: it's OK i.e. for vietnamese */

    /* Obtain section data */
    char *pt = static_cast<char*>(fstorage_section_get_all_data(section));
    if (pt == 0)
        return LP_ERROR_FS_FAILED;

    /* Fill suffix tree */
    for ( size_t i = 1; *pt != '\0'; pt += strlen(pt) + 1, i++ )
    {
        FlexionsHash::ResultStatus status = suffixes.access( pt, i );
        if ( unlikely(status != FlexionsHash::RESULT_STATUS_SUCCESS) )
            return LP_ERROR_ENOMEM;
                        
        suffixesBackRef.push_back( pt );
        if ( unlikely(suffixesBackRef.no_memory()) )
            return LP_ERROR_ENOMEM;
    }

    return LP_OK;
}

