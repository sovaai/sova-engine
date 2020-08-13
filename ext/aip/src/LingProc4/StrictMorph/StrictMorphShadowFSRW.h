/**
 * @file   StrictMorphShadowFSRW.h
 * @author swizard <me@swizard.info>
 * @date   Fri Jun  6 20:46:20 2008
 * 
 * @brief  Strict (dictionary) morphology class (fstorage, read/write version).
 * 
 * 
 */
#ifndef _STRICTMORPHSHADOWFSRW_H_
#define _STRICTMORPHSHADOWFSRW_H_

#include "StrictMorphShadowFS.h"

class StrictMorphShadowFSRW : public StrictMorphShadowFS
{
public:
	StrictMorphShadowFSRW();
	~StrictMorphShadowFSRW();

public:
	/** 
	 * Stores the strict StrictMorphShadowFSRW instance with the data into the fstorage provided.
	 *
	 * @param fs A fstorage for the morphology data
	 * @param startSection A first section within morphologies sections sequence
	 * @param underlyingError The error lifted if failure
	 * 
	 * @return Creation result as enum StrictMorphErrors
	 * @see enum StrictMorphErrors
	 */
	enum StrictMorphErrors save( fstorage		*fs,
					fstorage_section_id	 startSection,
					int			&underlyingError );
};

#endif /* _STRICTMORPHSHADOWFSRW_H_ */

