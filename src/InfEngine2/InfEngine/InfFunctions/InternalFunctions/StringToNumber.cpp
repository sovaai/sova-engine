#include "InternalFunction.hpp"
#include "InfEngine2/_Include/Errors.h"
#include "StringToNumber.hpp"
#include <NanoLib/NumberConvertor.hpp>
#include <NanoLib/LongFloat.hpp>
#include <NanoLib/KeyboardNumberParser.hpp>
#include <NanoLib/VoiceNumberParser.hpp>


using namespace InternalFunctions;

const unsigned int RESULT_BUFFER_LEN = 1024;

const DLFucntionArgInfo StringToNumber::ARGS[] = {
    { DLFAT_ANY, 0 },
};

const DLFunctionInfo StringToNumber::DLF_INFO = {
    "StringToNumber",
    "s2n",
    "Convert numeral to number.",
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

ExtICoreRO * StringToNumber::vBase = nullptr;


InfEngineErrors StringToNumber::Apply(const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * aMemoryAllocator, 
        FunctionContext * aFunctionContext )
{
    // Проверка аргументов.
    if( !aArgs[0] || aArgs[1] )
        return INF_ENGINE_ERROR_ARGC;
    
    // Получение информации о способе ввода данных.
    bool voice_input = false;
    if( aFunctionContext->vSession )
    {
        // Получаем часовой пояс из переменной в текущей сессии.
        const Vars::Vector::VarValue var_val = aFunctionContext->vSession->GetValue( InfEngineVarInputType );
        if( var_val )
        {
            const Vars::Vector::VarText var_value = var_val.AsText();
            if( var_value )
            {
                unsigned int var_len = 0;
                const char * value = var_value.GetValue( var_len );            
                if( value )
                {
                    if( !strncasecmp( value, "voiceinput", 10 ) )
                        voice_input = true;
                }
            }
        }
    }
    
    LongFloat res;
    if( voice_input )
    {
        VoiceNumberParser parser;
        nlReturnCode nlrc = parser.Parse( res, aArgs[0], strlen( aArgs[0] ) );
        if( nlrcSuccess != nlrc )
            return nlrcUnsuccess == nlrc ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_ERROR_NANOLIB;
    }
    else
    {
        KeyboardNumberParser parser;
        nlReturnCode nlrc = parser.Parse( res, aArgs[0], strlen( aArgs[0] ) );
        if( nlrcSuccess != nlrc )
            return nlrcUnsuccess == nlrc ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_ERROR_NANOLIB;
    }
    
    
    // Подготовка результата.
    char * buffer = static_cast<char *>( aMemoryAllocator->Allocate( RESULT_BUFFER_LEN ) );
    if( !buffer )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;
    if( !res.snprintf( buffer, RESULT_BUFFER_LEN - 1, 50 ) )
        return INF_ENGINE_ERROR_NANOLIB;
    
    aResult = buffer;
    aLength = strlen( buffer );
    
    return INF_ENGINE_SUCCESS;
}
