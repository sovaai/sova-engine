#include <cmath>

#include "../FunctionLib.hpp"
#include <unicode/ustring.h>


#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = {
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
	{ DLFAT_TEXT, "\"'`<[{(", DLFAA_NONE }
};

static const DLFunctionInfo DLF_INFO = {
	"ProperName",
	"Fltr",
	"Change case of caracters in given word to represent it as proper name.",
	DLFRT_TEXT,
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

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

InfEngineErrors ProperName( const char ** aArgs, const char *& aResult, unsigned int & aLength,
							nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ )
{
	// Проверка аргументов.
	if( !aArgs || !aArgs[0] )
		return INF_ENGINE_ERROR_ARGC;

	// Выделение стоп символов.
	static avector<UChar> StopSymbols;
	if( aArgs[1] )
	{
		unsigned int StopSymbolsLength = strlen( aArgs[1] );
		StopSymbols.resize( StopSymbolsLength + 1 );
		if( StopSymbols.no_memory() )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		bzero( StopSymbols.get_buffer(), StopSymbols.size_bytes() );
		u_charsToUChars( aArgs[1], StopSymbols.get_buffer(), StopSymbolsLength );
	}
	else
	{
		unsigned int StopSymbolsLength = strlen( ARGS[1].default_value );
		StopSymbols.resize( StopSymbolsLength + 1 );
		if( StopSymbols.no_memory() )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		bzero( StopSymbols.get_buffer(), StopSymbols.size_bytes() );
		u_charsToUChars( ARGS[1].default_value, StopSymbols.get_buffer(), StopSymbolsLength );
	}

	aLength = strlen( aArgs[0] );
	unsigned int AllocatedLength = ceil( ( ( (double)aLength ) * 4 ) / 3 );
	char * buffer = nAllocateObjects( *aMemoryAllocator, char, AllocatedLength + 1 );
	if( !buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	int32_t srcOffset = 0;
	int32_t dstOffset = 0;

	bool set_upper = true;
	for(; srcOffset < aLength; )
	{
		UChar32 ch;
		U8_NEXT_UNSAFE( aArgs[0], srcOffset, ch );

		if( set_upper )
		{
			if( !u_isspace( ch ) && !u_strchr32( StopSymbols.get_buffer(), ch ) )
			{
				ch = u_toupper( ch );
				set_upper = false;
			}
		}
		else
			ch = u_tolower( ch );

		if( dstOffset + U8_LENGTH( ch ) < AllocatedLength )
		{
			U8_APPEND_UNSAFE( buffer, dstOffset, ch );
		}
	}
	buffer[dstOffset] = '\0';
	aLength = dstOffset;

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
CREATE_TEST2( DO_NOTHING_01, "Q", 1, "Q" )
CREATE_TEST2( DO_NOTHING_02, ",", 1, "," )
CREATE_TEST2( DO_NOTHING_03, "Ab", 1, "ab" )
CREATE_TEST2( DO_NOTHING_04, "123a", 1, "123a" )

CREATE_TEST2( SIMPLE_00, " A", 1, " a" )
CREATE_TEST2( SIMPLE_01, "A ", 1, "a " )
CREATE_TEST2( SIMPLE_02, "A\t", 1, "a\t" )
CREATE_TEST2( SIMPLE_03, "\tA", 1, "\ta" )
CREATE_TEST2( SIMPLE_04, "A \t\r\n ", 1, "a \t\r\n " )
CREATE_TEST2( SIMPLE_05, " \t\r A", 1, " \t\r A" )
CREATE_TEST2( SIMPLE_06, " \t\r\n\"'<{([Qwerty])}>'\"", 1, " \t\r\n\"'<{([qWERTY])}>'\"" )

CREATE_TEST2( NORMAL_00, "  Hello   cruel  world    !!! ", 1, "  Hello   cruel  worlD    !!! " )
CREATE_TEST2( NORMAL_01, "Hello cruel world !!!", 1, "Hello cruel world !!!" )
CREATE_TEST2( NORMAL_02, "Hello cruel world !!!", 1, "hello cruel world !!!" )
CREATE_TEST2( NORMAL_03, "Глеб", 1, "ГлеБ" )
CREATE_TEST2( NORMAL_04, "Глеб", 1, "глеб" )
CREATE_TEST2( NORMAL_05, "123Глеб", 2, "123глеб", "123" )
CREATE_TEST2( NORMAL_06, "123глеб", 2, "123глеб", "13" )
CREATE_TEST2( NORMAL_07, "1cdef23Глеб", 2, "1cdef23глеб", "123abcdefgh" )


CREATE_TEST2(
	BIG_00,
	"As a child i played football, and when grown up started playing rock'n'roll",
	1,
	"As a child I played football, and when grown up started playing rock'n'roll" )

CREATE_TEST2(
	BIG_01,
	" 'As a child i played football, and when grown up started playing rock'n'roll",
	1,
	" 'aS a child I played football, and when grown UP startEd playing Rock'n'roll" )

CREATE_TEST2(
	BIG_02,
	"<<As a child i played football, and when grown up started playing rock'n'roll",
	1,
	"<<as a child I played football, and when grown up started playing rock'N'roll" )


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
	TEST( SIMPLE_06 ),
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
	TEST( BIG_02 ) )

DEFAULT_CHECK_DL_FUNCTION( ProperName )


#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
