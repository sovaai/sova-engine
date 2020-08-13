#ifndef _LIBBUCKWALTER_FORMS_H_
#define _LIBBUCKWALTER_FORMS_H_
#include "utils.h"
#include <lib/aptl/avector.h>
#include <lib/fstorage/fstorage.h>
#include <_include/_inttype.h>
#include <set>
#include <vector>
#include <map>
#include <string>

typedef std::set<std::string>           TParts;
typedef std::map<std::string, TParts>   TPartToGrammarStorage;
const TParts* PartsFor(const std::string& grammar, const TPartToGrammarStorage& storage);

typedef std::pair<const std::string*, const std::string*>   TPrefixSuffixCompat;
typedef std::vector<TPrefixSuffixCompat>    TForms;
void FormsFinish(TForms& forms);

// format:
// 32b - elements count (ec)
// 32b * elements count - starts of packed strings
// elements count * (pascal string, pascal string)
bool BundleForms(const TForms& forms, avector<uint8_t>& bundle);

class TBundledForms {
public:
    typedef uint32_t                            TIndexShift;
    typedef std::pair<TCharBuffer, TCharBuffer> TForm;

    TBundledForms();
    TBundledForms(TCharBuffer bundle);
    bool Verify()const;
    TIndexShift Size()const;
    TForm Form(TIndexShift formId)const;
    TCharBuffer Buffer()const;
private:
    TCharBuffer Bundle;
};

class TPrefixSuffixStorage {
public:
    static const TBundledForms::TIndexShift KNullFormId;

    bool Init(EMorphologyMode mode);
    bool AddPair(TCharBuffer first, TCharBuffer second, TBundledForms::TIndexShift idx);
    TBundledForms::TIndexShift Index(TCharBuffer first, TCharBuffer second) const;
    // \return section id after last used stored section, or kStoreError on error
    fstorage_section_id Store(fstorage* storage, fstorage_section_id startSection);
    // \return section id after last used stored section, or kStoreError on error
    fstorage_section_id Restore(fstorage* storage, fstorage_section_id startSection);
private:
    PatriciaTree    Storage;
};

#endif /* _LIBBUCKWALTER_FORMS_H_ */
