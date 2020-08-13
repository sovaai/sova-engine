#ifndef _LINGPROCDICT_H_
#define _LINGPROCDICT_H_

#include <stdlib.h>
#include <assert.h>

#include <_include/language.h>
#include <lib/fstorage/fstorage.h>
#include <lib/aptl/avector.h>
#include <lib/aptl/OpenAddressingDict.h>

#include "Constants.h"
#include "LingProcErrors.h"
#include "UCharSet/UCharSet.h"

class LingProcDict
{
public:
    LingProcDict() :
        wordsTable(),
        wordsBackRef(),
        charset( 0 ) { }

    void Clear()
    {
        wordsTable.clear();
        wordsBackRef.clear();
        charset = 0;
    }

    LingProcErrors Load( fstorage* fs, fstorage_section_id sec_id, const UCharSet *charset );
    LingProcErrors Save( fstorage* fs, fstorage_section_id sec_id );
    
    int ExactMatch( const char *string, size_t len = static_cast<size_t>(-1) ) const
    {
        if ( len != static_cast<size_t>(-1) && len > lpMaxWordLength )
            len = lpMaxWordLength;
        
        HashKey  key( string, len );
        size_t  *id = 0;
        if ( wordsTable.get( key, &id ) == WordsHash::RESULT_STATUS_SUCCESS )
            return static_cast<int>( *id );
        return 0;
    }
    
    int AddWord( const char *string );
    void Drop() { Clear(); }
    
    enum { MAX_DICT_WORD_LEN = 1023 };

protected:
    LingProcErrors Init( const char * const    *words,
                         const UCharSet        *charset );

protected:
    // copy prevention
    LingProcDict( const LingProcDict & ) { assert(false); abort(); }
    LingProcDict &operator=( const LingProcDict & ) { assert(false); abort(); return *this; }

private:
    struct HashKey
    {
        HashKey() : string(0), length(0) { }
        HashKey( const char *_string, size_t _length ) : string(_string), length(_length) { }
        
        const char *string;
        size_t      length;
    };
    
private:
    static bool isLast( HashKey &key, size_t i )
    {
        return ( key.length == static_cast<size_t>(-1) ? key.string[i] == '\0' : i >= key.length );
    }
    
    struct WordsHasher
    {
        uint32_t getHash(HashKey key) const
        {
            const char *str = key.string;
            /* A standard djb2 hash function for ASCII strings */
            uint32_t hash = 5381;
            for ( size_t i = 0; !LingProcDict::isLast( key, i ); i++ )
                hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
            return hash;
        }
    };
    
    struct WordsComparator
    {
        bool areKeysEqual(HashKey firstKey, HashKey secondKey) const
        {
            for ( size_t i = 0; ; i++ )
            {
                bool isLastFirst =  LingProcDict::isLast(firstKey, i);
                bool isLastSecond = LingProcDict::isLast(secondKey, i);
                
                if ( isLastFirst && isLastSecond )
                    return true;
                if ( isLastFirst || isLastSecond )
                    return false;

                if ( firstKey.string[i] != secondKey.string[i] )
                    return false;
            }
        }
    };
    
private:
    typedef OpenAddressingDict< HashKey, size_t, 2048, 0, 1, WordsComparator, WordsHasher > WordsHash;
    typedef avector<const char *> WordsBackRef;
    
protected:
    WordsHash       wordsTable;
    WordsBackRef    wordsBackRef;
    const UCharSet *charset;
};

#endif /* _LINGPROCDICT_H_ */
