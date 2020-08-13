/**
 * @file   StrictMorphParamsMap.h
 * @author swizard <me@swizard.info>
 * @date   Mon Jun 30 17:39:08 2008
 * 
 * @brief  Strict (dictionary) morphology creation parameters mappings.
 * 
 * 
 */
#ifndef _STRICTMORPHPARAMSMAP_H_
#define _STRICTMORPHPARAMSMAP_H_

#include <LingProc4/SimpleMorph/LPMorphCommon.h>

#include "API/StrictMorphParams.h"
#include "API/StrictMorph.h"

inline enum StrictMorphErrors mapStrictMorphParamsFlags( unsigned int flags, unsigned int &result )
{
	result = 0;
	
	if (flags & StrictMorphParams::FlagCaseInsensitive)
		result |= MORPH_FLAG_CASE_INSENSITIVE;
	if (flags & StrictMorphParams::FlagLwrAccent)
		result |= MORPH_FLAG_LWR_ACCENT;
	if (flags & StrictMorphParams::FlagLwrPunct)
		result |= MORPH_FLAG_LWR_PUNCT;
	if (flags & StrictMorphParams::FlagRemIgnore)
		result |= MORPH_FLAG_REM_IGNORE;
	if (flags & StrictMorphParams::FlagNativeOnly)
		result |= MORPH_FLAG_NATIVE_ONLY;
	if (flags & StrictMorphParams::FlagArabicSeqCollapse)
		result |= MORPH_FLAG_ARABIC_SEQ_COLLAPSE;
	if (flags & StrictMorphParams::FlagProcessCompounds)
		result |= MORPH_FLAG_PROCESS_COMPOUNDS;
	if (flags & StrictMorphParams::FlagUseDefault)
		result |= MORPH_FLAG_USE_DEFAULT;
	
	return STRICT_OK;
}

inline enum StrictMorphErrors mapStrictMorphParamsFlagsBack( unsigned int flags, unsigned int &result )
{
	result = 0;
	
	if (flags & MORPH_FLAG_CASE_INSENSITIVE)
		result |= StrictMorphParams::FlagCaseInsensitive;
	if (flags & MORPH_FLAG_LWR_ACCENT)
		result |= StrictMorphParams::FlagLwrAccent;
	if (flags & MORPH_FLAG_LWR_PUNCT)
		result |= StrictMorphParams::FlagLwrPunct;
	if (flags & MORPH_FLAG_REM_IGNORE)
		result |= StrictMorphParams::FlagRemIgnore;
	if (flags & MORPH_FLAG_NATIVE_ONLY)
		result |= StrictMorphParams::FlagNativeOnly;
	if (flags & MORPH_FLAG_PROCESS_COMPOUNDS)
		result |= StrictMorphParams::FlagProcessCompounds;
	if (flags & MORPH_FLAG_ARABIC_SEQ_COLLAPSE)
		result |= StrictMorphParams::FlagArabicSeqCollapse;
	if (flags & MORPH_FLAG_USE_DEFAULT)
		result |= StrictMorphParams::FlagUseDefault;
	
	return STRICT_OK;
}

#endif /* _STRICTMORPHPARAMSMAP_H_ */

