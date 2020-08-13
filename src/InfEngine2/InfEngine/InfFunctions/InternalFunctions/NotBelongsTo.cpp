#include "NotBelongsTo.hpp"
#include "InfEngine2/_Include/Errors.h"

using namespace InternalFunctions;

const DLFucntionArgInfo NotBelongsTo::ARGS[] = {
    { DLFAT_EXTENDED, 0 },
    { DLFAT_TEXT, 0 },
};
    
const DLFucntionArgInfo NotBelongsTo::VAR_ARGS = { DLFAT_EXTENDED, 0 };

const DLFunctionInfo NotBelongsTo::DLF_INFO = {
    "NotBelongsTo",
    "",
    "Check if word or phrase obtained by first argument interpretation not contained in specified dictionary or dictionaries.",
    DLFRT_BOOL,
    2,
    NotBelongsTo::ARGS,
    { 0, 0, 0 },
    { 0, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    3,
    InfDataProtocolVersion,
    &VAR_ARGS,
};

ExtICoreRO * NotBelongsTo::vBase = nullptr;

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;


InfEngineErrors NotBelongsTo::Apply( const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * aMemoryAllocator,
        FunctionContext * aFunctionContext )
{    
    InfEngineErrors iee = BelongsTo::Apply( aArgs, aResult, aLength, aMemoryAllocator, aFunctionContext );
    if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );
   
    if( !aLength )
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
