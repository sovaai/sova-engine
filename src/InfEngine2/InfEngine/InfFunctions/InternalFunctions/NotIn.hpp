#ifndef __NotIn_hpp__
#define __NotIn_hpp__

#include "In.hpp"

namespace InternalFunctions
{
    
    class NotIn: public InternalFunctions::In
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
        static ExtICoreRO * vBase;
    };
    
}

#endif // __NotIn_hpp__
