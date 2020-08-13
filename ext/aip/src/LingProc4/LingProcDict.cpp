
#include <_include/CodePages.h>

#include "LingProcDict.h"

LingProcErrors LingProcDict::Init( const char * const *words,
                                    const UCharSet *charset )
{
    Clear();

    if( charset == 0 || !charset->IsValid() || !words )
        return LP_ERROR_EINVAL;

    // fill index
    const char * const *next = words;
    for ( ; *next != 0; next++ )
    {
        if ( unlikely( AddWord( *next ) == 0 ) )
            return LP_ERROR_ENOMEM;
    }

    this->charset = charset;
    return LP_OK;
}

LingProcErrors LingProcDict::Load( fstorage *fs,
                                    fstorage_section_id sec_id,
                                    const UCharSet *charset )
{
    Clear();

    if ( charset == 0 || !charset->IsValid() )
        return LP_ERROR_EINVAL;

    // find section
    fstorage_section* sec = fstorage_find_section( fs, sec_id );
    if ( sec == 0 )
        return LP_ERROR_FS_FAILED;

    // getting section size and pointer
    char *pt = (char *)fstorage_section_get_all_data( sec );
    if( pt == 0 )
        return LP_ERROR_FS_FAILED;

    // fill index
    for ( ; *pt != '\0'; pt += strlen(pt) + 1 )
        if ( unlikely( AddWord( pt ) == 0 ) )
            return LP_ERROR_ENOMEM;

    this->charset = charset;
    return LP_OK;
}

LingProcErrors LingProcDict::Save( fstorage* fs, fstorage_section_id sec_id )
{
    // getting size needed
    size_t detect_size = 0;
    size_t n;
    
    for ( n = 0; n < wordsBackRef.size(); n++)
        detect_size += strlen( wordsBackRef[n] ) + 1;
    detect_size++;

    // find or create section
    fstorage_section *sec = fstorage_find_section( fs, sec_id );
    if ( sec == 0 )
        sec = fstorage_section_create( fs, sec_id );
    if ( sec == 0 )
        return LP_ERROR_FS_FAILED;

    // allocating memory
    if ( fstorage_section_get_size(sec) != detect_size )
    {
        if ( fstorage_section_realloc( sec, detect_size ) != FSTORAGE_OK )
            return LP_ERROR_FS_FAILED;
    }

    // getting pointer
    char *pt = (char *)fstorage_section_get_all_data( sec );
    if ( pt == 0 )
        return LP_ERROR_FS_FAILED;

    // storing data
    size_t wr = 0;
    for ( n = 0; n < wordsBackRef.size(); n++ )
    {
        const char *detect_word = wordsBackRef[ n ];
        size_t size = strlen( detect_word ) + 1;
        if ( wr + size > detect_size )
            return LP_ERROR_EFAULT;
        memcpy( pt + wr, detect_word, size );
        wr += size;
    }
    if ( wr != detect_size - 1 )
        return LP_ERROR_EFAULT;
    *(pt + wr) = '\0';

    return LP_OK;
}

int LingProcDict::AddWord( const char *string )
{
    size_t index = wordsBackRef.size() + 1;
        
    HashKey key( string, static_cast<size_t>(-1) );
    WordsHash::ResultStatus status = wordsTable.access( key, index );
    if ( unlikely(status != WordsHash::RESULT_STATUS_SUCCESS) )
        return 0;
        
    wordsBackRef.push_back( string );
    if ( unlikely(wordsBackRef.no_memory()) )
        return 0;

    return index;
}

