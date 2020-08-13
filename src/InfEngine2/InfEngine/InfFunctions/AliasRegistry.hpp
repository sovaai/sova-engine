#ifndef __AliasRegistry_hpp__
#define __AliasRegistry_hpp__

#include "Alias.hpp"
#include <InfEngine2/Functions/FunctionLib.hpp>
#include <lib/aptl/avector.h>
#include <InfEngine2/_Include/Errors.h>
#include <NanoLib/nMemoryAllocator.hpp>
#include <NanoLib/NameIndex.hpp>
#include <NanoLib/LogSystem.hpp>


// Реестр алиасов.
class AliasRegistry
{
public:
	AliasRegistry()
		{ vAliasIndex.Create(); }

    /**
     *  Регистрация нового алиаса.
     * @param aAlias - указатель на готовый алиас.
     */
    InfEngineErrors RegisterAlias( const Alias * aAlias );

    /**
     *  Возвращает указатель на заданный алиас.
     * @param aAliasName - имя алиаса.
     * @param aAliasNameLength - длина имени алиаса.
     */
    const Alias * GetAlias( const char * aAliasName, unsigned int aAliasNameLength ) const;

private:
    // Индекс имён алиасов.
    NanoLib::NameIndex vAliasIndex;

    // Список указателей на алиасы.
    avector<const Alias*> vAliases;

};

#endif // __AliasRegistry_hpp__
