#include "morphology.h"
#include "fstorage_utils.h"
#include <LingProc4/LanguageInfo.h>

const char      TBuckwalterMorphology::KArabicMarker[]      =   "BUCKWALTER ARABIC MORPHOLOGY";
const size_t    TBuckwalterMorphology::KArabicMarkerSize    =   sizeof(TBuckwalterMorphology::KArabicMarker) / sizeof(TBuckwalterMorphology::KArabicMarker[0]);
const size_t    kHeaderSize                                 =   TBuckwalterMorphology::KArabicMarkerSize + sizeof(fstorage_section_id);

ECompatTypes WhichCompat(EGrammarTypes fType, EGrammarTypes sType)
{
    return static_cast<ECompatTypes>(fType + sType - 1);
}

bool TBuildBuckwalterMorphology::Init()
{
    bool    res =   true;

    for (unsigned int gramNum = EPrefix; gramNum != EGrammarTypesSize; ++gramNum) {
        res &=  Grammars[gramNum].Init(EBuildMorphology);
    }

    for (unsigned int compatNum = EPrefixNStem; compatNum != ECompatTypesSize; ++compatNum) {
        res &=  Compats[compatNum].Init(EBuildMorphology);
    }

    res &=  PrefixNSuffix.Init(EBuildMorphology);

    for (unsigned int compatNum = EPrefixNStem; compatNum != ECompatTypesSize; ++compatNum) {
        res &=  PartsWoWth[compatNum].Init(EBuildMorphology);
    }

    return res;
}

static fstorage_section_id writeHeaderSection(fstorage* storage, avector<char>& headerStorage, fstorage_section_id startSection)
{
    const LanguageInfo* arabicInfo      =   LanguageInfo::getLanguageInfo(LNG_ARABIC);
    if (NULL == arabicInfo) {
        return kStoreError;
    }
    fstorage_section_id headerSectionId =   LanguageInfo::getLPSectionStrict(arabicInfo->fsectionOffset);
    fstorage_section*   headerSection   =   fstorage_get_sized_section( storage
                                                                      , headerSectionId
                                                                      , kHeaderSize );
    if (NULL == headerSection) {
        return kStoreError;
    }

    if (headerSectionId == startSection) {
        startSection    +=  1;
    }

    headerStorage.resize(kHeaderSize);
    char*   headerPtr   =   headerStorage.get_buffer();

    memcpy( headerPtr
          , TBuckwalterMorphology::KArabicMarker
          , TBuckwalterMorphology::KArabicMarkerSize );
    memcpy( headerPtr + TBuckwalterMorphology::KArabicMarkerSize
          , &startSection
          , sizeof(startSection) );
    int result  =   fstorage_section_set_data( headerSection
                                             , const_cast<void*>(reinterpret_cast<const void*>(headerPtr))
                                             , kHeaderSize );
    if (FSTORAGE_OK != result) {
        return kStoreError;
    }

    return startSection;
}

bool TBuildBuckwalterMorphology::Store(fstorage* storage, fstorage_section_id startSection)
{
    fstorage_section_id currentSection  =   writeHeaderSection(storage, HeaderStorage, startSection);
    bool                isStored        =   kStoreError != currentSection;

    unsigned int    gramNum     =   EPrefix;
    while ((isStored) && (gramNum != EGrammarTypesSize)) {
        currentSection  =   Grammars[gramNum].Store(storage, currentSection);
        isStored        =   kStoreError != currentSection;
        gramNum         +=  1;
    }

    unsigned int    compatNum   =   EPrefixNStem;
    while ((isStored) && (compatNum != ECompatTypesSize)) {
        currentSection  =   Compats[compatNum].Store(storage, currentSection);
        isStored        =   kStoreError != currentSection;
        compatNum       +=  1;
    }

    LexemeBundle.resize(TBundledStemsPacks::MemorySizeFor(StemsForLexems));
    TBundledStemsPacks::DoBundle(StemsForLexems, LexemeBundle.get_buffer());
    currentSection  =   StoreAvector(LexemeBundle, storage, currentSection);
    isStored        &=  kStoreError != currentSection;

    BundleForms(Forms, FormBundle);
    currentSection  =   StoreAvector(FormBundle, storage, currentSection);
    isStored        &=  kStoreError != currentSection;

    currentSection  =   PrefixNSuffix.Store(storage, currentSection);
    isStored        &=  kStoreError != currentSection;

    unsigned int partNum    =   EPrefix;
    while ((isStored) && (partNum != EGrammarTypesSize)) {
        currentSection  =   PartsWoWth[partNum].Store(storage, currentSection);
        isStored        =   kStoreError != currentSection;
        partNum         +=  1;
    }

    return isStored;
}

static fstorage_section_id readHeaderSection(fstorage* storage)
{
    const LanguageInfo* arabicInfo      =   LanguageInfo::getLanguageInfo(LNG_ARABIC);
    if (NULL == arabicInfo) {
        return kStoreError;
    }
    fstorage_section_id headerSectionId =   LanguageInfo::getLPSectionStrict(arabicInfo->fsectionOffset);
    fstorage_section*   headerSection   =   fstorage_find_section(storage, headerSectionId);
    if (NULL == headerSection) {
        return kStoreError;
    }
    char*   header          =   static_cast<char*>(fstorage_section_get_all_data(headerSection));
    size_t  headerLength    =   fstorage_section_get_size(headerSection);
    if (kHeaderSize != headerLength) {
        return kStoreError;
    }

    if (0 != memcmp(TBuckwalterMorphology::KArabicMarker, header, TBuckwalterMorphology::KArabicMarkerSize)) {
        return kStoreError;
    }

    fstorage_section_id startSection    =   *reinterpret_cast<fstorage_section_id*>(header + TBuckwalterMorphology::KArabicMarkerSize);
    if (headerSectionId == startSection) {
        startSection    +=  1;
    }

    return startSection;
}

bool TBuckwalterMorphology::Restore(fstorage* storage)
{
    fstorage_section_id currentSection  =   readHeaderSection(storage);
    bool                isRestored      =   kStoreError != currentSection;

    unsigned int    gramNum     =   EPrefix;
    while ((isRestored) && (gramNum != EGrammarTypesSize)) {
        currentSection  =   Grammars[gramNum].Restore(storage, currentSection);
        isRestored      =   kStoreError != currentSection;
        gramNum         +=  1;
    }

    unsigned int    compatNum   =   EPrefixNStem;
    while ((isRestored) && (compatNum != ECompatTypesSize)) {
        currentSection  =   Compats[compatNum].Restore(storage, currentSection);
        isRestored      =   kStoreError != currentSection;
        compatNum       +=  1;
    }

    TCharBuffer lexemeBuffer;
    currentSection  =   RestoreAvector(lexemeBuffer, storage, currentSection);
    isRestored      &=  kStoreError != currentSection;
    Lexemes         =   TBundledStemsPacks(lexemeBuffer);

    TCharBuffer formBuffer;
    currentSection  =   RestoreAvector(formBuffer, storage, currentSection);
    isRestored      &=  kStoreError != currentSection;
    Forms           =   TBundledForms(formBuffer);

    currentSection  =   PrefixNSuffix.Restore(storage, currentSection);
    isRestored      &=  kStoreError != currentSection;

    unsigned int partNum    =   EPrefix;
    while ((isRestored) && (partNum != EGrammarTypesSize)) {
        currentSection  =   PartsWoWth[partNum].Restore(storage, currentSection);
        isRestored      =   kStoreError != currentSection;
        partNum         +=  1;
    }

    return isRestored;
}

//TODO remove TBuildBuckwalterMorphology::Store using TBuildBuckwalterMorphology::Transform
bool TBuckwalterMorphology::Store( fstorage* storage
                                 , fstorage_section_id startSection )
{
    fstorage_section_id currentSection  =   writeHeaderSection(storage, HeaderStorage, startSection);
    bool                isStored        =   kStoreError != currentSection;

    unsigned int    gramNum     =   EPrefix;
    while ((isStored) && (gramNum != EGrammarTypesSize)) {
        currentSection  =   Grammars[gramNum].Store(storage, currentSection);
        isStored        =   kStoreError != currentSection;
        gramNum         +=  1;
    }

    unsigned int    compatNum   =   EPrefixNStem;
    while ((isStored) && (compatNum != ECompatTypesSize)) {
        currentSection  =   Compats[compatNum].Store(storage, currentSection);
        isStored        =   kStoreError != currentSection;
        compatNum       +=  1;
    }

    currentSection  =   StoreBuffer(Lexemes.Buffer(), storage, currentSection);
    isStored        =   kStoreError != currentSection;

    currentSection  =   StoreBuffer(Forms.Buffer(), storage, currentSection);
    isStored        =   kStoreError != currentSection;

    currentSection  =   PrefixNSuffix.Store(storage, currentSection);
    isStored        &=  kStoreError != currentSection;

    unsigned int partNum    =   EPrefix;
    while ((isStored) && (partNum != EGrammarTypesSize)) {
        currentSection  =   PartsWoWth[partNum].Store(storage, currentSection);
        isStored        =   kStoreError != currentSection;
        partNum         +=  1;
    }

    return isStored;
}

bool TBuckwalterMorphology::Init(fstorage* storage)
{
    bool            res     =   true;

    for (unsigned int gramNum = EPrefix; gramNum != EGrammarTypesSize; ++gramNum) {
        res &=  Grammars[gramNum].Init(EUseMorphology);
    }

    for (unsigned int compatNum = EPrefixNStem; compatNum != ECompatTypesSize; ++compatNum) {
        res &=  Compats[compatNum].Init(EUseMorphology);
    }

    res &=  PrefixNSuffix.Init(EUseMorphology);

    for (unsigned int compatNum = EPrefixNStem; compatNum != ECompatTypesSize; ++compatNum) {
        res &=  PartsWoWth[compatNum].Init(EUseMorphology);
    }

    res &=  Restore(storage);

    return res;
}

int TBuckwalterMorphology::Analyze(TConstString str, IAnalyzeResultsFunctor& handler)
{
    static const size_t kMaxPrefixSize  =   4;
    static const size_t kMinPrefixSize  =   0;

    static const size_t kMaxSuffixSize  =   6;
    static const size_t kMinSuffixSize  =   0;

    static const size_t kMinStemSize    =   1;

    bool    isContinue      =   true;
    int     variantsCount   =   0;

    if(!handler.NewWord()) {
        return 0;
    }

    unsigned int    strLenInSymbols =   str.Size() / 2;

    for (size_t prefixLen = kMinPrefixSize; prefixLen <= kMaxPrefixSize; ++prefixLen) {

        if (!isContinue) {
            break;
        }

        if (prefixLen > strLenInSymbols) {
            continue;
        }

        TBundledVariantsHelper  prefVariants    =   Grammars[EPrefix].GetVariants( reinterpret_cast<const uint8_t*>(str.Begin)
                                                                                 , prefixLen * 2 );
        if (prefVariants.IsEmpty()) {
            continue;
        }

        for (size_t suffixLen = kMinSuffixSize; suffixLen <= kMaxSuffixSize; ++suffixLen) {

            if (!isContinue) {
                break;
            }

            if (prefixLen + suffixLen + kMinStemSize > strLenInSymbols) {
                continue;
            }

            TBundledVariantsHelper  sufVariants =   Grammars[ESuffix].GetVariants( reinterpret_cast<const uint8_t*>(str.End - suffixLen * 2)
                                                                                 , suffixLen * 2 );

            if (sufVariants.IsEmpty()) {
                continue;
            }

            TBundledVariantsHelper  stemVariants    =   Grammars[EStem].GetVariants( reinterpret_cast<const uint8_t*>(str.Begin + prefixLen * 2)
                                                                                   , (strLenInSymbols - suffixLen - prefixLen) * 2 );

            if (stemVariants.IsEmpty()) {
                continue;
            }

            isContinue  =   Combine(prefVariants, stemVariants, sufVariants, &variantsCount, handler);
        }
    }
    return variantsCount;
}

bool TBuckwalterMorphology::Combine( TBundledVariantsHelper prefixVariants
                                   , TBundledVariantsHelper stemVariants
                                   , TBundledVariantsHelper suffixVariants
                                   , int* variantsCount
                                   , IAnalyzeResultsFunctor& handler)
{
    bool    isContinue  =   true;
    for (size_t prefNum = 0; prefNum != prefixVariants.Size(); ++prefNum) {

        if (!isContinue) {
            break;
        }

        TBundledGrammarHandler  prefGrammar =   prefixVariants.Variant(prefNum);

        for (size_t stemNum = 0; stemNum != stemVariants.Size(); ++stemNum) {

            if (!isContinue) {
                break;
            }

            TBundledGrammarHandler  stemGrammar =   stemVariants.Variant(stemNum);

            if (!Compats[EPrefixNStem].IsCompatible( prefGrammar.GetField(MorphoCategoryNum)
                                                   , stemGrammar.GetField(MorphoCategoryNum) )) {
                continue;
            }

            for (size_t sufNum = 0; sufNum != suffixVariants.Size(); ++sufNum) {

                if (!isContinue) {
                    break;
                }

                TBundledGrammarHandler  sufGrammar  =   suffixVariants.Variant(sufNum);

                if (!Compats[EPrefixNSuffix].IsCompatible( prefGrammar.GetField(MorphoCategoryNum)
                                                         , sufGrammar.GetField(MorphoCategoryNum) )) {
                    continue;
                }

                if (!Compats[EStemNSuffix].IsCompatible( stemGrammar.GetField(MorphoCategoryNum)
                                                       , sufGrammar.GetField(MorphoCategoryNum) )) {
                    continue;
                }

                *variantsCount  +=  1;
                isContinue      =   handler.NextGrammar(TWordGrammar(prefGrammar, stemGrammar, sufGrammar));
            }
        }
    }
    return isContinue;
}

TBundledStems TBuckwalterMorphology::Lexeme(TBundledStemsPacks::TShift lexemeId)const
{
    if (lexemeId >= Lexemes.Size()) {
        return TCharBuffer();
    }

    return Lexemes.Stems(lexemeId);
}
TBundledStems TBuckwalterMorphology::Lexeme(TWordGrammar word)const
{
    return Lexeme(word.StemGrammar.Id());
}

TBundledForms::TForm TBuckwalterMorphology::Form(TBundledForms::TIndexShift id)const
{
    if (id >= Forms.Size()) {
        return TBundledForms::TForm();
    }

    return Forms.Form(id);
}

TBundledForms::TIndexShift TBuckwalterMorphology::FormIdx(TWordGrammar word)const
{
    return PrefixNSuffix.Index( word.PrefixGrammar.GetField(EntryWthVowelsNum)
                              , word.SuffixGrammar.GetField(EntryWthVowelsNum) );
}

size_t TBuckwalterMorphology::Size()const
{
    return Lexemes.Size();
}

TBundledStemsPacks::TShift TBuckwalterMorphology::LexemeId(TWordGrammar word)
{
    return word.StemGrammar.Id();
}

class TStemCapturer: public TBuckwalterMorphology::IAnalyzeResultsFunctor {
public:
    TStemCapturer( TCharBuffer stem
                 , const TBuckwalterMorphology& morphology
                 , TBundledForms::TIndexShift formId
                 , TWordGrammar& word )
    : Stem(stem)
    , Morphology(morphology)
    , FormId(formId)
    , Word(word)
    , IsCaptureSmth(false)
    {}

    bool NewWord()
    {
        return true;
    }

    bool IsOk(TWordGrammar word)
    {
        return ( Stem.Cmpbody(word.StemGrammar.GetField(EntryWthVowelsNum))
               && Morphology.FormIdx(word) == FormId );
    }

    bool NextGrammar(TWordGrammar word)
    {
        if (IsOk(word)) {
            Word            =   word;
            IsCaptureSmth   =   true;
        }
        return !IsCaptureSmth;
    }

    bool IsWord()const
    {
        return IsCaptureSmth;
    }

private:
    TCharBuffer                     Stem;
    const TBuckwalterMorphology&    Morphology;
    TBundledForms::TIndexShift      FormId;
    TWordGrammar&                   Word;
    bool                            IsCaptureSmth;
};

class TFormGenerator {
public:
    TFormGenerator(const TBundledStems& prefixes, const TBundledStems& suffixes)
    : Prefixes(prefixes)
    , PrefixNum(0)
    , Suffixes(suffixes)
    , SuffixNum(0)
    {}

    TBundledForms::TForm    NextWo();

    static bool IsValid(TBundledForms::TForm form);

private:
    const TBundledStems&    Prefixes;
    TBundledStems::TShift   PrefixNum;
    const TBundledStems&    Suffixes;
    TBundledStems::TShift   SuffixNum;
};

TBundledForms::TForm TFormGenerator::NextWo()
{
    TBundledForms::TForm    form;
    if (Suffixes.Size() <= SuffixNum) {
        return form;
    }

    form.first  =   Prefixes.Stem(PrefixNum);
    form.second =   Suffixes.Stem(SuffixNum);

    PrefixNum   +=  1;
    if (Prefixes.Size() <= PrefixNum) {
        PrefixNum   =   0;
        SuffixNum   +=  1;
    }

    return form;
}

bool TFormGenerator::IsValid(TBundledForms::TForm form)
{
    return ((!form.first.IsInvalid()) && (!form.second.IsInvalid()));
}

bool TBuckwalterMorphology::MaterializeWord( TCharBuffer stem
                                           , TBundledForms::TIndexShift formId
                                           , TWordGrammar& word )
{
    TBundledForms::TForm    form        =   Form(formId);

    if ((form.first.IsInvalid()) || (form.second.IsInvalid())) {
        return false;
    }

    TBundledStems   prefixWo    =   PartsWoWth[EPrefix].GetWo(form.first.Begin, form.first.Size());
    TBundledStems   suffixWo    =   PartsWoWth[ESuffix].GetWo(form.second.Begin, form.second.Size());
    TFormGenerator  formsWo(prefixWo, suffixWo);

    bool    isNextForm  =   true;
    bool    isFindSmth  =   false;
    while ((isNextForm) && (!isFindSmth)) {

        TBundledForms::TForm    formWo  =   formsWo.NextWo();

        if (!formsWo.IsValid(formWo)) {
            isNextForm  =   false;
            continue;
        }

        TBundledVariantsHelper  preVariants =   Grammars[EPrefix].GetVariants( formWo.first.Begin
                                                                             , formWo.first.Size() );
        TBundledVariantsHelper  sufVariants =   Grammars[ESuffix].GetVariants( formWo.second.Begin
                                                                             , formWo.second.Size() );

        TBundledStems   stemsWo     =   PartsWoWth[EStem].GetWo(stem.Begin, stem.Size());
        unsigned int    stemWoNum   =   0;

        while ((!isFindSmth) && (stemWoNum < stemsWo.Size())) {
            TCharBuffer             stemWo          =   stemsWo.Stem(stemWoNum);

            TBundledVariantsHelper  stemVariants    =   Grammars[EStem].GetVariants( stemWo.Begin
                                                                                   , stemWo.Size() );
            //just to conform interface, nothing else
            int                     variantCount    =   0;

            TStemCapturer   capturer(stem, *this, formId, word);
            Combine(preVariants, stemVariants, sufVariants, &variantCount, capturer);

            isFindSmth  =   capturer.IsWord();
            stemWoNum   +=  1;
        }
    }
    return isFindSmth;
}

class TLexemeIdCapturer: public TBuckwalterMorphology::IAnalyzeResultsFunctor {
public:
    TLexemeIdCapturer( TBundledStemsPacks::TShift lexemeId
                     , const TBuckwalterMorphology& morphology
                     , TBundledForms::TIndexShift formId
                     , TWordGrammar& word )
    : LexemeId(lexemeId)
    , Morphology(morphology)
    , FormId(formId)
    , Word(word)
    , IsCaptureSmth(false)
    {}

    bool NewWord()
    {
        return true;
    }

    bool IsOk(TWordGrammar word)
    {
        return ( LexemeId == TBuckwalterMorphology::LexemeId(word)
               && Morphology.FormIdx(word) == FormId );
    }

    bool NextGrammar(TWordGrammar word)
    {
        if (IsOk(word)) {
            Word            =   word;
            IsCaptureSmth   =   true;
        }
        return !IsCaptureSmth;
    }

    bool IsWord()const
    {
        return IsCaptureSmth;
    }

private:
    TBundledStemsPacks::TShift      LexemeId;
    const TBuckwalterMorphology&    Morphology;
    TBundledForms::TIndexShift      FormId;
    TWordGrammar&                   Word;
    bool                            IsCaptureSmth;
};

bool TBuckwalterMorphology::MaterializeWord( TBundledStemsPacks::TShift lexemeId
                                           , TBundledForms::TIndexShift formId
                                           , TWordGrammar& word )
{
    TBundledForms::TForm    form        =   Form(formId);
    if ((form.first.IsInvalid()) || (form.second.IsInvalid())) {
        return false;
    }

    TBundledStems   prefixWo    =   PartsWoWth[EPrefix].GetWo(form.first.Begin, form.first.Size());
    TBundledStems   suffixWo    =   PartsWoWth[ESuffix].GetWo(form.second.Begin, form.second.Size());
    TFormGenerator  formsWo(prefixWo, suffixWo);

    bool    isNextForm  =   true;
    bool    isFindSmth  =   false;
    while ((isNextForm) && (!isFindSmth)) {

        TBundledForms::TForm    formWo  =   formsWo.NextWo();

        if (!formsWo.IsValid(formWo)) {
            isNextForm  =   false;
            continue;
        }

        TBundledVariantsHelper  preVariants =   Grammars[EPrefix].GetVariants( formWo.first.Begin
                                                                             , formWo.first.Size() );
        TBundledVariantsHelper  sufVariants =   Grammars[ESuffix].GetVariants( formWo.second.Begin
                                                                             , formWo.second.Size() );
        TBundledStems       stems           =   Lexeme(lexemeId);
        unsigned int        stemsSize       =   stems.Size();
        unsigned int        stemsNum        =   0;
        int                 variantsCount   =   0;
        TLexemeIdCapturer   capturer(lexemeId, *this, formId, word);

        while ((!isFindSmth) && (stemsNum < stemsSize)) {
            TCharBuffer             stem            =   stems.Stem(stemsNum);
            TBundledVariantsHelper  stemVariants    =   Grammars[EStem].GetVariants( stem.Begin
                                                                                   , stem.Size() );
            Combine(preVariants, stemVariants, sufVariants, &variantsCount, capturer);
            isFindSmth  =   capturer.IsWord();
            stemsNum    +=  1;
        }
    }

    return isFindSmth;
}

bool TBuckwalterMorphology::Enumerate(IAnalyzeResultsFunctor& functor)
{
    bool            isContinue  =   true;
    TWordGrammar    grammar;
    for (unsigned int lexemeId = 0; lexemeId != Lexemes.Size(); ++lexemeId) {
        isContinue  &=  functor.NewWord();
        if (!isContinue) {
            break;
        }
        for (unsigned int formId = 0; formId != Forms.Size(); ++formId) {
            if (!isContinue) {
                break;
            }
            if (MaterializeWord(lexemeId, formId, grammar)) {
                isContinue  &=  functor.NextGrammar(grammar);
            }
        }
    }
    return isContinue;
}

int TBuckwalterMorphology::MaterializeWord(TWordGrammar& grammar, char* buffer, int bufferSize)
{
    TPartGrammarFieldOrder  fieldToRestore  =   EntryWthVowelsNum;
    if (true) {
        fieldToRestore  =   EntryWoVowelsNum;
    }

    TCharBuffer             prefix          =   grammar.PrefixGrammar.GetField(fieldToRestore);
    TCharBuffer             stem            =   grammar.StemGrammar.GetField(fieldToRestore);
    TCharBuffer             suffix          =   grammar.SuffixGrammar.GetField(fieldToRestore);
    int                     requiredSize    =   prefix.Size() + stem.Size() + suffix.Size();
    if (bufferSize < requiredSize) {
        return requiredSize;
    }

    char*   pos =   buffer;
    memcpy(pos, prefix.Begin, prefix.Size());
    pos +=  prefix.Size();
    memcpy(pos, stem.Begin, stem.Size());
    pos +=  stem.Size();
    memcpy(pos, suffix.Begin, suffix.Size());
    pos +=  suffix.Size();

    if (bufferSize < requiredSize + 1) {
        return requiredSize;
    }

    *pos    =   '\0';
    return requiredSize;
}

TBundledForms::TIndexShift TBuckwalterMorphology::FormCount()const
{
    return Forms.Size();
}
