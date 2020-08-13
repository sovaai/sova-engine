#include <cmath>

#include "KeywordArgument.hpp"

#include <cstring>

using namespace InfPatternItems;

unsigned int KeywordArgument::GetNeedMemorySize( ) const
{
	if( vNeedMemorySize )
		return vNeedMemorySize;
	else if( vBuffer )
		return vBufferSize;
	else
	{
		vNeedMemorySize = 2 * sizeof ( uint32_t ) + vValue.GetNeedMemorySize( );
		vNeedMemorySize += (unsigned int)( ceil( (double)( vNameLength + 1 ) / sizeof (uint32_t ) ) * sizeof ( uint32_t ) );
		return vNeedMemorySize;
	}
}

unsigned int KeywordArgument::Save( void * aBuffer ) const
{
	// Провекар аргументов.
	if( !aBuffer )
		return 0;

	// Сохранение аргумента в буфер.
	if( vBuffer )
	{
		memcpy( aBuffer, vBuffer, vBufferSize );
		return vBufferSize;
	}
	else
	{
		char * OriginalBuffer = static_cast<char*>( aBuffer );
		char * ptr = static_cast<char*>( aBuffer );

		// Сохранение сдвига к значению аргумента.
		uint32_t * Shift = reinterpret_cast<uint32_t*>( ptr );
		ptr += sizeof ( uint32_t );

		// Сохранение длины имени аргумента.
		reinterpret_cast<uint32_t*>( ptr )[0] = vNameLength;
		ptr += sizeof ( uint32_t );

		// Сохранение имени аргумента.
		memcpy( ptr, vName, vNameLength );
		ptr[vNameLength] = '\0';
		ptr += (unsigned int)( ceil( (double)( vNameLength + 1 ) / sizeof (uint32_t ) ) * sizeof ( uint32_t ) );

		// Сохранение сдвига к значению.
		*Shift = ptr - OriginalBuffer;

		// Сохранение значения.
		ptr += vValue.Save( ptr );
		return ptr - OriginalBuffer;
	}
}
