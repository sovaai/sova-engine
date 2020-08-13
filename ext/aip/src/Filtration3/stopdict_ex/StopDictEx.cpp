

#include <_include/cc_compat.h>

#include <LingProc4/LingProc.h>

#include "StopDictEx.h"

StopDictEx::StopDictEx() :
    table(),
    modeRO( false )
{
}

StopDictEx::~StopDictEx()
{
}

bool StopDictEx::isStop( LEXID lex ) const
{
    return ( table.get( lex, 0 ) == StopTable::RESULT_STATUS_SUCCESS ? true : false );
}

StopDictEx::Status StopDictEx::save( fstorage *fs, fstorage_section_id stopSec ) const
{
    if ( modeRO )
        return StopDictEx::OK;
        
    fstorage_section *sec = fstorage_get_section( fs, stopSec );
    if ( unlikely(sec == 0) )
        return StopDictEx::ERROR_FS_FAILED;

    uint8_t *buffer       = const_cast<uint8_t *>( table.getBuffer() );
    size_t   bufferSize   = table.getBufferSize();

    int result = fstorage_section_set_data( sec, buffer, bufferSize );
    if ( unlikely(result != FSTORAGE_OK) )
        return StopDictEx::ERROR_FS_FAILED;
    
    return StopDictEx::OK;
}

StopDictEx::Status StopDictEx::load( fstorage *fs, fstorage_section_id stopSec )
{
    fstorage_section *sec = fstorage_find_section( fs, stopSec );
    
    int result = fstorage_load_section( fs, stopSec );
    if ( unlikely(result != FSTORAGE_OK) )
        return StopDictEx::ERROR_FS_FAILED;

    uint8_t *buffer = reinterpret_cast<uint8_t *>( fstorage_section_get_all_data(sec) );
    if ( unlikely(buffer == 0) )
        return StopDictEx::ERROR_FS_FAILED;
        
    table.setBuffer( buffer );

    modeRO = true;
    
    return StopDictEx::OK;
}

StopDictEx::Status StopDictEx::clear()
{
    if ( unlikely(modeRO) )
        return StopDictEx::ERROR_INVALID_MODE;

    table.clear();
    
    return StopDictEx::OK;
}

StopDictEx::Status StopDictEx::fill( StopDictEx::WordsProvider &provider, LingProc &lp, StopDictEx::FillMode mode )
{
    if ( unlikely(modeRO) )
        return StopDictEx::ERROR_INVALID_MODE;
    
    class LocalFunctor : public LemmatizeResultFunctor
    {
    public:
        LocalFunctor( StopTable &_table, FillMode _mode ) :
            table( _table ),
            mode( _mode )
        { }
        
    public:
	LingProcErrors reset() { return LP_OK; }
        LingProcErrors nextLemma( LEXID lexid, size_t /* compoundNumber */, const LPStrict::Info */* info */ )
        {
            uint8_t flag;
            StopTable::ResultStatus status;
            
            switch ( mode )
            {
            case FILL_MODE_ADD:
                flag = 1;

                status = table.access( lexid, flag );
                break;

            case FILL_MODE_CLR:
                status = table.remove( lexid );
                break;

            default:
                return LP_ERROR_EFAULT;
            }
            
            if ( unlikely(status != StopTable::RESULT_STATUS_SUCCESS) )
                return LP_ERROR_EFAULT;
            
            return LP_OK;
        }

    private:
        StopTable &table;
        FillMode   mode;
    } functor( table, mode );
    
    LingProc::EnabledMorphsSet morphs;
    morphs.
        set( MORPHNO_PUNCT ).
        set( MORPHNO_NUM).
        set( MORPHNO_HIER );
    
    for ( ;; )
    {
        const char *word;
        size_t      wordSize;

        StopDictEx::Status result = provider.getNext( word, wordSize );
        if ( unlikely(result != StopDictEx::OK) )
            return result;

        if ( word == 0 )
            break;
        
        LingProcErrors status = lp.LemmatizeWordWithMorph( functor, morphs, word, wordSize );
        if ( unlikely(status != LP_OK) )
            return StopDictEx::ERROR_LP_FAILED;
    }
        
    return StopDictEx::OK;
}

StopDictEx::Status StopDictEx::addWord( const char *word, size_t wordSize, LingProc &lp, StopDictEx::FillMode mode )
{
    struct LocalProvider: public WordsProvider
    {
        LocalProvider( const char *_word, size_t _wordSize ) : word(_word), wordSize(_wordSize) { }
        
        StopDictEx::Status getNext( const char *&word, size_t &wordSize )
        {
            word     = this->word;
            wordSize = this->wordSize;
            
            this->word     = 0;
            this->wordSize = 0;
            
            return StopDictEx::OK;
        }
        
    private:
        const char *word;
        size_t      wordSize;
    } provider( word, wordSize );

    return fill( provider, lp, mode );
}

StopDictEx::Status StopDictEx::addWords( const char **words, size_t wordsSize, LingProc &lp, StopDictEx::FillMode mode )
{
    struct LocalProvider: public WordsProvider
    {
        LocalProvider( const char **_words, size_t _wordsSize ) : words(_words), wordsSize(_wordsSize), index(0) { }
        
        StopDictEx::Status getNext( const char *&word, size_t &wordSize )
        {
            if ( index >= wordsSize )
            {
                word      = 0;
                wordsSize = 0;
                
                return StopDictEx::OK;
            }
            
            word     = words[index];
            wordSize = static_cast<size_t>(-1);
            
            index++;
            
            return StopDictEx::OK;
        }
        
    private:
        const char **words;
        size_t       wordsSize;
        size_t       index;
    } provider( words, wordsSize );

    return fill( provider, lp, mode );
}
