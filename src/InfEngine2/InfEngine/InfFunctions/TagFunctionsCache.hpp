#ifndef __TagFunctionsCache_hpp__
#define __TagFunctionsCache_hpp__

#include <NanoLib/LogSystem.hpp>
#include <lib/aptl/avector.h>
#include <InfEngine2/_Include/Errors.h>

class TagFunctionsCache
{
private:
    struct CacheNode
    {
        const char * vValue;
        unsigned int vLength;
        unsigned int vGeneration;
    };

public:
    /**
     *  Установить размер кэша. необходимо выполнять перед первой записью в кэш.
     * @param aTagsCount - количество тэгов, на которое расчитан кэш.
     */
    InfEngineErrors SetSize( unsigned int aTagsCount );

    /** Возвращает максимальное количество тэгов, которое может вместить кэш. */
    unsigned int GetSize() const;

    /**
     *  Закэшировать результат интерпретации тэга-функции.
     * @param aFuncTagId - идентификатор тэга-функции.
     * @param aValue - кэшируемое значение.
     * @param aValueLength - длина кэшируемого значения в байтах.
     */
    void SetValue( unsigned int aFuncTagId, const char * aValue, unsigned int aValueLength );

    /**
     *  Возвращает закэшированный результат интерпретации тэга-функции или nullptr, если в кэше его нет.
     * @param aFuncTagId - идентификатор тэга-функции.
     * @param aValue - возвращаемое закэшированное значение.
     * @param aValueLength - длина возвращаемого закэшированного значения в байтах.
     */
    InfEngineErrors GetValue( unsigned int aFuncTagId, const char *& aValue, unsigned int & aValueLength  ) const;

    //** Очистить кэша. */
    void Clear();

private:
    /** Кэш проинтерпретированных тэгов-функций. */
    avector<CacheNode> vCachedValues;

    /** Счётчик очисток кэша */
    unsigned int vCacheGeneration;

};


#endif /** __TagFunctionsCache_hpp__ */
