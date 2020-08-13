/**
 * @file   CrossTreeInterface.h
 * @author swizard <me@swizard.info>
 * @date   Wed Apr 29 15:04:31 2008
 * 
 * @brief  Patricia tree data storage with words cross-links interface -- common API.
 * 
 * 
 */
#ifndef _CROSSTREEINTERFACE_H_
#define _CROSSTREEINTERFACE_H_

#include <_include/_inttype.h>

#include "CrossPathFunctor.h"
#include "PatriciaTreeInterface.h"

struct CrossTreeInterface : public virtual PatriciaTreeInterface
{
	CrossTreeInterface() { }
	virtual ~CrossTreeInterface() { }

	virtual PatriciaTreeCore::ResultStatus lookupAll( const uint8_t  *key, CrossPathFunctor &functor) const = 0;
	virtual PatriciaTreeCore::ResultStatus lookupBest(const uint8_t **key,
							  const uint8_t **resultData,
							  size_t         *dataSize,
							  bool           *isExactMatch) const = 0;
};

#endif /* _CROSSTREEINTERFACE_H_ */

