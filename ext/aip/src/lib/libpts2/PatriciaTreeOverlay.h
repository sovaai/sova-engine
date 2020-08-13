/**
 * @file   PatriciaTreeOverlay.h
 * @author swizard <me@swizard.info>
 * @date   Wed Apr 23 00:34:05 2008
 * 
 * @brief  Patricia tree data storage -- implementation class.
 * 
 * 
 */
#ifndef _PATRICIATREEOVERLAY_H_
#define _PATRICIATREEOVERLAY_H_

#include <_include/cc_compat.h>
#include <_include/ntoh.h>

#include "PatriciaTreeCore.h"
#include "PatriciaTreeInterface.h"
#include "PatriciaStorageHandler.h"

template<typename PatriciaPolicies, typename DataStorage>
class PatriciaTreeOverlay :
	public PatriciaTreeCore, public DataStorage, public virtual PatriciaTreeInterface
{
	typedef typename PatriciaPolicies::NodeAddress _NodeAddress;

protected:
	/* The address conversion helpers: coming from PatriciaPolicy supplied */
	typename PatriciaPolicies::DataToNodeAddress  _dataToNodeAddress;
	typename PatriciaPolicies::OffsetToLink       _offsetToLink;
	typename PatriciaPolicies::LinkToOffset       _linkToOffset;
	/* The data access helpers: coming from PatriciaPolicy supplied */
	typename PatriciaPolicies::GetRightLink       _getRightLink;
	typename PatriciaPolicies::SetRightLink       _setRightLink;
	typename PatriciaPolicies::GetLeftLink        _getLeftLink;
	typename PatriciaPolicies::SetLeftLink        _setLeftLink;
	typename PatriciaPolicies::CheckRightTag      _checkRightTag;
	typename PatriciaPolicies::SetRightTag        _setRightTag;
	typename PatriciaPolicies::ClearRightTag      _clearRightTag;
	typename PatriciaPolicies::CheckLeftTag       _checkLeftTag;
	typename PatriciaPolicies::SetLeftTag         _setLeftTag;
	typename PatriciaPolicies::ClearLeftTag       _clearLeftTag;
	typename PatriciaPolicies::GetSkip            _getSkip;
	typename PatriciaPolicies::SetSkip            _setSkip;
	typename PatriciaPolicies::GetData            _getData;
	typename PatriciaPolicies::SetData            _setData;
	typename PatriciaPolicies::SetNullData        _setNullData;
	typename PatriciaPolicies::GetKey             _getKey;
	typename PatriciaPolicies::SetKey             _setKey;
	typename PatriciaPolicies::InitKey            _initKey;
	typename PatriciaPolicies::ClearNode          _clearNode;
        typename PatriciaPolicies::GetNodeAuxSize     _getNodeAuxSize;
	typename PatriciaPolicies::GetMinKeySize      _getMinKeySize;
	typename PatriciaPolicies::GetKeySize         _getKeySize;
	typename PatriciaPolicies::GetNullDataSize    _getNullDataSize;
	typename PatriciaPolicies::GetDataSize        _getDataSize;
	typename PatriciaPolicies::GetMaxTreeSize     _getMaxTreeSize;
	typename PatriciaPolicies::GetMaxDataSize     _getMaxDataSize;

	typename PatriciaPolicies::CommonPrefixLength _commonPrefixLength;
	
public:
	/** 
	 * Standard constructor with no parameters.
	 * 
	 */
	PatriciaTreeOverlay();
	/** 
	 * Standard destructor with no parameters.
	 * 
	 */
	~PatriciaTreeOverlay();

public: /* PatriciaTreeInterface implementation */
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
	 * 
	 * @param key The key for data identification
	 * @param data The data to append (if NULL only allocate dataSize bytes inside a node)
	 * @param dataSize The size of the data to append
	 * 
	 * @return appending result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus append(const uint8_t *key, const uint8_t **data, size_t dataSize);
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
	PatriciaTreeCore::ResultStatus lookupExact(const uint8_t *key, const uint8_t **resultData, size_t *dataSize) const;
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
	PatriciaTreeCore::ResultStatus append(const uint8_t *key, size_t keySize, const uint8_t **data, size_t dataSize);
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
	PatriciaTreeCore::ResultStatus lookupExact(const uint8_t *key, size_t keySize, const uint8_t **resultData, size_t *dataSize) const;
        /** 
         * Fast constant-time operation: checks if the tree is empty.
         * 
         * 
         * @return true if empty or false otherwise.
         */
        bool isEmpty() const
        {
                uint8_t *node = DataStorage::getDataBuffer() + headNode;
                return ( _checkLeftTag(node) != 0 );
        }
        /** 
         * Invokes the functor for each node of the tree.
         * 
         * @param functor A functor to invoke. 
         * 
	 * @return Bypassing result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
         */
        PatriciaTreeCore::ResultStatus bypass( PatriciaTreeBypassFunctor &functor ) const;
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
		DataStorage::getMemoryChunk( handler.chunks[PATRICIA_CHUNK_DATA] );
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
	PatriciaTreeCore::ResultStatus setStorageHandler(uint16_t /* flags */, PatriciaStorageHandler &handler)
	{
		if ( unlikely( handler.chunks[PATRICIA_CHUNK_DATA].size   != 0 &&
			       handler.chunks[PATRICIA_CHUNK_DATA].buffer == 0 ) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_NO_HANDLER_DATA;
		if ( unlikely(DataStorage::setMemoryChunk( handler.chunks[PATRICIA_CHUNK_DATA] ) != 0) )
			return PatriciaTreeCore::RESULT_STATUS_ERROR_LOADING_FAILED;
		return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
	}
	/** 
	 * Given the data address within a tree returns the internal link to the node.
	 * 
	 * @param data A data address within a tree
	 * 
	 * @return An internal link to the node (zero if an error occurred)
	 */
	uint32_t dataAddressToLink(const uint8_t *data) const
	{
		return _offsetToLink( _dataToNodeAddress(data) - DataStorage::getDataBuffer() );
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
		return _getData( DataStorage::getDataBuffer() + _linkToOffset(link) );
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
		return _getDataSize( _dataToNodeAddress(data) );
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
		return _getKey( DataStorage::getDataBuffer() + _linkToOffset(link) );
	}
	
protected:
	/** 
	 * A state for tracking during lookup.
	 * 
	 */
	struct LookupState
	{
		_NodeAddress previousNode;
		_NodeAddress currentNode;
		size_t       bitIndex;

	        LookupState() : previousNode(0), currentNode(0), bitIndex(0) { }
	};
	
protected:
	/** 
	 * A basic tree bypass functor.
	 * 
	 */
	struct BasicBypassFunctor
	{
		BasicBypassFunctor() { }
		virtual ~BasicBypassFunctor() { }

		virtual PatriciaTreeCore::ResultStatus apply(_NodeAddress node, uint8_t *address, const uint8_t *key) = 0;
	};
	
protected:
	PatriciaTreeCore::ResultStatus clear();
	PatriciaTreeCore::ResultStatus append( const uint8_t   *key,
                                               size_t           keyLength,
					       const uint8_t  **data,
					       size_t           dataSize,
					       size_t          &commonBitsCount,
					       _NodeAddress    &currentNode,
					       size_t          &nodeKeyLength,
					       _NodeAddress    &newNode );
	PatriciaTreeCore::ResultStatus bypassTree( BasicBypassFunctor &functor, _NodeAddress root, bool isEndpoint ) const;
	_NodeAddress lookup(const uint8_t *key, size_t bitsCount, LookupState *state) const;

private:
	void probe( const uint8_t *key,
                    size_t         keySize,
		    _NodeAddress  &node,
		    size_t        &keyBitsCount,
		    size_t        &commonBitsCount ) const;
};

#endif /* _PATRICIATREEOVERLAY_H_ */

