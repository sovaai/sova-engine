#ifndef _WORDNORMALIZERARABIC_H_
#define _WORDNORMALIZERARABIC_H_

#include <_include/_inttype.h>

#include "WordNormalizerMorphFuzzy.h"
#include "WordNormalizerMorphStrict.h"

#include "../Constants.h"
#include "../IndirectTable.h"
#include "../WordNormalizerInterface.h"

class UCharSet;

/* Implementation */

class ArabicSymbolPostProcessor
{
public:
    ArabicSymbolPostProcessor();
    
public:
    WordNormalizerResult postProcessSymbol( uint32_t       &ch,
                                            size_t          wordIdx,
                                            const char     *word,
                                            size_t          wordSize,
                                            const UCharSet *charset,
                                            uint32_t        flags );
    
private:
    IndirectTable table;
};

class ArabicSymbolCopier
{
public:
    ArabicSymbolCopier();
    
public:
    WordNormalizerResult copySymbol( uint32_t  ch,
                                     char     *outBuffer,
                                     size_t    outBufferSize,
                                     size_t   &outBufferIdx,
                                     uint32_t  flags );

private:
    IndirectTable table;
    IndirectTable seqs;
    uint32_t      prevChar;
    size_t        seqCount;
};

/* Policy */

struct FuzzyArabicWordNormalizerPolicy
{
    typedef DefaultWordPreProcessor    WordPreProcessor;
    typedef FuzzySymbolNativeChecker   SymbolNativeChecker;
    typedef ArabicSymbolPostProcessor  SymbolPostProcessor;
    typedef ArabicSymbolCopier         SymbolCopier;
};

struct StrictArabicWordNormalizerPolicy
{
    typedef DefaultWordPreProcessor     WordPreProcessor;
    typedef DefaultSymbolNativeChecker SymbolNativeChecker;
    typedef ArabicSymbolPostProcessor  SymbolPostProcessor;
    typedef ArabicSymbolCopier         SymbolCopier;
};

#endif /* _WORDNORMALIZERARABIC_H_ */

