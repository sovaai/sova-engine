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
	"DateGreater",
	"d>",
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

InfEngineErrors DateGreater( const char ** aArgs, const char *& aResult, unsigned int & aLength,
							 nMemoryAllocator* /*aMemoryAllocator*/, FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || !aArgs[1] || aArgs[2] )
		return INF_ENGINE_ERROR_ARGC;

	unsigned int day[2];
	unsigned int month[2];
	unsigned int year[2];
	bool is_short;

	for( unsigned int arg_n = 0; arg_n < 2; ++arg_n )
	{
		if( !TextFuncs::ParseDate( aArgs[arg_n], day[arg_n], month[arg_n], year[arg_n], is_short ) || is_short )
			return INF_ENGINE_WARN_UNSUCCESS;

		if( year[arg_n] < 100 )
			year[arg_n] += 2000;
	}

	bool greater = false;

	if( year[0] == year[1] )
	{
		if( month[0] == month[1] )
			greater = day[0] > day[1];
		else
			greater = month[0] > month[1];
	}
	else
		greater = year[0] > year[1];

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

#ifdef COMPILE_WITH_AUTOTEST_MODE


CREATE_TEST2( TEST_00, "", 2, "10.10.10", "10.10.10" )
CREATE_TEST2( TEST_01, "", 2, "10.10.10", "10.10.2010" )
CREATE_TEST2( TEST_02, "", 2, "10.10.2010", "10.10.10" )
CREATE_TEST2( TEST_03, "", 2, "10.10.2010", "10.10.2010" )

CREATE_TEST2( TEST_04, "", 2, "1.1.00", "01.1.01" )
CREATE_TEST2( TEST_05, "", 2, "1.1.00", "01.1.1" )
CREATE_TEST2( TEST_06, "", 2, "1.1.00", "01.1.2001" )
CREATE_TEST2( TEST_07, "True", 2, "01.1.01", "1.1.2000" )
CREATE_TEST2( TEST_08, "True", 2, "01.1.2001", "1.1.2000" )

CREATE_TEST2( TEST_09, "", 2, "1.1.01", "02.1.01" )
CREATE_TEST2( TEST_10, "", 2, "1.1.01", "01.2.1" )
CREATE_TEST2( TEST_11, "", 2, "1.1.01", "01.1.02" )
CREATE_TEST2( TEST_12, "True", 2, "01.1.02", "1.1.2001" )
CREATE_TEST2( TEST_13, "True", 2, "02.1.2001", "1.1.2001" )
CREATE_TEST2( TEST_14, "True", 2, "01.2.2001", "1.1.2001" )

CREATE_TEST2( TEST_15, "True", 2, "1.01.11", "03.2.1911" )
CREATE_TEST2( TEST_16, "True", 2, "31.12.2002", "1.01.02" )
CREATE_TEST2( TEST_17, "True", 2, "1.1.2012", "31.12.11" )

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

DEFAULT_CHECK_DL_FUNCTION( DateGreater )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
