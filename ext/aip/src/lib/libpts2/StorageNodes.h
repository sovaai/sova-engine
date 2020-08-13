/**
 * @file   StorageNodes.h
 * @author swizard <me@swizard.info>
 * @date   Mon May  5 01:23:07 2008
 * 
 * @brief  A nodes storage implementation.
 * 
 * 
 */
#ifndef _STORAGENODES_H_
#define _STORAGENODES_H_

#include <sys/types.h>

#include <_include/_inttype.h>
#include <_include/_string.h>
#include <_include/cc_compat.h>

#include "PatriciaTreeCore.h"

template<typename DataType>
class StorageNodes : public DataType
{
public:
	/** 
	 * Standard constructor with no parameters.
	 * 
	 */
        StorageNodes() : DataType(), alignmentOverhead(0)
	{
	}
	/** 
	 * Standard destructor with no parameters.
	 * 
	 */
	~StorageNodes() { };

public: /* Partial StorageInterface implementation */
	/** 
	 * Returns the allocation size overhead caused by alignment.
	 *
	 * 
	 * @return the alignment overhead in bytes
	 */
	size_t getAlignmentOverhead() const { return alignmentOverhead; }
	
protected:
	PatriciaTreeCore::ResultStatus nodeAlloc(size_t size, size_t maxSize)
	{
		size_t alignment = 0;
		if (size % PatriciaTreeCore::nodeAlignment != 0)
		{
			alignment          = (((size + (PatriciaTreeCore::nodeAlignment - 1)) /
					      PatriciaTreeCore::nodeAlignment) * PatriciaTreeCore::nodeAlignment) - 
size;
			alignmentOverhead += alignment;
			size              += alignment;
		}

		size_t newSize = DataType::getDataSize() + size;

		if (newSize > maxSize)
			return PatriciaTreeCore::RESULT_STATUS_ERROR_ALLOC_MAX_SIZE;
		
		PatriciaTreeCore::ResultStatus status = DataType::setDataSize(newSize);
		if ( unlikely( status != PatriciaTreeCore::RESULT_STATUS_SUCCESS ) )
			return status;
                
                if ( alignment > 0 )
                        memset( DataType::getDataBuffer() + newSize - alignment, 0, alignment );
	
		return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
	}

private:
	size_t alignmentOverhead;
};

#endif /* _STORAGENODES_H_ */

