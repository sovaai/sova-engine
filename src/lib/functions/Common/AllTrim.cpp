#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C" {
#endif

	static const DLFucntionArgInfo ARGS[] = {
		{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
		{ DLFAT_TEXT, "", DLFAA_NONE }
	};

	static const DLFunctionInfo DLF_INFO = {
		"AllTrim",
		"at",
		"Normalize specified string.",
		DLFRT_TEXT,
		sizeof( ARGS )/sizeof( DLFucntionArgInfo ),
		ARGS,
		{ 3, 1, 1 },
		{ 3, 0, 1 },
		DLFO_CACHE,
		DLFunctionInterfaceVersion,
		5,
		InfDataProtocolVersion,
		nullptr
	};

	const DLFunctionInfo * GetDLFucntionInfo() {
		return &DLF_INFO;
	}

	InfEngineErrors AllTrim( const char ** aArgs, const char *& aResult, unsigned int & aLength,
							 nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ ) {

		if( !aArgs[0] || ( aArgs[1] && aArgs[2] ) )
			return INF_ENGINE_ERROR_ARGC;

		aLength = strlen( aArgs[0] );
		char * Buffer = nAllocateObjects( *aMemoryAllocator, char, aLength + 1 );
		if( !Buffer )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		strncpy( Buffer, aArgs[0], aLength );
		Buffer[aLength] = '\0';

		aLength = TextFuncs::AllTrim( Buffer, aLength, true, true, aArgs[1] && strlen( aArgs[1] ) ? 1 : 0 );
		aResult = Buffer;

		return INF_ENGINE_SUCCESS;
	}

	/*******************************************************************************************************************
	 *
	 *  Автотесты.
	 *
	 ******************************************************************************************************************/

	#ifdef COMPILE_WITH_AUTOTEST_MODE

	CREATE_TEST2( EMPTY, "", 2, "", "" )

	CREATE_TEST2( SPACES_00, "", 2, " ", "" )
	CREATE_TEST2( SPACES_01, "", 2, "    ", "" )
	CREATE_TEST2( SPACES_02, "", 2, "\t", "" )
	CREATE_TEST2( SPACES_03, "", 2, " \t", "" )
	CREATE_TEST2( SPACES_04, "", 2, "\t \t  \t\t\t  \t   \t  \t\t  \t    ", "" )

	CREATE_TEST2( DO_NOTHING_00, "a", 2, "a", "" )
	CREATE_TEST2( DO_NOTHING_01, "Q", 2, "Q", "" )
	CREATE_TEST2( DO_NOTHING_02, ",", 2, ",", "" )
	CREATE_TEST2( DO_NOTHING_03, "ab", 2, "ab", "" )
	CREATE_TEST2( DO_NOTHING_04, "123", 2, "123", "" )

	CREATE_TEST2( SIMPLE_00, "a", 2, " a", "" )
	CREATE_TEST2( SIMPLE_01, "a", 2, "a ", "" )
	CREATE_TEST2( SIMPLE_02, "a", 2, "a\t", "" )
	CREATE_TEST2( SIMPLE_03, "a", 2, "\ta", "" )
	CREATE_TEST2( SIMPLE_04, "a", 2, "a \t\r\n ", "" )
	CREATE_TEST2( SIMPLE_05, "a", 2, " \t\r a", "" )

	CREATE_TEST2( NORMAL_00, "Hello cruel world !!!", 2, "  Hello   cruel  world    !!! ", "" )
	CREATE_TEST2( NORMAL_00I, "Hellocruelworld!!!", 2, "  Hello   cruel  world    !!! ", "1" )
	CREATE_TEST2( NORMAL_01, "Hello cruel world !!!", 2, "\t  Hello \n  cruel \t \r\r \t  world    !!! ", "" )
	CREATE_TEST2( NORMAL_01I, "Hellocruelworld!!!", 2, "\t  Hello \n  cruel \t \r\r \t  world    !!! ", "1" )
	CREATE_TEST2( NORMAL_02, "Hello cruel world !!!", 2, "  Hello   cruel  world    !!!\t\t\t\t\n", "" )
	CREATE_TEST2( NORMAL_02I, "Hellocruelworld!!!", 2, "  Hello   cruel  world    !!!\t\t\t\t\n", "1" )
	CREATE_TEST2( NORMAL_03, "Hello cruel world !!!", 2, "\t\t\tHello\t\t\t\tcruel  world    !!!\r\n\r\t\n ", "" )
	CREATE_TEST2( NORMAL_03I, "Hellocruelworld!!!", 2, "\t\t\tHello\t\t\t\tcruel  world    !!!\r\n\r\t\n ", "1" )
	CREATE_TEST2( NORMAL_04, "Hello cruel world !!!", 2, "\r\n  Hello\r\ncruel\r\nworld\r\n!!! ", "" )
	CREATE_TEST2( NORMAL_04I, "Hellocruelworld!!!", 2, "\r\n  Hello\r\ncruel\r\nworld\r\n!!! ", "1" )
	CREATE_TEST2( NORMAL_05, "Hello cruel world !!!", 2, "\nHello\ncruel\nworld\n!!!\n", "" )
	CREATE_TEST2( NORMAL_05I, "Hellocruelworld!!!", 2, "\nHello\ncruel\nworld\n!!!\n", "1" )

	CREATE_TEST2(
		BIG_00,
		"As a child I played football, and when grown up started playing rock'n'roll",
		2,
		"  \tAs \n\r   \t \r\n a  \t child \t I \r\n played   football,\nand\nwhen\tgrown\tup  \t\t started   playing rock'n'roll\n", "" )

	CREATE_TEST2(
		BIG_01,
		"As a child I played football, and when grown up started playing rock'n'roll",
		2,
		"\t\rAs a \n child \t   \r   \n  I played football, \n and   when\t\tgrown up\r\rstarted playing  rock'n'roll \t\r", "" )

	CREATE_TEST2(
		BIG_02,
		"As a child I played football, and when grown up started playing rock'n'roll",
		2,
		"\r\rAs a\tchild\tI played\nfootball,\n\n\n\nand\t\t\t\twhen\r\r\r\rgrown up started playing rock'n'roll", "" )

	CREATE_TEST2(
		BIG_03,
		"As a child I played football, and when grown up started playing rock'n'roll",
		2,
		"As a child I played football, and when grown up started playing rock'n'roll ", "" )

	CREATE_TEST2(
		BIG_04,
		"As a child I played football, and when grown up started playing rock'n'roll",
		2,
		" As a child I played football, and when grown up started playing rock'n'roll", "" )

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
		TEST( NORMAL_00I ),
		TEST( NORMAL_01 ),
		TEST( NORMAL_01I ),
		TEST( NORMAL_02 ),
		TEST( NORMAL_02I ),
		TEST( NORMAL_03 ),
		TEST( NORMAL_03I ),
		TEST( NORMAL_04 ),
		TEST( NORMAL_04I ),
		TEST( NORMAL_05 ),
		TEST( NORMAL_05I ),
		TEST( BIG_00 ),
		TEST( BIG_01 ),
		TEST( BIG_02 ),
		TEST( BIG_03 ),
		TEST( BIG_04 ) )

	DEFAULT_CHECK_DL_FUNCTION( AllTrim )

	#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
