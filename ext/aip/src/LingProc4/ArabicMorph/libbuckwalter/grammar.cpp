#include "grammar.h"
#include <limits>
#include <cstring>

const char* grammarFieldName(TPartGrammarFieldOrder fieldId)
{
    static const char*  kNames[TPartGrammarFieldOrderCount] =   { "without vowels"
                                                                , "with vowels   "
                                                                , "morphology    "
                                                                , "interpretation"
                                                                , "part of speech"
                                                                , "lexeme id     " };
    return kNames[fieldId];
}

static size_t shiftForField(unsigned int fieldIdx)
{
    return 2 * fieldIdx + sizeof(TBundledStemsPacks::TShift);
}
//we need TPartGrammarFieldOrderCount*2 bytes to store separate field shifts of begin and ends
//we need space to store LexemeId
static const uint8_t    kHeaderSize =   shiftForField(TPartGrammarFieldOrderCount);

bool BundleGrammar(const TPartGrammar& grammar, avector<uint8_t>& bundle)
{
    size_t      sizeBefore      =   bundle.size();
    bundle.resize(kHeaderSize + sizeBefore);
    uint8_t*    inplaceBundle   =   bundle.get_buffer() + sizeBefore;
    memcpy(inplaceBundle, &grammar.LexemeId, sizeof(grammar.LexemeId));
    size_t      bodySize        =   0;
    for ( unsigned int fieldIdx = EntryWoVowelsNum
        ; fieldIdx != TPartGrammarFieldOrderCount
        ; ++fieldIdx) {
        size_t  fieldSize        =   grammar.Fields[fieldIdx].size();
        size_t  shift            =   shiftForField(fieldIdx);
        inplaceBundle[shift]     =   bodySize + kHeaderSize;
        inplaceBundle[shift + 1] =   bodySize + kHeaderSize + fieldSize;
        bodySize                 +=  fieldSize;
    }
    bundle.resize(bodySize + kHeaderSize + sizeBefore);

    if ((bundle.size() - sizeBefore) > std::numeric_limits<uint8_t>::max()) {
        return false;
    }
    inplaceBundle   =   bundle.get_buffer() + sizeBefore;

    for ( unsigned int fieldIdx = EntryWoVowelsNum
        ; fieldIdx != TPartGrammarFieldOrderCount
        ; ++fieldIdx ) {
        size_t  shift   =   shiftForField(fieldIdx);
        memcpy( inplaceBundle + inplaceBundle[shift]
              , grammar.Fields[fieldIdx].c_str()
              , inplaceBundle[shift + 1] - inplaceBundle[shift] );
    }
    return true;
}

TBundledGrammarHandler::TBundledGrammarHandler(TCharBuffer bundledGrammar)
{
    FromBuffer(bundledGrammar);
}

void TBundledGrammarHandler::FromBuffer(TCharBuffer bundledGrammar)
{
    LexemeId    =   *reinterpret_cast<const TBundledStemsPacks::TShift*>(bundledGrammar.Begin);
    for ( unsigned int fieldIdx = EntryWoVowelsNum
        ; fieldIdx != TPartGrammarFieldOrderCount
        ; ++fieldIdx ) {
        TCharBuffer&    buffer(Fields[fieldIdx]);
        size_t          shift   =   shiftForField(fieldIdx);

        buffer.Begin    =   bundledGrammar.Begin + bundledGrammar.Begin[shift];
        buffer.End      =   bundledGrammar.Begin + bundledGrammar.Begin[shift + 1];
    }
}

TCharBuffer TBundledGrammarHandler::GetField(TPartGrammarFieldOrder num)const
{
    return Fields[num];
}

TBundledStemsPacks::TShift TBundledGrammarHandler::Id()const
{
    return LexemeId;
}

static const uint8_t    kEmptyGrammarBody[] =   { 0
                                                , 0
                                                , 0
                                                , 0
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize
                                                , kHeaderSize };

static const TCharBuffer kEmptyBuffer(kEmptyGrammarBody, kEmptyGrammarBody + sizeof(kEmptyGrammarBody) / sizeof(kEmptyGrammarBody[0]));

const TBundledGrammarHandler TBundledGrammarHandler::KEmpty =   TBundledGrammarHandler(kEmptyBuffer);
TBundledGrammarHandler::TBundledGrammarHandler()
{
    FromBuffer(kEmptyBuffer);
}

const TBundledGrammarHandler& TBundledGrammarHandler::operator = (const TBundledGrammarHandler& another)
{
    for (unsigned int fieldNum = 0; fieldNum != TPartGrammarFieldOrderCount; ++fieldNum) {
        Fields[fieldNum]    =   another.Fields[fieldNum];
    }
    LexemeId    =   another.LexemeId;
    return *this;
}

static const uint32_t kRatio  =   sizeof(uint32_t) / sizeof(uint8_t);
//here is a format
//1b - thug for libpts2
//4b - for variants count
//4b * 2 * variants count - for borders of variants
//all other is a bundled TPartGrammar
static uint32_t rangePositionForIthVariant(size_t i)
{
    return kRatio + kRatio*2*i + 1;
}

bool BundleVariants(const TPartGrammarVariants& variants, avector<uint8_t>& bundle)
{
    bundle.clear();
    uint32_t    variantsCount   =   variants.size();
    uint32_t    headerSize      =   rangePositionForIthVariant(variantsCount);
    bundle.resize(headerSize);
    bundle[0]   =   0;
    //copy buffer
    memcpy(bundle.get_buffer() + 1, &variantsCount, sizeof(variantsCount));
    for (unsigned int variantNum = 0; variantNum != variantsCount; ++variantNum ) {
        uint32_t    before  =   bundle.size();
        BundleGrammar(variants[variantNum], bundle);
        uint32_t    after    =   bundle.size();
        uint32_t    writePos =   rangePositionForIthVariant(variantNum);
        memcpy(bundle.get_buffer() + writePos, &before, sizeof(before));
        memcpy(bundle.get_buffer() + writePos + kRatio, &after, sizeof(after));
    }
    return true;
}

bool TBundledVariantsHelper::IsEmpty()
{
    return Size() == 0;
}

uint32_t TBundledVariantsHelper::Size()
{
    return *reinterpret_cast<const uint32_t*>(Bundle.Begin + 1);
}

TBundledGrammarHandler TBundledVariantsHelper::Variant(size_t idx)
{
    uint32_t    posOfStartPos   =   rangePositionForIthVariant(idx);
    uint32_t    posOfEndPos     =   posOfStartPos + kRatio;
    uint32_t    grammarStart    =   *reinterpret_cast<const uint32_t*>(Bundle.Begin + posOfStartPos);
    uint32_t    grammarEnd      =   *reinterpret_cast<const uint32_t*>(Bundle.Begin + posOfEndPos);
    return TBundledGrammarHandler(TCharBuffer(Bundle.Begin + grammarStart, Bundle.Begin + grammarEnd));
}

static const uint8_t        kEmptyBundleBody[5] =   {0, 0, 0, 0, 0};

const TCharBuffer           TBundledVariantsHelper::KEmptyBundleBuffer  =\
    TCharBuffer(kEmptyBundleBody, kEmptyBundleBody + kRatio);

const TBundledVariantsHelper TBundledVariantsHelper::KEmptyVariantsHelper   =\
    TBundledVariantsHelper();

TBundledVariantsHelper::TBundledVariantsHelper()
: Bundle(KEmptyBundleBuffer)
{}

const TWordGrammar& TWordGrammar::operator = (const TWordGrammar& another)
{
    PrefixGrammar   =   another.PrefixGrammar;
    SuffixGrammar   =   another.SuffixGrammar;
    StemGrammar     =   another.StemGrammar;
    return *this;
}
