#ifndef _DEFAULTLINEBREAKERSM_H_
#define _DEFAULTLINEBREAKERSM_H_

#include <_include/cc_compat.h>

#include "../../LingProcErrors.h"
#include "../../DocText/DocTextLine.h"

class UCharSet;

template< typename OwnerClass >
class DefaultLineBreakerSM
{
public:
    DefaultLineBreakerSM( const UCharSet & /* _genericCharset */ ) :
        lbState( LINE_BREAK_START ),
        lineStart( 0 ),
        lineEnd( 0 ),
        line()
    {
    }

    void reset()
    {
        lbState   = LINE_BREAK_START;
        lineStart = 0;
        lineEnd   = 0;
    }
    
private:
    enum LineBreakState
    {
        LINE_BREAK_START,
        LINE_BREAK_LINE,
        LINE_BREAK_DONE
    };
    
public:
    LingProcErrors nextChar( size_t current, uint32_t ch )
    {
        LingProcErrors status;
        
        // line construction
        for ( ;; )
        {
            switch ( lbState )
            {
            case LINE_BREAK_START:
                if ( ch == static_cast<uint32_t>(-1) )
                    break;
                    
                lineStart = current;
                lbState   = LINE_BREAK_LINE;
                continue;
                    
            case LINE_BREAK_LINE:
                if ( ch == static_cast<uint32_t>('\n') || ch == static_cast<uint32_t>(-1) )
                {
                    lbState = LINE_BREAK_DONE;
                    continue;
                }
                break;
                    
            case LINE_BREAK_DONE:
                lineEnd = current;

                status = line.Init( static_cast<uint32_t>( lineStart ),
                                    static_cast<uint32_t>( lineEnd - lineStart ),
                                    0,
                                    0,
                                    0 );
                if ( unlikely(status != LP_OK) )
                    return status;

                status = static_cast<OwnerClass *>(this)->receiveLine( line );
                if ( unlikely(status != LP_OK) )
                    return status;

                lbState = LINE_BREAK_START;
                break;
            };

            break;
        }
            
        return LP_OK;
    }

    LingProcErrors flush()
    {
        return LP_OK;
    }

private:
    LineBreakState  lbState;
    size_t          lineStart;
    size_t          lineEnd;
    DocTextLine     line;
};

#endif /* _DEFAULTLINEBREAKERSM_H_ */
