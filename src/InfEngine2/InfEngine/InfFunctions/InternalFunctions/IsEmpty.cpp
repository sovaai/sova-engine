#include "IsEmpty.hpp"
#include "InfEngine2/_Include/Errors.h"

using namespace InternalFunctions;

const DLFucntionArgInfo IsEmpty::ARGS[] = {
        { DLFAT_EXTENDED, 0 }
    };

const DLFunctionInfo IsEmpty::DLF_INFO = {
    "IsEmpty",
    "?",
    "Returns True if given argument is empty, else returns empty string.",
    DLFRT_BOOL,
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

ExtICoreRO * IsEmpty::vBase = nullptr;

static const char gTRUE[] = "True";
static const unsigned int gTRUE_LEN = 4;

static const char * gFALSE = "";
static const unsigned int gFALSE_LEN = 0;


InfEngineErrors IsEmpty::Apply( const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * /*aMemoryAllocator*/,
        FunctionContext * aSession  )
{    
    if( !aArgs[0] || aArgs[1] )
        return INF_ENGINE_ERROR_ARGC;
    
    if( aArgs[0][0] == '\0' )
    {
        aResult = gTRUE;
        aLength = gTRUE_LEN;
    }
    else
    {
        aResult = gFALSE;
        aLength = gFALSE_LEN;
    }
    return INF_ENGINE_SUCCESS;
}
