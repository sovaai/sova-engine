#include <InfEngine2/InfEngine/InfPattern/Items/TagFunction.hpp>
#include <stdint.h>

#include "Base.hpp"

using namespace InfPatternItems;

/** Получение размера памяти, необходимого для сохранения данного элемента. */
unsigned int TagFunction::GetNeedMemorySize() const
{
	if( vTagFuncId != static_cast<unsigned int>(-1))
	{
		// Если тело тэга сохранено в индексе.

		return 1                    // Флаг способа сохранения.
				+ sizeof(uint32_t); // Индентификатор проиндексированного тэга-функции.
	}
	else
	{
		// Если тело тэга сохраняется не в индексе.

		Array TmpArray( reinterpret_cast<Base**>(vArgs), vArgsCount );
		return 1                                // Флаг способа сохранения.
				+ sizeof(uint32_t)              // Идентификатор функции.
				+ TmpArray.GetNeedMemorySize(); // Список аргументов.
	}
}

/** Сохранение элемента в буффер. */
unsigned int TagFunction::Save( void * aBuffer ) const
{
	char * ptr = static_cast<char*>( aBuffer );

	if( vTagFuncId != static_cast<unsigned int>(-1) )
	{
		// Если тело тэга сохранено в индексе.
		TagFunctionOptions opt = TFO_INDEXED;
		if( vIsMultiple )
			opt = static_cast<TagFunctionOptions>( opt | TFO_MULTIPLE );

		*ptr = static_cast<char>( opt );
		++ptr;

		*reinterpret_cast<uint32_t*>(ptr) = vTagFuncId;
		ptr += sizeof(uint32_t);
	}
	else
	{
		// Если тело тэга сохраняется не в индексе.

		*ptr = vIsMultiple ? static_cast<char>( TFO_MULTIPLE ) : 0;
		++ptr;

		// Сохранить идентификатор фунции.
		*reinterpret_cast<uint32_t*>(ptr) = vFuncId;
		ptr += sizeof(uint32_t);

		Array ArgArray( reinterpret_cast<Base**>(vArgs), vArgsCount );


		// Сохранить аргументы функции.
		ptr += ArgArray.Save( ptr );
	}

	return ptr - static_cast<char*>( aBuffer );
}
