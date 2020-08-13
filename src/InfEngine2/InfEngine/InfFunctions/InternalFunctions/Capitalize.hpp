#ifndef __Capitalize_hpp__
#define __Capitalize_hpp__

#include <InfEngine2/Functions/FunctionLib.hpp>
#include <InfEngine2/InfEngine/CoreRO.hpp>
#include <NanoLib/LogSystem.hpp>

#include "InternalFunction.hpp"

#include <NanoLib/NameIndex.hpp>

namespace InternalFunctions
{
    
    class Capitalize
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
        
    private:
        
        static void InitTable();
        
    private:
        static ExtICoreRO * vBase;
    };
    
}

#endif // __Capitalize_hpp__
