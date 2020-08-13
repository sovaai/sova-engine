#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = { { DLFAT_ANY, nullptr, DLFAA_NONE } };

static const DLFunctionInfo DLF_INFO = {
	"Shuffle",
	"",
	"Shuffle characters in given string.",
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

InfEngineErrors Shuffle( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						 nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ )
{
	if( !aArgs[0] || aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;

	aLength = strlen( aArgs[0] );
	if( !aLength )
	{
		aResult = "";
		return INF_ENGINE_SUCCESS;
	}

	char * buffer = nAllocateObjects( *aMemoryAllocator, char, aLength + 1 );
	if( !buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	strncpy( buffer, aArgs[0], aLength );
	buffer[aLength] = 0;

	avector<UChar32> tmp;
	tmp.reserve( aLength + 1 );
	if( tmp.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	int32_t srcOffset = 0;
	for(; srcOffset < aLength; )
	{
		UChar32 ch;
		U8_NEXT_UNSAFE( aArgs[0], srcOffset, ch );
		tmp.push_back( ch );
	}

	unsigned int new_pos;
	for( unsigned int pos = 0; pos < tmp.size() - 1; ++pos )
	{
		new_pos = pos + 1 + rand() % ( tmp.size() - pos - 1 );
		std::swap( tmp[pos], tmp[new_pos] );
	}

	int32_t dstOffset = 0;
	for( unsigned int pos = 0; pos < tmp.size(); ++pos )
		if( dstOffset + U8_LENGTH( tmp[pos] ) < aLength + 1 )
		{
			U8_APPEND_UNSAFE( buffer, dstOffset, tmp[pos] );
		}
		else
			return INF_ENGINE_ERROR_INTERNAL;

	aResult = buffer;

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE

const char * PARAM_1[] = {
	"",
	"1",
	"12",
	"abcdef",
	"константинополь",
	"0123456789 abcdefghijklmnopqrstuvwxyz абвгдеёжзийклмнопрстуфхцчшщьыъэюя",
	"0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ 0123456789 abcdefghijklmnopqrstuvwxyz абвгдеёжзийклмнопрстуфхцчшщьыъэюя"
};


int Count_cmp( const void * a, const void * b )
{
	const std::pair<UChar32, unsigned int> * val1 = reinterpret_cast<const std::pair<UChar32, unsigned int> *>( a );
	const std::pair<UChar32, unsigned int> * val2 = reinterpret_cast<const std::pair<UChar32, unsigned int> *>( b );

	if( val1->first < val2->first )
		return -1;
	if( val1->first > val2->first )
		return 1;
	if( val1->second < val2->second )
		return -1;
	if( val1->second > val2->second )
		return 1;
	return 0;
}

InfEngineErrors Count( const char * aStr, avector<std::pair<UChar32, unsigned int> > & aResult )
{
	aResult.clear();
	unsigned int length = strlen( aStr );
	int32_t srcOffset = 0;
	for(; srcOffset < length; )
	{
		UChar32 ch;
		U8_NEXT_UNSAFE( aStr, srcOffset, ch );

		unsigned int i;
		for( i = 0; i < aResult.size(); ++i )
		{
			if( aResult[i].first == ch )
			{
				++aResult[i].second;
				break;
			}
		}
		if( i == aResult.size() )
		{
			aResult.push_back( std::pair<UChar32, unsigned int>( ch, 1 ) );
			if( aResult.no_memory() )
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
		}

		aResult.qsort( Count_cmp );
	}

	return INF_ENGINE_SUCCESS;
}

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
		const char * Args[] = { PARAM_1[val_n], nullptr };
		bool error = true;

		if( Shuffle( Args, Result, ResultLength, &MemoryAllocator, nullptr ) == INF_ENGINE_SUCCESS )
		{
			if( ResultLength != strlen( Args[0] )  )
				fprintf( stdout, "\n\t\tWrong result length: %u instead %lu\n", ResultLength, strlen( Args[0] ) );
			else
			{
				avector<std::pair<UChar32, unsigned int> > image1, image2;

				InfEngineErrors iee = Count( Args[0], image1 );
				if( INF_ENGINE_SUCCESS != iee )
					return iee;
				if( INF_ENGINE_SUCCESS != ( iee = Count( Result, image2 ) ) )
					return iee;

				if( image1.size() != image2.size() )
					fprintf( stdout, "\n\t\tWrong result: \"%s\" is incorrect result for argument \"%s\"\n", Result, Args[0] );
				else
				{
					error = false;
					for( unsigned int i = 0; i < image2.size(); ++i )
						if( image1[i] != image2[i] )
						{
							fprintf( stdout, "\n\t\tWrong result: \"%s\" is incorrect result for argument \"%s\"\n", Result, Args[0] );
							error = true;
							break;
						}
				}
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
