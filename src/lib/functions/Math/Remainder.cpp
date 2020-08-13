#include <NanoLib/NumberConvertor.hpp>

#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = {
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE }
};

static const DLFunctionInfo DLF_INFO = {
	"Remainder",
	"m%",
	"Returns integer remainder of division of two integers.",
	DLFRT_TEXT,
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

static const char EMPTY[] = "";
static const unsigned int EMPTY_LEN = 0;

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

InfEngineErrors Remainder( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						   nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || !aArgs[1] || aArgs[2] )
		return INF_ENGINE_ERROR_ARGC;

	// Конвертор чисел.
	static NumberConvertor Convertor;

	LongFloat arg = 0;
	nlReturnCode nlrc = Convertor.s2lf( aArgs[0], arg );
	if( nlrc )
		return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;

	LongFloat divider = 0;
	nlrc = Convertor.s2lf( aArgs[1], divider );
	if( nlrc )
		return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;

	LongFloat sum = divider == LongFloat( 0 ) ? LongFloat( 0 ) : arg.Mod( divider );

	char * buffer = static_cast<char *>( aMemoryAllocator->Allocate( 256 ) );
	if( !buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	aLength = sum.snprintf( buffer, 255, 0 );
	aResult = buffer;

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE


CREATE_TEST2( TEST_00, "0", 2, "10000000000223456", "10000000000223456" )
CREATE_TEST2( TEST_01, "0", 2, "-1223456", "-1223456" )
CREATE_TEST2( TEST_02, "0", 2, "1223456", "-1223456" )
CREATE_TEST2( TEST_03, "0", 2, "-1223456", "1223456" )

CREATE_TEST2( TEST_04, "0", 2, "0", "1" )
CREATE_TEST2( TEST_05, "0", 2, "0000", "-1" )
CREATE_TEST2( TEST_06, "0", 2, "-00", "-001" )
CREATE_TEST2( TEST_07, "1", 2, "123457", "123456" )
CREATE_TEST2( TEST_08, "1234", 2, "1234", "-123456" )

CREATE_TEST2( TEST_09, "1", 2, "3132251", "8375" )
CREATE_TEST2( TEST_10, "-1", 2, "-3132251", "8375" )
CREATE_TEST2( TEST_11, "-1", 2, "-3132251", "-374" )
CREATE_TEST2( TEST_12, "-374", 2, "-374", "-08375" )
CREATE_TEST2( TEST_13, "-2", 2, "-2", "-3" )
CREATE_TEST2( TEST_14, "-1", 2, "-003", "-02" )

CREATE_TEST2( TEST_15, "-56", 2, "-0369488", "-84" )
CREATE_TEST2( TEST_16, "30511631", 2, "369488369", "169488369" )
CREATE_TEST2( TEST_17, "1", 2, "369488369", "2" )

CREATE_TEST2( BIG_TEST_01, "256831512454640929439838856165388433", 2, "52349081239490827344652837597234658128592387456293785", "752458723874217834712386434634645612" )
CREATE_TEST2( BIG_TEST_02, "86021117065275428444376", 2, "237492134214921974982138748721", "122421421215340000003459" )

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

DEFAULT_CHECK_DL_FUNCTION( Remainder )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
