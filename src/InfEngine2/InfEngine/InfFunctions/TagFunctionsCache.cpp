#include <InfEngine2/InfEngine/InfFunctions/TagFunctionsCache.hpp>

InfEngineErrors TagFunctionsCache::SetSize( unsigned int aTagsCount )
{
    vCachedValues.resize( aTagsCount );

    if( vCachedValues.no_memory() )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;

    memset( vCachedValues.get_buffer(), 0, sizeof(CacheNode) * vCachedValues.size() );

    vCacheGeneration = 1;

    return INF_ENGINE_SUCCESS;
}

unsigned int TagFunctionsCache::GetSize() const
{
    return vCachedValues.size();
}

void TagFunctionsCache::SetValue( unsigned int aFuncTagId, const char * aValue, unsigned int aValueLength )
{
    // Если идентификатор тэга корректен и ещё не был закэширован.
    if( aFuncTagId < vCachedValues.size() && vCachedValues[aFuncTagId].vGeneration < vCacheGeneration )
    {
        // Сохраняем значение и его длину.
        vCachedValues[aFuncTagId].vValue = aValue;
        vCachedValues[aFuncTagId].vLength = aValueLength;
        vCachedValues[aFuncTagId].vGeneration = vCacheGeneration;
    }
}

InfEngineErrors TagFunctionsCache::GetValue( unsigned int aFuncTagId, const char *& aValue, unsigned int & aValueLength  ) const
{
    // Проверка корректности идентификатора тэга и наличия в кэше искомого значения.
    if( aFuncTagId < vCachedValues.size() && vCachedValues[aFuncTagId].vGeneration == vCacheGeneration )
    {
        aValue = vCachedValues[aFuncTagId].vValue;
        aValueLength = vCachedValues[aFuncTagId].vLength;

        return INF_ENGINE_SUCCESS;
    }

    return INF_ENGINE_WARN_UNSUCCESS;
}

void TagFunctionsCache::Clear()
{
    ++vCacheGeneration;
}
