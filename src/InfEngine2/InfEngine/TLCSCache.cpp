#include "TLCSCache.hpp"

#include <NanoLib/LogSystem.hpp>


InfEngineErrors TLCSCache::Put( unsigned int aPatternId, unsigned int aQuestionN,
                                const bool * aOk, unsigned int aTLCSNumber,
                                unsigned int & aId )
{
    InfEngineErrors iee = INF_ENGINE_SUCCESS;

    try
    {        
        // Проверка, был ли данные результаты добавлены в кэш ранее.
        if( INF_ENGINE_SUCCESS == ( iee = Find( aPatternId, aQuestionN, aId ) ) )
            return INF_ENGINE_SUCCESS;

        // Добавление результатов.
        aId = vOk.size();
        vOk.resize( aId + aTLCSNumber );
        for( unsigned int n = 0; n < aTLCSNumber; ++n )
            vOk[aId + n] = aOk[n];

        // Индексирование результатов.
        vIndex.insert( INDEX_NODE( INDEX_KEY( aPatternId, aQuestionN), INDEX_VALUE( aId) ) );
    }
    catch( ... )
    {
        ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
    }

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors TLCSCache::Find( unsigned int aPatternId, unsigned int aQuestionN, unsigned int & aId ) const
{
    // Поиск в индексе резльтатов для заданного шаблона и шаблон-вопроса.
    INDEX_ITER_RO iter = vIndex.find( INDEX_KEY( aPatternId, aQuestionN ) );
    if( vIndex.end() == iter )
        return INF_ENGINE_WARN_UNSUCCESS;

    // Результат найден.
    aId = iter->second;

    return INF_ENGINE_SUCCESS;
}
