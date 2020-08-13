/**
 * @file   ReversedIndexTreeInterface.h
 * @author swizard <me@swizard.info>
 * @date   Tue Sep  2 22:53:10 2008
 * 
 * @brief  Patricia tree data storage with reversed index id->node interface -- common API.
 * 
 * 
 */
#ifndef _REVERSEDINDEXTREEINTERFACE_H_
#define _REVERSEDINDEXTREEINTERFACE_H_

#include <_include/_inttype.h>

#include "PatriciaTreeCore.h"

struct ReversedIndexTreeInterface
{
        ReversedIndexTreeInterface() { }
        virtual ~ReversedIndexTreeInterface() { }

	virtual PatriciaTreeCore::ResultStatus indexedAppend( const uint8_t *key,  uint32_t *id )    = 0;
        virtual PatriciaTreeCore::ResultStatus getFromIndex(uint32_t id, uint32_t *resultLink) const = 0;
        virtual size_t getIndexSize() = 0;
};

#endif /* _REVERSEDINDEXTREEINTERFACE_H_ */

