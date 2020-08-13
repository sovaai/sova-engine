/**
 * @file   StorageStatic.h
 * @author swizard <me@swizard.info>
 * @date   Mon May  5 03:23:10 2008
 * 
 * @brief  Static memory buffer for PatriciaTree data.
 * 
 * 
 */
#ifndef _STORAGESTATIC_H_
#define _STORAGESTATIC_H_

#include <_include/_inttype.h>

#include "PatriciaTreeCore.h"
#include "StorageInterface.h"

template<typename ElementType>
class StorageStatic : public virtual StorageInterface
{
public:
	/** 
	 * Standard constructor with no parameters.
	 * 
	 */
        StorageStatic() : data(0), elementsCount(0)
	{
	}
	/** 
	 * Standard destructor with no parameters.
	 * 
	 */
	~StorageStatic() { };

public: /* StorageInterface implementation */
	/** 
	 * Returns the total size used by the data storage.
	 * 
	 * 
	 * @return the used size in bytes
	 */
	size_t getMemoryUsage() const
	{
		return elementsCount * sizeof(ElementType);
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
		chunk.size   = StorageStatic::getMemoryUsage();
		chunk.buffer = reinterpret_cast<const uint8_t *>( data );
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
		data          = const_cast<ElementType *>( reinterpret_cast<const ElementType *>(chunk.buffer) );
		elementsCount = chunk.size / sizeof(ElementType);
		return 0;
	}
	
protected:
	PatriciaTreeCore::ResultStatus setDataSize(size_t /* size */)
	{
		return PatriciaTreeCore::RESULT_STATUS_ERROR_REALLOC_NOT_SUPPORTED;
	}
	ElementType *getDataBuffer() const { return data;          }
	size_t       getDataSize()   const { return elementsCount; }

public:
	PatriciaTreeCore::ResultStatus dataClear()
	{
		return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;
	}
	
private:
	ElementType *data;
	size_t       elementsCount;
};

#endif /* _STORAGESTATIC_H_ */

