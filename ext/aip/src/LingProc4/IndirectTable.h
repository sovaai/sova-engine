#ifndef _INDIRECTTABLE_H_
#define _INDIRECTTABLE_H_

#include <_include/_inttype.h>
#include <_include/_string.h>
#include <lib/aptl/avector.h>
#include <lib/aptl/Functor.h>

class IndirectTable
{
protected:
    static const size_t base = 0x100;

public:
    typedef Functor<uint32_t> Enumerator;

public:
    IndirectTable(const IndirectTable& another)
    {
        const char  *rawData;
        size_t      dataSize;
        another.serialize( rawData, dataSize );
        deserializeCopy( rawData, dataSize );
    }

    IndirectTable() : memBuffer(), fixBuffer( 0 ), fixBufferSize( 0 ) { }

    bool noMemory() { return memBuffer.no_memory(); }

    void setValue( uint32_t chr, uint32_t flags )
    {
        if ( fixBuffer != 0 ) // no setValue support in fixed buffer mode
            return;

        if ( memBuffer.size() == 0 )
        {
            memBuffer.resize( base + 256 );
            if ( memBuffer.no_memory() )
                return;
        }

        uint32_t currentBase = base;

        for ( int shift = 24; shift >= 8; shift -= 8 )
        {
            uint32_t offset = ((chr >> shift) & 0xFF) + currentBase;

            if ( memBuffer[ offset ] == 0 )
            {
                memBuffer[ offset ] = memBuffer.size();
                memBuffer.resize( memBuffer.size() + 256 );
                if ( memBuffer.no_memory() )
                    return;
            }

            currentBase = memBuffer[ offset ];
        }

        memBuffer[ currentBase + (chr & 0xFF) ] = flags + 1;
    }

    uint32_t getValue( uint32_t chr ) const
    {
        const char     *rawData;
        size_t          dataSize;
        serialize( rawData, dataSize );
        const uint32_t *data = reinterpret_cast< const uint32_t * >( rawData );

        if ( dataSize == 0 )
            return 0;

        uint32_t value = data[ data[ data[ data[ base + ((chr >> 24) & 0xFF) ] +
                                           ((chr >> 16) & 0xFF) ] +
                                     ((chr >> 8) & 0xFF) ] +
                               (chr & 0xFF) ];

        return ( value == 0 ? 0 : value - 1 );
    }

    void clearValue( uint32_t chr )
    {
        if ( fixBuffer != 0 ) // no setValue support in fixed buffer mode
            return;

        if ( getValue( chr ) != 0 )
        {
            uint32_t offset =  memBuffer[ memBuffer[ memBuffer[ base + ((chr >> 24) & 0xFF) ] +
                                                     ((chr >> 16) & 0xFF) ] +
                                          ((chr >> 8) & 0xFF) ];
            memBuffer[ offset + (chr & 0xFF) ] = 0;
        }
    }

    int enumerate( Enumerator &enumerator ) const
    {
        const char     *rawData;
        size_t          dataSize;
        serialize( rawData, dataSize );
        const uint32_t *data = reinterpret_cast< const uint32_t * >( rawData );

        for ( uint32_t page_idx_00 = 0; page_idx_00 < 256; page_idx_00++ )
        {
            uint32_t page_oft_00 = data[ base + page_idx_00 ];
            if ( page_oft_00 == 0 )
                continue;

            for ( uint32_t page_idx_01 = 0; page_idx_01 < 256; page_idx_01++ )
            {
                uint32_t page_oft_01 = data[ page_oft_00 + page_idx_01 ];
                if ( page_oft_01 == 0 )
                    continue;

                for ( uint32_t page_idx_02 = 0; page_idx_02 < 256; page_idx_02++ )
                {
                    uint32_t page_oft_02 = data[ page_oft_01 + page_idx_02 ];
                    if ( page_oft_02 == 0 )
                        continue;

                    for ( uint32_t page_idx_03 = 0; page_idx_03 < 256; page_idx_03++ )
                    {
                        uint32_t value = data[ page_oft_02 + page_idx_03 ];
                        if ( value == 0 )
                            continue;

                        int invokeResult = enumerator.apply( (page_idx_00 << 24) | (page_idx_01 << 16) | (page_idx_02 << 8) | page_idx_03 );
                        if ( invokeResult != 0 )
                            return invokeResult;
                    }
                }
            }
        }

        return 0;
    }

    void serialize( const char *&rawBuffer, size_t &rawBufferSize ) const
    {
        if ( fixBuffer == 0 )
        {
            rawBuffer     = reinterpret_cast<const char *>( memBuffer.get_buffer() );
            rawBufferSize = memBuffer.size() * sizeof( uint32_t );
        }
        else
        {
            rawBuffer     = reinterpret_cast<const char *>( fixBuffer );
            rawBufferSize = fixBufferSize * sizeof( uint32_t );
        }
    }

    void deserializeCopy( const char *rawBuffer, size_t rawBufferSize )
    {
        memBuffer.resize( rawBufferSize / sizeof( uint32_t ) );
        if ( memBuffer.no_memory() )
            return;
        memcpy( memBuffer.get_buffer(), rawBuffer, rawBufferSize );
        fixBuffer     = 0;
        fixBufferSize = 0;
    }

    void deserializeFixed( const char *rawBuffer, size_t rawBufferSize )
    {
        memBuffer.clear();
        fixBuffer     = reinterpret_cast<const uint32_t *>( rawBuffer );
        fixBufferSize = rawBufferSize / sizeof( uint32_t );
    }

protected:
    avector<uint32_t>  memBuffer;
    const uint32_t    *fixBuffer;
    size_t             fixBufferSize;
};

#endif /* _INDIRECTTABLE_H_ */

