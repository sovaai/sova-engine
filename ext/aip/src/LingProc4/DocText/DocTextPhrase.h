#ifndef _DOCTEXTPHRASE_H_
#define _DOCTEXTPHRASE_H_

#include "DocTextWord.h"

class DocTextPhrase : public DocTextWordLong
{
public:
    // initialization
    DocTextPhrase() : DocTextWordLong(), startWordIdx( 0 ), wordsCount( 0 ) { }
    DocTextPhrase( const DocTextPhrase &src ) :
        DocTextWordLong( src ),
        startWordIdx( src.startWordIdx ),
        wordsCount( src.wordsCount ) { }

    DocTextPhrase &operator=( const DocTextPhrase &src )
    {
        this->DocTextWordLong::operator=( src );
        startWordIdx = src.startWordIdx;
        wordsCount   = src.wordsCount;
        return *this;
    }

    LingProcErrors Init( uint32_t offset, uint32_t length, uint16_t attrs, size_t startWordIdx, size_t wordsCount )
    {
        LingProcErrors status = DocTextWordLong::Init( offset, length, attrs );
        if ( status != LP_OK )
            return status;
        
        this->startWordIdx = startWordIdx;
        this->wordsCount   = wordsCount;
        
        return LP_OK;
    }
    
    // comparison
    bool operator<( const DocTextPhrase &src ) const
    {
        return DocTextWordLong::operator<( src );
    }
    
    bool operator==( const DocTextPhrase &src ) const
    {
        return ( DocTextWordLong::operator==(src) &&
                 startWordIdx == src.startWordIdx &&
                 wordsCount   == src.wordsCount );
    }

    // data access
    size_t StartWordIndex() const { return startWordIdx; }
    size_t WordsCount()     const { return wordsCount;   }
  
protected:
    size_t startWordIdx;
    size_t wordsCount;
};

typedef avector<DocTextPhrase> DocTextPhraseSet;

#endif /* _DOCTEXTPHRASE_H_ */


