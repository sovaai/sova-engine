/**
 * @file   PatriciaTree.h
 * @author swizard <me@swizard.info>
 * @date   Thu Apr 29 16:27:31 2008
 * 
 * @brief  Aggregate functionality for patricia tree data storage (implementation).
 * 
 * 
 */

#include "PatriciaTree.h"

#include "PatriciaTreeOverlay.h"
#include "CrossTree.h"

#include "PolicyStandard.h"
#include "PolicyStandardRaw.h"
#include "PolicyCompact.h"
#include "PolicyGeneric.h"

#include "StorageNodes.h"
#include "StorageStatic.h"
#include "StorageDynamic.h"
#include "CrossTable.h"
#include "ReverseIndexMixin.h"

PatriciaTree::PatriciaTree() :
	CrossTreeInterface(),
	StorageInterface(),
	notCreated( true ),
	flags( TREE_DEFAULT ),
	majorVersion( patriciaTreeMajorVersion ),
	minorVersion( patriciaTreeMinorVersion ),
	shadow( 0 )
{
}

PatriciaTree::~PatriciaTree()
{
	if ( !notCreated && shadow != 0 )
		delete shadow;
}

PatriciaTreeCore::ResultStatus PatriciaTree::create(uint16_t flags)
{
	PatriciaTreeCore::ResultStatus status = processFlags(flags);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;
	
	/* Try to create the shadow */
	status = shadow->create();
	if ( unlikely( status != PatriciaTreeCore::RESULT_STATUS_SUCCESS ) )
	{
		delete shadow;
		shadow = 0;
		return status;
	}

	notCreated  = false;
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

PatriciaTreeCore::ResultStatus PatriciaTree::processFlags(uint16_t flags)
{
	/* Check if already created */
	if ( unlikely( !notCreated ) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_ALREADY_CREATED;
			
	if ( flags == TREE_DEFAULT )
		flags = TREE_STANDARD | TREE_REVERSE_INDEX;

	/* Check if invalid flags combination is given */
	if ( unlikely( (flags & TREE_STANDARD     && flags & TREE_COMPACT)      ||
                       (flags & TREE_STANDARD     && flags & TREE_STANDARD_RAW) ||
                       (flags & TREE_STANDARD     && flags & TREE_GENERIC)      ||
                       (flags & TREE_COMPACT      && flags & TREE_STANDARD_RAW) ||
                       (flags & TREE_COMPACT      && flags & TREE_GENERIC)      ||
                       (flags & TREE_STANDARD_RAW && flags & TREE_GENERIC) ) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_INVALID_FLAGS;

	/* Choose appropriate class instance for shadow */
	if      ( flags   & TREE_STANDARD       &&
		  !(flags & TREE_CROSSLINKS)    &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new PatriciaTreeOverlay<PolicyStandard, StorageNodes<StorageDynamic<uint8_t> > >;
 	else if ( flags   & TREE_COMPACT        &&
		  !(flags & TREE_CROSSLINKS)    &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new PatriciaTreeOverlay<PolicyCompact, StorageNodes<StorageDynamic<uint8_t> > >;
 	else if ( flags   & TREE_STANDARD_RAW   &&
		  !(flags & TREE_CROSSLINKS)    &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new PatriciaTreeOverlay<PolicyStandardRaw, StorageNodes<StorageDynamic<uint8_t> > >;
 	else if ( flags   & TREE_GENERIC        &&
		  !(flags & TREE_CROSSLINKS)    &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new PatriciaTreeOverlay<PolicyGeneric, StorageNodes<StorageDynamic<uint8_t> > >;
 	else if ( flags   & TREE_STANDARD       &&
		  flags   & TREE_CROSSLINKS     &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new CrossTree<PolicyStandard,
                StorageNodes<StorageDynamic<uint8_t> >,
                CrossTable<PolicyStandard, StorageDynamic<PolicyStandard::NodeAddress> > >;
 	else if ( flags   & TREE_COMPACT        &&
		  flags   & TREE_CROSSLINKS     &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new CrossTree<PolicyCompact,
				       StorageNodes<StorageDynamic<uint8_t> >,
				       CrossTable<PolicyCompact, StorageDynamic<PolicyCompact::NodeAddress> > >;
 	else if ( flags   & TREE_STANDARD_RAW   &&
		  flags   & TREE_CROSSLINKS     &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new CrossTree<PolicyStandardRaw,
                StorageNodes<StorageDynamic<uint8_t> >,
                CrossTable<PolicyStandardRaw, StorageDynamic<PolicyCompact::NodeAddress> > >;
	else if ( flags   & TREE_STANDARD       &&
		  !(flags & TREE_CROSSLINKS)    &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  (flags & TREE_READ_ONLY) )
		shadow = new PatriciaTreeOverlay<PolicyStandard, StorageNodes<StorageStatic<uint8_t> > >;
 	else if ( flags   & TREE_COMPACT        &&
		  !(flags & TREE_CROSSLINKS)    &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  (flags & TREE_READ_ONLY) )
		shadow = new PatriciaTreeOverlay<PolicyCompact, StorageNodes<StorageStatic<uint8_t> > >;
 	else if ( flags   & TREE_STANDARD_RAW   &&
		  !(flags & TREE_CROSSLINKS)    &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  (flags & TREE_READ_ONLY) )
		shadow = new PatriciaTreeOverlay<PolicyStandardRaw, StorageNodes<StorageStatic<uint8_t> > >;
 	else if ( flags   & TREE_GENERIC        &&
		  !(flags & TREE_CROSSLINKS)    &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  (flags & TREE_READ_ONLY) )
		shadow = new PatriciaTreeOverlay<PolicyGeneric, StorageNodes<StorageStatic<uint8_t> > >;
 	else if ( flags   & TREE_STANDARD       &&
		  flags   & TREE_CROSSLINKS     &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  (flags & TREE_READ_ONLY) )
		shadow = new CrossTree<PolicyStandard,
				       StorageNodes<StorageStatic<uint8_t> >,
		                       CrossTable<PolicyStandard, StorageStatic<PolicyStandard::NodeAddress> > >;
 	else if ( flags   & TREE_COMPACT        &&
		  flags   & TREE_CROSSLINKS     &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  (flags & TREE_READ_ONLY) )
		shadow = new CrossTree<PolicyCompact,
                StorageNodes<StorageStatic<uint8_t> >,
                CrossTable<PolicyCompact, StorageStatic<PolicyCompact::NodeAddress> > >;
 	else if ( flags   & TREE_STANDARD_RAW   &&
		  flags   & TREE_CROSSLINKS     &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  (flags & TREE_READ_ONLY) )
		shadow = new CrossTree<PolicyStandardRaw,
				       StorageNodes<StorageStatic<uint8_t> >,
				       CrossTable<PolicyStandardRaw, StorageStatic<PolicyCompact::NodeAddress> > >;
        /* Reversed index mixin */
        else if ( flags   & TREE_STANDARD       &&
		  !(flags & TREE_CROSSLINKS)    &&
		  (flags & TREE_REVERSE_INDEX)  &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new ReverseIndexMixin<StorageDynamic<uint32_t>,
                PatriciaTreeOverlay<PolicyStandard, StorageNodes<StorageDynamic<uint8_t> > > >;
 	else if ( flags   & TREE_COMPACT        &&
		  !(flags & TREE_CROSSLINKS)    &&
		  (flags & TREE_REVERSE_INDEX)  &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new ReverseIndexMixin<StorageDynamic<uint32_t>,
                                               PatriciaTreeOverlay<PolicyCompact, StorageNodes<StorageDynamic<uint8_t> > > >;
 	else if ( flags   & TREE_STANDARD       &&
		  flags   & TREE_CROSSLINKS     &&
		  (flags & TREE_REVERSE_INDEX)  &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new ReverseIndexMixin<StorageDynamic<uint32_t>,
                CrossTree<PolicyStandard,
                StorageNodes<StorageDynamic<uint8_t> >,
                CrossTable<PolicyStandard, StorageDynamic<PolicyStandard::NodeAddress> > > >;
 	else if ( flags   & TREE_COMPACT        &&
		  flags   & TREE_CROSSLINKS     &&
		  (flags & TREE_REVERSE_INDEX)  &&
		  !(flags & TREE_READ_ONLY) )
		shadow = new ReverseIndexMixin<StorageDynamic<uint32_t>,
                CrossTree<PolicyCompact,
                StorageNodes<StorageDynamic<uint8_t> >,
                CrossTable<PolicyCompact, StorageDynamic<PolicyCompact::NodeAddress> > > >;
	else if ( flags   & TREE_STANDARD       &&
		  !(flags & TREE_CROSSLINKS)    &&
		  !(flags & TREE_REVERSE_INDEX) &&
		  (flags & TREE_READ_ONLY) )
		shadow = new ReverseIndexMixin<StorageStatic<uint32_t>,
                PatriciaTreeOverlay<PolicyStandard, StorageNodes<StorageStatic<uint8_t> > > >;
 	else if ( flags   & TREE_COMPACT        &&
		  !(flags & TREE_CROSSLINKS)    &&
		  (flags & TREE_REVERSE_INDEX)  &&
		  (flags & TREE_READ_ONLY) )
		shadow = new ReverseIndexMixin<StorageStatic<uint32_t>,
                                               PatriciaTreeOverlay<PolicyCompact, StorageNodes<StorageStatic<uint8_t> > > >;
 	else if ( flags   & TREE_STANDARD       &&
		  flags   & TREE_CROSSLINKS     &&
		  (flags & TREE_REVERSE_INDEX)  &&
		  (flags & TREE_READ_ONLY) )
		shadow = new ReverseIndexMixin<StorageStatic<uint32_t>,
                CrossTree<PolicyStandard,
                StorageNodes<StorageStatic<uint8_t> >,
                CrossTable<PolicyStandard, StorageStatic<PolicyStandard::NodeAddress> > > >;
 	else if ( flags   & TREE_COMPACT        &&
		  flags   & TREE_CROSSLINKS     &&
		  (flags & TREE_REVERSE_INDEX)  &&
		  (flags & TREE_READ_ONLY) )
		shadow = new ReverseIndexMixin<StorageStatic<uint32_t>,
                CrossTree<PolicyCompact,
                StorageNodes<StorageStatic<uint8_t> >,
                CrossTable<PolicyCompact, StorageStatic<PolicyCompact::NodeAddress> > > >;
        
	else
		return PatriciaTreeCore::RESULT_STATUS_ERROR_INVALID_FLAGS;
        
        crossTreeAccessor  = dynamic_cast<CrossTreeInterface *>(shadow);
        rindexTreeAccessor = dynamic_cast<ReversedIndexTreeInterface *>(shadow);
        
	this->flags = flags;
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

PatriciaTreeCore::ResultStatus PatriciaTree::getStorageHandler(PatriciaStorageHandler &handler)
{
	if ( unlikely(notCreated) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_NOT_CREATED;

	PatriciaTreeCore::ResultStatus status = shadow->getStorageHandler( handler );
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;

	getMemoryChunk( handler.chunks[PATRICIA_CHUNK_HEADER] );
		
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

PatriciaTreeCore::ResultStatus PatriciaTree::setStorageHandler(uint16_t flags, PatriciaStorageHandler &handler)
{
	/* Load the header */
	if ( unlikely(setMemoryChunk( handler.chunks[PATRICIA_CHUNK_HEADER] ) != 0) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_LOADING_FAILED;

	/* Check the versions */
	if ( unlikely(majorVersion != patriciaTreeMajorVersion ||
		      minorVersion >  patriciaTreeMinorVersion) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_VERSION_MISMATCH;
	
	/* Check if the data requested is present in the header */
	if ( unlikely( (flags & TREE_CROSSLINKS) && !(this->flags & TREE_CROSSLINKS) ) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_NO_HANDLER_CROSSLINKS;
	if ( unlikely( (flags & TREE_REVERSE_INDEX) && !(this->flags & TREE_REVERSE_INDEX) ) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_NO_HANDLER_REVERSE_INDEX;

	/* Create the tree internals */
	PatriciaTreeCore::ResultStatus status = processFlags(flags);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;

	/* Load and create the shadow */
	status = shadow->setStorageHandler(flags, handler);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
	{
		delete shadow;
		shadow = 0;
		return status;
	}

	notCreated  = false;
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

size_t PatriciaTree::getElementsCount() const
{
        if ( unlikely( notCreated ) )
                return static_cast<size_t>(0);
        
        struct Counter : public PatriciaTreeBypassFunctor
        {
                size_t cnt;
                
                Counter() : PatriciaTreeBypassFunctor(), cnt(0) { }
                
                PatriciaTreeCore::ResultStatus apply( const PatriciaTreeElement & )
                {
                        cnt++;
                        return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
                }
        } counter;
        
        if ( unlikely(bypass(counter) != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
                return 0;
        return counter.cnt;
}
