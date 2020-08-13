/**
 * @file   PatriciaStorageHandler.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr 30 17:17:20 2008
 * 
 * @brief  The common patricia tree storage handler.
 * 
 * 
 */
#ifndef _PATRICIASTORAGEHANDLER_H_
#define _PATRICIASTORAGEHANDLER_H_

#include <sys/types.h>
#include <_include/_inttype.h>
#include <_include/_string.h>

struct StorageChunk
{
	size_t         size;
	const uint8_t *buffer;
};

enum PatriciaChuncks
{
	PATRICIA_CHUNK_HEADER = 0,
	PATRICIA_CHUNK_DATA,
	PATRICIA_CHUNK_CROSSLINKS,
	PATRICIA_CHUNK_REVERSE_INDEX,

	PATRICIA_CHUNK_LAST
};

struct PatriciaStorageHandler
{
	StorageChunk chunks[PATRICIA_CHUNK_LAST];

	PatriciaStorageHandler()
	{
		bzero(this, sizeof(PatriciaStorageHandler));
	}
};

#endif /* _PATRICIASTORAGEHANDLER_H_ */

