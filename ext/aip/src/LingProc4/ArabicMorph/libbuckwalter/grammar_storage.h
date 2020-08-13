#ifndef _LIBBUCKWALTER_GRAMMAR_STORAGE_H_
#define _LIBBUCKWALTER_GRAMMAR_STORAGE_H_
#include "utils.h"
#include "grammar.h"
#include "lexemes.h"
#include <lib/libpts2/PatriciaTree.h>
#include <lib/fstorage/fstorage.h>

class TGrammarStorage {
public:
    bool Init(EMorphologyMode mode);
    bool AddVariants( TPartGrammarVariants& variants
                    , TLexemes* lexemes
                    , TStemsForLexemes* stemsForLexemes );
    TBundledVariantsHelper GetVariants(const uint8_t* key, uint32_t size);
    void DebugPrint();
    // \return section id after last used stored section, or kStoreError on error
    fstorage_section_id Store(fstorage* storage, fstorage_section_id startSection);
    // \return section id after last used stored section, or kStoreError on error
    fstorage_section_id Restore(fstorage* storage, fstorage_section_id startSection);
private:
    void ApplyZeroLengtVariants(TCharBuffer zeroVariants);
    PatriciaTree            Storage;
    avector<uint8_t>        ZeroLengthStorage;
    TBundledVariantsHelper  ZeroLengthVariants;
};
#endif /* _LIBBUCKWALTER_GRAMMAR_STORAGE_H_ */
