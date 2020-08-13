/**
 * @file   StorageInterface.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr 30 15:18:33 2008
 * 
 * @brief  PatriciaTree storages common API.
 * 
 * 
 */
#ifndef _STORAGEINTERFACE_H_
#define _STORAGEINTERFACE_H_

#include <sys/types.h>

#include "PatriciaStorageHandler.h"

struct StorageInterface
{
	StorageInterface() { }
	virtual ~StorageInterface() { }

	virtual size_t getMemoryUsage() const            = 0;
	virtual size_t getAlignmentOverhead() const      = 0;

	virtual void getMemoryChunk(StorageChunk &chunk) = 0; 
	virtual int  setMemoryChunk(StorageChunk &chunk) = 0; 
};

#endif /* _STORAGEINTERFACE_H_ */

