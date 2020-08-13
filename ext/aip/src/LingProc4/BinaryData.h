/**
 * @file   BinaryData.h
 * @author swizard <me@swizard.info>
 * @date   Wed Apr 21 03:27:10 2010
 * 
 * @brief  Short binary data container and binary utilities (used for serializing).
 * 
 * 
 */
#ifndef _BINARYDATA_H_
#define _BINARYDATA_H_

#include <_include/_inttype.h>

#include <assert.h>

#include <lib/aptl/avector.h>

template< uint64_t x, int bit = 63 >
struct BitsCounter
{
    static const unsigned n = ( (x >> bit) & 1 ) + BitsCounter< x, bit - 1 >::n;
};

template< uint64_t x >
struct BitsCounter< x, -1 >
{
    static const unsigned n = 0;
};

template< uint64_t x >
struct BitsMaskPower
{
    static const unsigned n = 1 + BitsMaskPower< (x >> 1) >::n;
};

template<>
struct BitsMaskPower< 0 >
{
    static const unsigned n = 0;
};

template< typename array_element_t = uint64_t >
struct BinaryFieldRaw
{
    static const size_t bits          = sizeof( array_element_t ) << 3;
    static const size_t shift         = BitsCounter< (bits - 1) >::n;
    static const array_element_t mask = static_cast< array_element_t >( -1 );
    
    array_element_t maskRsh( array_element_t len ) const
    {
        if ( len == 0 )
            return 0;
        return mask >> (bits - len);
    }
    
    array_element_t get( const array_element_t *field, size_t bitsOffset, size_t bitsLength ) const
    {
        array_element_t value = 0;
        array_element_t oshft = bitsOffset % bits;
        array_element_t llen  = bits - oshft;
        if ( llen > bitsLength )
            llen = bitsLength;
        array_element_t rlen  = bitsLength - llen;

        value |=  field[ bitsOffset >> shift ] & (maskRsh(llen) << oshft);
        value >>= oshft;
        if ( rlen != 0 )
            value |=  (field[ (bitsOffset + bitsLength) >> shift ] & maskRsh(rlen)) << llen;
        
        return value;
    }

    void set( array_element_t *field, array_element_t value, size_t bitsOffset, size_t bitsLength )
    {
        array_element_t oshft = bitsOffset % bits;
        array_element_t llen  = bits - oshft;
        if ( llen > bitsLength )
            llen = bitsLength;
        array_element_t rlen  = bitsLength - llen;

        field[ bitsOffset >> shift ]                &= ~(maskRsh(llen) << oshft);
        field[ (bitsOffset + bitsLength) >> shift ] &= ((mask >> rlen) << rlen);

        field[ bitsOffset >> shift ]                |= (value & maskRsh(llen)) << oshft;
        field[ (bitsOffset + bitsLength) >> shift ] |= (value >> llen) & maskRsh(rlen);
    }
};

template< size_t length, typename array_element_t = uint64_t >
struct BinaryField : public BinaryFieldRaw< array_element_t >
{
    static const size_t elements = ( length + BinaryFieldRaw< array_element_t >::bits - 1 ) >> BinaryFieldRaw< array_element_t >::shift;
    static const size_t octets   = elements * sizeof( array_element_t );
    
    array_element_t get( const array_element_t *field, size_t bitsOffset, size_t bitsLength ) const
    {
        assert( bitsLength <= BinaryFieldRaw< array_element_t >::bits );
        assert( bitsOffset + bitsLength <= length );

        return BinaryFieldRaw< array_element_t >::get( field, bitsOffset, bitsLength );
    }

    void set( array_element_t *field, array_element_t value, size_t bitsOffset, size_t bitsLength )
    {
        assert( bitsLength <= BinaryFieldRaw< array_element_t >::bits );
        assert( bitsOffset + bitsLength <= length );

        BinaryFieldRaw< array_element_t >::set( field, value, bitsOffset, bitsLength );
    }
};

struct BinaryData : BinaryField< 64, uint64_t >
{
    uint64_t data;
    
    BinaryData() : data( 0 ) { }
    BinaryData( uint64_t _data )
    {
        data = _data;
    }

    uint64_t get( size_t bitsOffset, size_t bitsLength ) const
    {
        return BinaryField< 64, uint64_t >::get( &data, bitsOffset, bitsLength );
    }

    void set( uint64_t value, size_t bitsOffset, size_t bitsLength )
    {
        BinaryField< 64, uint64_t >::set( &data, value, bitsOffset, bitsLength );
    }
    
    BinaryData &operator=( const BinaryData &value )
    {
        data = value.data;
        return *this;
    }
};

template< size_t length, typename array_element_t = uint64_t >
struct BinarySetBaseConst : public BinaryField< length >
{
    typedef BinaryField< length, array_element_t > Field;
    typedef array_element_t                        Element;
    
    array_element_t data[ Field::elements ];
    
    BinarySetBaseConst() : Field() { }

    array_element_t       *getData()       { return data; }
    const array_element_t *getData() const { return data; }
    size_t setSize() const { return length; }
    size_t elementsCount() const { return Field::elements; }
};

template< typename Base, typename Front >
struct BinarySetImpl : public Base
{
    BinarySetImpl() : Base() { clear(); }

    Front &clear()
    {
        for ( size_t i = 0; i < Base::elementsCount(); i++ )
            *( Base::getData() + i ) = 0;

        return static_cast<Front &>(*this);
    }
    
    Front &setAll()
    {
        for ( size_t i = 0; i < Base::elementsCount(); i++ )
            *( Base::getData() + i ) = Base::Field::mask;
        
        return static_cast<Front &>(*this);
    }
    
    Front &unset( size_t index )
    {
        if ( index < Base::setSize() )
            Base::Field::set( Base::getData(), 0, index, 1 );

        return static_cast<Front &>(*this);
    }
    
    Front &set( size_t index )
    {
        if ( index < Base::setSize() )
            Base::Field::set( Base::getData(), 1, index, 1 );
        
        return static_cast<Front &>(*this);
    }
    
    bool check( size_t index ) const
    {
        return ( index < Base::setSize() ? ( Base::Field::get( Base::getData(), index, 1 ) == 0 ? false : true ) : false );
    }

    Front &operator=( const Front &value )
    {
        for ( size_t i = 0; i < Base::elementsCount(); i++ )
            *( Base::getData() + i ) = *( value.Base::getData() + i );
        
        return static_cast<Front &>(*this);
    }
};

template< size_t length, typename array_element_t = uint64_t >
struct BinarySet : public BinarySetImpl< BinarySetBaseConst< length, array_element_t >, BinarySet< length, array_element_t > >
{
};


template< typename array_element_t = uint64_t >
struct BinarySetBaseRuntime : protected BinaryFieldRaw< array_element_t >,
                              protected avector< array_element_t >
{
    typedef BinaryFieldRaw< array_element_t > Field;
    typedef avector< array_element_t >        Vector;
    typedef array_element_t                   Element;

    size_t length;
    
    BinarySetBaseRuntime() : Field(), Vector(), length( 0 ) { }

    array_element_t       *getData()       { return Vector::get_buffer(); }
    const array_element_t *getData() const { return Vector::get_buffer(); }
    size_t setSize() const { return length; }
    size_t elementsCount() const { return ( length + Field::bits - 1 ) >> Field::shift; }

    bool no_memory() const { return Vector::no_memory(); }

    void resize( size_t newLength )
    {
        length = newLength;
        Vector::resize( elementsCount() );
    }
};

template< typename array_element_t = uint64_t >
struct BinarySetRuntime : public BinarySetImpl< BinarySetBaseRuntime< array_element_t >, BinarySetRuntime< array_element_t > >
{
};


#endif /* _BINARYDATA_H_ */

