/**
 * @file   FormGramRW.h
 * @author swizard <me@swizard.info>
 * @date   Fri Jan 29 16:17:40 2010
 * 
 * @brief  Word form grammatical description (read/write version).
 * 
 * 
 */
#ifndef _FORMGRAMRW_H_
#define _FORMGRAMRW_H_

#include "FormGram.h"

class SM_DLL FormGramRW : public FormGram, public ShadowedRW
{
public:
    FormGramRW &setMstform( Mstform value );
    FormGramRW &setVerbform( Verbform value );
    FormGramRW &setTense( Tense value );
    FormGramRW &setShort( Short value );
    FormGramRW &setRefl( Refl value );
    FormGramRW &setPerson( Person value );
    FormGramRW &setNumber( Number value );
    FormGramRW &setMood( Mood value );
    FormGramRW &setGender( Gender value );
    FormGramRW &setDegree( Degree value );
    FormGramRW &setCase( Case value );
    FormGramRW &setAnimate( Animate value );
    
public:
    FormGramRW( StrictAllocator *_allocator = 0 ) : FormGram(), ShadowedRW( _allocator ) { }
    ~FormGramRW() { }
    
    FormGramRW( FormGramRW &gram, StrictAllocator *_allocator = 0 ) : FormGram(), ShadowedRW( _allocator )       { operator=( gram ); }
    FormGramRW( const FormGramRW &gram, StrictAllocator *_allocator = 0 ) : FormGram(), ShadowedRW( _allocator ) { operator=( gram ); }

public:
    FormGramRW &operator=( const FormGram &gram );
    FormGramRW &operator=( const FormGramRW &gram );
    bool operator==( const FormGram &gram );
    
public:
    void serialize( const void *&buffer, size_t &bufferSize ) const;
    void deserialize( const void *buffer, size_t bufferSize );
};

#endif /* _FORMGRAMRW_H_ */

