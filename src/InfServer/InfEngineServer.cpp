#include "InfEngineServer.hpp"

#include <limits>

#include <ctime>
#include <unistd.h>

#include <ClientLib/Protocol.hpp>

/**
 *  Выделение памяти под объект csdata_data_t с aAttrNum аттрибутами. Выделение
 * производится через заданный аллокатор aMemoryAllocator.
 */
csdata_data_t * csDataAllocate( unsigned int aAttrNum, nMemoryAllocator &aMemoryAllocator )
{
	// Выделение памяти под основной объект.
	csdata_data_t * data = ( csdata_data_t * )aMemoryAllocator.Allocate( sizeof( csdata_data_t ) );
	if( !data )
		ReturnWithError( nullptr, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Заполнение полей основного объекта.
	data->body_ptr = nullptr;
	data->body_size = 0;

	// Выделение памяти для аттрибутов объекта.
	data->attr_count = aAttrNum;
	if( aAttrNum )
	{
		data->attr_list = ( csdata_attr_t* )aMemoryAllocator.Allocate( sizeof( csdata_attr_t ) * aAttrNum );
		if( !data->attr_list )
			ReturnWithError( nullptr, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}
	else
		data->attr_list = nullptr;

	return data;
}

InfEngineServer::InfEngineServer( const char * aDictsSwapFilePath, unsigned int aMemoryLimit ) :
	vUserSession( vBase.GetExtDLData().GetVarsRegistry() ),
	vInfProfileWR( vBase, aDictsSwapFilePath, aMemoryLimit ),
	vAnswer( true ),
	vEventInstructions( true ),
	vEllipsisAnswer( true )
{
	// Установка переменных по умолчанию для сессии.
	vUserSession.SetDefaultValues( vBase.GetVarsValues() );
}

InfEngineErrors InfEngineServer::AddCacheServer( const char * aHost, unsigned int aPort )
{
	NanoLib::Cache::ReturnCode nce;

	// Инициализация кэша.
	if( !vCache.IsInitialized() )
	{
		if( ( nce = vCache.Init() ) != NanoLib::Cache::rcSuccess )
			ReturnWithTrace( INF_ENGINE_ERROR_CACHE );
	}

	// Добавление сервера.
	if( ( nce = vCache.AddServer( aHost, aPort ) ) != NanoLib::Cache::rcSuccess )
		ReturnWithTrace( INF_ENGINE_ERROR_CACHE );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfEngineServer::SetUpEllipsisEngine( EllipsisDiscloseFunction aEllipsisDiscloseFunction, EllipsisMode aEllipsisMode, EllipsisDefault aEllipsisDefault )
{
	// Проверка аргументов.
	if( aEllipsisMode != EllipsisMode::Off && !aEllipsisDiscloseFunction )
		return INF_ENGINE_ERROR_INV_ARGS;

	vEllipsisMode = aEllipsisMode;
	vEllipsisDefault = aEllipsisDefault;
	vEllipsisDiscloseFunction = aEllipsisDiscloseFunction;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfEngineServer::Open( const char * aBasePath,
		const char * aFunctionsRootDir, const char * aFunctionsConfigPath, const char * aAliasesConfigPath )
{
	// Открытие базы.
	InfEngineErrors iee = OpenBase( aBasePath, aFunctionsRootDir, aFunctionsConfigPath );
	if( iee == INF_ENGINE_ERROR_STATE )
		ReturnWithTrace( iee );
	else if( iee != INF_ENGINE_SUCCESS )
	{
		Close();

		ReturnWithTrace( iee );
	}

	// Загрузка и регистрация алиасов.
	if( aAliasesConfigPath )
	{
		if( access( aAliasesConfigPath, R_OK ) == -1 )
		{
			switch( errno )
			{
			case ENOENT:
				LogWarn( "Aliases config file '%s' doesn't exists", aAliasesConfigPath );

			default:
				LogWarn( "Aliases config file '%s' is not accessible: %s", aAliasesConfigPath, strerror( errno ) );
			}
		}
		else
		{
			iee = vInfProfileWR.LoadAliasesFromFile( aAliasesConfigPath );
			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				LogWarn( "Alias compiled with errors" );
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
		}
	}

	// Создание основания для пользовательской базы.
	if( ( iee = vInfProfileWR.CreateFoundation() ) )
	{
		Close();

		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

void InfEngineServer::Close()
{
	vState = State::Closed;

	vEllipsisMode = EllipsisMode::Off;
	vEllipsisDefault = EllipsisDefault::None;

	vUserSession.Reset();

	vInfProfileWR.Close();

	vEllipsisDiscloseFunction = nullptr;

	vAnswer.Reset();

	vEllipsisAnswer.Reset();

	ExtISearchEngine::CloseBase();
}

InfEngineErrors InfEngineServer::CmdPurgeSession( unsigned int aSessionId )
{
	InfEngineErrors iee;
	if( ( iee = vUserSession.Delete( aSessionId, vCache ) ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfEngineServer::CmdSetSession( unsigned int aSessionId, unsigned int aInfId, avector<Vars::Raw> & aSession )
{
	// Для работы с чистой сессией используем реестр из основной базы.
	if( !vUserSession.SwitchRegistry( vBase.GetDLData().GetVarsRegistry() ) )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Corrupted data: Can't switch vars registry." );

	// Преобразование сессии.
	unsigned int UnknownVariables = 0;
	TryWithTrace( vUserSession.Set( aInfId, aSession, UnknownVariables ), INF_ENGINE_SUCCESS );

	if( UnknownVariables )
		LogWarn( "Received session with %u unknown session variables.", UnknownVariables );

	// Обработка события SetSession.
	TryWithTrace( ProcessEvent( InfPattern::Events::SetSession ), INF_ENGINE_SUCCESS );

	// Сохранение сессии.
	ReturnWithTraceExt( vUserSession.Save( aSessionId, vCache ), INF_ENGINE_SUCCESS );
}

InfEngineErrors InfEngineServer::CmdUpdateSession( unsigned int aSessionId, const avector<Vars::Raw> & aVarList )
{
	// Для работы с чистой сессией используем реестр из основной базы.
	if( !vUserSession.SwitchRegistry( vBase.GetDLData().GetVarsRegistry() ) )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Corrupted data: Can't switch vars registry." );

	// Загрузка сессии из кэша.
	InfEngineErrors iee = vUserSession.Load( aSessionId, vCache );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return INF_ENGINE_SUCCESS;
	else if( iee == INF_ENGINE_ERROR_CHECKSUMM )
	{
		// Удаление неверных данных из кэша.
		if( ( iee = vUserSession.Delete( aSessionId, vCache ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		return INF_ENGINE_SUCCESS;
	}
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Изменение значений переменных.
	aTextString tmp;
	for( unsigned int var_n = 0; var_n < aVarList.size(); ++var_n )
	{
		// Приведение имени переменной к нижнему регистру.
		if( tmp.assign( aVarList[var_n].vName ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		SetLower( tmp );

		iee = vUserSession.SetTextValue( tmp.ToConstChar(), tmp.size(), aVarList[var_n].vValue, strlen( aVarList[var_n].vValue ) );
		switch( iee )
		{
		case INF_ENGINE_WARN_UNSUCCESS:
			LogWarn( "Unknown variable: %s", tmp.ToConstChar() );
		case INF_ENGINE_SUCCESS:
			break;
		default:
			ReturnWithTrace( iee );
		}
	}

	// Обработка события SetSession.
	TryWithTrace( ProcessEvent( InfPattern::Events::UpdateSession ), INF_ENGINE_SUCCESS );

	// Сохранение сессии.
	if( INF_ENGINE_SUCCESS != ( iee = vUserSession.Save( aSessionId, vCache ) ) )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfEngineServer::CmdRequest( unsigned int aSessionId, unsigned int aInfId,
		const char * aRequest, unsigned int aRequestLength,
		const avector<Vars::Raw> & aVarsList,
		csdata_data_t ** aData, nMemoryAllocator & aCSDataAllocator )
{
	// Структура со статистикой выполнения запроса.
	RequestStat request_stat;
	TimeStamp request_ts;
	bool search_response = true;

	// Проверка аргументов.
	if( !aRequest || !aData )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка состояния.
	if( !vBase.IsOpened() || !vBase.IsExtBasePrepared() )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	int MissedMask = 0;
	vAnswer.Reset();

	// Получение инфа.
	bool success { false };
	auto iee = LoadUserBase( aInfId, request_stat, success );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	if( !success )
	{
		MissedMask |= INF_MASK_INF;

		// При отсутствии инфа нужно использовать реестр переменных из основной базы.
		if( !vUserSession.SwitchRegistry( vBase.GetDLData().GetVarsRegistry() ) )
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "Corrupted data: Can't switch vars registry." );
	}
	else if( !vUserSession.SwitchRegistry( vBase.GetExtDLData().GetVarsRegistry() ) )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Corrupted data: Can't switch vars registry." );

	// Получение сессии.
	iee = vUserSession.Load( aSessionId, vCache, request_stat );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		MissedMask |= INF_MASK_SESSION;
	else if( iee == INF_ENGINE_ERROR_CHECKSUMM )
	{
		// Удаление неверных данных из кэша.
		if( ( iee = vUserSession.Delete( aSessionId, vCache ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		MissedMask |= INF_MASK_SESSION;
	}
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	else
	{
		// Обработка события SetSession.
		switch( iee = ProcessEvent( InfPattern::Events::Request ) )
		{
		case INF_ENGINE_SUCCESS:
			break;
		case INF_ENGINE_EXCEPTION:
			search_response = false;
			break;
		default:
			ReturnWithTrace( iee );
		}
	}

	if( !MissedMask )
	{
		// Все данные загружены из кэша.

		if( search_response )
		{
			// Обновление сессии.
			for( unsigned int i = 0; i < aVarsList.size(); i++ )
			{
				iee = vUserSession.SetTextValue( aVarsList[i].vName, aVarsList[i].vName ? strlen( aVarsList[i].vName ) : 0, aVarsList[i].vValue, aVarsList[i].vValue ? strlen( aVarsList[i].vValue ) : 0 );
				if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithTrace( iee );
			}

			// Проверка чексуммы пользовательских словарей и сброс состояний словарей в сессии при необходимости.
			unsigned char dicts_check_sum[16];
			vBase.GetUserDictsCheckSum( dicts_check_sum );

			if( !vUserSession.CheckUserDictsCheckSum( dicts_check_sum ) )
			{
				if( INF_ENGINE_SUCCESS != ( iee = vUserSession.GetDictsStates().Drop() ) )
					ReturnWithTrace( iee );
				vUserSession.SetUserDictsCheckSum( dicts_check_sum );
			}

			// Получение ответа инфа.
			iee = GetInfAnswer( aRequest, aRequestLength, request_stat );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			// Суммирование счётчиков.
			request_stat.ex.timer_request_allbases_terms_patterns = request_stat.ex.timer_request_userbase_terms_patterns + request_stat.ex.timer_request_mainbase_terms_patterns;
			request_stat.ex.timer_request_allbases_terms_dicts = request_stat.ex.timer_request_userbase_terms_dicts + request_stat.ex.timer_request_mainbase_terms_dicts;
			request_stat.ex.timer_request_allbases_terms_that_match = request_stat.ex.timer_request_userbase_terms_that_match + request_stat.ex.timer_request_mainbase_terms_that_match;
			request_stat.ex.timer_request_allbases_terms_that_dicts = request_stat.ex.timer_request_userbase_terms_that_dicts + request_stat.ex.timer_request_mainbase_terms_that_dicts;
			request_stat.ex.timer_response_allbases_all = request_stat.ex.timer_response_userbase_all + request_stat.ex.timer_response_mainbase_all;
		}
		else
			TryWithTrace( vUserSession.Update( vAnswer ), INF_ENGINE_SUCCESS );

		// Увеличение счетчика обработанных запросов.
		char * end = nullptr;
		char text[std::numeric_limits<long>::digits10 + 1];
		const char * value = vUserSession.GetValue( Vars::Id { InfEngineVarAutoRequestCounter } ).AsText().GetValue();
		if( value )
			sprintf( text, "%lu", strtol( value, &end, 10 ) + 1 );
		else
			sprintf( text, "1" );
		TryWithTrace( vUserSession.SetTextValue( Vars::Id { InfEngineVarAutoRequestCounter }, text, strlen( text ) ), INF_ENGINE_SUCCESS );
		TryWithTrace( vAnswer.AddInstruct( InfEngineVarAutoRequestCounter, text, strlen( text ), true ), INF_ENGINE_SUCCESS );

		// Сохранение пользовательской сессии.
		if( !vUserSession.GetValue( Vars::Id { InfEngineVarDisableCache } ) ) {
			iee = vUserSession.Save( aSessionId, vCache );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
		}
	}

	// Подсчет необходимой памяти для формирования ответа.
	size_t memory = 4 + 3 * aVarsList.size();
	for( size_t i = 0; i < vAnswer.Size(); i++ )
	{
		switch( vAnswer[i].vType )
		{
		case iAnswer::Item::Type::Text:
			memory += 2;
			break;

		case iAnswer::Item::Type::Inf:
			memory += 3;
			break;

		case iAnswer::Item::Type::Br:
		case iAnswer::Item::Type::StartUList:
		case iAnswer::Item::Type::StartOList:
		case iAnswer::Item::Type::ListItem:
		case iAnswer::Item::Type::EndList:
			memory += 1;
			break;

		case iAnswer::Item::Type::Href:
			memory += 4;
			break;

		case iAnswer::Item::Type::Instruct:
			memory += 3;
			break;

		case iAnswer::Item::Type::OpenWindow:
			memory += 3;
			break;

		case iAnswer::Item::Type::RSS:
			memory += 7;
			break;

		default:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "%s, Invalid answer item type: %d", INF_ENGINE_STRING_ERROR_INVALID_DATA, vAnswer[i].vType );
		}
	}
	memory += vEventInstructions.Size() * 3;

	// Выделение памяти под ответ.
	csdata_data_t * reply = csDataAllocate( memory, aCSDataAllocator );
	if( !reply )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	reply->body_ptr = nullptr;


	// Заполнение данных.
	IEP_SetUInt32( reply, 2, "misseddatamask", MissedMask );

	int pos = 3;
	IEP_SetUInt32( reply, pos, "items", ( int )vAnswer.Size() + aVarsList.size() + vEventInstructions.Size() );
	pos++;

	for( unsigned int i = 0; i < aVarsList.size(); i++ )
	{
		IEP_SetInt( reply, pos, "cmd", InfPatternItems::itInstruct );
		pos++;
		IEP_SetString( reply, pos, "varname", aVarsList[i].vName );
		pos++;
		IEP_SetString( reply, pos, "varvalue", aVarsList[i].vValue );
		pos++;
	}

	for( unsigned int i = 0; i < vAnswer.Size(); i++ )
	{
		switch( vAnswer[i].vType )
		{
		case iAnswer::Item::Type::Text:
			IEP_SetInt( reply, pos, "cmd", InfPatternItems::itText );
			pos++;
			IEP_SetString( reply, pos, "textstring", vAnswer[i].vItem.vText.vValue );
			pos++;
			break;

		case iAnswer::Item::Type::Inf:
			IEP_SetInt( reply, pos, "cmd", InfPatternItems::itInf );
			pos++;
			IEP_SetString( reply, pos, "value", vAnswer[i].vItem.vInf.vValue );
			pos++;
			IEP_SetString( reply, pos, "request", vAnswer[i].vItem.vInf.vRequest );
			pos++;
			break;

		case iAnswer::Item::Type::Br:
			IEP_SetInt( reply, pos, "cmd", InfPatternItems::itBr );
			pos++;
			break;

		case iAnswer::Item::Type::StartUList:
			LogDebug( "IES: StartUList: %u", pos );
			IEP_SetInt( reply, pos++, "cmd", InfPatternItems::itStartUList );
			LogDebug( "IES: StartUList: %u", pos );
			break;

		case iAnswer::Item::Type::StartOList:
			LogDebug( "IES: StartOList: %u", pos );
			IEP_SetInt( reply, pos++, "cmd", InfPatternItems::itStartOList );
			LogDebug( "IES: StartOList: %u", pos );
			break;

		case iAnswer::Item::Type::ListItem:
			LogDebug( "IES: ListItem: %u", pos );
			IEP_SetInt( reply, pos++, "cmd", InfPatternItems::itListItem );
			LogDebug( "IES: ListItem: %u", pos );
			break;

		case iAnswer::Item::Type::EndList:
			LogDebug( "IES: EndList: %u", pos );
			IEP_SetInt( reply, pos++, "cmd", InfPatternItems::itEndList );
			LogDebug( "IES: EndList: %u", pos );
			break;

		case iAnswer::Item::Type::Href:
			IEP_SetInt( reply, pos, "cmd", InfPatternItems::itHref );
			pos++;
			IEP_SetString( reply, pos, "url", vAnswer[i].vItem.vHref.vURL );
			pos++;
			IEP_SetString( reply, pos, "target", vAnswer[i].vItem.vHref.vTarget );
			pos++;
			IEP_SetString( reply, pos, "link", vAnswer[i].vItem.vHref.vLink );
			pos++;
			break;

		case iAnswer::Item::Type::Instruct:
			IEP_SetInt( reply, pos, "cmd", InfPatternItems::itInstruct );
			pos++;
			IEP_SetString( reply, pos, "varname", vBase.GetDLData().GetVarsRegistry().GetVarNameById( vAnswer[i].vItem.vInstruct.vVarId ) );
			pos++;
			IEP_SetString( reply, pos, "varvalue", vAnswer[i].vItem.vInstruct.vValue );
			pos++;
			break;

		case iAnswer::Item::Type::OpenWindow:
			IEP_SetInt( reply, pos++, "cmd", InfPatternItems::itOpenWindow );
			IEP_SetString( reply, pos++, "url", vAnswer[i].vItem.vOpenWindow.vURL );
			IEP_SetUInt32( reply, pos++, "target", vAnswer[i].vItem.vOpenWindow.vTarget );
			break;

		case iAnswer::Item::Type::RSS:
			IEP_SetInt( reply, pos++, "cmd", InfPatternItems::itTagRSS );
			IEP_SetString( reply, pos++, "url", vAnswer[i].vItem.vRSS.vURL );
			IEP_SetString( reply, pos++, "alt", vAnswer[i].vItem.vRSS.vAlt );
			IEP_SetUInt32( reply, pos++, "offset", vAnswer[i].vItem.vRSS.vOffset );
			IEP_SetUInt32( reply, pos++, "show_title", vAnswer[i].vItem.vRSS.vShowTitle );
			IEP_SetUInt32( reply, pos++, "show_link", vAnswer[i].vItem.vRSS.vShowLink );
			IEP_SetUInt32( reply, pos++, "update_period", vAnswer[i].vItem.vRSS.vUpdatePeriod );
			break;


		default:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "%s Incorrect result item's type: %d", INF_ENGINE_STRING_ERROR_INVALID_DATA, vAnswer[i].vType );
		}
	}
	for( unsigned int i = 0; i < vEventInstructions.Size(); i++ )
	{
		IEP_SetInt( reply, pos, "cmd", InfPatternItems::itInstruct );
		pos++;
		IEP_SetString( reply, pos, "varname", vBase.GetDLData().GetVarsRegistry().GetVarNameById( vEventInstructions[i].vItem.vInstruct.vVarId ) );
		pos++;
		IEP_SetString( reply, pos, "varvalue", vEventInstructions[i].vItem.vInstruct.vValue );
		pos++;
	}

	*aData = reply;

	// Время выполнения запроса.
	request_stat.vRequestTm = request_ts.Interval();
	request_stat.ex.timer_global_iteration = request_ts.Interval();

	// Логирование статистики по процессу обработки запроса.
	//	LogDebug( "REQUEST_STAT:      Dicts: %ius      Qst: %i %i %i %ius      pwtQst: %i %i %i %ius      TermsTm: %ius %ius %ius %ius %ius    %ius      AllTm: %ius %ius %ius      Answ: %i   %ius %ius %ius      Cache: %ius %ius      Request: %ius",
	//			  request_stat.vDictTm, request_stat.vQst, request_stat.vQstDeep, request_stat.vQstAcc, request_stat.vQstTime,
	//			  request_stat.vPWTQst, request_stat.vPWTQstDeep, request_stat.vPWTQstAcc, request_stat.vPWTQstTime,
	//			  request_stat.vTermsCheckUserTm, request_stat.vTermsCheckBaseTm, request_stat.vTermsCheckDictTm,
	//			  request_stat.vTermsCheckThatTm, request_stat.vTermsCheckDictThatTm, request_stat.vTermsCheckTime,
	//			  request_stat.vUserTime, request_stat.vBaseTime, request_stat.vTime,
	//			  request_stat.vAnsAttempt, request_stat.vAnsUserTm, request_stat.vAnsBaseTm, request_stat.vAnsTm,
	//			  request_stat.vCacheLoadSessionTm, request_stat.vCacheLoadUserTm,
	//			  request_stat.vRequestTm );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfEngineServer::CmdPurgeInf( unsigned int aInfId )
{
	// Создание ключа.
	char CacheKey[CacheKeyLength];
	size_t CacheKeyLength;
	CreateKey( aInfId, CacheKey, CacheKeyLength );

	// Удаление из кэша.
	if( vCache.DeleteData( CacheKey, CacheKeyLength ) != NanoLib::Cache::rcSuccess )
		ReturnWithTrace( INF_ENGINE_ERROR_CACHE );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfEngineServer::CmdSetInf( unsigned int aInfId, const char * aTemplates, avector<Vars::Raw> &aInfProfile,
		const char ** aDictsNames, const char ** aDicts, unsigned int aDictsNumber )
{
	InfEngineErrors iee;
	RequestStat request_stat;

	// Подготовка профиля инфа.
	vInfProfileWR.Reset();

	// Счётчик для замера времени компиляции инфа.
	unsigned int compile_tm = 0;
	TimeStamp compile_ts;
	// Компиляция профиля инфа с флагом игнорирования ошибок компиляции.
	iee = vInfProfileWR.Create( aTemplates, aTemplates ? strlen( aTemplates ) : 0,
			aInfProfile, aDictsNames, aDicts, aDictsNumber, true );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	// Время компиляции инфа.
	compile_tm = compile_ts.Interval();
	request_stat.ex.timer_compilation_userbase = compile_ts.Interval();

	// Счётчик для замера времени сохранения инфа в кэш.
	unsigned int cache_tm = 0;
	TimeStamp save_ts;
	// Сохранение профиля инфа в кэш.
	iee = vInfProfileWR.Save( aInfId, vCache, cache_tm );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	// Время сохранения инфа.
	request_stat.ex.timer_cache_infdata_save = save_ts.Interval();

	//	LogDebug( "REQUEST_STAT: compile: %ius   save: %ius   cache: %ius",
	//			  request_stat.ex.timer_compilation_userbase,
	//			  request_stat.ex.timer_cache_infdata_save,
	//			  cache_tm );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfEngineServer::LoadUserBase( unsigned int aInfId, RequestStat & aRequestStat, bool & aSuccess )
{
	aSuccess = true;

	// Подготовка к загрузке кастомизирующей базы.
	if( vUserBaseBuffer )
		free( vUserBaseBuffer );
	vUserBaseBuffer = nullptr;

	auto iee = vBase.CloseExtBase();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Создание ключа.
	char CacheKey[CacheKeyLength];
	size_t CacheKeyLength;
	CreateKey( aInfId, CacheKey, CacheKeyLength );

	// Загрузка данных кастомизирующей базы из кэша.
	unsigned int buffersize;
	TimeStamp cache_tm;
	NanoLib::Cache::ReturnCode nce = vCache.GetData( CacheKey, static_cast<size_t>( CacheKeyLength ), vUserBaseBuffer, buffersize );
	// Время получения из кэша пользовательской базы.
	aRequestStat.vCacheLoadUserTm = cache_tm.Interval();
	aRequestStat.ex.timer_cache_infdata_load = cache_tm.Interval();
	if( nce == NanoLib::Cache::rcUnsuccess )
		aSuccess = false;
	else if( nce != NanoLib::Cache::rcSuccess )
		ReturnWithTrace( INF_ENGINE_WARN_NANOLIB );
	else
	{
		// Открываем кастомизирующую пользовательскую базу.
		if( ( iee = vBase.OpenExtBase( vUserBaseBuffer, buffersize, vJoiningMethod ) ) == INF_ENGINE_ERROR_CHECKSUMM )
		{
			// Удалаяем инфа из кэша.
			nce = vCache.DeleteData( CacheKey, static_cast<size_t>( CacheKeyLength ) );
			if( nce == NanoLib::Cache::rcErrorNoFreeMemory )
				ReturnWithTrace( INF_ENGINE_ERROR_NOFREE_MEMORY );
			else if( nce != NanoLib::Cache::rcSuccess )
				ReturnWithTrace( INF_ENGINE_ERROR_FAULT );

			// Устанавливаем флаг отсутствия инфа в кэше.
			aSuccess = false;
		}
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfEngineServer::ProcessEvent( InfPattern::Events aEvent )
{
	// Очистка списка инструкций.
	vEventInstructions.Reset();

	TagFunctionsCache TagFunctionsCache;
	TLCSCache TLCSCache;
	BFinder::DictsMatchingResults DictsMatchingResults;

	// Получение счетчика реплик.
	auto first_request = vUserSession.GetValue( Vars::Id { InfEngineVarAutoRequestCounter } ).IsNull();

	// Получение списка сработавших ранее событий.
	aTextString event_ids;
	if( event_ids.assign( vUserSession.GetValue( InfEngineVarAutoEventIds ).AsText().GetValue() ) != nlrcSuccess )
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	// Сброс списка, если последним событием был request.
	if( event_ids.size() > 1 && event_ids[event_ids.size() - 1] == '!' )
	{
		event_ids.clear();
	}

	// Загрузка данных инфа, для события SetSession или UpdateSession.
	if( aEvent == InfPattern::Events::SetSession || aEvent == InfPattern::Events::UpdateSession )
	{
		RequestStat request_stat;
		bool success { true };
		auto iee = LoadUserBase( vUserSession.GetInfId(), request_stat, success );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	static MatchedPattern mp{};

	unsigned int last_pattern_id = static_cast<unsigned int>(-1);
	bool last_conditions_ok = true;

	mp.vConditionsRegistry = &vBase.GetDLData().GetConditionsRegistry();
	mp.vDictsData = &vBase.GetDLData();
	mp.vFunctionsData = &vBase.GetDLData();
	mp.vPatternsData = &vBase.GetDLData();
	mp.vSession = &vUserSession;
	mp.vUserRequest = "";
	mp.vUserRequestLength = 0;
	mp.vTagFunctionsCache = &TagFunctionsCache;
	mp.vTLCSCache = &TLCSCache;
	mp.vNormalizedRequest = "";
	mp.vNormalizedRequestLength = 0;
	mp.vRequestMap = nullptr;
	mp.vDictsMatchingResults = &DictsMatchingResults;

	aTextString Answer;

	for( unsigned int i = 0; i < vBase.GetDLData().GetEventPatternsNumber( aEvent ) && Answer.empty(); i++ )
	{
		unsigned int pid, aid;
		auto ans = vBase.GetDLData().GetEventAnswer( aEvent, i, &pid, &aid );
		if( ans.IsFirst() && !first_request )
			continue;

		// Получение шаблона, связанного с событием.
		unsigned int pattern_id = vBase.GetDLData().GetEventPatternId( aEvent, i );
		if(  static_cast<unsigned int>(-1) == pattern_id )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

		// Проверка условий шаблона.
		if( pattern_id != last_pattern_id )
		{
			last_conditions_ok = true;
			InfPatternRO pattern;
			TryWithTrace( vBase.GetDLData().GetEventPattern( aEvent, i, pattern ), INF_ENGINE_SUCCESS );
			for( unsigned int cond_n = 0; cond_n < pattern.GetConditionsNum(); ++cond_n )
			{
				if( !pattern.GetCondition( cond_n ).Check( vBase.GetDLData().GetConditionsRegistry(), vUserSession ) )
				{
					last_conditions_ok = false;
					break;
				}
			}
			last_pattern_id = pattern_id;
		}

		// Если усовия шаблона не выполняются, пропускаем данный шаблон.
		if( !last_conditions_ok )
			continue;

		InfEngineErrors iee = mp.CreateAnswer( ans, vAnswer, vUserSession );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			continue;
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		vAnswer.DisableAutoVars( true );

		// Добавляем информацию о сработавшем шаблоне.
		InfPatternRO pattern;
		if( event_ids.append( "(" ) != nlrcSuccess ||
				event_ids.append( vBase.GetDLData().GetPattern( pid, pattern ).GetId() ) != nlrcSuccess ||
				event_ids.append( "," ) != nlrcSuccess ||
				event_ids.append( aid ) != nlrcSuccess ||
				event_ids.append( ")" ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		if( aEvent == InfPattern::Events::Request )
		{
			TryWithTrace( vAnswer.ToString( Answer ), INF_ENGINE_SUCCESS );
			if( !Answer.empty() )
			{
				TryWithTrace( vAnswer.AddInstruct( InfEngineVarAutoResponseType, "response by request event", strlen( "response by request event" ), false ), INF_ENGINE_SUCCESS );

				if( event_ids.append( "!" ) != nlrcSuccess )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				// Добавление инструкции на изменение переменной auto_reply_id.
				TryWithTrace( vEventInstructions.AddInstruct( InfEngineVarAutoReplyId, pattern.GetId(), strlen( pattern.GetId() ), true ), INF_ENGINE_SUCCESS );

				// Добавление инструкции на изменение переменной auto_reply_label.
				TryWithTrace( vEventInstructions.AddInstruct( InfEngineVarAutoReplyLabel, pattern.GetInstrLabel(), strlen( pattern.GetInstrLabel() ), true ), INF_ENGINE_SUCCESS );
			}
		}

		vUserSession.Update( vAnswer );

		// Сохраняем все инструкции по изменению сессии.
		for( unsigned int i = 0; i < vAnswer.Size(); i++ )
		{
			if( vAnswer[i].vType == iAnswer::Item::Type::Instruct )
				TryWithTrace( vEventInstructions.AddInstruct( vAnswer[i].vItem.vInstruct.vVarId, vAnswer[i].vItem.vInstruct.vValue, vAnswer[i].vItem.vInstruct.vValueLength, true ), INF_ENGINE_SUCCESS );
		}
	}

	// Добавление отметки, что последним событием был request.
	if( aEvent == InfPattern::Events::Request && !event_ids.empty() )
		if( event_ids.append( "!" ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Добавление инструкции на изменение переменной event_ids.
	TryWithTrace( vEventInstructions.AddInstruct( InfEngineVarAutoEventIds, event_ids.ToConstChar(), event_ids.size(), true ), INF_ENGINE_SUCCESS );

	return Answer.empty() ? INF_ENGINE_SUCCESS : INF_ENGINE_EXCEPTION;
}

InfEngineErrors InfEngineServer::GetInfAnswer( const char * aUserRequest, unsigned int aUserRequestLength, RequestStat & aRequestStat )
{
	// Проверка аргументов.
	if( !aUserRequest && aUserRequestLength != 0 )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Получение текущего времени.
	const time_t CurrentTime = time( nullptr );
	if( CurrentTime == static_cast<time_t>( -1 ) )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't get current time: %s", strerror( errno ) );

	tm* CurrentTimeStruct = localtime( const_cast<time_t*>( &CurrentTime ) );
	if( !CurrentTimeStruct )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't get current time." );

	// Представление времени в текстовом виде.
	char AutoTimeBuffer[256];
	size_t AutoTimeBufferLength = strftime( AutoTimeBuffer, 256, "%H:%M", CurrentTimeStruct );

	// Преставление даты в текстовом виде.
	char AutoDateBuffer[256];
	size_t AutoDateBufferLength = strftime( AutoDateBuffer, 100, "%Y-%m-%d", CurrentTimeStruct );

	// Добавление в сессию значений текущей даты и текущего времени.
	InfEngineErrors iee = vUserSession.SetTextValue( InfEngineVarAutoTime, AutoTimeBuffer, AutoTimeBufferLength );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = vUserSession.SetTextValue( InfEngineVarAutoDate, AutoDateBuffer, AutoDateBufferLength );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Корректировка счетчика итераций в диалоге.
	unsigned int ResponseCounter = 0;
	const char * Value = vUserSession.GetValue( InfEngineVarAutoResponseCounter ).AsText().GetValue();
	if( Value )
	{
		char * End = nullptr;
		ResponseCounter = strtoul( Value, &End, 10 );
		if( End && *End != '\0' )
		{
			// В случае некорректного числа, сичтаем реплику первой.
			ResponseCounter = 0;
		}
	}
	ResponseCounter++;
	char TmpBuffer[sizeof( unsigned int ) * 3];
	bzero( TmpBuffer, sizeof( unsigned int ) * 3 );
	sprintf( TmpBuffer, "%u", ResponseCounter );
	iee = vUserSession.SetTextValue( InfEngineVarAutoResponseCounter, TmpBuffer, strlen( TmpBuffer ) );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );


	// Вычисление необходимости и возможности раскрытия эллипсиса.

	// Флаг, показывающий требуется ли раскрытие эллипсиса.
	bool DiscloseEllipsisFlag = false;
	// Флаг, показывающий нужно ли отображать раскрытый эллипсис в ответе инфа.
	bool ShowEllipsisFlag = false;


	// Определение алгоритма реконструкции реплики пользователя.
	unsigned int InfEllipsisModeLength = 0;
	const char * InfEllipsisMode = vBase.GetVarsValues().GetValue( InfEngineVarInfEllipsisMode ).AsText().GetValue( InfEllipsisModeLength );

	if( vEllipsisMode != EllipsisMode::Off )
	{
		if( vEllipsisDefault == EllipsisDefault::Off )
		{
			if( ( InfEllipsisModeLength == 1 && !strcasecmp( "1", InfEllipsisMode ) ) ||
					( InfEllipsisModeLength == 2 && !strcasecmp( "on", InfEllipsisMode ) ) )
			{
				// Реконструкция реплики производится, но результат не добавляется к ответу инфа.
				DiscloseEllipsisFlag = true;
				ShowEllipsisFlag = false;
			}
			else if( ( InfEllipsisModeLength == 1 && !strcmp( "2", InfEllipsisMode ) ) ||
					( InfEllipsisModeLength == 8 && !strcmp( "on debug", InfEllipsisMode ) ) )
			{
				// Реконструкция реплики производится и ее результат добавляется к ответу инфа.
				DiscloseEllipsisFlag = true;
				ShowEllipsisFlag = true;
			}
			else
			{
				// Реконструкция реплики не производится.
				DiscloseEllipsisFlag = false;
				ShowEllipsisFlag = false;
			}
		}
		else if( vEllipsisDefault == EllipsisDefault::On )
		{
			if( ( InfEllipsisModeLength == 1 && !strcasecmp( "3", InfEllipsisMode ) ) ||
					( InfEllipsisModeLength == 3 && !strcasecmp( "off", InfEllipsisMode ) ) )
			{
				// Реконструкция реплики не производится.
				DiscloseEllipsisFlag = false;
				ShowEllipsisFlag = false;
			}
			else if( ( InfEllipsisModeLength == 1 && !strcmp( "2", InfEllipsisMode ) ) ||
					( InfEllipsisModeLength == 8 && !strcmp( "on debug", InfEllipsisMode ) ) )
			{
				// Реконструкция реплики производится и ее результат добавляется к ответу инфа.
				DiscloseEllipsisFlag = true;
				ShowEllipsisFlag = true;
			}
			else
			{
				// Реконструкция реплики производится, но ее результат не добавляется к ответу инфа.
				DiscloseEllipsisFlag = true;
				ShowEllipsisFlag = false;
			}
		}
		else
		{
			// Реконструкция реплики производится, но ее результат не добавляется к ответу инфа.
			DiscloseEllipsisFlag = true;
			ShowEllipsisFlag = false;
		}
	}
	else
	{
		// Реконструкция реплики не производится.
		DiscloseEllipsisFlag = false;
		ShowEllipsisFlag = false;
	}

	// Раскрытие эллипсиса.
	char * DisclosedEllipsis = nullptr;
	unsigned int DisclosedEllipsisLength = 0;
	char * EllipsisId = nullptr;
	unsigned int EllipsisIdLength = 0;
	char * EllipsisDetected = nullptr;
	unsigned int EllipsisDetectedLength = 0;
	if( DiscloseEllipsisFlag )
	{
		// Получение предыдущих реплики пользователя и ответа инфа.
		unsigned int UserPrevRequestLength;
		const char * UserPrevRequest = vUserSession.GetValue( InfEngineVarUserPrevRequest ).AsText().GetValue( UserPrevRequestLength );
		unsigned int InfPrevResponseLength;
		const char * InfPrevResponse = vUserSession.GetValue( InfEngineVarInfPrevResponse ).AsText().GetValue( InfPrevResponseLength );

		unsigned int ReqestWordCnt1 = 0;
		unsigned int ReqestWordCnt2 = 0;
		unsigned int ReqestWordCnt3 = 0;

		// Проверка количества слов в запросах.
		if( INF_ENGINE_SUCCESS != ( iee = CountRequestLength( aUserRequest, aUserRequestLength, ReqestWordCnt1 ) ) )
			ReturnWithTrace( iee );
		if( INF_ENGINE_SUCCESS != ( iee = CountRequestLength( InfPrevResponse, InfPrevResponseLength, ReqestWordCnt2 ) ) )
			ReturnWithTrace( iee );
		if( INF_ENGINE_SUCCESS != ( iee = CountRequestLength( UserPrevRequest, UserPrevRequestLength, ReqestWordCnt3 ) ) )
			ReturnWithTrace( iee );
		if( !vMaxEllRequestLength || ( ReqestWordCnt1 <= vMaxEllRequestLength && ReqestWordCnt2 <= vMaxEllRequestLength && ReqestWordCnt3 <= vMaxEllRequestLength ) )
		{
			// Раскрытие эллипсиса только на одной реплике пользователя не бывает.
			if( UserPrevRequest && UserPrevRequestLength != 0 && InfPrevResponse && InfPrevResponseLength != 0 &&
					!DetectEvent( UserPrevRequest, UserPrevRequestLength ) && !DetectEvent( aUserRequest, aUserRequestLength ) )
			{
				// Раскрытие эллипсиса.
				InfEngineErrors iee = vEllipsisDiscloseFunction( UserPrevRequest, UserPrevRequestLength,
						InfPrevResponse, InfPrevResponseLength,
						aUserRequest, aUserRequestLength,
						DisclosedEllipsis, DisclosedEllipsisLength,
						EllipsisId, EllipsisIdLength,
						EllipsisDetected, EllipsisDetectedLength );
				if( iee == INF_ENGINE_WARN_UNSUCCESS )
				{
					// Раскрыть эллипсис не удалось.
				}
				else if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );
			}
		}
	}

	bool UserAnswerFlag = false;

	// Поиск ответа инфа.
	if( DisclosedEllipsis && DisclosedEllipsisLength != 0 )
	{
		// Эллипсис был раскрыт.

		if( vEllipsisMode == EllipsisMode::Force )
		{
			// Производится поиск только раскрытого эллипсиса.

			unsigned int AnswerWeight = 0;
			bool UserAnswer = false;

			// Подсчет длины запроса в словах.
			iee = CountRequestLength( DisclosedEllipsis, DisclosedEllipsisLength, vUserSession );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			iee = Answer( vUserSession, DisclosedEllipsis, DisclosedEllipsisLength, vAnswer, AnswerWeight, UserAnswer, aRequestStat );
			if( iee != INF_ENGINE_SUCCESS )
			{
				free( DisclosedEllipsis );
				if( EllipsisId )
					free( EllipsisId );
				if( EllipsisDetected )
					free( EllipsisDetected );

				ReturnWithTrace( iee );
			}

			// Добавление автоматической информации.
			if( UserAnswer )
			{
				if( vAnswer.IsAutoVarsEnabled() )
					iee = vAnswer.AddInstruct( InfEngineVarAutoResponseType, "ellipsis request from custom base", strlen( "ellipsis request from custom base" ), false );
			}
			else
			{
				if( vAnswer.IsAutoVarsEnabled() )
					iee = vAnswer.AddInstruct( InfEngineVarAutoResponseType, "ellipsis request from main base", strlen( "ellipsis request from main base" ), false );
			}
			if( iee != INF_ENGINE_SUCCESS )
			{
				free( DisclosedEllipsis );
				if( EllipsisId )
					free( EllipsisId );
				if( EllipsisDetected )
					free( EllipsisDetected );

				switch( iee )
				{
				case INF_ENGINE_ERROR_NOFREE_MEMORY:
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				default:
					ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
				}
			}
		}
		else
		{
			// Производится поиск по раскрытому эллипсису и по запросу пользователя, затем выбирается лучший вариант.

			// Запрос ответа инфа на раскрытый эллипсис.
			unsigned int EllipsisAnswerWeight = 0;
			bool EllipsisUserAnswerFlag = false;

			// Подсчет длины запроса в словах.
			iee = CountRequestLength( DisclosedEllipsis, DisclosedEllipsisLength, vUserSession );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			iee = Answer( vUserSession, DisclosedEllipsis, DisclosedEllipsisLength, vEllipsisAnswer, EllipsisAnswerWeight, EllipsisUserAnswerFlag, aRequestStat );
			if( iee != INF_ENGINE_SUCCESS )
			{
				free( DisclosedEllipsis );
				if( EllipsisId )
					free( EllipsisId );
				if( EllipsisDetected )
					free( EllipsisDetected );

				ReturnWithTrace( iee );
			}

			// Запрос ответа инфа на реплику пользователя.
			unsigned int RequestAnswerWeight = 0;

			// Подсчет длины запроса в словах.
			iee = CountRequestLength( aUserRequest, aUserRequestLength, vUserSession );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			iee = Answer( vUserSession, aUserRequest, aUserRequestLength, vAnswer, RequestAnswerWeight, UserAnswerFlag, aRequestStat );
			if( iee != INF_ENGINE_SUCCESS )
			{
				free( DisclosedEllipsis );
				if( EllipsisId )
					free( EllipsisId );
				if( EllipsisDetected )
					free( EllipsisDetected );

				ReturnWithTrace( iee );
			}

			// Сравнение результатов.
			if( ( EllipsisUserAnswerFlag && !UserAnswerFlag ) ||
					( EllipsisUserAnswerFlag == UserAnswerFlag && EllipsisAnswerWeight > RequestAnswerWeight ) )
			{
				// Поиск по раскрытому эллипсису был удачнее.
				if( ( vAnswer = vEllipsisAnswer ) != INF_ENGINE_SUCCESS )
				{
					free( DisclosedEllipsis );
					if( EllipsisId )
						free( EllipsisId );
					if( EllipsisDetected )
						free( EllipsisDetected );

					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}

				UserAnswerFlag = EllipsisUserAnswerFlag;

				// Добавление автоматической информации.
				if( EllipsisUserAnswerFlag )
				{
					if( vAnswer.IsAutoVarsEnabled() )
						iee = vAnswer.AddInstruct( InfEngineVarAutoResponseType, "ellipsis request from custom base", strlen( "ellipsis request from custom base" ), false );
				}
				else
				{
					if( vAnswer.IsAutoVarsEnabled() )
						iee = vAnswer.AddInstruct( InfEngineVarAutoResponseType, "ellipsis request from main base", strlen( "ellipsis request from main base" ), false );
				}
				if( iee != INF_ENGINE_SUCCESS )
				{
					free( DisclosedEllipsis );
					if( EllipsisId )
						free( EllipsisId );
					if( EllipsisDetected )
						free( EllipsisDetected );
					switch( iee )
					{
					case INF_ENGINE_SUCCESS:
						break;
					case INF_ENGINE_ERROR_NOFREE_MEMORY:
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					default:
						ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
					}
				}
			}
			else
			{
				// Поиск по раскрытому эллипсису был мене удачен.

				// Добавление автоматической информации.
				if( UserAnswerFlag )
				{
					if( vAnswer.IsAutoVarsEnabled() )
						iee = vAnswer.AddInstruct( InfEngineVarAutoResponseType, "user request from custom base", strlen( "user request from custom base" ), false );
				}
				else
				{
					if( vAnswer.IsAutoVarsEnabled() )
						iee = vAnswer.AddInstruct( InfEngineVarAutoResponseType, "user request from main base", strlen( "user request from main base" ), false );
				}
				if( iee != INF_ENGINE_SUCCESS )
				{
					free( DisclosedEllipsis );
					if( EllipsisId )
						free( EllipsisId );
					if( EllipsisDetected )
						free( EllipsisDetected );

					switch( iee )
					{
					case INF_ENGINE_SUCCESS:
						break;
					case INF_ENGINE_ERROR_NOFREE_MEMORY:
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					default:
						ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
					}
				}
			}
		}
	}
	else
	{
		// Производится поиск только реплике пользователя.

		unsigned int AnswerWeight;

		// Подсчет длины запроса в словах.
		iee = CountRequestLength( aUserRequest, aUserRequestLength, vUserSession );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		iee = Answer( vUserSession, aUserRequest, aUserRequestLength, vAnswer, AnswerWeight, UserAnswerFlag, aRequestStat );
		if( iee != INF_ENGINE_SUCCESS )
		{
			if( DisclosedEllipsis )
				free( DisclosedEllipsis );
			if( EllipsisId )
				free( EllipsisId );
			if( EllipsisDetected )
				free( EllipsisDetected );

			ReturnWithTrace( iee );
		}

		// Добавление автоматической информации.
		if( UserAnswerFlag )
		{
			if( vAnswer.IsAutoVarsEnabled() )
				iee = vAnswer.AddInstruct( InfEngineVarAutoResponseType, "user request from custom base", strlen( "user request from custom base" ), false );
		}
		else
		{
			if( vAnswer.IsAutoVarsEnabled() )
				iee = vAnswer.AddInstruct( InfEngineVarAutoResponseType, "user request from main base", strlen( "user request from main base" ), false );
		}
		if( iee != INF_ENGINE_SUCCESS )
		{
			if( DisclosedEllipsis )
				free( DisclosedEllipsis );
			if( EllipsisId )
				free( EllipsisId );
			if( EllipsisDetected )
				free( EllipsisDetected );

			switch( iee )
			{
			case INF_ENGINE_SUCCESS:
				break;
			case INF_ENGINE_ERROR_NOFREE_MEMORY:
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			default:
				ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}

		iee = vUserSession.SetTextValue( InfEngineVarAutoEllipsisDisclosed, "", 0 );
		if( iee != INF_ENGINE_SUCCESS )
		{
			if( DisclosedEllipsis )
				free( DisclosedEllipsis );
			if( EllipsisId )
				free( EllipsisId );
			if( EllipsisDetected )
				free( EllipsisDetected );

			ReturnWithTrace( iee );
		}
	}

	// Добавление визуального отражения раскрытого эллипсиса.
	if( ShowEllipsisFlag && DisclosedEllipsis && DisclosedEllipsisLength )
	{
		if( ( iee = vAnswer.AddBr() ) != INF_ENGINE_SUCCESS ||
				( iee = vAnswer.AddBr() ) != INF_ENGINE_SUCCESS ||
				( iee = vAnswer.AddNullTermText( "You said: ", false ) ) != INF_ENGINE_SUCCESS ||
				( iee = vAnswer.AddText( DisclosedEllipsis, DisclosedEllipsisLength, true ) ) != INF_ENGINE_SUCCESS )
		{
			free( DisclosedEllipsis );
			if( EllipsisId )
				free( EllipsisId );
			if( EllipsisDetected )
				free( EllipsisDetected );

			switch( iee )
			{
			case INF_ENGINE_ERROR_NOFREE_MEMORY:
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			default:
				ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}
	}

	// Добавление информации о эллипсисах.
	if( vAnswer.IsAutoVarsEnabled() )
	{
		if( ( iee = AddInstruct( InfEngineVarAutoEllipsisDisclosed, DisclosedEllipsis, DisclosedEllipsisLength ) ) != INF_ENGINE_SUCCESS ||
				( iee = AddInstruct( InfEngineVarAutoEllipsisId, EllipsisId, EllipsisIdLength ) ) != INF_ENGINE_SUCCESS ||
				( iee = AddInstruct( InfEngineVarAutoEllipsisDetected, EllipsisDetected, EllipsisDetectedLength ) ) != INF_ENGINE_SUCCESS )
		{
			if( DisclosedEllipsis )
				free( DisclosedEllipsis );
			if( EllipsisId )
				free( EllipsisId );
			if( EllipsisDetected )
				free( EllipsisDetected );

			switch( iee )
			{
			case INF_ENGINE_ERROR_NOFREE_MEMORY:
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
						INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			default:
				ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}
	}

	// Исполнение инструкций по преобразованию переменных сессии.
	if( ( iee = vUserSession.Update( vAnswer ) ) != INF_ENGINE_SUCCESS )
	{
		if( DisclosedEllipsis )
		{
			free( DisclosedEllipsis );
			free( EllipsisId );
		}
		if( EllipsisDetected )
			free( EllipsisDetected );

		ReturnWithTrace( iee );
	}

	// Очистка памяти.
	if( DisclosedEllipsis )
		free( DisclosedEllipsis );
	if( EllipsisId )
		free( EllipsisId );
	if( EllipsisDetected )
		free( EllipsisDetected );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfEngineServer::AddInstruct( Vars::Id aVarId, const char * aValue, unsigned int aValueLength )
{
	// Проверка аргументов.
	if( !aValue && aValueLength != 0 )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	if( aValue )
	{
		// Значение не пусто. Добавляем инструкцию.
		InfEngineErrors iee = vAnswer.AddInstruct( aVarId, aValue, aValueLength, true );
		if( iee != INF_ENGINE_SUCCESS )
		{
			switch( iee )
			{
			case INF_ENGINE_ERROR_NOFREE_MEMORY:
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
						INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			default:
				ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}
	}
	else if( vUserSession.GetValue( aVarId ).AsText().GetValue() )
	{
		// Значение пусто, но нужно удалить ранее установленное значение.
		InfEngineErrors iee = vAnswer.AddInstruct( aVarId, aValue, aValueLength, true );
		if( iee != INF_ENGINE_SUCCESS )
		{
			switch( iee )
			{
			case INF_ENGINE_ERROR_NOFREE_MEMORY:
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
						INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			default:
				ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}
	}
	else
	{
		// Значение пусто и в сессии оно тоже пусто, поэтому ничего менять не нужно.
	}

	return INF_ENGINE_SUCCESS;
}

bool InfEngineServer::DetectEvent( const char * aString, unsigned int aStringLength ) const
{
	if( aStringLength == ( unsigned int ) -1 )
		aStringLength = strlen( aString );

	if( aStringLength < 42 )
		return false;

	if( strncasecmp( aString, "EVENT ", 6 ) )
		return false;

	const unsigned int pos[] = { 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 20, 21, 22, 23, 25, 26, 27, 28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };
	for( unsigned int i = 0; i < sizeof( pos ) / sizeof( unsigned int ); i++ )
	{
		if( ( '0' <= aString[pos[i]] && aString[pos[i]] <= '9' ) ||
				( 'a' < aString[pos[i]] && aString[pos[i]] <= 'f' ) ||
				( 'A' < aString[pos[i]] && aString[pos[i]] <= 'F' ) )
			continue;
		else
			return false;
	}

	if( aString[14] != '-' || aString[19] != '-' || aString[24] != '-' || aString[29] != '-' )
		return false;

	return true;
}

InfEngineErrors InfEngineServer::CountRequestLength( const char * aUserRequest, unsigned int aUserRequestLength, Session & aSession )
{
	CDocMessage DocMessage;

	int ret = DocMessage.SetMessage( nullptr, 0, aUserRequest, aUserRequestLength, true );
	if( ret != 0 )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );
	DocMessage.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );

	DocImage image;
	InfEngineErrors iee = vBase.GetIndexBase().LemmatizeDoc( DocMessage, image );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Обработка запроса пользователя.
	NDocImage ndoc_image;
	TermsDocImageIterator iter( image, &vBase.GetIndexBase().GetIndexQBase().GetStopDict(), false );
	if( INF_ENGINE_SUCCESS != ( iee = ndoc_image.Init( iter ) ) )
	{
		if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		else
			ReturnWithError( iee, INF_ENGINE_STRING_ERROR_INTERNAL );
	}

	// Вычисление длины запроса в словах.
	char counter[100];
	unsigned int length = sprintf( counter, "%u", ndoc_image.Size() - 2 );

	// Добавление значения в сессию.
	iee = aSession.SetTextValue( InfEngineVarRequestLength, counter, length );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return iee;
}

InfEngineErrors InfEngineServer::CountRequestLength( const char * aUserRequest, unsigned int aUserRequestLength, unsigned int &aWordsCnt )
{
	CDocMessage DocMessage;

	int ret = DocMessage.SetMessage( nullptr, 0, aUserRequest, aUserRequestLength, true );
	if( ret != 0 )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );
	DocMessage.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );

	DocImage image;
	InfEngineErrors iee = vBase.GetIndexBase().LemmatizeDoc( DocMessage, image );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	aWordsCnt = image.Size();

	return INF_ENGINE_SUCCESS;
}

bool InfEngineServer::CheckSignature( const unsigned char aSignature[16] ) const
{
	return vBase.GetDLData().CheckSignature( aSignature );
}
