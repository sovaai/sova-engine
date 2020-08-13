#ifndef __ClientLib_h__
#define __ClientLib_h__

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      Константы клиентской библиотеки InfEngine.
 *
 * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

/** Таймаут по умолчанию. В милисекундах. */
	#define CLIENT_LIB_DEFAULT_TIMEOUT 10000

/** Битовые маски, используемые в запросах к серверу и его ответах. */

/** Маска, сигнализирующая об отсутствии инфа в кэше сервера InfServer. */
	#define CLIENT_LIB_INF_MASK_INF     0x01
/** Маска, сигнализирующая об отсутствии сессии в кэше сервера InfServer. */
	#define CLIENT_LIB_INF_MASK_SESSION 0x02



/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      Информационные функции и константы.
 *
 * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

/** Версия библотеки. */
#define CLIENT_LIB_VERSION  "1.9"

/** Получение версии протокола передачи данных серверу, используемого библиотекой. */
unsigned int GetProtocolVersion();

/** Получение размера памяти, выделенной библиотекой ClientLib для внутреннего использования. */
unsigned int GetClientLibAllocatedMemorySize();



/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      Коды возврата функций и функции для их интерпретации.
 *
 * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

/**
 *  Коды возврата библиотечных функций библиотеки ClientLib.
 */
typedef enum
{
	/** Успех. */
	clrcSuccess             = 0,

	/** Функции были переданы некорректные аргументы. */
	clrcErrorInvArgs        = -1,

	/** Ошибка работы с памятью, не получается выделить нужный объем памяти. */
	clrcErrorNoFreeMemory   = -2,

	/** Неподдерживаемая версия протокола. */
	clrcErrorInvProtocol    = -3,

	/** Некорректные внутренние данные. */
	clrcErrorInvData        = -4,

	/** Неизвестная ошибка. */
	clrcErrorFault          = -5,

	/** Ошибка сервера, приведшая к его неработаспособности. */
	clrcErrorServerFatal    = -6,

	/** Ошибка компиляции данных инфа. **/
	clrcErrorInfCompilation = -7,

	/** Ошибка при соединении или в процессе обмена данными. **/
	clrcErrorConnection     = -8,

	/** Таймаут соединения с сервером. */
	clrcErrorTimeOut        = -9,

	/** Падение сервера. */
	clrcErrorServerCrashed  = -10,

	/** Сервер был убит сигналом KILL. */
	clrcErrorServerKilled   = -11
} ClientLibReturnCode;

/**
 *  Получение описания кода возврата.
 * @param aClientLibReturnCode - код возврата.
 */
const char * ClientLibReturnCodeToString( ClientLibReturnCode aClientLibReturnCode );



/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      Данные сессии и функции для работы с ней.
 *
 * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

/** Структура, описывающая сессию. */
typedef struct __SessionData SessionData;

/** Создание данных сессии. */
SessionData * cl_session_create();

/**
 *  Выделение памяти под переменные сессии. Все переменные, при этом действии, обнуляются.
 * @param aSessionData - данные сессии.
 * @param aVarsNumber - количество переменных.
 */
ClientLibReturnCode cl_session_resize( SessionData * aSessionData, unsigned int aVarsNumber );

/**
 *  Установка имени и значения aVarInd-ой переменной. Данные не копируются.
 * @param aSessionData - данные сессии.
 * @param aVarInd - номер переменной.
 * @param aVarName - имя переменной.
 * @param aVarValue - значение переменной.
 */
void cl_session_set_var( SessionData * aSessionData, unsigned int aVarInd, const char * aVarName, const char * aVarValue );

/**
 *  Установка идентификатора сессии.
 * @param aSessionData - данные сессии.
 * @param aSessionId - идентификатор сессии.
 */
void cl_session_set_id( SessionData * aSessionData, unsigned int aSessionId );

/**
 *  Установка идентификатор инфа, связанного с сессиией.
 * @param aSessionData - данные сессии.
 * @param aInfId - идентификатор инфа.
 */
void cl_session_set_inf_id( SessionData * aSessionData, unsigned int aInfId );

/**
 *  Получение числа переменных в сессии.
 * @param aSessionData - данные сессии.
 */
unsigned int cl_session_get_size( const SessionData * aSession );

/**
 *  Получение имени и значения aVarInd-ой переменной.
 * @param aSessionData - данные сессии.
 * @param aVarInd - номер переменной.
 * @param aVarName - имя переменной.
 * @param aVarValue - значение переменной.
 **/
ClientLibReturnCode cl_session_get( const SessionData * aSessionData, unsigned int aVarInd, const char ** aVarName, const char ** aVarValue );

/**
 *  Освобождение памяти выделенной под переменные сессии.
 * @param aSessionData - данные сессии.
 */
void cl_session_free( SessionData * aSessionData );



/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      Данные инфа и функции для работы с ними.
 *
 * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

/** Структура описывающая данные инфа. */
typedef struct __InfData InfData;

/** Создание профиля данных инфа. */
InfData * cl_inf_create();

/**
 *  Выделение памяти под переменные инфа. Все переменные, при этом действии, обнуляются.
 * @param aInfData - данные инфа.
 * @param aVarsNumber - количество переменных.
 * @param aDictsNumber - количество словарей.
 */
ClientLibReturnCode cl_inf_resize( InfData * aInfData, unsigned int aVarsNumber, unsigned int aDictsNumber );

/**
 *  Установка имени и значения переменной с номером aVarInd. Данные не копируются.
 * @param aInfData - данные инфа.
 * @param aVarInd - номер переменной.
 * @param aVarName - имя переменной.
 * @param aVarValue - значение переменной.
 */
void cl_inf_set_var( InfData * aInfData, unsigned int aVarInd, const char * aVarName, const char * aVarValue );

/**
 *  Установка имени и тела словаря с номером aVarInd. Данные не копируются.
 * @param aInfData - данные инфа.
 * @param aDictInd - номер словаря.
 * @param aDictName - имя словаря.
 * @param aDict - тело стоваря.
 */
void cl_inf_set_dict( InfData * aInfData, unsigned int aDictInd, const char * aDictName, const char * aDict );

/**
 *  Установка идентификатора инфа.
 * @param aInfData - данные инфа.
 * @param aInfId - идентификатор инфа.
 */
void cl_inf_set_id( InfData * aInfData, unsigned int aInfId );

/**
 *  Установка шаблонов инфа.
 * @param aInfData - данные инфа.
 * @param aTemplates - шаблоны инфа.
 */
void cl_inf_set_templates( InfData * aInfData, const char * aTemplates );

/**
 *  Получение числа переменных в данных инфа.
 * @param aInfData - данные инфа.
 */
unsigned int cl_inf_get_vars_cnt( const InfData * aInfData );

/**
 *  Получение числа словарей в данных инфа.
 * @param aInfData - данные инфа.
 */
unsigned int cl_inf_get_dicts_cnt( const InfData * aInfData );

/**
 *  Получение имени и значения переменной с номером aVarInd.
 * @param aInfData - данные инфа.
 * @param aVarInd - номер переменной.
 * @param aVarName - имя переменной.
 * @param aVarValue - значение переменной.
 */
ClientLibReturnCode cl_inf_get_var( const InfData * aInfData, unsigned int aVarInd, const char ** aVarName, const char ** aVarValue );

/**
 *  Получение имени и тела словаря с номером aDictInd.
 * @param aInfData - данные инфа.
 * @param aVarInd - номер словаря.
 * @param aVarName - имя словаря.
 * @param aVarValue - тело словаря.
 */
ClientLibReturnCode cl_inf_get_dict( const InfData * aInfData, unsigned int aDictInd, const char ** aDictName, const char ** aDicText );

/**
 *  Освобождение памяти.
 * @param aInfData - данные инфа.
 */
void cl_inf_free( InfData * aInfData );



/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      Ответ сервера на запрос REQUEST и функции для работы с ним.
 *
 * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

/** Тип элемента ответа сервера. */
typedef enum
{
	/** Текстовая строка. */
	AnswerItemTypeTextString    = 1,

	/** Тэг inf, обозначающий кликабельный ответ инфу. */
	AnswerItemTypeTagInf        = 2,

	/** Тэг br, обозначающий перенос строки. */
	AnswerItemTypeTagBr         = 3,

	/** Тэг href, обозначающий ссылку. */
	AnswerItemTypeTagHref       = 4,

	/** Инструкция на изменение сессии. */
	AnswerItemTypeInstruct      = 5,

	/** Тэг open_window, обозначающий команду открытия ссылки в окне браузера. */
	AnswerItemTypeTagOpenWindow = 6,

	/** Тэг запроса RSS. */
	AnswerItemTypeTagRSS        = 7,

	/** Начало ненумерованного форматированного списка. */
	AnswerItemTypeStartUList 	= 8,

	/** Начало нумерованного форматированного списка. */
	AnswerItemTypeStartOList	= 9,

	/** Начало элемента форматированного списка. */
	AnswerItemTypeListItem  	= 10,

	/** Конец форматированного списка. */
	AnswerItemTypeEndList    	= 11,

} AnswerItemType;

/** Ответ сервера. */
typedef struct __Answer Answer;

/**
 *  Освобождение памяти, занимаемой ответом сервера.
 * @param aAnswer - ответ сервера.
 */
void cl_answer_free( Answer * aAnswer );

/**
 *  Получение количества элементов в ответе сервера.
 * @param aAnswer - ответ сервера.
 */
unsigned int cl_answer_get_size( const Answer * aAnswer );

/**
 *  Получение типа aItemInd-ого элемента ответа сервера.
 * @param aAnswer - ответ сервера.
 * @param aItemInd - номер элемента.
 * @param aType - тип элемента ответа.
 */
ClientLibReturnCode cl_answer_get_type( const Answer * aAnswer, unsigned int aItemInd, AnswerItemType * aType );

/**
 *  Получение данных aItemInd-ого элемента как текстовой строки.
 * @param aAnswer - ответ сервера.
 * @param aItemInd - номер элемента.
 * @param aTextString - значение текстовой строки.
 * @param aTextStringLength - длина текстовой строки.
 */
ClientLibReturnCode cl_answer_get_text( const Answer * aAnswer, unsigned int aItemInd, const char ** aTextString, unsigned int * aTextStringLength );

/**
 *  Получение данных aItemInd-ого элемента как тэга запроса к инфу( tag inf ).
 * @param aAnswer - ответ сервера.
 * @param aItemInd - номер элемента.
 * @param aValue - текст ссылки.
 * @param aValueLength - длина запроса к инфу.
 * @param aRequest - запрос к движку.
 * @param aRequestLength - длина запроса к движку.
 */
ClientLibReturnCode cl_answer_get_tag_inf( const Answer * aAnswer, unsigned int aItemInd, const char ** aValue, unsigned int * aValueLength, const char ** aRequest, unsigned int * aRequestLength );

/**
 *  Получение данных aItemInd-ого элемента как тэга ссылки ( tag href ).
 * @param aAnswer - ответ сервера.
 * @param aItemInd - номер элемента.
 * @param aURL - URL.
 * @param aURLLength - длина URL'а.
 * @param aTarget - target ссылки.
 * @param aTargetLength - длина target'а ссылки.
 * @param aLink - текст ссылки.
 * @param aLinkLength - длина текста ссылки.
 */
ClientLibReturnCode cl_answer_get_tag_href( const Answer * aAnswer, unsigned int aItemInd, const char ** aURL, unsigned int * aURLLength,
											const char ** aTarget, unsigned int * aTargetLength, const char ** aLink, unsigned int * aLinkLength );

/**
 *  Получение данных aItemInd-ого элемента как инструкции к изменению сессии.
 * @param aAnswer - ответ сервера.
 * @param aItemInd - номер элемента.
 * @param aVarName - имя переменной.
 * @param aVarNameLength - длина имени переменной.
 * @param aValue - значение переменной.
 * @param aValueLength - длина значения переменной.
 */
ClientLibReturnCode cl_answer_get_instruct( const Answer * aAnswer, unsigned int aItemInd, const char ** aVarName, unsigned int * aVarNameLength,
											const char ** aValue, unsigned int * aValueLength );

/**
 *  Получение данных aItemInd-ого элемента как тэга открытия ссылки в окне браузера.
 * @param aAnswer - указатель на результат запроса к инфу.
 * @param aItemInd - номер требуемого элемента ответа инфа.
 * @param aURL - URL.
 * @param aURLLength - длина URL'а.
 * @param aTarget - идентификатор окна, в котором нужно открыть ссылку. ( New - 0; Parent - 1 )
 */
ClientLibReturnCode cl_answer_get_tag_open_window( const Answer * aAnswer, unsigned int aItemInd, const char ** aURL, unsigned int * aURLLength,
												   unsigned int * aTarget );

/**
 *  Получение данных aItemInd-ого элемента как тэга запроса RSS.
 * @param aAnswer - указатель на результат запроса к инфу.
 * @param aItemInd - номер требуемого элемента ответа инфа.
 * @param aURL - URL RSS'а.
 * @param aURLLength - длина URL'а.
 * @param aAlt - Текст, показываемый при недоступности RSS.
 * @param aAltLength - длина текста, показываемого при недоступности RSS.
 * @param aOffset - Номер RSS записи.
 * @param aShowTitle - Флаг показа заголовка RSS.
 * @param aShowLink - Флаг показа ссылки на RSS.
 * @param aUpdatePeriod - Частота обновления RSS.
 */
ClientLibReturnCode cl_answer_get_tag_rss( const Answer * aAnswer, unsigned int aItemInd, const char ** aURL, unsigned int * aURLLength,
										   const char ** aAlt, unsigned int * aAltLength, unsigned int * aOffset, unsigned int * aShowTitle,
										   unsigned int * aShowLink, unsigned int * aUpdatePeriod );



/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      Функции для коммуникации с сервером InfServer.
 *
 * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

/**
 *  Запрос инициализации сервером сессии с идентификатором aSessionId для инфа aInfId. В aMissedDataMask помещается
 * битовая маска недостающих компонентов или 0, если таковых нет.
 * @param aInfId - идентификатор инфа.
 * @param aSessionId - идентификатор сессии.
 * @param aMissedDataMask - маска недостающих компонентов.
 * @param aConnectionString - строка соединения. Формат строки соединения: tcp:host:2255 или unix:socket
 * @param aTimeOut - значение таймаута в секундах.
 * @param aPackDataFlag - флаг, упаковки данных при пересылке между клиентом и сервером InfServer.
 */
ClientLibReturnCode cl_cmd_init( unsigned int aInfId, unsigned int aSessionId, unsigned int * aMissedDataMask,
								 const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag );

/**
 *  Удаление сессии с идентификатором SessionId из кеша сервера.
 * @param aSessionId - идентификатор сессии.
 * @param aConnectionString - строка соединения. Формат строки соединения: tcp:host:2255 или unix:socket
 * @param aTimeOut - значение таймаута в секундах.
 * @param aPackDataFlag - флаг, упаковки данных при пересылке между клиентом и сервером InfServer.
 */
ClientLibReturnCode cl_cmd_purge_session( unsigned int aSessionId, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag );

/**
 *  Обновление или добавление сессии в кэш сервера.
 * @param aSessionData - данные сессии.
 * @param aConnectionString - строка соединения. Формат строки соединения: tcp:host:2255 или unix:socket
 * @param aTimeOut - значение таймаута в секундах.
 * @param aPackDataFlag - флаг, упаковки данных при пересылке между клиентом и сервером InfServer.
 */
ClientLibReturnCode cl_cmd_send_session( const SessionData * aSessionData, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag );

/**
 *  Обновление значений переменных сессии в кэшэ сервера.
 * @param aSessionData - данные сессии.
 * @param aConnectionString - строка соединения. Формат строки соединения: tcp:host:2255 или unix:socket
 * @param aTimeOut - значение таймаута в секундах.
 * @param aPackDataFlag - флаг, упаковки данных при пересылке между клиентом и сервером InfServer.
 */
ClientLibReturnCode cl_cmd_update_session( const SessionData * aSessionData, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag );

/**
 *  Запрос ответа сервера на запрос aRequest, относящийся к сессии с идентификатором aSessionId
 * и инфу с идентификатором aInfId.
 * @param aSessionId - идентификатор сессии.
 * @param aInfId - идентификатор инфа.
 * @param aRequest - реплика пользователя.
 * @param aMissedDataMask - маска недостающих компонентов.
 * @param aAnswer - ответ бэкэнда на вопрос пользователя. Память под ответ выделяется. Освобождать ее нужно  cl_answer_free.
 * @param aConnectionString - строка соединения. Формат строки соединения: tcp:host:2255 или unix:socket
 * @param aTimeOut - значение таймаута в секундах.
 * @param aPackDataFlag - флаг, упаковки данных при пересылке между клиентом и сервером InfServer.
 */
ClientLibReturnCode cl_cmd_request( unsigned int aSessionId, unsigned int aInfId, const char * aRequest, const SessionData * aVarsList, unsigned int * aMissedDataMask, Answer ** aAnswer,
									const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag );

/**
 *  Удаление инфа из кэша бэкэнда.
 * @param aInfId - идентификатор инфа.
 * @param aConnectionString - строка соединения. Формат строки соединения: tcp:host:2255 или unix:socket
 * @param aTimeOut - значение таймаута в секундах.
 * @param aPackDataFlag - флаг, упаковки данных при пересылке между клиентом и сервером InfServer.
 */
ClientLibReturnCode cl_cmd_purge_inf( unsigned int aInfId, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag );

/**
 *  Обновление или добавление в кэш сервера данных инфа.
 * @param aInfData - данные инфа.
 * @param aConnectionString - строка соединения. Формат строки соединения: tcp:host:2255 или unix:socket
 * @param aTimeOut - значение таймаута в секундах.
 * @param aPackDataFlag - флаг, упаковки данных при пересылке между клиентом и сервером InfServer.
 */
ClientLibReturnCode cl_cmd_send_inf( const InfData * aInfData, const char * aConnectionString, unsigned int aTimeOut, int aPackDataFlag );

#ifdef __cplusplus
}
#endif /** __cplusplus */

#endif /** __ClientLib_h__ */
