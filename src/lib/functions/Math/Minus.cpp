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
	"Minus",
	"m-",
	"Returns result of substraction of two integers.",
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

InfEngineErrors Minus( const char ** aArgs, const char *& aResult, unsigned int & aLength,
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

	LongFloat Sum = first - second;

	char * buffer = nullptr;
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


CREATE_TEST2( TEST_00, "0", 2, "10000000000223456", "10000000000223456" )
CREATE_TEST2( TEST_01, "0", 2, "-1223456", "-1223456" )
CREATE_TEST2( TEST_02, "2446912", 2, "1223456", "-1223456" )
CREATE_TEST2( TEST_03, "-2446912", 2, "-1223456", "1223456" )

CREATE_TEST2( TEST_04, "0", 2, "0", "-000" )
CREATE_TEST2( TEST_05, "0", 2, "0000", "-0000" )
CREATE_TEST2( TEST_06, "0", 2, "-00", "-0000" )
CREATE_TEST2( TEST_07, "1", 2, "123457", "123456" )
CREATE_TEST2( TEST_08, "124690", 2, "1234", "-123456" )

CREATE_TEST2( TEST_09, "-100000", 2, "123456", "223456" )
CREATE_TEST2( TEST_10, "0", 2, "-123456", "-123456" )
CREATE_TEST2( TEST_11, "-1", 2, "-3", "-2" )
CREATE_TEST2( TEST_12, "1000000", 2, "-2123456", "-3123456" )
CREATE_TEST2( TEST_13, "1", 2, "-2", "-3" )
CREATE_TEST2( TEST_14, "111324", 2, "-000", "-111324" )

CREATE_TEST2( TEST_15, "3616860", 2, "369488", "-3247372" )
CREATE_TEST2( TEST_16, "1", 2, "369488369", "369488368" )
CREATE_TEST2( TEST_17, "100", 2, "369488469", "369488369" )


CREATE_TEST2( TEST_18, "0", 2, "двенадцать", "двенадцать" )
CREATE_TEST2( TEST_19, "0", 2, "минус триста восемьдесят", "-380" )
CREATE_TEST2( TEST_20, "2446912", 2, "один миллион двести двадцать три тысячи четыреста пятьдесят шесть", "-1223456" )
CREATE_TEST2( TEST_21, "-2446912", 2, "-1223456", "один миллион двести двадцать три тысячи четыреста пятьдесят шесть" )
CREATE_TEST2( TEST_22, "один", 3, "369488369", "369488368", "RU" )
CREATE_TEST2( TEST_23, "сто", 3, "369488469", "369488369", "RU" )

CREATE_TEST2( TEST_24, "123.456", 2, "100.006", "-23.45" )

CREATE_TEST2( BIG_TEST_01, "-7572348090497009700500004703174562363213049228586986", 2, "37845124982475976592716787234921736491236489", "7572348128342134682975981295891349598134785719823475" )
CREATE_TEST2( BIG_TEST_02, "80323756035317341957782274108421403", 2, "79998932392453467234534512341293456", "-324823642863874723247761767127947" )

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
	TEST( TEST_23 ),
	TEST( TEST_24 ),
	TEST( BIG_TEST_01 ),
	TEST( BIG_TEST_02 ) )

DEFAULT_CHECK_DL_FUNCTION( Minus )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */


#ifdef __cplusplus
}
#endif
