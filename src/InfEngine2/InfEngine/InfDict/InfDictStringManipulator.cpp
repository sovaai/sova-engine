#include "InfDictStringManipulator.hpp"
#include "../DLData.hpp"

const PatternsStorage::Pattern InfDictStringManipulator::Get() const
{ 
	if( !vDLDataRO )
		return PatternsStorage::Pattern();
	
	return vDLDataRO->GetPatternFromStorage( GetId() ); 
}
