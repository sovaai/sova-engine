/**
 * @file   StrictMorphMemoryHandler.h
 * @author swizard <me@swizard.info>
 * @date   Wed Jun 11 13:47:06 2008
 * 
 * @brief  A memory image for strict morphology initialization.
 * 
 * 
 */
#ifndef _STRICTMORPHMEMORYHANDLER_H_
#define _STRICTMORPHMEMORYHANDLER_H_

#include "StrictMorphStorageChunk.h"

struct StrictMorphMemoryHandler
{
	StrictMorphStorageChunk header;
	
	StrictMorphStorageChunk dictInfo;
	StrictMorphStorageChunk paradigms;
	StrictMorphStorageChunk paradigmForms;
	StrictMorphStorageChunk wordForms;
	StrictMorphStorageChunk flexDistribs;
	StrictMorphStorageChunk flexions;

	StrictMorphStorageChunk lexemes;
	StrictMorphStorageChunk lexGramDescList;
	StrictMorphStorageChunk lexStemsList;
	StrictMorphStorageChunk lexOrthVariants;
	
	StrictMorphStorageChunk stemsTreeHeader;
	StrictMorphStorageChunk stemsTreeData;
	StrictMorphStorageChunk stemsTreeCrosslinks;
        
	StrictMorphStorageChunk excludeChains;
	StrictMorphStorageChunk excludeIndex;
        
        StrictMorphMemoryHandler() :
	        header(),
		dictInfo(),
	        paradigms(),
		paradigmForms(),
		wordForms(),
		flexDistribs(),
		flexions(),
		lexemes(),
		lexGramDescList(),
		lexStemsList(),
		stemsTreeHeader(),
		stemsTreeData(),
                stemsTreeCrosslinks(),
                excludeChains(),
                excludeIndex() { }
};

#endif /* _STRICTMORPHMEMORYHANDLER_H_ */

