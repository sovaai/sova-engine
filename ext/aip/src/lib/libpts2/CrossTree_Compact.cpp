/**
 * @file   CrossTree_Compact.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed Dec 17 19:54:21 2008
 * 
 * @brief  Patricia tree data storage with words cross-links -- separate object file for Compact policy.
 * 
 * 
 */

#include "CrossTree.cpp"

/* Force instantiation of the objects needed */
template class CrossTree<PolicyCompact,
			 StorageNodes<StorageDynamic<uint8_t> >,
			 CrossTable<PolicyCompact, StorageDynamic<PolicyCompact::NodeAddress> > >;
template class CrossTree<PolicyCompact,
			 StorageNodes<StorageStatic<uint8_t> >,
			 CrossTable<PolicyCompact, StorageStatic<PolicyCompact::NodeAddress> > >;

