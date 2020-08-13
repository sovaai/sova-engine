/**
 * @file   DictionnaryInfoFSRW.cpp
 * @author swizard <me@swizard.info>
 * @date   Sun Jun  8 19:38:19 2008
 * 
 * @brief  The dictionary information that strict morphology is using (fstorage, read/write version) (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_inttype.h>

#include "DictionnaryInfoFSRW.h"

DictionnaryInfoFSRW::DictionnaryInfoFSRW() :
	DictionnaryInfoRW()
{
}

StrictMorphStorageChunk::Status DictionnaryInfoFSRW::save(fstorage *fs, fstorage_section_id sectionId)
{
	/* Create a section for a dictionary */
	fstorage_section *section = fstorage_get_section(fs, sectionId);
	if ( unlikely(section == 0) )
		return StrictMorphStorageChunk::ERROR_FSTORAGE;

	/* Resize the section */
	int status = fstorage_section_realloc(section, sizeof(uint32_t) * DictionnaryInfo::infoFieldsCount);
	if ( unlikely(status != FSTORAGE_OK) )
		return StrictMorphStorageChunk::ERROR_FSTORAGE;
	
	/* Create a chunk wrapper */
	StrictMorphStorageChunk chunk;
	chunk.buffer = fstorage_section_get_all_data(section);
	chunk.size   = fstorage_section_get_size(section);

	/* Save the dictionary */
	return DictionnaryInfoRW::save(chunk);
}

