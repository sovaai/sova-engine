/**
 * @file   ReverseIndexMixin.h
 * @author swizard <me@swizard.info>
 * @date   Tue Sep  2 15:40:38 2008
 * 
 * @brief  A mixin for PatriciaTree with reverse index feature.
 * 
 * 
 */
#ifndef _REVERSEINDEXMIXIN_H_
#define _REVERSEINDEXMIXIN_H_

#include <_include/_inttype.h>

#include "PatriciaTreeCore.h"
#include "ReversedIndexTreeInterface.h"

template<typename DataStorage, typename PatriciaType>
class ReverseIndexMixin : public PatriciaType, public DataStorage, public ReversedIndexTreeInterface
{
public:
        ReverseIndexMixin();
        ~ReverseIndexMixin();

public:
        /** 
         * Searches the entry with the given id within the index and returns the result.
         * 
         * @param id An identificator to search
         * @param resultLink A result to return (node link)
         * 
         * @return RESULT_STATUS_SUCCESS or RESULT_STATUS_NOT_FOUND
         */
        PatriciaTreeCore::ResultStatus getFromIndex(uint32_t id, uint32_t *resultLink) const;
	/** 
	 * Append the new data to the patricia tree using the given key and make an indexed link to it.
	 * 
	 * @param key The key for data identification
         * @param id The result index id
	 * 
	 * @return appending result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus indexedAppend( const uint8_t *key, uint32_t *id );
        /** 
         * Returns the total number of nodes indexed.
         * 
         * 
         * @return The number of nodes in index
         */
        size_t getIndexSize()
        {
                return DataStorage::getDataSize();
        }

public:
	/** 
	 * Obtains the raw memory pointers for the tree serializing.
	 * 
	 * @param handler The handler for tree memory pointers
	 * 
	 * @return obtaining result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus getStorageHandler(PatriciaStorageHandler &handler)
	{
		PatriciaTreeCore::ResultStatus status = PatriciaType::getStorageHandler( handler );
		if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
			return status;
		getMemoryChunk( handler.chunks[PATRICIA_CHUNK_REVERSE_INDEX] );
		return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
	}
	/** 
	 * Uses the raw memory pointers given for the tree creation.
	 * 
	 * @param flags Additional flags for tree creating behavior
	 * @param handler The handler with tree memory pointers
	 * 
	 * @return setting result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus setStorageHandler(uint16_t flags, PatriciaStorageHandler &handler)
	{
		PatriciaTreeCore::ResultStatus status = PatriciaType::setStorageHandler( flags, handler );
		if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
			return status;

		if ( unlikely( handler.chunks[PATRICIA_CHUNK_REVERSE_INDEX].size   != 0 &&
			       handler.chunks[PATRICIA_CHUNK_REVERSE_INDEX].buffer == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NO_HANDLER_REVERSE_INDEX;

		if ( unlikely(setMemoryChunk( handler.chunks[PATRICIA_CHUNK_REVERSE_INDEX] ) != 0) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_LOADING_FAILED;
		return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
	}

        
public: /* StorageInterface */
	/** 
	 * Returns the total size used by the data storage.
	 * 
	 * 
	 * @return the used size in bytes
	 */
	size_t getMemoryUsage() const
	{
		return PatriciaType::getMemoryUsage() + DataStorage::getMemoryUsage();
	}
	/** 
	 * Returns the allocation size overhead caused by alignment.
	 * 
	 * 
	 * @return the alignment overhead in bytes
	 */
	size_t getAlignmentOverhead() const
	{
		return PatriciaType::getAlignmentOverhead() + DataStorage::getAlignmentOverhead();
	}
	/** 
	 * Requests the raw access to memory for reading for the current storage.
	 * 
	 * @param chunk The chunk structure to fill with the data pointers.
	 * 
	 */
	void getMemoryChunk(StorageChunk &chunk)
	{
                DataStorage::getMemoryChunk(chunk);
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
                return DataStorage::setMemoryChunk(chunk);
	}
        
protected:
        PatriciaTreeCore::ResultStatus addToIndex(uint32_t link);
};

#endif /* _REVERSEINDEXMIXIN_H_ */

