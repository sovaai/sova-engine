#include "length.hpp"

#include <limits>

namespace InternalFunctions {
	const DLFucntionArgInfo Length::ARGS[] = {
		{ DLFAT_EXTENDED, nullptr, DLFAA_NONE }
	};

	const DLFunctionInfo Length::DLF_INFO = {
		"Length",
		"len",
		"Get length of first element.",
		DLFRT_TEXT,
		1,
		Length::ARGS,
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		DLFO_CACHE,
		DLFunctionInterfaceVersion,
		5,
		InfDataProtocolVersion,
		nullptr
	};

	InfEngineErrors Length::Apply( const char ** aArgs, const char* & aResult, unsigned int & aLength, nMemoryAllocator * aMemoryAllocator,
									  FunctionContext * /*aFunctionContext*/ ) {
		if( !aArgs[0] )
			return INF_ENGINE_ERROR_ARGC;

		std::uint32_t length{ 0 };
		std::int32_t offset{ 0 };
		while( static_cast<std::uint32_t>( offset ) < std::strlen( aArgs[0] ) ) {
			if( length == std::numeric_limits<std::uint32_t>::max() )
				return INF_ENGINE_ERROR_OUT_OF_RANGE;

			length++;

			UChar32 ch;
			U8_NEXT_UNSAFE( aArgs[0], offset, ch );
		}

		auto buffer = new (*aMemoryAllocator) char[std::numeric_limits<std::uint32_t>::digits10+1];
		aLength = sprintf( buffer, "%u", length );
		buffer[aLength] = '\0';
		aResult = buffer;

		return INF_ENGINE_SUCCESS;
	}
}
