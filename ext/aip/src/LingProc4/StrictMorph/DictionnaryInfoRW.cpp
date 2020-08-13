/**
 * @file   DictionnaryInfoRW.cpp
 * @author swizard <me@swizard.info>
 * @date   Tue May 27 01:47:44 2008
 * 
 * @brief  The dictionary information that strict morphology is using (read/write variant) (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_inttype.h>
#include <_include/ntoh.h>

#include "DictionnaryInfoRW.h"

DictionnaryInfoRW::DictionnaryInfoRW() :
	DictionnaryInfo()
{
}

StrictMorphStorageChunk::Status DictionnaryInfoRW::save(StrictMorphStorageChunk &chunk)
{
	/* Check the buffer size first: it should be enough for all fields storing */
	if ( unlikely(chunk.size < sizeof(uint32_t) * DictionnaryInfo::infoFieldsCount) )
		return StrictMorphStorageChunk::ERROR_INVALID_SIZE;

	/* Save values then */

	uint8_t *buffer = reinterpret_cast<uint8_t *>( const_cast<void *>(chunk.buffer) );

	size_t *values[DictionnaryInfo::infoFieldsCount] =
		{
			&paradigmsCount,
			&paradigmFormsCount,
			&wordFormsCount,
			&flexCount,
			&flexDistribCount,
			&maxFlexLength,
			&lexemesCount,
			&lexGramDescListLength,
			&lexStemsListLength,
			&lexOrthVarsListLength,
                        &excludeChainsLength,
                        reinterpret_cast<size_t *>(&datasetBuildTime)
		};

	for ( register size_t i = 0; i < DictionnaryInfo::infoFieldsCount; i++ )
		*( reinterpret_cast<uint32_t *>(buffer + sizeof(uint32_t) * i) ) =
			htobe32( static_cast<uint32_t>( *values[i] ) );

	chunk.size = sizeof(uint32_t) * DictionnaryInfo::infoFieldsCount;
	
	return StrictMorphStorageChunk::SUCCESS;
}

