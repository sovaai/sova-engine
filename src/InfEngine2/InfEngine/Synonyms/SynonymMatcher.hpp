#ifndef __SynonymMatcher_hpp__
#define __SynonymMatcher_hpp__

#include <Filtration3/terms/FltTermsMatcher.h>

#include <InfEngine2/InfEngine/Synonyms/SynonymCommon.hpp>

#include <NanoLib/LogSystem.hpp>

#include <NanoLib/nMemoryAllocator.hpp>

/**
 *  Матчер для поиска неглавных синонимов в заданной фразе.
 */
class SynonymMatcher : public FltTermsMatcher {
public:

    /** Очистка памяти. */
    void Reset() {
        FltTermsMatcher::Reset();
        vSynonyms.clear();
        vPositions.clear();
        vLastAddedTerm = nullptr;
    }

    /**
     *  Возвращает количество найденных последним запросом синонимов.
     */
    inline unsigned int GetSynonymsCount() const {
        return vSynonyms.size();
    }

    /**
     *  Возвращает заданный синоним из найденных последним запросом.
     * @param aSynonymN - номер синонима.
     */
    inline const SynonymAttrsRO & GetSynonym(unsigned int aSynonymN) const {
        return vSynonyms[aSynonymN];
    }

    /**
     *  Возвращает позицию слова в размеченном тексте, соответствующего заданному синониму.
     * @param aSynonymN - номер синонима.
     */
    inline const size_t & GetPosition(unsigned int aSynonymN) const {
        return vPositions[aSynonymN];
    }

private:
    // Финальная обработка результатов поиска.

    enum FltTermsErrors FinalizeMatch(struct terms_di_info& /*cur_di_info*/) {
        return TERMS_OK;
    }

    /**
     *  Добавление найденных терминов в матчер.
     * @param aTerm - указатель на данные термина.
     * @param aLastPos - итератор позиции конца термина.
     */
    enum FltTermsErrors AddFound(const TERM* aTerm, const TermsDocImageIterator& aLastPos, TDATA_FLAGS_TYPE /*aWhere*/, bool /*aIsHomo*/);

private:
    /** Список найденных последним запросом синонимов. */
    avector<SynonymAttrsRO> vSynonyms;

    /** Список позици слов, для которых найденны синонимы. */
    avector<size_t> vPositions;

	const TERM* vLastAddedTerm = nullptr;
    
};

#endif //  __SynonymMatcher_hpp__
