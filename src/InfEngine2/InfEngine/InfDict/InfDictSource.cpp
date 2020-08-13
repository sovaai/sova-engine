#include "InfDictSource.hpp"

InfEngineErrors InfDictSource::ReadDictFromFile( const char * aDictPath )
{
	// Проверка аргументов.
	if( !aDictPath )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Очистка данных.
	Reset();

	// Открытие файла.
	FILE * aFH = fopen( aDictPath, "r" );
	if( !aFH )
		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open file %s: %s", aDictPath, strerror( errno ) );

	// Чтение из файла.
	nlReturnCode nlr;
	while( ( nlr = vTmp.ReadString( aFH ) ) == nlrcSuccess )
	{
		// Нормализация строки.
		AllTrim( vTmp );

		// Пропуск пустых строк.
		if( vTmp.empty() )
			continue;

		// Комментарии.
		if( vTmp[0] == '/' && vTmp[1] == '/' )
		{
			/**
			 *  Замещаем комментарии пустой строкой для корректности вычисления номера строки при генерации
			 * ошибки разбора.
			 */
		}
		else
		{
			nlr = vBuffer.append( vTmp );
			if( nlr != nlrcSuccess )
			{
				fclose( aFH );

				Reset();

				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
		}

		nlr = vBuffer.append( "\n" );
		if( nlr != nlrcSuccess )
		{
			fclose( aFH );

			Reset();

			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
	}

	// Закрытие файла.
	fclose( aFH );

	if( nlr != nlrcEOF )
	{
		Reset();

		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't read dict %s. Return code: %d", aDictPath );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfDictSource::SetName( const char * aDictName, unsigned int aDictNameLen )
{
	// Проверка аргументов.
	if( !aDictName )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка длины.
	if( aDictNameLen > (uint16_t)-1 )
		ReturnWithError( INF_ENGINE_ERROR_LIMIT, "Name of the dict is very long." );

	nlReturnCode nle = vName.assign( aDictName, aDictNameLen );
	if( nle != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Понижение регистра имени словаря.
	if( SetLower( vName ) != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}
