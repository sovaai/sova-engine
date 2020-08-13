/**
 * @file   StrictMorphHeaderFSRW.h
 * @author swizard <me@swizard.info>
 * @date   Mon Jun  9 03:06:03 2008
 * 
 * @brief  The header data for strict (dictionary) morphology (fstorage, read/write version).
 * 
 * 
 */
#ifndef _STRICTMORPHHEADERFSRW_H_
#define _STRICTMORPHHEADERFSRW_H_

#include <lib/fstorage/fstorage.h>

#include "StrictMorphHeaderRW.h"

/* Actual header */
struct StrictMorphHeaderFSRW : public StrictMorphHeaderRW
{
	StrictMorphStorageChunk::Status save(fstorage *fs, fstorage_section_id sectionId);
};
	
#endif /* _STRICTMORPHHEADERFSRW_H_ */

