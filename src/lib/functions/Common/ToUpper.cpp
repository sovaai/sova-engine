#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = { { DLFAT_EXTENDED, nullptr, DLFAA_NONE } };

static const DLFunctionInfo DLF_INFO = {
	"ToUpper",
	"/\\",
	"Apply characters in given string to upper case.",
	DLFRT_TEXT,
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

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

InfEngineErrors ToUpper( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						 nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;

	aLength = strlen( aArgs[0] );
	unsigned int AllocatedBufferLength = ceil( ( ( (double)aLength ) * 4 ) / 3 );
	char * buffer = nAllocateObjects( *aMemoryAllocator, char, AllocatedBufferLength + 1 );
	if( !buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	strncpy( buffer, aArgs[0], aLength );
	buffer[aLength] = 0;

	if( TextFuncs::ToUpper( buffer, aLength, AllocatedBufferLength + 1 ) != nlrcSuccess )
		return INF_ENGINE_ERROR_FAULT;
	aLength = strlen( buffer );
	aResult = buffer;

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE

CREATE_TEST2( EMPTY, "", 1, "" )

CREATE_TEST2( SPACES_00, " ", 1, " " )
CREATE_TEST2( SPACES_01, "    ", 1, "    " )
CREATE_TEST2( SPACES_02, "\t", 1, "\t" )
CREATE_TEST2( SPACES_03, " \t", 1, " \t" )
CREATE_TEST2( SPACES_04, "\t \t  \t\t\t  \t   \t  \t\t  \t    ", 1, "\t \t  \t\t\t  \t   \t  \t\t  \t    " )

CREATE_TEST2( DO_NOTHING_00, "A", 1, "a" )
CREATE_TEST2( DO_NOTHING_01, "Q", 1, "q" )
CREATE_TEST2( DO_NOTHING_02, ",", 1, "," )
CREATE_TEST2( DO_NOTHING_03, "AB", 1, "ab" )
CREATE_TEST2( DO_NOTHING_04, "123A", 1, "123a" )

CREATE_TEST2( SIMPLE_00, " A", 1, " a" )
CREATE_TEST2( SIMPLE_01, "A ", 1, "a " )
CREATE_TEST2( SIMPLE_02, "Ц\t", 1, "ц\t" )
CREATE_TEST2( SIMPLE_03, "\tГ", 1, "\tг" )
CREATE_TEST2( SIMPLE_04, "Ё \t\r\n ", 1, "ё \t\r\n " )
CREATE_TEST2( SIMPLE_05, " \t\r Ъ", 1, " \t\r ъ" )

CREATE_TEST2( NORMAL_00, "  HELLO   CRUEL   WORLD  !!!  ", 1, "  HEllO   cRUEL   wORLD  !!!  " )
CREATE_TEST2( NORMAL_01, "HELLO CRUEL WORLD !!!", 1, "hello cruel world !!!" )
CREATE_TEST2( NORMAL_02, "HELLO CRUEL WORLD !!!", 1, "Hello Cruel World !!!" )
CREATE_TEST2( NORMAL_03, "ГЛЕБ", 1, "глеб" )
CREATE_TEST2( NORMAL_04, "ГЛЕБ", 1, "глеБ" )


CREATE_TEST2(
	BIG_00,
	"0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ",
	1,
	"0123456789 abcdefghijklmnopqrstuvwxyz абвгдеёжзийклмнопрстуфхцчшщьыъэюя" )

CREATE_TEST2(
	BIG_01,
	"0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ 0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ",
	1,
	"0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ 0123456789 abcdefghijklmnopqrstuvwxyz абвгдеёжзийклмнопрстуфхцчшщьыъэюя" )

REGISTER_TESTS(
	TEST( EMPTY ),
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
	TEST( NORMAL_00 ),
	TEST( NORMAL_01 ),
	TEST( NORMAL_02 ),
	TEST( NORMAL_03 ),
	TEST( NORMAL_04 ),
	TEST( BIG_00 ),
	TEST( BIG_01 ) )

DEFAULT_CHECK_DL_FUNCTION( ToUpper )


#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
