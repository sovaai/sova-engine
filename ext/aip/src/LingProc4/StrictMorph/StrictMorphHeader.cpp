/**
 * @file   StrictMorphHeader.cpp
 * @author swizard <me@swizard.info>
 * @date   Mon Jun  9 02:06:02 2008
 * 
 * @brief  The header data for strict (dictionary) morphology (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/ntoh.h>

#include "StrictMorphHeader.h"

StrictMorphStorageChunk::Status StrictMorphHeader::load(const StrictMorphStorageChunk &chunk)
{
	/* Check the buffer size first */
	if ( unlikely(chunk.size != strictMorphHeaderSize) )
		return StrictMorphStorageChunk::ERROR_INVALID_SIZE;

	/* Load values then */
	magic = 
		static_cast<uint32_t>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 0) ) ));
	workFormatMajor =
		static_cast<unsigned int>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 1) ) ));
	workFormatMinor =
		static_cast<unsigned int>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 2) ) ));	
	dict.type =
		static_cast<MorphTypes>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 6) ) ));
	dict.version =
		static_cast<unsigned int>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 7) ) ));
	dict.variant =
		static_cast<unsigned int>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 8) ) ));
	dict.lang =
		static_cast<LangCodes>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 9) ) ));
	dict.globalId =
		static_cast<unsigned int>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 11) ) ));
	dict.internalNumber =
		static_cast<unsigned int>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 12) ) ));
	dict.minStemLength =
		static_cast<size_t>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 14) ) ));

        dict.minCompoundStemLength = dict.minStemLength;
        
	dict.creationFlags =
		static_cast<unsigned int>
		(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
								sizeof(uint32_t) * 15) ) ));

	/* Check the data */
	if ( unlikely(magic != strictMorphMagic) )
		return StrictMorphStorageChunk::ERROR_CORRUPT_DATA;

	/* Check the versions */
	if ( unlikely( workFormatMajor      != strictMorphWFMajor ||
		       workFormatMinor      >  strictMorphWFMinor ) )
		return StrictMorphStorageChunk::ERROR_VERSION_MISMATCH;
	
	return StrictMorphStorageChunk::SUCCESS;
}


