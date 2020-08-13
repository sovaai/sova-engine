#ifndef _LIBBUCKWALTER_GRAMMAR_H_
#define _LIBBUCKWALTER_GRAMMAR_H_
#include "utils.h"
#include "lexemes.h"
#include <lib/aptl/avector.h>
#include <string>
#include <vector>

enum TPartGrammarFieldOrder {
    EntryWoVowelsNum    =   0,  //in buckwalter translation
    EntryWthVowelsNum,          //in buckwalter translation
    MorphoCategoryNum,
    InterpretationNum,          //some kind of translation to english
    PartOfSpeechNum,
    LexemeIdNum,
};


static const size_t TPartGrammarFieldOrderCount    =   LexemeIdNum + 1;

const char* grammarFieldName(TPartGrammarFieldOrder fieldId);

//use only for compiling morph db, not in processing
struct TPartGrammar {
public:
    TPartGrammar()
    {
        Clear();
    }

    void Clear()
    {
        LexemeId    =   0;
        Fields.assign(TPartGrammarFieldOrderCount, "");
    }

    std::vector<std::string>    Fields;
    TBundledStemsPacks::TShift  LexemeId;
};

//use only for compiling morph db, not in processing
typedef std::vector<TPartGrammar>   TPartGrammarVariants;

//return false if TPartGrammar can not be bundled
bool BundleGrammar(const TPartGrammar& grammar, avector<uint8_t>& bundle);

class TBundledGrammarHandler {
public:
    static const TBundledGrammarHandler KEmpty;

    TBundledGrammarHandler();
    TBundledGrammarHandler(TCharBuffer bundledGrammar);
    TCharBuffer GetField(TPartGrammarFieldOrder num)const;
    TBundledStemsPacks::TShift Id()const;

    const TBundledGrammarHandler& operator = (const TBundledGrammarHandler& another);
private:
    void FromBuffer(TCharBuffer bundledGrammar);

    TCharBuffer                 Fields[TPartGrammarFieldOrderCount];
    TBundledStemsPacks::TShift  LexemeId;
};

bool BundleVariants(const TPartGrammarVariants& variants, avector<uint8_t>& bundle);

class TBundledVariantsHelper {
public:
    TBundledVariantsHelper();

    TBundledVariantsHelper(const TBundledVariantsHelper& another)
    : Bundle(another.Bundle.Begin, another.Bundle.End)
    {}

    TBundledVariantsHelper(TCharBuffer bundle)
    : Bundle(bundle)
    {}

    static const TBundledVariantsHelper KEmptyVariantsHelper;
    static const TCharBuffer KEmptyBundleBuffer;

    bool IsEmpty();
    uint32_t Size();
    TBundledGrammarHandler Variant(size_t idx);
private:
    TCharBuffer Bundle;
};

struct TWordGrammar {
    TWordGrammar()
    {}

    TWordGrammar( TBundledGrammarHandler prefix
                , TBundledGrammarHandler stem
                , TBundledGrammarHandler suffix )
    : PrefixGrammar(prefix)
    , StemGrammar(stem)
    , SuffixGrammar(suffix)
    {}

    const TWordGrammar& operator = (const TWordGrammar& another);

    TBundledGrammarHandler   PrefixGrammar;
    TBundledGrammarHandler   StemGrammar;
    TBundledGrammarHandler   SuffixGrammar;
};

#endif /* _LIBBUCKWALTER_GRAMMAR_H_ */
