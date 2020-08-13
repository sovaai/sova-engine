#ifndef _WORDNORMALIZERMORPHTILDE_H_
#define _WORDNORMALIZERMORPHTILDE_H_

#include <_include/_inttype.h>

#include "WordNormalizerDefault.h"

#include "../WordNormalizerInterface.h"

class UCharSet;

struct TildeSymbolNativeChecker
{
    WordNormalizerResult notNativeSymbol( uint32_t       &ch,
                                          size_t          wordIdx,
                                          const char*     /* word */,
                                          size_t          wordSize,
                                          const UCharSet* /* charset */,
                                          uint32_t        flags )
    {
        if ( ch      == static_cast<uint32_t>('~') &&
             wordIdx == wordSize                   &&
             (flags &  MORPH_FLAG_PROCESS_TILDE) )
            return NORMALIZE_SUCCESS;
        return NORMALIZE_ERROR_NOT_NATIVE;
    }
};

/* Default word normalizer */

struct TildeWordNormalizerPolicy
{
    typedef DefaultWordPreProcessor    WordPreProcessor;
    typedef TildeSymbolNativeChecker   SymbolNativeChecker;
    typedef DefaultSymbolPostProcessor SymbolPostProcessor;
    typedef DefaultSymbolCopier        SymbolCopier;
};

#endif /* _WORDNORMALIZERMORPHTILDE_H_ */

