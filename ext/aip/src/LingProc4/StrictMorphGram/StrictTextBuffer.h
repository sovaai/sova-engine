/**
 * @file   StrictTextBuffer.h
 * @author swizard <me@swizard.info>
 * @date   Tue Nov  3 14:22:52 2009
 * 
 * @brief  StrictLex::TextFunctor with buffering using avector.
 * 
 * 
 */
#ifndef _STRICTTEXTBUFFER_H_
#define _STRICTTEXTBUFFER_H_

#include <lib/aptl/avector.h>

#include "StrictLex.h"

class StrictTextBuffer : public StrictLex::TextFunctor, public avector<char>
{
public:
    char *allocateBuffer( size_t size )
    {
        resize(size);
        if ( no_memory() )
            return 0;
        return get_buffer();
    }

    virtual StrictMorphErrors apply( const char *text ) { return STRICT_OK; }
};


#endif /* _STRICTTEXTBUFFER_H_ */

