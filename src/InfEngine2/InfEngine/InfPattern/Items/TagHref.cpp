#include "TagHref.hpp"

using namespace InfPatternItems;

InfEngineErrors TagHref::Set( Array * aURL, const char * aTarget, unsigned int aTargetLen, Array * aLink )
{
	// Проверка аргументов.
	if( !aURL || !aLink || ( !aTarget && aTargetLen ) )
		return INF_ENGINE_ERROR_INV_ARGS;

	// Проверка длины Target'а.
	if( aTargetLen > (uint16_t) - 1 )
		return INF_ENGINE_ERROR_LIMIT;

	vURL = aURL;
	vTarget = aTarget;
	vTargetLen = aTargetLen;
	vLink = aLink;

	return INF_ENGINE_SUCCESS;
}

unsigned int TagHref::GetNeedMemorySize( ) const
{
	unsigned int NeedMemorySize  = sizeof(uint32_t)*2 // Ссылки на Target и Link.
								   + vURL->GetNeedMemorySize( ); // Память, необходимая для сохранения URL'а.
	binary_data_alignment( NeedMemorySize );
	NeedMemorySize += vLink->GetNeedMemorySize( ); // Память, необходимая для сохранения Link'а.
	binary_data_alignment( NeedMemorySize );
	NeedMemorySize += sizeof(uint16_t) // Длина Targetа.
					  + vTargetLen + 1; // Target.
	return NeedMemorySize;
}

unsigned int TagHref::Save( void * aBuffer ) const
{
	// Сохранение данных.
	void * ptr = static_cast<char*>( aBuffer );

	// Ссылки на Link и Target.
	auto shifts = binary_data_skip<uint32_t>( ptr, 2 );

	// Сохранение URL'а.
	binary_data_save_object( ptr, *vURL );

	// Сохранение Link'а.
	*shifts++ = binary_data_alignment( ptr, aBuffer );

	binary_data_save_object( ptr, *vLink );

	// Сохранение Target'а.
	*shifts++ = binary_data_alignment( ptr, aBuffer );

	binary_data_save<uint16_t>( ptr, vTargetLen );
	if( vTargetLen )
		binary_data_save_rs( ptr, vTarget, vTargetLen );
	binary_data_save( ptr, '\0' );

	return static_cast<char*>( ptr ) - static_cast<char*>( aBuffer );
}
