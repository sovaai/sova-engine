#ifndef _MARKUPBIN_H_
#define _MARKUPBIN_H_

#include "BinaryData.h"

class LPMarkupBin
{
public:
    typedef BinaryFieldRaw< uint64_t > Field;
    
protected:
    // static const uint6_t  totalSizeBits;
    // static const uint6_t  markupTypeBits;
    // static const uint6_t  markupAuxBits;
    // static const uint6_t  markupBegBits;
    // static const uint6_t  markupLenBits;
    
    static const uint64_t offsetTotalSizeBits  = 0;
    static const uint64_t widthTotalSizeBits   = 6;
    static const uint64_t offsetMarkupTypeBits = offsetTotalSizeBits + widthTotalSizeBits;
    static const uint64_t widthMarkupTypeBits  = 6;
    static const uint64_t offsetMarkupAuxBits  = offsetMarkupTypeBits + widthMarkupTypeBits;
    static const uint64_t widthMarkupAuxBits   = 6;
    static const uint64_t offsetMarkupBegBits  = offsetMarkupAuxBits + widthMarkupAuxBits;
    static const uint64_t widthMarkupBegBits   = 6;
    static const uint64_t offsetMarkupLenBits  = offsetMarkupBegBits + widthMarkupBegBits;
    static const uint64_t widthMarkupLenBits   = 6;

    static const uint64_t offsetFixedEnd       = offsetMarkupLenBits + widthMarkupLenBits;
    
protected:
    size_t widthBits( uint64_t value ) const
    {
        size_t width = 0;
        for ( ; value != 0; value >>= 1, width++ );
        return width;
    }

    struct FieldsBits
    {
        uint8_t totalSizeBits;
        uint8_t markupTypeBits;
        uint8_t markupAuxBits;
        uint8_t markupBegBits;
        uint8_t markupLenBits;
        uint8_t itemBits;

        FieldsBits() :
            totalSizeBits( 0 ),
            markupTypeBits( 0 ),
            markupAuxBits( 0 ),
            markupBegBits( 0 ),
            markupLenBits( 0 ),
            itemBits( 0 )
        {
        }
    };
};

#endif /* _MARKUPBIN_H_ */

