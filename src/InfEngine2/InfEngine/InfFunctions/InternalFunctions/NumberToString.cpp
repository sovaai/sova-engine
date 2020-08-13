#include "InternalFunction.hpp"
#include "InfEngine2/_Include/Errors.h"
#include "NumberToString.hpp"
#include <NanoLib/LongFloat.hpp>
#include <NanoLib/NumberParser.hpp>


using namespace InternalFunctions;

const DLFucntionArgInfo NumberToString::ARGS[] = {
    { DLFAT_EXTENDED, 0 },
};

const DLFunctionInfo NumberToString::DLF_INFO = {
    "NumberToString",
    "n2s",
    "Convert number to numeral.",
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

ExtICoreRO * NumberToString::vBase = nullptr;


InfEngineErrors NumberToString::Apply(const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * aMemoryAllocator, 
        FunctionContext * aFunctionContext )
{
    // Проверка аргументов.
    if( !aArgs[0] || aArgs[1] )
        return INF_ENGINE_ERROR_ARGC;
    
    // Подготовка записи числа цифрами.
    aTextString number, result;
    nlReturnCode nlrc = number.assign( aArgs[0] );
    if( nlrcSuccess != nlrc )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;
    
    // Представление записи числа прописью.
    if( nlrcSuccess != ( nlrc = NumberParser::NumberToString( number, result ) ) )
    {
        if( nlrcErrorNoFreeMemory == nlrc )
            return INF_ENGINE_ERROR_NOFREE_MEMORY;
        if( nlrcUnsuccess == nlrc )
            return INF_ENGINE_WARN_UNSUCCESS;
        return INF_ENGINE_ERROR_NANOLIB;
    }
    
    // Подготовка результата.
    char * buffer = static_cast<char *>( aMemoryAllocator->Allocate( result.size() + 1 ) );
    if( !buffer )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;
    strcpy( buffer, result.ToConstChar() );
    
    aResult = buffer;
    aLength = result.size();
    
    return INF_ENGINE_SUCCESS;
}
