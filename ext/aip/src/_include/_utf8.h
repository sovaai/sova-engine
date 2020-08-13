#ifndef _INCLUDE_UTF8_H_
#define _INCLUDE_UTF8_H_

#include <_include/_inttype.h>
#include <unicode/utf8.h>

inline size_t countUtf8( const char *string, size_t stringLen = static_cast<size_t>(-1) )
{
    size_t count, offset;
    for ( count = offset = 0; offset < stringLen; count++ )
    {
        uint32_t ch;
        U8_NEXT_UNSAFE( string, offset, ch );
        if ( ch == 0 )
            break;
    }
        
    return count;
}

#endif /* _INCLUDE_UTF8_H_ */

