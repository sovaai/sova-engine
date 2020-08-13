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
	"TimeLess",
	"t<",
	"Returns true if value given in first parameter less then value given in second parameter, otherwise returns false.",
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

InfEngineErrors TimeLess( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						  nMemoryAllocator * /*aMemoryAllocator*/,FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || !aArgs[1] || aArgs[2] )
		return INF_ENGINE_ERROR_ARGC;

	unsigned int hour[2] = { 0, 0 };
	unsigned int minute[2] = { 0, 0 };
	unsigned int second[2] = { 0, 0 };
	bool is_short;

	for( unsigned int arg_n = 0; arg_n < 2; ++arg_n )
	{
		if( !TextFuncs::ParseTime( aArgs[arg_n], hour[arg_n], minute[arg_n], second[arg_n], is_short ) )
			return INF_ENGINE_WARN_UNSUCCESS;
	}

	bool less = false;

	if( hour[0] == hour[1] )
	{
		if( minute[0] == minute[1] )
			less = second[0] < second[1];
		else
			less = minute[0] < minute[1];
	}
	else
		less = hour[0] < hour[1];

	if( less )
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


CREATE_TEST2( TEST_00, "", 2, "10:10:10", "10:10:10" )
CREATE_TEST2( TEST_01, "", 2, "00:00:00", "00:00:00" )

CREATE_TEST2( TEST_02, "True", 2, "11:12:13", "11:12:14" )
CREATE_TEST2( TEST_03, "True", 2, "11:12:13", "11:13:10" )
CREATE_TEST2( TEST_04, "True", 2, "11:12:13", "12:10:10" )

CREATE_TEST2( TEST_05, "", 2, "11:12:14", "11:12:13" )
CREATE_TEST2( TEST_06, "", 2, "11:13:10", "11:12:13" )
CREATE_TEST2( TEST_07, "", 2, "12:10:10", "11:12:13" )

REGISTER_TESTS(
	TEST( TEST_00 ),
	TEST( TEST_01 ),
	TEST( TEST_02 ),
	TEST( TEST_03 ),
	TEST( TEST_04 ),
	TEST( TEST_05 ),
	TEST( TEST_06 ),
	TEST( TEST_07 ) )

DEFAULT_CHECK_DL_FUNCTION( TimeLess )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
