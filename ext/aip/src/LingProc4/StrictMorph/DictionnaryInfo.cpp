/**
 * @file   DictionnaryInfo.cpp
 * @author swizard <me@swizard.info>
 * @date   Tue May 27 01:32:34 2008
 * 
 * @brief  The dictionary information that strict morphology is using (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_inttype.h>
#include <_include/ntoh.h>

#include "DictionnaryInfo.h"

DictionnaryInfo::DictionnaryInfo() : StrictDictInfo()
{
}
	
StrictMorphStorageChunk::Status DictionnaryInfo::load(const StrictMorphStorageChunk &chunk)
{
	/* Check the buffer size first */
	if ( unlikely(chunk.size != sizeof(uint32_t) * DictionnaryInfo::infoFieldsCount) )
		return StrictMorphStorageChunk::ERROR_INVALID_SIZE;

	/* Load values then */
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
		*values[i] = 
			static_cast<size_t>
			(be32toh( *( reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(chunk.buffer) +
									sizeof(uint32_t) * i) ) ));
	
	return StrictMorphStorageChunk::SUCCESS;
}

