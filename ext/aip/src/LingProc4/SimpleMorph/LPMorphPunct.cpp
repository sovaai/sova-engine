/**
 * @file   LPMorphPunct.cpp
 * @author swizard <me@swizard.info>
 * @date   Tue Apr  8 04:32:09 2008
 * 
 * @brief  Punctuation morphology for LingProc (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include "../IndirectTable.h"

#include "LPMorphPunct.h"

LingProcErrors LPMorphPunct::create( const UCharSet *charset, const LPMorphCreateParams *params )
{
    if ( unlikely(charset == 0 || params == 0) )
        return LP_ERROR_EINVAL;

    _charset             = charset;
    _info.type           = MORPH_TYPE_PUNCT;
    _info.internalNumber = params->internalNumber;
    _info.minStemLength  = 1;

    if ( unlikely(params->flags & (MORPH_FLAG_USE_DEFAULT | MORPH_FLAG_INVALID)) )
        return LP_ERROR_MORPH_FLAGS;
        
    _info.creationFlags = params->flags;

    /* Clear all except only meaning flags */
    unsigned int storeFlags  = MORPH_FLAG_LWR_PUNCT;
    _info.creationFlags     &= storeFlags;
        
    /* Initialize word normalizer */
    normalizer = WordNormalizer::factory( _charset->GetLang(), MORPH_TYPE_PUNCT );
    if ( unlikely(normalizer == 0) )
        return LP_ERROR_EFAULT;
    
    WordNormalizerResult status = normalizer->create( _charset, _info.creationFlags );
    if ( unlikely(status != NORMALIZE_SUCCESS) )
        return LP_ERROR_EFAULT;
        
    return LP_OK;
}

LingProcErrors LPMorphPunct::processWord( uint32_t &ch, const char *word, size_t wordLength )
{
    if ( unlikely(word == 0) )
        return LP_ERROR_EINVAL;

    char buffer[16];

    WordNormalizerTarget f( buffer, sizeof(buffer) );
    WordNormalizerResult status = normalizer->normalizeWord( f, word, wordLength );
    switch ( status )
    {
    case NORMALIZE_SUCCESS:
        break;
                
    case NORMALIZE_ERROR_PARAMETER:                
        return LP_ERROR_EINVAL;
        
    case NORMALIZE_ERROR_FLAGS:
        return LP_ERROR_MORPH_FLAGS;
                
    case NORMALIZE_ERROR_BUFFER:
    case NORMALIZE_ERROR_NOT_NATIVE:
        return LP_OK;
                
    case NORMALIZE_ERROR_FUNCTOR:
    case NORMALIZE_ERROR_INTERNAL:
        return LP_ERROR_EFAULT;
    };

    size_t i = 0;
    U8_NEXT_UNSAFE( buffer, i, ch );

    if ( wordLength != static_cast<size_t>(-1) && i < wordLength )
        ch = 0;
    
    return LP_OK;
}

int LPMorphPunct::lemmatize( LemmatizeResultFunctor &functor,
                             const char             *word,
                             size_t                  wordLength,
                             unsigned int            /* flags */ )
{
    uint32_t ch = 0;
    LingProcErrors status = processWord( ch, word, wordLength );
    if ( unlikely(status != LP_OK) )
        return status;

    if ( ch == 0 || ch >= LEX_IDHIER )
        return 0;

    /* Create the LEXID */
    LEXID lex = static_cast<LEXID>( ch );
    _LexSetNoMrph( &lex, _info.internalNumber );

    /* Invoke the functor */
    status = functor.nextLemma( lex, 0, 0 );
    if ( unlikely(status != LP_OK) )
        return status;
	
    return 1;
}

int LPMorphPunct::stemmatize( StemmatizeResultFunctor &functor,
                              const char              *word,
                              size_t                   wordLength,
                              unsigned int             /* flags */ )
{
    uint32_t ch = 0;
    LingProcErrors status = processWord( ch, word, wordLength );
    if ( unlikely(status != LP_OK) )
        return status;

    if ( ch == 0 || ch >= LEX_IDHIER )
        return 0;

    size_t chLength = U8_LENGTH( ch );
    
    /* Reserve the memory */
    unsigned char *stem = reinterpret_cast<unsigned char *>( functor.getStemBuffer( chLength + 1 ) );
    if ( unlikely(stem == 0) )
        return LP_ERROR_ENOMEM;

    /* Copy result */
    size_t i = 0;
    U8_APPEND_UNSAFE( stem, i, ch );
    stem[ i ] = '\0';
	
    /* Create the LEXID */
    LEXID lex = static_cast<LEXID>(0);
    _LexSetNoMrph( &lex, _info.internalNumber );

    /* Invoke the functor */
    status = functor.nextStem( lex, 0, reinterpret_cast<char *>(stem), chLength, 0 );
    if ( unlikely(status != LP_OK) )
        return status;
	
    return 1;
}

int LPMorphPunct::getLexText( LEXID lex, char *buffer, size_t bufferSize, const BinaryData * /* additionalData */ )
{
    if ( unlikely(LexNoMrph(lex) != _info.internalNumber) )
        return 0;

    uint32_t ch = static_cast<uint32_t>(lex & LEX_IDHIER);
    
    if ( unlikely(!_charset->IsPunct( ch )) )
        return 0;

    size_t chLength = U8_LENGTH( ch );
    if ( buffer != 0 && bufferSize >= chLength )
    {
        size_t i = 0;
        U8_APPEND_UNSAFE( buffer, i, ch );
        if ( bufferSize > chLength )
            buffer[ chLength ] = '\0';
    }
	
    return static_cast<int>( chLength );
}

int LPMorphPunct::getLexText( LEXID partialLex, const char *stem, char *buffer, size_t bufferSize, const BinaryData * /* additionalData */ )
{
    if ( unlikely(LexNoMrph(partialLex) != _info.internalNumber) )
        return 0;

    UChar32 ch;
    size_t  i = 0;
    U8_NEXT_UNSAFE( stem, i, ch );
    
    if ( unlikely(!_charset->IsPunct( ch ) || stem[ i ] != '\0') )
        return 0;
	
    if ( buffer != 0 )
    {
        for ( size_t j = 0; j < i && j < bufferSize; j++ )
            buffer[ j ] = stem[ j ];
        if ( bufferSize > i )
            buffer[ i ] = '\0';
    }
	
    return static_cast<int>( i );
}

LingProcErrors LPMorphPunct::enumerate( MorphEnumerator &enumerator )
{
    class Enumerator : public IndirectTable::Enumerator
    {
    public:
        Enumerator( const UCharSet &_charset, MorphEnumerator &_enumerator, unsigned int _internalNumber ) :
            charset( _charset ),
            enumerator( _enumerator ),
            internalNumber( _internalNumber ),
            status( LP_OK ) { }
        
        int apply( uint32_t wchar )
        {
            if ( charset.IsPunct( wchar ) && wchar < LEX_IDHIER )
            {
                LEXID lex = LexSetNoMrph( wchar, internalNumber );
                status    = enumerator.nextLexid( lex );
                if ( unlikely(status != LP_OK) )
                    return 1;
            }
            return 0;
        }

        LingProcErrors getLastStatus() const { return status; }
            
    private:
        const UCharSet  &charset;
        MorphEnumerator &enumerator;
        unsigned int     internalNumber;
        LingProcErrors   status;
    } e( *_charset, enumerator, _info.internalNumber );

    _charset->enumerateKnownSymbols( e );
    return e.getLastStatus();
}

size_t LPMorphPunct::size()
{
    if ( _size == 0 )
    {
        class Counter : public IndirectTable::Enumerator
        {
        public:
            Counter( const UCharSet &_charset ) : charset( _charset ), count( 0 ) { }
            
            int apply( uint32_t wchar )
            {
                if ( charset.IsPunct( wchar ) )
                    count++;
                return 0;
            }

            size_t getCount() const { return count; }
            
        private:
            const UCharSet &charset;
            size_t          count;
        } counter( *_charset );

        _charset->enumerateKnownSymbols( counter );
        _size = counter.getCount();
    }
    return _size;
}



