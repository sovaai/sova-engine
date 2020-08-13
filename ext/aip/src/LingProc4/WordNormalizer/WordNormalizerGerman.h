#ifndef _WORDNORMALIZERGERMAN_H_
#define _WORDNORMALIZERGERMAN_H_

#include <_include/_inttype.h>

#include "WordNormalizerMorphFuzzy.h"
#include "WordNormalizerMorphStrict.h"

#include "../Constants.h"
#include "../IndirectTable.h"
#include "../WordNormalizerInterface.h"

class UCharSet;

/* Implementation */

class GermanSymbolCopier
{
public:
    GermanSymbolCopier();
    
public:
    WordNormalizerResult copySymbol( uint32_t  ch,
                                     char     *outBuffer,
                                     size_t    outBufferSize,
                                     size_t   &outBufferIdx,
                                     uint32_t  flags );

private:
    IndirectTable         table;
    avector<const char *> mappings;
};

/* Policy */

struct FuzzyGermanWordNormalizerPolicy
{
    typedef DefaultWordPreProcessor    WordPreProcessor;
    typedef FuzzySymbolNativeChecker   SymbolNativeChecker;
    typedef DefaultSymbolPostProcessor SymbolPostProcessor;
    typedef GermanSymbolCopier         SymbolCopier;
};

struct StrictGermanWordNormalizerPolicy
{
    typedef DefaultWordPreProcessor    WordPreProcessor;
    typedef DefaultSymbolNativeChecker SymbolNativeChecker;
    typedef StrictSymbolPostProcessor  SymbolPostProcessor;
    typedef GermanSymbolCopier         SymbolCopier;
};

#endif /* _WORDNORMALIZERGERMAN_H_ */

