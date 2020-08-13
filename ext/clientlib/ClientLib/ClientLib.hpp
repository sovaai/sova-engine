#ifndef __ClientLib_hpp__
#define __ClientLib_hpp__

#include "ClientLib.h"

/**
 *  Класс для коммуникации с сервером InfServer.
 */
class InfEngineClient
{
public:

	/**
	 *  Конструктор.
	 * @param aConnectionString - строка соединения. Формат строки соединения: tcp:host:2255 или unix:socket
	 * @param aTimeOut - значение таймаута в секундах.
	 * @param aPackDataFlag - флаг, упаковки данных при пересылке между клиентом и сервером InfServer.
	 */
	InfEngineClient( const char * aConnectionString = NULL, unsigned int aTimeOut = CLIENT_LIB_DEFAULT_TIMEOUT, bool aPackDataFlag = true )
	{
		vConnectionString = aConnectionString;
		vTimeOut = aTimeOut;
		vPackDataFlag = aPackDataFlag;
	}


public:

	/**
	 *  Установка строки соединения, описывающей тип и адрес для его установки.
	 * @param aConnectionString - строка соединения. Формат строки соединения: tcp:host:2255 или unix:socket
	 */
	void SetConnectData( const char * aConnectionString )
	{
		vConnectionString = aConnectionString;
	}

	/**
	 *  Установка значения таймаута.
	 * @param aTimeOut - значение таймаута в секундах.
	 */
	void SetTimeOut( unsigned int aTimeOut )
	{
		vTimeOut = aTimeOut;
	}

	/**
	 *  Установка значения флага упаковки данных при пересылке между клиентом и сервером.
	 * @param aPackDataFlag - флаг, упаковки данных при пересылке между клиентом и сервером InfServer.
	 */
	void SetPackDataFlag( bool aPackDataFlag )
	{
		vPackDataFlag = aPackDataFlag;
	}


public:

	/**
	 *  Запрос инициализации сервером сессии с идентификатором aSessionId для инфа aInfId. В aMissedDataMask помещается
	 * битовая маска недостающих компонентов или 0, если таковых нет.
	 * @param aInfId - идентификатор инфа.
	 * @param aSessionId - идентификатор сессии.
	 * @param aMissedDataMask - маска недостающих компонентов.
	 */
	ClientLibReturnCode InitSession( unsigned int aInfId, unsigned int aSessionId, unsigned int & aMissedDataMask ) const
	{
		return cl_cmd_init( aInfId, aSessionId, &aMissedDataMask, vConnectionString, vTimeOut, vPackDataFlag ? 1 : 0 );
	}

	/**
	 *  Удаление сессии с идентификатором SessionId из кеша сервера.
	 * @param aSessionId - идентификатор сессии.
	 */
	ClientLibReturnCode PurgeSession( unsigned int aSessionId )
	{
		return cl_cmd_purge_session( aSessionId, vConnectionString, vTimeOut, vPackDataFlag ? 1 : 0 );
	}

	/**
	 *  Обновление или добавление сессии в кэш сервера.
	 * @param aSessionData - данные сессии.
	 */
	ClientLibReturnCode SendSession( const SessionData & aSessionData ) const
	{
		return cl_cmd_send_session( &aSessionData, vConnectionString, vTimeOut, vPackDataFlag ? 1 : 0 );
	}

	/**
	 * Обновление данных сессии.
	 * @param aSessionData - изменения, которые нужно внести в данные сессии.
	 */
	ClientLibReturnCode UpdateSession( const SessionData & aSessionData ) const
	{
		return cl_cmd_update_session( &aSessionData, vConnectionString, vTimeOut, vPackDataFlag );
	}



public:

	/**
	 *  Запрос ответа сервера на запрос aRequest, относящийся к сессии с идентификатором aSessionId
	 * и инфу с идентификатором aInfId.
	 * @param aSessionId - идентификатор сессии.
	 * @param aInfId - идентификатор инфа.
	 * @param aRequest - реплика пользователя.
	 * @param aVarsList - список переменных, которые нужно обновить до ответа на запрос.
	 * @param aMissedDataMask - маска недостающих компонентов.
	 * @param aAnswer - ответ бэкэнда на вопрос пользователя. Память под ответ выделяется. Освобождать ее нужно cl_answer_free.
	 */
	ClientLibReturnCode SendRequest( unsigned int aSessionId, unsigned int aInfId, const char * aRequest, const SessionData & aVarsList, unsigned int & aMissedDataMask, Answer* & aAnswer ) const
	{
		return cl_cmd_request( aSessionId, aInfId, aRequest, &aVarsList, &aMissedDataMask, &aAnswer, vConnectionString, vTimeOut, vPackDataFlag ? 1 : 0 );
	}


public:

	/**
	 *  Удаление инфа из кэша бэкэнда.
	 * @param aInfId - идентификатор инфа.
	 */
	ClientLibReturnCode PurgeInf( unsigned int aInfId )
	{
		return cl_cmd_purge_inf( aInfId, vConnectionString, vTimeOut, vPackDataFlag ? 1 : 0 );
	}

	/**
	 *  Обновление или добавление в кэш сервера данных инфа.
	 * @param aInfData - данные инфа.
	 */
	ClientLibReturnCode SendInf( const InfData & aInfData ) const
	{
		return cl_cmd_send_inf( &aInfData, vConnectionString, vTimeOut, vPackDataFlag ? 1 : 0 );
	}


private:
	/** Сторка соединения с сервером. */
	const char * vConnectionString;

	/** Таймаут. */
	unsigned int vTimeOut;

	/** Флаг, упаковки данных при пересылке между клиентом и сервером InfServer. */
	bool vPackDataFlag;
};

#endif  /** __ClientLib_hpp__ */
