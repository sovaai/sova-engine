#ifndef _RUNSM_H_
#define _RUNSM_H_

#include <_include/_inttype.h>
#include <_include/cc_compat.h>

#include <unicode/utf8.h>

#include "../../LingProcErrors.h"

class UCharSet;

template< typename TextSM >
class RunSM : public TextSM
{
public:
    RunSM( const UCharSet &_genericCharset ) : TextSM( _genericCharset ) { }
    
public:
    LingProcErrors run( const char *text, size_t textLength ) 
    {
        TextSM::reset();
        
        size_t current = 0;
        
        for ( size_t textOffset = 0; ; current = textOffset )
        {
            uint32_t ch = static_cast<uint32_t>(-1);
            if ( textOffset < textLength )
            {
                U8_NEXT_UNSAFE( text, textOffset, ch );
            }
                            
            LingProcErrors status = TextSM::nextChar( current, ch );
            if ( unlikely(status != LP_OK) )
                return status;
            
            if ( ch == static_cast<uint32_t>(-1) )
                break;
        }

        return TextSM::flush();
    }
};

#endif /* _RUNSM_H_ */

