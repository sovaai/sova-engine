
#include <_include/cc_compat.h>

#include "PhraseBreakerDict.h"

#include "../data/phrases_tables.h"

const size_t staticBufferSize = lpMaxWordLength * 4 + 1;

class PhraseBreakNormalizerFunctor : public WordNormalizerFunctor
{
public:
    PhraseBreakNormalizerFunctor( const PhraseBreakerDict *_dict = 0 ) :
        foundFlag( false ),
        callsCount( 0 ),
        dict( _dict ) { }
    
    void Reset()
    {
        foundFlag  = false;
        callsCount = 0;
    }
    
    bool Found()      const { return foundFlag;  }
    int  CallsCount() const { return callsCount; }

    void SetDict( const PhraseBreakerDict *dict )
    {
        this->dict = dict;
    }
    
    char *getResultBuffer( size_t requestSize );
    WordNormalizerResult nextResult( const char *result, size_t resultSize, WordNormalizerCaps caps );

protected:
    bool                     foundFlag;
    size_t                   callsCount;
    const PhraseBreakerDict *dict;
    char buf[ staticBufferSize ]; // using static allocation
};

char *PhraseBreakNormalizerFunctor::getResultBuffer( size_t requestSize )
{
    if ( unlikely(requestSize > staticBufferSize) )
        return 0;
    
    return buf;
}

WordNormalizerResult PhraseBreakNormalizerFunctor::nextResult( const char *result, size_t resultSize, WordNormalizerCaps /* caps */ )
{
    if ( !foundFlag )
        foundFlag = dict->ExactMatch( result, resultSize ) != 0;
    callsCount++;
    
    return NORMALIZE_SUCCESS;
}

class LoadDictNormalizerFunctor : public WordNormalizerFunctor
{
public:
    LoadDictNormalizerFunctor() : WordNormalizerFunctor(), len( 0 ) { }
    
    char   *GetBuf()       { return buf; }
    size_t  GetLen() const { return len; }
    void    Reset()        { len=0;      }

    char *getResultBuffer( size_t requestSize );
    WordNormalizerResult nextResult( const char *result, size_t resultSize, WordNormalizerCaps caps );

private:
    size_t len;
    char   buf[ staticBufferSize ];
};

char *LoadDictNormalizerFunctor::getResultBuffer( size_t requestSize )
{
    if ( unlikely(requestSize > staticBufferSize - 1) )
        return 0;
    
    return buf;
}

WordNormalizerResult LoadDictNormalizerFunctor::nextResult( const char* /* result */, size_t resultSize, WordNormalizerCaps /* caps */ )
{
    len               = resultSize;
    buf[ resultSize ] = '\0'; // assume \0 at the end
    
    return NORMALIZE_SUCCESS;
}

void PhraseBreakerDict::Clear()
{
    LingProcDict::Clear();
    normalizedStorage.clear();
}

LingProcErrors PhraseBreakerDict::InitEmpty( const UCharSet *charset)
{
    char *ptr = 0;
    return LingProcDict::Init( &ptr, charset );
}

LingProcErrors PhraseBreakerDict::Init( PhraseBreakerDict::Type  dictType,
                                        WordNormalizer          *normalizer,
                                        const UCharSet          *charset )
{
    if ( unlikely(charset == 0 || !charset->IsValid()) )
        return LP_ERROR_EINVAL;
    
    LingProcErrors status = InitEmpty( charset );
    if ( unlikely(status != LP_OK) )
        return status;
    
    const char **dicts[6];
    const char **dictDeu;
    int          dictsCount;
    
    //choose dictionary tables
    switch ( dictType )
    {
    case PhraseBreakerDict::PBDICT_ABBR:
        dicts[0]   = phrases_table_abbr_deu;
        dicts[1]   = phrases_table_abbr_eng;
        dicts[2]   = phrases_table_abbr_esp;
        dicts[3]   = phrases_table_abbr_fra;
        dicts[4]   = phrases_table_abbr_rus;
        dicts[5]   = phrases_table_abbr_vie;
        dictDeu    = phrases_table_abbr_deu;
        dictsCount = 6;
        break;
        
    case PhraseBreakerDict::PBDICT_STOPWORD:
        dicts[0]   = phrases_table_stopwords_deu;
        dicts[1]   = phrases_table_stopwords_eng;
        dicts[2]   = phrases_table_stopwords_esp;
        dicts[3]   = phrases_table_stopwords_fra;
        dicts[4]   = phrases_table_stopwords_rus;
        dicts[5]   = phrases_table_stopwords_vie;
        dictDeu    = phrases_table_stopwords_deu;
        dictsCount = 6;
        break;
        
    case PhraseBreakerDict::PBDICT_COMPEND:
        dicts[0]   = phrases_table_compends;
        dictDeu    = 0;
        dictsCount = 1;
        break;
        
    default:
        Clear();
        return LP_ERROR_EINVAL;
    }
    
    for ( int i = 0; i < dictsCount; i++ )
    {
        status = AppendDict( dicts[i], normalizer, normalizer->getDefaultFlags(), charset, true );
        if ( unlikely(status != LP_OK) )
            return status;
    }
    
    // additionally expand ligatures in German language (if needed)
    if ( dictDeu != 0 )
    {
        status = AppendDict( dictDeu,
                             normalizer,
                             normalizer->getDefaultFlags() | MORPH_FLAG_EXPAND_LIGATURES,
                             charset,
                             false );
        if ( unlikely(status != LP_OK) ) //here we may place false because the dictionary was already added without expanding ligatures
            return status;
    }
    
    return LP_OK;
}

LingProcErrors PhraseBreakerDict::AppendDict( const char * const *words,
                                              WordNormalizer     *normalizer,
                                              uint32_t            normalizerFlags,
                                              const UCharSet     *charset,
                                              bool                savemem )
{
    if ( words == 0 ) // consider empty dictionary in this case
        return LP_OK;
    
    LoadDictNormalizerFunctor functor;
    WordNormalizerResult      status;

    size_t  len;
    char   *buf = functor.GetBuf();
    
    for ( ; *words != 0; words++ )
    {
        functor.Reset(); // set len to zero to determine if the functor was called        
        status = normalizer->normalizeWord( functor, normalizerFlags, *words ); // try to normalize
        len    = functor.GetLen();

        // if normalizer has not been called, we perform strlwr
        if ( status != NORMALIZE_SUCCESS || len == 0 )
        {
            len = strlen( *words );
            if ( unlikely(len > staticBufferSize - 1) )  // if word is too long we have error
            {
                Clear();
                return LP_ERROR_INVALID_FORMAT;
            }

            size_t bufOffset = 0;
            charset->strlwr( *words, len, buf, bufOffset, staticBufferSize );
            buf[ bufOffset ] = '\0';
        }

        // if savemem == true we try to get rid of copying
        // in case where all dictionaries are not deallocated while using LingProc, if word already normalized, we do not allocate additional memory
        if ( savemem && memcmp( buf, *words, len ) == 0 )
        {
            if ( ExactMatch( *words, len ) == 0 )
                if ( unlikely(AddWord( *words ) == 0) )
                    return LP_ERROR_EFAULT;
        }
        else
        {
            if ( ExactMatch( buf, len ) == 0 )
            {
                char *buf1 = normalizedStorage.requestBuffer( len );
                if ( unlikely(normalizedStorage.hasErrorOccurred()) )
                    return LP_ERROR_ENOMEM;
                memcpy( buf1, buf, len );
                normalizedStorage.commitString( len );
               
            }
        }
    }
    //add internally stored words to hash. we cannot do it in previous loop due to storage buffer resize
    for(size_t i=0; i<normalizedStorage.size(); ++i){
        if ( unlikely(AddWord( normalizedStorage[i] ) == 0) )
            return LP_ERROR_EFAULT;
    }
    return LP_OK;
}

int PhraseBreakerDict::CheckWord( const char* word, size_t len ) const
/* returns 0 - not found, 1 - found, enum LingProcErrors - on errors
   word checking regardless letter case*/
{
    if ( unlikely(charset == 0) )
        return LP_ERROR_INVALID_MODE;
    if ( unlikely(len == 0) )
        return 0;

    size_t tmpOffset = 0;
    char   tmp[ lpMaxWordLength ];
    
    charset->strlwr( word, len, tmp, tmpOffset, sizeof(tmp) );

    if ( ExactMatch( tmp, len ) != 0 )
        return 1;
    return 0;
}

LingProcErrors PhraseBreakerData::Init( const UCharSet *charset )
{
    if ( normalizer == 0 )
    {
        normalizer = WordNormalizer::factory( charset->GetLang(), MORPH_TYPE_ALL );
        if ( unlikely(normalizer == 0) )
            return LP_ERROR_EFAULT;
    }
    
    WordNormalizerResult wstatus = normalizer->create( charset,
                                                       ( MORPH_FLAG_CASE_INSENSITIVE |
                                                         MORPH_FLAG_LWR_ACCENT       |
                                                         MORPH_FLAG_LWR_PUNCT        |
                                                         MORPH_FLAG_REM_IGNORE ) );
    if ( unlikely(wstatus != NORMALIZE_SUCCESS) )
        return LP_ERROR_EFAULT;
    
    LingProcErrors status;

    // abbreviations dictionary
    status = abbrs.Init( PhraseBreakerDict::PBDICT_ABBR, normalizer, charset );
    if ( unlikely(status != LP_OK) )
        return status;
    
    // right parts dictionary
    status = compends.Init( PhraseBreakerDict::PBDICT_COMPEND, normalizer, charset );
    if ( unlikely(status != LP_OK) )
        return status;

    // stopwords dictionary
    status = stopwords.Init( PhraseBreakerDict::PBDICT_STOPWORD, normalizer, charset );
    if ( unlikely(status != LP_OK) )
        return status;
    
    return LP_OK;
}

LingProcErrors PhraseBreakerData::Load( fstorage* fs, fstorage_section_id sec_id, const UCharSet *charset )
{
    if ( normalizer == 0 )
    {
        normalizer = WordNormalizer::factory( charset->GetLang(), MORPH_TYPE_ALL );
        if ( unlikely(normalizer == 0) )
            return LP_ERROR_EFAULT;
    }
    
    WordNormalizerResult wstatus = normalizer->create( charset,
                                                       ( MORPH_FLAG_CASE_INSENSITIVE |
                                                         MORPH_FLAG_LWR_ACCENT       |
                                                         MORPH_FLAG_LWR_PUNCT        |
                                                         MORPH_FLAG_REM_IGNORE ) );
    if ( unlikely(wstatus != NORMALIZE_SUCCESS) )
        return LP_ERROR_EFAULT;

    LingProcErrors status;
    
    // abbreviations dictionary
    status = abbrs.Load( fs, sec_id, charset );
    if ( unlikely(status != LP_OK) )
        return status;
    
    // right parts dictionary
    status = compends.Load( fs, sec_id + 1, charset );
    if ( unlikely(status != LP_OK) )
        return status;

    // stopwords dictionary
    status = stopwords.Load( fs, sec_id + 2, charset );
    if ( unlikely(status != LP_OK) )
        return status;

    return LP_OK;
}

LingProcErrors PhraseBreakerData::Save( fstorage *fs, fstorage_section_id sec_id )
{
    LingProcErrors status;
    
    // abbreviations dictionary
    status = abbrs.Save( fs, sec_id );
    if ( unlikely(status != LP_OK) )
        return status;
    
    // right parts dictionary
    status = compends.Save( fs, sec_id + 1 );
    if ( unlikely(status != LP_OK) )
        return status;

    // stopwords dictionary
    status = stopwords.Save( fs, sec_id + 2 );
    if ( unlikely(status != LP_OK) )
        return status;

    return LP_OK;
}

/* returns 0 - not found, 1 - found, enum LingProcErrors - on errors */
int PhraseBreakerData::CheckWordNormalized( const DocText &docText, const DocTextWord &word, const PhraseBreakerDict &dict) const
{
    PhraseBreakNormalizerFunctor pbfunctor( &dict );

    normalizer->normalizeWord( pbfunctor, docText.WordText( word ), word.Length() );

    if ( pbfunctor.CallsCount() !=0 )
        return pbfunctor.Found() ? 1 : 0;
    return dict.CheckWord( docText.WordText( word ), word.Length() );
}


