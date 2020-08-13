#include "Array.hpp"

using namespace InfPatternItems;

void Array::Set( Base ** aItems, unsigned int aItemsNum )
{
	vNeedMemorySize = 0;

	// Проверка аргументов.
	if( !aItems && aItemsNum )
		aItemsNum = 0;

	// Копирование данных.
	vItems = (Base**)aItems;
	vItemsNum = aItemsNum;
}

unsigned int Array::GetNeedMemorySize() const
{
	if( vNeedMemorySize )
		return vNeedMemorySize;

	// Вычисление объема памяти необходимого для сохранения массива.
	vNeedMemorySize = sizeof( uint32_t ); // Количество элементов.
	vNeedMemorySize += vItemsNum * sizeof( uint32_t ); // Ссылки на элементы.
	for( unsigned int i = 0; i < vItemsNum; i++ )
	{
		// Смещение элементов.
		vNeedMemorySize += vNeedMemorySize % 4 != 3 ? 3 - vNeedMemorySize % 4 : 0;

		// Тип элемента.
		vNeedMemorySize += 1;

		vNeedMemorySize += vItems[i]->GetNeedMemorySize();
	}

	return vNeedMemorySize;
}

unsigned int Array::Save( void * aBuffer ) const
{
	// Сохранение.
	char * ptr = static_cast<char*>( aBuffer );

	// Сохранение количества элементов ответа.
	*( (uint32_t*)ptr ) = vItemsNum;
	ptr += sizeof( uint32_t );

	// Смещения элементов.
	uint32_t * shifts = (uint32_t*)ptr;
	ptr += vItemsNum * sizeof( uint32_t );

	// Сохранение элементов шаблон-ответа.
	for( unsigned int i = 0; i < vItemsNum; i++ )
	{
		// Смещение.
		ptr += ( ptr - static_cast<char*>( aBuffer ) ) % 4 != 3 ? 3 - ( ptr - static_cast<char*>( aBuffer ) ) % 4 : 0;

		// Ссылка на элемент.
		*shifts = ptr - static_cast<char*>( aBuffer );
		shifts++;

		// Тип элемента.
		*ptr = (char)vItems[i]->GetType();
		ptr++;

		// Сохранение элемента.
		ptr += vItems[i]->Save( ptr );
	}

	return ptr - static_cast<char*>( aBuffer );
}
