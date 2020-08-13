#include "../InternalFunction.hpp"
#include "InfEngine2/_Include/Errors.h"
#include "FindContacts.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo FindContacts::ARGS[] = { { DLFAT_EXTENDED, "" } };

const DLFunctionInfo FindContacts::DLF_INFO = {
	"FindContacts",
	"",
	"Find contacts in given string",
	DLFRT_TEXT,
	1,
	ARGS,
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

ExtICoreRO * FindContacts::vBase = nullptr;

InfEngineErrors FindContacts::Apply( const char ** aArgs, const char *& aResult,
									 unsigned int & aLength, nMemoryAllocator * aMemoryAllocator,
									 FunctionContext * /*aFunctionContext*/ )
{
	return INF_ENGINE_SUCCESS;
}
