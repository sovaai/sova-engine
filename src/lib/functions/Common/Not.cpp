#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = { { DLFAT_EXTENDED, nullptr, DLFAA_NONE } };

static const DLFunctionInfo DLF_INFO = {
	"Not",
	"!",
	"Logical Not",
	DLFRT_BOOL,
	1,
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

InfEngineErrors Not( const char ** aArgs, const char *& aResult, unsigned int &aLength,
					 nMemoryAllocator* /*aMemoryAllocator*/, FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;

	if( aArgs[0][0] == '\0' )
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

CREATE_TEST2( EMPTY, "True", 1, "" )

CREATE_TEST2( NOT_EMPTY_00, "", 1, " " )
CREATE_TEST2( NOT_EMPTY_01, "", 1, "\t" )
CREATE_TEST2( NOT_EMPTY_02, "", 1, "\r" )
CREATE_TEST2( NOT_EMPTY_03, "", 1, "\n" )
CREATE_TEST2( NOT_EMPTY_04, "", 1, "\r\n" )
CREATE_TEST2( NOT_EMPTY_05, "", 1, "a" )
CREATE_TEST2( NOT_EMPTY_06, "", 1, "123" )
CREATE_TEST2( NOT_EMPTY_07, "", 1, "As a child I played football, and when grown up started playing rock'n'roll" )


REGISTER_TESTS(
	TEST( EMPTY ),
	TEST( NOT_EMPTY_00 ),
	TEST( NOT_EMPTY_01 ),
	TEST( NOT_EMPTY_02 ),
	TEST( NOT_EMPTY_03 ),
	TEST( NOT_EMPTY_04 ),
	TEST( NOT_EMPTY_05 ),
	TEST( NOT_EMPTY_06 ),
	TEST( NOT_EMPTY_07 ) )

DEFAULT_CHECK_DL_FUNCTION( Not )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
