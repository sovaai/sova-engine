#ifndef _MARKUPSTORAGE_H_
#define _MARKUPSTORAGE_H_

#include "Markup.h"

class LPMarkupStorage : public LPMarkup, protected LPMarkupBin::Field
{
protected:
    const uint64_t *data;
    size_t          dataSize;
    
protected:
    uint64_t get( uint64_t bitsOffset, uint64_t bitsLength ) const
    {
        return Field::get( data, static_cast<size_t>( bitsOffset ), static_cast<size_t>( bitsLength ) );
    }

    uint64_t offsetTotalSize;
    uint64_t offsetItems;
    uint64_t itemBits;
    uint64_t itemTypeShift;
    uint64_t itemAuxShift;
    uint64_t itemBegShift;
    uint64_t itemLenShift;

public:
    LPMarkupStorage() :
        LPMarkup(),
        data( 0 ),
        dataSize( 0 )
    {
    }

    size_t serializeSize() const { return dataSize; }

    void serialize( uint8_t *buffer ) const
    {
        if ( likely(data != 0) )
            memcpy( buffer, data, dataSize );
    }

    LingProcErrors load( const uint8_t *source, size_t sourceSize )
    {
        if ( unlikely((sourceSize << 3) < offsetFixedEnd) )
            return LP_ERROR_NOT_ENOUGH_DATA;

        data     = reinterpret_cast< const uint64_t * >( source );
        dataSize = sourceSize;

        offsetTotalSize = offsetFixedEnd;
        offsetItems     = offsetTotalSize + get( offsetTotalSizeBits, widthTotalSizeBits );

        itemBits        = ( get( offsetMarkupTypeBits, widthMarkupTypeBits ) +
                            get( offsetMarkupAuxBits,  widthMarkupAuxBits ) +
                            get( offsetMarkupBegBits,  widthMarkupBegBits ) +
                            get( offsetMarkupLenBits,  widthMarkupLenBits ) );

        uint64_t totalSize   = ( offsetItems +
                                 ( itemBits *
                                   get( offsetTotalSize, get( offsetTotalSizeBits, widthTotalSizeBits ) ) ) );
        
        if ( unlikely(totalSize > (sourceSize << 3)) )
            return LP_ERROR_NOT_ENOUGH_DATA;

        itemTypeShift         = 0;
        itemAuxShift          = itemTypeShift + get( offsetMarkupTypeBits, widthMarkupTypeBits );
        itemBegShift          = itemAuxShift + get( offsetMarkupAuxBits, widthMarkupAuxBits );
        itemLenShift          = itemBegShift + get( offsetMarkupBegBits, widthMarkupBegBits );
        
        return LP_OK;
    }
    
    int set( const LPMarkupItem &/* mi */ ) { return EINVAL; }

    int set( unsigned short /* type */, unsigned long /* nBeg */, unsigned long /* nLen */ ) { return EINVAL; }

    int append( const LPMarkupItem &/* mi */ ) { return EINVAL; }
    
    int append( unsigned short /* type */, unsigned long /* nBeg */, unsigned long /* nLen */ ) { return EINVAL; }

    int removeAt( size_t /* index */ ) { return EINVAL; }
    
    void Sort() { }

    size_t size() const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( offsetTotalSize, get( offsetTotalSizeBits, widthTotalSizeBits ) ) );
    }

    void clear() { }
    
    const LPMarkupItem operator[]( size_t index ) const
    {
        uint64_t itemOffset = offsetItems + ( itemBits * index );
        return LPMarkupItem( get( itemOffset + itemTypeShift, get( offsetMarkupTypeBits, widthMarkupTypeBits ) ),
                             get( itemOffset + itemBegShift,  get( offsetMarkupBegBits,  widthMarkupBegBits ) ),
                             get( itemOffset + itemLenShift,  get( offsetMarkupLenBits,  widthMarkupLenBits ) ), 
                             get( itemOffset + itemAuxShift,  get( offsetMarkupAuxBits,  widthMarkupAuxBits ) ) );
    }

private:
    // copy prevention
    LPMarkupStorage( const LPMarkup & ) { assert(false); abort(); }
    LPMarkupStorage &operator=( const LPMarkup & ) { assert(false); abort(); return *this; }
};

#endif /* _MARKUPSTORAGE_H_ */

