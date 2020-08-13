#include "NanoRandomBox.hpp"

#include <cstdlib>
#include <cstdint>
#include <cstring>

#include <InfEngine2/_Include/BinaryData.hpp>

NanoRandomBox::NanoRandomBox( unsigned int aCount )
{    
	Init( aCount );
}

bool NanoRandomBox::Init( unsigned int aCount )
{    
	vLeftInBox = aCount;
	vMaskSize = aCount;
	if( aCount )
	{
		vMask.resize( ( aCount + 7 ) >> 3 );
		if( vMask.no_memory() )
			return false;
		memset( vMask.get_buffer(), static_cast<unsigned char>(-1), vMask.size() );
		vQueue.reserve( aCount );
		if( vQueue.no_memory() )
			return false;

		Refresh();
	}

	return true;
}

void NanoRandomBox::Refresh()
{
	vQueue.clear();

	for( unsigned int pos = 0; pos < vMaskSize; ++pos )
		if( vMask[pos>>3] & ( 1 << (pos&7) )  )
			vQueue.push_back( pos );

	unsigned int tmp;
	if( vQueue.size() )
	{
		for( unsigned int pos = 0; pos < vQueue.size() - 1; ++pos )
		{
			unsigned int n = pos + rand() % ( vQueue.size() - pos );
			if( n != pos )
			{
				tmp = vQueue[pos];
				vQueue[pos] = vQueue[n];
				vQueue[n] = tmp;
			}
		}
	}

	vLeftInBox = vQueue.size();
}

void NanoRandomBox::Reset()
{
	Init( vMaskSize );
}

unsigned int NanoRandomBox::GetCount() const
{
	return vLeftInBox;
}

bool NanoRandomBox::IsEmpty() const
{
	return !vLeftInBox;
}

unsigned int NanoRandomBox::GetRemainingAmount() const
{
	return vLeftInBox;
}

bool NanoRandomBox::IsInBox( unsigned int aNum ) const
{
	return aNum < vMaskSize ? vMask[aNum>>3] & ( 1 << (aNum&7) ) : false;
}

bool NanoRandomBox::TakeFromBox( unsigned int aNum )
{    
	if( aNum >= vMaskSize )
		return false;

	vMask[aNum>>3] = vMask[aNum>>3] & ~( 1 << (aNum&7) );
	Refresh();
	return true;
}

void NanoRandomBox::RetriveToBox( unsigned int aNum )
{
	vMask[aNum>>3] = vMask[aNum>>3] | ( 1 << (aNum&7) );
	Refresh();
}

unsigned int NanoRandomBox::TakeFromBox()
{
	if( !vLeftInBox )
		return static_cast<unsigned int>(-1);

	unsigned int val = vQueue[vQueue.size() - vLeftInBox--];
	vMask[val>>3] = vMask[val>>3] & ~( 1 << (val&7) );
	return val;
}

unsigned int NanoRandomBox::GetNeedMemorySize() const
{
	return sizeof(uint32_t) + vMask.size();
}

unsigned int NanoRandomBox::Save( void * aBuffer ) const
{
	char * ptr = static_cast<char*>(aBuffer);
	binary_data_save<uint32_t>( ptr, vMaskSize );
	binary_data_save( ptr, vMask.get_buffer(), vMask.size() );
	return ptr - static_cast<char*>( aBuffer );
}

NanoRandomBox::ReturnCode NanoRandomBox::Load( const void * aBuffer, unsigned int aBufferSize )
{
	if( aBufferSize < sizeof( uint32_t ) )
		return RT_INVALID_DATA;

    vMaskSize = *reinterpret_cast<const uint32_t*>(aBuffer);
	vMask.resize( ( vMaskSize + 7 ) >> 3 );
	if( vMask.no_memory() )
		return RT_NO_MEMORY;
	vQueue.reserve( vMaskSize );
	if( vQueue.no_memory() )
		return RT_NO_MEMORY;

	if( vMask.size() * sizeof(uint8_t) + sizeof(uint32_t) > aBufferSize )
		return RT_INVALID_DATA;

    memcpy( vMask.get_buffer(), reinterpret_cast<const char*>(aBuffer)+sizeof(uint32_t), vMask.size() );
	Refresh();

	return RT_OK;
}
