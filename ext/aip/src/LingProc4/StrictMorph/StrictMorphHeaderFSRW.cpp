/**
 * @file   StrictMorphHeaderFSRW.cpp
 * @author swizard <me@swizard.info>
 * @date   Mon Jun  9 03:07:48 2008
 * 
 * @brief  The header data for strict (dictionary) morphology (fstorage, read/write version) (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include "StrictMorphHeaderFSRW.h"

StrictMorphStorageChunk::Status StrictMorphHeaderFSRW::save(fstorage *fs, fstorage_section_id sectionId)
{
	/* Create a section for a header */
	fstorage_section *section = fstorage_get_section(fs, sectionId);
	if ( unlikely(section == 0) )
		return StrictMorphStorageChunk::ERROR_FSTORAGE;

	/* Resize the section */
	int status = fstorage_section_realloc(section, strictMorphHeaderSize);
	if ( unlikely(status != FSTORAGE_OK) )
		return StrictMorphStorageChunk::ERROR_FSTORAGE;
	
	/* Create a chunk wrapper */
	StrictMorphStorageChunk chunk;
	chunk.buffer = fstorage_section_get_all_data(section);
	chunk.size   = fstorage_section_get_size(section);

	/* Save the dictionary */
	return StrictMorphHeaderRW::save(chunk);
}


