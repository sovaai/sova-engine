#include "FindPhone.hpp"

#include "../InternalFunction.hpp"
#include "contacts_finder.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo FindPhone::ARGS[] = { { DLFAT_EXTENDED, "" } };

const DLFunctionInfo FindPhone::DLF_INFO = {
	"FindPhone",
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

ExtICoreRO * FindPhone::vBase = nullptr;

InfEngineErrors FindPhone::Apply( const char ** aArgs, const char *& aResult,
								  unsigned int & aLength, nMemoryAllocator * aMemoryAllocator,
								  FunctionContext * /*aFunctionContext*/ )
{
	// Проверка аргументов.
	const char * request = nullptr;
	if( !aArgs[0] || aArgs[0][0] == '\0' )
		// Аргумент по умолчанию.
		request = ARGS[0].default_value;
	else if( aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;
	else
		request = aArgs[0];

	contacts_finder finder( request );
	static std::string phone;
	phone.clear();
	while( finder.search( phone ) != contacts_finder::eof )
	{
		if( finder.last_type() == contacts_finder::phone )
			break;
		else if( finder.last_type() == contacts_finder::error )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	}

	// Выделяем память под результат.
	char* Buffer = static_cast<char*>( aMemoryAllocator->Allocate( phone.size() + 1 ) );
	if( !Buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Копируем результат.
	memcpy( Buffer, phone.c_str(), phone.size() );
	Buffer[phone.size()] = '\0';
	aResult = Buffer;
	aLength = phone.size();
	
	return INF_ENGINE_SUCCESS;
}
