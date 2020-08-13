#ifndef _WORDNORMALIZERMORPHPUNCT_H_
#define _WORDNORMALIZERMORPHPUNCT_H_

#include <_include/_inttype.h>

#include "WordNormalizerDefault.h"

#include "../WordNormalizerInterface.h"

class UCharSet;

struct PunctSymbolPostProcessor
{
    WordNormalizerResult postProcessSymbol( uint32_t       &ch,
                                            size_t          /* wordIdx */,
                                            const char*     /* word */,
                                            size_t          /* wordSize */,
                                            const UCharSet* charset,
                                            uint32_t        /* flags */ )
    {
        if ( charset->IsPunct( ch ) )
            return NORMALIZE_SUCCESS;
        return NORMALIZE_ERROR_NOT_NATIVE;
    }
};

/* Default word normalizer */

struct PunctWordNormalizerPolicy
{
    typedef DefaultWordPreProcessor    WordPreProcessor;
    typedef DefaultSymbolNativeChecker SymbolNativeChecker;
    typedef PunctSymbolPostProcessor   SymbolPostProcessor;
    typedef DefaultSymbolCopier        SymbolCopier;
};

#endif /* _WORDNORMALIZERMORPHPUNCT_H_ */

