/**
 * @file   CrossTree.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed Apr 25 01:38:40 2008
 * 
 * @brief  Patricia tree data storage with words cross-links (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include "CrossTree.h"

#include "PolicyStandard.h"
#include "PolicyStandardRaw.h"
#include "PolicyCompact.h"

#include "StorageNodes.h"
#include "StorageStatic.h"
#include "StorageDynamic.h"
#include "CrossTable.h"

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
CrossTree() :
	CrossTreeInterface(),
	PatriciaTreeOverlay<PatriciaPolicies, DataStorage>(),
	crosslinks()
{
}

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
~CrossTree()
{
}

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
PatriciaTreeCore::ResultStatus
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
create()
{
	return clear();
}

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
PatriciaTreeCore::ResultStatus
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
cleanup()
{
	return clear();
}

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
PatriciaTreeCore::ResultStatus
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
clear()
{
	crosslinks.dataClear();

	/* Clear the PatriciaTreeOverlay class first */
	return PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::clear();
}

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
PatriciaTreeCore::ResultStatus
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
append(const uint8_t *key, const uint8_t **data, size_t dataSize)
{
	size_t       keyLength;
	size_t       nodeKeyLength;
	size_t       commonBitsCount;
	_NodeAddress currentNode;
	_NodeAddress newNode;
        
        for ( keyLength = 0; key[keyLength] != '\0'; keyLength++ );
        
	PatriciaTreeCore::ResultStatus status =
		PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::
		append(key, keyLength, data, dataSize, commonBitsCount, currentNode, nodeKeyLength, newNode);

	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;
	
	/* Search for the backward crosslinks */
	if ( commonBitsCount < 8 )
		return status;

	/* Adding the word which can contain crosslink to already existing node */

	bool   forceRecount           = false;
	size_t currentCommonBitsCount = commonBitsCount;
	for (;;)
	{
		uint8_t *nodeAddress = DataStorage::getDataBuffer() + currentNode;
		uint8_t *nodeKey     =
			PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::_getKey(nodeAddress);
		if (forceRecount)
			currentCommonBitsCount =
				PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::_commonPrefixLength(key, nodeAddress);
			
		if ( nodeKey[currentCommonBitsCount >> 3] == static_cast<uint8_t>('\0') )
		{
			/* The node key found is a part of the key given */
			status = addCrosslink(newNode, currentNode);
			if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
				return status;
		}
		else if ( _checkCrosslink(nodeAddress) )
		{
			/* Move along a crosslink if any */
			currentNode  = crosslinks.getCrosslink(currentNode);
			forceRecount = true;
			continue;
		}
		break;
	}

	/* Adding the word which can be the crosslink to already existing node */

	/* Find the node containing the shortest prefix got */
	typename PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::LookupState state;
	this->lookup(key, keyLength << 3, &state);

	_NodeAddress node = state.currentNode;
	
	CrossBypassFunctor functor(this, newNode, keyLength);
	PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::bypassTree( functor,
									node,
									state.previousNode == state.currentNode );
	
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
PatriciaTreeCore::ResultStatus
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
addBackCrosslink( _NodeAddress   node,
		  _NodeAddress   link,
		  size_t         nodeKeyLength,
		  uint8_t       *linkAddress,
		  const uint8_t */* linkKey */ )
{
	if (node == link)
		return PatriciaTreeCore::RESULT_STATUS_SUCCESS;

	if ( !_checkCrosslink(linkAddress) )
	{
		/* The node does not contain any link yet, link it to the root node */
		PatriciaTreeCore::ResultStatus status = addCrosslink(link, node);
		if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
			return status;
	}
	else
	{
		/* The node is already linked: inject the new link */

		_NodeAddress  crossNode    = crosslinks.getCrosslink(link);
		uint8_t      *crossAddress = DataStorage::getDataBuffer() + crossNode;

		if (nodeKeyLength > PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::_getKeySize(crossAddress) )
		{
			/* Link the old link to the current if not yet */
			uint8_t *nodeAddress = DataStorage::getDataBuffer() + node;
			if ( !_checkCrosslink(nodeAddress) )
			{
				PatriciaTreeCore::ResultStatus status = addCrosslink(node, crossNode);
				if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
					return status;
			}

			/* Modify the old link */
			crosslinks.updateCrosslink(link, node);
		}
	}

	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
PatriciaTreeCore::ResultStatus
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
addCrosslink(_NodeAddress node, _NodeAddress link)
{
	/* Append new entry */
	PatriciaTreeCore::ResultStatus status = crosslinks.addCrosslink(node, link);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;

	/* Mark the crosslink availability in node data */
	_setCrosslink(DataStorage::getDataBuffer() + node);
	
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
PatriciaTreeCore::ResultStatus
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
lookupAll(const uint8_t *key, CrossPathFunctor &functor) const
{
	/* Calculate bits count in a key given */
	size_t keyBitsCount = 8;
	for (size_t i = 0; key[i] != static_cast<uint8_t>('\0'); i++)
		keyBitsCount += 8; 

	/* Find out the node, which is stored in the tree */
	_NodeAddress   node = PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::lookup(key, keyBitsCount, 0);
        
        /* Nothing found */
        if ( node == 0 )
                return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
        
	bool traceOn = false;
	for (;;)
	{
		uint8_t       *address    = DataStorage::getDataBuffer() + node;
		const uint8_t *currentKey =
			PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::_getKey(address);

		bool isExactMatch = false;
		if ( !traceOn )
		{
			size_t keyLength       =
				PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::_getKeySize(address);
			size_t commonBitsCount =
				PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::_commonPrefixLength(key, address);

			if ( (commonBitsCount >> 3) >= keyLength )
				traceOn = true;
			if ( commonBitsCount == keyBitsCount )
				isExactMatch = true;
		}
		
		if ( traceOn )
		{
			const uint8_t *currentData     =
				PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::_getData(address);
			size_t         currentDataSize =
				PatriciaTreeOverlay<PatriciaPolicies, DataStorage>::_getDataSize(address);
			if ( functor.nextPart(currentKey, currentData, currentDataSize, isExactMatch) != 0 )
				break;
		}

		if ( _checkCrosslink(address) )
			node = crosslinks.getCrosslink(node);
		else
			break;
	}

	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

struct BestMatchFunctor : public CrossPathFunctor
{
	BestMatchFunctor() : foundData(0), foundDataSize(0) { }
	~BestMatchFunctor() { }
	
	int nextPart(const uint8_t *part, const uint8_t *data, size_t dataSize, bool isExactMatch)
	{
		foundPart       = part;
		foundData       = data;
		foundDataSize   = dataSize;
		foundMatchExact = isExactMatch;
		return 1;
	}

	const uint8_t *foundPart;
	const uint8_t *foundData;
	size_t         foundDataSize;
	bool           foundMatchExact;
};

template<typename PatriciaPolicies, typename DataStorage, typename CrossStorage>
PatriciaTreeCore::ResultStatus
CrossTree<PatriciaPolicies, DataStorage, CrossStorage>::
lookupBest(const uint8_t **key, const uint8_t **resultData, size_t *dataSize, bool *isExactMatch) const
{
	BestMatchFunctor               functor;
	PatriciaTreeCore::ResultStatus status = lookupAll(*key, functor);
	if ( unlikely( status != PatriciaTreeCore::RESULT_STATUS_SUCCESS ) )
		return status;

	if (functor.foundData == 0)
		return PatriciaTreeCore::RESULT_STATUS_NOT_FOUND;

	*resultData   = functor.foundData;
	*dataSize     = functor.foundDataSize;
	*key          = functor.foundPart;
	*isExactMatch = functor.foundMatchExact;
	
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

