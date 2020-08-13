/**
 * @file   StrictMorphHeaderRW.h
 * @author swizard <me@swizard.info>
 * @date   Mon Jun  9 02:23:40 2008
 * 
 * @brief  The header data for strict (dictionary) morphology (read/write version).
 * 
 * 
 */
#ifndef _STRICTMORPHHEADERRW_H_
#define _STRICTMORPHHEADERRW_H_

#include <_include/language.h>

#include "StrictMorphHeader.h"

/* Actual header */
struct StrictMorphHeaderRW : public StrictMorphHeader
{
	void                            setDefaults(LangCodes language);
	StrictMorphStorageChunk::Status save(const StrictMorphStorageChunk &chunk);
};

#endif /* _STRICTMORPHHEADERRW_H_ */

