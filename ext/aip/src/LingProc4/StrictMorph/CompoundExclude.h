/**
 * @file   CompoundExclude.h
 * @author swizard <me@swizard.info>
 * @date   Mon Feb 16 22:01:44 2009
 * 
 * @brief  Compounds decomposition exclude dictionary.
 * 
 * 
 */
#ifndef _COMPOUNDEXCLUDE_H_
#define _COMPOUNDEXCLUDE_H_

#include <lib/aptl/OpenAddressingDict.h>

typedef uint32_t ExcludePatriciaLink;
typedef uint32_t ExcludeChainOffset;

typedef OpenAddressingDict< ExcludePatriciaLink,
                            ExcludeChainOffset,
                            2048,
                            256 > ExcludeIndex;

typedef const uint64_t *ExcludeChain;

#endif /* _COMPOUNDEXCLUDE_H_ */

