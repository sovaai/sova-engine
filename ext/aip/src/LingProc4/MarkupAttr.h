#ifndef _MARKUPATTR_H_
#define _MARKUPATTR_H_ 

#include <_include/_inttype.h>
#include <lib/aptl/avector.h>

#include <LingProc4/MarkupTypes.h>

class LPMarkupAttrs
{
private:
    struct TagIdx
    {
        uint16_t size;   // number of attributes
        size_t   offset; // offset in data array
    };
    
    // data: 1st attr name length, 1st attr value length, ... last attr name length, last attr value length, name, value, ...
    //               ^ 1 byte               ^ 1 byte                  ^ 1 byte               ^ 1 byte
    avector<uint8_t> data;
    avector<TagIdx>  index;
    
    // this resize reallocates memory only if capacity is not enough
    // and shifts internal size field otherwise;
    // there are quite many resizes in LPMarkupAttrs::add()
    // but most of them won't actually reallocate anything
    template<class T> bool resize_array( avector<T> &a, size_t new_size )
    {
        a.resize( new_size );
        return ( a.size() >= new_size );
    }
    
    inline bool resize_index( size_t new_size )
    {
        return resize_array<TagIdx>( index, new_size );
    }
    inline bool resize_data( size_t new_size )
    {
        return resize_array<uint8_t>( data, new_size );
    }
    
public:
    // construct both index and data with growing_power = 1 to double the size on each realloc
    // to do later: some grow paramaters may be passed into constructor
    LPMarkupAttrs() :
        data( 32, 1 ),
        index( 32, 1 )
    {
    }
    
    bool empty() const
    {
        return index.empty();
    }
    
    bool empty( size_t i ) const
    {
        if ( i >= size() )
            return true;
        return ( index[i].size == 0 );
    }
    
    // container size
    // should be equal to number of tags (doctext.GetTextMarkup().GetLen())
    // (it must be ensured by caller - CDocText::AppendMarkup)
    size_t size() const
    {
        return index.size();
    }
    
    // number of attributes for i-th tag
    size_t size( unsigned i ) const
    {
        if ( i >= size() )
            return 0;
        return index[i].size;
    }
    
    // get j-th attribute of i-th tag
    MarkupAttr get( size_t i, size_t j ) const
    {
        MarkupAttr  result;
        uint8_t    *p;
        size_t     name_shift = 0, value_shift = 0, k;
        
        if ( i >= size() || j > index[i].size )
        {
            result.name = result.value = NULL;
            return result;
        }
        
        // i-th tag data starts at index[i].offset
        //p = (unsigned char *)data + index[i].offset;
        p = data.get_buffer() + index[ i ].offset;
        for ( k = 0; k < 2 * j; k++ )
            name_shift += p[ k ];
        value_shift = name_shift + p[ k ];
        
        result.name  = (char *)p + 2 * index[ i ].size + name_shift + 2 * j;
        //                bytes with sizes ^             terminators ^
        result.value = (char *)p + 2 * index[ i ].size + value_shift + 2 * j + 1;
        
        return result;
    }
    
    size_t add( MarkupAttrArray &a )
    {
        size_t  i, l1, l2;
        size_t  p_off, q_off;
        char    terminator = 0, *name, *value;
        const   size_t ATTR_MAX = 255;
        
        if ( !resize_index(index.size() + 1) )
            return 0;

        index[ index.size() - 1 ].size = a.size;
        if ( a.size == 0 )
            return size();
        
        index[ index.size() - 1 ].offset = data.size();

        p_off = data.size();
        q_off = p_off + 2 * a.size;
        
        // resize to store lengths (two bytes - name, value - for each attribute)
        if ( !resize_data(data.size() + 2 * a.size) )
            return 0;
        
        for ( i = 0; i < a.size; i++ )
        {
            l1 = a.attr[i].name  ? strlen( a.attr[i].name )  : 0;
            l2 = a.attr[i].value ? strlen( a.attr[i].value ) : 0;
            
            if ( l1 > ATTR_MAX ) 
                l1 = ATTR_MAX;
            if ( l2 > ATTR_MAX )
                l2 = ATTR_MAX;
            
            name  = l1 ? a.attr[i].name  : &terminator;
            value = l2 ? a.attr[i].value : &terminator;
            
            // resize to store names and values (with terminating zeros)
            if ( !resize_data( data.size() + (l1 + 1) + (l2 + 1)) ) 
                return 0;
            
            // name length (is always <= 255)
            data[ p_off + 2 * i ] = (unsigned char)l1;
            // value length (is always <= 255)
            data[ p_off + 2 * i + 1 ] = (unsigned char)l2;
            
            // name
            memcpy( data.get_buffer() + q_off, name, l1 + 1 );
            data[ q_off + l1 ] = terminator; // ensure terminating zero
            q_off += (l1 + 1);
            
            // value
            memcpy( data.get_buffer() + q_off, value, l2 + 1 );
            data[ q_off + l2 ] = terminator; // ensure terminating zero
            q_off += (l2 + 1);
        }
        return size();
    }
    
    // clear does not free the memory, just resets index data
    void clear()
    {
        index.clear();
        data.clear();
    }
};

#endif /* _MARKUPATTR_H_ */

