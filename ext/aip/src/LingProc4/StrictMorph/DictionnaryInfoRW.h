/**
 * @file   DictionnaryInfoRW.h
 * @author swizard <me@swizard.info>
 * @date   Tue May 27 01:46:09 2008
 * 
 * @brief  The dictionary information that strict morphology is using (read/write variant).
 * 
 * 
 */
#ifndef _DICTIONNARYINFORW_H_
#define _DICTIONNARYINFORW_H_

#include "DictionnaryInfo.h"

struct DictionnaryInfoRW : public DictionnaryInfo
{
	DictionnaryInfoRW();

	/** 
	 * Given the raw memory buffer as a chunk serializes the data to it.
	 * The chunk size should be more or equal than this class fields count
	 * multiple by the sizeof(uint32_t).
	 * 
	 * @param chunk A chunk with a buffer and buffer size set
	 * 
	 * @return Deserialization status as StrictMorphStorageChunk::Status
	 */
	StrictMorphStorageChunk::Status save(StrictMorphStorageChunk &chunk);
};

#endif /* _DICTIONNARYINFORW_H_ */

