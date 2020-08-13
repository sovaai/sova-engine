#include "forms.h"
#include "fstorage_utils.h"
#include "pascalstrings.h"
#include <algorithm>
#include <cstring>
#include <limits>

const TBundledForms::TIndexShift TPrefixSuffixStorage::KNullFormId  =   std::numeric_limits<TBundledForms::TIndexShift>::max();

const TParts* PartsFor(const std::string& grammar, const TPartToGrammarStorage& storage)
{
    TPartToGrammarStorage::const_iterator   pos =   storage.find(grammar);
    if (storage.end() == pos) {
        return NULL;
    }
    return &pos->second;
}

bool compareForms(TPrefixSuffixCompat l, TPrefixSuffixCompat r)
{
    return ((*l.first < *r.first) || ((*l.first == *r.first) && (*l.second < *r.second)));
}

bool equalForms(TPrefixSuffixCompat l, TPrefixSuffixCompat r)
{
    return ((!compareForms(l, r)) && (!compareForms(r, l)));
}

void FormsFinish(TForms& forms)
{
    std::sort(forms.begin(), forms.end(), compareForms);
    forms.erase(std::unique(forms.begin(), forms.end(), equalForms), forms.end());
}


static const unsigned int kRatio    =   sizeof(TBundledForms::TIndexShift) / sizeof(uint8_t);


static TBundledForms::TIndexShift indexShift(TBundledForms::TIndexShift elementNumber)
{
    return kRatio * (elementNumber + 1);
}

static TBundledForms::TIndexShift* indexPos(uint8_t* indexStart, TBundledForms::TIndexShift elementNumber)
{
    return reinterpret_cast<TBundledForms::TIndexShift*>(indexStart + indexShift(elementNumber));
}

bool BundleForms(const TForms& forms, avector<uint8_t>& bundle)
{
    TBundledForms::TIndexShift    formCount  =   forms.size();
    bundle.resize(indexShift(0));
    memcpy(bundle.get_buffer(), static_cast<void*>(&formCount), sizeof(formCount));

    bundle.resize(indexShift(formCount));
    TBundledForms::TIndexShift  shift   =   bundle.size();
    for (unsigned int formNum = 0; formNum != formCount; ++formNum) {
        TBundledForms::TIndexShift*   indexPtr    =   indexPos(bundle.get_buffer(), formNum);
        memcpy(indexPtr, static_cast<void*>(&shift), sizeof(shift));
        const TPrefixSuffixCompat   form(forms[formNum]);
        shift   +=  pascal_size(*form.first) + pascal_size(*form.second);
    }
    bundle.resize(shift);
    for (unsigned int formNum = 0; formNum != formCount; ++formNum) {
        TBundledForms::TIndexShift  shift   =   *indexPos(bundle.get_buffer(), formNum);
        const TPrefixSuffixCompat   form(forms[formNum]);
        pascal_write(*form.first, bundle.get_buffer() + shift);
        pascal_write(*form.second, bundle.get_buffer() + shift + pascal_size(*form.first));
    }
    return true;
}

static const uint8_t        kEmptyForms[]       =   {0, 0, 0, 0};
static const TCharBuffer    kEmptyFormsBuffer   =   TCharBuffer(kEmptyForms, kEmptyForms + 4);

TBundledForms::TBundledForms()
: Bundle(kEmptyFormsBuffer)
{}

TBundledForms::TBundledForms(TCharBuffer bundle)
: Bundle(bundle)
{}

bool TBundledForms::Verify()const
{
    TIndexShift size    =   Size();
    bool        ret     =   true;
    TIndexShift formNum =   0;
    while ((true == ret) && (formNum != size)) {
        TForm   form    =   Form(formNum);
        ret     =   ((Bundle.End > form.first.End) && (Bundle.End > form.second.End));
        formNum +=  1;
    }
    return ret;
}

TBundledForms::TIndexShift TBundledForms::Size()const
{
    return *reinterpret_cast<const TIndexShift*>(Bundle.Begin);
}

TBundledForms::TForm TBundledForms::Form(TBundledForms::TIndexShift formId)const
{
    TIndexShift shift   =   *reinterpret_cast<const TBundledForms::TIndexShift*>(Bundle.Begin + indexShift(formId));
    TForm       form;
    form.first  =   pascal_get(Bundle.Begin + shift);
    form.second =   pascal_get(form.first.End);

    return form;
}

static void glue(TCharBuffer first, TCharBuffer second, avector<uint8_t>& result)
{
    static const uint8_t    kConnectionSymbol   =   '+';
    result.clear();
    result.resize(first.Size() + 1 + second.Size());
    memcpy(result.get_buffer(), first.Begin, first.Size());
    result[first.Size()]    =   kConnectionSymbol;
    memcpy(result.get_buffer() + first.Size() + 1, second.Begin, second.Size());
}

bool TPrefixSuffixStorage::Init(EMorphologyMode mode)
{
    PatriciaTreeCore::ResultStatus  res =   PatriciaTreeCore::RESULT_STATUS_SUCCESS;
    if (EBuildMorphology == mode) {
        res =   Storage.create(kTreeFlags);
    }
    return res == PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

static void putValue(uint8_t* buffer, TBundledForms::TIndexShift value)
{
    buffer[0]   =   0;
    memcpy(buffer + 1, &value, sizeof(value));
}

static TBundledForms::TIndexShift getValue(const uint8_t* buffer)
{
    return *reinterpret_cast<const TBundledForms::TIndexShift*>(buffer + 1);
}

TCharBuffer TBundledForms::Buffer()const
{
    return Bundle;
}

bool TPrefixSuffixStorage::AddPair(TCharBuffer first, TCharBuffer second, TBundledForms::TIndexShift idx)
{
    if (idx == KNullFormId) {
        return false;
    }

    if (KNullFormId != Index(first, second)) {
        return false;
    }

    avector<uint8_t>    key;
    glue(first, second, key);
    uint8_t             value[kRatio + 1];
    putValue(value, idx);
    const uint8_t*      valuePtr    =   value;
    PatriciaTreeCore::ResultStatus  res =   Storage.append( key.get_buffer()
                                                          , key.size()
                                                          , &valuePtr
                                                          , kRatio + 1 );
    return res == PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

TBundledForms::TIndexShift TPrefixSuffixStorage::Index(TCharBuffer first, TCharBuffer second) const
{
    avector<uint8_t>    key;
    glue(first, second, key);
    const uint8_t*                  value   =   NULL;
    size_t                          size    =   0;
    PatriciaTreeCore::ResultStatus  res     =   Storage.lookupExact( key.get_buffer()
                                                                   , key.size()
                                                                   , &value
                                                                   , &size );
    if (  (NULL == value)
       || (kRatio + 1 != size)
       || (PatriciaTreeCore::RESULT_STATUS_SUCCESS != res) ) {
        return KNullFormId;
    }

    return getValue(value);
}


fstorage_section_id TPrefixSuffixStorage::Store(fstorage* storage, fstorage_section_id startSection)
{
    return StorePatriciaTree(Storage, storage, startSection);
}

fstorage_section_id TPrefixSuffixStorage::Restore(fstorage* storage, fstorage_section_id startSection)
{
    return RestorePatriciaTree(Storage, kTreeFlags, storage, startSection);
}
