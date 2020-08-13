/**
 * @file   PatriciaTree.h
 * @author swizard <me@swizard.info>
 * @date   Thu Apr 29 16:27:14 2008
 * 
 * @brief  Aggregate functionality for patricia tree data storage.
 * 
 * 
 */
#ifndef _PATRICIATREE_H_
#define _PATRICIATREE_H_

#include <_include/_inttype.h>
#include <_include/cc_compat.h>

#include "PatriciaTreeCore.h"
#include "CrossPathFunctor.h"

#include "PatriciaTreeInterface.h"
#include "CrossTreeInterface.h"
#include "StorageInterface.h"
#include "ReversedIndexTreeInterface.h"

class PatriciaTree : public CrossTreeInterface, public StorageInterface, public ReversedIndexTreeInterface
{
        enum Constants
        {
                patriciaTreeHeaderSize   = sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t),
                patriciaTreeMajorVersion = 3,
                patriciaTreeMinorVersion = 1
        };

public:
	enum PatriciaTreeFlags
	{
		TREE_DEFAULT        = 0x0000U,
		TREE_STANDARD       = 0x0001U,
		TREE_COMPACT        = 0x0002U,
		TREE_CROSSLINKS     = 0x0004U,
		TREE_REVERSE_INDEX  = 0x0008U,
		TREE_READ_ONLY      = 0x0010U,
		TREE_STANDARD_RAW   = 0x0020U,
		TREE_GENERIC        = 0x0040U
	};
	
public:
	/** 
	 * Standard constructor with no parameters.
	 * 
	 */
	PatriciaTree();
	/** 
	 * Standard destructor with no parameters.
	 * 
	 */
	virtual ~PatriciaTree();
	/** 
	 * Initializes the tree internals.
	 * 
	 * @param flags The flags that specify the tree functionality
	 * 
	 * @return creation result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus create(uint16_t flags);

public: /* PatriciaTreeInterface implementation */
	/** 
	 * Initializes the tree internals.
	 * 
	 * 
	 * @return creation result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus create()
	{
		return create(TREE_DEFAULT);
	}
	/** 
	 * Cleans the current tree.
	 * 
	 * 
	 * @return cleaning result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus cleanup()
	{
		if ( unlikely(notCreated) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		return shadow->cleanup();
	}
	/** 
	 * Append the new data to the patricia tree using the given key.
	 * 
	 * @param key The key for data identification
	 * @param data The data to append (if NULL only allocate dataSize bytes inside a node)
	 * @param dataSize The size of the data to append
	 * 
	 * @return appending result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus append(const uint8_t *key, const uint8_t **data, size_t dataSize)
	{
		if ( unlikely( notCreated ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		if ( unlikely( flags & TREE_READ_ONLY ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_READ_ONLY;
		return shadow->append(key, data, dataSize);
	}
	/** 
	 * Searches the data within the tree using the key provided. Returns only exact key match.
	 * 
	 * @param key The key for data identification
	 * @param data The data found
	 * @param dataSize The size of the data found
	 * 
	 * @return searching result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus lookupExact(const uint8_t *key, const uint8_t **resultData, size_t *dataSize) const
	{
		if ( unlikely(notCreated) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		return shadow->lookupExact(key, resultData, dataSize);
	}
	/** 
	 * Append the new data to the patricia tree using the given key.
	 * 
	 * @param key The key for data identification
         * @param keySize The key size
	 * @param data The data to append (if NULL only allocate dataSize bytes inside a node)
	 * @param dataSize The size of the data to append
	 * 
	 * @return appending result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus append(const uint8_t *key, size_t keySize, const uint8_t **data, size_t dataSize)
	{
		if ( unlikely( notCreated ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		if ( unlikely( !(flags & TREE_GENERIC) ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;
		if ( unlikely( flags & TREE_READ_ONLY ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_READ_ONLY;
		return shadow->append(key, keySize, data, dataSize);
	}
	/** 
	 * Searches the data within the tree using the key provided. Returns only exact key match.
	 * 
	 * @param key The key for data identification
         * @param keySize The key size
	 * @param data The data found
	 * @param dataSize The size of the data found
	 * 
	 * @return searching result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus lookupExact(const uint8_t *key, size_t keySize, const uint8_t **resultData, size_t *dataSize) const
	{
		if ( unlikely(notCreated) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		if ( unlikely( !(flags & TREE_GENERIC) ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;
		return shadow->lookupExact(key, keySize, resultData, dataSize);
	}
        /** 
         * Fast constant-time operation: checks if the tree is empty.
         * 
         * 
         * @return true if empty or false otherwise.
         */
        bool isEmpty() const
        {
		if ( unlikely(notCreated) )
			return true;
		return shadow->isEmpty();
        }
        /** 
         * Invokes the functor for each node of the tree.
         * 
         * @param functor A functor to invoke. 
         * 
	 * @return Bypassing result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
         */
        PatriciaTreeCore::ResultStatus bypass( PatriciaTreeBypassFunctor &functor ) const
        {
		if ( unlikely(notCreated) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		return shadow->bypass(functor);
        }
	/** 
	 * Obtains the raw memory pointers for the tree serializing.
	 * 
	 * @param handler The handler for tree memory pointers
	 * 
	 * @return obtaining result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus getStorageHandler(PatriciaStorageHandler &handler);
	/** 
	 * Uses the raw memory pointers given for the tree creation.
	 * 
	 * @param flags Additional flags for tree creating behavior
	 * @param handler The handler with tree memory pointers
	 * 
	 * @return setting result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus setStorageHandler(uint16_t flags, PatriciaStorageHandler &handler);
	/** 
	 * Given the data address within a tree returns the internal link to the node.
	 * 
	 * @param data A data address within a tree
	 * 
	 * @return An internal link to the node (zero if an error occurred)
	 */
	uint32_t dataAddressToLink(const uint8_t *data) const
	{
		if ( unlikely(notCreated) )
			return static_cast<uint32_t>(0);
		return shadow->dataAddressToLink(data);
	}
	/** 
	 * Given the internal link to the node returns the actual memory pointer to the data in it.
	 * 
	 * @param offset An internal link to the node
	 * 
	 * @return An actual memory pointer to its data (null if an error occurred)
	 */
	const uint8_t *linkToDataAddress(uint32_t link) const
	{
		if ( unlikely(notCreated) )
			return 0;
		return shadow->linkToDataAddress(link);
	}
	/** 
	 * Given the data address within a tree returns that data size.
	 * 
	 * @param data A data address within a tree
	 * 
	 * @return A size of the data which address is given
	 */
	size_t getDataSize(const uint8_t *data) const
	{
		if ( unlikely(notCreated) )
			return static_cast<size_t>(-1);
		return shadow->getDataSize(data);
	}
	/** 
	 * Given the internal link to the node returns the actual memory pointer to the key in it.
	 * 
	 * @param offset An internal link to the node
	 * 
	 * @return An actual memory pointer to its key (null if an error occurred)
	 */
	const uint8_t *linkToKeyAddress(uint32_t link) const
	{
		if ( unlikely(notCreated) )
			return 0;
		return shadow->linkToKeyAddress(link);
	}
	
public: /* CrossTreeInterface */
	/** 
	 * Searches the data within the tree using the key provided. Returns best matche if no exact match found.
	 * 
	 * @param key The key for data identification
	 * @param data The data found
	 * @param dataSize The size of the data found
	 * @param isExactMatch Is set when the key found is the same as requested
	 * 
	 * @return searching result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus lookupBest( const uint8_t **key,
						   const uint8_t **resultData,
						   size_t         *dataSize,
						   bool           *isExactMatch ) const
	{
		if ( unlikely( notCreated ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		if ( unlikely( !(flags & TREE_CROSSLINKS) ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;

		if ( unlikely( crossTreeAccessor == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_INTERNAL;
		return crossTreeAccessor->lookupBest(key, resultData, dataSize, isExactMatch);
	}
	/** 
	 * Lookups the key (or the best match to it if not found) and traces all its
	 * taken up words with the functor provided.
	 * 
	 * @param key A key to lookup
	 * @param functor A functor to trace the word path
	 * 
	 * @return searching result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus lookupAll(const uint8_t *key, CrossPathFunctor &functor) const
	{
		if ( unlikely( notCreated ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		if ( unlikely( !(flags & TREE_CROSSLINKS) ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;

		if ( unlikely( crossTreeAccessor == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_INTERNAL;
		return crossTreeAccessor->lookupAll(key, functor);
	}
        
public: /* ReversedIndexTreeInterface */
	/** 
	 * Append the new data to the patricia tree using the given key and make an indexed link to it.
	 * 
	 * @param key The key for data identification
         * @param id The result index id
	 * 
	 * @return appending result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus indexedAppend( const uint8_t *key, uint32_t *id )
        {
		if ( unlikely( notCreated ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		if ( unlikely( !(flags & TREE_REVERSE_INDEX) ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;

		if ( unlikely( rindexTreeAccessor == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_INTERNAL;
		return rindexTreeAccessor->indexedAppend(key, id);
        }
        /** 
         * Searches the entry with the given id within the index and returns the result.
         * 
         * @param id An identificator to search
         * @param resultLink A result to return (node link)
         * 
         * @return RESULT_STATUS_SUCCESS or RESULT_STATUS_NOT_FOUND
         */
        PatriciaTreeCore::ResultStatus getFromIndex(uint32_t id, uint32_t *resultLink) const
        {
		if ( unlikely( notCreated ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		if ( unlikely( !(flags & TREE_REVERSE_INDEX) ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;

		if ( unlikely( rindexTreeAccessor == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_INTERNAL;
		return rindexTreeAccessor->getFromIndex(id, resultLink);
        }
        /** 
         * Returns the total number of nodes indexed.
         * 
         * 
         * @return The number of nodes in index
         */
        size_t getIndexSize()
        {
		if ( unlikely( notCreated ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;
		if ( unlikely( !(flags & TREE_REVERSE_INDEX) ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;

		if ( unlikely( rindexTreeAccessor == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_INTERNAL;
		return rindexTreeAccessor->getIndexSize();
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
		if ( unlikely( notCreated ) )
			return static_cast<size_t>(0);
		StorageInterface *t = dynamic_cast<StorageInterface *>(shadow);
		if ( unlikely( t == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_INTERNAL;
		return t->getMemoryUsage();
	}
	/** 
	 * Returns the allocation size overhead caused by alignment.
	 * 
	 * 
	 * @return the alignment overhead in bytes
	 */
	size_t getAlignmentOverhead() const
	{
		if ( unlikely( notCreated ) )
			return static_cast<size_t>(0);
		StorageInterface *t = dynamic_cast<StorageInterface *>(shadow);
		if ( unlikely( t == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_INTERNAL;
		return t->getAlignmentOverhead();
	}
	/** 
	 * Requests the raw access to memory for reading for the current storage.
	 * 
	 * @param chunk The chunk structure to fill with the data pointers.
	 * 
	 */
	void getMemoryChunk(StorageChunk &chunk)
	{
		memcpy(header + 0,                                    &flags,        sizeof(flags));
		memcpy(header + sizeof(flags),                        &majorVersion, sizeof(majorVersion));
		memcpy(header + sizeof(flags) + sizeof(majorVersion), &minorVersion, sizeof(minorVersion));
		chunk.size   = patriciaTreeHeaderSize;
		chunk.buffer = reinterpret_cast<const uint8_t *>(header);
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
		if ( unlikely(chunk.size != patriciaTreeHeaderSize) )
			return 1;
		memcpy(reinterpret_cast<void *>(&flags),
		       chunk.buffer + 0,
		       sizeof(flags));
		memcpy(reinterpret_cast<void *>(&majorVersion),
		       chunk.buffer + sizeof(flags),
		       sizeof(majorVersion));
		memcpy(reinterpret_cast<void *>(&minorVersion),
		       chunk.buffer + sizeof(flags) + sizeof(majorVersion),
		       sizeof(minorVersion));
		return 0;
	}
        
public: /* Utils */
        /** 
         * Counts the elements of the tree by bypassing it.
         * Warning: slow O(N) operation.
         * 
         * 
         * @return tree elements count
         */
        size_t getElementsCount() const;
        
protected:
	PatriciaTreeCore::ResultStatus processFlags(uint16_t flags);
	
protected:
	bool                   notCreated;
	uint16_t               flags;
	uint8_t                majorVersion;
	uint8_t                minorVersion;
	uint8_t                header[patriciaTreeHeaderSize];
	PatriciaTreeInterface *shadow;
        
        CrossTreeInterface         *crossTreeAccessor;
        ReversedIndexTreeInterface *rindexTreeAccessor;
};

#endif /* _PATRICIATREE_H_ */

