#ifndef __BelongsToExt_hpp__
#define __BelongsToExt_hpp__

#include <InfEngine2/InfEngine/InfFunctions/InternalFunctions/BelongsTo.hpp>

namespace InternalFunctions
{
    
    class BelongsToExt: public BelongsTo 
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

#endif // __BelongsToExt_hpp__
