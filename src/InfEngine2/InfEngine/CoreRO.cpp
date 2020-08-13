#include "CoreRO.hpp"

InfEngineErrors BCoreRO::Open( fstorage* aFStorage, const char * aFunctionsRootDir, const char * aFunctionsConfigPath )
{
	// Проверка агрументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка состояния поискового ядра.
	if( IsOpened() )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Загрузка данных DL.
	InfEngineErrors iee = vDLData.Open( aFStorage, aFunctionsRootDir, aFunctionsConfigPath );
	if( iee != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	// Изменение состояния.
	vState = State::Opened;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ICoreRO::Open( fstorage* aFStorage, const char * aFunctionsRootDir, const char * aFunctionsConfigPath )
{
	// Проверка агрументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Состояние проверять не нужно, т.к. оно будет проверено при открытии базовой части.

	// Загрузка базовой части.
	InfEngineErrors iee;
	if( ( iee = BCoreRO::Open( aFStorage, aFunctionsRootDir, aFunctionsConfigPath ) ) != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	// Открытие индексной базы.
	if( ( iee = vIndexBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	// Изменение состояния.
	vState = State::Opened;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreRO::Open( fstorage* aFStorage, const char * aFunctionsRootDir, const char * aFunctionsConfigPath )
{
	InfEngineErrors iee;
	if( ( iee = ICoreRO::Open( aFStorage, aFunctionsRootDir, aFunctionsConfigPath ) ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreRO::OpenExtBaseFoundation()
{
	// Проверка состояния.
	if( !IsOpened() )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "Main base is not opened" );
	if( vExtState != State::Closed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "Ext base foundation is opened already" );

	// Создание основания поискового ядра дополнительной базы.
	vFoundation = fstorage_create();
	if( !vFoundation )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE,
						 "Can't create fstorage. Return Code: %d", fstorage_last_error( vFoundation ) );

	// Создание snapshot'а, по которому можно будет возвращаться к исходному состоянию основания ядра.
	int ret = fstorage_trx_snapshot( vFoundation, FSTORAGE_TRX_READ_ONLY );
	if( ret != 0 )
	{
		fstorage_destroy( vFoundation );
		vFoundation = nullptr;

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't make fstorage snapshot. Return Code: %d", ret );
	}

	// Изменение состояния.
	vExtState = State::Prepared;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreRO::CloseExtBaseFoundation()
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	iee = CloseExtBase();

	if( vFoundation )
	{
		fstorage_close( vFoundation );
		fstorage_destroy( vFoundation );
	}

	vFoundation = nullptr;
	vExtState = State::Closed;

	if( iee != INF_ENGINE_SUCCESS )
	{
		ReturnWithTrace( iee );
	}
	else
		return iee;
}

InfEngineErrors ExtICoreRO::OpenExtBase( const void* aBuffer, unsigned int aBufferSize, InfDictWrapManipulator::JoiningMethod aJoiningMethod )
{
	// Проверка аргументов.
	if( !aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка данных.
	if( aBufferSize < 2*sizeof( uint32_t ) + 16)
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Проверка состояния.
	if( !IsOpened() )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "Main base is not opened" );
	if( vExtState == State::Closed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "Ext base foundation is not loaded." );
	if( vExtState == State::Opened )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "Ext base is opened already" );

	const uint32_t* ptrs = static_cast<const uint32_t*>(aBuffer);

	unsigned int memory = *ptrs;
	ptrs++;

	if( memory > aBufferSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	unsigned int sectnum = *ptrs;
	ptrs++;

	const void* data;
	for( unsigned int i = 0; i < sectnum; i++ )
	{
		// Получаем указатель на нужную секцию.
		data = static_cast<const char *>(aBuffer) + ptrs[i];

		fstorage_section_id id = *static_cast<const fstorage_section_id*>(data);
		data = static_cast<const char *>(data) + sizeof( fstorage_section_id );

		size_t sectsize = *static_cast<const uint32_t*>(data);
		data = static_cast<const char *>(data) + sizeof( uint32_t );

		// Установка секции fstorage.
		int ret = fstorage_trx_provide_section( vFoundation, FSTORAGE_TRX_READ_ONLY,
												id, const_cast<void*>(data), sectsize );
		if( ret != 0 )
		{
			unsigned int errorid = fstorage_last_error(vFoundation);
			CloseExtBase();
			ReturnWithError( INF_ENGINE_ERROR_FSTORAGE,
							 "Can't provide section to fstorage. Return Code: %d, %d", ret, errorid );
		}

		data = static_cast<const char *>(data) + sectsize;
	}


	// Проверка хэш-кода реестра функций.
	unsigned char aDigits[16];
	GetDLData().GetFunctionsRegistry().GetMD5Digits(aDigits);

	if( memcmp( data, aDigits, 16) )
	{
		LogInfo( "Mismatch Function Registry check summ" );
		return INF_ENGINE_ERROR_CHECKSUMM;
	}

	// Загрузка данных DL.
	InfEngineErrors iee;
	if( ( iee = vExtDLData.Open( vFoundation, nullptr, nullptr, aJoiningMethod ) ) != INF_ENGINE_SUCCESS )
	{
		CloseExtBase();

		ReturnWithTrace( iee );
	}

	// Открытие индексной базы.
	if( ( iee = vExtIndexBase.Open( vFoundation ) ) != INF_ENGINE_SUCCESS )
	{
		CloseExtBase();

		ReturnWithTrace( iee );
	}

	// Установка статуса.
	vExtState = State::Opened;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreRO::CloseExtBase()
{
	vExtDLData.Close();
	vExtIndexBase.Close();

	// Откат основания.
	if( vFoundation && vExtState != State::Closed )
	{
		int ret = fstorage_trx_rollback( vFoundation );
		if( ret != 0 )
		{
			fstorage_close( vFoundation );
			fstorage_destroy( vFoundation );

			vExtState = State::Closed;

			ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't rallback fstorage. Return Code: %d", ret );
		}

		// Создание snapshot'а.
		ret = fstorage_trx_snapshot( vFoundation, FSTORAGE_TRX_READ_ONLY );
		if( ret != 0 )
		{
			fstorage_close( vFoundation );
			fstorage_destroy( vFoundation );

			vExtState = State::Closed;

			ReturnWithError( INF_ENGINE_ERROR_FSTORAGE,
							 "Can't make fstorage snapshot. Return Code: %d", ret );
		}

		vExtState = State::Prepared;
	}
	else
		vExtState = State::Closed;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors EliCoreRO::Open( fstorage* aFStorage )
{
	// Проверка агрументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Состояние проверять не нужно, т.к. оно будет проверено при открытии базовой части.

	// Загрузка базовой части.
	InfEngineErrors iee;
	if( ( iee = BCoreRO::Open( aFStorage, nullptr, nullptr ) ) != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	// Открытие индексной базы.
	if( ( iee = vIndexBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	vState = State::Opened;

	return INF_ENGINE_SUCCESS;
}

