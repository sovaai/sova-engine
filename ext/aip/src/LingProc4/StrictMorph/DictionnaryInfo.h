/**
 * @file   DictionnaryInfo.h
 * @author swizard <me@swizard.info>
 * @date   Tue May 27 00:55:30 2008
 * 
 * @brief  The dictionary information that strict morphology is using.
 * 
 * 
 */
#ifndef _DICTIONNARYINFO_H_
#define _DICTIONNARYINFO_H_

#include <LingProc4/StrictMorphGram/StrictDictInfo.h>

#include "API/StrictMorphStorageChunk.h"

struct DictionnaryInfo : public StrictDictInfo
{
	DictionnaryInfo();

	/** 
	 * Given the chunk of a raw memory deserializes the data from it.
	 * 
	 * @param chunk A chunk with a buffer and buffer size
	 * 
	 * @return Deserialization status as StrictMorphStorageChunk::Status
	 */
	StrictMorphStorageChunk::Status load(const StrictMorphStorageChunk &chunk);
};

#endif /* _DICTIONNARYINFO_H_ */

