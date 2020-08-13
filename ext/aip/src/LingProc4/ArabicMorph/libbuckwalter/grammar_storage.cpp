#include "grammar_storage.h"
#include "fstorage_utils.h"
#include <lib/aptl/Functor.h>
#include <lib/fstorage/fstorage.h>
#include <iostream>
#include <limits>
#include <algorithm>


void TGrammarStorage::ApplyZeroLengtVariants(TCharBuffer zeroVariants)
{
    ZeroLengthStorage.resize(zeroVariants.Size());
    memcpy(ZeroLengthStorage.get_buffer(), zeroVariants.Begin, zeroVariants.Size());
    ZeroLengthVariants  =   TBundledVariantsHelper(TCharBuffer( ZeroLengthStorage.get_buffer()
                                                              , ZeroLengthStorage.get_buffer() + ZeroLengthStorage.size() ));
}

bool TGrammarStorage::Init(EMorphologyMode mode)
{
    PatriciaTreeCore::ResultStatus  res =   PatriciaTreeCore::RESULT_STATUS_SUCCESS;
    if (EBuildMorphology == mode) {
        res =   Storage.create(kTreeFlags);
    }
    ApplyZeroLengtVariants(TBundledVariantsHelper::KEmptyBundleBuffer);
    return res == PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

//here I have strange bug:
//  if
//      static const TBundledLexemes::TIndexShift   kLexemeIdError =    TBundledLexemes::KNoLexemeId - 1;
//  then I have
//      kLexemeIdError == TBundledLexemes::KNoLexemeId
//
//so I use 2 as a thug
static const TBundledStemsPacks::TShift   kLexemeIdError =    TBundledStemsPacks::KNoLexemeId - 2;
static TBundledStemsPacks::TShift findLexemeId( const std::string& lexeme
                                              , TLexemes* lexemesPtr )
{
    if (NULL == lexemesPtr) {
        return TBundledStemsPacks::KNoLexemeId;
    }
    TLexemes&           lexemes(*lexemesPtr);
    TLexemes::iterator  pos =   std::lower_bound(lexemes.begin(), lexemes.end(), lexeme);
    if ((pos == lexemes.end()) || (*pos != lexeme)) {
        return kLexemeIdError;
    }
    return std::distance(lexemes.begin(), pos);
}


bool TGrammarStorage::AddVariants( TPartGrammarVariants& variants
                                 , TLexemes* lexemes
                                 , TStemsForLexemes* stemsForLexemes )
{
    if (variants.empty()) {
        return true;
    }

    //check consistency
    unsigned int        idx             =   1;
    bool                isConsistent    =   true;
    const std::string&  key             =   variants[0].Fields[EntryWoVowelsNum];
    while ((isConsistent) && (idx < variants.size())) {
        isConsistent    =   (key == variants[idx].Fields[EntryWoVowelsNum]);
        idx             +=  1;
    }

    if (!isConsistent) {
        return false;
    }

    bool    isLexemeId  =   true;
    idx =   0;
    while ((isLexemeId) && (idx < variants.size())) {
        TPartGrammar&               variant(variants[idx]);
        const std::string&          lexeme      =   variant.Fields[LexemeIdNum];
        TBundledStemsPacks::TShift  lexemeId    =   findLexemeId(lexeme, lexemes);
        variant.LexemeId    =   lexemeId;
        isLexemeId          =   kLexemeIdError != lexemeId;
        idx                 +=  1;
        if ((isLexemeId) && (NULL != stemsForLexemes)) {
            TStemsForLexemes&   stems(*stemsForLexemes);
            if (stems.size() <= lexemeId) {
                stems.resize(lexemeId + 1, TStems());
            }
            stems[lexemeId].push_back(variant.Fields[EntryWoVowelsNum]);
        }
    }

    if (!isLexemeId) {
        return false;
    }


    avector<uint8_t>    bundle;
    if (!BundleVariants(variants, bundle)) {
        return false;
    }

    bool    isOk    =   true;

    if (0 == key.size()) {
        TCharBuffer bundleBuffer(bundle.get_buffer(), bundle.get_buffer() + bundle.size());
        ApplyZeroLengtVariants(bundleBuffer);
    }
    else {
        const uint8_t*                  buffer  =   bundle.get_buffer();
        PatriciaTreeCore::ResultStatus  res     =   Storage.append( reinterpret_cast<const uint8_t *>(key.c_str())
                                                                  , key.size()
                                                                  , &buffer
                                                                  , bundle.size() );

        isOk    =   (res == PatriciaTreeCore::RESULT_STATUS_SUCCESS);
    }
    return isOk;
}

TBundledVariantsHelper TGrammarStorage::GetVariants(const uint8_t* key, uint32_t size)
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
        return TBundledVariantsHelper::KEmptyVariantsHelper;
    }
    return TBundledVariantsHelper(TCharBuffer(bundledStart, bundledStart + bundledSize));
}



class TStoragePrinter : public PatriciaTreeBypassFunctor
{
public:
    PatriciaTreeCore::ResultStatus apply(const PatriciaTreeElement &);
};

static void printGrammar(TBundledGrammarHandler grammar)
{
    for (size_t fieldIdx = 0; fieldIdx != TPartGrammarFieldOrderCount; ++fieldIdx) {
        TCharBuffer field   =   grammar.GetField(static_cast<TPartGrammarFieldOrder>(fieldIdx));
        std::cout << "\t\t" << fieldIdx << " : \"";
        std::cout.write(reinterpret_cast<const char*>(field.Begin), field.Size());
        std::cout << "\"" << std::endl;
    }
}

static void printVarinats(TBundledVariantsHelper variants)
{
    for (unsigned int varNum = 0; varNum != variants.Size(); ++varNum) {
        printGrammar(variants.Variant(varNum));
    }
}

PatriciaTreeCore::ResultStatus TStoragePrinter::apply(const PatriciaTreeElement& el)
{
    std::cout << "node \"";
    std::cout.write(reinterpret_cast<const char*>(el.key), el.keySize);
    std::cout << "\" has " << el.dataSize << " bytes of data" << std::endl;


    TCharBuffer             buffer(el.data, el.dataSize + el.data);
    printVarinats(TBundledVariantsHelper(buffer));
    return PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

void TGrammarStorage::DebugPrint()
{
    TStoragePrinter printer;
    Storage.bypass(printer);
}

enum EStorageSectionMap {
    EZeroLengthVariantsSectionId    =   0,
    EPatriciaStartSectionId,
};

fstorage_section_id TGrammarStorage::Store(fstorage* storage, fstorage_section_id startSection)
{
    fstorage_section_id zeroSectionId   =   StoreAvector( ZeroLengthStorage
                                                        , storage
                                                        , EZeroLengthVariantsSectionId + startSection );
    if (kStoreError == zeroSectionId) {
        return kStoreError;
    }

    return StorePatriciaTree(Storage, storage, startSection + EPatriciaStartSectionId);
}

fstorage_section_id TGrammarStorage::Restore(fstorage* storage, fstorage_section_id startSection)
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
