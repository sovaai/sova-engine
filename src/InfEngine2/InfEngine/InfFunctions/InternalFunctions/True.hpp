#ifndef __True_hpp__
#define __True_hpp__

#include <InfEngine2/Functions/FunctionLib.hpp>
#include <InfEngine2/InfEngine/CoreRO.hpp>
#include <NanoLib/LogSystem.hpp>

namespace InternalFunctions
{

    class True
    {
      public:
		static void Init( ExtICoreRO & aBase )
			{ vBase = &aBase; }

        static InfEngineErrors Apply( const char ** aArgs,
                const char *& aResult,
                unsigned int & aLength,
                nMemoryAllocator * aMemoryAllocaor,
                FunctionContext* aFunctionContext );
        
    public:
        
        static const DLFucntionArgInfo ARGS[];

        static const DLFucntionArgInfo VAR_ARGS;

        static const DLFunctionInfo DLF_INFO;
        
    private:
        static ExtICoreRO * vBase;
    };

}

#endif /** __True_hpp__ */
