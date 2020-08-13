#include "TagIf.hpp"

using namespace InfPatternItems;

unsigned int TagIf::GetNeedMemorySize() const
{
	unsigned int NeedMemory = sizeof( uint32_t ) +    // размер тэга в байтах.
							  sizeof( uint32_t ) +    // смещение к условию.
							  sizeof( uint32_t ) +    // смещение к then-элементам.
							  sizeof( uint32_t ) +    // смещение к else-элементам.
							  vCondition->GetNeedMemorySize();  // условие.
	NeedMemory += NeedMemory%4 ? 4-NeedMemory%4 : 0;    // выравнивание адреса.
	NeedMemory += vTrueItems->GetNeedMemorySize();      // then-элементы.
	if( vFalseItems )
	{
		NeedMemory += NeedMemory%4 ? 4-NeedMemory%4 : 0;    // выравнивание адреса.
		NeedMemory += vFalseItems->GetNeedMemorySize();     // else-элементы.
	}

	return NeedMemory;
}

unsigned int TagIf::Save( void * aBuffer ) const
{
	// Таблица сдвигов.
	uint32_t * shift = reinterpret_cast<uint32_t*>( aBuffer ) + 1;

	char * ptr = static_cast<char*>( aBuffer ) +
				 sizeof( uint32_t ) +   // размер тэга в байтах.
				 sizeof( uint32_t ) +   // смещение к уловию.
				 sizeof( uint32_t ) +   // смещение к then-объектам.
				 sizeof( uint32_t );    // смещение к else-объектам.

	// Сохраняем смещение к условию.
	*shift = ptr - static_cast<char*>( aBuffer );
	++shift;

	// Сохраняем условие.
	ptr += vCondition->Save( ptr );

	// Выравнивание адреса.
	ptr += ( ptr - static_cast<char*>( aBuffer ))%4 ? 4 - ( ptr - static_cast<char*>( aBuffer ) )%4 : 0;

	// Сохраняем смещение к then-объектам.
	*shift = ptr - static_cast<char*>( aBuffer );
	++shift;

	// Сохраняем then-объекты.
	ptr += vTrueItems->Save( ptr );

	// Сохраняем смещение к else-объектам.
	if( vFalseItems )
	{
		// Выравнивание адреса.
		ptr += ( ptr - static_cast<char*>( aBuffer ))%4 ? 4 - ( ptr - static_cast<char*>( aBuffer ) )%4 : 0;

		*shift = ptr - static_cast<char*>( aBuffer );
		++shift;

		// Сохраняем else-объекты.
		ptr += vFalseItems->Save( ptr );
	}
	else
		*shift = 0;

	// Сохраняем размер тэга в байтах.
	*(uint32_t*)aBuffer = ptr - static_cast<char*>( aBuffer );

	return ptr - static_cast<char*>( aBuffer );
}
