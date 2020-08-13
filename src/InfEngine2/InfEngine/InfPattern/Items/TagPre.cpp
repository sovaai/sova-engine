#include "TagPre.hpp"

#include <cstring>

using namespace InfPatternItems;

unsigned int TagPre::GetNeedMemorySize( ) const
{
	unsigned int NeedMemorySize  = sizeof(uint32_t) + vTextLen + 1; // Длина текста + сам текст + терминатор.
								  
//	binary_data_alignment( NeedMemorySize );
	return NeedMemorySize;
}

unsigned int TagPre::Save( void * aBuffer ) const
{	
	char * ptr = static_cast<char*>(aBuffer);
        
        // Сохранение длины текста.
        *reinterpret_cast<uint32_t*>(ptr) = vTextLen;
        ptr += sizeof(uint32_t);
        
        // Сохранение текста.
        memcpy( ptr, vText, vTextLen );
        ptr += vTextLen;
        
        // Сохранение терминатора.
        *reinterpret_cast<char*>(ptr) = 0;
        ptr += sizeof(char);

	return static_cast<char*>( ptr ) - static_cast<char*>( aBuffer );
}
