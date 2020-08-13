/**
 * @file   LPMorphNumber.cpp
 * @author swizard <me@swizard.info>
 * @date   Tue Apr  8 23:40:56 2008
 * 
 * @brief  Numerical morphology for LingProc (implementation).
 * 
 * 
 */

#define NUM_MAX_WORD_LEN 63
#define NUM_MAX_VALUE    ((1UL << 28) - 1)

#include <_include/_stdio_.h>
#include <_include/_string.h>
#include <_include/cc_compat.h>

#include "LPMorphNumber.h"

LingProcErrors LPMorphNumber::create(const UCharSet *charset, const LPMorphCreateParams *params)
{
    if ( unlikely(charset == 0 || params == 0) ) 
        return LP_ERROR_EINVAL;

    this->_charset             = charset;
    this->_info.type           = MORPH_TYPE_NUM;
    this->_info.internalNumber = params->internalNumber;
    this->_info.minStemLength  = 1;

    if ( unlikely(params->flags & (MORPH_FLAG_USE_DEFAULT | MORPH_FLAG_INVALID)) )
        return LP_ERROR_MORPH_FLAGS;
        
    this->_info.creationFlags = 0; // No useful flags for number morphology
	
    return LP_OK;
}

size_t LPMorphNumber::processWord( char         *outBuffer,
				   LEXID        &lex,
				   const char   *word,
				   size_t        wordLength )
{
    size_t i    = 0;
    size_t j    = 0;
    size_t size = 0;

    for ( ; i < wordLength; size++ )
    {
        /* check maximum size */
        if ( unlikely(size >= NUM_MAX_WORD_LEN) )
            return 0;
        
        uint32_t ch;
        U8_NEXT_UNSAFE( word, i, ch );
        if ( ch == 0 )
            break;

        /* Check the symbol for validity */
        if ( !_charset->IsDigit( ch ) )
            return 0;

        /* Copy the symbol to local buffer */
        U8_APPEND_UNSAFE( outBuffer, j, ch );
    }

    /* Empty string given */
    if ( unlikely(size == 0) )
        return 0;

    outBuffer[ j ] = '\0';

    /* Create the LEXID */
    lex = static_cast<LEXID>( strtoul(outBuffer, NULL, 10) );

    // strtoul returns ULONG_MAX on overflow, this is greater than NUM_MAX_VALUE than SMINum can handle on
    // 32bit & 64bit lexemes
    if ( unlikely(lex > NUM_MAX_VALUE) )
        return 0;

    /* Update the LEXID */
    _LexSetNoMrph( &lex, _info.internalNumber );
	
    return size;
}

int LPMorphNumber::lemmatize( LemmatizeResultFunctor &functor,
                              const char             *word,
                              size_t                  wordLength,
                              unsigned int            /* flags */ )
{
    if ( unlikely(word == 0) )
        return LP_ERROR_EINVAL;

    /* Check and copy the word */
    char  lemmatizeBuffer[(NUM_MAX_WORD_LEN * 4) + 1];
    LEXID lex;
    if ( processWord( lemmatizeBuffer, lex, word, wordLength ) == 0 )
        return 0;

    /* Invoke the functor */
    LingProcErrors status = functor.nextLemma( lex, 0, 0 );
    if ( unlikely(status != LP_OK) )
        return status;

    return 1;
}

int LPMorphNumber::stemmatize( StemmatizeResultFunctor &functor,
                               const char              *word,
                               size_t                   wordLength,
                               unsigned int             /* flags */ )
{
    if ( unlikely(word == 0) )
        return LP_ERROR_EINVAL;

    /* Reserve the memory */
    char *stem = functor.getStemBuffer( (NUM_MAX_WORD_LEN * 4) + 1 );
    if ( stem == 0 )
        return LP_ERROR_ENOMEM;

    /* Check and copy the word */
    LEXID  lex  = 0;
    size_t size = processWord( stem, lex, word, wordLength );
    if ( size == 0 )
        return 0;

    /* Clear the lex id part */
    //lex &= ~(LEX_IDHIER);
    if ( (lex & LEX_IDHIER) >= LEX_IDHIER - 1 )
        lex = 0;
	
    /* Invoke the functor */
    LingProcErrors status = functor.nextStem( lex, 0, stem, size, 0 );
    if ( status != LP_OK )
        return status;

    return 1;
}

int LPMorphNumber::getLexText( LEXID lex, char *buffer, size_t bufferSize, const BinaryData * /* additionalData */ )
{
    if ( unlikely(LexNoMrph(lex) != _info.internalNumber) )
        return 0;

    if ( unlikely((lex & LEX_IDHIER) > NUM_MAX_VALUE) )
        return 0;
	
    char   tmp[ NUM_MAX_WORD_LEN ];
    size_t size;

    if ( buffer == 0 || bufferSize < 11 )
    {
        size = sprintf( tmp, "%llu", (long long unsigned int)(lex & LEX_IDHIER) );
        if ( buffer != 0 && bufferSize >= size )
            strncpy( buffer, tmp, bufferSize );
    }
    else
        size = sprintf( buffer, "%llu", (long long unsigned int)(lex & LEX_IDHIER) );

    return static_cast<int>(size);
}

int LPMorphNumber::getLexText(LEXID partialLex, const char *stem, char *buffer, size_t bufferSize, const BinaryData * /* additionalData */)
{
    if ( unlikely(LexNoMrph(partialLex) != _info.internalNumber) )
        return 0;

    size_t i = 0;
    for ( ;; )
    {
        uint32_t ch;
        U8_NEXT_UNSAFE( stem, i, ch );
        if ( ch == 0 )
            break;

        if ( !_charset->IsDigit( ch ) )
            return 0;
    }
        
    if ( buffer != 0 && bufferSize >= i )
    {
        memcpy( buffer, stem, i );
        if ( bufferSize > i )
            buffer[ i ] = '\0';
    }

    return static_cast<int>( i );
}

LingProcErrors LPMorphNumber::enumerate( MorphEnumerator &enumerator )
{
    for ( LEXID i = 0; i <= NUM_MAX_VALUE; i++ )
    {
        LEXID lex = LexSetNoMrph( i, _info.internalNumber );
        LingProcErrors result = enumerator.nextLexid( lex );
        if ( result )
            return result;
    }
    return LP_OK;
}

size_t LPMorphNumber::size()
{
    return static_cast<size_t>( NUM_MAX_VALUE );
}

