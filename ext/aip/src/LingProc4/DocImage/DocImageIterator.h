#ifndef _DOCIMAGEITERATOR_H_
#define _DOCIMAGEITERATOR_H_

#include "DocImage.h"

class DocImageIterator
{
public:
    DocImageIterator() :
        di( 0 ),
        wordPos( -1 ),
        compoundPos( 0 )
    {
    }

    DocImageIterator( const DocImage &_di ) :
        di( &_di ),
        wordPos( 0 ),
        compoundPos( 0 )
    {
    }

    DocImageIterator( const DocImageIterator &it ) :
        di( it.di ),
        wordPos( it.wordPos ),
        compoundPos( it.compoundPos )
    {
    }

    ~DocImageIterator() {}

    DocImageIterator &operator=( const DocImageIterator &src )
    {
        di           = src.di;
        wordPos      = src.wordPos;
        compoundPos  = src.compoundPos;
        return *this;
    }
    
    void Set( size_t _wordPos, size_t _compoundPos = 0 )
    {
        if ( di && _wordPos > di->Size() )
        {
            wordPos     = ( int )di->Size();
            compoundPos = 0;
        }
        else
        {
            wordPos     = ( int )_wordPos;
            compoundPos = ( int )_compoundPos;
        }
    }

    DocImageIterator &operator=( size_t _wordPos )
    {
        Set(_wordPos );
        return *this;
    }

    DocImageIterator &operator++()
    {
        if ( IsEnd() )
            return *this;
        
        if ( IsCompoundPos() )
        {
            /* Advance the compound position */
            compoundPos++;
            if ( static_cast<size_t>(compoundPos) < GetCompoundChainLen() )
                return *this;
        }

        /* Advance the word position */
        wordPos++;
        compoundPos = 0;
        return *this;
    }
    
    DocImageIterator &operator--()
    {
        if ( IsHead() )
            return *this;
        
        if ( IsCompoundPos() )
        {
            /* Step back the compound position */
            compoundPos--;
            if ( compoundPos >= 0 )
                return *this;
        }

        /* Step back the word position */
        wordPos--;
        if ( IsCompoundPos() )
            compoundPos = ( int )GetCompoundChainLen() - 1;
        else
            compoundPos = 0;
        return *this;
    }
    
    DocImageIterator &ForwardWord()
    {
        if ( IsEnd() )
            return *this;

        /* Advance the word position */
        wordPos++;
        compoundPos = 0;
        return *this;
    }    
    
    DocImageIterator &BackWord()
    {
        if ( IsHead() )
            return *this;
        
        /* Step back the word position */
        wordPos--;
        compoundPos = 0;
        return *this;
    }
    
    bool IsEnd() const
    {
        return ( !di || wordPos >= static_cast<int>( di->Size() ) );
    }
    
    bool IsHead() const
    {
        return ( !di || wordPos < 0 );
    }

    bool operator==( const DocImageIterator &it ) const
    {
        return ( wordPos == it.wordPos && compoundPos == it.compoundPos );
    }

    bool operator<( const DocImageIterator &it ) const
    {
        return ( wordPos < it.wordPos || ( wordPos == it.wordPos && compoundPos < it.compoundPos ) );
    }

    bool operator<=( const DocImageIterator &it ) const
    {
        return ( wordPos < it.wordPos || ( wordPos == it.wordPos && compoundPos <= it.compoundPos ) );
    }

    bool operator>( const DocImageIterator &it ) const
    {
        return ( wordPos > it.wordPos || ( wordPos == it.wordPos && compoundPos > it.compoundPos ) );
    }

    bool operator>=( const DocImageIterator &it ) const
    {
        return ( wordPos > it.wordPos || ( wordPos == it.wordPos && compoundPos >= it.compoundPos ) );
    }

    size_t GetWordPos()     const { return static_cast<size_t>( wordPos );     }
    size_t GetCompoundPos() const { return static_cast<size_t>( compoundPos ); }

    size_t GetCompoundChainLen() const
    {
        if ( di && wordPos >= 0 && static_cast<size_t>(wordPos) < di->Size() )
        {
            if ( di->GetCompoundChainsNumber( static_cast<size_t>( wordPos ) ) > 0 )
                return di->GetCompoundChainLength( static_cast<size_t>( wordPos ), 0 );
        }
        return 0;
    }

    bool IsCompoundPos() const
    {
        return GetCompoundChainLen() > 0;
    }

    bool IsLastCompoundPos() const
    {
        return ( static_cast<size_t>(compoundPos) + 1 == GetCompoundChainLen() );
    }
    
    bool IsMiddleCompoundPos() const
    {
        return IsCompoundPos() && !IsLastCompoundPos();
    }
    
protected:
    const DocImage *di;
    
    int wordPos;
    int compoundPos;
};

#endif /* _DOCIMAGEITERATOR_H_ */

