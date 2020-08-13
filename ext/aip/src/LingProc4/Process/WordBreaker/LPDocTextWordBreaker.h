#ifndef _LPDOCTEXTWORDBREAKER_H_
#define _LPDOCTEXTWORDBREAKER_H_

#include "DefaultWordBreakerSM.h"
#include "RunSM.h"

#include "../../DocText/DocTextWordBreaker.h"

class UCharSet;

class LPDocTextWordBreaker : public RunSM< DefaultWordBreakerSM< LPDocTextWordBreaker > >,
                             public DocTextWordBreaker
{
public:
    LPDocTextWordBreaker( const UCharSet &_genericCharset ) :
        RunSM< DefaultWordBreakerSM< LPDocTextWordBreaker > >( _genericCharset ),
        DocTextWordBreaker(),
        currentReceiver( 0 )
    {
    }

public:
    LingProcErrors performWordBreaking( const char *text, size_t textLength, WordReceiver &receiver )
    {
        currentReceiver = &receiver;
        return run( text, textLength );
    }

    LingProcErrors receiveWord( const DocTextWord &word )
    {
        if ( unlikely(currentReceiver == 0) )
            return LP_ERROR_EFAULT;

        return currentReceiver->apply( word );
    }

private:
    WordReceiver *currentReceiver;
};

#endif /* _LPDOCTEXTWORDBREAKER_H_ */

