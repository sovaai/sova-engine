#include <ctime>

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
	"DateDist",
	"ddst",
	"Returns distance in days between two date given as parameters.",
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

InfEngineErrors DateDist( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						  nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || !aArgs[1] || aArgs[2] )
		return INF_ENGINE_ERROR_ARGC;

	unsigned int day;
	unsigned int month;
	unsigned int year;
	bool is_short[2];
	time_t sec[2];

	for( unsigned int arg_n = 0; arg_n < 2; ++arg_n )
	{
		if( !TextFuncs::ParseDate( aArgs[arg_n], day, month, year, is_short[arg_n] ) )
			return INF_ENGINE_WARN_UNSUCCESS;

		// Хотябы у одного из параметров должен быть указан год.
		if( arg_n == 1 && is_short[0] && is_short[1] )
			return INF_ENGINE_WARN_UNSUCCESS;

		// Если у текущего параметра не указан год.
		if( is_short[arg_n] )
		{
			// Получаем текущее время.
			time_t rawtime;
			time( &rawtime );
			struct tm * t = localtime( &rawtime );
			if( !t )
				return INF_ENGINE_WARN_UNSUCCESS;

			if( static_cast<unsigned int>( t->tm_mon ) < month - 1 ||
					( static_cast<unsigned int>( t->tm_mon ) == month - 1 && static_cast<unsigned int>( t->tm_mday ) <= day ) )
				year = t->tm_year + 1900;
			else
				year = t->tm_year + 1901;


		}
		else if( year < 100 )
			year += 2000;

		struct tm t;
		t.tm_sec = 0;
		t.tm_min = 0;
		t.tm_hour = 0;
		t.tm_mday = day;
		t.tm_mon = month - 1;
		t.tm_year = year - 1900;
		t.tm_isdst = 0;

		if( ( sec[arg_n] = mktime( &t ) ) == -1 )
			return INF_ENGINE_WARN_UNSUCCESS;
	}

	unsigned int dist = abs( sec[0] - sec[1] ) / 3600 / 24;

	char * buffer = static_cast<char *>( aMemoryAllocator->Allocate( 10 ) );
	if( !buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	aLength = sprintf( buffer, "%i", dist );
	aResult = buffer;

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE


CREATE_TEST2( TEST_00, "0", 2, "10.10.10", "10.10.10" )
CREATE_TEST2( TEST_01, "0", 2, "10.10.10", "10.10.2010" )
CREATE_TEST2( TEST_02, "0", 2, "10.10.2010", "10.10.10" )
CREATE_TEST2( TEST_03, "0", 2, "10.10.2010", "10.10.2010" )

CREATE_TEST2( TEST_04, "366", 2, "1.1.00", "01.1.01" )
CREATE_TEST2( TEST_05, "366", 2, "1.1.00", "01.1.1" )
CREATE_TEST2( TEST_06, "366", 2, "1.1.00", "01.1.2001" )
CREATE_TEST2( TEST_07, "366", 2, "1.1.2000", "01.1.01" )
CREATE_TEST2( TEST_08, "366", 2, "1.1.2000", "01.1.2001" )

CREATE_TEST2( TEST_09, "365", 2, "1.1.01", "01.1.02" )
CREATE_TEST2( TEST_10, "365", 2, "1.1.01", "01.1.2" )
CREATE_TEST2( TEST_11, "365", 2, "1.1.01", "01.1.2002" )
CREATE_TEST2( TEST_12, "365", 2, "1.1.2001", "01.1.02" )
CREATE_TEST2( TEST_13, "365", 2, "1.1.2001", "01.1.2002" )

CREATE_TEST2( TEST_14, "33", 2, "1.01.1998", "03.2.1998" )
CREATE_TEST2( TEST_15, "3", 2, "30.12.2002", "2.01.03" )
CREATE_TEST2( TEST_16, "8", 2, "28.02.2002", "8.03.02" )

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
	TEST( TEST_16 ) )

DEFAULT_CHECK_DL_FUNCTION( DateDist )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
