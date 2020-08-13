/**
 * @file   PatriciaTreeOverlay_Compact.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed Dec 17 19:47:18 2008
 * 
 * @brief  Patricia tree data storage -- separate object file for Compact policy.
 * 
 * 
 */

#include "PatriciaTreeOverlay.cpp"

/* Force instantiation of the objects needed */
template class PatriciaTreeOverlay<PolicyCompact,     StorageNodes<StorageStatic<uint8_t> > >;
template class PatriciaTreeOverlay<PolicyCompact,     StorageNodes<StorageDynamic<uint8_t> > >;

