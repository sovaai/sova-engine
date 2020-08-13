#ifndef __InfEngineErrors_h__
#define __InfEngineErrors_h__

/**
 *   Коды возврата ошибок.
 */
typedef enum
{
	/** Успех. */
	INF_ENGINE_SUCCESS = 0,

	/** Неизвестная ошибка. **/
	INF_ENGINE_ERROR_FAULT = -200,
	/** Выход данных за допустимые границы. **/
	INF_ENGINE_ERROR_OUT_OF_RANGE = -201,
	/** Ошибка при работе с fstorage. **/
	INF_ENGINE_ERROR_FSTORAGE = -202,
	/** Ошибка работы с файлами. **/
	INF_ENGINE_ERROR_FILE = -203,
	/** Ошибка в данных. **/
	INF_ENGINE_ERROR_DATA = -204,
	/** Некорректное состояние объекта. **/
	INF_ENGINE_ERROR_STATE = -205,
	/** Функции были переданы некорректные аргументы. **/
	INF_ENGINE_ERROR_INV_ARGS = -206,
	/** Неверное количество аргументов при вызове функции. */
	INF_ENGINE_ERROR_ARGC = -207,
	/** Не найдена точка входа в динамической библиотеке. */
	INF_ENGINE_ERROR_ENTRY_POINT = -208,
	/** Ошибка работы с фнкцией в динамической библиотеке. */
	INF_ENGINE_ERROR_DLF = -209,
	/** Ошибка работы с фнкцией в динамической библиотеке. */
	INF_ENGINE_ERROR_INVALID_ARG = -210,
	/** Недопустимая версия функции. */
	INF_ENGINE_ERROR_FUNC_VERSION = -211,
	/** Исключить из рассмотрения шаблон-ответ, в котором функция вернула такой результат. */
	INF_ENGINE_ERROR_DLF_SKIP_ANSWER = -212,
	/** Недопустимая версия API функции. */
	INF_ENGINE_ERROR_FUNC_API_VERSION = -213,
	/** Низвестный алиас. */
	INF_ENGINE_ERROR_UNKNOWN_ALIAS = -214,
	/** Низвестный тэг, представленный как функция. */
	INF_ENGINE_ERROR_UNKNOWN_FUNCTIONALIKE_TAG = -215,
	/** Внтуренняя DL-функция не инициализирована. */
	INF_ENGINE_ERROR_FUNC_NOT_INITIALIZED = -216,

	/** Неудачная попытка, необходмо повторить процедуру. **/
	INF_ENGINE_AGAIN = 200,





	/** Внутренняя ошибка. **/
	INF_ENGINE_ERROR_INTERNAL = -101,



	/*  Ошибка работы с памятью - не могу выделить нужный объем памяти. */
	INF_ENGINE_ERROR_NOFREE_MEMORY = -6,
	/*  Ошибка открытия файла или другого объекта. */
	INF_ENGINE_ERROR_CANT_OPEN = -8,
	/* Обшика разбора. */
	INF_ENGINE_ERROR_CANT_PARSE = -9,
	/*  Некорректные данные. */
	INF_ENGINE_ERROR_INV_DATA = -10,
	/*  Ошибка операции создания объекта. */
	INF_ENGINE_ERROR_CANT_CREATE = -12,
	/*  Ошибка операции чтения. */
	INF_ENGINE_ERROR_CANT_READ = -13,
	/*  Ошибка операции записи. */
	INF_ENGINE_ERROR_CANT_WRITE = -14,
	/*  Ошибка при соединении или в процессе обмена данными. */
	INF_ENGINE_ERROR_CANT_CONNECT = -15,
	/*  Ошибка чексуммы. */
	INF_ENGINE_ERROR_CHECKSUMM = -16,
	/*  На входе - пустая строка. */
	INF_ENGINE_ERROR_EMPTY_STRING = -17,

	/*  Некорректный протокол. */
	INF_ENGINE_ERROR_INV_PROTOCOL = -100,

	/*  Фатальная ошибка сервера. */
	INF_ENGINE_ERROR_FATAL = -102,

	/* Ошибки нет, но операция не успешна. */
	INF_ENGINE_WARN_UNSUCCESS = 1,

	/*  Ошибка библиотеки NanoLib */
	INF_ENGINE_ERROR_NANOLIB = -102,
	/*  Достигнут лимит на количест-во элементов. */
	INF_ENGINE_ERROR_LIMIT = -103,
	/**/
	INF_ENGINE_ERROR_EMPTY = -104,
	/*  Не получается добавить элемент в хэш. */
	INF_ENGINE_ERROR_INSERT = -105,

	/*  Ошибка при попытке добавить термин. */
	INF_ENGINE_ERROR_ADD_TERM = -107,

	// Ошибка работы с кэшем.
	INF_ENGINE_ERROR_CACHE = -108,




	/*  Предупреждение библиотеки NanoLib */
	INF_ENGINE_WARN_NANOLIB = 102,

	/*  Предупреждение библиотеки NanoLib */
	INF_ENGINE_WARN_SYNONYM_DUPLICATE = 103,

	INF_ENGINE_WARN_ELEMENT_EXISTS = 104, /** Элемент существует. */

	INF_ENGINE_WARN_ELEMENT_DOESNT_EXIST = 105, /** Элемент не существует. */

	INF_ENGINE_WARN_BAD_FORMAT = 106, /** Неверный формат данных. */


	/** Ошибка таймаута. */
	INF_ENGINE_ERROR_TIMEOUT = -300,

	/** Ошибка компиляции шаблонов. */
	INF_ENGINE_ERROR_PATTERN_COMPILATION = -301,

	/** Процесс завершился падением. */
	INF_ENGINE_SERVER_CRASHED = -303,

	/** Процесс сервера получил сигналл KILL. */
	INF_ENGINE_ERROR_SERVER_KILLED = -302,

} InfEngineErrors;
#endif // __InfEngineErrors_h__


#define INF_ENGINE_STRING_ERROR_INTERNAL "Internal error."

#define INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS "Internal error. Invalid arguments."

#define INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY "Can't allocate memory."

#define INF_ENGINE_STRING_ERROR_INVALID_DATA "Internal error. Invalid data."

#define INF_ENGINE_STRING_ERROR_RECREATE "Internal error. Trying recreate object, that has been created already."
#define INF_ENGINE_STRING_ERROR_REINIT   "Internal error. Trying reinitialize object, that has been initialized already."
#define INF_ENGINE_STRING_ERROR_STATE   "Internal error. Object has incorrect status."


#define INF_ENGINE_STRING_ERROR_LOW_SAVE_BUFFER_SIZE                "Internal error. Size of buffer is not enough for saving object."
#define INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_SAVING_RESULT     "Internal error. Incorrect size of saving result."
#define INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_LOADING_RESULT    "Internal error. Incorrect size of loading result."

#define INF_ENGINE_STRING_ERROR_FAILED_LOAD_DL     "Internal error. Loading dynamic library failed."
#define INF_ENGINE_STRING_ERROR_FAILED_FOUND_ENTRY_POINT     "Internal error. Binding dynamic library failed."
#define INF_ENGINE_STRING_ERROR_DLF     "Internal error. Dynamic library function error."
