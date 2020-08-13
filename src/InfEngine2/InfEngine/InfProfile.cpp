#include "InfProfile.hpp"

#include <cstring>
#include <unistd.h>

InfProfile::InfProfile( const FunctionsRegistry & aFunctionsRegistry ) :
	vFunctionsRegistry( aFunctionsRegistry )
{
	// Очистка чексуммы базы основания.
	memset( vFoundationChecksum, 0, 16 );
}

InfEngineErrors InfProfile::CreateFoundation() {
	// Закрытие базы основания, если она открыта.
	Close();

	// Создание основания.
	vFoundation = fstorage_create();
	if( !vFoundation )
	{
		LogError( "Can't create fstorage. Return Code: %d", fstorage_last_error( vFoundation ) );

		Close();

		return INF_ENGINE_ERROR_FSTORAGE;
	}

	// Установка статуса.
	vState = State::FoundationIsLoaded;

	return INF_ENGINE_SUCCESS;
}

void InfProfile::Close() {
	// Основание профиля инфа.
	if( vFoundation ) {
		fstorage_close( vFoundation );
		fstorage_destroy( vFoundation );
	}
	vFoundation = nullptr;

	// Очистка чексуммы основания.
	memset( vFoundationChecksum, 0, 16 );

	// Состояние объекта.
	vState = State::Empty;
}

const char* InfProfile::CreateKey( unsigned int aInfId, size_t & aKeyLength ) {
	static char key[CacheKeyLength];

	::CreateKey( aInfId, key, aKeyLength );

	return key;
}

InfProfileWR::InfProfileWR( ICoreRO & aMainCore, const char * aTmpFilePath2, unsigned int aMemoryLimit ) :
	InfProfile( aMainCore.GetDLData().GetFunctionsRegistry() ),
    vExtBase( &aMainCore, &aMainCore.GetIndexBase().GetSymbolymBase(), &aMainCore.GetIndexBase().GetSynonymBase(), aTmpFilePath2, aMemoryLimit )
{}

InfEngineErrors InfProfileWR::CreateFoundation() {
	auto iee = InfProfile::CreateFoundation();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Создание snapshot'а.
	int ret = fstorage_trx_snapshot( vFoundation, FSTORAGE_TRX_READ_WRITE );
	if( ret != 0 ) {
		LogError( "Can't make fstorage snapshot. Return Code: %d", ret );

		Close();

		return INF_ENGINE_ERROR_FSTORAGE;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfProfileWR::Reset() {
	// Сброс вспомогательных объектов.
	vExtBase.Destroy();
	vSections.clear();

	if( vState == State::Empty || vState == State::FoundationIsLoaded )
		return INF_ENGINE_SUCCESS;

	// Откат fstorage.
	int ret = fstorage_trx_rollback( vFoundation );
	if( ret != 0 ) {
		LogError( "Can't rallback fstorage. Return Code: %d", ret );

		Close();

		return INF_ENGINE_ERROR_FSTORAGE;
	}

	// Создание snapshot'а.
	ret = fstorage_trx_snapshot( vFoundation, FSTORAGE_TRX_READ_WRITE );
	if( ret != 0 ) {
		LogError( "Can't make fstorage snapshot. Return Code: %d", ret );

		Close();

		return INF_ENGINE_ERROR_FSTORAGE;
	}

	// Установка статуса.
	vState = State::FoundationIsLoaded;

	return INF_ENGINE_SUCCESS;
}

void InfProfileWR::Close() {
	// Очистка вспомогательных объектов.
	vExtBase.Destroy();
	vSections.free_buffer();

	InfProfile::Close();
}

InfEngineErrors InfProfileWR::Create( const char * aTemplates, unsigned int aTemplatesSize, avector<Vars::Raw> & aSourceVars,
									  const char ** aDictsNames, const char ** aDicts, unsigned int aDictsNumber, bool aIgnoreErrors ) {
	InfEngineErrors iee;

	// Проверка аргументов.
	if( !aTemplates )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка состояния.
	if( vState == State::Empty )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );
	else if( vState == State::ExtensionIsCreated ) {
		// Сброс расширения.
		iee = Reset();
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	// Создание компилятора.
	iee = vExtBase.Create();
	if( iee != INF_ENGINE_SUCCESS ) {
		Reset();

		ReturnWithTrace( iee );
	}

	for( unsigned int i = 0; i < aSourceVars.size(); i++ ) {
		Vars::Id id;
		auto iee = vExtBase.RegistryNewVariable( aSourceVars[i].vName, std::strlen( aSourceVars[i].vName ), id );
		if( iee != INF_ENGINE_WARN_ELEMENT_EXISTS && iee != INF_ENGINE_SUCCESS ) {
			Reset();

			ReturnWithTrace( iee );
		}

		iee = vExtBase.AddVariable( id, aSourceVars[i].vValue, std::strlen( aSourceVars[i].vValue ) );
		if( iee != INF_ENGINE_SUCCESS ) {
			Reset();

			ReturnWithTrace( iee );
		}
	}

	// Освобождение памяти, выделенной ранее для замены синонимов.
	vExtBase.ResetSynonymsMaps();

	// Регистрация имён словарей.
	for( unsigned int dict_n = 0; dict_n < aDictsNumber; dict_n++ )
		if( INF_ENGINE_SUCCESS != ( iee = vExtBase.RegistryDictName( aDictsNames[dict_n], strlen( aDictsNames[dict_n] ) ) ) )
			if( INF_ENGINE_WARN_UNSUCCESS != iee )
				ReturnWithTrace( iee );

	//Компиляция словарей.
	aTextString error_description;
	aTextString error_string;
	for( unsigned int dict_n = 0; dict_n < aDictsNumber; dict_n++ ) {
		iee = vExtBase.CompileDictFromBuffer( aDictsNames[dict_n], strlen( aDictsNames[dict_n] ), aDicts[dict_n], strlen( aDicts[dict_n] ),
											  error_description, error_string, NanoLib::Encoding::UTF8, true );
		if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE ) {
			//  Если стоит флаг игнорирования ошибок, то компиляция прошедшая не полностью считается
			// достаточной и сообщение об ошибке не генерируется.
			ReturnWithTrace( iee );
		}
	}

	// Компиляция шаблонов инфа.
	iee = vExtBase.CompilePatternsFromBuffer( aTemplates, aTemplatesSize, aIgnoreErrors );
	if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE ) {
		//  Если стоит флаг игнорирования ошибок, то компиляция прошедшая не полностью считается
		// достаточной и сообщение об ошибке не генерируется.
		ReturnWithTrace( iee );
	}

	for( auto & error : vExtBase.GetWarnings() )
		LogDebug( "WARNING: %s, Object Id: %s, String: %s", error.description(), error.GetObjectId().c_str(), error.GetString().c_str() );

	for( auto & error : vExtBase.GetErrors() )
		LogDebug( "ERROR: %s, Object Id: %s, String: %s", error.description(), error.GetObjectId().c_str(), error.GetString().c_str() );

	// Сохранение ядра.
	iee = vExtBase.Save( vFoundation );
	if( iee != INF_ENGINE_SUCCESS ) {
		Reset();

		ReturnWithTrace( iee );
	}

	// Изменение статуса.
	vState = State::ExtensionIsCreated;

	return INF_ENGINE_SUCCESS;
}

int CommitInfProfile( struct fstorage_trx_committer * fscm, fstorage_section_id sid, void * buf, size_t buf_size ) {
	// Преобразование структуры.
	auto * Sections = static_cast<avector<FStorageSection>*>( fscm->context );

	FStorageSection * sect { Sections->grow() };
	if( !sect )
		return FSTORAGE_ERROR_NOMEMORY;

	sect->vId = sid;
	sect->vData = buf;
	sect->vDataSize = buf_size;

	return FSTORAGE_OK;
};

InfEngineErrors InfProfileWR::Save( unsigned int aInfId, NanoLib::Cache & aCache, unsigned int & CacheInsertTm ) {
	// Проверка состояния.
	if( vState != State::ExtensionIsCreated )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	struct fstorage_trx_committer fscm;
	fscm.context = &vSections;
	fscm.receive_section = CommitInfProfile;
	int res = fstorage_trx_commit( vFoundation, &fscm );
	if( res != 0 )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, "Can't commit inf profile into cache. Return Code: %d", res );

	// Подсчет необходимой памяти.
	size_t memory = 2 * sizeof( uint32_t );
	for( unsigned int i = 0; i < vSections.size(); i++ )
	{
		// Проверка длины секции.
		if( vSections[i].vDataSize > (uint32_t)-1 )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, "Can't save section: section size to large ( %zu )", vSections[i].vDataSize );

		memory += sizeof( uint32_t );

		// Выравнивание до кратности 32ух бит.
		if( memory % sizeof( uint32_t ) )
			memory += sizeof( uint32_t ) - memory % sizeof( uint32_t );

		memory += sizeof( uint32_t );
		memory += sizeof( fstorage_section_id );
		memory += vSections[i].vDataSize;
	}

	// Память для сохранения md5 суммы реестра функций.
	memory += 16;

	// Выделение памяти.
	char * buffer = static_cast<char*>( malloc( memory ) );
	if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Создание данных.
	uint32_t * ptr = reinterpret_cast<uint32_t*>( buffer );
	*ptr = memory;
	ptr++;
	*ptr = vSections.size();
	ptr++;

	char * data = buffer + ( vSections.size() + 2 ) * sizeof( uint32_t );

	for( unsigned int i = 0; i < vSections.size(); i++ ) {
		unsigned int mem = data - buffer;
		// Выравнивание до кратности 32ух бит.
		if( mem % sizeof( uint32_t ) )
			data += sizeof( uint32_t ) - mem % sizeof( uint32_t );

		// Сохранение смещения к секции.
		*ptr = data - buffer;
		ptr++;

		// Сохранение идентификатора секции.
		*( reinterpret_cast<fstorage_section_id*>( data ) ) = vSections[i].vId;
		data += sizeof( fstorage_section_id );

		// Сохранение длины секции.
		*( reinterpret_cast<uint32_t*>( data ) ) = vSections[i].vDataSize;
		data += sizeof( uint32_t );

		// Сохранение данных секции.
		memcpy( data, vSections[i].vData, vSections[i].vDataSize );
		data += vSections[i].vDataSize;
	}

	// Проверка md5 суммы реестра функций.
	unsigned char aDigits[16];
	vFunctionsRegistry.GetMD5Digits( aDigits );
	memcpy( data, aDigits, 16 );

	// Создание ключа.
	size_t keylen;
	const char * key = CreateKey( aInfId, keylen );
	if( !key ) {
		free( buffer );
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	// Сохранение в кэш.
	TimeStamp cache_ts;
	auto nce = aCache.InsertData( key, keylen, buffer, memory );
	if( nce != NanoLib::Cache::rcSuccess ) {
		free( buffer );
		ReturnWithTrace( INF_ENGINE_ERROR_CACHE );
	}
	// Время записи в кэш.
	CacheInsertTm = cache_ts.Interval();

	// Очистка памяти.
	free( buffer );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfProfileWR::LoadAliasesFromFile( const char * aAliasesPath )
{
	// Количество успешно скомпилированных алиасов.
	unsigned int SuccessAliases;
	// Количество алиасов, при компиляции которых произошла ошибка.
	unsigned int FaultAliases;
	// Список ошибок, произошедших при компиляции.
	AliasParser::Errors Errors;

	// Создание компилятора.
	InfEngineErrors iee = vExtBase.Create();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Загрузка алиасов.
	iee = vExtBase.CompileAliasesFromFile( aAliasesPath, SuccessAliases, FaultAliases, Errors );
	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return INF_ENGINE_ERROR_CANT_PARSE;
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}
