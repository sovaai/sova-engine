/**
 * @file   DictionnaryInfoFSRW.h
 * @author swizard <me@swizard.info>
 * @date   Sun Jun  8 19:36:20 2008
 * 
 * @brief  The dictionary information that strict morphology is using (fstorage, read/write version).
 * 
 * 
 */
#ifndef _DICTIONNARYINFOFSRW_H_
#define _DICTIONNARYINFOFSRW_H_

#include <lib/fstorage/fstorage.h>

#include "DictionnaryInfoRW.h"

struct DictionnaryInfoFSRW : public DictionnaryInfoRW
{
	DictionnaryInfoFSRW();

	StrictMorphStorageChunk::Status save(fstorage *fs, fstorage_section_id sectionId);
};

#endif /* _DICTIONNARYINFOFSRW_H_ */

