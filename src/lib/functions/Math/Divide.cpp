#include <math.h>

#include <NanoLib/NumberConvertor.hpp>

#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = {
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
	{ DLFAT_TEXT, "", DLFAA_NONE }
};

static const DLFunctionInfo DLF_INFO = {
	"Divide",
	"m/",
	"Returns result of divison of two integers.",
	DLFRT_TEXT,
	3,
	ARGS,
	{ 3, 1, 0 },
	{ 3, 0, 0 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

static const char EMPTY[] = "";
static const unsigned int EMPTY_LEN = 0;

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

InfEngineErrors Divide( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ )
{
	// Проверка аргументов.
	if( !aArgs[0] || !aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;
	else if( aArgs[2] )
	{
		if( aArgs[3] || ( aArgs[2][0] && strcasecmp( aArgs[2], "RU" ) ) )
			return INF_ENGINE_ERROR_ARGC;
	}

	long long divider = TextFuncs::ToInteger( aArgs[1] );
	long long sum = divider ? TextFuncs::ToInteger( aArgs[0] ) / divider : 0;
	// Конвертор чисел.
	static NumberConvertor Convertor;

	// Разбор первого аргумента.
	LongFloat first = 0;
	nlReturnCode nlrc = Convertor.s2lf( aArgs[0], first );
	if( nlrc )
		return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;

	// Разбор второго аргумента.
	LongFloat second = 0;
	nlrc = Convertor.s2lf( aArgs[1], second );
	if( nlrc )
		return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;

	LongFloat Sum = first / second;

	char* buffer = nullptr;
	if( aArgs[2] && !strcasecmp( aArgs[2], "RU" ) )
	{
		aTextString Tmp;
		nlReturnCode nlrc = Convertor.d2s( Sum, Tmp );
		if( nlrc )
			return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;
		buffer = static_cast<char *>( aMemoryAllocator->Allocate( Tmp.size() + 1 ) );
		if( !buffer )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		memcpy( buffer, Tmp.ToConstChar(), Tmp.size() + 1 );
		aLength = Tmp.size();
	}
	else
	{
		buffer = static_cast<char *>( aMemoryAllocator->Allocate( 256 ) );
		if( !buffer )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;

		aLength = Sum.snprintf( buffer, 255, 10 );
	}
	aResult = buffer;

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE


CREATE_TEST2( TEST_00, "1", 2, "12234560000000000", "12234560000000000" )
CREATE_TEST2( TEST_01, "1", 2, "-1223456", "-1223456" )
CREATE_TEST2( TEST_02, "-1", 2, "1223456", "-1223456" )
CREATE_TEST2( TEST_03, "-1", 2, "-1223456", "1223456" )

CREATE_TEST2( TEST_04, "0", 2, "0", "1" )
CREATE_TEST2( TEST_05, "0", 2, "0000", "-1" )
CREATE_TEST2( TEST_06, "0", 2, "-00", "-001" )
CREATE_TEST2( TEST_07, "1.0000081000", 2, "123457", "123456" )
CREATE_TEST2( TEST_08, "-0.0099954639", 2, "1234", "-123456" )

CREATE_TEST2( TEST_09, "374.0001194029", 2, "3132251", "8375" )
CREATE_TEST2( TEST_10, "-374.0001194029", 2, "-3132251", "8375" )
CREATE_TEST2( TEST_11, "8375.0026737967", 2, "-3132251", "-374" )
CREATE_TEST2( TEST_12, "0.0446567164", 2, "-374", "-08375" )
CREATE_TEST2( TEST_13, "0.6666666666", 2, "-2", "-3" )
CREATE_TEST2( TEST_14, "1.5", 2, "-003", "-02" )

CREATE_TEST2( TEST_15, "4398.6666666666", 2, "-0369488", "-84" )
CREATE_TEST2( TEST_16, "2.1800219754", 2, "369488369", "169488369" )
CREATE_TEST2( TEST_17, "184744184.5", 2, "369488369", "2" )

CREATE_TEST2( TEST_18, "2.1800219754", 2, "триста шестьдесят девять миллионов четыреста восемьдесят восемь тысяч триста шестьдесят девять", "169488369" )
CREATE_TEST2( TEST_19, "184744184", 2, "369488368", "два" )
CREATE_TEST2( TEST_20, "один и пять десятых", 3, "-003", "-02", "RU" )
CREATE_TEST2( TEST_21, "один и двадцать пять сотых", 3, "5", "4", "RU" )

CREATE_TEST2( TEST_22, "-29799556.7105834823", 2, "-36948.8369", "0.0012399123" )

CREATE_TEST2( BIG_TEST_01, "121167387.2561170569", 2, "997356782189397452398475975623", "8231231231232532353228" )

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
	TEST( TEST_18 ),
	TEST( TEST_19 ),
	TEST( TEST_20 ),
	TEST( TEST_21 ),
	TEST( TEST_22 ),
	TEST( BIG_TEST_01 ) )

DEFAULT_CHECK_DL_FUNCTION( Divide )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */


#ifdef __cplusplus
}
#endif
