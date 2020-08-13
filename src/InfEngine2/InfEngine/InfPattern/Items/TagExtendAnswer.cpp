#include <InfEngine2/InfEngine/InfPattern/Items/TagExtendAnswer.hpp>
#include <cstring>

using namespace InfPatternItems;

unsigned int TagExtendAnswer::GetNeedMemorySize() const
{
	if( !vCount || !vPatternsId || !vId )
		return 0;

	unsigned int memory = sizeof(uint32_t) +			// Размер списка.
						  vCount * sizeof(uint32_t) +	// Номера шаблонов.
						  vCount * sizeof(uint32_t);	// Размер таблицы смещений к идентификаторам шаблонов.

	// Размеры идентификаторов шаблонов.
	for( unsigned int id_n = 0; id_n < vCount; ++id_n )
		memory += strlen( vPatternsId[id_n] ) + 1;

	return memory;
}

unsigned int TagExtendAnswer::Save( void * aBuffer ) const
{
	char * ptr = static_cast<char*>( aBuffer );
	// Сохранение длины списка идентификаторов шаблонов.
	*reinterpret_cast<uint32_t*>( ptr ) = vCount;
	ptr += sizeof( uint32_t );

	// Сохранение номеров шаблонов.
	memcpy( ptr, vId, vCount * sizeof( uint32_t ) );
	ptr += vCount * sizeof( uint32_t );

	// Сохранение идентификаторов шаблонов.
	uint32_t * shift = reinterpret_cast<uint32_t*>( ptr );
	ptr += vCount * sizeof( uint32_t );
	for( unsigned int id_n = 0; id_n < vCount; ++id_n )
	{
		// Сдвиг к очередному идентификатору шаблона.
		*shift = ptr - static_cast<char*>( aBuffer );
		++shift;

		// Сохранение очередного идентификатора шаблона.
		unsigned int len = strlen( vPatternsId[id_n] );
		memcpy( ptr, vPatternsId[id_n], len + 1);
		ptr += len + 1;
	}

	return ptr - static_cast<char*>( aBuffer );
}
