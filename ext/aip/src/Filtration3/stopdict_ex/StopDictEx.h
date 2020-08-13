#ifndef _STOPDICTEX_
#define _STOPDICTEX_

#include <lib/aptl/OpenAddressingDict.h>
#include <lib/fstorage/fstorage.h>
#include <LingProc4/LexID.h>

class LingProc;
class StopDictEx
{
public:
    enum Status
    {
        OK,
        ENUM_STOP,
        ERROR_FS_FAILED,
        ERROR_INVALID_MODE,
        ERROR_LP_FAILED
    };
    
public:
    StopDictEx();
    ~StopDictEx();
        
    /* Save / load */
    Status save( fstorage *fs, fstorage_section_id stopSec ) const;
    Status load( fstorage *fs, fstorage_section_id stopSec );
    
    /* Table filling */
    struct WordsProvider
    {
        virtual StopDictEx::Status getNext( const char *&word, size_t &wordSize ) = 0;
    };

    enum FillMode
    {
        FILL_MODE_ADD,
        FILL_MODE_CLR
    };
    
    Status fill( WordsProvider &provider, LingProc &lp, FillMode mode = FILL_MODE_ADD );
    Status clear();
    Status addWord( const char *word, size_t wordSize, LingProc &lp, FillMode mode = FILL_MODE_ADD );
    Status addWords( const char **words, size_t wordsSize, LingProc &lp, FillMode mode = FILL_MODE_ADD );

    /* Checking */
    bool isStop( LEXID lex ) const;
    
protected:
    struct StopTableHasher
    {
        uint32_t getHash(LEXID key) const
        {
            return static_cast<uint32_t>((static_cast<uint64_t>(key) & 0xFFFFFFFFUL) ^ (static_cast<uint64_t>(key) >> 32));
        }
    };
        
protected:
    typedef OpenAddressingDict< LEXID,
                                uint8_t,
                                256,
                                256,
                                maxSizeTBits,
                                KeyEqualComparator< LEXID >,
                                StopTableHasher > StopTable;

protected:
    StopTable table;
    bool      modeRO;
};

#endif /* _STOPDICTEX_ */

