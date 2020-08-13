/**
 * @file   CrossTree.h
 * @author swizard <me@swizard.info>
 * @date   Wed Apr 25 01:37:59 2008
 * 
 * @brief  Patricia tree data storage with words cross-links.
 * 
 * 
 */
#ifndef _CROSSTREE_H_
#define _CROSSTREE_H_

#include <lib/aptl/GrowingPool.h>

#include "CrossPathFunctor.h"
#include "PatriciaTreeOverlay.h"
#include "CrossTreeInterface.h"

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
class CrossTree : public PatriciaTreeOverlay<PatriciaPolicies, DataStorage>, public virtual CrossTreeInterface
{
	typedef typename PatriciaPolicies::NodeAddress _NodeAddress;

protected:
	typename PatriciaPolicies::CheckUserFlag1  _checkCrosslink;
	typename PatriciaPolicies::SetUserFlag1    _setCrosslink;
	typename PatriciaPolicies::ClearUserFlag1  _clearCrosslink;
	
public:
	/** 
	 * Standard constructor with no parameters.
	 * 
	 */
	CrossTree();
	/** 
	 * Standard destructor with no parameters.
	 * 
	 */
	~CrossTree();

public:
	/** 
	 * Initializes the tree internals.
	 * 
	 * 
	 * @return creation result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus create();
	/** 
	 * Cleans the current tree.
	 * 
	 * 
	 * @return cleaning result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus cleanup();
	/** 
	 * Append the new data to the patricia tree using the given key.
	 * This also creates the crosslinks between words.
	 * 
	 * @param key The key for data identification
	 * @param data The data to append
	 * @param dataSize The size of the data to append
	 * 
	 * @return appending result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus append(const uint8_t *key, const uint8_t **data, size_t dataSize);
	/** 
	 * Append the new data to the patricia tree using the given key.
	 * This also creates the crosslinks between words.
	 * 
	 * @param key The key for data identification
         * @param keySize The key size
	 * @param data The data to append
	 * @param dataSize The size of the data to append
	 * 
	 * @return appending result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus append(const uint8_t */* key */, size_t /* keySize */, const uint8_t **/* data */, size_t /* dataSize */)
        {
                return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;
        }
	/** 
	 * Searches the data within the tree using the key provided. Returns only exact key match.
	 * 
	 * @param key The key for data identification
	 * @param resultData The data found
	 * @param dataSize The size of the data found
	 * 
	 * @return searching result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus lookupExact(const uint8_t *key, const uint8_t **resultData, size_t *dataSize) const
	{
		return PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::lookupExact(key, resultData, dataSize);
	}
	/** 
	 * Searches the data within the tree using the key provided. Returns only exact key match.
	 * 
	 * @param key The key for data identification
         * @param keySize The key size
	 * @param resultData The data found
	 * @param dataSize The size of the data found
	 * 
	 * @return searching result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus lookupExact(const uint8_t */* key */, size_t /* keySize */, const uint8_t **/* resultData */, size_t */* dataSize */) const
	{
                return PatriciaTreeCore::RESULT_STATUS_ERROR_UNSUPPORTED;
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
	PatriciaTreeCore::ResultStatus lookupAll(const uint8_t *key, CrossPathFunctor &functor) const;
	/** 
	 * Searches the data within the tree using the key provided. Returns best match if no exact match found.
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
						   bool           *isExactMatch ) const;
	/** 
	 * Returns the total size used by the data storage.
	 * 
	 * 
	 * @return the used size in bytes
	 */
	size_t getMemoryUsage() const { return DataStorage::getMemoryUsage() + crosslinks.getMemoryUsage(); }
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
		PatriciaTreeCore::ResultStatus status =
			PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::getStorageHandler( handler );
		if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
			return status;
		crosslinks.getMemoryChunk( handler.chunks[PATRICIA_CHUNK_CROSSLINKS] );
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
		PatriciaTreeCore::ResultStatus status =
			PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::setStorageHandler( flags, handler );
		if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
			return status;

		if ( unlikely( handler.chunks[PATRICIA_CHUNK_CROSSLINKS].size   != 0 &&
			       handler.chunks[PATRICIA_CHUNK_CROSSLINKS].buffer == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NO_HANDLER_CROSSLINKS;

		if ( unlikely(crosslinks.setMemoryChunk( handler.chunks[PATRICIA_CHUNK_CROSSLINKS] ) != 0) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_LOADING_FAILED;
		return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
	}
	
private:
	/** 
	 * A crosslink tree bypass functor.
	 * 
	 */
	class CrossBypassFunctor : public PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::BasicBypassFunctor
	{
	public:
	        CrossBypassFunctor( CrossTree<PatriciaPolicies, DataStorage, CrossStorage> *_self,
			            _NodeAddress                                            _root,
				    size_t                                                  _rootKeyLength ) :
		        PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::BasicBypassFunctor(),
			self( _self ),
			root( _root ),
			rootKeyLength( _rootKeyLength ) { }
		~CrossBypassFunctor() { }

		PatriciaTreeCore::ResultStatus apply(_NodeAddress node, uint8_t *address, const uint8_t *key)
		{
			return self->addBackCrosslink(root, node, rootKeyLength, address, key);
		}

	private:
		CrossTree<PatriciaPolicies, DataStorage, CrossStorage> *self;
		_NodeAddress                                            root;
		size_t                                                  rootKeyLength;
	};
	friend class CrossBypassFunctor;

private:
	PatriciaTreeCore::ResultStatus addBackCrosslink( _NodeAddress   node,
							 _NodeAddress   link,
							 size_t         nodeKeyLength,
							 uint8_t       *linkAddress,
							 const uint8_t *linkKey );
	
protected:
	PatriciaTreeCore::ResultStatus clear();
	PatriciaTreeCore::ResultStatus addCrosslink(_NodeAddress node, _NodeAddress link);
	
protected:
	CrossStorage crosslinks;
};

#endif /* _CROSSTREE_H_ */

