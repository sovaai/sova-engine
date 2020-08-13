/**
 * @file   CrossTree_StandardRaw.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed Dec 17 19:54:21 2008
 * 
 * @brief  Patricia tree data storage with words cross-links -- separate object file for StandardRaw policy.
 * 
 * 
 */

#include "CrossTree.cpp"

/* Force instantiation of the objects needed */
template class CrossTree<PolicyStandardRaw,
			 StorageNodes<StorageDynamic<uint8_t> >,
			 CrossTable<PolicyStandardRaw, StorageDynamic<PolicyStandard::NodeAddress> > >;
template class CrossTree<PolicyStandardRaw,
			 StorageNodes<StorageStatic<uint8_t> >,
			 CrossTable<PolicyStandardRaw, StorageStatic<PolicyStandard::NodeAddress> > >;

