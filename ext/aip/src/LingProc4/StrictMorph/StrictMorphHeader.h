/**
 * @file   StrictMorphHeader.h
 * @author swizard <me@swizard.info>
 * @date   Mon Jun  9 01:52:59 2008
 *
 * @brief  The header data for strict (dictionary) morphology.
 *
 *
 */
#ifndef _STRICTMORPHHEADER_H_
#define _STRICTMORPHHEADER_H_

#include <sys/types.h>
#include <_include/_inttype.h>

#include <LingProc4/SimpleMorph/LPMorphCommon.h>

#include "API/StrictMorphStorageChunk.h"

/* Constants */
const uint32_t     strictMorphMagic      = 0xB767847C;
const unsigned int strictMorphWFMajor    = 4;
const unsigned int strictMorphWFMinor    = 0;

const size_t       strictMorphHeaderSize = sizeof(uint32_t) * 16;

/* Actual header */
struct StrictMorphHeader
{
	uint32_t     magic;
	unsigned int workFormatMajor;
	unsigned int workFormatMinor;

	LPMorphDictInfo dict;

	StrictMorphStorageChunk::Status load(const StrictMorphStorageChunk &chunk);
};

#endif /* _STRICTMORPHHEADER_H_ */

