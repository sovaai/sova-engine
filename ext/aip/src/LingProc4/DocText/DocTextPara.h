#ifndef _DOCTEXTPARA_H_
#define _DOCTEXTPARA_H_

#include "DocTextLine.h"

class DocTextPara : public DocTextLine
{
public:
    // initialization
    DocTextPara() : DocTextLine(), startPhraseIdx( 0 ), phrasesCount( 0 ) { }
    DocTextPara( const DocTextPara &src ) :
        DocTextLine( src ),
        startPhraseIdx( src.startPhraseIdx ),
        phrasesCount( src.phrasesCount ) { }

    DocTextPara &operator=( const DocTextPara &src )
    {
        this->DocTextLine::operator=( src );
        startPhraseIdx = src.startPhraseIdx;
        phrasesCount   = src.phrasesCount;
        return *this;
    }

    LingProcErrors Init( uint32_t   offset,
                         uint32_t   length,
                         uint16_t   attrs,
                         size_t     startWordIdx,
                         size_t     wordsCount,
                         size_t     startPhraseIdx,
                         size_t     phrasesCount )
    {
        LingProcErrors status = DocTextLine::Init( offset, length, attrs, startWordIdx, wordsCount );
        if ( status != LP_OK )
            return LP_OK;
        
        this->startPhraseIdx = startPhraseIdx;
        this->phrasesCount   = phrasesCount;
        
        return LP_OK;
    }
    
    // comparison
    bool operator<( const DocTextPara &src ) const { return DocTextLine::operator<( src ); }
    bool operator==( const DocTextPara &src ) const
    {
        return ( DocTextLine::operator==( src ) &&
                 startPhraseIdx == src.startPhraseIdx &&
                 phrasesCount   == src.phrasesCount );
    }

    // data access
    size_t StartPhraseIndex() const { return startPhraseIdx; }
    size_t PhrasesCount()     const { return phrasesCount;   }
  
protected:
    size_t startPhraseIdx;
    size_t phrasesCount;
};

typedef avector<DocTextPara> DocTextParaSet;

#endif /* _DOCTEXTPARA_H_ */

