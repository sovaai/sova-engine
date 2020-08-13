#include <csignal>
#include <getopt.h>
#include <unistd.h>
#include <libgen.h>
#include <cstring>


#include <NanoLib/LogSystem.hpp>
#include <NanoLib/nMemoryAllocator.hpp>
#include <InfEngine2/_Include/ConsoleOutput.hpp>
#include <ClientLib/csDataPack.h>
#include <ClientLib/Protocol.hpp>
#include <NanoLib/ConfigFile.hpp>

#include "InfEngineServer.hpp"


#include <InfEngine2/Build.hpp>

#include <InfEngine2/InfEngine/Signature/SignatureUtils.hpp>

/** Global statistic's counters. */
unsigned int gCounters[INF_ENGINE_COMMANDS_NUMBER+1];
/** InfServer debug mode. ["OFF", "ON", "EXT"]. */
const char * gInfServerDebugMode = nullptr;

unsigned int gProtocolVersion;


#ifdef LOG_DATA_FLOW_ENABLED
/** Flag for full logging of data flow between server and client. */
bool LogDataFlow = false;
#endif  /** LOG_DATA_FLOW_ENABLED */


/** InfEngine server. */
InfEngineServer ies( nullptr, 0 );


/** Memory allocator for communications between server and client. */
nMemoryAllocator csAllocator;


/** Код команды по протоколу передачи данных. */
InfEngineProtocol gCmd;
/** Текст запроса пользователя, в случае команды INF_ENGINE_PROTOCOL_REQUEST. */
const char * gRequest;
/** Идентификатор инфа, в случае команды INF_ENGINE_PROTOCOL_REQUEST. */
unsigned int gInfId;

/** Функции логгирования потока данных. */
void LogCSData( csdata_data_t * aData, const char * aPrefix )
{
	// Логгирование тела сообщения.
	if( aData->body_ptr )
		LogDebug( "%s Body: %s", aPrefix, aData->body_ptr );

	// Логгирование аттрибутов.
	for( size_t i = 0; i < aData->attr_count; i++ )
	{
		if( aData->attr_list[i].attr_type & CSDATA_INT )
			LogDebug( "%s Attr[%d]: %s => %d", aPrefix, i, aData->attr_list[i].attr_name, aData->attr_list[i].attr_value.attr_int );
		else if( aData->attr_list[i].attr_type & CSDATA_STRING )
			LogDebug( "%s Attr[%d]: |%s| => |%s|(%d)", aPrefix, i, aData->attr_list[i].attr_name, aData->attr_list[i].attr_value.attr_string, aData->attr_list[i].attr_value_size );
		else
			LogDebug( "%s Attr[%d]: |%s| => NULL", aPrefix, i, aData->attr_list[i].attr_name );
	}
}

/** Создание сообщения об ошибке для отправки через процесс сервер. */
csdata_data_t* MakeError( InfEngineErrors aError )
{
	// Выделение памяти.
	csdata_data_t * reply = csDataAllocate( 2, csAllocator );
	if( !reply )
		ReturnWithError( nullptr, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Формирование ответа.
	reply->body_ptr = nullptr;
	IEP_SetProtocol( reply, gProtocolVersion );
	IEP_SetStatus( reply, aError );

	// Сигнлазизируем о необходимости завершения исполнения процесса после ответа.
	//    sighup( 0 );

	return csDataPack( reply, csAllocator, CSDATAPACK_FULL );
}

/** Callback функция для реакции на запрос клиента. Dummy mode. */
csdata_data_t * iedummy( csdata_data_t * input )
{
	if( !input || input->attr_count < 2 )
		ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Формирование ответа.
	csdata_data_t * reply = csDataAllocate( 3, csAllocator );
	if( !reply )
		ReturnWithError( MakeError( INF_ENGINE_ERROR_NOFREE_MEMORY ),
						 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	reply->body_ptr = nullptr;
	IEP_SetProtocol( reply, gProtocolVersion );
	IEP_SetStatus( reply, INF_ENGINE_SUCCESS );

	return reply;
}

/** Callback функция для работы в режиме сигнализации об ошибке. */
csdata_data_t * ieerror( csdata_data_t* /*input*/ )
{
	// Формирование ответа.
	csdata_data_t * reply = csdata_alloc_data( 3 );
	if( !reply )
		ReturnWithError( MakeError( INF_ENGINE_ERROR_NOFREE_MEMORY ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	reply->body_ptr = nullptr;
	IEP_SetProtocol( reply, gProtocolVersion );
	IEP_SetStatus( reply, INF_ENGINE_ERROR_FATAL );

	return reply;
}

/**
 *  Callback функция для реакции на запрос клиента.
 * @param input_packed - запакованный запрос.
 */
csdata_data_t * ieserver( csdata_data_t * input_packed )
{
	// Установка версии протокола по умолчанию.
	gProtocolVersion = INF_ENGINE_PROTOCOL_VERSION;

	// Сброс ранее выделенной памяти.
	csAllocator.Reset();

	// Распаковка полученных данных.
	csdata_data_t * input = csDataUnPack( input_packed, csAllocator );
	if( !input )
	{
		if( errno == ENOMEM )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_NOFREE_MEMORY ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		else
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), INF_ENGINE_STRING_ERROR_INVALID_DATA );
	}
	else if( input->attr_count < 2 )
		ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), INF_ENGINE_STRING_ERROR_INVALID_DATA );

#    ifdef LOG_DATA_FLOW_ENABLED

	// Логгирование потока данных.
	if( LogDataFlow )
		LogCSData( input, "Request" );

#    endif  /** LOG_DATA_FLOW_ENABLED */

	/**
	 *  Проверка протокола.
	 */
	int Protocol;
	if( IEP_GetProtocol( input, &Protocol ) != INF_ENGINE_SUCCESS )
	{
		LogCSData( input, "Request" );
		ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_PROTOCOL ), "Invalid request package: can't get protocol version." );
	}

	if( !CheckProtocolVersion( Protocol ) )
	{
		LogCSData( input, "Request" );
		ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_PROTOCOL ), "Unsupported protocol: %d", Protocol );
	}
	gProtocolVersion = Protocol;

	InfEngineProtocol cmd;
	if( IEP_GetCmd( input, &cmd ) != INF_ENGINE_SUCCESS )
	{
		LogCSData( input, "Request" );
		ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get request command." );
	}

	gCmd = cmd;

	csdata_data_t * reply = nullptr;
	if( cmd == INF_ENGINE_PROTOCOL_INIT )
	{
		// Инициализация новой сессии.
		if( input->attr_count != 4 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"init\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Получение идентификатора инфа.
		unsigned int InfId;
		if( IEP_GetInfId( input, 2, &InfId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get infid." );
		}

		// Получение идентификатора сессии.
		unsigned int SessionId;
		if( IEP_GetSessionId( input, 3, &SessionId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get sessionid." );
		}

		// @todo Нужно избавиться от этого метода в библиотеке.

		// Формирование ответа.
		reply = csDataAllocate( 3, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		reply->body_ptr = nullptr;
		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
		IEP_SetUInt32( reply, 2, "misseddatamask", 0 );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_PURGE_SESSION )
	{
		// Удаление сессии из кэша.
		if( input->attr_count != 3 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"purge session\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Получение идентификатора сессии.
		unsigned int SessionId;
		if( IEP_GetSessionId( input, 2, &SessionId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get sessionid." );
		}

		// Вызов нужной функции.
		if( ies.CmdPurgeSession( SessionId ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( MakeError( INF_ENGINE_ERROR_INTERNAL ) );

		// Формирование ответа.
		reply = csDataAllocate( 2, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		reply->body_ptr = nullptr;
		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_SET_SESSION )
	{
		// Добавление данных сессии в кэш.

		if( input->attr_count < 5 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"send session\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Получение идентификатора сессии.
		unsigned int SessionId;
		if( IEP_GetSessionId( input, 2, &SessionId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get sessionid." );
		}

		// Получение идентификатора инфа.
		unsigned int InfId;
		if( IEP_GetInfId( input, 3, &InfId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get infid." );
		}

		/** Контейнер для сессии. */
		avector<Vars::Raw> lSession;

		unsigned int items;
		if( IEP_GetUInt32( input, 4, "items", &items ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get number of items." );
		}

		// Разбор переменных сессии.
		for( unsigned int i = 0; i < items; i++ )
		{
			Vars::Raw var;
			if( input->attr_count <= 6 + 2 * i )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: incorrect number of items." );
			}

			// Получение имени переменной.
			InfEngineErrors iee = IEP_GetString( input, 5 + 2 * i, "name", &var.vName );
			if( iee != INF_ENGINE_SUCCESS )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get variable name. Return Code: %d", iee );
			}

			// Получение значения переменной.
			iee = IEP_GetString( input, 6 + 2 * i, "value", &var.vValue );
			if( iee != INF_ENGINE_SUCCESS )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get variable value. Return Code: %d", iee );
			}

			// Добавление переменной в сессию.
			lSession.push_back( var );
			if( lSession.no_memory() )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't make session." );
			}
		}

		// Вызов функции добавления сессии в кэш.
		if( ies.CmdSetSession( SessionId, InfId, lSession ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithTrace( MakeError( INF_ENGINE_ERROR_INTERNAL ) );
		}

		// Формирование ответа.
		reply = csDataAllocate( 2, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		reply->body_ptr = nullptr;
		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_UPDATE_SESSION )
	{
		// Изменение значения переменной.
		if( input->attr_count < 4 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"set variable value\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Получение идентификатора сессии.
		unsigned int SessionId;
		if( IEP_GetSessionId( input, 2, &SessionId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get sessionid." );
		}

		// Контейнер для спика переменных.
		avector<Vars::Raw> var_list;

		// Количесво переменных.
		unsigned int items;
		if( IEP_GetUInt32( input, 3, "items", &items ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get number of items." );
		}

		// Разбор переменных сессии.
		for( unsigned int i = 0; i < items; i++ )
		{
			Vars::Raw var;
			if( input->attr_count < 5 + 2 * i )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: incorrect number of items." );
			}

			// Получение имени переменной.
			InfEngineErrors iee = IEP_GetString( input, 4 + 2 * i, "name", &var.vName );
			if( iee != INF_ENGINE_SUCCESS )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get variable name. Return Code: %d", iee );
			}

			// Получение значения переменной.
			iee = IEP_GetString( input, 5 + 2 * i, "value", &var.vValue );
			if( iee != INF_ENGINE_SUCCESS )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get variable value. Return Code: %d", iee );
			}

			// Добавление переменной в список.
			var_list.push_back( var );
			if( var_list.no_memory() )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't make variable list." );
			}
		}

		// Изменение значений переменных.
		if( ies.CmdUpdateSession( SessionId, var_list ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( MakeError( INF_ENGINE_ERROR_INTERNAL ) );

		// Формирование ответа.
		reply = csDataAllocate( 2, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		reply->body_ptr = nullptr;
		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_REQUEST )
	{
		// Запрос ответа от инфа.
		if( input->attr_count != 5 && ( input->attr_count < 6 || ( input->attr_count - 6 ) % 2 ) )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"request\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Получение идентификатора сессии.
		unsigned int SessionId;
		if( IEP_GetSessionId( input, 2, &SessionId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get sessionid." );
		}

		// Получение идентификатора инфа.
		unsigned int InfId;
		if( IEP_GetInfId( input, 3, &InfId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get infid." );
		}

		// Получение запроса пользователя.
		const char * Request;
		if( IEP_GetString( input, 4, "request", &Request ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get request text." );
		}

		// Получение переменных.
		avector<Vars::Raw> Vars;
		if( input->attr_count > 5 )
		{
			unsigned int vars;
			if( IEP_GetUInt32( input, 5, "vars", &vars ) != INF_ENGINE_SUCCESS )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get number of vars." );
			}

			if( input->attr_count != 6 + 2 * vars )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"request\" has incorrect number of variables( %d )", vars );
			}

			// Разбор переменных для обновления сессии.
			for( unsigned int i = 0; i < vars; i++ )
			{
				Vars::Raw var;

				// Получение имени переменной.
				InfEngineErrors iee = IEP_GetString( input, 6 + 2 * i, "name", &var.vName );
				if( iee != INF_ENGINE_SUCCESS )
				{
					LogCSData( input, "Request" );
					ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get variable name. Return Code: %d", iee );
				}

				// Получение значения переменной.
				iee = IEP_GetString( input, 7 + 2 * i, "value", &var.vValue );
				if( iee != INF_ENGINE_SUCCESS )
				{
					LogCSData( input, "Request" );
					ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get variable value. Return Code: %d", iee );
				}

				// Добавление переменной в список.
				Vars.push_back( var );
				if( Vars.no_memory() )
				{
					LogCSData( input, "Request" );
					ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't make variable list." );
				}
			}
		}

		gRequest = Request;
		gInfId = InfId;

		// Вызов функции запроса ответа.
		// @todo Получение длины запроса пользователя.
		InfEngineErrors iee = ies.CmdRequest( SessionId, InfId, Request, Request ? strlen( Request ) : 0, Vars, &reply, csAllocator );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( MakeError( INF_ENGINE_ERROR_INTERNAL ) );
		reply->body_ptr = nullptr;
		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_PURGE_INF )
	{
		// Удаление инфа из кэша.

		if( input->attr_count != 3 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"purge inf\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Получение идентификатора инфа.
		unsigned int InfId;
		if( IEP_GetInfId( input, 2, &InfId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get infid." );
		}

		// Вызов нужной функции.
		if( ies.CmdPurgeInf( InfId ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( MakeError( INF_ENGINE_ERROR_INTERNAL ) );

		// Выделение памяти для ответа.
		reply = csDataAllocate( 2, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_SET_INF )
	{
		// Получение инфа и сохранение его в кэш.

		if( input->attr_count < 5 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"send inf\" has incorrect number of attributes( %d )", input->attr_count );
		}

		unsigned int input_ptr = 2;

		// Получение идентификатора инфа.
		unsigned int InfId;
		if( IEP_GetInfId( input, input_ptr++, &InfId ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Can't get InfId" );
		}

		// Получение шаблонов инфа.
		const char * templates;
		if( IEP_GetString( input, input_ptr++, "templates", &templates ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Can't get templates." );
		}

		// Получение количества переменных инфа.
		unsigned int vars_cnt;
		if( IEP_GetUInt32( input, input_ptr++, "items", &vars_cnt ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get number of items." );
		}

		// Получение количества словарей инфа.
		unsigned int dicts_cnt;
		if( IEP_GetUInt32( input, input_ptr, "dicts", &dicts_cnt ) != INF_ENGINE_SUCCESS )
			dicts_cnt = 0;
		else
			input_ptr++;

		// Получение переменных инфа.
		avector<Vars::Raw> InfProfile;

		if( input->attr_count < input_ptr + 2 * vars_cnt )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: incorrect number of items." );
		}

		for( size_t i = 0; i < vars_cnt; i++ )
		{
			// Получение имени переменной.
			const char * name;
			InfEngineErrors iee = IEP_GetString( input, input_ptr++, "name", &name );
			if( iee != INF_ENGINE_SUCCESS )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get variable name. Return Code: %d", iee );
			}

			// Получение значения переменной.
			const char * value;
			iee = IEP_GetString( input, input_ptr++, "value", &value );
			if( iee != INF_ENGINE_SUCCESS )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get variable value. Return Code: %d", iee );
			}

			// Добавление переменной в профиль инфа.
			InfProfile.grow();
			if( InfProfile.no_memory() )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't make inf profile." );
			}
			InfProfile.back().vName = name;
			InfProfile.back().vValue = value;
		}

		// Получение словарей инфа.
		avector<const char*> dicts_names;
		avector<const char*> dicts;

		if( input->attr_count < input_ptr + 2 * dicts_cnt )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: incorrect number of dictionaries." );
		}

		for( size_t i = 0; i < dicts_cnt; i++ )
		{
			// Получение имени словаря.
			const char * dict_name;
			InfEngineErrors iee = IEP_GetString( input, input_ptr++, "name", &dict_name );
			if( iee != INF_ENGINE_SUCCESS )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get dictionary name. Return Code: %d", iee );
			}

			// Получение текста словаря.
			const char * dict;
			iee = IEP_GetString( input, input_ptr++, "value", &dict );
			if( iee != INF_ENGINE_SUCCESS )
			{
				LogCSData( input, "Request" );
				ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get dictionary. Return Code: %d", iee );
			}

			// Добавление имени словаря в список имён словарей.
			char * buf = nAllocateObjects( csAllocator, char, strlen( dict_name ) + 1 );
			if( !buf )
				ReturnWithError( MakeError( INF_ENGINE_ERROR_NOFREE_MEMORY ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			strcpy( buf, dict_name );

			dicts_names.push_back( buf );
			if( dicts_names.no_memory() )
				ReturnWithError( MakeError( INF_ENGINE_ERROR_NOFREE_MEMORY ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Добавление словаря в список словарей.
			buf = nAllocateObjects( csAllocator, char, strlen( dict ) + 1 );
			if( !buf )
				ReturnWithError( MakeError( INF_ENGINE_ERROR_NOFREE_MEMORY ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			strcpy( buf, dict );

			dicts.push_back( buf );
			if( dicts_names.no_memory() )
				ReturnWithError( MakeError( INF_ENGINE_ERROR_NOFREE_MEMORY ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}

		// Вызов функции сохранения профиля в кэш.
		if( ies.CmdSetInf( InfId, templates, InfProfile, dicts_names.get_buffer(), dicts.get_buffer(), dicts_names.size() ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( MakeError( INF_ENGINE_ERROR_INTERNAL ) );

		// Выделение памяти под ответ.
		reply = csDataAllocate( 2, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_GET_CACHED_SESSIONS_COUNT )
	{
		// Подсчет количества закэшированных сессий.

		if( input->attr_count != 2 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"session count\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Вызов функции подсчета количества закэшированных сессий.
		int Count = 0;

		// @info: Данная функциональность не поддержана.

		// Выделение памяти под ответ.
		reply = csDataAllocate( 3, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
		IEP_SetInt( reply, 2, "count", Count );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_GET_SESSIONS_IDS )
	{
		// Запрос всех идентификаторов закэшированных сессий.

		if( input->attr_count != 2 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"get sessions ids\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Вызов функции сбора всех идентификаторов закэшированных сессий.
		int Count = 0;
		// @info: Данная функциональность не поддержана.

		// Выделение памяти под ответ.
		reply = csDataAllocate( 3, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
		IEP_SetInt( reply, 2, "count", Count );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_GET_CACHED_SESSION_DATA )
	{
		// Запрос закэшированных сессий.

		if( input->attr_count != 3 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"get session data\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Получение идентификатора сессии.
		unsigned int SessionID;
		if( IEP_GetSessionId( input, 2, &SessionID ) != INF_ENGINE_SUCCESS )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: can't get sessionid." );
		}

		// Вызов функции сбора закэшированных сессий.
		// @info: Данная функциональность не поддержана.

		// Выделение памяти.
		reply = csDataAllocate( 2, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
	}
	else if( cmd == INF_ENGINE_PROTOCOL_GET_CACHED_SESSIONS_ALL_DATA )
	{
		// Запрос всех закэшированных сессий.

		if( input->attr_count != 2 )
		{
			LogCSData( input, "Request" );
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid request package: command \"get all sessions data\" has incorrect number of attributes( %d )", input->attr_count );
		}

		// Вызов нужной функции.
		int Count = 0;
		// @info: Данная функциональность не поддержана.

		// Выделение памяти под ответ.
		reply = csDataAllocate( 3, csAllocator );
		if( !reply )
			ReturnWithError( MakeError( INF_ENGINE_ERROR_INTERNAL ), INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		IEP_SetProtocol( reply, gProtocolVersion );
		IEP_SetStatus( reply, INF_ENGINE_SUCCESS );
		IEP_SetInt( reply, 2, "count", Count );
	}
	else
	{
		LogCSData( input, "Request" );
		ReturnWithError( MakeError( INF_ENGINE_ERROR_INV_DATA ), "Invalid package: uncnown command: %d", cmd );
	}
	gCounters[cmd]++;

#    ifdef LOG_DATA_FLOW_ENABLED

	// Логгирование потока данных.
	if( LogDataFlow )
		LogCSData( reply, "Response" );

#    endif

	// Упаковка ответа.
	csdata_data_t * reply_packed = csDataPack( reply, csAllocator, CSDATAPACK_FULL );
	if( !reply_packed )
	{
		if( errno == ENOMEM )
			return MakeError( INF_ENGINE_ERROR_NOFREE_MEMORY );
		else
			return MakeError( INF_ENGINE_ERROR_INV_DATA );
	}

	return reply_packed;
}

/** Logging memeory statistics. */
void LogMemoryStatistics()
{
#    ifdef INF_ENGINE_STATISTICS
	LogInfo( "nMemoryAllocator statistics ( client-server ): %u", csAllocator.GetAllocatedMemorySize() );
	LogInfo( "Counter INIT request: %u", gCounters[INF_ENGINE_PROTOCOL_INIT] );
	LogInfo( "Counter PURGE INF request: %u", gCounters[INF_ENGINE_PROTOCOL_PURGE_INF] );
	LogInfo( "Counter PURGE SESSION request: %u", gCounters[INF_ENGINE_PROTOCOL_PURGE_SESSION] );
	LogInfo( "Counter REQUEST request: %u", gCounters[INF_ENGINE_PROTOCOL_REQUEST] );
	LogInfo( "Counter SEND INF request: %u", gCounters[INF_ENGINE_PROTOCOL_SET_INF] );
	LogInfo( "Counter SEND SESSION request: %u", gCounters[INF_ENGINE_PROTOCOL_SET_SESSION] );
	LogInfo( "Counter UPDATE SESSION request: %u", gCounters[INF_ENGINE_PROTOCOL_UPDATE_SESSION] );
#    endif
}

/** Timeout alarm function. */
void time_to_die( int /*aSignal*/ )
{
	if( !strcasecmp( gInfServerDebugMode, "EXT" ) )
		abort();
	csAbnormalExit( MakeError( INF_ENGINE_ERROR_TIMEOUT ), INF_ENGINE_ERROR_TIMEOUT, 5 );
}

/** Crash detection functions. */
void SignalKill( int /*aSignal*/ )
{
	csAbnormalExit( MakeError( INF_ENGINE_ERROR_SERVER_KILLED ), INF_ENGINE_ERROR_SERVER_KILLED, 5 );
}

void SignalCrash( int /*sig*/ )
{
	csAbnormalExit( MakeError( INF_ENGINE_ERROR_FATAL ), INF_ENGINE_ERROR_FATAL, 5 );
}




































EliSearchEngine gEllipsisDiscloseEngine;

/**
 *  Функция раскрытия эллипсиса в случае днопроцессного варианта.
 * @param aUserPreRequest
 * @param aUserPreRequestLength
 * @param aInfPreResponse
 * @param aInfPreResponseLength
 * @param aUserRequest
 * @param aUserRequestLength
 * @param aResult
 * @param aResultLength
 */
InfEngineErrors EllipsisDiscloseLocal( const char * aUserPreRequest, unsigned int aUserPreRequestLength,
									   const char * aInfPreResponse, unsigned int aInfPreResponseLength,
									   const char * aUserRequest, unsigned int aUserRequestLength,
									   char* & aResult, unsigned int & aResultLength,
									   char* & aEllipsisId, unsigned int & aEllipsisIdLength,
									   char* & aEllipsisPartialIDs, unsigned int & aEllipsisPartialIDsLength )
{
	static aTextString Result, EllipsisId, EllipsisPartialIDs;

	aResult = nullptr;
	aResultLength = 0;
	aEllipsisId = nullptr;
	aEllipsisIdLength = 0;
	aEllipsisPartialIDs = nullptr;
	aEllipsisPartialIDsLength = 0;

	// Раскрытие эллипсиса.
	EliSearchEngine::ReturnCode eserc = gEllipsisDiscloseEngine.EllipsisDisclose( aUserRequest, aUserRequestLength,
																				  aInfPreResponse, aInfPreResponseLength, aUserPreRequest,
																				  aUserPreRequestLength, Result, EllipsisId, EllipsisPartialIDs );
	switch( eserc )
	{
	case EliSearchEngine::rcSuccess:
		break;
	case EliSearchEngine::rcErrorNoFreeMemory:
		ReturnWithTrace( INF_ENGINE_ERROR_NOFREE_MEMORY );
	case EliSearchEngine::rcErrorInvArgs:
		ReturnWithTrace( INF_ENGINE_ERROR_INV_ARGS );
	case EliSearchEngine::rcErrorInvData:
		ReturnWithTrace( INF_ENGINE_ERROR_INV_DATA );
	case EliSearchEngine::rcErrorBaseIsNotOpen:
		ReturnWithTrace( INF_ENGINE_ERROR_STATE );
	case EliSearchEngine::rcErrorFStorage:
		ReturnWithTrace( INF_ENGINE_ERROR_FSTORAGE );
	case EliSearchEngine::rcErrorBaseIsOpened:
	case EliSearchEngine::rcErrorFault:
		ReturnWithTrace( INF_ENGINE_ERROR_FAULT );
	case EliSearchEngine::rcErrorDLF:
		ReturnWithTrace( INF_ENGINE_ERROR_DLF );
	case EliSearchEngine::rcUnsuccess:
		break;
	}

	aEllipsisPartialIDs = static_cast<char*>( malloc( EllipsisPartialIDs.size() + 1 ) );
	if( !aEllipsisPartialIDs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	memcpy( aEllipsisPartialIDs, EllipsisPartialIDs.ToConstChar(), EllipsisPartialIDs.size() );
	aEllipsisPartialIDs[EllipsisPartialIDs.size()] = '\0';
	aEllipsisPartialIDsLength = EllipsisPartialIDs.size();

	if( eserc == EliSearchEngine::rcUnsuccess )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Выделение памяти для ответа.
	if( Result.empty() )
	{
		aResult = nullptr;
		aResultLength = 0;
		aEllipsisId = nullptr;
		aEllipsisIdLength = 0;
	}
	else
	{
		aResult = static_cast<char*>( malloc( Result.size() + 1 ) );
		if( !aResult )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		memcpy( aResult, Result.ToConstChar(), Result.size() );
		aResult[Result.size()] = '\0';
		aResultLength = Result.size();

		aEllipsisId = static_cast<char*>( malloc( EllipsisId.size() + 1 ) );
		if( !aEllipsisId )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		memcpy( aEllipsisId, EllipsisId.ToConstChar(), EllipsisId.size() );
		aEllipsisId[EllipsisId.size()] = '\0';
		aEllipsisIdLength = EllipsisId.size();
	}

	return INF_ENGINE_SUCCESS;
}

/** Идентификатор процесса раскрытия эллипсиса. */
pid_t gEllipsisProcesId;

#define APPLICATION "InfServer"

#define VERSION "2.0"

#define USAGE_STRING "    " APPLICATION " v." VERSION "\n" \
													  "\n" \
													  "  USAGE: " APPLICATION " [OPTIONS] ConfigPath\n\n" \
																			  "    --root=path       set root path\n" \
																			  "    --version         print program version and build information\n" \
																			  "    --help            give this help list\n" \
																			  "    --signature       print signature for this binary\n" \
																			  "    --signature-bin   print signature in binary format\n"

#define BUILDINFO \
	"    " APPLICATION " v." VERSION "\n" \
	"\n" \
	"      Release version: " InfEngineReleaseVersion " ( " __DATE__ " " __TIME__ " )\n" \
	"\n" \
	"      Build system: \n" \
	"       * Server: " ServerName " " BuildSystem " " ServerType "\n" \
	"       * GCC " __VERSION__ "\n"

int main( int argc, char ** argv )
{
	csAllocator.SetMemoryStep( 1024 * 1024 );

	NanoLib::SysLogLogger Logger( "InfEngine Server" );
	NanoLib::LogSystem::SetLogger( Logger, LSL_WARN );

	// Отключение сообщений от обшибках в getopt.
	opterr = 0;

	// Описание аргументов.
	struct option long_options[] = {
		{ "help",           0, 0, 0 },
		{ "version",        0, 0, 0 },
		{ "signature",      0, 0, 0 },
		{ "signature-bin",  0, 0, 0 },
		{ nullptr,          0, 0, 0 }
	};

	// Parse command line arguments.
	while( 1 )
	{
		int option_index { 0 };
		int c = getopt_long_only( argc, argv, "", long_options, &option_index );
		if( c == -1 )
			break;

		switch( c )
		{
		case 0:
			if( option_index == 0 )
			{
				printf( "\n" USAGE_STRING "\n\n" );
				return INF_ENGINE_SUCCESS;
			}
			else if( option_index == 1 )
			{
				printf( "\n" BUILDINFO "\n\n" );
				return INF_ENGINE_SUCCESS;
			}
			else if( option_index == 2 )
			{
				NanoLib::SysLogLogger Logger( "InfCompiler" );
				NanoLib::LogSystem::SetLogger( Logger, LSL_DBG );

				char * signature = nullptr;
				unsigned int size = 0;
				nMemoryAllocator aAllocator;

				InfEngineErrors iee = BuildBinarySignature( signature, size, aAllocator );
				if( INF_ENGINE_SUCCESS != iee )
					ReturnWithTrace( iee );

				if( INF_ENGINE_SUCCESS != ( iee = PrintBinarySignature( signature ) ) )
					ReturnWithTrace( iee );

				aAllocator.Reset();

				return INF_ENGINE_SUCCESS;
			}
			else if( option_index == 3 )
			{
				NanoLib::SysLogLogger Logger( "InfCompiler" );
				NanoLib::LogSystem::SetLogger( Logger, LSL_DBG );

				char * signature = nullptr;
				unsigned int size = 0;
				char * str = nullptr;
				unsigned int str_len = 0;
				nMemoryAllocator aAllocator;

				InfEngineErrors iee = BuildBinarySignature( signature, size, aAllocator );
				if( INF_ENGINE_SUCCESS != iee )
					ReturnWithTrace( iee );

				if( INF_ENGINE_SUCCESS != ( iee = SignatureToString( signature, size, str, str_len, aAllocator ) ) )
					ReturnWithTrace( iee );

				printf( "%s", str );

				aAllocator.Reset();

				return INF_ENGINE_SUCCESS;
			}
			break;

		default:
			printf( "\n" USAGE_STRING "\n\n" );
			return INF_ENGINE_ERROR_INV_ARGS;
		}
	}

	// Проверка и подготовка аргументов командной строки.

	if( optind >= argc || argc - optind != 1 )
	{
		printf( "\n" USAGE_STRING "\n\n" );

		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, "Config file path is missed." );
	}
	const char * ConfigFilePath { argv[optind] };
	char ConfigFilePathBuffer[PATH_MAX];
	InfEngineErrors iee = PrepareFilePath( ConfigFilePath, ConfigFilePathBuffer );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( iee, "Invalid Config file path: %s", strerror( errno ) );

	NanoLib::ConfigFile Config;

	NanoLib::ConfigFile::ReturnCode cfrc = Config.ParseFile( ConfigFilePath );
	if( cfrc != NanoLib::ConfigFile::rcSuccess )
		if( cfrc != NanoLib::ConfigFile::rcSuccess )
		{
			switch( cfrc )
			{
			case NanoLib::ConfigFile::rcSuccess:
				break;
			case NanoLib::ConfigFile::rcErrorNoFreeMemory:
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			case NanoLib::ConfigFile::rcErrorInvFileFormat:
				ReturnWithError( INF_ENGINE_ERROR_FILE, "Invalid config file format." );
			case NanoLib::ConfigFile::rcErrorFileAccess:
			case NanoLib::ConfigFile::rcErrorFileRead:
				ReturnWithError( INF_ENGINE_ERROR_FILE, "Can't access config file." );
			default:
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't parse config file: %s", Config.ReturnCodeToString( cfrc ) );
			}
		}


	const char * cMainRootDir = nullptr;

	const char * cFunctionsRootDir = nullptr;
	const char * cFunctionsConfigFile = nullptr;

	const char * cLogLevel = nullptr;
	const char * cLogIdentificator = nullptr;
	const char * cLogDataFlow = nullptr;

	const char * cCacheServers = nullptr;
	const char * cCacheTTL = nullptr;

	const char * cEllipsisMode = nullptr;
	const char * cEllipsisBasePath = nullptr;
	const char * cEllipsisDefault = nullptr;
	const char * cEllipsisMaxRequestLen = nullptr;

	const char * cSearchEngineDictMergeMethod = nullptr;
	const char * cSearchEngineDictMergeEffect = nullptr;

	const char * cInfServerBasePath = nullptr;
	const char * cInfServerMaxRequestsNumber = nullptr;
	const char * cInfServerTimeOut = nullptr;
	const char * cInfServerDLAliases = nullptr;

	const char * cSearchEngineUserBasePriority = nullptr;

	for( unsigned int i = 0; i < Config.GetSectionsNumber(); i++ )
	{
		if( !strcasecmp( "Main", Config.GetSectionName( i ) ) )
		{
			for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ )
			{
				const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );

				if( !strcasecmp( "RootDir", Record->vName ) )
				{
					if( !cMainRootDir )
						cMainRootDir = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else
					LogWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
			}
		}
		else if( !strcasecmp( "Functions", Config.GetSectionName( i ) ) )
		{
			for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ )
			{
				const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );

				if( !strcasecmp( "RootDir", Record->vName ) )
				{
					if( !cFunctionsRootDir )
						cFunctionsRootDir = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "ConfigFile", Record->vName ) )
				{
					if( !cFunctionsConfigFile )
						cFunctionsConfigFile = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else
					LogWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
			}
		}
		else if( !strcasecmp( "Log", Config.GetSectionName( i ) ) )
		{
			for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ )
			{
				const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );

				if( !strcasecmp( "Level", Record->vName ) )
				{
					if( !cLogLevel )
						cLogLevel = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "Identificator", Record->vName ) )
				{
					if( !cLogIdentificator )
						cLogIdentificator = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "DataFlow", Record->vName ) )
				{
					if( !cLogDataFlow )
						cLogDataFlow = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else
					LogWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
			}
		}
		else if( !strcasecmp( "Cache", Config.GetSectionName( i ) ) )
		{
			for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ )
			{
				const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );

				if( !strcasecmp( "Servers", Record->vName ) )
				{
					if( !cCacheServers )
						cCacheServers = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "TTL", Record->vName ) )
				{
					if( !cCacheTTL )
						cCacheTTL = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else
					LogWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
			}
		}
		else if( !strcasecmp( "Ellipsis", Config.GetSectionName( i ) ) )
		{
			for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ )
			{
				const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );

				if( !strcasecmp( "Mode", Record->vName ) )
				{
					if( !cEllipsisMode )
						cEllipsisMode = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "BasePath", Record->vName ) )
				{
					if( !cEllipsisBasePath )
						cEllipsisBasePath = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "Default", Record->vName ) )
				{
					if( !cEllipsisDefault )
						cEllipsisDefault = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "MaxRequestLength", Record->vName ) )
				{
					if( !cEllipsisMaxRequestLen )
						cEllipsisMaxRequestLen = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else
					LogWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
			}
		}
		else if( !strcasecmp( "SearchEngine", Config.GetSectionName( i ) ) )
		{
			for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ )
			{
				const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );

				if( !strcasecmp( "UserBasePriority", Record->vName ) )
				{
					if( !cSearchEngineUserBasePriority )
						cSearchEngineUserBasePriority = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( Record->vName && !strcasecmp( "DictMergeMethod", Record->vName ) )
				{
					if( !cSearchEngineDictMergeMethod )
						cSearchEngineDictMergeMethod = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( Record->vName && !strcasecmp( "DictMergeEffect", Record->vName ) )
				{
					if( !cSearchEngineDictMergeEffect )
						cSearchEngineDictMergeEffect = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else
					LogWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
			}
		}
		else if( !strcasecmp( "InfServer", Config.GetSectionName( i ) ) )
		{
			for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ )
			{
				const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );

				if( !strcasecmp( "BasePath", Record->vName ) )
				{
					if( !cInfServerBasePath )
						cInfServerBasePath = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "MaxRequestsNumber", Record->vName ) )
				{
					if( !cInfServerMaxRequestsNumber )
						cInfServerMaxRequestsNumber = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "TimeOut", Record->vName ) )
				{
					if( !cInfServerTimeOut )
						cInfServerTimeOut = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "DLAliases", Record->vName ) )
				{
					if( !cInfServerDLAliases )
						cInfServerDLAliases = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else if( !strcasecmp( "DebugMode", Record->vName ) )
				{
					if( !gInfServerDebugMode )
						gInfServerDebugMode = Record->vValue;
					else
						LogWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
				else
					LogWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );

			}
		}
		else
			LogWarn( "Unexpected section in config file: [%s]", Config.GetSectionName( i ) );
	}
	
	// Разбор переменных окружения.
	std::string eLogLevel{ getenv( "IFS_LOG_LEVEL" ) ?: "" };
	if( !eLogLevel.empty() )
		cLogLevel = eLogLevel.c_str();
	
	std::string eLogDataFlow{ getenv( "IFS_LOG_DATA_FLOW" ) ?: "" };
	if( !eLogDataFlow.empty() )
		cLogDataFlow = eLogDataFlow.c_str();
	
	std::string eCacheServers{ getenv( "IFS_CACHE_SERVERS" ) ?: "" };
	if( !eCacheServers.empty() )
		cCacheServers = eCacheServers.c_str();
	
	std::string eCacheTTL{ getenv( "IFS_CACHE_TTL" ) ?: "" };
	if( !eCacheTTL.empty() )
		cCacheTTL = eCacheTTL.c_str();

	// Настройка логгера.
	if( cLogIdentificator )
		Logger.SetIdentificator( cLogIdentificator );

	if( !cLogLevel || !strcasecmp( cLogLevel, "NONE" ) )
		NanoLib::LogSystem::SetLogLevel( LSL_NONE );
	else if( !strcasecmp( cLogLevel, "ERROR" ) || !strcasecmp( cLogLevel, "ERRORS" ) )
		NanoLib::LogSystem::SetLogLevel( LSL_ERR );
	else if( !strcasecmp( cLogLevel, "WARN" ) || !strcasecmp( cLogLevel, "WARNING" ) || !strcasecmp( cLogLevel, "WARNINGS" ) )
		NanoLib::LogSystem::SetLogLevel( LSL_WARN );
	else if( !strcasecmp( cLogLevel, "INFO" ) )
		NanoLib::LogSystem::SetLogLevel( LSL_INFO );
	else if( !strcasecmp( cLogLevel, "DEBUG" ) )
		NanoLib::LogSystem::SetLogLevel( LSL_DBG );
	else
	{
		LogWarn( "Invalid record value in config file: [Log] Level. Logging is turned off." );

		NanoLib::LogSystem::SetLogLevel( LSL_NONE );
	}

#    ifdef LOG_DATA_FLOW_ENABLED
	// Включение/выключение полного логгирования данных.
	LogDataFlow = false;
	if( cLogDataFlow )
	{
		if( !strcasecmp( cLogDataFlow, "TRUE" ) || !strcasecmp( cLogDataFlow, "YES" ) )
			LogDataFlow = true;
		else if( !strcasecmp( cLogDataFlow, "FALSE" ) || !strcasecmp( cLogDataFlow, "NO" ) )
			LogDataFlow = false;
		else
		{
			LogWarn( "Invalid record value in config file: [Log] DataFlow. Data flow logging is turned off." );
			LogDataFlow = false;
		}
	}
#    endif  /** LOG_DATA_FLOW_ENABLED */

	// Get root dir from config file.
	char MainRootDirBuffer[4096];
	if( !cMainRootDir )
	{
		// Get default root dir. Default root dir is parent dir for the dir containing config file.

		// It is necessary to copy path from const string, because of using 'dirname' function, which could modify it's argument value.
		char TmpBufferString[4096];
		strcpy( TmpBufferString, ConfigFilePath );

		// Get dir containing config file.
		char * ConfigFileDir = dirname( TmpBufferString );
		unsigned int ConfigFileDirNameLength = strlen( ConfigFileDir );

		// Create path to root dir.
		strcpy( TmpBufferString + ConfigFileDirNameLength, "/../" );
		cMainRootDir = TmpBufferString;

		// Validate and convert path to absolute value.
		iee = PrepareDirPath( cMainRootDir, MainRootDirBuffer );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithError( iee, "Invalid root dir path. %s", strerror( errno ) );
	}
	else
	{
		iee = PrepareDirPath( cMainRootDir, MainRootDirBuffer );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithError( iee, "Invalid record value in config file: [Main] RootDir: %s", strerror( errno ) );
	}

	// Change current dir to root dir.
	if( cMainRootDir && chdir( cMainRootDir ) == -1 )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", cMainRootDir, strerror( errno ) );

	// Parse configuration for choosing answer logic.
	if( cSearchEngineUserBasePriority )
	{
		if( !strcasecmp( "High", cSearchEngineUserBasePriority ) )
			ies.SetSkipUnknownLogic( InfBFinderExt::SkipUnknownLogic::Off );
		else if( !strcasecmp( "CustomByVar", cSearchEngineUserBasePriority ) )
			ies.SetSkipUnknownLogic( InfBFinderExt::SkipUnknownLogic::Var );
		else if( !strcasecmp( "ForceCustom", cSearchEngineUserBasePriority ) )
			ies.SetSkipUnknownLogic( InfBFinderExt::SkipUnknownLogic::Force );
		else
		{
			LogWarn( "Invalid record value in config file: [SearchEngine] UserBasePriority. Set default value: \"High\"." );
			ies.SetSkipUnknownLogic( InfBFinderExt::SkipUnknownLogic::Off );
		}
	}
	else ies.SetSkipUnknownLogic( InfBFinderExt::SkipUnknownLogic::Off );

	// Parse configuration for ellipsis engine.
	InfEngineServer::EllipsisMode EllipsisMode = InfEngineServer::EllipsisMode::Off;
	if( !cEllipsisMode || !strcasecmp( "NONE", cEllipsisMode ) || !strcasecmp( "OFF", cEllipsisMode ) )
		EllipsisMode = InfEngineServer::EllipsisMode::Off;
	else if( !strcasecmp( "FORCE", cEllipsisMode ) )
		EllipsisMode = InfEngineServer::EllipsisMode::Force;
	else if( !strcasecmp( "BEST", cEllipsisMode ) )
		EllipsisMode = InfEngineServer::EllipsisMode::Best;
	else
	{
		LogWarn( "Invalid record value in config file: [Ellipsis] Mode. Set default value: \"OFF\"." );
		EllipsisMode = InfEngineServer::EllipsisMode::Off;
	}

	// Request length limit for ellipsis disclosing.
	int EllipsisMaxRequestLen = 0;
	if( cEllipsisMaxRequestLen )
	{
		char * end = nullptr;
		EllipsisMaxRequestLen = strtol( cEllipsisMaxRequestLen, &end, 10 );
		if( ( end && *end ) || EllipsisMaxRequestLen < 0 )
		{
			LogWarn( "Invalid record value in config file: [Ellipsis] MaxRequestLength. Set default value: \"0\"." );
			EllipsisMaxRequestLen = 0;
		}
	}

	InfEngineServer::EllipsisDefault EllipsisDefault = InfEngineServer::EllipsisDefault::None;
	if( !cEllipsisDefault || !strcasecmp( "NONE", cEllipsisDefault ) )
		EllipsisDefault = InfEngineServer::EllipsisDefault::None;
	else if( !strcasecmp( "ON", cEllipsisDefault ) )
		EllipsisDefault = InfEngineServer::EllipsisDefault::On;
	else if( !strcasecmp( "OFF", cEllipsisDefault ) )
		EllipsisDefault = InfEngineServer::EllipsisDefault::Off;
	else
	{
		LogWarn( "Invalid record value in config file: [Ellipsis] Default. Set default value: \"OFF\"." );
		EllipsisDefault = InfEngineServer::EllipsisDefault::Off;
		EllipsisMode = InfEngineServer::EllipsisMode::Off;
	}

	if( EllipsisMode != InfEngineServer::EllipsisMode::Off )
	{
		char EllipsisBasePathBuffer[4096];
		iee = PrepareFilePath( cEllipsisBasePath, EllipsisBasePathBuffer );
		if( iee != INF_ENGINE_SUCCESS )
		{
			LogError( "Invalid record value in config file: [Ellipsis] BasePath: %s", strerror( errno ) );
			EllipsisMode = InfEngineServer::EllipsisMode::Off;
		}
		else
		{
			if( !cEllipsisBasePath )
			{
				LogWarn( "Ellipsis base path is not defined. Ellipsis Disclosing Engine is turned off." );
				EllipsisMode = InfEngineServer::EllipsisMode::Off;
			}
			else
			{
				// Start ellipsis disclosing engine.
				EliSearchEngine::ReturnCode eserc = gEllipsisDiscloseEngine.LoadEllisisBase( cEllipsisBasePath );
				if( eserc != EliSearchEngine::rcSuccess )
				{
					switch( eserc )
					{
					case EliSearchEngine::rcSuccess:
						break;
					case EliSearchEngine::rcErrorNoFreeMemory:
						ReturnWithTrace( INF_ENGINE_ERROR_NOFREE_MEMORY );
					case EliSearchEngine::rcErrorInvArgs:
					case EliSearchEngine::rcErrorInvData:
					case EliSearchEngine::rcErrorBaseIsNotOpen:
					case EliSearchEngine::rcErrorFStorage:
					case EliSearchEngine::rcErrorFault:
					case EliSearchEngine::rcUnsuccess:
					case EliSearchEngine::rcErrorBaseIsOpened:
					case EliSearchEngine::rcErrorDLF:
						LogWarn( "Can't load ellipsis base. Ellipsis disclosing engine is turned off." );
						EllipsisMode = InfEngineServer::EllipsisMode::Off;
					}
				}
			}
		}

		ies.SetMaxEllRequestLength( EllipsisMaxRequestLen );
	}

	// Set dictionaries merging configuration.
	if( !cSearchEngineDictMergeMethod )
	{
		ies.SetDictMergeMethod( InfDictWrapManipulator::jmDefault );
	}
	else if( !strcasecmp( cSearchEngineDictMergeMethod, "replace" ) )
	{
		ies.SetDictMergeMethod( InfDictWrapManipulator::jmSubstitution );
	}
	else if( !strcasecmp( cSearchEngineDictMergeMethod, "expand" ) )
	{
		ies.SetDictMergeMethod( InfDictWrapManipulator::jmAddiction );
	}
	else
	{
		ies.SetDictMergeMethod( InfDictWrapManipulator::jmDefault );
		LogWarn( "Invalid record value in config file: DictMergeMethod. Use default value: \"SUBSTITUTION\".", strerror( errno ) );
	}

	// Prepare functions config file path.
	char FunctionsConfigFileBuffer[4096];
	if( ( iee = PrepareFilePath( cFunctionsConfigFile, FunctionsConfigFileBuffer ) ) != INF_ENGINE_SUCCESS )
	{
		LogWarn( "Invalid record value in config file: [Functions] ConfigFile: %s", strerror( errno ) );
		cFunctionsConfigFile = nullptr;
	}

	// Prepare functions root path.
	char FunctionsRootDirBuffer[4096];
	if( ( iee = PrepareDirPath( cFunctionsRootDir, FunctionsRootDirBuffer ) ) != INF_ENGINE_SUCCESS )
	{
		LogWarn( "Invalid record value in config file: [Functions] RootDir: %s", strerror( errno ) );
		cFunctionsRootDir = nullptr;
	}


	// Set limit for requests number that could be processed by one process.
	int MaxRequestsNumber = 10000;

	char * end = nullptr;
	if( cInfServerMaxRequestsNumber )
	{
		MaxRequestsNumber = strtol( cInfServerMaxRequestsNumber, &end, 10 );
		if( ( end && *end ) || MaxRequestsNumber < 1 )
		{
			LogWarn( "Invalid record value in config file: [InfServer] MaxRequestsNumber. Set default value: \"10000\"." );
			MaxRequestsNumber = 10000;
		}
	}

	// Set timeout for one response generation.
	// @todo Set 2 different timeouts for answer and sources compilation.
	int TimeOut = 30;
	if( cInfServerTimeOut )
	{
		TimeOut = strtol( cInfServerTimeOut, &end, 10 );
		if( ( end && *end ) || TimeOut < 0 )
		{
			LogWarn( "Invalid record value in config file: [InfServer] MaxRequestsNumber. Set default value: \"30\"." );
			TimeOut = 30;
		}
	}
	// Set timeout watcher.
	signal( SIGALRM, time_to_die );

	// Set crash watchers.
	if( !gInfServerDebugMode || !strcasecmp( gInfServerDebugMode, "OFF" ) )
	{
		signal( SIGKILL, SignalKill );
		signal( SIGBUS, SignalCrash );
		signal( SIGSEGV, SignalCrash );
	}

	// Get path to base.
	char InfServerBasePathBuffer[4096];
	if( ( iee = PrepareFilePath( cInfServerBasePath, InfServerBasePathBuffer ) ) != INF_ENGINE_SUCCESS )
		ReturnWithError( iee, "Invalid record value in config file: [InfServer] BasePath: %s", strerror( errno ) );

	// Prepare DL aliases config file.
	char InfServerDLAliasesBuffer[4096];
	if( ( iee = PrepareFilePath( cInfServerDLAliases, InfServerDLAliasesBuffer ) ) != INF_ENGINE_SUCCESS )
	{
		LogWarn( "Invalid record value in config file: [InfServer] DLAliases: %s", strerror( errno ) );
		cInfServerDLAliases = nullptr;
	}

	// Open main base.
	iee = ies.Open( cInfServerBasePath, cFunctionsRootDir, cFunctionsConfigFile, cInfServerDLAliases );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		LogTrace();
	else if( iee != INF_ENGINE_SUCCESS )
	{
		LogTrace();

		// Switch to fatal error mode.
		cs_server_main( ieerror, 10, 30 );

		ReturnWithInfo( iee, "Stop server process." );
	}

	// Check base signature.
	unsigned char Signature[16];
	MD5_CTX context;
	MD5Init( &context );
	MD5Update( &context, (const unsigned char*)InfEngineReleaseVersion, strlen( InfEngineReleaseVersion ) );
	MD5Final( Signature, &context );

	// Setup ellipsis engine.
	if( EllipsisMode != InfEngineServer::EllipsisMode::Off )
	{
		iee = ies.SetUpEllipsisEngine( EllipsisDiscloseLocal, EllipsisMode, EllipsisDefault );
		if( iee != INF_ENGINE_SUCCESS )
		{
			LogError( "Ellipsis disclosing system proccess is failed. Return Code: %d", iee );
			EllipsisMode = InfEngineServer::EllipsisMode::Off;
		}
	}

	// Set cache ttl.
	int CacheTTL = 604800;
	if( cCacheTTL )
	{
		CacheTTL = strtol( cCacheTTL, &end, 10 );
		if( ( end && *end ) || CacheTTL < 60 )
		{
			LogWarn( "Invalid record value in config file: [Cache] TTL. Set default value: \"7*24*60\"." );
			CacheTTL = 604800;
		}
		ies.SetCacheTTL( CacheTTL );
	}

	// Memcached server.
	const char * old = cCacheServers;
	char * cur;
	do
	{
		cur = std::strstr( const_cast<char*>( old ), "," );
		if( cur )
			*cur = '\0';

		// Parse server value.
		char * ct = strstr( (char*)old, ":" );
		if( !ct )
			LogWarn( "Invalid record value in config file: [Cache] Servers. Can't parse value: %s", old );
		else
		{
			*ct = '\0';

			char * end = nullptr;
			unsigned int port = strtol( ct + 1, &end, 10 );
			if( ( end && *end ) || MaxRequestsNumber < 1 )
			{
				LogWarn( "Invalid record value in config file: [Cache] Servers. Can't parse value: %s", old );
				continue;
			}

			iee = ies.AddCacheServer( old, port );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			LogDebug( "Cache Server: %s:%d", old, port );

			*ct = ':';
		}

		if( cur )
		{
			*cur = ',';
			old = cur + 1;
			for(; *old == ' '; old++ )
				;
		}
	} while( cur != nullptr );

	LogDebug( "Start server process. LogLevel: %d", NanoLib::LogSystem::GetLogLevel() );
	LogDebug( "Proc number: %d", MaxRequestsNumber );
	LogDebug( "Timeout: %d", TimeOut );

	cs_server_main( ieserver, MaxRequestsNumber, TimeOut );

	LogInfo( "Stop server process." );

	// Logging memory statistics.
	LogMemoryStatistics();

	return INF_ENGINE_SUCCESS;
}
