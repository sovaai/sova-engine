#include "AliasRegistry.hpp"

InfEngineErrors AliasRegistry::RegisterAlias( const Alias * aAlias )
{
    // Проверка на уникальность имени алиаса.
    unsigned int Id;
    NanoLib::NameIndex::ReturnCode rc = vAliasIndex.AddName( aAlias->GetName(), Id );
    if ( rc == NanoLib::NameIndex::rcElementExists )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Alias \"%s\" already have been registered", aAlias->GetName() );
    if ( rc == NanoLib::NameIndex::rcErrorNoFreeMemory)
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, "Failed to add alias \"%s\" to AliasIndex because no memory", aAlias->GetName() );
    else if ( rc != NanoLib::NameIndex::rcSuccess)
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Failed to add alias \"%s\" to AliasIndex", aAlias->GetName() );

    // Проверка на корректность индекса.
    if( Id != vAliases.size() )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Wrong alias \"%s\"  Id", aAlias->GetName() );

    // Добавление алиаса в сисок.
    vAliases.push_back( aAlias );
    if( vAliases.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

    return INF_ENGINE_SUCCESS;
}

const Alias * AliasRegistry::GetAlias( const char * aAliasName, unsigned int aAliasNameLength ) const
{
    // Поиск идентификатора алиаса по его имени.
    const unsigned int * Id = vAliasIndex.Search( aAliasName, aAliasNameLength );

    // Возвращение найденного алиаса.
    return Id ? vAliases[*Id] : nullptr;
}
