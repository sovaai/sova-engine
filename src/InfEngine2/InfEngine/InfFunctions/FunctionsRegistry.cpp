#include "FunctionsRegistry.hpp"

#include <unistd.h>

InfEngineErrors FunctionsRegistry::Create()
{
	vFuncNameIndex.Create();
	vFuncShortNameIndex.Create();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors FunctionsRegistry::Search( const char * aFuncName, unsigned int aFuncNameLength, unsigned int & aFuncId ) const
{
	// Пропускаем восклицаетльный знак в начале функции, указывающий, что это именно функция, а не алиас.
	if( aFuncName && '!' == aFuncName[0] )
	{
		++aFuncName;
		if( aFuncNameLength )
			--aFuncNameLength;
	}

	const unsigned int * func_id = vFuncShortNameIndex.Search( aFuncName, aFuncNameLength );
	if( !func_id )
		func_id = vFuncNameIndex.Search( aFuncName, aFuncNameLength );
	if( !func_id )
		return INF_ENGINE_WARN_UNSUCCESS;

	aFuncId = *func_id;

	return INF_ENGINE_SUCCESS;
}

const FDLFucntion FunctionsRegistry::GetFunction( unsigned int aFuncId ) const
{
	return nullptr;
}

InfEngineErrors FunctionsRegistryWR::Create()
{
	InfEngineErrors iee = FunctionsRegistry::Create();
	if( iee != INF_ENGINE_SUCCESS )
		return iee;
	MD5Init( &vMD5State );

	vIfGroupIsLoaded = false;

	return INF_ENGINE_SUCCESS;
}

unsigned int FunctionsRegistryWR::GetNeedMemorySize() const
{
	unsigned int memory_size = sizeof( uint32_t ) + 16;

	for( size_t func_n = 0; func_n < vFunctions.size(); ++func_n )
		memory_size += vFunctions[func_n]->GetNeedMemorySize();

	return memory_size;
}

/**
 *  Сохранение реестра в fstorage.
 * @param aFStorage - открытый fstorage.
 */
InfEngineErrors FunctionsRegistryWR::Save( fstorage * aFStorage ) const
{
	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка статуса.
	if( vFuncNameIndex.GetState() == NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );
	if( vFuncShortNameIndex.GetState() == NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Получение указателя на секцию.
	fstorage_section * SectData = fstorage_get_section( aFStorage, FSTORAGE_SECTION_FUNCTIONS_REGISTRY );
	if( !SectData )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Выделение памяти.
	unsigned int MemorySize = sizeof( uint32_t ) + GetNeedMemorySize();
	MemorySize += MemorySize % 4 ? 4 - MemorySize % 4 : 0;
	MemorySize += vFuncNameIndex.GetNeedMemorySize();
	MemorySize += vFuncShortNameIndex.GetNeedMemorySize();
	int ret = fstorage_section_realloc( SectData, MemorySize );
	if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Получение указателя на память.
	char * Buffer = static_cast<char*>( fstorage_section_get_all_data( SectData ) );
	if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

	// Сдвиг к месту расположения индекса имен.
	uint32_t * Shift = reinterpret_cast<uint32_t*>( Buffer );
	Buffer += sizeof( uint32_t );

	memcpy( Buffer, vMD5Digest, 16 );
	Buffer += 16;

	*reinterpret_cast<uint32_t*>( Buffer ) = vFunctions.size();
	Buffer += sizeof( uint32_t );


	// Копирование функций.
	unsigned int ResultSize;
	InfEngineErrors iee;
	for( size_t func_n = 0; func_n < vFunctions.size(); ++func_n )
	{
		iee = vFunctions[func_n]->Save( Buffer, MemorySize - ( Buffer - reinterpret_cast<char*>( Shift ) ), ResultSize );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		Buffer += ResultSize;
	}

	// Выравнивание памяти.
	*Shift = Buffer - reinterpret_cast<char*>( Shift );
	Buffer += ( *Shift ) % 4 ? 4 - ( *Shift ) % 4 : 0;
	*Shift = Buffer - reinterpret_cast<char*>( Shift );

	// Сохранение индекса.
	Buffer += vFuncNameIndex.Save( Buffer );
	vFuncShortNameIndex.Save( Buffer );

	return INF_ENGINE_SUCCESS;
}

void FunctionsRegistryWR::Close()
{
	vFuncNameIndex.Reset();
	vFuncShortNameIndex.Reset();
	vFunctions.clear();
	MD5Init( &vMD5State );
}

InfEngineErrors FunctionsRegistryWR::Registry( const char * aFilePath, unsigned int & aFuncId )
{
	// Проверка аргументов.
	if( !aFilePath )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Выделение памяти для новой функции.
	ExternalInfFunction * function = nAllocateObject( vLocalAllocator, ExternalInfFunction );
	if( !function )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new( (nMemoryAllocatorElementPtr*)function )ExternalInfFunction;

	// Загрузка информации о функции.
	if( function->Load( aFilePath, vLocalAllocator ) != INF_ENGINE_SUCCESS )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Can't add new function." );

	InfEngineErrors iee = Registry( function, aFuncId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors FunctionsRegistryWR::Registry( const DLFunctionInfo * aDLFunctionInfo, unsigned int & aFuncId )
{
	// Проверка аргументов.
	if( !aDLFunctionInfo )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Выделение памяти для новой функции.
	InternalInfFunction * function = nAllocateObject( vLocalAllocator, InternalInfFunction );
	if( !function )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new( (nMemoryAllocatorElementPtr*)function )InternalInfFunction;

	// Сохранение информации о функции.
	InfEngineErrors iee = function->SetDLFunctionInfo( aDLFunctionInfo );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	if( INF_ENGINE_SUCCESS != ( iee = Registry( function, aFuncId ) ) )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors FunctionsRegistryWR::Registry( InfFunction * aFunction, unsigned int aFuncId )
{
	// Приведение имени функции к нижнему регистру.
	const char * FunctionName = aFunction->GetInfo()->name;
	unsigned int FunctionNameLength = strlen( FunctionName );
	char * FuncNameLowerCase = nAllocateObjects( vLocalAllocator, char, FunctionNameLength + 1 );
	if( !FuncNameLowerCase )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( FuncNameLowerCase, FunctionName, FunctionNameLength );
	FuncNameLowerCase[FunctionNameLength] = '\0';
	TextFuncs::ToLower( FuncNameLowerCase, FunctionNameLength );

	// Флаг переопределения внешней функции внутренней.
	bool override = false;
	// Проверка на уникальность имени функции.
	const unsigned int * func_id = vFuncNameIndex.Search( FuncNameLowerCase, FunctionNameLength );
	if( func_id )
	{
		aFuncId = *func_id;

		// Разрешается переопределение внешних функций внутренними.
		if( DLFT_EXTERNAL == vFunctions[aFuncId]->GetDLFunctionType() &&
			DLFT_INTERNAL == aFunction->GetDLFunctionType() )
		{
			// Требуем совпадения альтернативных имён функций.
			if( strcasecmp( vFunctions[aFuncId]->GetInfo()->short_name, aFunction->GetInfo()->short_name ) )
				ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Trying to override external function \"%s\" using short name \"%s\" different from \"%s\"",
								 FuncNameLowerCase, aFunction->GetInfo()->short_name, vFunctions[aFuncId]->GetInfo()->short_name );
			override = true;
		}
		else
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Function with name \"%s\" already registred", FuncNameLowerCase );
	}

	// Приведение альтернативного имени функции к нижнему регистру.
	const char * FunctionShortName = aFunction->GetInfo()->short_name;
	unsigned int FunctionShortNameLength = strlen( FunctionShortName );
	char * FuncShortNameLowerCase = nAllocateObjects( vLocalAllocator, char, FunctionShortNameLength + 1 );
	if( !FuncShortNameLowerCase )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( FuncShortNameLowerCase, FunctionShortName, FunctionShortNameLength );
	FuncShortNameLowerCase[FunctionShortNameLength] = '\0';
	TextFuncs::ToLower( FuncShortNameLowerCase, FunctionShortNameLength );

	if( !override )
	{
		// Проверка на уникальность альтернативного имени функции.
		if( !FunctionShortNameLength )
		{
			FuncShortNameLowerCase = FuncNameLowerCase;
			FunctionShortNameLength = FunctionNameLength;
		}

		if( vFuncShortNameIndex.Search( FuncShortNameLowerCase, FunctionShortNameLength ) )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Function with short name \"%s\" aready registred", FuncShortNameLowerCase );

		if( FuncNameLowerCase != FuncShortNameLowerCase &&
			vFuncShortNameIndex.Search( FuncNameLowerCase, FunctionNameLength ) )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Function name \"%s\" aready registred as short name", FuncShortNameLowerCase );

		if( FuncNameLowerCase != FuncShortNameLowerCase &&
			vFuncNameIndex.Search( FuncShortNameLowerCase, FunctionShortNameLength ) )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Short name \"%s\" aready registred as primary name", FuncShortNameLowerCase );
	}

	// Проверка минимальной версии API функции.
	if( aFunction->GetInfo()->DLFunctionInterfaceMinVer > DLFunctionInterfaceVersion )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Wrong DL-function \"%s\" min API version: %i instead %i",
						 FuncNameLowerCase, aFunction->GetInfo()->DLFunctionInterfaceMinVer, DLFunctionInterfaceVersion );

	// Проверка текущей версии API функции.
	if( aFunction->GetInfo()->DLFunctionInterfaceVer < DLFunctionInterfaceVersion )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Wrong DL-function \"%s\" API version: %i instead %i",
						 FuncNameLowerCase, aFunction->GetInfo()->DLFunctionInterfaceVer, DLFunctionInterfaceVersion );

	// Проверка версии протокола упаковки данных.
	if( aFunction->GetInfo()->InfDataProtocolVer != InfDataProtocolVersion )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Wrong DL-function \"%s\" Inf Protocol version: %i instead %i",
						 FuncNameLowerCase, aFunction->GetInfo()->InfDataProtocolVer, InfDataProtocolVersion );

	// При переопределении внешней функции внутренней подменяем переопределяемую функцию в реестре.
	if( override )
	{
		vFunctions[aFuncId] = aFunction;

		// Пересчитываем md5.
		MD5Init( &vMD5State );
		for( unsigned int func_n = 0; func_n < vFunctions.size(); ++func_n )
		{
			unsigned int name_length = strlen( vFunctions[func_n]->GetInfo()->name );
			char * name_lower_case = nAllocateObjects( vLocalAllocator, char, name_length + 1 );
			if( !name_lower_case )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			memcpy( name_lower_case, vFunctions[func_n]->GetInfo()->name, name_length );
			name_lower_case[name_length] = '\0';
			TextFuncs::ToLower( name_lower_case, name_length );

			MD5Update( &vMD5State, reinterpret_cast<const unsigned char*>( name_lower_case ), name_length );
			MD5Update( &vMD5State, reinterpret_cast<const unsigned char*>( &vFunctions[func_n]->GetInfo()->version ), sizeof( DLFunctionVersion ) );
			MD5Update( &vMD5State, reinterpret_cast<const unsigned char*>( &vFunctions[func_n]->GetInfo()->min_version ), sizeof( DLFunctionVersion ) );
		}

		// Генерируем md5 чексумму.
		MD5_CTX MD5Tmp = vMD5State;
		MD5Final( vMD5Digest, &MD5Tmp );
	}
	else
	{
		// Добавление функции в реестр.
		vFunctions.push_back( aFunction );
		NanoLib::NameIndex::ReturnCode rcResult = vFuncNameIndex.AddName( FuncNameLowerCase, FunctionNameLength, aFuncId );
		if( rcResult != NanoLib::NameIndex::rcSuccess )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Failed to registrer function name \"%s\" to index", FuncNameLowerCase );

		unsigned int ShortNameID;
		rcResult = vFuncShortNameIndex.AddName( FuncShortNameLowerCase, FunctionShortNameLength, ShortNameID );
		if( rcResult != NanoLib::NameIndex::rcSuccess )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Failed to registrer function short name \"%s\" to index", FuncShortNameLowerCase );
		if( aFuncId != ShortNameID )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Function Name ID %i and Short Name ID %i does not math", aFuncId, ShortNameID );

		// Изменяем md5 состояние.
		MD5Update( &vMD5State, reinterpret_cast<const unsigned char*>( FuncNameLowerCase ), FunctionNameLength );
		MD5Update( &vMD5State, reinterpret_cast<const unsigned char*>( &( aFunction->GetInfo()->version ) ), sizeof( DLFunctionVersion ) );
		MD5Update( &vMD5State, reinterpret_cast<const unsigned char*>( &( aFunction->GetInfo()->min_version ) ), sizeof( DLFunctionVersion ) );

		// Генерируем md5 чексумму.
		MD5_CTX MD5Tmp = vMD5State;
		MD5Final( vMD5Digest, &MD5Tmp );

		if( !vIfGroupIsLoaded )
		{
			const unsigned int * IsEmptyId = 0;
			const unsigned int * IsNotEmptyId = 0;
			const unsigned int * IsEqualId = 0;
			const unsigned int * IsNotEqualId = 0;

			if( ( IsEmptyId = vFuncNameIndex.Search( "isempty" ) ) &&
				( IsNotEmptyId = vFuncNameIndex.Search( "isnotempty" ) ) &&
				( IsEqualId = vFuncNameIndex.Search( "isequal" ) ) &&
				( IsNotEqualId = vFuncNameIndex.Search( "isnotequal" ) ) )
			{
				vIfGroupIsLoaded = vFunctions[*IsEmptyId]->GetInfo() &&
								   vFunctions[*IsNotEmptyId]->GetInfo() &&
								   vFunctions[*IsEqualId]->GetInfo() &&
								   vFunctions[*IsNotEqualId]->GetInfo();

			}

		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors FunctionsRegistryWR::RegistryAll( const char * aFilePath, const char * aRootDir )
{
	// Разбор файла со списком функций.
	if( aFilePath )
	{
		// Открытие файла со списком функций.
		FILE * funcFH = fopen( aFilePath, "r" );
		if( !funcFH )
			ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open functions config file: %s", strerror( errno ) );

		// Смена каталога на корневой, если он указан.
		char PreviousDir[4096];
		if( aRootDir )
		{
			// Запоминаем текущий каталог.
			if( !getcwd( PreviousDir, 4096 ) )
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't get current dir: %s", strerror( errno ) );

			// Переходим в корневой каталог.
			if( chdir( aRootDir ) == -1 )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", aRootDir, strerror( errno ) );
		}

		// Чтение функций из файла.
		aTextString String;
		aTextString FuncPath;
		nlReturnCode nrc;
		while( ( nrc = String.ReadString( funcFH ) ) == nlrcSuccess )
		{
			// Нормализация строки.
			AllTrim( String );

			// Пропуск комментариев и пустых строк.
			if( String.empty() || String[0] == '#' )
				continue;

			nrc = FuncPath.assign( String );
			if( nrc != nlrcSuccess )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Загрузка и регистрация функции.
			unsigned int func_id = 0;
			InfEngineErrors iee = Registry( FuncPath.ToConstChar(), func_id );
			if( INF_ENGINE_SUCCESS != iee )
				ReturnWithTrace( iee );
		}

		if( aRootDir )
		{
			// Переходим в предыдущий каталог.
			if( chdir( PreviousDir ) == -1 )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", PreviousDir, strerror( errno ) );
		}
	}

	return INF_ENGINE_SUCCESS;
}

void FunctionsRegistryWR::GetMD5Digits( unsigned char aDigest[16] ) const
{
	memcpy( aDigest, vMD5Digest, 16 );
}

const DLFunctionType* FunctionsRegistryWR::GetFunctionType( unsigned int aFuncId ) const
{
	return aFuncId < vFunctions.size() ? &vFunctions[aFuncId]->GetDLFunctionType() : nullptr;
}

const DLFunctionResType* FunctionsRegistryWR::GetFunctionResType( unsigned int aFuncId ) const
{
	return aFuncId < vFunctions.size() ? &( vFunctions[aFuncId]->GetInfo()->rest ) : nullptr;
}

const unsigned int* FunctionsRegistryWR::GetFunctionArgCount( unsigned int aFuncId ) const
{
	return aFuncId < vFunctions.size() ? &( vFunctions[aFuncId]->GetInfo()->argc ) : nullptr;
}

const DLFucntionArgInfo* FunctionsRegistryWR::GetFunctionArgInfo( unsigned int aFuncId, unsigned int aArgNum ) const
{
	if( aFuncId >= vFunctions.size() )
		return nullptr;

	return aArgNum < vFunctions[aFuncId]->GetInfo()->argc
		   ? vFunctions[aFuncId]->GetArgInfo( aArgNum )
		   : vFunctions[aFuncId]->GetInfo()->VarArgType;
}

bool FunctionsRegistryWR::HasVarArgs( unsigned int aFuncId ) const
{
	return aFuncId < vFunctions.size() ? ( vFunctions[aFuncId]->GetInfo()->VarArgType ? true : false ) : false;
}

bool FunctionsRegistryWR::IfGroupIsLoaded() const
{
	return vIfGroupIsLoaded;
}

const DLFunctionOptions* FunctionsRegistryWR::GetOptions( unsigned int aFuncId ) const
{
	return aFuncId < vFunctions.size() ? &( vFunctions[aFuncId]->GetInfo()->options ) : nullptr;
}


const DLFunctionInfo* FunctionsRegistryWR::GetFunctionInfo( unsigned int aFuncId ) const
{
	return aFuncId < vFunctions.size() ? vFunctions[aFuncId]->GetInfo() : nullptr;
}



InfEngineErrors FunctionsRegistryRO::Open( fstorage * aFStorage, const char * aRootDir, const char * aConfigPath )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Сброс данных реестра.
	Close();

	// Получение секции fstorage.
	fstorage_section * SectData = fstorage_get_section( aFStorage, FSTORAGE_SECTION_FUNCTIONS_REGISTRY );
	if( !SectData )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Получение размера секции.
	unsigned int SectionSize = fstorage_section_get_size( SectData );
	if( SectionSize <= sizeof( uint32_t ) + 1 )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Получение указателя на память.
	char * Buffer = static_cast<char*>( fstorage_section_get_all_data( SectData ) );
	if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

	// Получение сдвига сохранения индекса.
	unsigned int Shift = *reinterpret_cast<uint32_t*>( Buffer );

	if( Shift >= SectionSize )
		ReturnWithError( INF_ENGINE_ERROR_DATA, "Wrong data format." );

	// Открытие индекса имен.
	NanoLib::NameIndex::ReturnCode nirc = vFuncNameIndex.Open( Buffer + Shift, SectionSize - Shift );
	if( nirc != NanoLib::NameIndex::rcSuccess )
	{
		switch( nirc )
		{
		case NanoLib::NameIndex::rcErrorInvData:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

		default:
			ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
	}

	// Открытие индекса альтернативных имен.
	nirc = vFuncShortNameIndex.Open( Buffer + Shift + vFuncNameIndex.GetNeedMemorySize(), SectionSize - Shift );
	if( nirc != NanoLib::NameIndex::rcSuccess )
	{
		switch( nirc )
		{
		case NanoLib::NameIndex::rcErrorInvData:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

		default:
			ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
	}

	// Загрузка md5 чексуммы.
	Buffer += sizeof( uint32_t );
	vMD5Digest = reinterpret_cast<const unsigned char*>( Buffer );
	Buffer += 16;

	// Загрузка размера списка функций.
	unsigned func_cnt = *reinterpret_cast<const uint32_t*>( Buffer );
	if( func_cnt != vFuncNameIndex.GetNamesNumber() || func_cnt != vFuncShortNameIndex.GetNamesNumber() )
		ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Если реестр функций пуст.
	if( !func_cnt )
	{
		vFunctionsRO = nullptr;
		return INF_ENGINE_SUCCESS;
	}

	// Загрузка списка функций.
	Buffer += sizeof( uint32_t );
	vFunctionsRO = nAllocateObjects( vLocalAllocator, InfFunctionManipulator*, func_cnt );
	if( !vFunctionsRO )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	for( size_t func_n = 0; func_n < func_cnt; ++func_n )
	{
		switch( InfFunctionManipulator( Buffer ).GetDLFunctionType() )
		{
		case DLFT_INTERNAL:
			// Выделяем память под манипулятор для внутренней функции.
			vFunctionsRO[func_n] = nAllocateObject( vLocalAllocator, InternalInfFunctionManipulator );
			if( !vFunctionsRO[func_n] )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Создаём манипулятор для внешней функции и загружаем в него данные.
			new( (nMemoryAllocatorElementPtr*)vFunctionsRO[func_n] )InternalInfFunctionManipulator;
			iee = vFunctionsRO[func_n]->SetBuffer( Buffer );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithError( iee, "Can't set FunctionRO buffer." );

			break;

		case DLFT_EXTERNAL:
			// Выделяем память под манипулятор для внешней функции.
			vFunctionsRO[func_n] = nAllocateObject( vLocalAllocator, ExternalInfFunctionManipulator );
			if( !vFunctionsRO[func_n] )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Создаём манипулятор для внешней функции и загружаем в него данные.
			new( (nMemoryAllocatorElementPtr*)vFunctionsRO[func_n] )ExternalInfFunctionManipulator;
			iee = vFunctionsRO[func_n]->SetBuffer( Buffer );
			if( iee == INF_ENGINE_ERROR_FUNC_VERSION )
				ReturnWithError( iee, "There is version conflict for DL function \"%s\".", vFuncNameIndex.GetName( func_n ) );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithError( iee, "Can't set FunctionRO buffer." );
			break;

		default:
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Unknown DL function type: %i.", InfFunctionManipulator( Buffer ).GetDLFunctionType() );
		}

		Buffer += vFunctionsRO[func_n]->GetMemorySize();
	}

	iee = LoadDynamicLibraries( aRootDir, aConfigPath );
	if(  iee != INF_ENGINE_SUCCESS )
		return iee;

	const unsigned int * IsEmptyId = 0;
	const unsigned int * IsNotEmptyId = 0;
	const unsigned int * IsEqualId = 0;
	const unsigned int * IsNotEqualId = 0;

	if( ( IsEmptyId = vFuncNameIndex.Search( "isempty" ) ) && ( IsNotEmptyId = vFuncNameIndex.Search( "isnotempty" ) ) &&
		( IsEqualId = vFuncNameIndex.Search( "isequal" ) ) && ( IsNotEqualId = vFuncNameIndex.Search( "isnotequal" ) ) )
	{
		// @todo Сделать подгрузку дефолтных функций.
		vIfGroupIsLoaded = vFunctionsRO[*IsEmptyId]->GetFunciton() &&
						   vFunctionsRO[*IsNotEmptyId]->GetFunciton() &&
						   vFunctionsRO[*IsEqualId]->GetFunciton() &&
						   vFunctionsRO[*IsNotEqualId]->GetFunciton();

		if( !vIfGroupIsLoaded )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Function \"if\" is not supported." );
	}

	vIsOpened = true;

	return INF_ENGINE_SUCCESS;
}

bool FunctionsRegistryRO::IsOpened() const
{
	return vIsOpened;
}

const FDLFucntion FunctionsRegistryRO::GetFunction( unsigned int aFuncId ) const
{
	return aFuncId < vFuncNameIndex.GetNamesNumber() ? vFunctionsRO[aFuncId]->GetFunciton() : nullptr;
}

const DLFunctionResType* FunctionsRegistryRO::GetFunctionResType( unsigned int aFuncId ) const
{
	return aFuncId < vFuncNameIndex.GetNamesNumber() ? &vFunctionsRO[aFuncId]->GetInfo()->rest : nullptr;
}

const unsigned int* FunctionsRegistryRO::GetFunctionArgCount( unsigned int aFuncId ) const
{
	return aFuncId < vFuncNameIndex.GetNamesNumber() ? &( vFunctionsRO[aFuncId]->GetInfo()->argc ) :  nullptr;
}

const DLFucntionArgInfo* FunctionsRegistryRO::GetFunctionArgInfo( unsigned int aFuncId, unsigned int aArgNum ) const
{
	if( aFuncId < vFuncNameIndex.GetNamesNumber() )
	{
		return aArgNum < vFunctionsRO[aFuncId]->GetInfo()->argc ?
			   vFunctionsRO[aFuncId]->GetArgInfo( aArgNum ) : vFunctionsRO[aFuncId]->GetInfo()->VarArgType;
	}
	return nullptr;
}

bool FunctionsRegistryRO::HasVarArgs( unsigned int aFuncId ) const
{
	return aFuncId < vFuncNameIndex.GetNamesNumber() ? ( vFunctionsRO[aFuncId]->GetInfo()->VarArgType ? true : false ) : false;
}


void FunctionsRegistryRO::GetMD5Digits( unsigned char aDigest[16] ) const
{
	memcpy( aDigest, vMD5Digest, 16 );
}

InfEngineErrors FunctionsRegistryRO::LoadDynamicLibraries( const char * aRootDir, const char * aConfigPath )
{

	// Повторная регистрация функций.


	// Разбор файла со списком функций.
	if( aConfigPath )
	{
		// Открытие файла со списком функций.
		FILE * funcFH = fopen( aConfigPath, "r" );
		if( !funcFH )
			ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open functions config file: %s", strerror( errno ) );

		// Смена каталога на корневой, если он указан.
		char PreviousDir[4096];
		if( aRootDir )
		{
			// Запоминаем текущий каталог.
			if( !getcwd( PreviousDir, 4096 ) )
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't get current dir: %s", strerror( errno ) );

			// Переходим в корневой каталог.
			if( chdir( aRootDir ) == -1 )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", aRootDir, strerror( errno ) );
		}

		// Чтение функций из файла.
		aTextString String;
		aTextString FuncPath;
		nlReturnCode nrc;
		while( ( nrc = String.ReadString( funcFH ) ) == nlrcSuccess )
		{
			// Нормализация строки.
			AllTrim( String );

			// Пропуск комментариев и пустых строк.
			if( String.empty() || String[0] == '#' )
				continue;

			nrc = FuncPath.assign( String );
			if( nrc != nlrcSuccess )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Загрузка функции.
			ExternalInfFunction TmpFunction;
			if( TmpFunction.Load( FuncPath.ToConstChar(), vLocalAllocator ) != INF_ENGINE_SUCCESS )
			{
				LogDebug( "Can't load function: %s", String.ToConstChar() );
				continue;
			}

			aTextString FuncNameLowerCase;
			nlReturnCode nlrc = FuncNameLowerCase.assign( TmpFunction.GetInfo()->name );
			if( nlrc != nlrcSuccess )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			TextFuncs::ToLower( FuncNameLowerCase.ToChar(), FuncNameLowerCase.size() );

			aTextString FuncShortNameLowerCase;
			nlrc = FuncShortNameLowerCase.assign( TmpFunction.GetInfo()->short_name );
			if( nlrc != nlrcSuccess )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			TextFuncs::ToLower( FuncShortNameLowerCase.ToChar(), FuncShortNameLowerCase.size() );
			if( !strlen( FuncShortNameLowerCase.ToConstChar() ) )
			{
				nlrc = FuncShortNameLowerCase.assign( FuncNameLowerCase );
				if( nlrc != nlrcSuccess )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			const unsigned int * FuncId = vFuncNameIndex.Search( FuncNameLowerCase.ToConstChar() );
			if( !FuncId )
			{
				LogWarn( "Can't load DL function %s. Function wasn't registred in base.", FuncNameLowerCase.ToConstChar() );
				continue;
			}
			const unsigned int * FuncShortId = vFuncShortNameIndex.Search( FuncShortNameLowerCase.ToConstChar() );
			if( !FuncShortId )
			{
				LogWarn( "Can't load DL function %s. Function wasn't registred in base.", FuncNameLowerCase.ToConstChar() );
				continue;
			}

			if( vFunctionsRO[*FuncId]->GetDLFunctionType() == DLFT_EXTERNAL )
			{
				ExternalInfFunctionManipulator * Function = dynamic_cast<ExternalInfFunctionManipulator*>( vFunctionsRO[*FuncId] );

				if( TmpFunction.GetInfo()->version < *Function->GetMinVersion() )
				{
					LogWarn( "Can't load DL function %s. Incorrect function version", TmpFunction.GetInfo()->name );
					continue;
				}

				if( TmpFunction.GetInfo()->DLFunctionInterfaceMinVer > DLFunctionInterfaceVersion )
				{
					LogWarn( "Can't load DL function %s. Incorrect function min API version: %i instead %i.", TmpFunction.GetInfo()->name, TmpFunction.GetInfo()->DLFunctionInterfaceMinVer, DLFunctionInterfaceVersion );
					continue;
				}

				if( TmpFunction.GetInfo()->DLFunctionInterfaceVer < DLFunctionInterfaceVersion )
				{
					LogWarn( "Can't load DL function %s. Incorrect function API version: %i instead %i.", TmpFunction.GetInfo()->name, TmpFunction.GetInfo()->DLFunctionInterfaceVer, DLFunctionInterfaceVersion );
					continue;
				}

				if( Function->Load( FuncPath.ToConstChar() ) != INF_ENGINE_SUCCESS )
				{
					LogWarn( "Can't load DL function %s. Internal error.", TmpFunction.GetInfo()->name );
					continue;
				}
			}
			else
			{
				LogWarn( "Can't load DL function %s. Function was registred as Internal function.", FuncNameLowerCase.ToConstChar() );
				continue;
			}
		}

		fclose( funcFH );

		if( nrc == nlrcErrorNoFreeMemory )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		else if( nrc != nlrcEOF && nrc != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't read from fuctions list file." );

		if( aRootDir )
		{
			// Переходим в предыдущий каталог.
			if( chdir( PreviousDir ) == -1 )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", PreviousDir, strerror( errno ) );
		}
	}

	const unsigned int * IsEmptyId = 0;
	const unsigned int * IsNotEmptyId = 0;
	const unsigned int * IsEqualId = 0;
	const unsigned int * IsNotEqualId = 0;

	if( ( IsEmptyId = vFuncNameIndex.Search( "isempty" ) ) && ( IsNotEmptyId = vFuncNameIndex.Search( "isnotempty" ) ) &&
		( IsEqualId = vFuncNameIndex.Search( "isequal" ) ) && ( IsNotEqualId = vFuncNameIndex.Search( "isnotequal" ) ) )
	{
		vIfGroupIsLoaded = vFunctionsRO[*IsEmptyId]->GetFunciton() &&
						   vFunctionsRO[*IsNotEmptyId]->GetFunciton() &&
						   vFunctionsRO[*IsEqualId]->GetFunciton() &&
						   vFunctionsRO[*IsNotEqualId]->GetFunciton();

		if( !vIfGroupIsLoaded )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Function \"if\" is not supported." );
	}

	return INF_ENGINE_SUCCESS;
}

bool FunctionsRegistryRO::IfGroupIsLoaded() const
{
	return vIfGroupIsLoaded;
}

void FunctionsRegistryRO::Close()
{
	vFuncNameIndex.Reset();
	vFuncShortNameIndex.Reset();
	for( size_t func_n = 0; func_n < vFuncNameIndex.GetNamesNumber(); ++func_n )
		vFunctionsRO[func_n]->Reset();
	vIsOpened = false;
}

const DLFunctionOptions* FunctionsRegistryRO::GetOptions( unsigned int aFuncId ) const
{
	return aFuncId < vFuncNameIndex.GetNamesNumber() ? &( vFunctionsRO[aFuncId]->GetInfo()->options ) : nullptr;
}

const DLFunctionInfo* FunctionsRegistryRO::GetFunctionInfo( unsigned int aFuncId ) const
{
	return aFuncId < vFuncNameIndex.GetNamesNumber() ? vFunctionsRO[aFuncId]->GetInfo() : nullptr;
}
