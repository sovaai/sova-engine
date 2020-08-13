#include "InfFunction.hpp"

InfEngineErrors InfFunction::Save( char * aBuffer, unsigned int aBufferSize, unsigned int & aResultSize ) const
{
	// Проверяем, был ли инициализирова объект.
	if( !vDLFunctionInfo )
		ReturnWithError( INF_ENGINE_ERROR_DLF, INF_ENGINE_STRING_ERROR_DLF );

	unsigned int memory = InfFunction::GetNeedMemorySize();

	if( aBufferSize < memory )
		ReturnWithError( INF_ENGINE_ERROR_DLF, INF_ENGINE_STRING_ERROR_DLF );

	char * ptr = aBuffer;

	// Сохранить тип реализации функции.
	*ptr = (uint8_t)vDLFunctionType;
	ptr += sizeof( uint8_t );

	// Сохранить дину имени функции.
	memcpy( ptr, &vNameLength, sizeof( uint32_t ) );
	ptr += sizeof( uint32_t );

	// Сохранить имя фукции с завершающим нулём.
	memcpy( ptr, vDLFunctionInfo->name, vNameLength );
	ptr += vNameLength;
	*ptr = 0;
	++ptr;

	aResultSize = ptr - aBuffer;
	if( memory != aResultSize )
		ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_SAVING_RESULT );

	return INF_ENGINE_SUCCESS;
}
