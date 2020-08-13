#ifndef INF_ENGINE_FUNCTION_LENGTH_HPP
#define INF_ENGINE_FUNCTION_LENGTH_HPP

#include <InfEngine2/Functions/FunctionLib.hpp>

namespace InternalFunctions {
	class Length {
		public:
			static InfEngineErrors Apply( const char ** aArgs, const char* & aResult, unsigned int & aLength, nMemoryAllocator * aMemoryAllocator,
										  FunctionContext * aFunctionContext );

	public:
		static const DLFucntionArgInfo ARGS[];
		static const DLFunctionInfo DLF_INFO;
	};
}

#endif // INF_ENGINE_FUNCTION_LENGTH_HPP
