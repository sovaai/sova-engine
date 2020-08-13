#ifndef _LIBBUCKWALTER_MORPHOLOGY_STORAGE_H_
#define _LIBBUCKWALTER_MORPHOLOGY_STORAGE_H_
#include "grammar_storage.h"
#include "lexemes.h"
#include "forms.h"
#include "compatibility_storage.h"
#include <lib/fstorage/fstorage.h>
#include <lib/fstorage/fstorage_id.h>

enum EGrammarTypes {
    EPrefix = 0,
    EStem,
    ESuffix,

    EGrammarTypesSize
};

enum ECompatTypes {
    EPrefixNStem = 0,
    EPrefixNSuffix,
    EStemNSuffix,

    ECompatTypesSize
};

ECompatTypes WhichCompat(EGrammarTypes fType, EGrammarTypes sType);

class TBuildBuckwalterMorphology {
public:
    bool Init();
    bool Store(fstorage* storage, fstorage_section_id startSection = 1);

    TGrammarStorage         Grammars[EGrammarTypesSize];
    TCompatibilityStorage   Compats[ECompatTypesSize];
    TLexemes                Lexemes;
    TStemsForLexemes        StemsForLexems;
    avector<uint8_t>        LexemeBundle;

    //use in calculations, do not store
    TPartToGrammarStorage   GramsForPrefix;
    TPartToGrammarStorage   GramsForSuffix;

    TForms                  Forms;
    avector<uint8_t>        FormBundle;

    TPrefixSuffixStorage    PrefixNSuffix;

    TStemsWoForStemsWth     PartsWoWthCollected[EGrammarTypesSize];
    TWoToWthMapper          PartsWoWth[EGrammarTypesSize];

    //as fstorage use deferred storing
    //we need global storage for header
    avector<char>           HeaderStorage;
};

class TBuckwalterMorphology {
public:
    class IAnalyzeResultsFunctor {
    public:
        virtual ~IAnalyzeResultsFunctor()
        {}

        virtual bool NewWord() = 0;
        //return false if need no results anymore, true otherwise
        virtual bool NextGrammar(TWordGrammar word) = 0;
    };

    static const char   KArabicMarker[];
    static const size_t KArabicMarkerSize;

    bool Init(fstorage* storage);
    //return analyze variants count
    int Analyze(TConstString str, IAnalyzeResultsFunctor& handler);

    static TBundledStemsPacks::TShift LexemeId(TWordGrammar word);
    TBundledStems Lexeme(TWordGrammar word)const;
    TBundledStems Lexeme(TBundledStemsPacks::TShift lexemeId)const;
    TBundledForms::TForm Form(TBundledForms::TIndexShift id)const;
    TBundledForms::TIndexShift FormIdx(TWordGrammar word)const;
    size_t  Size()const;
    bool MaterializeWord(TCharBuffer stem, TBundledForms::TIndexShift formId, TWordGrammar& word);
    bool MaterializeWord(TBundledStemsPacks::TShift lexemeId, TBundledForms::TIndexShift formId, TWordGrammar& word);
    bool Enumerate(IAnalyzeResultsFunctor& functor);
    static int MaterializeWord(TWordGrammar& grammar, char* buffer, int bufferSize);
    TBundledForms::TIndexShift FormCount()const;
    bool Store(fstorage* storage, fstorage_section_id startSection);

private:
    bool Combine( TBundledVariantsHelper prefixVariants
                , TBundledVariantsHelper stemVarinats
                , TBundledVariantsHelper suffixVariants
                , int* variantsCount
                , IAnalyzeResultsFunctor& handler);
    bool Restore(fstorage* storage);

    TGrammarStorage         Grammars[EGrammarTypesSize];
    TCompatibilityStorage   Compats[ECompatTypesSize];
    TBundledStemsPacks      Lexemes;
    TBundledForms           Forms;
    TPrefixSuffixStorage    PrefixNSuffix;
    TWoToWthMapper          PartsWoWth[EGrammarTypesSize];

    //as fstorage use deferred storing
    //we need global storage for header
    avector<char>           HeaderStorage;
};

#endif /* _LIBBUCKWALTER_MORPHOLOGY_STORAGE_H_ */
