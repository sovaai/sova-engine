#include "LPArabicMorph.h"
#include <LingProc4/LanguageInfo.h>
#include <_include/ntoh.h>
#include <limits>
#include <algorithm>

const TBundledForms::TIndexShift    TLPArabicMorph::KDictionaryFormCommand    =   std::numeric_limits<TBundledForms::TIndexShift>::max();

static LEXID applyFormId(LEXID lexId, TBundledForms::TIndexShift formId)
{
    LEXID       ret     =   lexId;
    uint16_t    form    =   formId % 1024;
    ret =   LexSetForm(ret, form);
    uint8_t     formEnd =   formId / 1024;
    ret =   LexSetOrthVar(ret, formEnd);
    return ret;
}

static TBundledForms::TIndexShift extractFormId(LEXID lexId)
{
    uint8_t     formEnd =   LexOrthVar(lexId);
    uint16_t    form    =   LexIdForm(lexId);
    return (formEnd * 1024) + form;
}

LingProcErrors TLPArabicMorph::Init( const UCharSet *charset, const LPMorphDictInfo &info )
{
    if (NULL == charset) {
        return LP_ERROR_EINVAL;
    }

    _charset    =   charset;
    _info       =   info;
    LangInfo    =   _info.internalNumber;

    TBundledForms::TIndexShift  internalFormCount   =   Morphology.FormCount();
    TBundledForms::TIndexShift  nextExtendedFormId  =   Morphology.FormCount();
    InternalToExternal.resize(internalFormCount);
    ExternalToInternal.resize(internalFormCount);
    for (TBundledForms::TIndexShift  formId = 0; formId != internalFormCount; ++formId) {
        LEXID   morphNo     =   LexSetNoMrph(0, LangInfo);
        LEXID   applied     =   applyFormId(morphNo, formId);
        LEXID   setDictForm =   _LexSetDictForm(applied);
        if (setDictForm == applied) {
            InternalToExternal[formId]  =   nextExtendedFormId;
            if (ExternalToInternal.size() <= nextExtendedFormId) {
                ExternalToInternal.resize(nextExtendedFormId + 1);
            }
            ExternalToInternal[nextExtendedFormId]  =   formId;
            ExternalToInternal[formId]              =   KDictionaryFormCommand;
            nextExtendedFormId                      +=  1;
        }
        else {
            InternalToExternal[formId]  =   formId;
            ExternalToInternal[formId]  =   formId;
        }
    }

    WordNormalizer* normalizerPtr   =   WordNormalizer::factory( LNG_ARABIC
                                                               , MORPH_TYPE_STRICT );

    if (NULL == normalizerPtr) {
        return LP_ERROR_EFAULT;
    }

    WordNormalizerResult    status  =   normalizerPtr->create(_charset, _info.creationFlags);
    if (NORMALIZE_SUCCESS != status) {
        return LP_ERROR_EFAULT;
    }

    Normalizer.Set(normalizerPtr);

    return LP_OK;
}

template<class T>
static void store(T what, uint32_t*& where)
{
    uint32_t    what32  =   static_cast<uint32_t>(what);
    where[0]    =   htobe32(what32);
    where       +=  1;
}

template<class T>
static void restore(T& what, uint32_t*& where)
{
    uint32_t    what32  =   *where;
    what    =   static_cast<T>(be32toh(what32));
    where   +=  1;
}

static const size_t kInfoSize   =   sizeof(uint32_t) * 8; //count of LPMorphDictInfo fields
LingProcErrors TLPArabicMorph::FSSave( fstorage* fs
                                     , fstorage_section_id sectionId
                                     , bool /*transport*/ )
{
    HeaderStorage.resize(kInfoSize);
    uint32_t*   headerPtr   =   HeaderStorage.get_buffer();

    store(_info.type, headerPtr);
    store(_info.version, headerPtr);
    store(_info.variant, headerPtr);
    store(_info.lang, headerPtr);
    store(_info.globalId, headerPtr);
    store(_info.internalNumber, headerPtr);
    store(_info.minStemLength, headerPtr);
    store(_info.creationFlags, headerPtr);

    fstorage_section*   section =   fstorage_get_sized_section( fs
                                                              , sectionId
                                                              , kInfoSize );
    if (NULL == section) {
        return LP_ERROR_FS_FAILED;
    }

    int writeStatus =   fstorage_section_set_data( section
                                                 , static_cast<void*>(HeaderStorage.get_buffer())
                                                 , HeaderStorage.size() );
    if (FSTORAGE_OK != writeStatus) {
        return LP_ERROR_FS_FAILED;
    }

    return LP_OK;

}

LingProcErrors TLPArabicMorph::ReadInfo( const UCharSet* charset
                                       , fstorage* fs
                                       , fstorage_section_id sectionId )
{

    fstorage_section*   section     =   fstorage_find_section(fs, sectionId);
    if (NULL == section) {
        return LP_ERROR_FS_FAILED;
    }

    uint32_t*   headerPtr   =   static_cast<uint32_t*>(fstorage_section_get_all_data(section));

    if (NULL == headerPtr) {
        return LP_ERROR_FS_FAILED;
    }

    if (kInfoSize != fstorage_section_get_size(section)) {
        return LP_ERROR_FS_FAILED;
    }

    LPMorphDictInfo info;
    restore(info.type, headerPtr);
    restore(info.version, headerPtr);
    restore(info.variant, headerPtr);
    restore(info.lang, headerPtr);
    restore(info.globalId, headerPtr);
    restore(info.internalNumber, headerPtr);
    restore(info.minStemLength, headerPtr);
    restore(info.creationFlags, headerPtr);
    return Init(charset, info);
}

LingProcErrors TLPArabicMorph::FSOpenRW( const UCharSet* charset
                                       , fstorage* fs
                                       , fstorage_section_id sectionId )
{
    return ReadInfo(charset, fs, sectionId);
}

LingProcErrors TLPArabicMorph::FSOpenRO( const UCharSet* charset
                                       , fstorage* fs
                                       , fstorage_section_id sectionId )
{
    return ReadInfo(charset, fs, sectionId);
}

LingProcErrors TLPArabicMorph::create( const UCharSet *charset, const LPMorphCreateParams *params )
{
    if ( unlikely(params == 0 || charset == 0) )
        return LP_ERROR_EINVAL;

    if ( unlikely(params->type != MORPH_TYPE_STRICT) )
        return LP_ERROR_UNSUPPORTED_MORPH;


    /* Fill the LPMorphDictInfo info */
    LPMorphDictInfo info;

    info.type            = params->type;
    info.lang            = params->lang;
    info.globalId        = (_info.type << 24) | (_info.lang);
    info.internalNumber  = params->internalNumber;


    if (params->flags & MORPH_FLAG_USE_DEFAULT)
        return LP_ERROR_MORPH_FLAGS;

    info.creationFlags = params->flags;

    /* Clear all except only meaning flags */
    unsigned int storeFlags     =
        MORPH_FLAG_CASE_INSENSITIVE    |
        MORPH_FLAG_LWR_PUNCT           |
        MORPH_FLAG_REM_IGNORE          |
        MORPH_FLAG_NATIVE_ONLY         |
        MORPH_FLAG_PROCESS_COMPOUNDS   |
        MORPH_FLAG_ARABIC_SEQ_COLLAPSE;
    info.creationFlags &= storeFlags;
    return Init(charset, info);
}

TLPArabicMorph::TLPArabicMorph()
{
}

LingProcErrors TLPArabicMorph::close()
{
    Normalizer.Clear();
    return LP_OK;
}

LingProcErrors TLPArabicMorph::FSAttachStrictMorph( fstorage* fs )
{
    LingProcErrors  ret =   LP_OK;
    if(!Morphology.Init(fs)) {
        ret =   LP_ERROR_FS_FAILED;
    }
    return ret;
}

size_t TLPArabicMorph::size()
{
    return Morphology.Size();
}

class TBuckwalterToLPLemmaAdapter : public TBuckwalterMorphology::IAnalyzeResultsFunctor {
public:
    TBuckwalterToLPLemmaAdapter( LemmatizeResultFunctor& functor
                               , const TBuckwalterMorphology& morphology
                               , uint16_t langInfo
                               , const TLPArabicMorph::TFormMapper& internalToExternal )
    : Functor(functor)
    , Morphology(morphology)
    , LangInfo(langInfo)
    , InternalToExternal(internalToExternal)
    {}

    bool NewWord();
    bool NextGrammar(TWordGrammar word);

private:
    LemmatizeResultFunctor&             Functor;
    const TBuckwalterMorphology&        Morphology;
    uint16_t                            LangInfo;
    const TLPArabicMorph::TFormMapper&  InternalToExternal;
};


bool TBuckwalterToLPLemmaAdapter::NewWord()
{
    return LP_OK == Functor.reset();
}

static LEXID packLexId( TBundledStemsPacks::TShift lexemeId
                      , TBundledForms::TIndexShift formId
                      , uint16_t langCode
                      , const TLPArabicMorph::TFormMapper& internalToExternal )
{
    LEXID   ret =   lexemeId;
    ret =   LexSetNoMrph(ret, langCode);
    ret =   LexSetNoCase(ret);
    TBundledForms::TIndexShift  external    =   formId;
    if (formId < internalToExternal.size()) {
        external    =   internalToExternal[formId];
    }
    ret =   applyFormId(ret, external);
    return ret;
}

static void unpackLexid( LEXID lexId
                       , TBundledStemsPacks::TShift *lexemeId
                       , TBundledForms::TIndexShift *formId
                       , uint16_t *langCode
                       , const TLPArabicMorph::TFormMapper& externalToInternal )
{
    *lexemeId   =   LexIdNorm(lexId);
    *langCode   =   LexNoMrph(lexId);
    TBundledForms::TIndexShift  external    =   extractFormId(lexId);
    *formId =   external;
    if (external < externalToInternal.size()) {
        *formId =   externalToInternal[external];
    }
    return;
}

bool TBuckwalterToLPLemmaAdapter::NextGrammar(TWordGrammar word)
{
    TBundledStemsPacks::TShift  lexemeId    =   TBuckwalterMorphology::LexemeId(word);
    TBundledStemsPacks::TShift  formId      =   Morphology.FormIdx(word);
    LEXID                       lexId       =   packLexId(lexemeId, formId, LangInfo, InternalToExternal);
    LingProcErrors              res         =   Functor.nextLemma(lexId, 0, NULL);
    return LP_OK == res;
}

static const unsigned int       kMaxWordLength      =   128;//in bytes
//formula taken from LingProc4/WordNormalizer.h:57
//size_t outBufferSize = wordSize * 4 + 4;
static const unsigned int       kBufferSize         =   kMaxWordLength * 4 + 4;
int TLPArabicMorph::lemmatize( LemmatizeResultFunctor &functor
                             , const char *word
                             , size_t wordLength
                             , unsigned int /*flags*/ )
{
    //TODO handle flags
    if (kLengthUnknown == wordLength) {
        wordLength  =   strlen(word);
    }

    //there is no such long words in arabic language
    if (wordLength > kMaxWordLength) {
        return 0;
    }

    char                    woDiacritic[kBufferSize];
    WordNormalizerTarget    result(woDiacritic, kBufferSize);
    WordNormalizerResult    status  =   Normalizer->normalizeWord(result, word, wordLength);

    if (NORMALIZE_ERROR_NOT_NATIVE == status) {
        return 0;
    }

    if (NORMALIZE_SUCCESS != status) {
        return LP_ERROR_EFAULT;
    }

    size_t  woDiacriticLen  =   result.resultSize;

    if (0 == woDiacriticLen) {
        return 0;
    }

    TBuckwalterToLPLemmaAdapter adapter(functor, Morphology, LangInfo, InternalToExternal);
    TConstString                wordBuffer(woDiacritic, woDiacriticLen);
    int                         variantsCount   =   Morphology.Analyze(wordBuffer, adapter);

    return variantsCount;
}

class TBuckwalterToLPStemAdapter : public TBuckwalterMorphology::IAnalyzeResultsFunctor {
public:
    TBuckwalterToLPStemAdapter( StemmatizeResultFunctor& functor
                              , const TBuckwalterMorphology& morphology
                              , uint16_t langInfo
                              , const TLPArabicMorph::TFormMapper& internalToExternal )
    : Functor(functor)
    , Morphology(morphology)
    , LangInfo(langInfo)
    , InternalToExternal(internalToExternal)
    {}

    bool NewWord();
    bool NextGrammar(TWordGrammar word);

private:
    StemmatizeResultFunctor&            Functor;
    const TBuckwalterMorphology&        Morphology;
    uint16_t                            LangInfo;
    const TLPArabicMorph::TFormMapper&  InternalToExternal;
};

bool TBuckwalterToLPStemAdapter::NewWord()
{
    return LP_OK == Functor.reset();
}

static const char   kHexLine[]      =   "0123456789ABCDEF";
static const size_t kHexLineSize    =   sizeof(kHexLine) / sizeof(kHexLine[0]);
static const char*  kHexLineEnd     =   kHexLine + kHexLineSize;

static size_t toHex(TBundledStemsPacks::TShift lexemeId, char* buffer)
{
    TBundledStemsPacks::TShift  r   =   lexemeId;
    char*                       ptr =   buffer;
    while (r > 0) {
        *ptr    =   kHexLine[r % 16];
        ptr     +=  1;
        r       /=  16;
    }

    std::reverse(buffer, ptr);

    if (ptr == buffer) {
        *ptr    =   kHexLine[0];
        ptr     +=  1;
    }

    return ptr - buffer;
}

static bool fromHex(const char* buffer, size_t len, TBundledStemsPacks::TShift* lexemeId)
{
    TBundledStemsPacks::TShift  r       =   0;
    const char*                 begin   =   buffer;
    const char*                 end     =   begin + len;
    bool                        isOk    =   true;

    while ((isOk) && (begin != end)) {
        r       *=  16;
        const char* pos =   std::find(kHexLine, kHexLineEnd, *begin);

        if (kHexLineEnd == pos) {
            isOk    =   false;
            continue;
        }

        r       +=  std::distance(kHexLine, pos);
        begin   +=  1;
    }

    if (isOk) {
        *lexemeId   =   r;
    }

    return isOk;
}

bool TBuckwalterToLPStemAdapter::NextGrammar(TWordGrammar word)
{
    TBundledStemsPacks::TShift  formId      =   Morphology.FormIdx(word);
    TBundledStemsPacks::TShift  lexemeId    =   TBuckwalterMorphology::LexemeId(word);
    LEXID                       lexId       =   packLexId(0, formId, LangInfo, InternalToExternal);
    //we map 2^16 to hex
    char*                       buffer      =   Functor.getStemBuffer(4 + 1);
    if (NULL == buffer) {
        return false;
    }

    size_t  len =   toHex(lexemeId, buffer);
    buffer[len] =   '\0';
    LingProcErrors  res         =   Functor.nextStem(lexId, 0, buffer, len, NULL);
    return LP_OK == res;
}

int TLPArabicMorph::stemmatize( StemmatizeResultFunctor &functor
                              , const char              *word
                              , size_t                  wordLength
                              , unsigned int            /*flags*/ )
{
    if (kLengthUnknown == wordLength) {
        wordLength  =   strlen(word);
    }

    if (wordLength > kMaxWordLength) {
        return 0;
    }

    char                    woDiacritic[kBufferSize];
    WordNormalizerTarget    result(woDiacritic, kBufferSize);
    WordNormalizerResult    status  =   Normalizer->normalizeWord(result, word, wordLength);

    if (NORMALIZE_ERROR_NOT_NATIVE == status) {
        return 0;
    }

    if (NORMALIZE_SUCCESS != status) {
        return LP_ERROR_EFAULT;
    }

    size_t  woDiacriticLen  =   result.resultSize;

    if (0 == woDiacriticLen) {
        return 0;
    }

    TBuckwalterToLPStemAdapter  adapter(functor, Morphology, LangInfo, InternalToExternal);
    TConstString                wordBuffer(woDiacritic, woDiacriticLen);
    int                         variantsCount   =   Morphology.Analyze(wordBuffer, adapter);

    return variantsCount;
}

class TBuckwalterToLPNormalizeAdapter : public WordNormalizerFunctor {
public:

    TBuckwalterToLPNormalizeAdapter(NormalizeResultFunctor& functor, LEXID lexId)
    : Functor(functor)
    , LexId(lexId)
    , VariantCounter(0)
    {}

    char *getResultBuffer( size_t requestSize );
    WordNormalizerResult nextResult( const char         *result
                                    , size_t              resultSize
                                    , WordNormalizerCaps  caps );
    size_t  VariantsCount();

private:
    NormalizeResultFunctor& Functor;
    LEXID                   LexId;
    size_t                  VariantCounter;
};

char* TBuckwalterToLPNormalizeAdapter::getResultBuffer( size_t requestSize )
{
    return Functor.getResultBuffer(requestSize);
}

WordNormalizerResult TBuckwalterToLPNormalizeAdapter::nextResult( const char *result
                                                                , size_t resultSize
                                                                , WordNormalizerCaps  /*caps*/ )
{
    VariantCounter  +=  1;

    LingProcErrors          status  =   Functor.nextResult(LexId, result, resultSize);
    WordNormalizerResult    nStatus =   NORMALIZE_SUCCESS;
    if (LP_OK != status) {
        nStatus =   NORMALIZE_ERROR_INTERNAL;
    }
    return nStatus;
}

size_t TBuckwalterToLPNormalizeAdapter::VariantsCount()
{
    return VariantCounter;
}

int TLPArabicMorph::normalize( NormalizeResultFunctor &functor
                             , const char              *word
                             , size_t                  wordLength
                             , unsigned int            /*flags*/ )
{
    if (NULL == Normalizer.Get()) {
        return LP_ERROR_EFAULT;
    }

    LEXID   lexId   =   0;
    lexId   =   LexSetNoMrph(lexId, LangInfo);
    lexId   =   LexSetNoCase(lexId);

    if (NULL == word) {
        return LP_ERROR_EINVAL;
    }

    LingProcErrors  status  =   functor.reset();

    if (LP_OK != status) {
        return status;
    }

    TBuckwalterToLPNormalizeAdapter adapter(functor, lexId);
    WordNormalizerResult    nStatus =   Normalizer->normalizeWord( adapter
                                                                 , word
                                                                 , wordLength );

    if (NORMALIZE_ERROR_NOT_NATIVE == nStatus) {
        return 0;
    }

    if (NORMALIZE_SUCCESS != nStatus) {
        return LP_ERROR_EFAULT;
    }

    return adapter.VariantsCount();
}

int TLPArabicMorph::getLexText( TBundledStemsPacks::TShift lexemeId
                              , TBundledForms::TIndexShift formId
                              , char *buffer
                              , size_t bufferSize )
{
    TWordGrammar    grammar;
    bool            isFindSmth  =   false;

    if (formId == KDictionaryFormCommand) {
        //we think what dictionary form it is a form with minimal formId
        TBundledForms::TIndexShift mayBeForm = 0;
        while ((!isFindSmth) && (mayBeForm != Morphology.FormCount())) {
            isFindSmth  =   Morphology.MaterializeWord(lexemeId, mayBeForm, grammar);
            mayBeForm   +=  1;
        }
    }
    else {
        isFindSmth  =   Morphology.MaterializeWord(lexemeId, formId, grammar);
    }
    if (!isFindSmth) {
        return 0;
    }

    return Morphology.MaterializeWord(grammar, buffer, bufferSize);
}

int TLPArabicMorph::getLexText( LEXID lex, char *buffer, size_t bufferSize, const BinaryData* /*additionalData*/ )
{
    TBundledStemsPacks::TShift  lexemeId;
    TBundledForms::TIndexShift  formId;
    uint16_t                    langCode;
    unpackLexid(lex, &lexemeId, &formId, &langCode, ExternalToInternal);

    if (langCode != LangInfo) {
        return 0;
    }

    return getLexText(lexemeId, formId, buffer, bufferSize);
}


int TLPArabicMorph::getLexText( LEXID partialLex, const char *stem, char *buffer, size_t bufferSize, const BinaryData* /*additionalData*/ )
{
    TBundledStemsPacks::TShift  lexemeId;
    TBundledForms::TIndexShift  formId;
    uint16_t                    langCode;
    unpackLexid(partialLex, &lexemeId, &formId, &langCode, ExternalToInternal);

    if (langCode != LangInfo) {
        return 0;
    }

    int     stemLen =   strlen(stem);
    bool    isHex   =   fromHex(stem, stemLen, &lexemeId);

    if (isHex) {
        return getLexText(lexemeId, formId, buffer, bufferSize);
    }

    TCharBuffer                 stemBuffer( reinterpret_cast<const uint8_t*>(stem)
                                          , reinterpret_cast<const uint8_t*>(stem + stemLen) );
    TWordGrammar                grammar;
    bool                        isFindSmth  =   false;
    if (formId == KDictionaryFormCommand) {
        //we think what dictionary form it is a form with minimal formId
        TBundledForms::TIndexShift mayBeForm = 0;
        while ((!isFindSmth) && (mayBeForm != Morphology.FormCount())) {
            isFindSmth  =   Morphology.MaterializeWord(stemBuffer, mayBeForm, grammar);
            mayBeForm   +=  1;
        }
    }
    else {
        isFindSmth  =   Morphology.MaterializeWord(stemBuffer, formId, grammar);
    }

    if (!isFindSmth) {
        return 0;
    }

    return Morphology.MaterializeWord(grammar, buffer, bufferSize);
}

class TBuckwalterToLPEnumeratorAdapter : public TBuckwalterMorphology::IAnalyzeResultsFunctor {
public:
    TBuckwalterToLPEnumeratorAdapter( MorphEnumerator& functor
                                    , const TBuckwalterMorphology& morphology
                                    , uint16_t langInfo
                                    , const TLPArabicMorph::TFormMapper& internalToExternal )
    : Functor(functor)
    , Morphology(morphology)
    , LangInfo(langInfo)
    , InternalToExternal(internalToExternal)
    {}

    bool NewWord();
    bool NextGrammar(TWordGrammar word);

private:
    MorphEnumerator&                    Functor;
    const TBuckwalterMorphology&        Morphology;
    uint16_t                            LangInfo;
    const TLPArabicMorph::TFormMapper&  InternalToExternal;
};

bool TBuckwalterToLPEnumeratorAdapter::NewWord()
{
    return true;
}

bool TBuckwalterToLPEnumeratorAdapter::NextGrammar(TWordGrammar word)
{
    TBundledStemsPacks::TShift  lexemeId    =   TBuckwalterMorphology::LexemeId(word);
    TBundledStemsPacks::TShift  formId      =   Morphology.FormIdx(word);
    LEXID                       lexId       =   packLexId(lexemeId, formId, LangInfo, InternalToExternal);
    LingProcErrors              res         =   Functor.nextLexid(lexId);
    return LP_OK == res;
}

LingProcErrors TLPArabicMorph::enumerate( MorphEnumerator &enumerator )
{
    TBuckwalterToLPEnumeratorAdapter    adapter(enumerator, Morphology, LangInfo, InternalToExternal);
    LingProcErrors                      res =   LP_OK;
    if (Morphology.Enumerate(adapter)) {
        res =   LP_ERROR_EFAULT;
    }
    return res;
}
