#include "fstorage_utils.h"
#include <lib/libpts2/PatriciaTree.h>
#include <limits>

const fstorage_section_id kStoreError =   std::numeric_limits<fstorage_section_id>::max();

static const char   kSomeData[] =   {0};

fstorage_section_id StorePatriciaTree(PatriciaTree& tree, fstorage* storage, fstorage_section_id startSection)
{
    PatriciaStorageHandler  patData;
    if (PatriciaTreeCore::RESULT_STATUS_SUCCESS != tree.getStorageHandler(patData)) {
        return kStoreError;
    }
    bool                isContinue  =   true;
    unsigned int        patChunkId  =   PATRICIA_CHUNK_HEADER;
    fstorage_section_id sectionId   =   startSection;
    while ((true == isContinue) && (patChunkId != PATRICIA_CHUNK_LAST)) {
        StorageChunk        chunk       =   patData.chunks[patChunkId];
        fstorage_section*   section     =   fstorage_get_sized_section(storage, sectionId, chunk.size);

        isContinue  =   section != NULL;

        if (!isContinue) {
            continue;
        }

        //const cast as C code has no const
        void*   data    =   const_cast<void*>(static_cast<const void*>(chunk.buffer));
        if (NULL == data) {
            data    =   const_cast<void*>(static_cast<const void*>(kSomeData));
        }

        int writeStatus =   fstorage_section_set_data(section, data, chunk.size);
        isContinue  =   writeStatus == FSTORAGE_OK;
        patChunkId  +=  1;
        sectionId   +=  1;
    }
    if (!isContinue) {
        return kStoreError;
    }
    return sectionId;
}

fstorage_section_id RestorePatriciaTree(PatriciaTree& tree, uint16_t flags, fstorage* storage, fstorage_section_id startSection)
{
    PatriciaStorageHandler  patData;
    bool                    isContinue  =   true;
    unsigned int            patChunkId  =   PATRICIA_CHUNK_HEADER;
    fstorage_section_id     sectionId   =   startSection;
    while ((true == isContinue) && (patChunkId != PATRICIA_CHUNK_LAST)) {
        fstorage_section*   section     =   fstorage_find_section(storage, sectionId);
        isContinue  =   section != NULL;

        if (!isContinue) {
            continue;
        }

        StorageChunk&   chunk(patData.chunks[patChunkId]);
        chunk.buffer    =   static_cast<uint8_t*>(fstorage_section_get_all_data(section));
        isContinue      =   chunk.buffer != NULL;
        if (!isContinue) {
            continue;
        }
        chunk.size      =   fstorage_section_get_size(section);

        if (0 == chunk.size) {
            chunk.buffer    =   NULL;
        }

        patChunkId  +=  1;
        sectionId   +=  1;
    }
    if (!isContinue) {
        return kStoreError;
    }
    if (PatriciaTreeCore::RESULT_STATUS_SUCCESS != tree.setStorageHandler(flags, patData)) {
        return kStoreError;
    }
    return sectionId;
}

fstorage_section_id StoreBuffer(TCharBuffer buffer, fstorage* storage, fstorage_section_id startSection)
{
    fstorage_section*   section =   fstorage_get_sized_section( storage
                                                              , startSection
                                                              , buffer.Size() );
    if (NULL == section) {
        return kStoreError;
    }

    int writeStatus =   fstorage_section_set_data( section
                                                 , const_cast<void*>(static_cast<const void*>(buffer.Begin))
                                                 , buffer.Size() );
    if (FSTORAGE_OK != writeStatus) {
        return kStoreError;
    }

    return startSection + 1;
}

fstorage_section_id StoreAvector(avector<uint8_t>& v, fstorage* storage, fstorage_section_id startSection)
{
    fstorage_section*   section =   fstorage_get_sized_section( storage
                                                              , startSection
                                                              , v.size() );
    if (NULL == section) {
        return kStoreError;
    }

    int writeStatus =   fstorage_section_set_data( section
                                                 , static_cast<void*>(v.get_buffer())
                                                 , v.size() );
    if (FSTORAGE_OK != writeStatus) {
        return kStoreError;
    }

    return startSection + 1;
}

fstorage_section_id RestoreAvector(TCharBuffer& buffer, fstorage* storage, fstorage_section_id startSection)
{
    fstorage_section*   section     =   fstorage_find_section(storage, startSection);
    if (NULL == section) {
        return kStoreError;
    }

    buffer.Begin    =   static_cast<uint8_t*>(fstorage_section_get_all_data(section));
    if (NULL == buffer.Begin) {
        return kStoreError;
    }
    buffer.End      =   buffer.Begin + fstorage_section_get_size(section);
    return startSection + 1;
}
