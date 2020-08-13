#include <NanoLib/aTextString.hpp>

#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = {
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
	{ DLFAT_TEXT, nullptr, DLFAA_BOTH_SPACE },
	{ DLFAT_EXTENDED, nullptr, DLFAA_BOTH_SPACE }
};

static const DLFunctionInfo DLF_INFO = {
	"Replace",
	"",
	"Replace a substring given in second parameter to string ginven in third parameter in string given in first parameter.",
	DLFRT_TEXT,
	3,
	ARGS,
	{ 3, 0, 1 },
	{ 3, 0, 1 },
	(DLFunctionOptions)( DLFO_CACHE | DLFO_ARGS_ATTRS ),
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

InfEngineErrors Replace( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						 nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ )
{
	// Проверка аргументов.
	if( !aArgs[0] || !aArgs[1] || !aArgs[2] || aArgs[3] )
		return INF_ENGINE_ERROR_ARGC;

	aTextString TmpString;

	// Вычисление длины аргументов.
	unsigned int str_len = strlen( aArgs[0] );
	if( str_len == 0 )
		return INF_ENGINE_WARN_UNSUCCESS;

	unsigned int substr_len = strlen( aArgs[1] );
	if( substr_len == 0 )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Исполнение замены.
	unsigned int begin = 0;
	while( const char * pos = strstr( aArgs[0] + begin, aArgs[1] ) )
	{
		unsigned int end = pos - aArgs[0];

		if( TmpString.append( aArgs[0] + begin, end - begin ) != nlrcSuccess || TmpString.append( aArgs[2] ) != nlrcSuccess )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		begin = end + substr_len;
	}
	TmpString.append( aArgs[0] + begin, str_len - begin );

	// Выделение памяти под результат.
	aLength = TmpString.size();
	char * Buffer = nAllocateObjects( *aMemoryAllocator, char, aLength + 1 );
	if( !Buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	strncpy( Buffer, TmpString.ToConstChar(), aLength );
	Buffer[aLength] = 0;
	aResult = Buffer;

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE

CREATE_TEST2( SPACES_00, " ", 3, " ", " ", " " )
CREATE_TEST2( SPACES_01, "    ", 3, "    ", "    ", "    " )
CREATE_TEST2( SPACES_02, "  ", 3, "\t", "\t", "  " )
CREATE_TEST2( SPACES_03, "\n", 3, " \t ", " \t ", "\n" )
CREATE_TEST2( SPACES_04, "\t \t  \t\t\t  \t   \t  \t\t  \t    ", 3, "\r  \r   \r \r \r   \r    \r   \r \r   \r     ", "\r ", "\t" )

CREATE_TEST2( DO_NOTHING_00, "A", 3, "A", "A", "A" )
CREATE_TEST2( DO_NOTHING_01, "ABC", 3, "ABC", "ABC", "ABC" )
CREATE_TEST2( DO_NOTHING_02, "ABC", 3, "ABC", "CBA", "123" )
CREATE_TEST2( DO_NOTHING_03, "ABC ", 3, "ABC ", " ", " " )
CREATE_TEST2( DO_NOTHING_04, "123a", 3, "123a", "23", "23" )

CREATE_TEST2( SIMPLE_00, "ABCDEF", 3, " ", " ", "ABCDEF" )
CREATE_TEST2( SIMPLE_01, "ABCDEF", 3, "abcdef", "abcdef", "ABCDEF" )
CREATE_TEST2( SIMPLE_02, "AbAbAb", 3, "aBaBaB", "aB", "Ab" )
CREATE_TEST2( SIMPLE_03, "AbABaB", 3, "aBaBaB", "aBa", "AbA" )
CREATE_TEST2( SIMPLE_04, "123456", 3, "1A6", "A", "2345" )
CREATE_TEST2( SIMPLE_05, "1A6", 3, "123456", "2345", "A" )

CREATE_TEST2( NORMAL_00, "  hello   cruel  world    !!! ", 3, "  hello   cruel  world    ? ", "?", "!!!" )
CREATE_TEST2( NORMAL_01, "Hello cruel world !!!", 3, "HeLLo crueL worLd !!!", "L", "l" )
CREATE_TEST2( NORMAL_02, "Hello cruel world !!!", 3, "Hello cruel  world !!!", "el ", "el" )
CREATE_TEST2( NORMAL_03, "Глеб", 3, "ФилиппФилипп", "ФилиппФилипп", "Глеб" )
CREATE_TEST2( NORMAL_04, "Глеб", 3, "глеб", "г", "Г" )


CREATE_TEST2(
	BIG_00,
	"As a child i PLAYed football, and when grown up started PLAYing rock'n'roll",
	3,
	"As a child i played football, and when grown up started playing rock'n'roll",
	"play",
	"PLAY" )

CREATE_TEST2(
	BIG_01,
	" as a child i played football, and when grown up started playing rock'n'roll",
	3,
	"  as  a child i played football,  and when grown up started playing rock'n'roll",
	" a",
	"a" )

CREATE_TEST2(
	BIG_02,
	"As a child I played football, and when grown up started playing rock'n'roll",
	3,
	"As a child i played football, and when grown up started playing rock'n'roll",
	" i ",
	" I " )


REGISTER_TESTS(
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
	TEST( BIG_01 ),
	TEST( BIG_02 ) )

DEFAULT_CHECK_DL_FUNCTION( Replace )


#endif	/** COMPILE_WITH_AUTOTEST_MODE */


#ifdef __cplusplus
}
#endif
