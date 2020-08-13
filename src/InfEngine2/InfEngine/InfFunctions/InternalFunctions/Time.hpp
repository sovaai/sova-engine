#ifndef __Time_hpp__
#define __Time_hpp__

#include <InfEngine2/Functions/FunctionLib.hpp>
#include <InfEngine2/InfEngine/CoreRO.hpp>
#include <NanoLib/LogSystem.hpp>

#include "InternalFunction.hpp"

namespace InternalFunctions
{
	class Time
	{
	public:
		static void Init( ExtICoreRO & aBase )
			{ vBase = &aBase; }

		static InfEngineErrors Apply( const char ** aArgs, const char *& aResult, unsigned int & aLength,
				nMemoryAllocator * aMemoryAllocator, FunctionContext * aFunctionContext );

	public:
		static const DLFucntionArgInfo ARGS[];
		static const DLFucntionArgInfo VAR_ARGS;
		static const DLFunctionInfo DLF_INFO;

	private:
		static ExtICoreRO * vBase;
	};
}

#endif /** __Time_hpp__ */
