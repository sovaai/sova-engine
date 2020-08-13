#include "DictsNotMatch.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo DictsNotMatch::ARGS[] = {
    { DLFAT_EXTENDED, 0 },
    { DLFAT_TEXT, 0 },
};
    
const DLFucntionArgInfo DictsNotMatch::VAR_ARGS = { DLFAT_EXTENDED, 0 };

const DLFunctionInfo DictsNotMatch::DLF_INFO = {
    "DictsNotMatch",
    "",
    "Check if word or phrase obtained by first argument interpretation not contained in specified dictionary or dictionaries.",
    DLFRT_BOOL,
    2,
    DictsNotMatch::ARGS,
    { 0, 0, 0 },
    { 0, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    3,
    InfDataProtocolVersion,
    &VAR_ARGS,
};

ExtICoreRO * DictsNotMatch::vBase = nullptr;

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;


InfEngineErrors DictsNotMatch::Apply( const char ** /*aArgs*/,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * /*aMemoryAllocator*/, 
        FunctionContext * /*aFunctionContext*/ )
{   
	if( !vBase )
        return INF_ENGINE_ERROR_FUNC_NOT_INITIALIZED;
    
	LogError( "Function DictsNotMatch stub called" );
    
    aResult = "";
    aLength = 0;
    
    return INF_ENGINE_WARN_UNSUCCESS;
}
