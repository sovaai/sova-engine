/**
 * @file   PatriciaTreeCore.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr 21 15:16:16 2008
 * 
 * @brief  PatriciaTree data storage -- base class.
 * 
 * 
 */
#ifndef _PATRICIATREECORE_H_
#define _PATRICIATREECORE_H_

#include <sys/types.h>
#include <_include/_inttype.h>

class PatriciaTreeCore
{
public:
	enum ResultStatus
	{
		RESULT_STATUS_SUCCESS,
		RESULT_STATUS_DUPLICATE_KEY,
		RESULT_STATUS_NOT_FOUND,
		RESULT_STATUS_ERROR_NOT_CREATED,
		RESULT_STATUS_ERROR_ALREADY_CREATED,
		RESULT_STATUS_ERROR_INVALID_FLAGS,
		RESULT_STATUS_ERROR_UNSUPPORTED,
		RESULT_STATUS_ERROR_ALLOC,
		RESULT_STATUS_ERROR_ALLOC_MAX_SIZE,
		RESULT_STATUS_ERROR_DATA_TOO_LARGE,
		RESULT_STATUS_ERROR_REALLOC_NOT_SUPPORTED,
		RESULT_STATUS_ERROR_INTERNAL,
		RESULT_STATUS_ERROR_READ_ONLY,
		RESULT_STATUS_ERROR_LOADING_FAILED,
		RESULT_STATUS_ERROR_NO_HANDLER_DATA,
		RESULT_STATUS_ERROR_NO_HANDLER_CROSSLINKS,
		RESULT_STATUS_ERROR_NO_HANDLER_REVERSE_INDEX,
		RESULT_STATUS_ERROR_OBTAIN_SECTION,
		RESULT_STATUS_ERROR_STREAM_SECTION,
		RESULT_STATUS_ERROR_GET_SECTION_DATA,
		RESULT_STATUS_ERROR_WRONG_MAGIC_VALUE,
		RESULT_STATUS_ERROR_VERSION_MISMATCH
	};

public:
        enum Constants
        {
                poolReallocSize = 256 * 1024,
                nodeAlignment   = sizeof(uint32_t),
                headNode        = 0
        };
	
public:
	/** 
	 * Standard constructor with no parameters.
	 * 
	 */
	PatriciaTreeCore() { };
	/** 
	 * Standard destructor with no parameters.
	 * 
	 */
	~PatriciaTreeCore() { };
};

#endif /* _PATRICIATREECORE_H_ */

