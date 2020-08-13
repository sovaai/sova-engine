/**
 * @file   StrictMorphHeaderRW.cpp
 * @author swizard <me@swizard.info>
 * @date   Mon Jun  9 02:24:58 2008
 * 
 * @brief  The header data for strict (dictionary) morphology (read/write version) (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_inttype.h>
#include <_include/ntoh.h>

#include <LingProc4/LanguageInfo.h>

#include "StrictMorphHeaderRW.h"

void StrictMorphHeaderRW::setDefaults(LangCodes language)
{
	/* Obtain the full information for the language */
	const LanguageInfo *langFullInfo = LanguageInfo::getLanguageInfo( language );
	if ( unlikely(langFullInfo == 0) )
		return;

	magic                = strictMorphMagic;
	workFormatMajor      = strictMorphWFMajor;
	workFormatMinor      = strictMorphWFMinor;

	dict.type            = MORPH_TYPE_STRICT;
	dict.version         = 1;
	dict.variant         = 1;
	
	dict.lang            = language;
	dict.globalId        = (static_cast<unsigned int>(dict.type) << 24) | (static_cast<unsigned int>(dict.lang));

	dict.internalNumber  = langFullInfo->strictNumber;
	dict.minStemLength   = 0;
        dict.minCompoundStemLength = 0;
	dict.creationFlags   = 0;
}

StrictMorphStorageChunk::Status StrictMorphHeaderRW::save(const StrictMorphStorageChunk &chunk)
{
	/* Check the buffer size first: it should be enough for all fields storing */
	if ( unlikely(chunk.size < strictMorphHeaderSize) )
		return StrictMorphStorageChunk::ERROR_INVALID_SIZE;

	/* Save values then */
	
	uint8_t *buffer = reinterpret_cast<uint8_t *>( const_cast<void *>(chunk.buffer) );
	
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 0) ) =
		htobe32( static_cast<uint32_t>(magic) );
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 1) ) =
		htobe32( static_cast<uint32_t>(workFormatMajor) );
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 2) ) =
		htobe32( static_cast<uint32_t>(workFormatMinor) );
	
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 6) ) =
		htobe32( static_cast<uint32_t>(dict.type) );
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 7) ) =
		htobe32( static_cast<uint32_t>(dict.version) );
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 8) ) =
		htobe32( static_cast<uint32_t>(dict.variant) );
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 9) ) =
		htobe32( static_cast<uint32_t>(dict.lang) );
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 11) ) =
		htobe32( static_cast<uint32_t>(dict.globalId) );
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 12) ) =
		htobe32( static_cast<uint32_t>(dict.internalNumber) );
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 14) ) =
		htobe32( static_cast<uint32_t>(dict.minStemLength) );
	*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * 15) ) =
		htobe32( static_cast<uint32_t>(dict.creationFlags) );

	return StrictMorphStorageChunk::SUCCESS;
}

