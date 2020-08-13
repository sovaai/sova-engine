/**
 * @file   LexGramRW.cpp
 * @author swizard <me@swizard.info>
 * @date   Mon Feb  1 04:17:15 2010
 * 
 * @brief  Lexeme grammatical description (read/write version, implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include <LingProc4/StrictMorphGram/LexGramRW.h>

#include "LexGramDescShadowRW.h"

static LexGramDescShadowRW *obtainDesc( Shadowed &obj, void *&userBuffer, StrictAllocator &allocator )
{
    if ( unlikely(userBuffer == 0) )
    {
        LexGramDescShadowRW *desc = 
            reinterpret_cast<LexGramDescShadowRW *>( allocator.alloc( sizeof(LexGramDescShadowRW) ) );
        if ( unlikely(desc == 0) )
            return 0;

        if ( obj.getShadow() != 0 )
            *desc = *( reinterpret_cast<const LexGramDescShadowRW *>( obj.getShadow() ) );
        else
            memset( desc, 0, sizeof(LexGramDescShadowRW) );
        
        obj.assignShadow( desc );
        userBuffer = desc;
    }

    return const_cast<LexGramDescShadowRW *>( reinterpret_cast<const LexGramDescShadowRW *>(obj.getShadow()) );
}

LexGramRW &LexGramRW::setRefl( Refl value )
{
    LexGramDescShadowRW *desc = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(desc != 0) )
        desc->setRefl( value );
    return *this;
}

LexGramRW &LexGramRW::setTransitivity( Transitivity value )
{
    LexGramDescShadowRW *desc = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(desc != 0) )
        desc->setTransitivity( value );
    return *this;
}

LexGramRW &LexGramRW::setAspect( Aspect value )
{
    LexGramDescShadowRW *desc = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(desc != 0) )
        desc->setAspect( value );
    return *this;
}

LexGramRW &LexGramRW::setAnimate( Animate value )
{
    LexGramDescShadowRW *desc = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(desc != 0) )
        desc->setAnimate( value );
    return *this;
}

LexGramRW &LexGramRW::setNumber( Number value )
{
    LexGramDescShadowRW *desc = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(desc != 0) )
        desc->setNumber( value );
    return *this;
}

LexGramRW &LexGramRW::setGender( Gender value )
{
    LexGramDescShadowRW *desc = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(desc != 0) )
        desc->setGender( value );
    return *this;
}

LexGramRW &LexGramRW::setSuppl( Suppl value )
{
    LexGramDescShadowRW *desc = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(desc != 0) )
        desc->setSuppl( value );
    return *this;
}

LexGramRW &LexGramRW::setSubtype( Subtype value )
{
    LexGramDescShadowRW *desc = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(desc != 0) )
        desc->setSubtype( value );
    return *this;
}

LexGramRW &LexGramRW::setPartOfSpeech( PartOfSpeech value )
{
    LexGramDescShadowRW *desc = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(desc != 0) )
        desc->setPartOfSpeech( value );
    return *this;
}

LexGramRW &LexGramRW::operator=( const LexGram &gram )
{
    if ( unlikely(userBuffer != 0) )
        dropUserBuffer();
    
    assignShadow( gram.getShadow() );
    
    return *this;
}

LexGramRW &LexGramRW::operator=( const LexGramRW &gram )
{
    deserialize( reinterpret_cast<const void *>(gram.getShadow()), sizeof(LexGramDescShadowRW) );
    return *this;
}


bool LexGramRW::operator==( const LexGram &gram )
{
    if ( unlikely(getShadow() == 0 || gram.getShadow() == 0) )
        return false;
    
    return memcmp( reinterpret_cast<const LexGramDescShadow *>(getShadow()),
                   reinterpret_cast<const LexGramDescShadow *>(gram.getShadow()),
                   LexGramDescShadow::descOffset ) == 0;
}

void LexGramRW::serialize( const void *&buffer, size_t &bufferSize ) const
{
    buffer     = getShadow();
    bufferSize = sizeof(LexGramDescShadowRW);
}

void LexGramRW::deserialize( const void *buffer, size_t bufferSize )
{
    if ( unlikely(buffer == 0 || bufferSize != sizeof(LexGramDescShadowRW)) )
        return;

    LexGramDescShadowRW *desc = 
        reinterpret_cast<LexGramDescShadowRW *>( userAllocator->alloc( sizeof(LexGramDescShadowRW) ) );
    if ( unlikely(desc == 0) )
        return;
    
    if ( unlikely(userBuffer != 0) )
        dropUserBuffer();
    
    *desc = *( reinterpret_cast<const LexGramDescShadowRW *>( buffer ) );
        
    assignShadow( desc );
    userBuffer = desc;
}

