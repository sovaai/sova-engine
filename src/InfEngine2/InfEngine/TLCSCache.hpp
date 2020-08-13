#ifndef __TLCSCache_hpp__
#define __TLCSCache_hpp__

#include <map>
#include <vector>

#include <InfEngine2/_Include/Errors.h>

class TLCSCache
{
public:
    TLCSCache() {}

    /**
     *  Добавить результаты проверок УОУШ.
     * @param aPatternId - идентификатор шаблона, выданный хранилищем шаблонов.
     * @param aQuestionN - номер сработавшего шаблон-вопроса.
     * @param aOk - вектор с результатами проверки УОУШ для данного шаблона.
     * @param aTLCSNumber - длина вектора с результатами.
     * @param aId - идентификатор, по которому можно запрашивать резльутаты через метод Ok.
     */
    InfEngineErrors Put( unsigned int aPatternId, unsigned int aQuestionN, const bool * aOk, unsigned int aTLCSNumber, unsigned int & aId );

    /**
     *  Поиск результатов проверки УОУШ.
     * @param aPatternId - идентификатор шаблона, выданный хранилищем шаблонов.
     * @param aQuestionN - номер сработавшего шаблон-вопроса.
     * @param aId - идентификатор, по которому можно запрашивать резльутаты через метод Ok.
     */
    InfEngineErrors Find( unsigned int aPatternId, unsigned int aQuestionN, unsigned int & aId ) const;

    /**
     *  Получение результата проверки N-ого УОУШ.
     * @param aId - идентификатор, выданный методом Find.
     * @param aTLCS_N - номер УОУШ в заданном шаблоне.
     */
    inline bool Ok( unsigned int aId, unsigned int aTLCS_N ) const { return static_cast<unsigned int>(-1) == aId ? true : vOk[aId+aTLCS_N]; }

    /**
     *  Сброс кэша.
     */
    void Clear()
    {
        vIndex.clear();
        vOk.clear();
    }

protected:

    // Индекс (id шаблона, номер вопроса) -> смещение к началу битового вектора проверок условных операторов уровня шаблона.
    typedef std::pair<unsigned int, unsigned int> INDEX_KEY;
    typedef unsigned int INDEX_VALUE;
    typedef std::pair<INDEX_KEY, INDEX_VALUE> INDEX_NODE;
    typedef std::map<INDEX_KEY,INDEX_VALUE> INDEX;
    typedef std::map<INDEX_KEY,INDEX_VALUE>::const_iterator INDEX_ITER_RO;

    INDEX vIndex;

    // Битовой вектор проверок условных операторов уровня шаблона.
    std::vector<bool> vOk;

};

#endif // __TLCSCache_hpp__
