#ifndef _WORDNORMALIZER_H_
#define _WORDNORMALIZER_H_

#include <_include/cc_compat.h>
#include <_include/_inttype.h>
#include <_include/_string.h>

#include <unicode/utf8.h>

#include "UCharSet/UCharSet.h"

#include "SimpleMorph/LPMorphCommon.h"

#include "WordNormalizerInterface.h"

/* The class with implementation */
template< typename WordNormalizerPolicy >
class CustomWordNormalizer :
    public WordNormalizer,
    public WordNormalizerPolicy::WordPreProcessor,
    public WordNormalizerPolicy::SymbolNativeChecker,
    public WordNormalizerPolicy::SymbolPostProcessor,
    public WordNormalizerPolicy::SymbolCopier
{
public:
    CustomWordNormalizer() :
        WordNormalizer(),
        WordNormalizerPolicy::WordPreProcessor(),
        WordNormalizerPolicy::SymbolNativeChecker(),
        WordNormalizerPolicy::SymbolPostProcessor(),
        WordNormalizerPolicy::SymbolCopier()
    {
    }

    ~CustomWordNormalizer() { }

public:
    WordNormalizerResult normalizeWord( WordNormalizerFunctor &functor,
                                        uint32_t               flags,
                                        const char            *word,
                                        size_t                 wordSize = static_cast<size_t>(-1) )
    {
        /* Check if some parameters are invalid */
        if ( unlikely(charset == 0) )
            return NORMALIZE_ERROR_PARAMETER;
        
        if ( unlikely(word == 0) )
            return NORMALIZE_ERROR_PARAMETER;

        if ( unlikely(flags & (MORPH_FLAG_INVALID | MORPH_FLAG_USE_DEFAULT)) )
            return NORMALIZE_ERROR_FLAGS;
        
        /* Create local copy of the word performing checks and conversions */
        if ( wordSize == static_cast<size_t>(-1) )
            wordSize = strlen(word);

        size_t outBufferSize = wordSize * 4 + 4;
        
        char *outBuffer = functor.getResultBuffer( outBufferSize );
        if ( unlikely(outBuffer == 0) )
            return NORMALIZE_ERROR_BUFFER;

        WordNormalizerResult status =
            WordNormalizerPolicy::WordPreProcessor::preProcessWord( word, wordSize, charset, flags );
        if ( unlikely(status != NORMALIZE_SUCCESS) )
            return status;
        
        /* Perform normalization */
        uint32_t ch;
        size_t   wordIdx = 0;
        size_t   outIdx  = 0;
        size_t   upper      = 0;
        size_t   lower      = 0;
        bool     upperFirst = false;
        
        while ( wordIdx < wordSize && outIdx < outBufferSize )
        {
            U8_NEXT_UNSAFE( word, wordIdx, ch );
            if ( ch == static_cast<uint32_t>( '\0' ) )
                break;

            if ( flags & MORPH_FLAG_REM_IGNORE && charset->IsWordIgnore( ch ) )
                continue;

            /* Process capitalization if any */
            if ( charset->IsLower( ch ) )
                lower++;
            else if ( charset->IsUpper( ch ) )
            {
                if ( !lower && !upper )
                    upperFirst = true;
                upper++;
            }

            /* Perform conversions */
            if ( flags & MORPH_FLAG_CASE_INSENSITIVE )
                ch = charset->ToLower( ch );
            if ( flags & MORPH_FLAG_LWR_ACCENT )
                ch = charset->ToLoAccent( ch );
            if ( flags & MORPH_FLAG_LWR_PUNCT )
                ch = charset->ToLoPunct( ch );
            
            /* Check if native */
            if ( flags & MORPH_FLAG_NATIVE_ONLY )
                if ( !charset->IsLangChar( ch ) )
                {
                    status = WordNormalizerPolicy::SymbolNativeChecker::notNativeSymbol( ch, wordIdx, word, wordSize, charset, flags );
                    if ( unlikely(status != NORMALIZE_SUCCESS) )
                        return status;
                }
                
            /* Postprocess symbol */
            status = WordNormalizerPolicy::SymbolPostProcessor::postProcessSymbol( ch, wordIdx, word, wordSize, charset, flags );
            if ( unlikely(status != NORMALIZE_SUCCESS) )
                return status;
            
            status = WordNormalizerPolicy::SymbolCopier::copySymbol( ch, outBuffer, outBufferSize, outIdx, flags );
            if ( unlikely(status != NORMALIZE_SUCCESS) )
                return status;                                
        }

        if ( outIdx < outBufferSize )
            outBuffer[ outIdx ] = '\0';

        size_t length = outIdx;

        /* Detect capitalization scheme */
        WordNormalizerCaps caps;
        if ( upperFirst && lower )
            caps = NORMALIZE_CAPS_TITLE_CASE;
        else if ( upper > lower )
            caps = NORMALIZE_CAPS_UPPER_CASE;
        else if ( lower != 0 )
            caps = NORMALIZE_CAPS_LOWER_CASE;
        else
            caps = NORMALIZE_CAPS_NO_CASE;
    
        return functor.nextResult( outBuffer, length, caps );
    }
};

#endif /* _WORDNORMALIZER_H_ */

