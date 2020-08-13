/**
 * @file   CrossTable.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr 28 07:22:43 2008
 * 
 * @brief  A crosslink lookup table.
 * 
 * 
 */
#ifndef _CROSSTABLE_H_
#define _CROSSTABLE_H_

#include <stdlib.h>

#include <_include/cc_compat.h>
#include <lib/aptl/OpenAddressingDict.h>

#include "PatriciaTreeCore.h"
#include "StorageInterface.h"

#ifndef PTS2_CROSS_HASHTABLE_SIZE
#define PTS2_CROSS_HASHTABLE_SIZE 65536
#endif

#ifndef PTS2_CROSS_HASHTABLE_INC
#define PTS2_CROSS_HASHTABLE_INC 32758
#endif

template<typename PatriciaPolicies, typename DataStorage>
class CrossTable : public StorageInterface
{
	typedef typename PatriciaPolicies::NodeAddress _NodeAddress;
        typedef OpenAddressingDict< _NodeAddress,
                                    _NodeAddress,
                                    PTS2_CROSS_HASHTABLE_SIZE,
                                    PTS2_CROSS_HASHTABLE_INC > CrossTableStorage;

public:
	/** 
	 * Standard constructor with no parameters.
	 * 
	 */
        CrossTable() : StorageInterface(), table(), readOnly(false)
	{
	}
	/** 
	 * Standard destructor with no parameters.
	 * 
	 */
	~CrossTable() { }

public:
	PatriciaTreeCore::ResultStatus addCrosslink(_NodeAddress node, _NodeAddress link)
	{
                OpenAddressingCommon::ResultStatus status = table.access(node, link);
                if ( unlikely(status != CrossTableStorage::RESULT_STATUS_SUCCESS) )
                        return tableToPatriciaError(status);
                
		return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
	}
	
	_NodeAddress getCrosslink(_NodeAddress node) const
	{
                _NodeAddress *result = 0;

                OpenAddressingCommon::ResultStatus status = table.get(node, &result);
                if ( status == CrossTableStorage::RESULT_STATUS_SUCCESS )
                        return *result;
                else if ( status == CrossTableStorage::RESULT_STATUS_NOT_FOUND )
			return static_cast<_NodeAddress>(0);
                else
                        return tableToPatriciaError(status);
	}
	
	void updateCrosslink(_NodeAddress node, _NodeAddress link)
	{
                _NodeAddress *address = 0;
                
                OpenAddressingCommon::ResultStatus status = table.access(node, &address);
                if ( likely(status == CrossTableStorage::RESULT_STATUS_SUCCESS) )
                {
                        /* Rewrite crosslink data */
                        *address = link;
                }
	}
        
	PatriciaTreeCore::ResultStatus dataClear()
        {
                if ( readOnly )
                        return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;
                table.clear();
                return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
        }
        
public: /* StorageInterface partial implementation */
	/** 
	 * Requests the raw access to memory for reading for the current storage.
	 * 
	 * @param chunk The chunk structure to fill with the data pointers.
	 * 
	 */
	void getMemoryChunk(StorageChunk &chunk)
	{
                chunk.buffer = table.getBuffer();
                chunk.size   = table.getBufferSize();
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
                table.setBuffer( const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer)) );
                readOnly = true;
		return 0;
	}
	/** 
	 * Returns the total size used by the data storage.
	 * 
	 * 
	 * @return the used size in bytes
	 */
	size_t getMemoryUsage() const
        {
                return table.getBufferSize();
        }
	/** 
	 * Returns the allocation size overhead caused by alignment.
	 * 
	 * 
	 * @return the alignment overhead in bytes
	 */
	size_t getAlignmentOverhead() const { return 0; }
	
private:
        PatriciaTreeCore::ResultStatus tableToPatriciaError( OpenAddressingCommon::ResultStatus status ) const
        {
                switch (status)
                {
                case OpenAddressingCommon::RESULT_STATUS_ERROR_ALLOCATION:
                        return PatriciaTreeCore::RESULT_STATUS_ERROR_ALLOC;
                        
                case OpenAddressingCommon::RESULT_STATUS_ERROR_USER_MEMORY_RESIZE:
                        return PatriciaTreeCore::RESULT_STATUS_ERROR_REALLOC_NOT_SUPPORTED;

                default:
                        return PatriciaTreeCore::RESULT_STATUS_ERROR_INTERNAL;
                }
        }
        
private:
        CrossTableStorage table;
        bool              readOnly;
};

#endif /* _CROSSTABLE_H_ */

