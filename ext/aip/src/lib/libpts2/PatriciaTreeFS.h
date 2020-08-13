/**
 * @file   PatriciaTreeFS.h
 * @author swizard <me@swizard.info>
 * @date   Sun May  4 14:13:45 2008
 * 
 * @brief  PatriciaTree with fstorage save/load capability.
 * 
 * 
 */
#ifndef _PATRICIATREEFS_H_
#define _PATRICIATREEFS_H_

#include <_include/_inttype.h>

#include <lib/fstorage/fstorage.h>

#include "PatriciaTreeCore.h"
#include "PatriciaTree.h"
#include "PatriciaStorageHandler.h"

class PatriciaTreeFS : public PatriciaTree
{
public:
	/** 
	 * Standard constructor with no parameters.
	 * 
	 */
        PatriciaTreeFS();
	/** 
	 * Standard destructor with no parameters.
	 * 
	 */
	~PatriciaTreeFS();

public:
	/** 
	 * Store the tree in work format into the fstorage given.
	 * 
	 * @param fs The fstorage handler
	 * @param startSection The start section to use in fstorage
	 * 
	 * @return saving result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus save(fstorage *fs, fstorage_section_id startSection);
	/** 
	 * Load the tree in work format from the fstorage given.
	 * 
	 * @param flags The flags that specify the tree functionality
	 * @param fs The fstorage handler
	 * @param startSection The start section to use in fstorage
	 * 
	 * @return loading result as ResultStatus
	 * @see PatriciaTreeCore::ResultStatus
	 */
	PatriciaTreeCore::ResultStatus load(uint16_t flags, fstorage *fs, fstorage_section_id startSection);

private:
	PatriciaTreeCore::ResultStatus storeSection(fstorage *fs, fstorage_section_id sectionId, StorageChunk &chunk);
	PatriciaTreeCore::ResultStatus loadSection(fstorage *fs, fstorage_section_id sectionId, StorageChunk &chunk);
        static int writeChunkCallback(fstorage_stream_section_config *fscc, fstorage_section_access_config *fsac);

private:
	PatriciaStorageHandler handler;
};

#endif /* _PATRICIATREEFS_H_ */

