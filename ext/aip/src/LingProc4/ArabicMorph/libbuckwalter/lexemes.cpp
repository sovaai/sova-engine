#include "lexemes.h"
#include "pascalstrings.h"
#include "fstorage_utils.h"
#include <algorithm>
#include <limits>

bool AddLexeme(TLexemes& lexemes, const std::string& lexeme)
{
    if (lexeme.size() > std::numeric_limits<uint8_t>::max()) {
        return false;
    }

    if (lexemes.size() == std::numeric_limits<TBundledStemsPacks::TShift>::max()) {
        return false;
    }

    lexemes.push_back(lexeme);
    return true;
}

void FinishAdding(TLexemes& lexemes)
{
    std::sort(lexemes.begin(), lexemes.end());
    lexemes.erase(std::unique(lexemes.begin(), lexemes.end()), lexemes.end());
}

void FinishAdding(TStemsForLexemes& stemsForLexemes)
{
    for (unsigned int stemNum = 0; stemNum != stemsForLexemes.size(); ++stemNum) {
        FinishAdding(stemsForLexemes[stemNum]);
    }
}

//============================================

static const TBundledStems::TShift  kEmptyStems[]       =   {0};
static const TCharBuffer            kEmptyStemsBuffer   =   TCharBuffer(kEmptyStems, kEmptyStems + 1);

TBundledStems::TBundledStems()
: Bundle(kEmptyStemsBuffer)
{}

TBundledStems::TBundledStems(TCharBuffer bundle)
: Bundle(bundle)
{}

TBundledStems::TShift TBundledStems::Size()const
{
    return *Bundle.Begin;
}

TBundledStems::TShift TBundledStems::ShiftForIdx(TShift idx)
{
    return 1 + idx;
}

TCharBuffer TBundledStems::Stem(TShift idx)const
{
    const uint8_t*  indexPos    =   Bundle.Begin + ShiftForIdx(idx);
    const uint8_t*  stemStart   =   Bundle.Begin + *indexPos;
    return pascal_get(stemStart);
}

TBundledStems::TShift TBundledStems::MemorySizeFor(const TStems& stems)
{
    TShift  ret         =   0;
    size_t  size        =   stems.size();
    TShift  headerSize  =   ShiftForIdx(size);

    ret +=  headerSize;

    for (size_t stemNum = 0; stemNum != stems.size(); ++stemNum) {
        ret +=  pascal_size(stems[stemNum]);
    }
    return ret;
}

uint8_t* TBundledStems::DoBundle(const TStems& stems, uint8_t* start)
{
    TShift      size    =   static_cast<TShift>(stems.size());
    memcpy(start, &size, sizeof(size));
    TShift  collectedSize   =   ShiftForIdx(size);
    for (TShift stemNum = 0; stemNum != size; ++stemNum) {
        TShift*             idxPos  =   start + ShiftForIdx(stemNum);
        TShift*             stemPos =   start + collectedSize;
        const std::string&  stem    =   stems[stemNum];
        pascal_write(stem, stemPos);
        *idxPos         =   collectedSize;
        collectedSize   +=  pascal_size(stem);
    }
    return start + collectedSize;
}

//============================================

static const unsigned int kStemPacksRatio    =   sizeof(TBundledStemsPacks::TShift) / sizeof(uint8_t);

const TBundledStemsPacks::TShift TBundledStemsPacks::KNoLexemeId    =   std::numeric_limits<TBundledStemsPacks::TShift>::max();
TBundledStemsPacks::TShift TBundledStemsPacks::ShiftForIdx(TBundledStemsPacks::TShift idx)
{
    return (1 + idx * 2) * kStemPacksRatio;
}

void TBundledStemsPacks::DoBundle(const TStemsForLexemes& stems, uint8_t* start)
{
    TShift      size    =   static_cast<TShift>(stems.size());
    memcpy(start, &size, sizeof(size));
    TShift  collectedSize   =   ShiftForIdx(size);
    for (TShift stemNum = 0; stemNum != size; ++stemNum) {
        uint8_t*            idxPos  =   start + ShiftForIdx(stemNum);
        uint8_t*            stemPos =   start + collectedSize;
        const TStems&       stem(stems[stemNum]);
        TBundledStems::DoBundle(stem, stemPos);
        memcpy(idxPos, &collectedSize, sizeof(collectedSize));
        collectedSize   +=  TBundledStems::MemorySizeFor(stem);
    }
}

TBundledStemsPacks::TShift TBundledStemsPacks::MemorySizeFor(const TStemsForLexemes& stems)
{
    TShift  ret         =   0;
    size_t  size        =   stems.size();
    TShift  headerSize  =   ShiftForIdx(size);

    ret +=  headerSize;

    for (size_t stemNum = 0; stemNum != stems.size(); ++stemNum) {
        ret +=  TBundledStems::MemorySizeFor(stems[stemNum]);
    }
    return ret;
}

static const TBundledStems::TShift  kEmptyStemsPack[]       =   {0, 0, 0, 0};
static const TCharBuffer            kEmptyStemsPackBuffer   =   TCharBuffer(kEmptyStemsPack, kEmptyStemsPack + 4);
TBundledStemsPacks::TBundledStemsPacks()
: Bundle(kEmptyStemsPackBuffer)
{}

TBundledStemsPacks::TBundledStemsPacks(TCharBuffer bundle)
: Bundle(bundle)
{}

TBundledStemsPacks::TShift TBundledStemsPacks::Size()const
{
    return *reinterpret_cast<const TShift*>(Bundle.Begin);
}

TBundledStems TBundledStemsPacks::Stems(TShift idx)const
{
    const uint8_t*  indexPosBegin       =   Bundle.Begin + ShiftForIdx(idx);
    const uint8_t*  indexPosEnd         =   indexPosBegin + kStemPacksRatio;
    TCharBuffer     stemBuffer;
    stemBuffer.Begin    =   Bundle.Begin + *reinterpret_cast<const TShift*>(indexPosBegin);
    stemBuffer.End      =   Bundle.Begin + *reinterpret_cast<const TShift*>(indexPosEnd);

    return TBundledStems(stemBuffer);
}

void FinishAdding(TStemsWoForStemsWth& stems)
{
    for ( TStemsWoForStemsWth::iterator stemPos = stems.begin()
        ; stemPos != stems.end()
        ; ++stemPos ) {
        TStems& stems(stemPos->second);
        FinishAdding(stems);
    }
}

TCharBuffer TBundledStemsPacks::Buffer()const
{
    return Bundle;
}

//============================================
void TWoToWthMapper::ApplyZeroLengtVariants(TCharBuffer zeroVariants)
{
    ZeroLengthStorage.resize(zeroVariants.Size());
    memcpy(ZeroLengthStorage.get_buffer(), zeroVariants.Begin, zeroVariants.Size());
    ZeroLengthVariants  =   TBundledStems(TCharBuffer( ZeroLengthStorage.get_buffer()
                                                     , ZeroLengthStorage.size() ));
}

bool TWoToWthMapper::Init(EMorphologyMode mode)
{
    PatriciaTreeCore::ResultStatus  res =   PatriciaTreeCore::RESULT_STATUS_SUCCESS;
    if (EBuildMorphology == mode) {
        res =   Storage.create(kTreeFlags);
    }
    ApplyZeroLengtVariants(kEmptyStemsBuffer);
    return res == PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

bool TWoToWthMapper::AddWthToWoMapping(const std::string wth, const TStems& woVariants)
{
    avector<uint8_t>    buffer;
    buffer.resize(TBundledStems::MemorySizeFor(woVariants) + 1);
    buffer[0]   =   0;
    TBundledStems::DoBundle(woVariants, buffer.get_buffer() + 1);

    bool    isOk    =   true;
    if (0 == wth.size()) {
        TCharBuffer bundleBuffer(buffer.get_buffer() + 1, buffer.size() - 1);
        ApplyZeroLengtVariants(bundleBuffer);
    }
    else {
        const uint8_t*                  bufferStart =   buffer.get_buffer();
        PatriciaTreeCore::ResultStatus  res         =   Storage.append( reinterpret_cast<const uint8_t *>(wth.c_str())
                                                                      , wth.size()
                                                                      , &bufferStart
                                                                      , buffer.size() );

        isOk    =   res == PatriciaTreeCore::RESULT_STATUS_SUCCESS;
    }
    return isOk;
}

TBundledStems TWoToWthMapper::GetWo(const uint8_t* key, uint32_t size)
{
    if (0 == size) {
        return ZeroLengthVariants;
    }

    const uint8_t*  bundledStart    =   NULL;
    size_t          bundledSize     =   0;

    PatriciaTreeCore::ResultStatus  res =   Storage.lookupExact( key
                                                               , size
                                                               , &bundledStart
                                                               , &bundledSize );
    if ( (NULL == bundledStart)
      || (0 == bundledSize)
      || (PatriciaTreeCore::RESULT_STATUS_SUCCESS != res)) {
        return TBundledStems();
    }
    return TBundledStems(TCharBuffer(bundledStart + 1, bundledSize - 1));
}

enum EStorageSectionMap {
    EZeroLengthVariantsSectionId    =   0,
    EPatriciaStartSectionId,
};

fstorage_section_id TWoToWthMapper::Store(fstorage* storage, fstorage_section_id startSection)
{
    fstorage_section_id zeroSectionId   =   StoreAvector( ZeroLengthStorage
                                                        , storage
                                                        , EZeroLengthVariantsSectionId + startSection );
    if (kStoreError == zeroSectionId) {
        return kStoreError;
    }

    return StorePatriciaTree(Storage, storage, startSection + EPatriciaStartSectionId);
}

fstorage_section_id TWoToWthMapper::Restore(fstorage* storage, fstorage_section_id startSection)
{
    TCharBuffer         zeroBuffer;
    fstorage_section_id zeroSectionId   =   RestoreAvector( zeroBuffer
                                                          , storage
                                                          , EZeroLengthVariantsSectionId + startSection );
    if (kStoreError == zeroSectionId) {
        return kStoreError;
    }

    ApplyZeroLengtVariants(zeroBuffer);
    return RestorePatriciaTree(Storage, kTreeFlags, storage, startSection + EPatriciaStartSectionId);
}
