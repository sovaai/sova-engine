/**
 * @file   StrictMorphShadowFS.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 21 17:07:50 2008
 * 
 * @brief  Strict (dictionary) morphology class (fstorage version).
 * 
 * 
 */
#ifndef _STRICTMORPHSHADOWFS_H_
#define _STRICTMORPHSHADOWFS_H_

#include <lib/fstorage/fstorage.h>
#include <lib/libpts2/PatriciaTreeFS.h>
#include <LingProc4/UCharSet/UCharSet.h>

#include "API/StrictMorph.h"
#include "StrictMorphShadow.h"

class StrictMorphShadowFS : public StrictMorphShadow
{
public:
	StrictMorphShadowFS();
	~StrictMorphShadowFS();

public:
	/** 
	 * Initializes the StrictMorphShadowFS instance with the data in the fstorage provided.
	 * 
	 * @param charset A charset to use for the current morphology
	 * @param params  A structure with creation parameters filled
	 * @param fs A fstorage with the morphology data
	 * @param startSection A first section within morphologies sections sequence
	 * @param underlyingError The error lifted if failure
	 * 
	 * @return Creation result as enum StrictMorphErrors
	 * @see enum StrictMorphErrors
	 */
	enum StrictMorphErrors load(    const UCharSet          *charset,
					const StrictMorphParams &params,
					fstorage		*fs,
					fstorage_section_id	 startSection,
					int			&underlyingError );
	
protected:
	PatriciaTree *createPatriciaTree();
	
private:
	enum StrictMorphErrors createStemsTree( fstorage		*fs,
						   fstorage_section_id	 startSection,
						   int                  &underlyingError );
};

#endif /* _STRICTMORPHSHADOWFS_H_ */

