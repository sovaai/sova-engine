/**
 * @file   StrictMorph.cpp
 * @author swizard <me@swizard.info>
 * @date   Tue May 27 00:18:46 2008
 * 
 * @brief  Strict (dictionary) morphology user interface implementation.
 * 
 * 
 */

#include <stdlib.h>

#include <_include/cc_compat.h>
#include <_include/_inttype.h>

#include "LexUnit.h"
#include "API/StrictMorph.h"
#include "StrictMorphShadow.h"

StrictMorph::StrictMorph() :
        shadow( 0 )
{
}

StrictMorph::~StrictMorph()
{
	if ( likely(shadow != 0) )
		delete shadow;
}

StrictMorphErrors StrictMorph::create( const UCharSet			*charset,
				       const StrictMorphParams          &params,
				       const StrictMorphMemoryHandler	&handler,
				       int				&underlyingError )
{
	shadow = new StrictMorphShadow();

	if ( unlikely(shadow == 0) )
		return STRICT_ERROR_INTERNAL_ERROR;
	
	return shadow->create(charset, params, handler, underlyingError);
}

StrictMorphErrors StrictMorph::enumerate( StrictLex::LemmatizeFunctor &enumerator ) const
{
	if ( unlikely(shadow == 0) )
		return STRICT_ERROR_NOT_CREATED;
	
	return shadow->enumerate( enumerator );
}

StrictMorphErrors StrictMorph::lemmatize( StrictLex::LemmatizeFunctor &functor,
					  const char                  *word,
					  size_t                       wordLength,
                                          unsigned int                 flags ) const
{
	if ( unlikely(shadow == 0) )
		return STRICT_ERROR_NOT_CREATED;

	return shadow->lemmatize( functor, word, wordLength, flags );
}

StrictMorphErrors StrictMorph::normalizeWord( WordNormalizerFunctor    &functor,
                                              const char               *word,
                                              size_t                    wordLength,
                                              unsigned int              flags ) const
{
	if ( unlikely(shadow == 0) )
		return STRICT_ERROR_NOT_CREATED;

	return shadow->normalizeWord( functor, word, wordLength, flags );
}

const StrictDictInfo *StrictMorph::getDictInfo() const
{
	if ( unlikely(shadow == 0) )
		return 0;

	return shadow->getDictInfo();
}

unsigned int StrictMorph::getFlags() const
{
	if ( unlikely(shadow == 0) )
		return StrictMorphParams::FlagInvalid;

	return shadow->getFlags();
}

unsigned int StrictMorph::getMorphNumber() const
{
	if ( unlikely(shadow == 0) )
		return 0;

	return shadow->getMorphNumber();
}

template<typename PackType>
StrictMorphErrors StrictMorph::pack( const StrictLex &lex, PackType &result, BinaryData *additionalData ) const
{
	return STRICT_ERROR_UNSUPPORTED_PACK_TYPE;
}

template<>
StrictMorphErrors StrictMorph::pack<uint32_t>( const StrictLex &lex, uint32_t &result, BinaryData *additionalData ) const
{
	if ( unlikely(shadow == 0) )
		return STRICT_ERROR_NOT_CREATED;

	return shadow->pack32( lex, result, additionalData );
}

template<>
StrictMorphErrors StrictMorph::pack<uint64_t>( const StrictLex &lex, uint64_t &result, BinaryData *additionalData ) const
{
	if ( unlikely(shadow == 0) )
		return STRICT_ERROR_NOT_CREATED;

	return shadow->pack64( lex, result, additionalData );
}

template<typename PackType>
StrictMorphErrors StrictMorph::callWithUnpack( PackType packedLex, StrictMorph::UnpackFunctor &functor, const BinaryData *additionalData ) const
{
	return STRICT_ERROR_UNSUPPORTED_PACK_TYPE;
}

template<>
StrictMorphErrors StrictMorph::callWithUnpack<uint32_t>( uint32_t packedLex, StrictMorph::UnpackFunctor &functor, const BinaryData *additionalData ) const
{
	if ( unlikely(shadow == 0) )
		return STRICT_ERROR_NOT_CREATED;
	
	return shadow->callWithUnpack32( packedLex, functor, additionalData );
}

template<>
StrictMorphErrors StrictMorph::callWithUnpack<uint64_t>( uint64_t packedLex, StrictMorph::UnpackFunctor &functor, const BinaryData *additionalData ) const
{
	if ( unlikely(shadow == 0) )
		return STRICT_ERROR_NOT_CREATED;
	
	return shadow->callWithUnpack64( packedLex, functor, additionalData );
}

unsigned long StrictMorph::getDataSerial() const
{
	if ( unlikely(shadow == 0) )
		return 0;
        return shadow->getDataSerial();
}

unsigned long StrictMorph::getDataMajor() const
{
	if ( unlikely(shadow == 0) )
		return 0;
        return shadow->getDataMajor();
}

unsigned long StrictMorph::getDataMinor() const
{
	if ( unlikely(shadow == 0) )
		return 0;
        return shadow->getDataMinor();
}

class DefaultAllocator : public StrictAllocator
{
public:
    void *alloc( size_t size ) { return ::malloc( size ); }
    void  free( void *ptr )    { return ::free( ptr ); }
} _defaultAllocator;

ShadowedRW::ShadowedRW( StrictAllocator *_allocator ) :
    userBuffer( 0 ),
    userAllocator( _allocator )
{
    if ( likely(_allocator == 0) )
        userAllocator = &_defaultAllocator;
}

void ShadowedRW::dropUserBuffer()
{
    if ( userBuffer != 0 )
    {
        userAllocator->free( userBuffer );
        userBuffer = 0;
    }
}

ShadowedRW::~ShadowedRW()
{
    dropUserBuffer();
}

