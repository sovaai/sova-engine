#include "True.hpp"
#include "InfEngine2/_Include/Errors.h"

using namespace InternalFunctions;

const DLFucntionArgInfo True::ARGS[] = {
        { DLFAT_EXTENDED, 0 }
    };

const DLFunctionInfo True::DLF_INFO = {
    "True",
    "",
    "Returns True.",
    DLFRT_BOOL,
    0,
    nullptr,
    { 3, 0, 1 },
    { 3, 0, 1 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,
    InfDataProtocolVersion,
    nullptr
    };

ExtICoreRO * True::vBase = nullptr;

static const char gTRUE[] = "True";
static const unsigned int gTRUE_LEN = 4;

static const char* gFALSE = "";
static const unsigned int gFALSE_LEN = 0;


InfEngineErrors True::Apply( const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * /*aMemoryAllocaor*/,
        FunctionContext * aSession  )
{
    aResult = gTRUE;
    aLength = gTRUE_LEN;

    return INF_ENGINE_SUCCESS;
}
