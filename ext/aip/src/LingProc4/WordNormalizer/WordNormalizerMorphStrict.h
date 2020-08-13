#ifndef _WORDNORMALIZERMORPHSTRICT_H_
#define _WORDNORMALIZERMORPHSTRICT_H_

#include <_include/_inttype.h>

#include "WordNormalizerDefault.h"

#include "../UCharSet/UCharSet.h"
#include "../WordNormalizerInterface.h"

struct StrictSymbolPostProcessor
{
    WordNormalizerResult postProcessSymbol( uint32_t       &ch,
                                            size_t          /* wordIdx */,
                                            const char*     /* word */,
                                            size_t          /* wordSize */,
                                            const UCharSet *charset,
                                            uint32_t        /* flags */ )
    {
        /* Force lowercase for dictionary matching */
        ch = charset->ToLower( ch );
        return NORMALIZE_SUCCESS;
    }
};

/* Default word normalizer */

struct StrictWordNormalizerPolicy
{
    typedef DefaultWordPreProcessor    WordPreProcessor;
    typedef DefaultSymbolNativeChecker SymbolNativeChecker;
    typedef StrictSymbolPostProcessor  SymbolPostProcessor;
    typedef DefaultSymbolCopier        SymbolCopier;
};

#endif /* _WORDNORMALIZERMORPHSTRICT_H_ */

