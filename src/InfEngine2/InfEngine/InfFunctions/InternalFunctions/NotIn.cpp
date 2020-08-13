#include "NotIn.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo NotIn::ARGS[] = {
    { DLFAT_VARIABLE, 0 },    
};
    
const DLFucntionArgInfo NotIn::VAR_ARGS = { DLFAT_EXTENDED, 0 };
    
const DLFunctionInfo NotIn::DLF_INFO = {
    "NotIn",
    "",
    "Check if variable value in first parameter is not equal to all of strings in other parameters.",
    DLFRT_BOOL,
    1,
    NotIn::ARGS,
    { 1, 0, 0 },
    { 1, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,
    InfDataProtocolVersion,
    &VAR_ARGS,
};

ExtICoreRO * NotIn::vBase = nullptr;

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;


InfEngineErrors NotIn::Apply( const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * /*aMemoryAllocator*/, 
        FunctionContext * aFunctionContext )
{    
	if( !vBase )
        return INF_ENGINE_ERROR_FUNC_NOT_INITIALIZED;
    
    if( !aArgs[0] || !aArgs[1] )
        return INF_ENGINE_ERROR_ARGC;
    
    bool not_in = !In::_apply( aArgs[0], aArgs + 1, aFunctionContext );            
            
    // Подготовка результата.
    if( not_in )
    {
        aResult = CTRUE;
        aLength = CTRUE_LEN;
    }
    else
    {
        aResult = CFALSE;
        aLength = CFALSE_LEN;
    }
    
    return INF_ENGINE_SUCCESS;
}
