#include "FindEmail.hpp"

#include "../InternalFunction.hpp"
#include "contacts_finder.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo FindEmail::ARGS[] = { { DLFAT_EXTENDED, "" } };

const DLFunctionInfo FindEmail::DLF_INFO = {
	"FindEmail",
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

ExtICoreRO * FindEmail::vBase = nullptr;

InfEngineErrors FindEmail::Apply( const char ** aArgs, const char *& aResult,
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
	static std::string email;
	email.clear();
	while( finder.search( email ) != contacts_finder::eof )
	{
		if( finder.last_type() == contacts_finder::email )
			break;
		else if( finder.last_type() == contacts_finder::error )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	}
	
	// Выделяем память под результат.
	char* Buffer = static_cast<char*>( aMemoryAllocator->Allocate( email.size() + 1 ) );
	if( !Buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	
	// Копируем результат.
	memcpy( Buffer, email.c_str(), email.size() );
	Buffer[email.size()] = '\0';
	aResult = Buffer;
	aLength = email.size();
	
	return INF_ENGINE_SUCCESS;
}
