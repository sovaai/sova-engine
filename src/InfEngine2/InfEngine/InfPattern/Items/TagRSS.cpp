#include "TagRSS.hpp"

#include <cstring>

using namespace InfPatternItems;

unsigned int TagRSS::Save( void * aBuffer ) const
{
	char* ptr = static_cast<char*>( aBuffer );

	// Сохранение Offset'а.
	*reinterpret_cast<uint32_t*>(ptr) = vArgOffset;
	ptr += sizeof(uint32_t);

	// Сохранение ShowTitle и ShowLink.
	*reinterpret_cast<uint32_t*>(ptr) = (vArgShowTitle ? 0x1 : 0) | (vArgShowLink ? 0x2 : 0);
	ptr += sizeof(uint32_t);

	// Сохранение UpdatePeriod'а.
	*reinterpret_cast<uint32_t*>(ptr) = vArgUpdatePeriod;
	ptr += sizeof(uint32_t);

	// Сохранение сдвига к Alt'у.
	unsigned int* Shift = reinterpret_cast<uint32_t*>(ptr);
	ptr += sizeof(uint32_t);

	// Сохранение URL'а и Alt'а.
	*reinterpret_cast<uint32_t*>(ptr) = vArgURLLength;
	ptr += sizeof(uint32_t);
	*reinterpret_cast<uint32_t*>(ptr) = vArgAltLength;
	ptr += sizeof(uint32_t);

	if( vArgURLLength )
		memcpy( ptr, vArgURL, vArgURLLength );
	ptr[vArgURLLength] = '\0';
	ptr += vArgURLLength + 1 + ((vArgURLLength + 1) % sizeof(uint32_t) ? sizeof(uint32_t) - (vArgURLLength + 1) % sizeof(uint32_t) : 0);

	*Shift = ptr - static_cast<char*>( aBuffer );
	if( vArgAltLength )
		memcpy( ptr, vArgAlt, vArgAltLength );
	ptr[vArgAltLength] = '\0';
	ptr += vArgAltLength + 1;

	return ptr - static_cast<char*>( aBuffer );
}
