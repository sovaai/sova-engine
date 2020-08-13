#include "utils.h"
#include <lib/fstorage/fstorage.h>
#include <_include/_inttype.h>

class PatriciaTree;

extern const fstorage_section_id    kStoreError;
// \return section id after last used stored section, or kStoreError on error
fstorage_section_id StorePatriciaTree(PatriciaTree& tree, fstorage* storage, fstorage_section_id startSection);
// \return section id after last used stored section, or kStoreError on error
fstorage_section_id RestorePatriciaTree(PatriciaTree& tree, uint16_t flags, fstorage* storage, fstorage_section_id startSection);
fstorage_section_id StoreAvector(avector<uint8_t>& v, fstorage* storage, fstorage_section_id startSection);
fstorage_section_id RestoreAvector(TCharBuffer& buffer, fstorage* storage, fstorage_section_id startSection);
fstorage_section_id StoreBuffer(TCharBuffer buffer, fstorage* storage, fstorage_section_id startSection);
