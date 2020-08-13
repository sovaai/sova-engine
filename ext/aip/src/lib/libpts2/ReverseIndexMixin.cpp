/**
 * @file   ReverseIndexMixin.cpp
 * @author swizard <me@swizard.info>
 * @date   Tue Sep  2 15:48:51 2008
 * 
 * @brief  A mixin for PatriciaTree with reverse index feature (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include "PolicyCompact.h"
#include "PolicyStandard.h"
#include "StorageStatic.h"
#include "StorageDynamic.h"
#include "StorageNodes.h"
#include "PatriciaTreeOverlay.h"
#include "CrossTable.h"
#include "CrossTree.h"

#include "ReverseIndexMixin.h"

template<typename DataStorage, typename PatriciaType>
ReverseIndexMixin<DataStorage, PatriciaType>::
ReverseIndexMixin() :
        PatriciaType(), 
        DataStorage()
{
}

template<typename DataStorage, typename PatriciaType>
ReverseIndexMixin<DataStorage, PatriciaType>::
~ReverseIndexMixin()
{
}

template<typename DataStorage, typename PatriciaType>
PatriciaTreeCore::ResultStatus
ReverseIndexMixin<DataStorage, PatriciaType>::
indexedAppend( const uint8_t  *key,
               uint32_t       *id )
{
        uint32_t       newId = DataStorage::getDataSize() + 1;
        const uint8_t *data  = reinterpret_cast<const uint8_t *>( &newId );
                
        PatriciaTreeCore::ResultStatus status = PatriciaType::append(key, &data, sizeof(uint32_t));
        if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
        {
                if ( likely(status == PatriciaTreeCore::RESULT_STATUS_DUPLICATE_KEY) )
                {
                        *id = *( reinterpret_cast<const uint32_t *>(data) );
                        return PatriciaTreeCore::RESULT_STATUS_DUPLICATE_KEY;
                }
                return status;
        }
        
        uint32_t link = PatriciaType::dataAddressToLink( data );
        status = addToIndex(link);
        if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
                return status;

        *id = newId;
        
        return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

template<typename DataStorage, typename PatriciaType>
PatriciaTreeCore::ResultStatus
ReverseIndexMixin<DataStorage, PatriciaType>::
getFromIndex(uint32_t id, uint32_t *resultLink) const
{
        if ( unlikely(id == 0 || id > DataStorage::getDataSize()) )
                return PatriciaTreeCore::RESULT_STATUS_NOT_FOUND;

        *resultLink = *(DataStorage::getDataBuffer() + id - 1);
        return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

template<typename DataStorage, typename PatriciaType>
PatriciaTreeCore::ResultStatus
ReverseIndexMixin<DataStorage, PatriciaType>::
addToIndex(uint32_t link)
{
        PatriciaTreeCore::ResultStatus status = DataStorage::setDataSize( DataStorage::getDataSize() + 1 );
        if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
                return status;

        *(DataStorage::getDataBuffer() + DataStorage::getDataSize() - 1) = link;
        return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

/* Force instantiation of the objects needed */

template class ReverseIndexMixin< StorageStatic<uint32_t>,
                                  PatriciaTreeOverlay<PolicyCompact,  StorageNodes<StorageStatic<uint8_t> > > >;
template class ReverseIndexMixin< StorageDynamic<uint32_t>,
                                  PatriciaTreeOverlay<PolicyStandard, StorageNodes<StorageDynamic<uint8_t> > > >;
template class ReverseIndexMixin< StorageDynamic<uint32_t>,
                                  PatriciaTreeOverlay<PolicyCompact,  StorageNodes<StorageDynamic<uint8_t> > > >;
template class ReverseIndexMixin< StorageStatic<uint32_t>,
                                  PatriciaTreeOverlay<PolicyStandard, StorageNodes<StorageStatic<uint8_t> > > >;


template class ReverseIndexMixin< StorageDynamic<uint32_t>,
                                  CrossTree<PolicyStandard,
                                            StorageNodes<StorageDynamic<uint8_t> >,
                                            CrossTable<PolicyStandard, StorageDynamic<PolicyStandard::NodeAddress> > > >;
template class ReverseIndexMixin< StorageDynamic<uint32_t>,
                                  CrossTree<PolicyCompact,
                                            StorageNodes<StorageDynamic<uint8_t> >,
                                            CrossTable<PolicyCompact, StorageDynamic<PolicyCompact::NodeAddress> > > >;
template class ReverseIndexMixin< StorageStatic<uint32_t>,
                                  CrossTree<PolicyStandard,
                                            StorageNodes<StorageStatic<uint8_t> >,
                                            CrossTable<PolicyStandard, StorageStatic<PolicyStandard::NodeAddress> > > >;
template class ReverseIndexMixin< StorageStatic<uint32_t>,
                                  CrossTree<PolicyCompact,
                                            StorageNodes<StorageStatic<uint8_t> >,
                                            CrossTable<PolicyCompact, StorageStatic<PolicyCompact::NodeAddress> > > >;

