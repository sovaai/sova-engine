/**
 * @file   PatriciaTreeFS.cpp
 * @author swizard <me@swizard.info>
 * @date   Sun May  4 19:38:42 2008
 * 
 * @brief  PatriciaTree with fstorage save/load capability (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_string.h>

#include "PatriciaTreeFS.h"

const uint32_t magicValue = 0x4BB6AE65;

PatriciaTreeFS::PatriciaTreeFS() :
	PatriciaTree(),
	handler()
{
}

PatriciaTreeFS::~PatriciaTreeFS()
{
}

PatriciaTreeCore::ResultStatus PatriciaTreeFS::save(fstorage *fs, fstorage_section_id startSection)
{
	/* Obtain the memory buffers */
	PatriciaTreeCore::ResultStatus status = getStorageHandler(handler);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;

	/* Store the buffers obtained to the sections given */
	status = storeSection(fs, startSection + 0, handler.chunks[PATRICIA_CHUNK_HEADER]);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;
	status = storeSection(fs, startSection + 1, handler.chunks[PATRICIA_CHUNK_DATA]);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;
	status = storeSection(fs, startSection + 2, handler.chunks[PATRICIA_CHUNK_CROSSLINKS]);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;
	status = storeSection(fs, startSection + 3, handler.chunks[PATRICIA_CHUNK_REVERSE_INDEX]);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;
		
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

PatriciaTreeCore::ResultStatus PatriciaTreeFS::load(uint16_t flags, fstorage *fs, fstorage_section_id startSection)
{
	PatriciaTreeCore::ResultStatus status;

	/* Load the buffers from the sections given */
	status = loadSection(fs, startSection + 0, handler.chunks[PATRICIA_CHUNK_HEADER]);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;
	status = loadSection(fs, startSection + 1, handler.chunks[PATRICIA_CHUNK_DATA]);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;
	status = loadSection(fs, startSection + 2, handler.chunks[PATRICIA_CHUNK_CROSSLINKS]);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;
	status = loadSection(fs, startSection + 3, handler.chunks[PATRICIA_CHUNK_REVERSE_INDEX]);
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
		return status;

	return setStorageHandler(flags, handler);
}

PatriciaTreeCore::ResultStatus PatriciaTreeFS::storeSection(fstorage            *fs,
							    fstorage_section_id  sectionId,
							    StorageChunk        &chunk)
{
	fstorage_section *section = fstorage_get_section(fs, sectionId);
	if ( unlikely(section == 0) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_OBTAIN_SECTION;

	struct fstorage_stream_section_config fssc;
	bzero(&fssc, sizeof(struct fstorage_stream_section_config));
	fssc.on_write = PatriciaTreeFS::writeChunkCallback;
	fssc.context  = reinterpret_cast<void *>(&chunk);

	if ( unlikely(fstorage_section_stream(section, &fssc) != 0) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_STREAM_SECTION;
		
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

PatriciaTreeCore::ResultStatus PatriciaTreeFS::loadSection(fstorage            *fs,
							   fstorage_section_id  sectionId,
							   StorageChunk        &chunk)
{
	fstorage_section *section = fstorage_find_section(fs, sectionId);
	if ( unlikely(section == 0) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_OBTAIN_SECTION;

	/* Load the data */
	chunk.size   = fstorage_section_get_size(section);
	chunk.buffer = reinterpret_cast<const uint8_t *>( fstorage_section_get_all_data(section) );
	
	if ( unlikely(chunk.buffer == 0) )
		return PatriciaTreeCore::RESULT_STATUS_ERROR_GET_SECTION_DATA;

	/* Check the magic value */
	uint32_t magic = 0;

	chunk.size -= sizeof(uint32_t);
	memcpy(&magic, chunk.buffer, sizeof(uint32_t));

	if (magic != magicValue + chunk.size)
		return PatriciaTreeCore::RESULT_STATUS_ERROR_WRONG_MAGIC_VALUE;

	chunk.buffer += sizeof(uint32_t);
	
	return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}


int PatriciaTreeFS::writeChunkCallback(fstorage_stream_section_config *fscc,
				       fstorage_section_access_config *fsac)
{
	StorageChunk *chunk = reinterpret_cast<StorageChunk *>(fscc->context);

	/* Write down magic value */
	uint32_t magic  = magicValue + chunk->size;
	int      result = fsac->write( fsac, reinterpret_cast<const char *>(&magic), sizeof(uint32_t) );
	if ( unlikely(result < 0 || static_cast<size_t>(result) < sizeof(uint32_t)) )
		return 1;

	/* Write the data provided */
	result = fsac->write( fsac, reinterpret_cast<const char *>(chunk->buffer), chunk->size );
	if ( unlikely(result < 0 || static_cast<size_t>(result) < chunk->size) )
		return 1;
		
	return 0;
}


