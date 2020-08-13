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
	"Multiply",
	"m*",
	"Returns result of multiplication of two integers.",
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

InfEngineErrors Multiply( const char ** aArgs, const char *& aResult, unsigned int & aLength,
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

	LongFloat Sum = first * second;

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


CREATE_TEST2( TEST_00, "10000000001", 2, "1", "10000000001" )
CREATE_TEST2( TEST_01, "1", 2, "-1", "-1" )
CREATE_TEST2( TEST_02, "-1", 2, "1", "-1" )
CREATE_TEST2( TEST_03, "-1", 2, "-1", "1" )

CREATE_TEST2( TEST_04, "0", 2, "0", "1" )
CREATE_TEST2( TEST_05, "0", 2, "0000", "-1" )
CREATE_TEST2( TEST_06, "0", 2, "-00", "-001" )
CREATE_TEST2( TEST_07, "0", 2, "123457", "0" )
CREATE_TEST2( TEST_08, "0", 2, "-0000", "-00" )

CREATE_TEST2( TEST_09, "23341", 2, "23341", "1" )
CREATE_TEST2( TEST_10, "-3132251", 2, "1", "-3132251" )
CREATE_TEST2( TEST_11, "201259", 2, "-313", "-643" )
CREATE_TEST2( TEST_12, "-827842", 2, "-9974", "083" )
CREATE_TEST2( TEST_13, "6", 2, "-2", "-3" )
CREATE_TEST2( TEST_14, "6", 2, "-003", "-02" )

CREATE_TEST2( TEST_15, "23341", 2, "двадцать три тысячи триста сорок один", "1" )
CREATE_TEST2( TEST_16, "-3132251", 2, "1", "минус три миллиона сто тридцать две тысячи двести пятьдесят один" )
CREATE_TEST2( TEST_17, "шесть", 3, "-2", "-3", "RU" )

CREATE_TEST2( TEST_18, "-42301654.055435264", 2, "-97245.1102", "435.00032" )

CREATE_TEST2( BIG_TEST_01, "-48817718761230195773472727415157053380482818684809662", 2, "74629236634876255173462362", "-654136648885623990003726651" )
CREATE_TEST2( BIG_TEST_02, "3733693730473151038664162389709651492626266042277396780460928", 2, "-37365524571267887423324", "-99923492934023040230491001949248573472" )

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
	TEST( BIG_TEST_01 ),
	TEST( BIG_TEST_02 ) )

DEFAULT_CHECK_DL_FUNCTION( Multiply )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
