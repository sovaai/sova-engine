#ifndef _STRICTMORPHFS_H_
#define _STRICTMORPHFS_H_

#include <lib/fstorage/fstorage.h>

#include "StrictMorph.h"

class UCharSet;
class StrictMorphFS : public StrictMorph
{
public:
	StrictMorphFS();
	~StrictMorphFS();

public:
	/** 
	 * Initializes the StrictMorphFS instance with the data in the fstorage provided.
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
	StrictMorphErrors load( const UCharSet          *charset,
                                const StrictMorphParams &params,
                                fstorage		*fs,
                                fstorage_section_id	 startSection,
                                int			&underlyingError );
};

#endif /* _STRICTMORPHFS_H_ */

