/**
 * @file   StrictAllocator.h
 * @author swizard <me@swizard.info>
 * @date   Sun Jan 31 06:09:36 2010
 * 
 * @brief  Allocator interface for strict morph.
 * 
 * 
 */
#ifndef _STRICTALLOCATOR_H_
#define _STRICTALLOCATOR_H_

#include <stdlib.h>

class SM_DLL StrictAllocator
{
public:
    virtual void *alloc( size_t size ) = 0;
    virtual void  free( void *ptr )    = 0;
};

#endif /* _STRICTALLOCATOR_H_ */
