#ifndef __In_hpp__
#define __In_hpp__

#include "InternalFunction.hpp"
#include "InfEngine2/_Include/Errors.h"
#include <InfEngine2/Functions/FunctionLib.hpp>
#include <InfEngine2/InfEngine/CoreRO.hpp>
#include <NanoLib/LogSystem.hpp>

namespace InternalFunctions
{
    
    class In
    {
    public:
		static void Init( ExtICoreRO & aBase )
			{ vBase = &aBase; }
        
        static InfEngineErrors Apply( const char ** aArgs,
                const char *& aResult,
                unsigned int & aLength,
                nMemoryAllocator * aMemoryAllocator, 
                FunctionContext * aFunctionContext );
        
    public:
        
        static const DLFucntionArgInfo ARGS[];
        
        static const DLFucntionArgInfo VAR_ARGS;

        static const DLFunctionInfo DLF_INFO;            
        
    protected:
        
        static bool _apply( const char * aSerializedVarId, const char ** aStrings, FunctionContext * aFunctionContext );
    
    private:
        static ExtICoreRO * vBase;
    };
    
}

#endif // __In_hpp__
