#include "DictsMatchExt.hpp"


using namespace InternalFunctions;

const DLFucntionArgInfo DictsMatchExt::ARGS[] = {
    { DLFAT_EXTENDED, 0 },
    { DLFAT_TEXT, 0 },
    { DLFAT_EXTENDED, 0 },
};
    
const DLFucntionArgInfo DictsMatchExt::VAR_ARGS = { DLFAT_EXTENDED, 0 };
    
const DLFunctionInfo DictsMatchExt::DLF_INFO = {
    "DictsMatchExt",
    "",
    "Check if word or phrase obtained by first argument interpretation contained in specified dictionary or dictionaries. Return first line of matched dictionary in given variable",
    DLFRT_BOOL,
    3,
    DictsMatchExt::ARGS,
    { 0, 0, 0 },
    { 0, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,
    InfDataProtocolVersion,
    &VAR_ARGS,
};

ExtICoreRO * DictsMatchExt::vBase = nullptr;

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;


InfEngineErrors DictsMatchExt::Apply( const char ** /*aArgs*/,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * /*aMemoryAllocator*/, 
        FunctionContext * /*aFunctionContext*/ )
{   
	if( !vBase )
        return INF_ENGINE_ERROR_FUNC_NOT_INITIALIZED;
    
	LogError( "Function DictsMatchExt stub called" );
    
    aResult = "";
    aLength = 0;
    
    return INF_ENGINE_WARN_UNSUCCESS;
}
