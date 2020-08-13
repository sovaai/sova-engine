#include "InfInternalFunction.hpp"

#include "InternalFunctions/InternalFunction.hpp"


InfEngineErrors InternalInfFunctionManipulator::Load()
{
	vDLFunctionInfo = InternalFunctions::GetFunctionInfo( GetName() );
	if( !vDLFunctionInfo )
		return INF_ENGINE_ERROR_DLF;

	vFunction = InternalFunctions::GetFunctionEntryPoint( GetName() );
	if( !vFunction )
		return INF_ENGINE_ERROR_ENTRY_POINT;

	return INF_ENGINE_SUCCESS;
}
