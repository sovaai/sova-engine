#ifndef _STRICTMORPHSTORAGECHUNK_H_
#define _STRICTMORPHSTORAGECHUNK_H_

#include <sys/types.h>

struct StrictMorphStorageChunk
{
	const void *buffer;
	size_t      size;

        StrictMorphStorageChunk() : buffer(0), size(0) { }
	
	enum Status
	{
		SUCCESS,
		ERROR_INVALID_SIZE,
		ERROR_CORRUPT_DATA,
		ERROR_VERSION_MISMATCH,
		ERROR_FSTORAGE
	};
};

#endif /* _STRICTMORPHSTORAGECHUNK_H_ */

