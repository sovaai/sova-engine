/**
 * @file   FormGramRW.cpp
 * @author swizard <me@swizard.info>
 * @date   Sun Jan 31 06:17:50 2010
 * 
 * @brief  Word form grammatical description (read/write version, implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include <LingProc4/StrictMorphGram/FormGramRW.h>

#include "ParadigmRW.h"
#include "FormGramDescShadowRW.h"

static FormGramDescShadowRW *obtainDesc( Shadowed &obj, void *&userBuffer, StrictAllocator &allocator )
{
    if ( unlikely(userBuffer == 0) )
    {
        ParadigmFormRW *pf =
            reinterpret_cast<ParadigmFormRW *>( allocator.alloc( sizeof(ParadigmFormRW) ) );
        if ( unlikely(pf == 0) )
            return 0;
        
        if ( obj.getShadow() != 0 )
            *pf = *( reinterpret_cast<const ParadigmFormRW *>( obj.getShadow() ) );
        else
            memset( pf, 0, sizeof(ParadigmFormRW) );
        
        obj.assignShadow( pf );
        userBuffer = pf;
    }

    ParadigmFormRW       *pf   = const_cast<ParadigmFormRW *>( reinterpret_cast<const ParadigmFormRW *>(obj.getShadow()) );
    FormGramDescShadowRW *gram = pf->getFormGramDesc();
    
    return gram;
}

FormGramRW &FormGramRW::setMstform( Mstform value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setMstform( value );
    return *this;
}

FormGramRW &FormGramRW::setVerbform( Verbform value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setVerbform( value );
    return *this;
}

FormGramRW &FormGramRW::setTense( Tense value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setTense( value );
    return *this;
}

FormGramRW &FormGramRW::setShort( Short value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setShort( value );
    return *this;
}

FormGramRW &FormGramRW::setRefl( Refl value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setRefl( value );
    return *this;
}

FormGramRW &FormGramRW::setPerson( Person value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setPerson( value );
    return *this;
}

FormGramRW &FormGramRW::setNumber( Number value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setNumber( value );
    return *this;
}

FormGramRW &FormGramRW::setMood( Mood value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setMood( value );
    return *this;
}

FormGramRW &FormGramRW::setGender( Gender value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setGender( value );
    return *this;
}

FormGramRW &FormGramRW::setDegree( Degree value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setDegree( value );
    return *this;
}

FormGramRW &FormGramRW::setCase( Case value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setCase( value );
    return *this;
}

FormGramRW &FormGramRW::setAnimate( Animate value )
{
    FormGramDescShadowRW *gram = obtainDesc( *this, userBuffer, *userAllocator );
    if ( likely(gram != 0) )
        gram->setAnimate( value );
    return *this;
}

FormGramRW &FormGramRW::operator=( const FormGram &gram )
{
    if ( unlikely(userBuffer != 0) )
        dropUserBuffer();
    
    assignShadow( gram.getShadow() );
    
    return *this;
}

FormGramRW &FormGramRW::operator=( const FormGramRW &gram )
{
    deserialize( reinterpret_cast<const void *>(gram.getShadow()), sizeof(ParadigmFormRW) );
        
    return *this;
}

bool FormGramRW::operator==( const FormGram &gram )
{
    if ( unlikely(getShadow() == 0 || gram.getShadow() == 0) )
        return false;
    
    const ParadigmForm       *pfA   = reinterpret_cast<const ParadigmForm *>(getShadow());
    const FormGramDescShadow *gramA = pfA->getDesc();
    const ParadigmForm       *pfB   = reinterpret_cast<const ParadigmForm *>(gram.getShadow());
    const FormGramDescShadow *gramB = pfB->getDesc();
    
    return memcmp( gramA, gramB, sizeof(FormGramDescShadow) ) == 0;
}

void FormGramRW::serialize( const void *&buffer, size_t &bufferSize ) const
{
    buffer     = getShadow();
    bufferSize = sizeof(ParadigmFormRW);
}

void FormGramRW::deserialize( const void *buffer, size_t bufferSize )
{
    if ( unlikely(buffer == 0 || bufferSize != sizeof(ParadigmFormRW)) )
        return;
    
    ParadigmFormRW *pf =
        reinterpret_cast<ParadigmFormRW *>( userAllocator->alloc( sizeof(ParadigmFormRW) ) );
    if ( unlikely(pf == 0) )
        return;
    
    if ( unlikely(userBuffer != 0) )
        dropUserBuffer();
    
    *pf = *( reinterpret_cast<const ParadigmFormRW *>( buffer ) );
        
    assignShadow( pf );
    userBuffer = pf;
}

