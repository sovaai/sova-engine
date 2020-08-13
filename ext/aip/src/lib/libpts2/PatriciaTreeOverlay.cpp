/**
 * @file   PatriciaTreeOverlay.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed Apr 23 00:44:18 2008
 *
 * @brief  Patricia tree data storage -- implementation class (implementation).
 *
 *
 */

#include <lib/aptl/avector.h>

#include "PatriciaTreeOverlay.h"

#include "PolicyStandard.h"
#include "PolicyStandardRaw.h"
#include "PolicyCompact.h"
#include "PolicyGeneric.h"

#include "StorageNodes.h"
#include "StorageStatic.h"
#include "StorageDynamic.h"

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
PatriciaTreeOverlay() :
	PatriciaTreeInterface(),
	PatriciaTreeCore(),
	DataStorage()
{
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
~PatriciaTreeOverlay()
{
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
create()
{
	return clear();
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
cleanup()
{
	return clear();
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
clear()
{
	PatriciaTreeCore::ResultStatus status = PatriciaTreeCore::RESULT_STATUS_SUCCESS;

	status = DataStorage::dataClear();
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;

	/* Create the head node */
	size_t nodeSize = _getNodeAuxSize() + _getNullDataSize() + _getMinKeySize();
	status          = DataStorage::nodeAlloc( nodeSize, _getMaxTreeSize() );
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;

	uint8_t *node = DataStorage::getDataBuffer() + headNode;
	_clearNode(node, nodeSize);
	_setLeftTag(node);
	_setLeftLink(node, headNode);
	_setNullData(node);
	_setKey(node, reinterpret_cast<const uint8_t *>(""), 0);

	return status;
}

template<typename PatriciaPolicies, typename DataStorage>
typename PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::_NodeAddress
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
lookup(const uint8_t *key, size_t bitsCount, LookupState *state) const
{
	_NodeAddress     node             = headNode;
	_NodeAddress     nextNode         = headNode;
	register int     currentBitValue  = 0;

	/* Current bit number --- numbered from 1 */
	register size_t  currentBitIndex  = 0;
        uint8_t         *buffer           = DataStorage::getDataBuffer();

	/* Start the tree bypass process */
	for (;;)
        {
		uint8_t *currentNode = buffer + node;
		if (currentBitValue == 0)
                {
			nextNode = _getLeftLink(currentNode);
			if ( _checkLeftTag(currentNode) )
                        {
				if (state != 0)
                                {
					state->previousNode = node;
					state->currentNode  = nextNode;
					state->bitIndex     = currentBitIndex;
                                }

				node = nextNode;
				break;
                        }
                }
		else /* currentBitValue == 1 */
                {

			nextNode = _getRightLink(currentNode);
			if ( _checkRightTag(currentNode) )
                        {
				if (state != 0)
                                {
					state->previousNode = node;
					state->currentNode  = nextNode;
					state->bitIndex     = currentBitIndex;
                                }

				node = nextNode;
				break;
                        }
                }

		currentBitIndex += _getSkip(buffer + nextNode);

		if (currentBitIndex > bitsCount)
                {
			if (state != 0)
			{
				state->previousNode = node;
				state->currentNode  = nextNode;
				state->bitIndex     = currentBitIndex;
			}

			node = nextNode;
			break;
                }

                register size_t cbi = currentBitIndex - 1;
                currentBitValue     = (static_cast<int>(key[cbi >> 3]) >> (7 - (cbi & 0x7))) & 0x01;

		node = nextNode;
        }

	return node;
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
append(const uint8_t *key, const uint8_t **data, size_t dataSize)
{
	register size_t keyLength;
        for ( keyLength = 0; key[keyLength] != '\0'; keyLength++ );

        return append( key, keyLength, data, dataSize);
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
append(const uint8_t *key, size_t keySize, const uint8_t **data, size_t dataSize)
{
	size_t       nodeKeyLength;
	size_t       commonBitsCount;
	_NodeAddress currentNode;
	_NodeAddress newNode;

        avector<uint8_t>  globalBuffer;
        uint8_t           localBuffer[1024];
        const uint8_t    *newKeyPtr = key;

        if ( _initKey() )
        {
                uint8_t *tmpPtr       = 0;
                size_t   requiredSize = _getKeySize(keySize);
                if ( requiredSize <= sizeof(localBuffer) )
                        tmpPtr = localBuffer;
                else
                {
                        globalBuffer.resize(requiredSize);
                        if ( unlikely(globalBuffer.no_memory()) )
                                return RESULT_STATUS_ERROR_ALLOC;
                        tmpPtr = globalBuffer.get_buffer();
                }

                _initKey( key, keySize, tmpPtr );
                newKeyPtr = tmpPtr;
        }

	return append( newKeyPtr, keySize, data, dataSize, commonBitsCount, currentNode, nodeKeyLength, newNode );
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
append( const uint8_t   *key,
        size_t           keySize,
	const uint8_t  **data,
	size_t           dataSize,
	size_t          &commonBitsCount,
	_NodeAddress    &currentNode,
	size_t          &nodeKeyLength,
	_NodeAddress    &newNode )
{
	if ( dataSize > _getMaxDataSize() )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_DATA_TOO_LARGE;

	PatriciaTreeCore::ResultStatus status = PatriciaTreeCore::RESULT_STATUS_SUCCESS;

	size_t keyBitsCount = 0;
	probe( key, keySize, currentNode, keyBitsCount, commonBitsCount );

	nodeKeyLength = _getKeySize(DataStorage::getDataBuffer() + currentNode);

	if ( unlikely(commonBitsCount == keyBitsCount &&
		      keySize         == nodeKeyLength) )
        {
		/* Already have a node with the key given */
		*data = _getData(DataStorage::getDataBuffer() + currentNode);
		return PatriciaTreeCore::RESULT_STATUS_DUPLICATE_KEY;
        }

	/* Add the new node to the tree */
	newNode         = static_cast<_NodeAddress>( DataStorage::getDataSize() );
	size_t nodeSize = _getNodeAuxSize() + _getDataSize(dataSize) + _getKeySize(keySize);

	/* Create the current node */
	status = DataStorage::nodeAlloc( nodeSize, _getMaxTreeSize() );
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;

	uint8_t *nodeAddress = DataStorage::getDataBuffer() + newNode;
	_clearNode(nodeAddress, nodeSize);

	/* Fill the current node data */
	_setData( nodeAddress, *data, dataSize );
	_setKey( nodeAddress, key, keySize );

	/* Change the data pointer */
	*data = _getData(nodeAddress);

	/* Find the node containing the prefix got */
	LookupState state;
	lookup(key, commonBitsCount, &state);

	/* Change the tree */
	bool markTag = false;

	int keyBitDirection = key[commonBitsCount >> 3] & static_cast<uint8_t>(1 << ( 7 - (commonBitsCount % 8) ));

	uint8_t *previousNode = DataStorage::getDataBuffer() + state.previousNode;
	if (_getLeftLink(previousNode) == state.currentNode)
        {
		/* Came from left link */
		if (_checkLeftTag(previousNode))
                {
			markTag = true;
			_clearLeftTag(previousNode);
                }
		_setLeftLink(previousNode, newNode);
        }
	else
        {
		/* Came from right link */
		if (_checkRightTag(previousNode))
                {
			markTag = true;
			_clearRightTag(previousNode);
                }
		_setRightLink(previousNode, newNode);
        }

	uint8_t *newNodeAddress = DataStorage::getDataBuffer() + newNode;
	if (keyBitDirection != 0)
        {
		_setLeftLink(newNodeAddress, state.currentNode);
		if (markTag)
			_setLeftTag(newNodeAddress);

		_setRightLink(newNodeAddress, newNode);
		_setRightTag(newNodeAddress);
        }
	else
        {
		_setRightLink(newNodeAddress, state.currentNode);
		if (markTag)
			_setRightTag(newNodeAddress);

		_setLeftLink(newNodeAddress, newNode);
		_setLeftTag(newNodeAddress);
        }

	if (markTag)
		_setSkip(newNodeAddress, (1 + commonBitsCount - state.bitIndex));
	else
        {
		_setSkip(newNodeAddress,
			 ( 1 + commonBitsCount - state.bitIndex + _getSkip(DataStorage::getDataBuffer() + state.currentNode) ));
		_setSkip(DataStorage::getDataBuffer() + state.currentNode,
			 ( state.bitIndex - commonBitsCount - 1) );
        }

	return status;
}

template<typename PatriciaPolicies, typename DataStorage>
inline
void
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
probe( const uint8_t *key,
       size_t        keyOctetsCount,
       _NodeAddress  &node,
       size_t        &keyBitsCount,
       size_t        &commonBitsCount ) const
{
	/* Calculate bits count in a key given */
        keyBitsCount    = _getKeySize(keyOctetsCount) << 3;

	/* Find out the length of prefix, which is already stored in the tree */
	node            = lookup(key, keyBitsCount, 0);

	/* Calculate common bits count in key and key of the node found */
	commonBitsCount = _commonPrefixLength( key, (DataStorage::getDataBuffer() + node) );
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
lookupExact(const uint8_t *key, const uint8_t **data, size_t *dataSize) const
{
	register size_t keyLength;
        for ( keyLength = 0; key[keyLength] != '\0'; keyLength++ );

        return PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::lookupExact( key, keyLength, data, dataSize);
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
lookupExact(const uint8_t *key, size_t keySize, const uint8_t **data, size_t *dataSize) const
{
	_NodeAddress node            = headNode;
	size_t       keyBitsCount    = 0;
	size_t       commonBitsCount = 0;

        avector<uint8_t>  globalBuffer;
        uint8_t           localBuffer[1024];
        const uint8_t    *newKeyPtr = key;

        if ( _initKey() )
        {
                uint8_t *tmpPtr       = 0;
                size_t   requiredSize = _getKeySize(keySize);
                if ( requiredSize <= sizeof(localBuffer) )
                        tmpPtr = localBuffer;
                else
                {
                        globalBuffer.resize(requiredSize);
                        if ( unlikely(globalBuffer.no_memory()) )
                                return RESULT_STATUS_ERROR_ALLOC;
                        tmpPtr = globalBuffer.get_buffer();
                }

                _initKey( key, keySize, tmpPtr );
                newKeyPtr = tmpPtr;
        }

	probe( newKeyPtr, keySize, node, keyBitsCount, commonBitsCount );

	if (commonBitsCount != keyBitsCount)
		return PatriciaTreeCore::RESULT_STATUS_NOT_FOUND;
	else
	{
		uint8_t *address = DataStorage::getDataBuffer() + node;
		*data     = _getData(address);
		*dataSize = _getDataSize(address);
	}

	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
bypassTree( BasicBypassFunctor &functor, _NodeAddress root, bool isEndpoint ) const
{
	PatriciaTreeCore::ResultStatus status;

	uint8_t       *node = DataStorage::getDataBuffer() + root;
	const uint8_t *key  = _getKey(node);

	if (isEndpoint)
 		return functor.apply(root, node, key);
	else
	{
		status = bypassTree( functor, _getLeftLink(node),  _checkLeftTag(node)  );
		if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS ) )
			return status;

                if ( likely(root != headNode) )
                {
                        status = bypassTree( functor, _getRightLink(node), _checkRightTag(node) );
                        if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS ) )
                                return status;
                }
	}

	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

template<typename PatriciaPolicies, typename DataStorage>
PatriciaTreeCore::ResultStatus
PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
bypass( PatriciaTreeBypassFunctor &functor ) const
{
        if ( isEmpty() )
                return PatriciaTreeCore::RESULT_STATUS_SUCCESS;

        class Walker : public BasicBypassFunctor
        {
        private:
                typename PatriciaPolicies::GetData     _getData;
                typename PatriciaPolicies::GetKeySize  _getKeySize;
                typename PatriciaPolicies::GetDataSize _getDataSize;

                PatriciaTreeBypassFunctor &functor;
                _NodeAddress               headNode;

        public:
                Walker( PatriciaTreeBypassFunctor &_functor, _NodeAddress _headNode ) :
                        BasicBypassFunctor(), functor(_functor), headNode(_headNode) { }

                PatriciaTreeCore::ResultStatus apply(_NodeAddress node, uint8_t *address, const uint8_t *key)
                {
                        if ( unlikely(node == headNode) )
                                return PatriciaTreeCore::RESULT_STATUS_SUCCESS;

                        PatriciaTreeElement element;
                        element.key      = key;
                        element.keySize  = _getKeySize( address );
                        element.data     = _getData( address );
                        element.dataSize = _getDataSize( address );

                        return functor.apply( element );
                }
        } walker(functor, headNode);

        return bypassTree( walker, headNode, false );
}

