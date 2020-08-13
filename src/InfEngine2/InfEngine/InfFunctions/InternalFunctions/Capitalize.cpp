#include "InternalFunction.hpp"
#include "InfEngine2/_Include/Errors.h"
#include "Capitalize.hpp"

#include <unicode/ustring.h>

using namespace InternalFunctions;

const DLFucntionArgInfo Capitalize::ARGS[] = {
    { DLFAT_EXTENDED, 0 },
    { DLFAT_TEXT, "\"'`<[{(" }
};

const DLFunctionInfo Capitalize::DLF_INFO = {
    "Capitalize",
    "cap",
    "Change all words in text to represent them as proper names.",
    DLFRT_TEXT,
    1,
    ARGS,
    { 3, 0, 0 },
    { 3, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,  
    InfDataProtocolVersion,
    nullptr
};

ExtICoreRO * Capitalize::vBase = nullptr;


InfEngineErrors Capitalize::Apply(const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * aMemoryAllocator, 
        FunctionContext * aFunctionContext )
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
        if( StopSymbols.no_memory( ) )
            return INF_ENGINE_ERROR_NOFREE_MEMORY;
        bzero( StopSymbols.get_buffer( ), StopSymbols.size_bytes( ) );
	    u_charsToUChars( aArgs[1], StopSymbols.get_buffer( ), StopSymbolsLength );
    }
    else
    {
        unsigned int StopSymbolsLength = strlen( ARGS[1].default_value );
        StopSymbols.resize( StopSymbolsLength + 1 );
        if( StopSymbols.no_memory( ) )
            return INF_ENGINE_ERROR_NOFREE_MEMORY;
        bzero( StopSymbols.get_buffer( ), StopSymbols.size_bytes( ) );
	    u_charsToUChars( ARGS[1].default_value, StopSymbols.get_buffer( ), StopSymbolsLength );
    }

    aLength = strlen( aArgs[0] );
    unsigned int AllocatedLength = ceil( ( ( (double)aLength )*4 ) / 3 );
    char* buffer = nAllocateObjects( *aMemoryAllocator, char, AllocatedLength + 1 );
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
            if( !u_isspace( ch ) && !u_strchr32( StopSymbols.get_buffer( ), ch ) )
            {
                ch = u_toupper( ch );
                set_upper = false;
            }
        }
        else
        {
            if( u_isspace( ch ) || u_strchr32( StopSymbols.get_buffer( ), ch ) )
                set_upper = true;
            else
                ch = u_tolower( ch );
        }

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
