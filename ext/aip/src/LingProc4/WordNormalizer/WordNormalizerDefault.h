#ifndef _WORDNORMALIZERDEFAULT_H_
#define _WORDNORMALIZERDEFAULT_H_

#include <_include/_inttype.h>

#include <unicode/utf8.h>

#include "../WordNormalizerInterface.h"

class UCharSet;

struct DefaultWordPreProcessor
{
    WordNormalizerResult preProcessWord( const char     *&/* word */,
                                         size_t          &/* wordSize */,
                                         const UCharSet*  /* charset */,
                                         uint32_t         /* flags */ )
    {
        return NORMALIZE_SUCCESS;
    }
};

struct DefaultSymbolNativeChecker
{
    WordNormalizerResult notNativeSymbol( uint32_t       &/* ch */,
                                          size_t          /* wordIdx */,
                                          const char*     /* word */,
                                          size_t          /* wordSize */,
                                          const UCharSet* /* charset */,
                                          uint32_t        /* flags */ ) 
    {
        return NORMALIZE_ERROR_NOT_NATIVE;
    }
};

struct DefaultSymbolPostProcessor
{
    WordNormalizerResult postProcessSymbol( uint32_t       &/* ch */,
                                            size_t          /* wordIdx */,
                                            const char*     /* word */,
                                            size_t          /* wordSize */,
                                            const UCharSet* /* charset */,
                                            uint32_t        /* flags */ )
    {
        return NORMALIZE_SUCCESS;
    }
};

struct DefaultSymbolCopier
{
    WordNormalizerResult copySymbol( uint32_t  ch,
                                     char     *outBuffer,
                                     size_t    /* outBufferSize */,
                                     size_t   &outBufferIdx,
                                     uint32_t  /* flags */ )
    {
        U8_APPEND_UNSAFE( outBuffer, outBufferIdx, ch );
        return NORMALIZE_SUCCESS;
    }
};

/* Default word normalizer */

struct DefaultWordNormalizerPolicy
{
    typedef DefaultWordPreProcessor    WordPreProcessor;
    typedef DefaultSymbolNativeChecker SymbolNativeChecker;
    typedef DefaultSymbolPostProcessor SymbolPostProcessor;
    typedef DefaultSymbolCopier        SymbolCopier;
};

#endif /* _WORDNORMALIZERDEFAULT_H_ */

