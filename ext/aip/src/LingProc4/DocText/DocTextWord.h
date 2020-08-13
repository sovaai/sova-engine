#ifndef _DOCTEXTWORD_H_
#define _DOCTEXTWORD_H_

#include <_include/_inttype.h>
#include <lib/aptl/avector.h>

#include "../LingProcErrors.h"

template<typename LengthType>
class DocTextWordTmpl
{
private:
    typedef DocTextWordTmpl<LengthType> DocTextWordCustom;

public:
    // initialization
    DocTextWordTmpl() : offset( 0 ), length( 0 ), attrs( 0 ) { }
    DocTextWordTmpl( const DocTextWordCustom &src ) :
        offset( src.offset ),
        length( src.length ),
        attrs( src.attrs ) { }

    DocTextWordCustom &operator=( const DocTextWordCustom &src )
    {
        offset = src.offset;
        length = src.length;
        attrs  = src.attrs;
        return *this;
    }

    LingProcErrors Init( uint32_t offset, LengthType length, uint16_t attrs )
    {
        this->offset = offset;
        this->length = length;
        this->attrs  = attrs;
        
        return LP_OK;
    }
    
    // comparison
    static int Compare( const DocTextWordCustom &r1, const DocTextWordCustom &r2 )
    {
        int rccomp;
        if ( (rccomp = (r1.offset > r2.offset) - (r1.offset < r2.offset)) == 0 )
            rccomp = (r1.length < r2.length) - (r1.length > r2.length);
        return rccomp;
    }
  
    bool operator<( const DocTextWordCustom &src ) const
    {
        return Compare( *this, src ) < 0;
    }
    
    bool operator==( const DocTextWordCustom &src ) const
    {
        return Compare( *this, src ) == 0;
    }

    // data access
    LengthType Length() const { return length; }
    uint16_t   Attrs() const  { return attrs;  }
    uint32_t   Offset() const { return offset; }

protected:
    uint32_t    offset;
    LengthType  length;
    uint16_t    attrs;
};

typedef DocTextWordTmpl<uint16_t> DocTextWord;
typedef DocTextWordTmpl<uint32_t> DocTextWordLong;
typedef avector<DocTextWord>      DocTextWordSet;

#endif /* _DOCTEXTWORD_H_ */

