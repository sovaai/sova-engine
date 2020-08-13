#ifndef __NotBelongsTo_hpp__
#define __NotBelongsTo_hpp__

#include "BelongsTo.hpp"

namespace InternalFunctions
{
    
    class NotBelongsTo: public BelongsTo
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

#endif /** __NotBelongsTo_hpp__ */
