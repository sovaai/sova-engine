/**
 * @file   LexGramRW.h
 * @author swizard <me@swizard.info>
 * @date   Mon Feb  1 04:03:45 2010
 * 
 * @brief  Lexeme grammatical description (read/write version).
 * 
 * 
 */
#ifndef _LEXGRAMRW_H_
#define _LEXGRAMRW_H_

#include "LexGram.h"

class SM_DLL LexGramRW : public LexGram, public ShadowedRW
{
public:
    LexGramRW &setRefl( Refl value );
    LexGramRW &setTransitivity( Transitivity value );
    LexGramRW &setAspect( Aspect value );
    LexGramRW &setAnimate( Animate value );
    LexGramRW &setNumber( Number value );
    LexGramRW &setGender( Gender value );
    LexGramRW &setSuppl( Suppl value );
    LexGramRW &setSubtype( Subtype value );
    LexGramRW &setPartOfSpeech( PartOfSpeech value );
    
public:
    LexGramRW( StrictAllocator *_allocator = 0 ) : LexGram(), ShadowedRW( _allocator ) { }
    ~LexGramRW() { }
    
    LexGramRW( LexGramRW &gram, StrictAllocator *_allocator = 0 ) : LexGram(), ShadowedRW( _allocator )       { operator=( gram ); }
    LexGramRW( const LexGramRW &gram, StrictAllocator *_allocator = 0 ) : LexGram(), ShadowedRW( _allocator ) { operator=( gram ); }
    
public:
    LexGramRW &operator=( const LexGram &gram );
    LexGramRW &operator=( const LexGramRW &gram );
    bool operator==( const LexGram &gram );
    
public:
    void serialize( const void *&buffer, size_t &bufferSize ) const;
    void deserialize( const void *buffer, size_t bufferSize );
};

#endif /* _LEXGRAMRW_H_ */
