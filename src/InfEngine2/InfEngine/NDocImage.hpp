#ifndef  __NDocImage_hpp__
#define __NDocImage_hpp__

#include <LingProc4/DocImage/DocImageIterator.h>
#include <Filtration3/terms/TermsDocImageIterator.h>
#include <lib/aptl/avector.h>
#include <InfEngine2/_Include/Errors.h>

class NDocImage
{
public:
    class Iterator;
    struct TermCoords;
    
    NDocImage()
    { }
    
    InfEngineErrors Init( const TermsDocImageIterator & aIterator );
    
    
    inline const Iterator & operator[]( unsigned int aPos ) const
    { return vImage[aPos]; }
        
    inline const Iterator & First() const
    { return vImage[0]; }
    
    inline const Iterator & Last() const
    { return vImage[vImage.size()-1]; }
    
    
    inline unsigned int Size( ) const
    { return vImage.size(); }
    
    
    inline void Reset()
    { vImage.clear(); }     
    
protected:
    avector<Iterator> vImage;
};


class NDocImage::Iterator: public TermsDocImageIterator
{
public:
    Iterator():
        vPos( 0 )
    { }

    Iterator( const TermsDocImageIterator & aIt, unsigned int aPos ):
        TermsDocImageIterator( aIt ),
        vPos( aPos )
    { }
    
    inline Iterator & operator++()
    {
        TermsDocImageIterator::operator++();
        ++vPos;
        return *this;
    }
    
    inline Iterator & operator--()
    {
        TermsDocImageIterator::operator--();
        --vPos;
        return *this;
    }
    
    
    inline bool operator==( const TermsDocImageIterator & aDocImageIterator ) const
    { 
        return TermsDocImageIterator::operator==( aDocImageIterator );
    }
    
    inline bool operator!=( const TermsDocImageIterator & aDocImageIterator ) const
    { 
        return !TermsDocImageIterator::operator==( aDocImageIterator );
    }
    
    
    inline bool operator==( const Iterator & aIterator ) const
    { 
        return vPos == aIterator.vPos;
    }
    
    inline bool operator!=( const Iterator & aIterator ) const
    { 
        return !operator==(aIterator);
    }
    
    inline bool operator<( const Iterator & aIterator ) const
    { 
        return vPos < aIterator.vPos; 
    }
    
    inline bool operator>( const Iterator & aIterator ) const
    { 
        return !operator<(aIterator) && operator!=(aIterator); 
    }
    
    inline bool operator<=( const Iterator & aIterator ) const
    { 
        return operator<(aIterator) || operator==(aIterator); 
    }
    
    inline bool operator>=( const Iterator & aIterator ) const
    { 
        return operator>(aIterator) || operator==(aIterator); 
    }
    
	inline unsigned int GetPos() const
    {
        return vPos;
    }
       
protected:
    
    unsigned int vPos;

};

struct NDocImage::TermCoords
{
    NDocImage::Iterator vFirstPos;
    NDocImage::Iterator vLastPos;
};

#endif // __NDocImage_hpp__
