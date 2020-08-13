#include <NanoLib/NumberConvertor.hpp>

#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = {
	{ DLFAT_ANY, nullptr, DLFAA_NONE },
	{ DLFAT_ANY, nullptr, DLFAA_NONE }
};

static const DLFunctionInfo DLF_INFO = {
	"Greater",
	"m>",
	"Returns true if value given in first parameter greater then value given in second parameter, otherwise returns false.",
	DLFRT_BOOL,
	2,
	ARGS,
	{ 3, 0, 1 },
	{ 3, 0, 1 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

static const char True[] = "True";
static const unsigned int TrueLen = 4;

static const char False[] = "";
static const unsigned int FalseLen = 0;

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

InfEngineErrors Greater( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						 nMemoryAllocator* /*aMemoryAllocator*/, FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || !aArgs[1] || aArgs[2] )
		return INF_ENGINE_ERROR_ARGC;

	// Конвертор чисел.
	static NumberConvertor Convertor;

	LongFloat first = 0;
	nlReturnCode nlrc = Convertor.s2lf( aArgs[0], first );
	if( nlrc )
		return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;

	LongFloat second = 0;
	nlrc = Convertor.s2lf( aArgs[1], second );
	if( nlrc )
		return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;

	if( first > second )
	{
		aResult = True;
		aLength = TrueLen;
	}
	else
	{
		aResult = False;
		aLength = FalseLen;
	}

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE


CREATE_TEST2( TEST_00, "", 2, "12234560000000000", "12234560000000000" )
CREATE_TEST2( TEST_01, "", 2, "-1223456", "-1223456" )
CREATE_TEST2( TEST_02, "True", 2, "1223456", "-1223456" )
CREATE_TEST2( TEST_03, "", 2, "-1223456", "1223456" )

CREATE_TEST2( TEST_04, "", 2, "0", "-000" )
CREATE_TEST2( TEST_05, "", 2, "0000", "-0000" )
CREATE_TEST2( TEST_06, "", 2, "-00", "-0000" )
CREATE_TEST2( TEST_07, "True", 2, "123457", "123456" )
CREATE_TEST2( TEST_08, "True", 2, "1234", "-123456" )

CREATE_TEST2( TEST_09, "", 2, "123456", "223456" )
CREATE_TEST2( TEST_10, "", 2, "-123456", "-123456" )
CREATE_TEST2( TEST_11, "", 2, "-3", "-2" )
CREATE_TEST2( TEST_12, "True", 2, "-2123456", "-3123456" )
CREATE_TEST2( TEST_13, "True", 2, "-2", "-3" )
CREATE_TEST2( TEST_14, "True", 2, "-000", "-111324" )

CREATE_TEST2( TEST_15, "True", 2, "369488", "-3247372" )
CREATE_TEST2( TEST_16, "True", 2, "369488369", "369488368" )
CREATE_TEST2( TEST_17, "True", 2, "369488469", "369488369" )

CREATE_TEST2( BIG_TEST_01, "True", 2, "7572348128342134682975981295891349598134785719823475", "37845124982475976592716787234921736491236489" )
CREATE_TEST2( BIG_TEST_02, "True", 2, "79998932392453467234534512341293456", "-324823642863874723247761767127947" )

REGISTER_TESTS(
	TEST( TEST_00 ),
	TEST( TEST_01 ),
	TEST( TEST_02 ),
	TEST( TEST_03 ),
	TEST( TEST_04 ),
	TEST( TEST_05 ),
	TEST( TEST_06 ),
	TEST( TEST_07 ),
	TEST( TEST_08 ),
	TEST( TEST_09 ),
	TEST( TEST_10 ),
	TEST( TEST_11 ),
	TEST( TEST_12 ),
	TEST( TEST_13 ),
	TEST( TEST_14 ),
	TEST( TEST_15 ),
	TEST( TEST_16 ),
	TEST( TEST_17 ),
	TEST( BIG_TEST_01 ),
	TEST( BIG_TEST_02 ) )

DEFAULT_CHECK_DL_FUNCTION( Greater )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
