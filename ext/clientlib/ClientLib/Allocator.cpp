#include <nMemoryAllocator/nMemoryAllocator.hpp>
#include "csDataPack.h"

/** Аллокатор памяти для обмена данными с сервером. */
nMemoryAllocator ClientLibAllocator;

extern "C" csdata_data_t * AllocateCSData( unsigned int aAttrNumber )
{
	// Выделение памяти под структуру.
	nAllocateNewObject( ClientLibAllocator, csdata_data_t, CSData );
	if( !CSData )
		return NULL;

	// Выделение памяти под элементы структуры.
	if( aAttrNumber > 0 )
	{
		CSData->attr_list = nAllocateObjects( ClientLibAllocator, csdata_attr_t, aAttrNumber );
		if( !( CSData->attr_list ) )
			return NULL;
	}
	else
		CSData->attr_list = NULL;

	CSData->attr_count = aAttrNumber;

	return CSData;
}

extern "C" void ClientLibAllocatorReset()
{
	ClientLibAllocator.Reset();
}

extern "C" unsigned int GetAllocatedMemorySize()
{
	return ClientLibAllocator.GetAllocatedMemorySize();
}

extern "C" csdata_data_t * ClientLibPackData( csdata_data_t * aSource, int aDataPackFlags )
{
	return csDataPack( aSource, ClientLibAllocator, aDataPackFlags );
}

extern "C" csdata_data_t * ClientLibUnPackData( csdata_data_t * aSource )
{
	return csDataUnPack( aSource, ClientLibAllocator, false );
}
