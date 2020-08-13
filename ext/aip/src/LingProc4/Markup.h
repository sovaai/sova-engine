#ifndef _MARKUP_H_
#define _MARKUP_H_

#include <errno.h>
#include <stdlib.h>
#include <assert.h>

#include <_include/cc_compat.h>
#include <lib/aptl/avector.h>

#include <LingProc4/MarkupTypes.h>

#include "MarkupBin.h"

struct LPMarkupItem : public MarkupItem
{
    LPMarkupItem( unsigned short type = 0,
                  size_t         nBeg = 0,
                  size_t         nLen = 0,
                  unsigned short aux  = 0 )
    {
        m_nBeg = nBeg;
        m_nLen = nLen;
        m_Type = type; 
        m_Aux  = aux;
    }

    LPMarkupItem( const LPMarkupItem &src )
    {
        m_nBeg = src.m_nBeg;
        m_nLen = src.m_nLen;
        m_Type = src.m_Type;
        m_Aux  = src.m_Aux;
    }
  
    // operators
    LPMarkupItem &operator=( const LPMarkupItem &src )
    {
        m_nBeg = src.m_nBeg;
        m_nLen = src.m_nLen;
        m_Type = src.m_Type; 
        m_Aux  = src.m_Aux;
        return *this;
    }
  
    // comparison
    static int compare( const LPMarkupItem &r1, const LPMarkupItem &r2 )
    {
        int rccomp;
        if ( (rccomp = (r1.m_nBeg > r2.m_nBeg) - (r1.m_nBeg < r2.m_nBeg)) == 0 )
        {
            if ( (rccomp = (r1.m_nLen < r2.m_nLen) - (r1.m_nLen > r2.m_nLen)) == 0 )
                rccomp = r1.m_Type - r2.m_Type;
        }
        return rccomp;
    }
  
    bool operator==( const LPMarkupItem &mi ) const
    { return compare( *this, mi ) == 0; }
    bool operator!=( const LPMarkupItem &mi ) const
    { return compare( *this, mi ) != 0; }
    bool operator<( const LPMarkupItem &mi ) const 
    { return compare( *this, mi ) < 0; }
    bool operator>( const LPMarkupItem &mi ) const 
    { return compare( *this, mi ) > 0; }
    bool operator>=( const LPMarkupItem &mi ) const
    { return compare( *this, mi ) >= 0; }
    bool operator<=( const LPMarkupItem &mi ) const
    { return compare( *this, mi ) <= 0; }
    bool operator==(unsigned short type ) const
    { return m_Type == type; }
    bool operator!=( unsigned short type ) const
    { return ! operator==( type ); }
    
    bool has( unsigned long nPos ) const
    {
        return m_nBeg <= nPos && nPos < m_nBeg + m_nLen;
    }
};

class LPMarkup : public LPMarkupBin
{
public:
    virtual int set( const LPMarkupItem &mi ) = 0;

    virtual int set( unsigned short type, unsigned long nBeg, unsigned long nLen ) = 0;

    virtual int append( const LPMarkupItem &mi ) = 0;
    
    virtual int append( unsigned short type, unsigned long nBeg, unsigned long nLen ) = 0;

    virtual int removeAt( size_t index ) = 0;
        
    virtual void Sort() = 0;

    virtual size_t size() const = 0;

    virtual void clear() = 0;

    virtual const LPMarkupItem operator[]( size_t index ) const = 0;

    virtual size_t serializeSize() const = 0;

    virtual void serialize( uint8_t *buffer ) const = 0;

    virtual bool NextFormat( unsigned short type, int &rindex ) const
    {
        int i;
        for ( i = rindex + 1; static_cast<size_t>(i) < size(); i++ )
            if ( operator[]( i ).m_Type == type )
                break;

        if ( static_cast<size_t>(i) >= size() || operator[]( i ).m_Type != type )
            return false;

        rindex = i;
        return true;
    }

    virtual int where_is( unsigned long nPos ) const
    {
        LPMarkupItem mritem( 0, nPos, 0 );

        int sindex;
        for ( sindex = 0; static_cast<size_t>(sindex) < size(); sindex++ )
            if ( operator[]( sindex ) == mritem )
                break;

        if ( sindex > 0 && operator[]( sindex - 1 ).has( nPos ) )
            --sindex;
        
        return sindex;
    }
};

class LPMarkupArray : public LPMarkup
{
protected:
    avector<LPMarkupItem> data;
    
public:
    LPMarkupArray() :
        LPMarkup(),
        data( 64 )
    {
    }

    int set( const LPMarkupItem &mi )
    {
        for ( size_t i = 0; i < data.size(); i++ )
        {
            if ( data[ i ] == mi )
            {
                data[ i ] = mi;
                return 0;
            }
        }

        LPMarkupItem *newItem = data.grow();
        if ( unlikely(newItem == 0) )
            return ENOMEM;
        *newItem = mi;
        return 0;
    }

    int set( unsigned short type, unsigned long nBeg, unsigned long nLen )
    {
        return set( LPMarkupItem(type, nBeg, nLen) );
    }

    int append( const LPMarkupItem &mi )
    {
        data.push_back(mi);
        if ( unlikely(data.no_memory()) )
            return ENOMEM;
        return 0;
    }
    
    int append( unsigned short type, unsigned long nBeg, unsigned long nLen )
    {
        return append( LPMarkupItem( type, nBeg, nLen ) );
    }

    int removeAt( size_t index )
    {
        if ( data.size() == 0 )
            return 0;
        if ( unlikely(index >= data.size()) )
            return EINVAL;
        
        for ( size_t i = index; i < data.size() - 1; i++ )
            data[ i ] = data[ i + 1 ];

        data.pop_back();

        return 0;
    }
    
    static int LPMarkupCompare( const void *a, const void *b )
    {
        return LPMarkupItem::compare( *(reinterpret_cast<const LPMarkupItem *>(a)),
                                      *(reinterpret_cast<const LPMarkupItem *>(b)) );
    }
    
    void Sort()
    {
        qsort( data.get_buffer(),
               data.size(),
               sizeof(LPMarkupItem),
               LPMarkupCompare );
    }

    size_t size() const
    {
        return data.size();
    }

    void clear() { data.clear(); }

    const LPMarkupItem operator[]( size_t index ) const
    {
        return data[ index ];
    }

    FieldsBits prepareSerialize() const
    {
        LPMarkupItem max;

        for ( size_t i = 0; i < data.size(); i++ )
        {
            if ( data[ i ].m_Type > max.m_Type )
                max.m_Type = data[ i ].m_Type;
            if ( data[ i ].m_Aux > max.m_Aux )
                max.m_Aux = data[ i ].m_Aux;
            if ( data[ i ].m_nBeg > max.m_nBeg )
                max.m_nBeg = data[ i ].m_nBeg;
            if ( data[ i ].m_nLen > max.m_nLen )
                max.m_nLen = data[ i ].m_nLen;
        }

        FieldsBits bits;
        bits.totalSizeBits  = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( data.size() ) ) );
        bits.markupTypeBits = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.m_Type ) ) );
        bits.markupAuxBits  = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.m_Aux ) ) );
        bits.markupBegBits  = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.m_nBeg ) ) );
        bits.markupLenBits  = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.m_nLen ) ) );
        bits.itemBits       = bits.markupTypeBits + bits.markupAuxBits + bits.markupBegBits + bits.markupLenBits;

        return bits;
    }
    
    size_t serializeSize() const
    {
        FieldsBits bits = prepareSerialize();

        return static_cast<size_t>( ( ( offsetFixedEnd + 
                                        bits.totalSizeBits +
                                        ( static_cast<uint64_t>( bits.itemBits ) * data.size() ) +
                                        63 /* uint64_t padding */ ) >> 6 ) << 3 );
    }

    void serialize( uint8_t *buffer ) const
    {
        FieldsBits  bits = prepareSerialize();
        uint64_t   *data = reinterpret_cast<uint64_t *>( buffer );

        LPMarkupBin::Field f;

        f.set( data, bits.totalSizeBits, offsetTotalSizeBits, widthTotalSizeBits );
        f.set( data, bits.markupTypeBits, offsetMarkupTypeBits, widthMarkupTypeBits );
        f.set( data, bits.markupAuxBits, offsetMarkupAuxBits, widthMarkupAuxBits );
        f.set( data, bits.markupBegBits, offsetMarkupBegBits, widthMarkupBegBits );
        f.set( data, bits.markupLenBits, offsetMarkupLenBits, widthMarkupLenBits );

        uint64_t offset = offsetFixedEnd;

        f.set( data, this->data.size(), (size_t)offset, bits.totalSizeBits );
        offset += bits.totalSizeBits;
        
        for ( size_t i = 0; i < this->data.size(); i++ )
        {
            f.set( data, this->data[ i ].m_Type, (size_t)offset, bits.markupTypeBits );
            offset += bits.markupTypeBits;
            f.set( data, this->data[ i ].m_Aux,  (size_t)offset, bits.markupAuxBits );
            offset += bits.markupAuxBits;
            f.set( data, this->data[ i ].m_nBeg, (size_t)offset, bits.markupBegBits );
            offset += bits.markupBegBits;
            f.set( data, this->data[ i ].m_nLen, (size_t)offset, bits.markupLenBits );
            offset += bits.markupLenBits;
        }
    }
    
private:
    // copy prevention
    LPMarkupArray( const LPMarkup &) { assert(false); abort(); }
    LPMarkupArray &operator=( const LPMarkup &) { assert(false); abort(); return *this; }
};

class LPMarkupTag : public MarkupTag
{
public:
    LPMarkupTag( unsigned short type = 0, unsigned long nBeg = 0, unsigned short aux = 0 )
    {
        m_nBeg = nBeg;
        m_Type = type;
        m_Aux  = aux;
    }

    LPMarkupTag( const LPMarkupTag &src )
    {
        m_nBeg = src.m_nBeg;
        m_Type = src.m_Type;
        m_Aux  = src.m_Aux;
    }
    
    // operators
    LPMarkupTag &operator=( const LPMarkupTag &src )
    {
        m_nBeg = src.m_nBeg;
        m_Type = src.m_Type;
        m_Aux  = src.m_Aux;
        return *this;
    }
  
    // comparison
    bool operator==( const LPMarkupTag &mt ) const
    {
        return m_nBeg == mt.m_nBeg && m_Type == mt.m_Type;
    }

    bool operator!=( const LPMarkupTag &mt ) const
    {
        return !operator==( mt );
    }

    bool operator<( const LPMarkupTag &mt ) const 
    {
        if ( m_nBeg < mt.m_nBeg )
            return true;
        if ( m_nBeg > mt.m_nBeg )
            return false;
        return m_Type < mt.m_Type;
    }
    
    bool operator>( const LPMarkupTag &mt ) const 
    {
        if ( m_nBeg > mt.m_nBeg )
            return true;
        if ( m_nBeg < mt.m_nBeg )
            return false;
        return m_Type > mt.m_Type;
    }
    
    bool operator>=( const LPMarkupTag &mt ) const
    {
        return !operator<(mt);
    }
    
    bool operator<=( const LPMarkupTag &mt ) const
    {
        return !operator>(mt);
    }

    bool operator==( unsigned short type ) const
    {
        return m_Type == type;
    }
    
    bool operator!=( unsigned short type ) const
    {
        return !operator==(type);
    }
};

class LPMarkupTagged
{
protected:                                      \
    avector<LPMarkupTag> data;
    
public:
    LPMarkupTagged() :
        data( 0x100 )
    {
    }

    int Append( const LPMarkupTag &mt )
    {
        if ( data.size() > 0 )
            data[ data.size() - 1 ].m_Aux &= ~(unsigned short)(MARKUP_LAST);

        LPMarkupTag *newItem = data.grow();
        if ( unlikely(newItem == 0) )
            return ENOMEM;
        *newItem = mt;

        newItem->m_Aux |= MARKUP_LAST;

        return 0;
    }

    int Append( unsigned short type, unsigned long nBeg, unsigned short aux = 0 )
    {
        return Append( LPMarkupTag(type, nBeg, aux) );
    }

    // Compatibility helpers:
    size_t size() const { return data.size(); }

    LPMarkupTag *begin()
    {
        return size() == 0 ? 0 : data.get_buffer();
    }
    
    const LPMarkupTag *begin() const
    {
        return size() == 0 ? 0 : data.get_buffer();
    }
    
    void clear() { data.clear(); }
    
    void resize( int nDim ) { data.resize( static_cast<size_t>(nDim) ); }

private:
    // copy prevention
    LPMarkupTagged( const LPMarkupTagged &) { assert(false); abort(); }
    LPMarkupTagged &operator=( const LPMarkupTagged&) { assert(false); abort(); return *this; }
};


#endif /* _MARKUP_H_ */

