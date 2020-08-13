/**
 * @file   StorageDynamic.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr 29 13:31:15 2008
 * 
 * @brief  Dynamic memory buffer for PatriciaTree data.
 * 
 * 
 */
#ifndef _STORAGEDYNAMIC_H_
#define _STORAGEDYNAMIC_H_

#include <_include/_string.h>
#include <_include/_inttype.h>
#include <_include/cc_compat.h>

#include <lib/aptl/avector.h>

#include "PatriciaTreeCore.h"
#include "StorageInterface.h"

template<typename ElementType>
class StorageDynamic : public virtual StorageInterface
{
	typedef avector<ElementType> Pool;
        
public:
	/** 
	 * Standard constructor with no parameters.
	 * 
	 */
        StorageDynamic() : data( (PatriciaTreeCore::poolReallocSize / sizeof(ElementType)), 3 )
	{
	}
	/** 
	 * Standard destructor with no parameters.
	 * 
	 */
	~StorageDynamic() { };

public: /* StorageInterface implementation */
	/** 
	 * Returns the total size used by the data storage.
	 * 
	 * 
	 * @return the used size in bytes
	 */
	size_t getMemoryUsage() const
	{
		return data.size() * sizeof(ElementType);
	}
	/** 
	 * Returns the allocation size overhead caused by alignment.
	 * 
	 * 
	 * @return the alignment overhead in bytes
	 */
	size_t getAlignmentOverhead() const { return static_cast<size_t>(0); }
	/** 
	 * Requests the raw access to memory for reading for the current storage.
	 * 
	 * @param chunk The chunk structure to fill with the data pointers.
	 * 
	 */
	void getMemoryChunk(StorageChunk &chunk)
	{
		chunk.size   = StorageDynamic::getMemoryUsage();
		chunk.buffer = reinterpret_cast<const uint8_t *>( data.get_buffer() );
	}
	/** 
	 * Uses the raw access to memory provided for setting current storage data.
	 * 
	 * @param chunk The chunk structure with the data pointers.
	 * 
	 * @return 0 if success or 1 otherwise
	 */
	int setMemoryChunk(StorageChunk &chunk)
	{
                data.resize(chunk.size / sizeof(ElementType));
		if ( unlikely(data.no_memory()) )
			return 1;
		memcpy(data.get_buffer(), chunk.buffer, chunk.size);
		return 0;
	}
	
protected:
	PatriciaTreeCore::ResultStatus setDataSize(size_t size)
	{
                data.resize( size );
		if ( unlikely(data.no_memory()) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_ALLOC;
		return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
	}
	ElementType *getDataBuffer() const { return const_cast<ElementType *>(data.get_buffer()); }
	size_t       getDataSize()   const { return data.size();                                  }

public:
	PatriciaTreeCore::ResultStatus dataClear()
	{
		data.clear();
		return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
	}
	
private:
	Pool   data;
	size_t alignmentOverhead;
};

#endif /* _STORAGEDYNAMIC_H_ */

