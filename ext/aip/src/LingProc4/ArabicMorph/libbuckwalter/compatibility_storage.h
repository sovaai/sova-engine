#ifndef _LIBBUCKWALTER_COMPATABILITY_STORAGE_H_
#define _LIBBUCKWALTER_COMPATABILITY_STORAGE_H_
#include <LingProc4/ArabicMorph/libbuckwalter/grammar.h>
#include <lib/libpts2/PatriciaTree.h>
#include <lib/fstorage/fstorage.h>

class TCompatibilityStorage {
public:
    bool Init(EMorphologyMode mode);
    bool AddComaptibleRule(TCharBuffer first, TCharBuffer second);
    bool IsCompatible(TCharBuffer first, TCharBuffer second) const;
    // \return section id after last used stored section, or kStoreError on error
    fstorage_section_id Store(fstorage* storage, fstorage_section_id startSection);
    // \return section id after last used stored section, or kStoreError on error
    fstorage_section_id Restore(fstorage* storage, fstorage_section_id startSection);
private:
    PatriciaTree    Storage;
};

#endif /* _LIBBUCKWALTER_COMPATABILITY_STORAGE_H_ */
