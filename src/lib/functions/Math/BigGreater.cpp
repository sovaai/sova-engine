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
	"BigGreater",
	"",
	"Returns true if big integer value given in first parameter greater then big integer value given in second parameter, otherwise returns false.",
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

InfEngineErrors BigGreater( const char ** aArgs, const char *& aResult, unsigned int & aLength,
							nMemoryAllocator* /*aMemoryAllocator*/, FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || !aArgs[1] || aArgs[2] )
		return INF_ENGINE_ERROR_ARGC;

	const char * a = aArgs[0];
	const char * b = aArgs[1];

	while( *a && TextFuncs::IsSpace( *a ) )
		++a;

	while( *b && TextFuncs::IsSpace( *b ) )
		++b;

	bool a_negative = false;
	bool b_negative = false;

	if( *a == '-' )
	{
		a_negative = true;
		++a;
	}
	else if( *a == '+' )
	{
		++a;
	}
	else if( !TextFuncs::IsDigit( *a ) )
	{
		aResult = False;
		aLength = FalseLen;
		return INF_ENGINE_SUCCESS;
	}

	if( *b == '-' )
	{
		b_negative = true;
		++b;
	}
	else if( *b == '+' )
	{
		++b;
	}
	else if( !TextFuncs::IsDigit( *b ) )
	{
		aResult = False;
		aLength = FalseLen;
		return INF_ENGINE_SUCCESS;
	}

	while( *a && *a == '0' )
		++a;

	while( *b && *b == '0' )
		++b;

	unsigned int len_a = strlen( a );
	unsigned int len_b = strlen( b );

	bool greater;

	if( !len_a && !len_b )
	{
		greater = false;
	}
	else if( a_negative && !b_negative )
	{
		greater = false;
	}
	else if( !a_negative && b_negative )
	{
		greater = true;
	}
	else if( len_a == len_b )
	{
		while( *a && *b && *a == *b )
		{
			++a;
			++b;
		}
		greater = a_negative ? *a < *b : *a > *b;
	}
	else if( a_negative && b_negative )
	{
		greater = len_a < len_b;
	}
	else if( !a_negative && !b_negative )
	{
		greater = len_a > len_b;
	}

	if( greater )
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

#    ifdef COMPILE_WITH_AUTOTEST_MODE


CREATE_TEST2( TEST_00, "", 2, "12234567890", "12234567890" )
CREATE_TEST2( TEST_01, "", 2, "-12234567890", "-12234567890" )
CREATE_TEST2( TEST_02, "", 2, "-12234567890", "12234567890" )
CREATE_TEST2( TEST_03, "True", 2, "12234567890", "-12234567890" )

CREATE_TEST2( TEST_04, "", 2, "0", "-000" )
CREATE_TEST2( TEST_05, "", 2, "0000", "-0000" )
CREATE_TEST2( TEST_06, "", 2, "-00", "-0000" )
CREATE_TEST2( TEST_07, "True", 2, "1234567891", "1234567890" )
CREATE_TEST2( TEST_08, "True", 2, "1234567890", "-12345678987654321" )

CREATE_TEST2( TEST_09, "", 2, "12345678987654321", "22345678987654321" )
CREATE_TEST2( TEST_10, "", 2, "-1234567890987654321", "-1234567890987654311" )
CREATE_TEST2( TEST_11, "", 2, "-3", "-2" )
CREATE_TEST2( TEST_12, "True", 2, "-2123456787654321", "-312345678987654321" )
CREATE_TEST2( TEST_13, "True", 2, "-2", "-3" )
CREATE_TEST2( TEST_14, "True", 2, "-000", "-11132432346344" )

CREATE_TEST2( TEST_15, "True", 2, "36948836948882054828205482", "-3247372" )
CREATE_TEST2( TEST_16, "True", 2, "36948836948882054828205482", "36948836948882054828205481" )
CREATE_TEST2( TEST_17, "True", 2, "3694883694888205482820548236948836948882054828205482", "3694883694888205482820548236948836948882054828205182" )

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
	TEST( TEST_17 ) )

DEFAULT_CHECK_DL_FUNCTION( BigGreater )

#    endif	/** COMPILE_WITH_AUTOTEST_MODE */

#    ifdef __cplusplus
}
#endif
