#ifndef __InternalFunction_hpp__
#define __InternalFunction_hpp__

#include <InfEngine2/Functions/FunctionLib.hpp>
#include <InfEngine2/InfEngine/CoreRO.hpp>
#include "../../Session.hpp"

namespace InternalFunctions 
{	
	extern const DLFunctionInfo * FUNCTIONS[];

	extern unsigned int FUNCTIONS_COUNT;

	void InitFunctions( ExtICoreRO & aBase );

	FDLFucntion GetFunctionEntryPoint( const char * aFunctionName );

    unsigned int GetFunctionsNumber();
    
    const DLFunctionInfo *GetFunctionInfo( unsigned int aFunctionN );
    
	const DLFunctionInfo * GetFunctionInfo( const char * aFunctionName );
}

struct FunctionContext
{
	FunctionContext( Session * aSession, iAnswer * aAnswer ) :vSession( aSession ) ,vAnswer( aAnswer ) {}

	Session * vSession;
	iAnswer * vAnswer;
};
#endif /** __InternalFunction_hpp__ */
