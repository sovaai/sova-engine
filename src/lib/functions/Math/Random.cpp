#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = {
	{ DLFAT_EXTENDED, "1", DLFAA_NONE },
	{ DLFAT_EXTENDED, "100", DLFAA_NONE }
};

static const DLFunctionInfo DLF_INFO = {
	"Random",
	"rand",
	"Returns random integer from specified range. It's [1..100] by default.",
	DLFRT_TEXT,
	2,
	ARGS,
	{ 3, 0, 1 },
	{ 3, 0, 1 },
	DLFO_NONE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

static const char EMPTY[] = "";
static const unsigned int EMPTY_LEN = 0;

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

InfEngineErrors Random( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ )
{
	int left_range;
	int right_range;

	if( !aArgs[0] )
	{
		left_range = 1;
		right_range = 100;
	}
	else if( aArgs[0] && !aArgs[1] )
	{
		left_range = TextFuncs::ToInteger( aArgs[0] );
		right_range = 100;
	}
	else if( aArgs[0] && aArgs[1] && !aArgs[2] )
	{
		left_range = TextFuncs::ToInteger( aArgs[0] );
		right_range = TextFuncs::ToInteger( aArgs[1] );
	}
	else
		return INF_ENGINE_WARN_UNSUCCESS;

	int sum;

	if( left_range > right_range )
		sum = 0;
	else if( left_range == right_range )
		sum = left_range;
	else
		sum = rand() % ( right_range - left_range + 1 ) + left_range;

	char* buffer = static_cast<char*>( aMemoryAllocator->Allocate( 10 ) );
	if( !buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	aLength = sprintf( buffer, "%i", sum );
	aResult = buffer;

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE

const char * PARAM_1[] = { "-1", "-2", "-3", "-4", "-5", "1", "-2" };
const char * PARAM_2[] = { "10", "9", "8", "7", "6", "1", "-1" };

InfEngineErrors CheckDLFucntion()
{
	unsigned int aTestsCnt = sizeof ( PARAM_1 ) / sizeof ( const char *);
	const char * Result;
	unsigned int ResultLength;
	unsigned int aErrorsCnt = 0;
	nMemoryAllocator MemoryAllocator;

	fprintf( stdout, "\n" );
	for( unsigned int val_n = 0; val_n < aTestsCnt; ++val_n )
	{
		fprintf( stdout, "  Test    %-30i ", val_n + 1 );
		const char * Args[] = { PARAM_1[val_n], PARAM_2[val_n], nullptr };
		bool error = false;
		int LowerBound = TextFuncs::ToInteger( PARAM_1[val_n] );
		int UpperBound = TextFuncs::ToInteger( PARAM_2[val_n] );

		for( unsigned int test_n = 0; test_n < 10; ++test_n )
		{
			if( Random( Args, Result, ResultLength, &MemoryAllocator, nullptr ) == INF_ENGINE_SUCCESS )
			{
				int ResultValue = TextFuncs::ToInteger( Result );

				if( ResultValue < LowerBound || ResultValue > UpperBound )
				{
					fprintf( stdout, "\n\t\tResult %i not in range [%i;%i]\n", ResultValue, LowerBound, UpperBound );
					error = true;
					break;
				}
			}
			else
			{
				error = true;
				break;
			}
		}

		if( error )
		{
			fprintf( stdout, "[FAIL]\n" );
			++aErrorsCnt;
		}
		else
			fprintf( stdout, "[SUCCESS]\n" );

		fflush( stdout );
	}

	fprintf( stdout, "\nSUCCESS: %i\n", aTestsCnt - aErrorsCnt );
	fprintf( stdout, "FAIL:    %i\n", aErrorsCnt );

	return INF_ENGINE_SUCCESS;
}

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
