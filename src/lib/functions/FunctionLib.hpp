#ifndef __InfEngineFunctionLib_h__
#define __InfEngineFunctionLib_h__

#include <sys/types.h>

#include <NanoLib/nMemoryAllocator.hpp>

#include <InfEngine2/_Include/Errors.h>
#include <InfEngine2/Common.hpp>

#include <InfEngine2/InfEngine/Session.hpp>

#include <cstring>

#ifdef __cplusplus
extern "C"
{
#endif

#define DLF_ENTRY_POINT "GetDLFucntionInfo"
#define CHECK_ENTRY_POINT "CheckDLFucntion"


// Если есть это объявление, то при сборке в библиотеки включается код для автотестирования.
#define COMPILE_WITH_AUTOTEST_MODE


/**
 *  Тип реализации функции.
 */
enum DLFunctionType
{
	DLFT_UNKNOWN = 0,

	/** Функция реализована внутри движка и может использовать его механизмы. */
	DLFT_INTERNAL = 1,

	/**
	 * Функция реализована вне движка и не может использовать его механизмы.
	 * На текущий момент такие функции реализуются в виде динамических библиотек.
	 */
	DLFT_EXTERNAL = 2,
};

/**
 *  Тип аргумента функции.
 */
enum DLFunctionArgType
{
	/** Текстовый. */
	DLFAT_TEXT = 0,

	/** Динамический (текст, звёздочки, референции, переменные). */
	DLFAT_DYNAMIC = 1,

	/** Имя переменной. */
	DLFAT_VARIABLE = 2,

	/** Расширенный (текст, звёздочки, референции, переменные, вызов функций). */
	DLFAT_EXTENDED = 3,

	/** Данные любого типа, кроме переменных, переданных по ссылке, и интерфейсных тегов. */
	DLFAT_ANY = 4,

	/** Данные любого типа, кроме переменных, переданных по ссылке. */
	DLFAT_IF = 5,

	/** Логический. Принимает только логические функции и выражающийся через них синтаксический сахар.*/
	DLFAT_BOOL = 6,
};

/**
 *  Тип возвращаемого функией значения.
 */
enum DLFunctionResType
{
	/** Текстовое значение */
	DLFRT_TEXT,

	/** Логическое значение */
	DLFRT_BOOL,
};

/**
 *  Аттрибуты аргументов функций.
 */
enum DLFunctionArgAttrs
{
	/** Фсе флаги опущены. */
	DLFAA_NONE  = 0,

	/** Запоминать пробельный символ в начале аргумента. */
	DLFAA_LEFT_SPACE  = 1,

	/** Запоминать пробельный символ в конце аргумента. */
	DLFAA_RIGHT_SPACE = 2,

	/** Запоминать пробельный символ в начале и в конце аргумента. */
	DLFAA_BOTH_SPACE  = 3,
};

/**
 *  Описание аргумента функции.
 */
struct DLFucntionArgInfo
{
	/** Тип аргумента функции. */
	DLFunctionArgType type;

	/** Значение по умолчанию или nullptr, если аргумент обязательный. */
	const char * default_value;

	/** Атрибуты аргумента */
	DLFunctionArgAttrs attrs;
};


/**
 *  Версия функции.
 */
struct DLFunctionVersion
{
	/** Версия функции (увеличивается, если изменятся определение функции). */
	int32_t a;

	/** Версия набора аргументов (увеличивается, если изменяется сигнатура функции). */
	int32_t b;

	/** Номер сборки (увеличивается при каждой сборке динамической библиотеки). */
	int32_t c;

	/** Представлене версии в виде строки. **/
	void ToString( char * str ) const
	{
		sprintf( str, "%i.%i.%i", a, b, c );
	}

	/** Загрузка структуры из строки. **/
	bool FromString( const char * str )
	{
		return sscanf( str, "%i.%i.%i", &a, &b, &c ) == 3;
	}

	/** Сравнение версий **/
	inline bool operator<( const DLFunctionVersion & aDLFunctionVersion ) const
	{
		if( a == aDLFunctionVersion.a )
		{
			if( b == aDLFunctionVersion.b )
				return c < aDLFunctionVersion.c;
			else
				return b < aDLFunctionVersion.b;
		}
		else
			return a < aDLFunctionVersion.a;
	}
};

/** Параметры, определяющие особенности применения функции. Могут комбинироватсья функцией побитового ИЛИ. */
enum DLFunctionOptions
{
	/** Все флаги опущены. */
	DLFO_NONE = 0,

	/** Разрешено кэшировать результат функции. */
	DLFO_CACHE = 1,

	/** Разрешено использовать функцию в плюс-строках. Этот флаг можно поднимать только при поднятом флаге DLFO_CACHE. */
	DLFO_PLUS_STR = 2,

	/** Аргументы функции имеют аттрибуты */
	DLFO_ARGS_ATTRS = 4,
};


/**
 *  Информация о функции.
 */
struct DLFunctionInfo
{
	/** Имя функции (по этому имени осуществляется обращение к функции в языке DL и внутри динамической библиотеки). */
	const char * name;

	/** Альтернативное имя функции (по этому имени осуществляется обращение к функции в языке DL). */
	const char * short_name;

	/** Описание функции.*/
	const char * description;

	/** Тип возвращаемого значения */
	DLFunctionResType rest;

	/** Количество аргументов функции. */
	const unsigned int argc;

	/** Массив с описанием каждого из аргументов функции. */
	const DLFucntionArgInfo * argt;

	/** Версия функции. */
	const DLFunctionVersion version;

	/** Минимальная версия функции, для которой гарантируется обратная совместимость. */
	const DLFunctionVersion min_version;

	/** Вектор флагов - параметров функции. */
	DLFunctionOptions options;

	/** Версия API, с которой была собрана функция. */
	unsigned int DLFunctionInterfaceVer;

	/** Минмальная версия API, с которой совместима данная функция. */
	unsigned int DLFunctionInterfaceMinVer;

	/** Версия протокола упаковки данных, с которой была собрана функция. */
	unsigned int InfDataProtocolVer;

	/** Тип аргумента для случая переменного числа аргументов. */
	const DLFucntionArgInfo * VarArgType;

};

/**
 *  Возвращает указатель на структуру, описывающую функцию, или nullptr в случае ошибки.
 */
typedef const DLFunctionInfo * (*FGetDLFucntionInfo)();

struct FunctionContext;

/**
 *  Функция.
 * @param aArgs - список аргументов функции, завершаемый значением nullptr.
 * @param aResult - результат функции.
 * @param aLength - длина результата функции.
 * @param aMemoryAllocator - указатель на объект, выделяющий память.
 * @param aSession - текущая сессия. Используется только внутренними функциями.
 */
typedef InfEngineErrors (*FDLFucntion)( const char ** aArgs,
		const char *& aResult,
		unsigned int & aLength,
		nMemoryAllocator * aMemoryAllocaor,
		FunctionContext * aSession );


/////////////////////////////// Автотестирование //////////////////////////////

typedef InfEngineErrors (*FCheckDLFucntion)();

// Структура, описывающая один тест.
struct Test
{
	// Кол-во аргументов.
	unsigned int vArgsCount;
	// Список аргументов.
	const char ** vArgs;

	// Длина результата.
	unsigned int vResultLength;
	// Результат.
	const char * vResult;

	// Название теста.
	const char * vName;
};

// Определение теста с заданным названием.
#define CREATE_TEST( N, RESULT_LENGTH, RESULT, ARGS_COUNT, ... ) \
	static const unsigned int TEST_ARGS_COUNT_ ## N    = ARGS_COUNT; \
	static const char *       TEST_ARGS_ ## N[]        = { __VA_ARGS__, nullptr }; \
	static const unsigned int TEST_RESULT_LENGTH_ ## N = (int)strlen( RESULT ); \
	static const char TEST_RESULT_ ## N[]      = RESULT; \
	static const char TEST_NAME_ ## N[]      = #N;

// Определение теста с заданным названием.
#define CREATE_TEST2( N, RESULT, ARGS_COUNT, ... ) \
	static const unsigned int TEST_ARGS_COUNT_ ## N    = ARGS_COUNT; \
	static const char *       TEST_ARGS_ ## N[]        = { __VA_ARGS__, nullptr }; \
	static const unsigned int TEST_RESULT_LENGTH_ ## N = (int)strlen( RESULT ); \
	static const char TEST_RESULT_ ## N[]      = RESULT; \
	static const char TEST_NAME_ ## N[]      = #N;

// Подстановка структуры-теста с заданным названием.
#define TEST( N ) \
	{ TEST_ARGS_COUNT_ ## N, TEST_ARGS_ ## N, TEST_RESULT_LENGTH_ ## N, TEST_RESULT_ ## N, TEST_NAME_ ## N }

// Подготовка списка тестов.
#define REGISTER_TESTS( ... ) static const Test TESTS[] = { __VA_ARGS__};

// Простейшая функция для проверки библиотеки.
#define DEFAULT_CHECK_DL_FUNCTION( DL_FUNC ) \
	InfEngineErrors CheckDLFucntion() \
	{ \
		return CommonTesting( DL_FUNC, TESTS, sizeof(TESTS) / sizeof(Test) ); \
	}

// Функция для проверки стандартных тестов.
inline InfEngineErrors CommonTesting( FDLFucntion DLFunction, const Test aTests[], unsigned int aTestsCnt )
{
	nMemoryAllocator aMemoryAllocator;
	const char * aResult;
	unsigned int aResultLength;
	unsigned int aErrorsCnt = 0;

	printf( "\n" );
	for( unsigned int test_n = 0; test_n < aTestsCnt; ++test_n )
	{
		fprintf( stdout, "  Test    %-30s ", aTests[test_n].vName );
		fflush( stdout );

		bool error = true;
		if( DLFunction( aTests[test_n].vArgs, aResult, aResultLength, &aMemoryAllocator, nullptr ) == INF_ENGINE_SUCCESS )
		{
			if( aResultLength == aTests[test_n].vResultLength )
			{
				if( !memcmp( aResult, aTests[test_n].vResult, aResultLength ) )
					error = false;
				else
					printf( "\n\t\t\"%s\"\n\t\t\"%s\"\n", aResult, aTests[test_n].vResult );

			}
			else
			{
				printf( "\n\t\tresult length %i <> %i\n", aResultLength, aTests[test_n].vResultLength );
				printf( "\t\tresult \"%s\" <> \"%s\"\n", aResult, aTests[test_n].vResult );
			}
		}

		if( error )
		{
			fprintf( stdout, "[FAIL]\n" );
			++aErrorsCnt;
		}
		else
			fprintf( stdout, "[SUCCESS]\n" );

		fflush( stdout );
	}

	fprintf( stdout, "\nSUCCESS: %i\n", aTestsCnt - aErrorsCnt  );
	fprintf( stdout, "FAIL:    %i\n", aErrorsCnt );

	return aErrorsCnt ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_SUCCESS;
}



#ifdef __cplusplus
}
#endif

#endif
