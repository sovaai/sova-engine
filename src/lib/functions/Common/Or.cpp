#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = {
	{ DLFAT_BOOL, nullptr, DLFAA_NONE },
	{ DLFAT_BOOL, nullptr, DLFAA_NONE }
};

const DLFucntionArgInfo VAR_ARGS = { DLFAT_BOOL, nullptr, DLFAA_NONE };

static const DLFunctionInfo DLF_INFO = {
	"Or",
	"",
	"Logical Or",
	DLFRT_BOOL,
	2,
	ARGS,
	{ 3, 0, 1 },
	{ 3, 0, 1 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	&VAR_ARGS
};

static const char True[] = "True";
static const unsigned int TrueLen = 4;

static const char False[] = "";
static const unsigned int FalseLen = 0;

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

InfEngineErrors Or( const char ** aArgs, const char *& aResult, unsigned int & aLength,
					nMemoryAllocator* /*aMemoryAllocator*/,FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || !aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;

	bool res = false;
	for( const char ** arg = aArgs; *arg && !res; ++arg )
		if( (*arg)[0] != '\0' )
			res = true;

	if( res )
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


CREATE_TEST2( TEST_00, "True", 2, "True", "TRUE" )
CREATE_TEST2( TEST_01, "True", 3, "True", "TRUE", "TrUE" )
CREATE_TEST2( TEST_02, "True", 4, "True", "TRUE", "TrUE", "true" )
CREATE_TEST2( TEST_03, "True", 5, "True", "TRUE", "TrUE", "true", "" )
CREATE_TEST2( TEST_04, "True", 3, "True", "", "TrUE" )

CREATE_TEST2( TEST_05, "", 2, "", "" )
CREATE_TEST2( TEST_06, "", 3, "", "", "" )
CREATE_TEST2( TEST_07, "", 4, "", "", "", "" )
CREATE_TEST2( TEST_08, "", 5, "", "", "", "", "" )

REGISTER_TESTS(
	TEST( TEST_00 ),
	TEST( TEST_01 ),
	TEST( TEST_02 ),
	TEST( TEST_03 ),
	TEST( TEST_04 ),
	TEST( TEST_05 ),
	TEST( TEST_06 ),
	TEST( TEST_07 ),
	TEST( TEST_08 ) )

DEFAULT_CHECK_DL_FUNCTION( Or )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
