#ifndef _DOCTEXTWORDBREAKER_
#define _DOCTEXTWORDBREAKER_

#include <lib/aptl/Functor.h>

#include "DocTextWord.h"

class DocTextWordBreaker
{
public:
    typedef Functor< const DocTextWord &, LingProcErrors > WordReceiver;
    
public:
    virtual LingProcErrors performWordBreaking( const char *text, size_t textLength, WordReceiver &receiver ) = 0;
};

#endif /* _DOCTEXTWORDBREAKER_ */

