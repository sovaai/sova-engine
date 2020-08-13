#ifndef __InfEngineProtocol_hpp__
#define __InfEngineProtocol_hpp__

#include <string.h>

#ifndef CSDATA_EXTENDED
	#define CSDATA_EXTENDED
#endif /** CSDATA_EXTENDED */

#include "Errors.h"

#define PEDANTIC_PROTOCOL

#include "CSData.hpp"

/** Текущая версия протокола. */
const int INF_ENGINE_PROTOCOL_VERSION =  6;

/** Минимальная версия, совместимая с текущей. */
const int INF_ENGINE_MIN_COMPATIBLE_PROTOCOL_VERSION = 4;

/** Количество команд в протоколе. */
const int INF_ENGINE_COMMANDS_NUMBER = 11;

/** Команды протокола. */
typedef enum InfEngineProtocol
{
	INF_ENGINE_PROTOCOL_INIT                            =  1,
	INF_ENGINE_PROTOCOL_PURGE_SESSION                   =  2,
	INF_ENGINE_PROTOCOL_SET_SESSION                     =  3,
	INF_ENGINE_PROTOCOL_REQUEST                         =  4,
	INF_ENGINE_PROTOCOL_PURGE_INF                       =  5,
	INF_ENGINE_PROTOCOL_SET_INF                         =  6,
	INF_ENGINE_PROTOCOL_GET_CACHED_SESSIONS_COUNT       =  7,
	INF_ENGINE_PROTOCOL_GET_SESSIONS_IDS                =  8,
	INF_ENGINE_PROTOCOL_GET_CACHED_SESSION_DATA         =  9,
	INF_ENGINE_PROTOCOL_GET_CACHED_SESSIONS_ALL_DATA    = 10,
	INF_ENGINE_PROTOCOL_UPDATE_SESSION                  = 11
} InfEngineProtocol;

/**
 *  Проверка версии протокола.
 * @param aProtocolVersion - версия протокола.
 * @return 1, в случае положительной проверки, иначе 0.
 */
inline int CheckProtocolVersion( unsigned int aProtocolVersion )
{
	return INF_ENGINE_MIN_COMPATIBLE_PROTOCOL_VERSION <= aProtocolVersion && aProtocolVersion <= INF_ENGINE_PROTOCOL_VERSION ? 1 : 0;
}

/**
 *  Получение значение поля типа int.
 *
 * @param aData - указатель на пакет.
 * @param aPos - позиция элемента.
 * @param aName - имя для проверки названия элемента.
 * @param aResult - указатель на результат.
 *
 * @retval INF_ENGINE_SUCCESS, INF_ENGINE_ERROR_INV_DATA
 */
inline InfEngineErrors IEP_GetInt( csdata_data_t * aData, int aPos, const char * aName, int * aResult )
{
	if( aData->attr_count < (unsigned int)aPos + 1 )
		return INF_ENGINE_ERROR_INV_DATA;

	if( !( aData->attr_list[aPos].attr_type & CSDATA_INT ) )
		return INF_ENGINE_ERROR_INV_DATA;

#ifdef PEDANTIC_PROTOCOL
	if( aName && strcmp( aData->attr_list[aPos].attr_name, aName ) )
		return INF_ENGINE_ERROR_INV_DATA;
#endif

	*aResult = aData->attr_list[aPos].attr_value.attr_int;

	return INF_ENGINE_SUCCESS;
}

inline InfEngineErrors IEP_GetUInt32( csdata_data_t * aData, int aPos, const char * aName, uint32_t * aResult )
{
	if( aData->attr_count < (unsigned int)aPos + 1 )
		return INF_ENGINE_ERROR_INV_DATA;

	if( !( aData->attr_list[aPos].attr_type & CSDATA_INT ) )
		return INF_ENGINE_ERROR_INV_DATA;

#ifdef PEDANTIC_PROTOCOL
	if( aName && strcmp( aData->attr_list[aPos].attr_name, aName ) )
		return INF_ENGINE_ERROR_INV_DATA;
#endif

	// Копирование без каста.
	memcpy( aResult, &( aData->attr_list[aPos].attr_value.attr_int ), sizeof( uint32_t ) );

	return INF_ENGINE_SUCCESS;
}

/**
 *  Получение значение поля типа string.
 *
 * @param aData - указатель на пакет.
 * @param aPos - позиция элемента.
 * @param aName - имя для проверки названия элемента.
 * @param aResult - указатель на результат.
 *
 * @retval INF_ENGINE_SUCCESS, INF_ENGINE_ERROR_INV_DATA
 */
inline InfEngineErrors IEP_GetString( csdata_data_t * aData, int aPos, const char * aName, const char ** aResult )
{
	if( aData->attr_count < (unsigned int)aPos + 1 )
		return INF_ENGINE_ERROR_INV_DATA;

	if( !( aData->attr_list[aPos].attr_type & CSDATA_STRING ) )
		return INF_ENGINE_ERROR_INV_DATA;

#ifdef PEDANTIC_PROTOCOL
	if( aName && strcmp( aData->attr_list[aPos].attr_name, aName ) )
		return INF_ENGINE_ERROR_INV_DATA;
#endif

	*aResult = aData->attr_list[aPos].attr_value.attr_string;

	return INF_ENGINE_SUCCESS;
}

/**
 *  Получение версии протокола.
 *
 * @param aData - указатель на пакет.
 * @param aResult - указатель на результат.
 *
 * @retval INF_ENGINE_SUCCESS, INF_ENGINE_ERROR_INV_DATA
 */
inline InfEngineErrors IEP_GetProtocol( csdata_data_t * aData, int * aResult )
{
	return IEP_GetInt( aData, 0, "protocol", aResult );
}

/**
 *  Получение статуса.
 *
 * @param aData - указатель на пакет.
 * @param aResult - указатель на результат.
 *
 * @retval: INF_ENGINE_SUCCESS, INF_ENGINE_ERROR_INV_DATA
 */
inline InfEngineErrors IEP_GetStatus( csdata_data_t * aData, int * aResult )
{
	return IEP_GetInt( aData, 1, "status", aResult );
}

/**
 *   Получение команды.
 *
 * @param aData - указатель на пакет.
 * @param aResult - указатель на результат.
 *
 * @retval: INF_ENGINE_SUCCESS, INF_ENGINE_ERROR_INV_DATA
 */
inline InfEngineErrors IEP_GetCmd( csdata_data_t * aData, InfEngineProtocol * aResult )
{
	int tmp = 0;
	InfEngineErrors iee = IEP_GetInt( aData, 1, "cmd", &tmp );
	*aResult = (InfEngineProtocol)tmp;
	return iee;
}

/**
 *  Получение идентификатора инфа.
 *
 * @param aData - указатель на пакет.
 * @param aResult - указатель на результат.
 *
 * @retval INF_ENGINE_SUCCESS, INF_ENGINE_ERROR_INV_DATA
 */
inline InfEngineErrors IEP_GetInfId( csdata_data_t * aData, int aPos, uint32_t * aResult )
{
	return IEP_GetUInt32( aData, aPos, "infid", aResult );
}

/**
 *  Получение идентификатора сессии.
 *
 * @param aData - указатель на пакет.
 * @param aResult - указатель на результат.
 *
 * @retval INF_ENGINE_SUCCESS, INF_ENGINE_ERROR_INV_DATA
 */
inline InfEngineErrors IEP_GetSessionId( csdata_data_t * aData, int aPos, uint32_t * aResult )
{
	return IEP_GetUInt32( aData, aPos, "sessionid", aResult );
}

inline void IEP_SetInt( csdata_data_t * aData, int aPos, const char * aName, int aValue )
{
	aData->attr_list[aPos].attr_name = (char*)aName;
	aData->attr_list[aPos].attr_name_size = aName ? strlen( aName ) : 0;
	aData->attr_list[aPos].attr_type = (CSDATA_DATA_TYPES)( CSDATA_INT | CSDATA_KEEP_ANAME );
	aData->attr_list[aPos].attr_value.attr_int = aValue;
	aData->attr_list[aPos].attr_value_size = sizeof( uint32_t );
}

inline void IEP_SetUInt32( csdata_data_t * aData, int aPos, const char * aName, uint32_t aValue )
{
	int tmp;

	// Копируем данные каста.
	memcpy( &tmp, &aValue, sizeof( uint32_t ) );

	aData->attr_list[aPos].attr_name = (char*)aName;
	aData->attr_list[aPos].attr_name_size = aName ? strlen( aName ) : 0;
	aData->attr_list[aPos].attr_type = (CSDATA_DATA_TYPES)( CSDATA_INT | CSDATA_KEEP_ANAME );
	aData->attr_list[aPos].attr_value.attr_int = tmp;
	aData->attr_list[aPos].attr_value_size = sizeof( uint32_t );
}

inline void IEP_SetString( csdata_data_t * aData, int aPos, const char * aName, const char * aValue )
{
	if( aName )
	{
		aData->attr_list[aPos].attr_name = (char*)aName;
		aData->attr_list[aPos].attr_name_size = strlen( aName );
	}
	else
	{
		aData->attr_list[aPos].attr_name = (char*)"";
		aData->attr_list[aPos].attr_name_size = 0;
	}
	aData->attr_list[aPos].attr_type = (CSDATA_DATA_TYPES)( CSDATA_STRING | CSDATA_KEEP_ANAME | CSDATA_KEEP_AVALUE );
	aData->attr_list[aPos].attr_value.attr_string = (char*)aValue;
	aData->attr_list[aPos].attr_value_size = aValue ? strlen( aValue ) : 0;
}

/*
 *   Проверка на ошибку.
 */


/*
 *   Передача версии протокола.
 */
inline void IEP_SetProtocol( csdata_data_t * aData, unsigned int aProtocolVersion )
{
	IEP_SetInt( aData, 0, "protocol", aProtocolVersion );
}
inline void IEP_SetCmd( csdata_data_t * aData, InfEngineProtocol aCmd )
{
	IEP_SetInt( aData, 1, "cmd", aCmd );
}
inline void IEP_SetInfId( csdata_data_t * aData, int aPos, uint32_t aInfId )
{
	IEP_SetUInt32( aData, aPos, "infid", aInfId );
}
inline void IEP_SetSessionId( csdata_data_t * aData, int aPos, uint32_t aSession )
{
	IEP_SetUInt32( aData, aPos, "sessionid", aSession );
}
inline void IEP_SetStatus( csdata_data_t * aData, int aStatus )
{
	IEP_SetInt( aData, 1, "status", aStatus );
}

#endif /** __InfEngineProtocol_hpp__ */
