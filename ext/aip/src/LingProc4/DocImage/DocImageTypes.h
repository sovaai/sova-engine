#ifndef _DOCIMAGETYPES_H_
#define _DOCIMAGETYPES_H_

#include <_include/_inttype.h>

#include <lib/aptl/avector.h>

struct DocImageTypes
{
    struct DocImageWord
    {
        uint32_t imgIndex;
        uint16_t imgLength;
        uint16_t attrs;
        uint32_t chainsIndex;
        uint32_t chainsCount;
    };
  
    typedef avector<LEXID>              ImageArray;
    typedef avector<DocImageWord>       WordsArray;
    typedef avector<LEXID>              CompoundLexPool;
    typedef avector<StrictWordFormsSet> FormsSetsArray;
    
    struct DecompositionChain
    {
        size_t chainOffset;
        size_t chainLength;
    };

    typedef avector<DecompositionChain> ChainsPool;
};

#endif /* _DOCIMAGETYPES_H_ */

