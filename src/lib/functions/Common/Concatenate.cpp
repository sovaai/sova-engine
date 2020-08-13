#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = { };

static const DLFucntionArgInfo VAR_ARG_TYPE = { DLFAT_ANY, nullptr, DLFAA_NONE };

static const DLFunctionInfo DLF_INFO = {
	"Concatenate",
	"+",
	"Concatenate given strings.",
	DLFRT_TEXT,
	0,
	ARGS,
	{ 3, 0, 1 },
	{ 3, 0, 1 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	&VAR_ARG_TYPE
};

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

InfEngineErrors Concatenate( const char ** aArgs, const char *& aResult, unsigned int & aLength,
							 nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ )
{
	aLength = 0;
	for( const char ** cur_arg = aArgs; *cur_arg; ++cur_arg )
		aLength += strlen( *cur_arg );

	char * Buffer = nAllocateObjects( *aMemoryAllocator, char, aLength + 1 );
	if( !Buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	Buffer[aLength] = '\0';

	char* pos = Buffer;
	for( const char ** cur_arg = aArgs; *cur_arg; ++cur_arg )
	{
		strcpy( pos, *cur_arg );
		pos += strlen( *cur_arg );
	}

	aResult = Buffer;

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE

CREATE_TEST2( EMPTY_00, "", 0, nullptr )
CREATE_TEST2( EMPTY_01, "", 1, "" )
CREATE_TEST2( EMPTY_02, "", 2, "", "" )

CREATE_TEST2( SPACES_00, " ", 2, " ", "" )
CREATE_TEST2( SPACES_01, " ", 2, "", " " )
CREATE_TEST2( SPACES_02, "    ", 2, "  ", "  " )
CREATE_TEST2( SPACES_03, "\t \n", 2, "\t ", "\n" )
CREATE_TEST2( SPACES_04, "  \n\n", 2, "  \n", "\n" )

CREATE_TEST2( DO_NOTHING_00, "a", 2, "a", "" )
CREATE_TEST2( DO_NOTHING_01, "Q", 2, "Q", "" )
CREATE_TEST2( DO_NOTHING_02, ",", 2, "", "," )
CREATE_TEST2( DO_NOTHING_03, "ab", 2, "ab", "" )
CREATE_TEST2( DO_NOTHING_04, "123", 2, "", "123" )

CREATE_TEST2( SIMPLE_00, "ab", 2, "a", "b" )
CREATE_TEST2( SIMPLE_01, "ab", 2, "ab", "" )
CREATE_TEST2( SIMPLE_02, "ab", 2, "", "ab" )
CREATE_TEST2( SIMPLE_03, "ab 12", 2, "ab ", "12" )
CREATE_TEST2( SIMPLE_04, "ab 12", 2, "ab", " 12" )
CREATE_TEST2( SIMPLE_05, "abc", 3, "a", "b", "c" )
CREATE_TEST2( SIMPLE_06, "abc", 3, "a", "", "bc" )
CREATE_TEST2( SIMPLE_07, "abc", 10, "", "", "a", "", "", "b", "", "c", "", "" )


CREATE_TEST2( NORMAL_00, "Hello cruel world !!!", 2, "Hello", " cruel world !!!" )
CREATE_TEST2( NORMAL_01, "Hello cruel world !!!", 2, "Hello cruel", " world !!!" )
CREATE_TEST2( NORMAL_02, "Hello cruel world !!!", 2, "Hello cruel world", " !!!" )
CREATE_TEST2( NORMAL_03, "Hello cruel world !!!", 2, "Hello cruel world !!!", "" )
CREATE_TEST2( NORMAL_04, "Hello cruel world !!!", 2, "", "Hello cruel world !!!" )

CREATE_TEST2( NORMAL_05, "Hello cruel world !!!", 3, "", "Hello cr", "uel world !!!" )
CREATE_TEST2( NORMAL_06, "Hello cruel world !!!", 4, "Hello", " cru", "el world !!!", "" )
CREATE_TEST2( NORMAL_07, "Hello cruel world !!!", 5, "H", "ell", "", "o cruel world !", "!!" )

CREATE_TEST2(
	BIG_00,
	"As a child I played football, and when grown up started playing rock'n'roll",
	2,
	"As a ",
	"child I played football, and when grown up started playing rock'n'roll" )

CREATE_TEST2(
	BIG_01,
	"As a child I played football, and when grown up started playing rock'n'roll",
	2,
	"As a child I played",
	" football, and when grown up started playing rock'n'roll" )

CREATE_TEST2(
	BIG_02,
	"As a child I played football, and when grown up started playing rock'n'roll",
	2,
	"As a child I played football",
	", and when grown up started playing rock'n'roll" )

CREATE_TEST2(
	BIG_03,
	"As a child I played football, and when grown up started playing rock'n'roll",
	2,
	"As a child I played football, and when grown up started ",
	"playing rock'n'roll" )

CREATE_TEST2(
	BIG_04,
	"As a child I played football, and when grown up started playing rock'n'roll",
	2,
	"As a child I played football, and when grown up started playing rock'n'rol",
	"l" )

CREATE_TEST2(
	BIG_05,
	"As a child I played football, and when grown up started playing rock'n'roll",
	3,
	"As a ",
	"child I played football",
	", and when grown up started playing rock'n'roll" )

CREATE_TEST2(
	BIG_06,
	"As a child I played football, and when grown up started playing rock'n'roll",
	5,
	"As a child ",
	"I played ",
	"football, ",
	"and when grown up started playing rock",
	"'n'roll" )

CREATE_TEST2(
	BIG_07,
	"As a child I played football, and when grown up started playing rock'n'roll",
	7,
	"As",
	" a child ",
	"I played football",
	", and when ",
	"grown up started ",
	"playing rock",
	"'n'roll" )

REGISTER_TESTS(
	TEST( EMPTY_00 ),
	TEST( EMPTY_01 ),
	TEST( EMPTY_02 ),
	TEST( SPACES_00 ),
	TEST( SPACES_01 ),
	TEST( SPACES_02 ),
	TEST( SPACES_03 ),
	TEST( SPACES_04 ),
	TEST( DO_NOTHING_00 ),
	TEST( DO_NOTHING_01 ),
	TEST( DO_NOTHING_02 ),
	TEST( DO_NOTHING_03 ),
	TEST( DO_NOTHING_04 ),
	TEST( SIMPLE_00 ),
	TEST( SIMPLE_01 ),
	TEST( SIMPLE_02 ),
	TEST( SIMPLE_03 ),
	TEST( SIMPLE_04 ),
	TEST( SIMPLE_05 ),
	TEST( SIMPLE_06 ),
	TEST( SIMPLE_07 ),
	TEST( NORMAL_00 ),
	TEST( NORMAL_01 ),
	TEST( NORMAL_02 ),
	TEST( NORMAL_03 ),
	TEST( NORMAL_04 ),
	TEST( NORMAL_05 ),
	TEST( NORMAL_06 ),
	TEST( NORMAL_07 ),
	TEST( BIG_00 ),
	TEST( BIG_01 ),
	TEST( BIG_02 ),
	TEST( BIG_03 ),
	TEST( BIG_04 ),
	TEST( BIG_05 ),
	TEST( BIG_06 ),
	TEST( BIG_07 ) )

DEFAULT_CHECK_DL_FUNCTION( Concatenate )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
