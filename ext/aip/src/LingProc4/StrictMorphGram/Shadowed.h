/**
 * @file   Shadowed.h
 * @author swizard <me@swizard.info>
 * @date   Tue Oct 20 03:55:54 2009
 * 
 * @brief  Real implementation shadow interface.
 * 
 * 
 */
#ifndef _SHADOWED_H_
#define _SHADOWED_H_

#include "sm_dll.h"

#include "StrictAllocator.h"

class SM_DLL Shadowed
{
public:
    Shadowed() : shadow(0) { }
    ~Shadowed() { }

    void assignShadow( const void *shadow ) { this->shadow = shadow; }
    const void *getShadow() const { return shadow; }
    
private:
    const void *shadow;
};

class SM_DLL ShadowedRW
{
public:
    ShadowedRW( StrictAllocator *_allocator = 0 );
    ~ShadowedRW();
    
protected:
    void dropUserBuffer();
    
protected:
    void            *userBuffer;
    StrictAllocator *userAllocator;
};

#endif /* _SHADOWED_H_ */

