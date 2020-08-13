#ifndef _LIBBUCKWALTER_LEXEMES_H_
#define _LIBBUCKWALTER_LEXEMES_H_
#include "utils.h"
#include <lib/fstorage/fstorage.h>
#include <lib/aptl/avector.h>
#include <_include/_inttype.h>
#include <vector>
#include <map>
#include <string>

//TODO: now we have only 55888 < 2^16 uniq stems, it may be used for bunling LEXID32
typedef std::vector<std::string>    TLexemes;
bool AddLexeme(TLexemes& lexemes, const std::string& lexeme);
void FinishAdding(TLexemes& lexemes);

typedef TLexemes            TStems;
typedef std::vector<TStems> TStemsForLexemes;

void FinishAdding(TStemsForLexemes& stemsForLexemes);


// format:
// 8b count
// 8b * count - shifts from begining
// count * pascal strings
class TBundledStems {
public:
    typedef uint8_t TShift;

    static uint8_t* DoBundle(const TStems& stems, uint8_t* start);
    static TShift MemorySizeFor(const TStems& stems);
    static TShift ShiftForIdx(TShift idx);
    TBundledStems();
    TBundledStems(TCharBuffer bundle);

    TShift  Size()const;
    TCharBuffer Stem(TShift idx)const;
private:
    TCharBuffer Bundle;
};

// format:
// 32b count
// 2 *32b * count - shifts from begining for begin and end
// count * TBundledStems
class TBundledStemsPacks {
public:
    typedef uint32_t    TShift;
    static const TShift KNoLexemeId;

    static void DoBundle(const TStemsForLexemes& stems, uint8_t* start);
    static TShift MemorySizeFor(const TStemsForLexemes& stems);
    static TShift ShiftForIdx(TShift idx);

    TBundledStemsPacks();
    TBundledStemsPacks(TCharBuffer bundle);
    TCharBuffer Buffer()const;

    TShift  Size()const;
    TBundledStems Stems(TShift idx)const;
private:
    TCharBuffer Bundle;
};

typedef std::map<std::string, TStems>   TStemsWoForStemsWth;
void FinishAdding(TStemsWoForStemsWth& stems);

class TWoToWthMapper {
public:
    bool Init(EMorphologyMode mode);
    bool AddWthToWoMapping(const std::string wth, const TStems& woVariants);
    TBundledStems GetWo(const uint8_t* key, uint32_t size);
    // \return section id after last used stored section, or kStoreError on error
    fstorage_section_id Store(fstorage* storage, fstorage_section_id startSection);
    // \return section id after last used stored section, or kStoreError on error
    fstorage_section_id Restore(fstorage* storage, fstorage_section_id startSection);
private:
    void ApplyZeroLengtVariants(TCharBuffer zeroVariants);
    PatriciaTree        Storage;
    avector<uint8_t>    ZeroLengthStorage;
    TBundledStems       ZeroLengthVariants;
};

#endif /* _LIBBUCKWALTER_LEXEMES_H_ */
