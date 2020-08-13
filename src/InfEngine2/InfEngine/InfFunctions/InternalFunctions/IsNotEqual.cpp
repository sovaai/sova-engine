#include "InfEngine2/_Include/Errors.h"
#include "IsNotEqual.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo IsNotEqual::ARGS[] = {
    { DLFAT_EXTENDED, 0},
    { DLFAT_EXTENDED, 0}
};

const DLFunctionInfo IsNotEqual::DLF_INFO = {
    "IsNotEqual",
    "!=",
    "Return True if given strings is equal and empty string in another way.",
    DLFRT_BOOL,
    2,
    ARGS,
    { 3, 0, 1},
    { 3, 0, 1}
    , DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,
    InfDataProtocolVersion,
    nullptr
};

ExtICoreRO * IsNotEqual::vBase = nullptr;

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;

InfEngineErrors IsNotEqual::Apply(const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * /*aMemoryAllocator*/,
        FunctionContext * aFunctionContext)
{
    if (!aArgs[0] || !aArgs[1] || aArgs[2])
        return INF_ENGINE_ERROR_ARGC;

    aResult = nullptr;
    aLength = 0;

    if (strcmp(aArgs[0], aArgs[1]))
    {
        aResult = CTRUE;
        aLength = CTRUE_LEN;
    } else
    {
        aResult = CFALSE;
        aLength = CFALSE_LEN;
    }

    return INF_ENGINE_SUCCESS;
}
