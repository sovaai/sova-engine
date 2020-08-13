/**
 * @file   CrossTree_Standard.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed Dec 17 19:54:21 2008
 * 
 * @brief  Patricia tree data storage with words cross-links -- separate object file for Standard policy.
 * 
 * 
 */

#include "CrossTree.cpp"

/* Force instantiation of the objects needed */
template class CrossTree<PolicyStandard,
			 StorageNodes<StorageDynamic<uint8_t> >,
			 CrossTable<PolicyStandard, StorageDynamic<PolicyStandard::NodeAddress> > >;
template class CrossTree<PolicyStandard,
			 StorageNodes<StorageStatic<uint8_t> >,
			 CrossTable<PolicyStandard, StorageStatic<PolicyStandard::NodeAddress> > >;

