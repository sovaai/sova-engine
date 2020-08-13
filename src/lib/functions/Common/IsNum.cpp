#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = { { DLFAT_EXTENDED, nullptr, DLFAA_NONE } };

static const DLFunctionInfo DLF_INFO = {
	"IsNum",
	"0-9",
	"Return True if given string contains only digits.",
	DLFRT_BOOL,
	1,
	ARGS,
	{ 1, 0, 1 },
	{ 1, 0, 1 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

#define CTRUE "True"
#define CTRUE_LEN strlen( CTRUE )

#define CFALSE ""
#define CFALSE_LEN strlen( CFALSE )

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

// Проверка строки на наличие нечисловых символов.

InfEngineErrors IsNum( const char ** aArgs, const char *& aResult, unsigned int & aLength,
					   nMemoryAllocator* /*aMemoryAllocator*/, FunctionContext * /*aSession*/ )
{
	// Проверка аргументов.
	if( !aArgs[0] )
		return INF_ENGINE_ERROR_ARGC;

	aResult = nullptr;
	aLength = 0;

	int32_t CurrentPos = 0;

	while( 1 )
	{
		// Выделение символа UTF.
		UChar32 ch;
		U8_NEXT_UNSAFE( aArgs[0], CurrentPos, ch );
		if( ch == 0 )
		{
			// Конец строки.
			aResult = CTRUE;
			aLength = CTRUE_LEN;

			return INF_ENGINE_SUCCESS;
		}

		// Проверка цифры.
		if( u_isdigit( ch ) || u_isspace( ch ) )
			continue;
		else
		{
			aResult = CFALSE;
			aLength = CFALSE_LEN;

			return INF_ENGINE_SUCCESS;
		}
	}
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#    ifdef COMPILE_WITH_AUTOTEST_MODE

CREATE_TEST2( EMPTY, CTRUE, 1, "" )

CREATE_TEST2( NUMBER_01, CTRUE, 1, "12347452 " )
CREATE_TEST2( NUMBER_02, CTRUE, 1, "5324 67534518" )
CREATE_TEST2( NUMBER_03, CTRUE, 1, "23561" )
CREATE_TEST2( NUMBER_04, CTRUE, 1, "231" )
CREATE_TEST2( NUMBER_05, CTRUE, 1, "909 3562" )
CREATE_TEST2( NUMBER_06, CTRUE, 1, "4" )
CREATE_TEST2( NUMBER_07, CTRUE, 1, "2321 321" )
CREATE_TEST2( NUMBER_08, CTRUE, 1, " 49 82 30962321" )
CREATE_TEST2( NUMBER_09, CTRUE, 1, "13" )
CREATE_TEST2( NUMBER_10, CTRUE, 1, "98237 " )

CREATE_TEST2( NOT_NUMBER_01, CFALSE, 1, " 12,347452" )
CREATE_TEST2( NOT_NUMBER_02, CFALSE, 1, "5324s67534518" )
CREATE_TEST2( NOT_NUMBER_03, CFALSE, 1, "235ы61" )
CREATE_TEST2( NOT_NUMBER_04, CFALSE, 1, "23в1" )
CREATE_TEST2( NOT_NUMBER_05, CFALSE, 1, "90 в 93562" )
CREATE_TEST2( NOT_NUMBER_06, CFALSE, 2, "4s" )
CREATE_TEST2( NOT_NUMBER_07, CFALSE, 1, "232!1321" )
CREATE_TEST2( NOT_NUMBER_08, CFALSE, 1, "498230962321!" )
CREATE_TEST2( NOT_NUMBER_09, CFALSE, 1, "13, " )
CREATE_TEST2( NOT_NUMBER_10, CFALSE, 1, "98237. " )


REGISTER_TESTS(
	TEST( EMPTY ),
	TEST( NUMBER_01 ),
	TEST( NUMBER_02 ),
	TEST( NUMBER_03 ),
	TEST( NUMBER_04 ),
	TEST( NUMBER_05 ),
	TEST( NUMBER_06 ),
	TEST( NUMBER_07 ),
	TEST( NUMBER_08 ),
	TEST( NUMBER_09 ),
	TEST( NUMBER_10 ),
	TEST( NOT_NUMBER_01 ),
	TEST( NOT_NUMBER_02 ),
	TEST( NOT_NUMBER_03 ),
	TEST( NOT_NUMBER_04 ),
	TEST( NOT_NUMBER_05 ),
	TEST( NOT_NUMBER_06 ),
	TEST( NOT_NUMBER_07 ),
	TEST( NOT_NUMBER_08 ),
	TEST( NOT_NUMBER_09 ),
	TEST( NOT_NUMBER_10 ) )

DEFAULT_CHECK_DL_FUNCTION( IsNum )


#endif	/* COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
