/**
 * @file   PatriciaTreeInterface.h
 * @author swizard <me@swizard.info>
 * @date   Wed Apr 29 15:03:44 2008
 *
 * @brief  Patricia tree data storage interface -- common API.
 *
 *
 */
#ifndef _PATRICIATREEINTERFACE_H_
#define _PATRICIATREEINTERFACE_H_

#include <sys/types.h>

#include <_include/_inttype.h>
#include <lib/aptl/Functor.h>

#include "PatriciaTreeCore.h"
#include "PatriciaStorageHandler.h"

struct PatriciaTreeElement
{
	const uint8_t *key;
        size_t         keySize;
        const uint8_t *data;
        size_t         dataSize;
};

typedef Functor<const PatriciaTreeElement &, PatriciaTreeCore::ResultStatus> PatriciaTreeBypassFunctor;

struct PatriciaTreeInterface
{
	PatriciaTreeInterface() { }
	virtual ~PatriciaTreeInterface() { }

	virtual PatriciaTreeCore::ResultStatus create()                                                          = 0;
	virtual PatriciaTreeCore::ResultStatus cleanup()                                                         = 0;
	virtual PatriciaTreeCore::ResultStatus append(const uint8_t *key, const uint8_t **data, size_t dataSize) = 0;
	virtual PatriciaTreeCore::ResultStatus append( const uint8_t  *key,
                                                       size_t          keySize,
                                                       const uint8_t **data,
                                                       size_t          dataSize ) = 0;
	virtual PatriciaTreeCore::ResultStatus lookupExact(const uint8_t  *key,
							   const uint8_t **resultData,
							   size_t         *dataSize) const = 0;
	virtual PatriciaTreeCore::ResultStatus lookupExact(const uint8_t  *key,
                                                           size_t          keySize,
							   const uint8_t **resultData,
							   size_t         *dataSize) const = 0;

        virtual bool isEmpty() const = 0;
        virtual PatriciaTreeCore::ResultStatus bypass( PatriciaTreeBypassFunctor &functor ) const = 0;

	virtual PatriciaTreeCore::ResultStatus getStorageHandler(PatriciaStorageHandler &handler)                 = 0;
	virtual PatriciaTreeCore::ResultStatus setStorageHandler(uint16_t flags, PatriciaStorageHandler &handler) = 0;

	virtual uint32_t       dataAddressToLink(const uint8_t *data) const = 0;
	virtual const uint8_t *linkToDataAddress(uint32_t link) const       = 0;
	virtual size_t         getDataSize(const uint8_t *data) const       = 0;
	virtual const uint8_t *linkToKeyAddress(uint32_t link) const        = 0;
};

#endif /* _PATRICIATREEINTERFACE_H_ */

