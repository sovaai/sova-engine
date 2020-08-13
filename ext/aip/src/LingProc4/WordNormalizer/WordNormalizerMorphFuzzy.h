#ifndef _WORDNORMALIZERMORPHFUZZY_H_
#define _WORDNORMALIZERMORPHFUZZY_H_

#include <_include/_inttype.h>

#include "WordNormalizerDefault.h"

#include "../WordNormalizerInterface.h"

class UCharSet;

struct FuzzySymbolNativeChecker
{
    WordNormalizerResult notNativeSymbol( uint32_t       &ch,
                                          size_t          wordIdx,
                                          const char*     /* word */,
                                          size_t          wordSize,
                                          const UCharSet* /* charset */,
                                          uint32_t        /* flags */ )
    {
        if ( ch == static_cast<uint32_t>( '$' ) && wordIdx == wordSize )
            return NORMALIZE_SUCCESS;
        return NORMALIZE_ERROR_NOT_NATIVE;
    }
};

/* Default word normalizer */

struct FuzzyWordNormalizerPolicy
{
    typedef DefaultWordPreProcessor    WordPreProcessor;
    typedef FuzzySymbolNativeChecker   SymbolNativeChecker;
    typedef DefaultSymbolPostProcessor SymbolPostProcessor;
    typedef DefaultSymbolCopier        SymbolCopier;
};

#endif /* _WORDNORMALIZERMORPHFUZZY_H_ */

