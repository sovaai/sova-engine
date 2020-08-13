#include "ClientLib.h"

#include <stdlib.h>

#include "Protocol.hpp"
#include "ItemTypes.hpp"

#include "SessionData.h"
#include "Answer.h"
#include "InfData.h"
#include "Allocator.h"

const char * ClientLibReturnCodeToString( ClientLibReturnCode aClientLibReturnCode )
{
	switch( aClientLibReturnCode )
	{
		case clrcSuccess:
			return "Success";
		case clrcErrorInvArgs:
			return "Invalid arguments";
		case clrcErrorNoFreeMemory:
			return "Can't allocate memory";
		case clrcErrorInvProtocol:
			return "Unsupported protocol version";
		case clrcErrorInvData:
			return "Invalid data";
		case clrcErrorFault:
			return "Unrecognized error";
		case clrcErrorServerFatal:
			return "Server fatal error";
		case clrcErrorInfCompilation:
			return "Can't compile inf templates";
		case clrcErrorConnection:
			return "Server connection error";
		case clrcErrorTimeOut:
			return "Timeout error";
		case clrcErrorServerCrashed:
			return "Server has been crashed";
		case clrcErrorServerKilled:
			return "Server has been killed";
		default:
			return NULL;
	}
}

unsigned int GetProtocolVersion( )
	{ return INF_ENGINE_PROTOCOL_VERSION; }

unsigned int GetClientLibAllocatedMemorySize( )
	{ return GetAllocatedMemorySize( ); }

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * 
 *      Функции для работы с сервером.
 * 
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

// Вспомогательные определения.
#define clear_response() \
	do \
	{ \
		csdata_clear_data( PackedResponse ); \
	} \
	while( 0 )

#define allocate_request( aItemsNumber, aCmd ) \
	/** Сброс аллокатора памяти. */ \
	ClientLibAllocatorReset( ); \
	 \
	/** Создание структуры запроса. */ \
	csdata_data_t * Request = AllocateCSData( aItemsNumber ); \
	if( !Request ) \
		return clrcErrorNoFreeMemory; \
	 \
	/** Инициализация запроса. */ \
	Request->body_ptr = NULL; \
	IEP_SetProtocol( Request, INF_ENGINE_PROTOCOL_VERSION ); \
	IEP_SetCmd( Request, aCmd );

#define make_request( aConnectionString, aConnectTimeOut, aTransportFlags ) \
	/** Упаковка запроса. */ \
	csdata_data_t * PackedRequest = ClientLibPackData( Request, CSDATAPACK_FULL ); \
	if( !PackedRequest ) \
	{ \
		switch( errno ) \
		{ \
			case EINVAL: \
				return clrcErrorInvData; \
			case ENOMEM: \
				return clrcErrorNoFreeMemory; \
			default: \
				return clrcErrorFault; \
		} \
	} \
	 \
	/** Посылка запроса серверу. */ \
	int status; \
	csdata_data_t* PackedResponse = csdata_rpc_call( aConnectionString, aConnectTimeOut, PackedRequest, &status ); \
	 \
	/** Проверка результата. */ \
	if( status || !PackedResponse ) \
	{ \
		if( PackedResponse ) \
		{ \
			/** Освобождение памяти. */ \
			clear_response(); \
		} \
		 \
		if( status ) \
		{ \
			if( status == ENOMEM ) \
				return clrcErrorNoFreeMemory; \
			else \
				return clrcErrorConnection; \
		} \
		else \
			return clrcErrorTimeOut; \
	} \
	 \
	/** Сброс аллокатора памяти. */ \
	ClientLibAllocatorReset( ); \
	 \
	/** Распаковка результата. */ \
	csdata_data_t * Response = ClientLibUnPackData( PackedResponse ); \
	if( !Response ) \
	{ \
		/** Освобождение памяти. */ \
		clear_response(); \
		 \
		if( errno == EINVAL ) \
			return clrcErrorInvData; \
		else if( errno == ENOMEM ) \
			return clrcErrorNoFreeMemory; \
		else \
			return clrcErrorFault; \
	} \
	 \
	/** Проверка протокола. */ \
	int Protocol; \
	if( IEP_GetProtocol( Response, &Protocol ) != INF_ENGINE_SUCCESS ) \
	{ \
		/** Освобождение памяти. */ \
		clear_response(); \
		 \
		return clrcErrorInvData; \
	} \
	 \
	if( !CheckProtocolVersion( Protocol ) ) \
	{ \
		/** Освобождение памяти. */ \
		clear_response(); \
		 \
		return clrcErrorInvProtocol; \
	} \
	 \
	/** Проверка кода ошибки. */ \
	int Status; \
	if( IEP_GetStatus( Response, &Status ) != INF_ENGINE_SUCCESS ) \
	{ \
		/** Освобождение памяти. */ \
		clear_response(); \
		 \
		return clrcErrorInvData; \
	} \
	 \
	if( Status != INF_ENGINE_SUCCESS ) \
	{ \
		/** Освобождение памяти. */ \
		clear_response(); \
		 \
		switch( Status ) \
		{ \
			case INF_ENGINE_ERROR_FATAL: \
				return clrcErrorServerFatal; \
			case INF_ENGINE_ERROR_TIMEOUT: \
				return clrcErrorTimeOut; \
			case INF_ENGINE_ERROR_PATTERN_COMPILATION: \
				return clrcErrorInfCompilation; \
			case INF_ENGINE_SERVER_CRASHED: \
				return clrcErrorServerCrashed; \
			case INF_ENGINE_ERROR_SERVER_KILLED: \
				return clrcErrorServerKilled; \
			case INF_ENGINE_ERROR_INV_PROTOCOL: \
				return clrcErrorInvProtocol; \
			default: \
				return clrcErrorFault; \
		} \
	}

#define check_response_items_number( aItemsNumber ) \
	do \
	{ \
		/** Проверка количества элементов. */ \
		if( Response->attr_count != aItemsNumber ) \
		{ \
			/** Освобождение памяти. */ \
			clear_response(); \
			 \
			return clrcErrorInvData; \
		} \
	} \
	while( 0 )

#define get_missed_data_mask() \
	do \
	{ \
		/** Получение маски недостающих компонентов. */ \
		if( IEP_GetUInt32( Response, 2, "misseddatamask", aMissedDataMask ) != INF_ENGINE_SUCCESS ) \
		{ \
			/** Освобождение памяти. */ \
			clear_response(); \
			 \
			return clrcErrorInvData; \
		} \
	} \
	while( 0 )

ClientLibReturnCode cl_cmd_init( unsigned int aInfId, unsigned int aSessionId, unsigned int * aMissedDataMask,
								 const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag )
{
	// Проверка аргументов.
	if( !aMissedDataMask || !aConnectionString )
		return clrcErrorInvArgs;

	// Создание структуры запроса.
	allocate_request( 4, INF_ENGINE_PROTOCOL_INIT );
	IEP_SetInfId( Request, 2, aInfId );
	IEP_SetSessionId( Request, 3, aSessionId );

	// Запрос к серверу.
	make_request( aConnectionString, aTimeOut, aPackDataFlag ? CSDATAPACK_FULL : 0 );

	// Проверка количества элементов в ответе сервера.
	check_response_items_number( 3 );

	// Получение маски недостающих компонентов.
	get_missed_data_mask( );

	// Освобождение памяти.
	clear_response( );

	return clrcSuccess;
}

ClientLibReturnCode cl_cmd_purge_session( unsigned int aSessionId, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag )
{
	// Проверка аргументов.
	if( !aConnectionString )
		return clrcErrorInvArgs;

	// Создание структуры запроса.
	allocate_request( 3, INF_ENGINE_PROTOCOL_PURGE_SESSION );
	IEP_SetSessionId( Request, 2, aSessionId );

	// Запрос к серверу.
	make_request( aConnectionString, aTimeOut, aPackDataFlag ? CSDATAPACK_FULL : 0 );

	// Проверка количества элементов в ответе сервера.
	check_response_items_number( 2 );

	// Освобождение памяти.
	clear_response( );

	return clrcSuccess;
}

ClientLibReturnCode cl_cmd_send_session( const SessionData * aSessionData, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag )
{
	// Проверка аргументов.
	if( !aSessionData || !aConnectionString || ( aSessionData->vVarsNumber > 0 && !aSessionData->vVars ) )
		return clrcErrorInvArgs;

	// Создание структуры запроса.
	allocate_request( 5 + 2 * aSessionData->vVarsNumber, INF_ENGINE_PROTOCOL_SET_SESSION );
	IEP_SetSessionId( Request, 2, aSessionData->vSessionId );
	IEP_SetInfId( Request, 3, aSessionData->vInfId );
	IEP_SetUInt32( Request, 4, "items", aSessionData->vVarsNumber );

	unsigned int i = 0, pos = 5;
	for( i = 0; i < aSessionData->vVarsNumber; i++ )
	{
		IEP_SetString( Request, pos++, "name", aSessionData->vVars[i].vName );
		IEP_SetString( Request, pos++, "value", aSessionData->vVars[i].vValue );
	}

	// Запрос к серверу.
	make_request( aConnectionString, aTimeOut, aPackDataFlag ? CSDATAPACK_FULL : 0 );

	// Проверка количества элементов в ответе сервера.
	check_response_items_number( 2 );

	// Освобождение памяти.
	clear_response( );

	return clrcSuccess;
}

ClientLibReturnCode cl_cmd_update_session( const SessionData * aSessionData, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag )
{
	// Проверка аргументов.
	if( !aSessionData || !aConnectionString || ( aSessionData->vVarsNumber > 0 && !aSessionData->vVars ) )
		return clrcErrorInvArgs;

	// Создание структуры запроса.
	allocate_request( 4 + 2 * aSessionData->vVarsNumber, INF_ENGINE_PROTOCOL_UPDATE_SESSION );
	IEP_SetSessionId( Request, 2, aSessionData->vSessionId );
	IEP_SetUInt32( Request, 3, "items", aSessionData->vVarsNumber );

	unsigned int i = 0, pos = 4;
	for( i = 0; i < aSessionData->vVarsNumber; i++ )
	{
		IEP_SetString( Request, pos++, "name", aSessionData->vVars[i].vName );
		IEP_SetString( Request, pos++, "value", aSessionData->vVars[i].vValue );
	}

	// Запрос к серверу.
	make_request( aConnectionString, aTimeOut, aPackDataFlag ? CSDATAPACK_FULL : 0 );

	// Проверка количества элементов в ответе сервера.
	check_response_items_number( 2 );

	// Освобождение памяти.
	clear_response( );

	return clrcSuccess;
}

ClientLibReturnCode cl_cmd_request( unsigned int aSessionId, unsigned int aInfId, const char * aRequest, const SessionData * aVarsList, unsigned int * aMissedDataMask, Answer ** aAnswer,
									const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag )
{
	// Проверка аргументов.
	if( !aRequest || !aMissedDataMask || !aAnswer || !aVarsList || !aConnectionString )
		return clrcErrorInvArgs;

	// Создание структуры.
	allocate_request( 6 + 2 * aVarsList->vVarsNumber, INF_ENGINE_PROTOCOL_REQUEST );
	IEP_SetSessionId( Request, 2, aSessionId );
	IEP_SetInfId( Request, 3, aInfId );
	IEP_SetString( Request, 4, "request", aRequest );
	IEP_SetUInt32( Request, 5, "vars", aVarsList->vVarsNumber );

	unsigned int i = 0, pos = 6;
	for( i = 0; i < aVarsList->vVarsNumber; i++ )
	{
		IEP_SetString( Request, pos++, "name", aVarsList->vVars[i].vName );
		IEP_SetString( Request, pos++, "value", aVarsList->vVars[i].vValue );
	}

	// Запрос к серверу.
	make_request( aConnectionString, aTimeOut, aPackDataFlag ? CSDATAPACK_FULL : 0 );

	// Проверка количества элементов в ответе сервера.
	if( Response->attr_count < 4 )
	{
		// Освобождение памяти.
		clear_response( );

		return clrcErrorInvData;
	}

	// Получение маски недостающих компонентов.
	get_missed_data_mask( );

	// Преобразование ответа сервера.
	unsigned int ItemsNumber;
	if( IEP_GetUInt32( Response, 3, "items", &ItemsNumber ) != INF_ENGINE_SUCCESS )
	{
		// Освобождение памяти.
		clear_response( );

		return clrcErrorInvData;
	}

	// Выделение памяти.
	Answer * NewAnswer = (Answer*)malloc( sizeof ( Answer ) );
	if( !NewAnswer )
	{
		// Освобождение памяти.
		clear_response( );

		return clrcErrorNoFreeMemory;
	}

	// Генерация ответа.
	NewAnswer->vItemsNumber = ItemsNumber;
	NewAnswer->vItems = (__AnswerItem*)malloc( sizeof (__AnswerItem ) * ItemsNumber );
	if( !NewAnswer->vItems )
	{
		// Освобождение памяти.
		cl_answer_free( NewAnswer );
		clear_response( );

		return clrcErrorNoFreeMemory;
	}
	bzero( NewAnswer->vItems, sizeof (__AnswerItem ) * ItemsNumber );

	// Получение данных.
	pos = 4;
	for( i = 0; i < ItemsNumber; i++ )
	{
		if( pos >= Response->attr_count )
		{
			// Освобождение памяти.
			cl_answer_free( NewAnswer );
			clear_response( );

			return clrcErrorInvData;
		}

		int cmd;
		if( IEP_GetInt( Response, pos++, "cmd", &cmd ) != INF_ENGINE_SUCCESS )
		{
			// Освобождение памяти.
			cl_answer_free( NewAnswer );
			clear_response( );

			return clrcErrorInvData;
		}

		if( cmd == itText )
		{
			// Текстовый элемент.

			if( pos >= Response->attr_count )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}

			// Установка типа элемента.
			NewAnswer->vItems[i].vType = AnswerItemTypeTextString;

			// Установка текста.
			const char * Text = NULL;
			if( IEP_GetString( Response, pos++, "textstring", &Text ) != INF_ENGINE_SUCCESS ||
				( NewAnswer->vItems[i].vValue.vString.vText = strdup( Text ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vString.vTextLength = strlen( Text );
		}
		else if( cmd == itInf )
		{
			// Тэг ответа боту.

			if( pos >= Response->attr_count )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}

			// Установка типа элемента.
			NewAnswer->vItems[i].vType = AnswerItemTypeTagInf;

			// Установка запроса.
			const char * Value = NULL;
			if( IEP_GetString( Response, pos++, "value", &Value ) != INF_ENGINE_SUCCESS || ( NewAnswer->vItems[i].vValue.vInf.vValue = strdup( Value ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}

			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vInf.vValueLength = strlen( Value );

			const char * Request = NULL;
			if( IEP_GetString( Response, pos, "request", &Request ) == INF_ENGINE_SUCCESS )
			{
				pos++;

				// Параметр request может быть опущен.
				if( !Request )
					Request = "";

				if( ( NewAnswer->vItems[i].vValue.vInf.vRequest = strdup( Request ) ) == NULL )
				{
					// Освобождение памяти.
					cl_answer_free( NewAnswer );
					clear_response( );

					return clrcErrorInvData;
				}
				NewAnswer->vItems[i].vValue.vInf.vRequestLength = strlen( Request );
			}
			else
			{
				NewAnswer->vItems[i].vValue.vInf.vRequest = "";
				NewAnswer->vItems[i].vValue.vInf.vRequestLength = 0;
			}
		}
		else if( cmd == itBr )
		{
			// Тэг переноса строки.

			NewAnswer->vItems[i].vType = AnswerItemTypeTagBr;
		}
		else if( cmd == itStartUList )
		{
			NewAnswer->vItems[i].vType = AnswerItemTypeStartUList;
		}
		else if( cmd == itStartOList )
		{
			NewAnswer->vItems[i].vType = AnswerItemTypeStartOList;
		}
		else if( cmd == itListItem )
		{
			NewAnswer->vItems[i].vType = AnswerItemTypeListItem;
		}
		else if( cmd == itEndList )
		{
			NewAnswer->vItems[i].vType = AnswerItemTypeEndList;
		}
		else if( cmd == itHref )
		{
			// Тэг ссылки.

			if( pos + 2 >= Response->attr_count )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}

			// Установка типа элемента.
			NewAnswer->vItems[i].vType = AnswerItemTypeTagHref;

			// Установка урла.
			const char * URL = NULL;
			if( IEP_GetString( Response, pos++, "url", &URL ) != INF_ENGINE_SUCCESS || ( NewAnswer->vItems[i].vValue.vHref.vURL = strdup( URL ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vHref.vURLLength = strlen( URL );

			// Установка параметра target ссылки.
			const char * Target = NULL;
			if( IEP_GetString( Response, pos++, "target", &Target ) != INF_ENGINE_SUCCESS ||
				( NewAnswer->vItems[i].vValue.vHref.vTarget = strdup( Target ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vHref.vTargetLength = strlen( Target );

			// Установка текста ссылки.
			const char * Link = NULL;
			if( IEP_GetString( Response, pos++, "link", &Link ) != INF_ENGINE_SUCCESS || ( NewAnswer->vItems[i].vValue.vHref.vLink = strdup( Link ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vHref.vLinkLength = strlen( Link );
		}
		else if( cmd == itInstruct )
		{
			// Инструкция на изменение сессии.

			if( pos + 1 >= Response->attr_count )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}

			// Установка типа элемента.
			NewAnswer->vItems[i].vType = AnswerItemTypeInstruct;

			// Установка имени переменной.
			const char * VarName = NULL;
			if( IEP_GetString( Response, pos++, "varname", &VarName ) != INF_ENGINE_SUCCESS ||
				( NewAnswer->vItems[i].vValue.vInstruct.vVarName = strdup( VarName ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vInstruct.vVarNameLength = strlen( VarName );

			// Установка присваевоемого значения.
			const char * VarValue = NULL;
			if( IEP_GetString( Response, pos++, "varvalue", &VarValue ) != INF_ENGINE_SUCCESS ||
				( NewAnswer->vItems[i].vValue.vInstruct.vValue = strdup( VarValue ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vInstruct.vValueLength = strlen( VarValue );
		}
		else if( cmd == itOpenWindow )
		{
			// Инструкция.

			if( pos + 1 >= Response->attr_count )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}

			// Установка типа элемента.
			NewAnswer->vItems[i].vType = AnswerItemTypeTagOpenWindow;

			// Установка URL'а.
			const char * URL = NULL;
			if( IEP_GetString( Response, pos++, "url", &URL ) != INF_ENGINE_SUCCESS ||
				( NewAnswer->vItems[i].vValue.vOpenWindow.vURL = strdup( URL ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vOpenWindow.vURLLength = strlen( URL );

			if( IEP_GetUInt32( Response, pos++, "target", &(NewAnswer->vItems[i].vValue.vOpenWindow.vParentWindow) ) != INF_ENGINE_SUCCESS )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
		}
		else if( cmd == itTagRSS )
		{
			// Тэг ссылки.

			if( pos + 5 >= Response->attr_count )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}

			// Установка типа элемента.
			NewAnswer->vItems[i].vType = AnswerItemTypeTagRSS;

			// Установка URL'а.
			const char * URL = NULL;
			if( IEP_GetString( Response, pos++, "url", &URL ) != INF_ENGINE_SUCCESS || ( NewAnswer->vItems[i].vValue.vRSS.vURL = strdup( URL ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vRSS.vURLLength = strlen( URL );

			// Установка Alt'а.
			const char * Alt;
			if( IEP_GetString( Response, pos++, "alt", &Alt ) != INF_ENGINE_SUCCESS || ( NewAnswer->vItems[i].vValue.vRSS.vAlt = strdup( Alt ) ) == NULL )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
			// @todo Нужно протянуть выборку длины из полученных данных.
			NewAnswer->vItems[i].vValue.vRSS.vAltLength = strlen( Alt );

			// Установка остальных аргументов.
			if( IEP_GetUInt32( Response, pos++, "offset", &( NewAnswer->vItems[i].vValue.vRSS.vOffset ) ) != INF_ENGINE_SUCCESS ||
				IEP_GetUInt32( Response, pos++, "show_title", &( NewAnswer->vItems[i].vValue.vRSS.vShowTitle ) ) != INF_ENGINE_SUCCESS ||
				IEP_GetUInt32( Response, pos++, "show_link", &( NewAnswer->vItems[i].vValue.vRSS.vShowLink ) ) != INF_ENGINE_SUCCESS ||
				IEP_GetUInt32( Response, pos++, "update_period", &( NewAnswer->vItems[i].vValue.vRSS.vUpdatePeriod ) ) != INF_ENGINE_SUCCESS )
			{
				// Освобождение памяти.
				cl_answer_free( NewAnswer );
				clear_response( );

				return clrcErrorInvData;
			}
		}
		else
		{
			// Освобождение памяти.
			cl_answer_free( NewAnswer );
			clear_response( );

			return clrcErrorInvData;
		}
	}

	*aAnswer = NewAnswer;

	clear_response( );

	return clrcSuccess;
}

ClientLibReturnCode cl_cmd_purge_inf( unsigned int aInfId, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag )
{
	// Проверка аргументов.
	if( !aConnectionString )
		return clrcErrorInvArgs;

	// Создание структуры.
	allocate_request( 3, INF_ENGINE_PROTOCOL_PURGE_INF );
	IEP_SetInfId( Request, 2, aInfId );

	// Запрос к серверу.
	make_request( aConnectionString, aTimeOut, aPackDataFlag ? CSDATAPACK_FULL : 0 );

	// Проверка количества элементов в ответе сервера.
	check_response_items_number( 2 );

	// Освобождение памяти.
	clear_response( );

	return clrcSuccess;
}

ClientLibReturnCode cl_cmd_send_inf( const InfData * aInfData, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag )
{
	// Проверка аргументов.
	if( !aInfData || !aConnectionString )
		return clrcErrorInvArgs;

	if( aInfData->vVarsNumber > 0 && !aInfData->vVars )
		return clrcErrorInvArgs;

	// Создание структуры.
	allocate_request( 6 + 2 * aInfData->vVarsNumber + 2 * aInfData->vDictsNumber, INF_ENGINE_PROTOCOL_SET_INF );
	IEP_SetInfId( Request, 2, aInfData->vInfId );
	IEP_SetString( Request, 3, "templates", aInfData->vTemplates );
	IEP_SetUInt32( Request, 4, "items", aInfData->vVarsNumber );
	IEP_SetUInt32( Request, 5, "dicts", aInfData->vDictsNumber );

	unsigned int i = 0, pos = 6;
	for( i = 0; i < aInfData->vVarsNumber; i++ )
	{
		IEP_SetString( Request, pos++, "name", aInfData->vVars[i].vName );
		IEP_SetString( Request, pos++, "value", aInfData->vVars[i].vValue );
	}

	for( i = 0; i < aInfData->vDictsNumber; i++ )
	{
		IEP_SetString( Request, pos++, "name", aInfData->vDicts[i].vName );
		IEP_SetString( Request, pos++, "value", aInfData->vDicts[i].vData );
	}

	// Запрос к серверу.
	make_request( aConnectionString, aTimeOut, aPackDataFlag ? CSDATAPACK_FULL : 0 );

	// Проверка количества элементов в ответе сервера.
	check_response_items_number( 2 );

	// Освобождение памяти.
	clear_response( );

	return clrcSuccess;
}
