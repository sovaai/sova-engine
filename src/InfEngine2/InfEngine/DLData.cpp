#include "DLData.hpp"

#include <algorithm>

#include <lib/aptl/OpenAddressingDict/BufferDictStorage.h>

#include <InfEngine2/_Include/BinaryData.hpp>

#include "InfDict/InfDict.hpp"
#include "InfDict/InfDictInstrInclude.hpp"

DLDataRO::DLDataRO( const DLDataRO * aMainDataRO ):
	vFuncTagRegistry( true ),
	vDefaultVars( vVarsRegistry )
{
	vMainDataRO = aMainDataRO;
	if( vMainDataRO )
		vDefaultVars.SetDefaultValues( vMainDataRO->GetDefaultVars() );

	memset( vSignature, 0, 16 );
}

InfEngineErrors DLDataRO::Open( fstorage * aFStorage, const char * aFunctionsRootDir, const char * aFunctionsConfigPath, InfDictWrapManipulator::JoiningMethod aJoiningMethod )
{
	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка состояния.
	if( vDictsRegistry.GetState() != NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Загрузка данных.
	fstorage_section * Section = fstorage_get_section( aFStorage, FSTORAGE_SECTION_DLDATA );
	if( !Section )
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't get fstorage section pointer." );

	vDictJoiningMethod = aJoiningMethod;

	const char * ptr = static_cast<const char *>(fstorage_section_get_all_data( Section ));
	if( !ptr )
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't get fstorage section pointer." );

	// Загрузка данных.
	InfEngineErrors iee = LoadFromBuffer( ptr, fstorage_section_get_size( Section ) );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = vConditionsRegistry.Open( aFStorage );
	if( iee != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	iee = vVarsRegistry.Open( aFStorage, vMainDataRO ? &vMainDataRO->GetVarsRegistry() : nullptr );
	if( iee != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	iee = vFuncTagRegistry.Open( aFStorage, aFunctionsRootDir, aFunctionsConfigPath );
	if( iee != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	iee = vPatternsStorage.Open( *aFStorage );
	if( iee != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	// Загрузка реестра InfPerson'ов.
	Section = fstorage_get_section( aFStorage, FSTORAGE_SECTION_INF_PERSON_REGISTRY );
	if( !Section )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );
	}

	ptr = static_cast<char *>(fstorage_section_get_all_data( Section ));
	if( !ptr )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );
	}

	NanoLib::NameIndex::ReturnCode nirc = vInfPersonRegistry.Load( ptr, fstorage_section_get_size( Section ) );
	if( nirc != NanoLib::NameIndex::rcSuccess )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't load InfPerson registry: %s", vInfPersonRegistry.ReturnCodeToString( nirc ) );
	}

	// Загрузка связей событий и шаблон-ответов.
	Section = fstorage_get_section( aFStorage, FSTORAGE_SECTION_DLDATA_EVENTS_BINDINGS );
	if( !Section )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );
	}

	vEventsBinding = static_cast<const uint32_t *>(fstorage_section_get_all_data( Section ));
	if( !vEventsBinding )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );
	}

	return INF_ENGINE_SUCCESS;
}

void DLDataRO::Close()
{
	vDictionaries = nullptr;
	vDictsIndex = nullptr;
	vDictsCount = 0;
	vPatterns = nullptr;
	vLabelCorrespondenceTable = nullptr;
	vEventsBinding = nullptr;
	vVarsRegistry.Close();
	vFuncTagRegistry.Close();
	vInfPersonRegistry.Destroy();
	vDictsRegistry.Reset();
	vUserDictIDs = nullptr;
	vUserDictIDsCount = 0;
	vConditionsRegistry.Close();
	vDefaultVars.Reset();
	vTablePatternsId.Reset();
	vTableIndexId = nullptr;
}

InfEngineErrors DLDataRO::LoadFromBuffer( const char * aBuffer, unsigned int aBufferSize )
{
	// Проверка аргументов.
	if( !aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	const char * ptr = aBuffer;

	/**
	 * - общий объем занимаемой объектом памяти.
	 * - версия протокола данных.
	 * - чексумма сохранения данных.
	 * - чексумма словарей.
	 */
	unsigned int memory = *((uint32_t *)ptr);
	ptr += sizeof( uint32_t );

	// Проверка памяти.
	if( aBufferSize < memory )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_LOADING_RESULT );

	// Проверка версии протокола данных.
	if( InfDataProtocolVersion != *((uint32_t *)ptr) )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Invalid data protocol." );
	ptr += sizeof( uint32_t );

	// Проверка чексуммы DLData.
	unsigned char md5[16];
	MD5_CTX context;
	MD5Init( &context );
	MD5Update( &context, (const unsigned char *)ptr+32, memory-( ptr - aBuffer )-32 );
	MD5Final( md5, &context );

	// @todo: разобраться с чексуммой.

	ptr += 16;

	// Загрузка чексуммы словарей.
	memcpy( vDictsCheckSum, (const unsigned char *)ptr, 16 );
	ptr += 16;

	/**
	 * Смещения к данным.
	 *  - индекс словарей.
	 *  - буффер со словарями.
	 *  - шаблоны.
	 *  - дефолтные переменные.
	 *  - реестр имён словарей.
	 *  - таблица соответствий меток.
	 *  - хранилище шаблонов словарей.
	 */

	// Смещения.
	uint32_t * shifts = (uint32_t *)ptr;

	/**
	 * Индекс словарей.
	 */

	// Начало сохранённых словарей.
	ptr = aBuffer + *shifts;
	shifts++;

	// Загрузка количества словарей.
	vDictsCount = *(uint32_t *)( ptr );
	ptr += sizeof( uint32_t );

	// Загрузка индекса словарей.
	vDictsIndex = reinterpret_cast<const uint32_t *>( ptr );
	ptr += vDictsCount * sizeof( uint32_t);

	// Загрузка словарей.
	vDictionaries = ptr;

	/**
	 * Загрузка шаблонов.
	 */
	vPatterns = aBuffer+*shifts;
	shifts++;

	/**
	 * Дефолтные переменные.
	 */

	// Загрузка дефолтных значений переменных.
	ptr = aBuffer + *shifts;
	shifts++;

	unsigned int ResultSize;
	InfEngineErrors iee = vDefaultVars.Load( ptr, memory - ( ptr - aBuffer ), ResultSize );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	ptr += ResultSize;

	// Загрузка реестра имен словарей.
	ptr = aBuffer + *shifts;
	shifts++;
	// Загрузка таблицы внешних идентификаторов словарей.
	vUserDictIDsCount = *reinterpret_cast<const uint32_t *>( ptr );
	ptr += sizeof(uint32_t);
	vUserDictIDs = reinterpret_cast<const uint32_t *>( ptr );
	ptr += vUserDictIDsCount * sizeof(uint32_t);
	// Загрузка индекса имён.
	NanoLib::NameIndex::ReturnCode nlnirc = vDictsRegistry.Load( ptr, memory-(ptr-aBuffer) );
	if( nlnirc != NanoLib::NameIndex::rcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, vDictsRegistry.ReturnCodeToString( nlnirc ) );
	ptr += vDictsRegistry.GetNeedMemorySize();

	// Загрузка таблицы соответствия меток.
	vLabelCorrespondenceTable = reinterpret_cast<const uint32_t *>(aBuffer + *shifts);
	shifts++;

	ptr = (char *)vLabelCorrespondenceTable+(vLabelCorrespondenceTable[0]+1)*sizeof(uint32_t);

	// Загрузка таблицы соответствия идентификаторов шаблонов их индексам в базе.
	ptr = aBuffer + *shifts;
	shifts++;
	unsigned int table_size = *reinterpret_cast<const uint32_t *>( ptr );
	ptr += sizeof( uint32_t );
	if( table_size )
	{
		vTableIndexId = reinterpret_cast<const uint32_t *>( ptr );
		ptr += table_size * sizeof( uint32_t );
		nlnirc = vTablePatternsId.Load( ptr, memory - ( ptr - aBuffer ) );
		if( NanoLib::NameIndex::rcSuccess != ( nlnirc = vTablePatternsId.Load( ptr, memory - ( ptr - aBuffer ) ) ) )
			ReturnWithError( INF_ENGINE_ERROR_FAULT, vTablePatternsId.ReturnCodeToString( nlnirc ) );
		ptr += vTablePatternsId.GetNeedMemorySize();
	}
	else
		vTableIndexId = 0;

	// Загрузка подписи.
	ptr = aBuffer + *shifts;
	shifts++;
	memcpy( vSignature, ptr, 16 );
	ptr += 16;

	// Проверка загрузки.
	if( (unsigned int)( ptr - aBuffer ) != memory )
		ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_LOADING_RESULT );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataRO::GetDictById( unsigned int aDictId, InfDictManipulator & aDictManipulator ) const
{
	// Поиск словаря в текущей базе.
	if( aDictId < vDictsCount )
		return aDictManipulator.Attach( vDictionaries + vDictsIndex[aDictId] );

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLDataRO::GetDictByName( const char * aDictName, unsigned int aLength, InfDictManipulator & aDictManipulator ) const
{
	// Поиск словаря в текущей базе.
	const unsigned int * dict_id = vDictsRegistry.Search( aDictName, aLength );
	if( !dict_id )
		return INF_ENGINE_WARN_UNSUCCESS;

	return GetDictById( *dict_id, aDictManipulator );
}

bool DLDataRO::IsDictExisted( unsigned int aDictId ) const
{
	// Если основная база указана.
	if( vMainDataRO )
	{
		// Словарь зарегестрирован только в пользователськой базе.
		if( aDictId >= vMainDataRO->GetDictMaxId() )
			return true;

		// Словарь зарегестрирован в основной базе.
		unsigned int dict_name_len = 0;
		const char * dict_name = vMainDataRO->vDictsRegistry.GetName( aDictId, dict_name_len );

		// Поиск одноимённого словаря в пользовательской базе.
		return nullptr != vDictsRegistry.Search( dict_name, dict_name_len );
	}
	else
		return aDictId < GetDictMaxId();
}

InfEngineErrors DLDataRO::GetDictById( unsigned int aDictId, InfDictWrapManipulator & aDictManipulator ) const
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	InfDictManipulator dict_manip;

	aDictManipulator.SetDLData( this );

	if( vMainDataRO )
	{
		// Основная база указана.
		if( aDictId >= vMainDataRO->GetDictMaxId() )
		{
			// Словарь находится только в пользовательской базе.
			if( INF_ENGINE_SUCCESS != ( iee = GetDictById( aDictId - vMainDataRO->GetDictMaxId(), dict_manip ) ) )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
			// Добавление основного словаря в обёртку.
			if( INF_ENGINE_SUCCESS != ( iee = aDictManipulator.Attach( dict_manip, vDictJoiningMethod, this ) ) )
			{
				if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				else
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}
		else
		{
			// Словарь находится в основной базе.
			if( INF_ENGINE_SUCCESS != ( iee = vMainDataRO->GetDictById( aDictId, dict_manip ) ) )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
			if( INF_ENGINE_SUCCESS != ( iee = aDictManipulator.Attach( dict_manip, vDictJoiningMethod, vMainDataRO ) ) )
			{
				if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				else
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
			}

			// Поиск одноимённого словаря в пользовательской базе.
			unsigned int dict_name_len = 0;
			const char * dict_name = vMainDataRO->vDictsRegistry.GetName( aDictId, dict_name_len );

			if( dict_name )
			{
				// Если словарь стаким именем находится ещё и в пользовательской базе.
				if( INF_ENGINE_SUCCESS == ( iee = GetDictByName( dict_name, dict_name_len, dict_manip ) ) )
				{
					// Добавление пользовательского словаря в обёртку.
					if( INF_ENGINE_SUCCESS != ( iee = aDictManipulator.Attach( dict_manip, vDictJoiningMethod, this ) ) )
					{
						if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						else
							ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
					}
				}
				else if( INF_ENGINE_WARN_UNSUCCESS != iee )
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

			}
		}
	}
	else
	{
		// Родительская база не указана.
		if( INF_ENGINE_SUCCESS != ( iee = GetDictById( aDictId, dict_manip ) ) )
		{
			if( INF_ENGINE_WARN_UNSUCCESS == iee )
				return iee;
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
		// Добавление пользовательского словаря в обёртку.
		if( INF_ENGINE_SUCCESS != ( iee = aDictManipulator.Attach( dict_manip, vDictJoiningMethod, this ) ) )
		{
			if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			else
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
	}

	return INF_ENGINE_SUCCESS;
}

const unsigned int * DLDataRO::GetDictIdByName( const char * aDictName, unsigned int aLength ) const
{
	const unsigned int * dict_id = nullptr;

	// Поиск словаря в основной базе.
	if( vMainDataRO )
		if( ( dict_id = vMainDataRO->GetDictIdByName( aDictName, aLength ) ) )
			return dict_id;

	// Поиск словаря в пользовательской базе.
	if( ( dict_id = vDictsRegistry.Search( aDictName, aLength ) ) )
		return ( vMainDataRO ? &vUserDictIDs[*dict_id] : dict_id );

	return nullptr;
}

InfPatternRO & DLDataRO::GetPattern( unsigned int aPatternId, InfPatternRO & aPattern ) const
{
	try
	{
		// Очистка шаблона.
		aPattern.Clear();

		const char * buffer = static_cast<const char *>( GetPatternPtr( aPatternId ) );

		// Идентификатор шаблона.
		aPattern.vPatternId = buffer + *reinterpret_cast<const uint32_t *>( buffer ) + sizeof( uint32_t );

		// InfPerson.
		aPattern.vInfPerson = reinterpret_cast<const uint32_t *>( buffer )[5];

		// Шаблон-условия.
		aPattern.vConditions.resize( GetConditionStringsNum( buffer ) );
		for( unsigned int i = 0; i < aPattern.vConditions.size(); i++ )
			aPattern.vConditions[i] = GetConditionString( buffer, i );

		// Шаблон-вопросы.
		auto ptr = GetQuestionsBlockPtr( buffer );
		aPattern.vQuestions.resize( GetStringsNumber( ptr ) );
		for( unsigned int i = 0; i < GetStringsNumber( ptr ); i++ )
			aPattern.vQuestions[i] = GetQuestionString( ptr, i );

		// That-шаблон.
		aPattern.vThatString = GetThatString( buffer );

		// Шаблон-ответы.
		ptr = GetAnswersBlockPtr( buffer );
		aPattern.vAnswers.resize( GetStringsNumber( ptr ) );
		for( unsigned int i = 0; i < GetStringsNumber( ptr ); i++ )
			aPattern.vAnswers[i] = GetAnswerString( ptr, i );

		// Условные операторы уровня шаблона.
		ptr = GetTLCSBlockPtr( buffer );
		aPattern.vTLCS.resize( GetStringsNumber( ptr ) );
		for( unsigned int i = 0; i < GetStringsNumber( ptr ); i++ )
			aPattern.vTLCS[i] = GetTLCSString( ptr, i );

		// Метка --Label.
		aPattern.vInstrLabel = buffer + reinterpret_cast<const uint32_t *>( buffer )[6] + sizeof( uint32_t );
	}
	catch( ... )
	{
		aPattern.Clear();
	};

	return aPattern;
}

DLDataWR::DLDataWR( const DLDataRO * aMainDataRO, const char * aTmpFilePath, unsigned int aMemoryLimit ):
	vCompiledDicts( aTmpFilePath, aMemoryLimit ), vDefaultVars( vVarsRegistry )
{
	vMainDataRO = aMainDataRO;
	memset( vSignature, 0, 16 );
}

InfEngineErrors DLDataWR::Create()
{
	if( vDictsRegistry.GetState() != NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Создание реестра переменных.
	auto iee = vVarsRegistry.Create( vMainDataRO ? &vMainDataRO->GetVarsRegistry() : nullptr );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Создание реестра InfPerson'ов.
	vInfPersonRegistry.Create();

	// Добавление пустого InfPerson'а в реестр.
	unsigned int InfPersonId;
	NanoLib::NameIndex::ReturnCode nie = vInfPersonRegistry.AddName( "", 0, InfPersonId );
	if( nie != NanoLib::NameIndex::rcSuccess )
	{
		Destroy();

		ReturnWithError( INF_ENGINE_ERROR_CANT_CREATE, "Can't add empty string to InfPerson registry. Return value: %s", vInfPersonRegistry.ReturnCodeToString( nie ) );
	}

	// Создание реестра словарей.
	vDictsRegistry.Create();

	// Создание реестра условий.
	vConditionsRegistry.Create();

	// Создание реестра тэгов-функций.
	iee = vFuncTagRegistry.Create();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( iee, "Failed to create FunctagRegistry" );

	// Создание реестра меток.
	vPatternLabels.Create();

	// Сброс идентификаторов шаблонов.
	vPatternId = vMainDataRO ? vMainDataRO->GetNextPatternId() : 0;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::Save( fstorage * aFStorage )
{
	// Подготовка словарей для сохранения.
	if( !vDictsAreFinalized )
	{
		InfEngineErrors iee = FinalizeDicts();
		if( INF_ENGINE_SUCCESS != iee )
			ReturnWithTrace( iee );
	}

	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Сохранение данных.
	fstorage_section * sectdata = fstorage_get_section( aFStorage, FSTORAGE_SECTION_DLDATA );
	if( !sectdata )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Выделение памяти.
	unsigned int memory = GetNeedMemorySize();
	int ret = fstorage_section_realloc( sectdata, memory );
	if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't allocate memory in fstorage." );

	// Получение указателя на память.
	void * buffer = fstorage_section_get_all_data( sectdata );
	if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, "Can't get fstorage pointer." );

	void * ptr = buffer;

	/**
	 * - общий объем занимаемой объектом памяти.
	 * - версия протокола данных.
	 * - чексумма сохранения данных.
	 * - чексумма словарей.
	 */

	// Сохранение общего объема занимаемой объектом памяти.
	binary_data_save<uint32_t>( ptr, memory );

	// Сохранение версии протокола данных.
	binary_data_save<uint32_t>( ptr, InfDataProtocolVersion );

	// Сохранение чексуммы сохранения данных.
	auto checksumm = binary_data_skip<unsigned char>( ptr, 16 );

	// Сохранение чексуммы словарей.
	auto dicts_checksum = binary_data_skip<unsigned char>( ptr, 16 );

	/**
	 * Смещения к данным.
	 *  - буффер со словарями.
	 *  - шаблоны.
	 *  - дефолтные переменные.
	 *  - реестр имён словарей.
	 *  - таблица соответствий меток.
	 *  - хранилище шаблонов для словарей.
	 */


	// Смещение к данным.
	auto shift = binary_data_skip<uint32_t>( ptr, 7 );

	// Сдвиг к буферу словарей.
	*shift++ = binary_data_alignment( ptr, buffer );

	// Сохраняем количество словарей.
	binary_data_save<uint32_t>( ptr, vCompiledDicts.Count() );

	// Индекс словарей.
	auto index = binary_data_skip<uint32_t>( ptr, vCompiledDicts.Count() );

	// Начало словарей.
	void * dicts = ptr;

	MD5_CTX context;
	MD5Init( &context );
	for( unsigned int block_n = 0; block_n < vCompiledDicts.Count(); ++block_n )
	{
		// Сохранение смещения к очередному словарю.
		*index++ = binary_data_alignment( ptr, dicts );

		// Сохранение очередного словаря.
		binary_data_save( ptr, vCompiledDicts.Data( block_n ), vCompiledDicts.DataSize( block_n ) );

		// Вычисление md5-чексуммы словарей.
		MD5Update( &context, reinterpret_cast<const unsigned char *>( vCompiledDicts.Data( block_n ) ), vCompiledDicts.DataSize( block_n ) );
	}
	MD5Final( dicts_checksum, &context );

	// Сдвиг.
	binary_data_alignment( ptr, buffer );

	// Сохранение шаблонов.
	auto patterns = ptr;

	// Сдвиг к шаблонам относительно начала буффера сохранения.
	*shift++ = binary_data_alignment( patterns, buffer );

	// Сохраняем шаблоны.
	binary_data_save<uint32_t>( ptr, vPatternsData.size() );
	binary_data_save<uint32_t>( ptr, vMainDataRO ? vMainDataRO->GetNextPatternId() : 0 );

	// Сдвиги к шаблонам относительно начала блока шаблонов.
	auto pshifts = binary_data_skip<uint32_t>( ptr, vPatternsData.size() );

	for( unsigned int i = 0; i < vPatternsData.size(); i++ )
	{
		*pshifts++ = binary_data_alignment( ptr, patterns );
		binary_data_save( ptr, ( const char *)vPatternsData[i].first, vPatternsData[i].second );
	}

	// Сдвиг к дефолтным значениям переменных.
	*shift++ = binary_data_alignment( ptr, buffer );

	// Дефолтные значения переменных.
	binary_data_save_object( ptr, vDefaultVars );

	// Сдвиг к реестру имен словарей.
	*shift++ = binary_data_alignment( ptr, buffer );

	// Размер списка внешних идентификаторов словарей.
	binary_data_save<uint32_t>( ptr, vUserDictsIDs.size() );

	// Список внешних идентификаторов словарей.
	binary_data_save( ptr, vUserDictsIDs.get_buffer(), vUserDictsIDs.size() );

	// Индекс имён словарей.
	binary_data_save_object( ptr, vDictsRegistry );

	// Сдвиг к реестру имен словарей.
	*shift++ = binary_data_alignment( ptr, buffer );

	binary_data_save<uint32_t>( ptr, vLabelCorrespondence.size() );
	for( unsigned int i = 0; i < vLabelCorrespondence.size(); i++ )
		binary_data_save<uint32_t>( ptr, vLabelCorrespondence[i] );

	// Сдвиг к таблице соответствия идентификаторов шаблонов их индексам в базе.
	*shift++ = binary_data_alignment( ptr, buffer );

	// Таблица соответствия идентификаторов шаблонов их индексам в базе.
	if( vTableIndexId )
	{
		binary_data_save<uint32_t>( ptr, vTableIndexId->size() );
		for( unsigned int i = 0; i < vTableIndexId->size(); i++ )
			binary_data_save<uint32_t>( ptr, (*vTableIndexId)[i] );

		binary_data_save_object( ptr, *vTablePatternsId );
	}
	else
		binary_data_save<uint32_t>( ptr, 0 );

	// Сдвиг к подписи.
	*shift++ = binary_data_alignment( ptr, buffer );

	// Сохранение подписи.
	binary_data_save( ptr, vSignature, 16 );

	// Проверка корректности сохранения.
	if( (char *)ptr - (char *)buffer != memory )
		ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_SAVING_RESULT );

	// Вычисление MD5 чексуммы.
	MD5Init( &context );
	MD5Update( &context, (const unsigned char *)buffer+32, memory-32 );
	MD5Final( checksumm, &context );

	TryWithTrace( vConditionsRegistry.Save( aFStorage ), INF_ENGINE_SUCCESS );
	TryWithTrace( vVarsRegistry.Save( aFStorage ), INF_ENGINE_SUCCESS );
	TryWithTrace( vFuncTagRegistry.Save( aFStorage ), INF_ENGINE_SUCCESS );
	TryWithTrace( vPatternsStorage.Save( *aFStorage ), INF_ENGINE_SUCCESS );

	// Сохранение реестра InfPerson'ов.
	sectdata = fstorage_get_section( aFStorage, FSTORAGE_SECTION_INF_PERSON_REGISTRY );
	if( !sectdata )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get fstorage section." );

	// Выделение памяти.
	ret = fstorage_section_realloc( sectdata, vInfPersonRegistry.GetNeedMemorySize() );
	if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Получение указателя на память.
	char * base_ptr = static_cast<char *>(fstorage_section_get_all_data( sectdata ));
	if( !base_ptr )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, "Can't get fstorage pointer." );

	vInfPersonRegistry.Save( base_ptr );

	// Сохранение связей для событий.
	TryWithTrace( SaveEventsBindings( aFStorage ), INF_ENGINE_SUCCESS );

	return INF_ENGINE_SUCCESS;
}

void DLDataWR::Destroy()
{
	vPatternsData.clear();
	vDicts.clear();

	vCompiledDicts.Clear();
	vDictsRegistry.Reset();
	vUserDictsIDs.clear();
	vDefaultVars.Reset();
	vConditionsRegistry.Close();
	vFuncTagRegistry.Close();

	vPatternLabels.Reset();
	vLabelValidate.clear();
	vLabelCorrespondence.clear();

	vVarsRegistry.Close();

	vInfPersonRegistry.Destroy();

	vPatternsStorage.Reset();

	vUsedDLFunctionsIds.clear();

	vPatternId = 0;
	vDictsAreFinalized = false;

	for( auto & event : vEvents )
		event.clear();
}

InfEngineErrors DLDataWR::RegistrateTagFunction( InfPatternItems::TagFunction * aTagFunction, unsigned int & aTagFuncId, bool aIsMultiple )
{
	aTagFunction->SetMultiple( aIsMultiple );
	InfEngineErrors iee = vFuncTagRegistry.Registrate( aTagFunction, aTagFuncId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	aTagFunction->SetTagId( aTagFuncId );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::RegistrateFunction( const char * aFuncPath, unsigned int & aFuncId )
{
	if( vMainDataRO )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't add function to the extended base" );

	InfEngineErrors iee = vFuncTagRegistry.RegistryNewFunction( aFuncPath, aFuncId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::RegistrateFunction( const DLFunctionInfo * aFuncInfo, unsigned int & aFuncId )
{
	if( vMainDataRO )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't add function to the extended base" );

	InfEngineErrors iee = vFuncTagRegistry.RegistryNewFunction( aFuncInfo, aFuncId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::AddPattern( const InfPattern & aPattern, unsigned int & aPatternId )
{
	// Присвоение нового идентификатора шаблона.
	aPatternId = vPatternId++;

	// Добавление метки.
	unsigned int LabelLength = 0;
	const char * Label = aPattern.GetLabel( LabelLength );

	if( Label )
	{
		if( vPatternLabels.GetState() == NanoLib::NameIndex::stClosed )
			vPatternLabels.Create();

		unsigned int label_id;
		NanoLib::NameIndex::ReturnCode nlnie = vPatternLabels.AddName( Label, LabelLength, label_id );
		if( nlnie != NanoLib::NameIndex::rcSuccess && nlnie != NanoLib::NameIndex::rcElementExists )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		if( vLabelValidate.size() <= label_id )
		{
			unsigned int prevsize = vLabelValidate.size();
			vLabelValidate.resize( label_id+1 );
			vLabelCorrespondence.resize( label_id+1 );
			if( vLabelValidate.no_memory() || vLabelCorrespondence.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			for( ; prevsize < vLabelValidate.size(); prevsize++ )
			{
				vLabelValidate[prevsize] = false;
				vLabelCorrespondence[prevsize] = 0;
			}
		}

		vLabelValidate[label_id] = true;
		vLabelCorrespondence[label_id] = aPatternId;
	}

	// Выделением памяти для сохранения всех атрибутов шаблона.
	unsigned int memory_for_attrs = GetNeedMemorySizeForInfPattern( aPattern );
	nAllocateNewObjects( vAllocator, char, memory_for_attrs, attrs );
	if( !attrs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	try { vPatternsData.push_back( block( attrs, memory_for_attrs ) ); } catch( ... ) {
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}


	// Сохранение ссылок на блоки данных.
	char * ptr = attrs;

	// Ссылка на идентификатор.
	auto shift_id = binary_data_skip<uint32_t>( ptr );

	// Ссылка на шаблон-условия.
	auto shift_conds = binary_data_skip<uint32_t>( ptr );

	// Ссылка на шаблон-вопросы.
	auto shift_qsts = binary_data_skip<uint32_t>( ptr );

	// Ссылка на that-шаблон.
	auto shift_that = binary_data_skip<uint32_t>( ptr );

	// Ссылка на шаблон-ответы.
	auto shift_anses = binary_data_skip<uint32_t>( ptr );

	// Ссылка на условные операторы уровня шаблона.
	auto shift_tlcs = binary_data_skip<uint32_t>( ptr );

	// Ссылка на метку --Label.
	auto shift_instr_label = binary_data_skip<uint32_t>( ptr );

	// Сохранение общих аттрибутов шаблона.

	// Идентификатор infperson.
	binary_data_save<uint32_t>( ptr, aPattern.GetInfPerson() );


	/*
	 * Сохранение идентификатора шаблона.
	 * - длина идентификатора шаблона.
	 * - идентификатор шаблона.
	 */
	unsigned int pattern_id_length;
	const char * pattern_id = aPattern.GetPatternId( &pattern_id_length );

	*shift_id = binary_data_alignment( ptr, attrs );
	binary_data_save<uint32_t>( ptr, pattern_id_length );

	binary_data_save( ptr, pattern_id, pattern_id_length );
	binary_data_save( ptr, '\0' );


	/*
	 * Сохранение шаблон-условий.
	 * - смещение
	 * - количество условий.
	 * - ссылки на условия.
	 * - условия со смещениями.
	 */
	*shift_conds = binary_data_alignment( ptr, attrs );

	binary_data_save<uint32_t>( ptr, aPattern.GetConditionsNum() );

	auto shifts = binary_data_skip<uint32_t>( ptr, aPattern.GetConditionsNum() );

	for( unsigned int i = 0; i < aPattern.GetConditionsNum(); i++ )
	{
		*shifts++ = binary_data_alignment( ptr, attrs );
		binary_data_save_object( ptr, aPattern.GetCondition( i ) );
	}

	/*
	 * Сохранение шаблон-вопросов.
	 * - смещение
	 * - количество шаблон-вопросов.
	 * - ссылки на шаблон-вопросы.
	 * - шаблон-вопросы со смещениями.
	 */
	*shift_qsts = binary_data_alignment( ptr, attrs );
	auto questions_ptr = ptr;
	binary_data_save<uint32_t>( ptr, aPattern.GetQuestionNumber() );
	shifts = binary_data_skip<uint32_t>( ptr, aPattern.GetQuestionNumber() );
	InfEngineErrors iee;
	for( unsigned int i = 0; i < aPattern.GetQuestionNumber(); i++ )
	{
		unsigned int id = static_cast<unsigned int>( -1 );
		iee = AddPatternString( aPattern.GetQuestion( i ), id );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		*shifts++ = binary_data_alignment( ptr, questions_ptr );
		binary_data_save<uint32_t>( ptr, id );
		ptr += aPattern.GetQuestion( i ).SaveAttrs( ptr );
	}

	/*
	 * Сохранение that-шаблона.
	 * - смещение
	 * - that-строка.
	 */
	if( aPattern.GetThat() )
	{
		unsigned int id = static_cast<unsigned int>( -1 );
		if( INF_ENGINE_SUCCESS != ( iee = AddPatternString( *aPattern.GetThat(), id ) ) )
			ReturnWithTrace( iee );

		*shift_that = binary_data_alignment( ptr, attrs );
		binary_data_save<uint32_t>( ptr, id );
		ptr += aPattern.GetThat()->SaveAttrs( ptr );
	}
	else
		*shift_that = 0;

	/*
	 * Сохранение шаблон-ответов.
	 * - количество шаблон-ответов.
	 * - идентификаторы шаблон-ответов.
	 */

	// Смещение к блоку шаблон-ответов.
	*shift_anses = binary_data_alignment( ptr, attrs );
	auto answers_ptr = ptr;

	// Количество шаблон-овтетов.
	binary_data_save<uint32_t>( ptr, aPattern.GetAnswerNumber() );

	// Смещения к шаблон-ответам.
	shifts = binary_data_skip<uint32_t>( ptr, aPattern.GetAnswerNumber() );

	for( unsigned int i = 0; i < aPattern.GetAnswerNumber(); ++i )
	{
		// Добавление шаблон-ответа в хранилище и получение его идентификатора.
		unsigned int id = static_cast<unsigned int>( -1 );
		if( INF_ENGINE_SUCCESS != ( iee = AddPatternString( aPattern.GetAnswer( i ), id ) ) )
			ReturnWithTrace( iee );

		*shifts++ = binary_data_alignment( ptr, answers_ptr );
		binary_data_save<uint32_t>( ptr, id );
		ptr += aPattern.GetAnswer( i ).SaveAttrs( ptr );
	}

	/*
	 * Сохранение условных операторов уровня шаблона.
	 * - количество условных операторов уровня шаблона.
	 * - атрибуты условных операторов уровня шаблона.
	 */

	// Смещение к блоку условных операторов уровня шаблона.
	*shift_tlcs = binary_data_alignment( ptr, attrs );
	auto tlcs_ptr = ptr;

	// Количество условных операторова уровня шаблона.
	binary_data_save<uint32_t>( ptr, aPattern.GetTLCSNumber() );

	// Смещение к условным операторам уровня шаблона.
	shifts = binary_data_skip<uint32_t>( ptr, aPattern.GetTLCSNumber() );

	for( unsigned int i = 0; i < aPattern.GetTLCSNumber(); ++i )
	{
		// Добавление условного оператора уровня шаблона в хранилище и получение его идентификатора.
		unsigned int id = static_cast<unsigned int>( -1 );
		if( INF_ENGINE_SUCCESS != ( iee = AddPatternString( aPattern.GetTLCS( i ), id ) ) )
			ReturnWithTrace( iee );

		*shifts++ = binary_data_alignment( ptr, tlcs_ptr );
		binary_data_save<uint32_t>( ptr, id );
		ptr += aPattern.GetTLCS( i ).SaveAttrs( ptr );
	}

	/*
	 * Сохранение метки --Lable.
	 * - длина метки.
	 * - текст метки.
	 */
	unsigned int instr_label_length;
	const char * instr_label = aPattern.GetInstrLabel( &instr_label_length );

	*shift_instr_label = binary_data_alignment( ptr, attrs );
	binary_data_save<uint32_t>( ptr, instr_label_length );

	binary_data_save( ptr, instr_label, instr_label_length );
	binary_data_save( ptr, '\0' );

	// Проверка.
	if( memory_for_attrs != ptr - attrs )
		return INF_ENGINE_ERROR_CHECKSUMM;

	// Добавление связей для событий.
	if( aPattern.CheckEvents() )
	{
		try
		{
			for( int i = 0; i < aPattern.GetAnswerNumber(); i++ )
			{
				if( aPattern.CheckEvent( InfPattern::Events::Request ) )
					vEvents[(int)InfPattern::Events::Request].push_back( std::make_tuple( aPatternId, i, aPattern.GetAnswer( i ).GetWeightModifier() ) );
				if( aPattern.CheckEvent( InfPattern::Events::SetSession ) )
					vEvents[(int)InfPattern::Events::SetSession].push_back( std::make_tuple( aPatternId, i, aPattern.GetAnswer( i ).GetWeightModifier() ) );
				if( aPattern.CheckEvent( InfPattern::Events::UpdateSession ) )
					vEvents[(int)InfPattern::Events::UpdateSession].push_back( std::make_tuple( aPatternId, i, aPattern.GetAnswer( i ).GetWeightModifier() ) );
			}
		}
		catch( ... )
		{
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::RegistryDictName( const char * aDictName, unsigned int aDictNameLength )
{
	// Добавление имени в индекс.
	unsigned int DictId;
	NanoLib::NameIndex::ReturnCode nlnirc = vDictsRegistry.AddName( aDictName, aDictNameLength, DictId );
	if( nlnirc != NanoLib::NameIndex::rcSuccess )
	{
		switch( nlnirc )
		{
		case NanoLib::NameIndex::rcElementExists:
			return INF_ENGINE_WARN_UNSUCCESS;
		case NanoLib::NameIndex::rcErrorLimitIsReached:
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "Too many dicts" );
		case NanoLib::NameIndex::rcErrorNoFreeMemory:
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		default:
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't add dict name to the DictsRegistry: %s", vDictsRegistry.ReturnCodeToString( nlnirc ) );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::DiscloseDictIncludes( InfDict * aDict, std::set<unsigned int> & aDisclosedDicts )
{
	// Проверка аргументов
	if( !aDict )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Данный словарь уже раскрывался.
	if( aDisclosedDicts.find( aDict->GetID() ) != aDisclosedDicts.end() )
		return INF_ENGINE_SUCCESS;

	try
	{
		aDisclosedDicts.insert( aDict->GetID() );
	}
	catch(...)
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}


	// Поиск "--include" среди инструкций.
	for( unsigned int instr_n = 0; instr_n < aDict->GetInstructionsNum(); ++instr_n )
	{
		const InfDictInstruction * instr = aDict->GetInstruction( instr_n );

		// Обработка включаемого словаря.
		if( InfDictInstruction::IT_INCLUDE == instr->GetType() )
		{
			const InfDictInstrInclude * include = static_cast<const InfDictInstrInclude *>( instr );
			if( aDisclosedDicts.find( include->GetDictID() ) == aDisclosedDicts.end() )
			{
				// Если включается словарь из текущей базы.
				if( !vMainDataRO || include->GetDictID() < vMainDataRO->GetDictMaxId() )
				{
					InfEngineErrors iee = DiscloseDictIncludes( vDicts[include->GetDictID()], aDisclosedDicts );
					if( INF_ENGINE_SUCCESS != iee )
						ReturnWithTrace( iee );
				}
				// Если включается словарь из основной базы.
				else
				{
					// Включение подсловарей словаря из основной базы реализуется в обёртке-манипуляторе.
					try
					{
						aDisclosedDicts.insert( include->GetDictID() );
					} catch(...)
					{
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					}
				}
			}
		}
	}


	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::DiscloseDictsIncludes()
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	for( unsigned int dict_n = 0; dict_n < vDicts.size(); ++dict_n )
	{
		// Список раскрытых словарей. Чтобы не допустить зацикливания.
		std::set<unsigned int> disclosed_dicts;
		if( INF_ENGINE_SUCCESS != ( iee = DiscloseDictIncludes( vDicts[dict_n], disclosed_dicts ) ) )
			ReturnWithTrace( iee );

		// Создание списка всех включаемых словарей.
		unsigned int disclosed_lines_count = vDicts[dict_n]->GetStringsNum();
		unsigned int ids_cnt = 0;
		unsigned int * ids = nullptr;
		if( disclosed_dicts.size() )
		{
			ids_cnt = disclosed_dicts.size() - 1;
			ids = nAllocateObjects( vAllocator, unsigned int, ids_cnt );
			unsigned int n = 0;
			for( std::set<unsigned int>::const_iterator iter = disclosed_dicts.begin(); iter != disclosed_dicts.end(); ++iter )
			{
				// Есловарь не может включать сам себя.
				if( *iter != vDicts[dict_n]->GetID() )
				{
					ids[n++] = *iter;
					disclosed_lines_count += vDicts[*iter]->GetStringsNum();
				}
			}
		}

		vDicts[dict_n]->SetDisclosedStringsCount( disclosed_lines_count );

		vDicts[dict_n]->SetSubdicts( ids, ids_cnt );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::SaveDict( const InfDict * aDict )
{
	// Вычисление количества памяти, необходимого для сохранения словаря.

	unsigned int dict_size = aDict->GetNeedMemorySize();
	unsigned int used_memory = 0;

	if( dict_size )
	{
		// Выделение участка памяти для сохранения словаря.
		char * buffer = vCompiledDicts.Add( dict_size );
		if( !buffer )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Сохранения словаря.
		InfEngineErrors iee = aDict->Save( buffer, dict_size, used_memory );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithError( iee, "Can't save dictionary '%s'. Return code: %d", aDict->GetName(), iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::SaveEventsBindings( fstorage * aFStorage )
{
	size_t memory = sizeof( uint32_t );
	for( auto & events : vEvents )
	{
		std::sort( events.begin(), events.end(), []( const EventType & a, const EventType & b ) -> bool { return std::get<2>(a) < std::get<2>(b); } );
		memory += sizeof( uint32_t ) * ( 2 + 2 * events.size() );
	}

	// Сохранение данных.
	fstorage_section * sectdata = fstorage_get_section( aFStorage, FSTORAGE_SECTION_DLDATA_EVENTS_BINDINGS );
	if( !sectdata )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Выделение памяти.
	if( fstorage_section_realloc( sectdata, memory ) != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't allocate memory in fstorage." );

	// Получение указателя на память.
	void * buffer = fstorage_section_get_all_data( sectdata );
	if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, "Can't get fstorage pointer." );

	constexpr unsigned int size = sizeof( vEvents ) / sizeof( std::vector<EventType> );

	// Сохранение числа событий.
	void * ptr = buffer;
	binary_data_save<uint32_t>( ptr, size );

	// Сохранение ссылок на события.
	auto shifts = binary_data_skip<uint32_t>( ptr, size );

	// Сохранение событий.
	for( auto & events : vEvents )
	{
		*shifts++ = binary_data_alignment( ptr, buffer ) / sizeof( uint32_t );
		binary_data_save<uint32_t>( ptr, events.size() );
		for( auto & event : events )
		{
			binary_data_save<uint32_t>( ptr, std::get<0>( event ) );
			binary_data_save<uint32_t>( ptr, std::get<1>( event ) );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::FinalizeDicts()
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Проверка необходимости выполнения процедуры.
	if( vDictsAreFinalized )
		return INF_ENGINE_SUCCESS;

	// Раскрытие инструкций "--include" в словарях.
	if( INF_ENGINE_SUCCESS != ( iee = DiscloseDictsIncludes() ) )
		ReturnWithTrace( iee );

	// Сохранение словарей.
	vCompiledDicts.Clear();
	for( unsigned int dict_n = 0; dict_n < vDicts.size(); ++dict_n )
	{
		if( INF_ENGINE_SUCCESS != ( iee = SaveDict( vDicts[dict_n] ) ) )
			ReturnWithTrace( iee );
	}

	// Поднятия флага готовности словарей к сохранению.
	vDictsAreFinalized = true;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::AddDict( InfDict * aDict )
{
	// Добавление словаря в список разобранных.
	vDicts.push_back( aDict );
	if( vDicts.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Опускание флага готовности словарей к сохранению.
	vDictsAreFinalized = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::AddMainDict( InfDict * aDict )
{
	// Получение идентификатора словаря в текущей базе.
	unsigned int dict_id = aDict->GetID();

	// Добавление внешнего идентификатора.
	vUserDictsIDs.push_back( dict_id );
	if( vUserDictsIDs.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	if( dict_id != vUserDictsIDs.size() - 1 )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Добавлене словаря.
	InfEngineErrors iee = AddDict( aDict );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLDataWR::AddUserDict( InfDict * aDict )
{
	// Поиск одноимённого словаря в основной базе.
	const unsigned int * main_base_id = nullptr;
	if( vMainDataRO )
		main_base_id = vMainDataRO->GetDictIdByName( aDict->GetName(), aDict->GetNameLength() );

	// Получение идентификатора словаря в текущей базе.
	unsigned int dict_id = aDict->GetID();

	// Добавление внешнего идентификатора.
	if( dict_id >= vUserDictsIDs.size() )
	{
		vUserDictsIDs.push_back( main_base_id ? *main_base_id : dict_id + ( vMainDataRO ? vMainDataRO->GetDictsCount() : 0 ) );
		if( vUserDictsIDs.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		if( dict_id != vUserDictsIDs.size() - 1 )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	}
	// Установка нового идентификатора словаря.
	aDict->SetID( vUserDictsIDs[dict_id] );

	// Добавлене словаря.
	InfEngineErrors iee = AddDict( aDict );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

const unsigned int * DLDataWR::GetDictId( const char * aDictName, unsigned int aLength ) const
{
	const unsigned int * dict_id = nullptr;
	// Поиск словаря в основной базе.
	if( vMainDataRO )
	{
		if( ( dict_id = vMainDataRO->GetDictIdByName( aDictName, aLength ) ) )
			return dict_id;
	}

	// Поиск словаря в пользовательской базе.
	dict_id = vDictsRegistry.Search( aDictName, aLength );
	if( !dict_id )
		return nullptr;

	// Получение внешнего идентификатора словаря пользовательской базы.
	return ( vMainDataRO ? &vUserDictsIDs[*dict_id] : dict_id );
}

unsigned int DLDataWR::GetNeedMemorySizeForInfPattern( const InfPattern & aPattern )
{
	// Вычисление необходимой памяти.
	/**
	 * - ссылка на идентификатор.
	 * - ссылка на условия.
	 * - ссылка на шаблон-вопросы.
	 * - ссылка на that-строку.
	 * - ссылка на шаблон-ответы.
	 * - ссылка на условные операторы уровня шаблона.
	 * - ссылка на метку --Lable.
	 * - внутренний идентификатор.
	 * - InfPerson.
	 */
	unsigned int NeedMemorySize = 8*sizeof( uint32_t );

	// Идентификатор шаблона.
	unsigned int pattern_id_length;
	aPattern.GetPatternId( &pattern_id_length );
	NeedMemorySize += sizeof( uint32_t ) + pattern_id_length + 1;

	// Шаблон-условия.
	binary_data_alignment( NeedMemorySize );
	NeedMemorySize += sizeof( uint32_t ) + aPattern.GetConditionsNum()*sizeof( uint32_t );
	for( unsigned int i = 0; i < aPattern.GetConditionsNum(); i++ )
	{
		binary_data_alignment( NeedMemorySize );
		NeedMemorySize += aPattern.GetCondition( i ).GetNeedMemorySize();
	}

	// Шаблон-вопросы.
	binary_data_alignment( NeedMemorySize );
	NeedMemorySize += sizeof( uint32_t ) + aPattern.GetQuestionNumber()*sizeof( uint32_t );
	for( unsigned int i = 0; i < aPattern.GetQuestionNumber(); i++ )
	{
		binary_data_alignment( NeedMemorySize );
		NeedMemorySize += sizeof( uint32_t ) +aPattern.GetQuestion( i ).GetNeedMemorySizeForAttrs();
	}

	// That-строка.
	binary_data_alignment( NeedMemorySize );
	NeedMemorySize += aPattern.GetThat() ? sizeof( uint32_t ) + aPattern.GetThat()->GetNeedMemorySizeForAttrs() : 0;

	// Шаблон-ответы.
	binary_data_alignment( NeedMemorySize );
	NeedMemorySize += sizeof( uint32_t ) + aPattern.GetAnswerNumber()*sizeof( uint32_t );
	for( unsigned int i = 0; i < aPattern.GetAnswerNumber(); i++ )
	{
		binary_data_alignment( NeedMemorySize );
		NeedMemorySize += sizeof (uint32_t ) + aPattern.GetAnswer( i ).GetNeedMemorySizeForAttrs();
	}

	// Условные операторы уровня шаблона.
	binary_data_alignment( NeedMemorySize );
	NeedMemorySize += sizeof( uint32_t ) + aPattern.GetTLCSNumber()*sizeof( uint32_t );
	for( unsigned int i = 0; i < aPattern.GetTLCSNumber(); i++ )
	{
		binary_data_alignment( NeedMemorySize );
		NeedMemorySize += sizeof (uint32_t ) + aPattern.GetTLCS( i ).GetNeedMemorySizeForAttrs();
	}

	// Идентификатор шаблона.
	binary_data_alignment( NeedMemorySize );
	unsigned int instr_label_length;
	aPattern.GetInstrLabel( &instr_label_length );
	NeedMemorySize += sizeof( uint32_t ) + instr_label_length + 1;

	return NeedMemorySize;
}

unsigned int DLDataWR::GetNeedMemorySize() const
{
	/**
	 * - общий объем занимаемой объектом памяти.
	 * - версия протокола данных.
	 * - чексумма сохранения данных.
	 * - чексумма словарей.
	 */
	unsigned int NeedMemorySize = 2*sizeof( uint32_t )+16+16;

	/**
	 * Смещения к данным.
	 *  - буффер со словарями.
	 *  - шаблоны.
	 *  - дефолтные переменные.
	 *  - реестр имен словарей.
	 *  - таблица соответствий меток.
	 *  - таблица соответствия строковых идентификаторов шаблонов их номерам в базе.
	 *  - подпись.
	 */
	NeedMemorySize += 7*sizeof( uint32_t );

	// Буффер словарей.
	NeedMemorySize += sizeof( uint32_t ) + vCompiledDicts.Count() * sizeof( uint32_t );
	for( unsigned int i = 0; i < vCompiledDicts.Count(); i++ )
	{
		binary_data_alignment( NeedMemorySize );
		NeedMemorySize += vCompiledDicts.DataSize( i );
	}
	// Сдвиг.
	NeedMemorySize += NeedMemorySize%4 ? 4-NeedMemorySize%4 : 0;

	// Шаблоны.
	NeedMemorySize += ( 2 + vPatternsData.size() )*sizeof( uint32_t );
	for( unsigned int i = 0; i < vPatternsData.size(); i++ )
	{
		/** Сдвиг. **/
		NeedMemorySize += NeedMemorySize%4 ? 4-NeedMemorySize%4 : 0;

		/** Шаблон. **/
		NeedMemorySize += vPatternsData[i].second;
	}

	// Сдвиг.
	NeedMemorySize += NeedMemorySize%4 ? 4-NeedMemorySize%4 : 0;

	/**
	 * Дефолтные переменные.
	 */
	NeedMemorySize += vDefaultVars.GetNeedMemorySize();

	/** Сдвиг. **/
	NeedMemorySize += NeedMemorySize%4 ? 4-NeedMemorySize%4 : 0;

	// Реестр имен словарей.
	NeedMemorySize += sizeof(uint32_t) + vUserDictsIDs.size() * sizeof(uint32_t);
	NeedMemorySize += vDictsRegistry.GetNeedMemorySize();

	// Таблица соответствия меток.
	NeedMemorySize += NeedMemorySize%4 ? 4-NeedMemorySize%4 : 0;
	NeedMemorySize += sizeof(uint32_t)+vLabelCorrespondence.size()*sizeof(uint32_t);

	/** Сдвиг. **/
	NeedMemorySize += NeedMemorySize%4 ? 4-NeedMemorySize%4 : 0;

	/** Таблица соответствия строковых идентификаторов шаблонов их номерам в базе. **/
	NeedMemorySize += sizeof(uint32_t);                         // Количество.
	if( vTableIndexId )
	{
		NeedMemorySize += vTableIndexId->size() * sizeof(uint32_t); // Таблица.
		NeedMemorySize += vTablePatternsId->GetNeedMemorySize();    // Оригинальные строковые идентификаторы (для отладки, потом их можно будет убрать).
	}

	/** Сдвиг. **/
	NeedMemorySize += NeedMemorySize%4 ? 4-NeedMemorySize%4 : 0;

	/** Сдвиг. **/
	NeedMemorySize += NeedMemorySize%4 ? 4-NeedMemorySize%4 : 0;

	// Подпись.
	NeedMemorySize += 16;

	return NeedMemorySize;
}

void DLDataWR::GetTagFuncRegistryStat( unsigned int & aRealSize, unsigned int & aIndexedSize,
									   unsigned int & aTagFuncCount, unsigned int & aUniqueTagFuncCount,
									   unsigned int & aMultipleTagFuncCount,
									   unsigned int & aHashTableMaxListLen, double & aHashTableAvgListLen) const

{
	const FuncTagRegistry & FuncTagReg = vMainDataRO ? vMainDataRO->GetFuncTagRegistry() : vFuncTagRegistry;

	aRealSize = FuncTagReg.GetRealSize();
	aIndexedSize = FuncTagReg.GetIndexedSize();
	aTagFuncCount = FuncTagReg.GetTagFunctionsCount();
	aUniqueTagFuncCount = FuncTagReg.GetUniqueTagFunctionsCount();
	aMultipleTagFuncCount = FuncTagReg.GetMultipleTagFunctionsCount();
	aHashTableMaxListLen = FuncTagReg.GetHashTableMaxListLen();
	aHashTableAvgListLen = FuncTagReg.GetHashTableAvgListLen();
}

InfEngineErrors DLDataWR::SetFunctionUsed( unsigned int aFuncId )
{
	// Проверка идентификатора функции.
	const FunctionsRegistry & functions_registry = GetFunctionsRegistry();
	if( functions_registry.GetFunctionsNumber() <= aFuncId )
		ReturnWithError( INF_ENGINE_ERROR_INVALID_ARG, "Function id %u is out of bounds [0;%u)", aFuncId, functions_registry.GetFunctionsNumber() );

	// Увеличение массива флагов при необходимости.
	if( vUsedDLFunctionsIds.size() <= aFuncId )
	{
		unsigned int prev_size = vUsedDLFunctionsIds.size();
		vUsedDLFunctionsIds.resize( functions_registry.GetFunctionsNumber() );
		if( vUsedDLFunctionsIds.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		memset( vUsedDLFunctionsIds.get_buffer() + prev_size, 0, sizeof(bool) * (vUsedDLFunctionsIds.size() - prev_size ) );
	}

	// Отмечаем, что данная функция использовалась в шаблонах.
	vUsedDLFunctionsIds[aFuncId] = true;

	return INF_ENGINE_SUCCESS;
}