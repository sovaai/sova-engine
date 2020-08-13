#include "DictsMatch.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo DictsMatch::ARGS[] = {
    { DLFAT_EXTENDED, 0 },
    { DLFAT_EXTENDED, 0 },
};
    
const DLFucntionArgInfo DictsMatch::VAR_ARGS = { DLFAT_EXTENDED, 0 };
    
const DLFunctionInfo DictsMatch::DLF_INFO = {
    "DictsMatch",
    "",
    "Check if word or phrase obtained by first argument interpretation contained in specified dictionary or dictionaries.",
    DLFRT_BOOL,
    2,
    DictsMatch::ARGS,
    { 0, 0, 0 },
    { 0, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,
    InfDataProtocolVersion,
    &VAR_ARGS,
};

ExtICoreRO * DictsMatch::vBase = nullptr;

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;


InfEngineErrors DictsMatch::Apply( const char ** /*aArgs*/,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * /*aMemoryAllocator*/, 
        FunctionContext * /*aFunctionContext*/ )
{   
	if( !vBase )
        return INF_ENGINE_ERROR_FUNC_NOT_INITIALIZED;
    
	LogError( "Function DictsMatch stub called" );
    
    aResult = "";
    aLength = 0;
    
    return INF_ENGINE_WARN_UNSUCCESS;
}
