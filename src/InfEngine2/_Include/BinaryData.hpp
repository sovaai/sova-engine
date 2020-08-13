#ifndef __BinaryData_hpp__
#define __BinaryData_hpp__

#include <cstring>
#include <cstdlib>
#include <_include/_inttype.h>

/**
 * Функции для работы с сохраненными бинарно данными.
 */

template<class T>
inline unsigned int binary_data_alignment( T *& aBuffer, T * aBaseBuffer )
{
	unsigned int size = ( (char*)aBuffer - (char*)aBaseBuffer )%sizeof(uint32_t);
	aBuffer = (char*)aBuffer + ( size ? sizeof(uint32_t) - size : 0 );
	return (char*)aBuffer - (char*)aBaseBuffer;
}

template<typename S>
inline void binary_data_alignment( S & aMemorySize )
{
	aMemorySize += aMemorySize%sizeof(uint32_t) ? sizeof(uint32_t)-aMemorySize%sizeof(uint32_t) : 0;
}

template<class T, class B = char>
T * binary_data_save( B *& aBuffer, const T aValue )
{
	T * buffer = (T*)aBuffer;
	*buffer = aValue;
	aBuffer = (B*)(buffer + 1);
	return buffer;
}

template<class T, class B = char>
unsigned int binary_data_save_rs( B *& aBuffer, const T aValue )
{
	T * buffer = (T*)aBuffer;
	*buffer = aValue;
	aBuffer = (B*)(buffer + 1);
	return sizeof(T);
}

template<class T, class B = char>
T * binary_data_save( B *& aBuffer, const T * aValue, unsigned int aValueNumber )
{
	T * buffer = (T*)aBuffer;
    memcpy( (void*)buffer, (const void*)aValue, sizeof(T)*aValueNumber );
	aBuffer = (B*)( buffer + aValueNumber );
	return buffer;
}

template<class T, class B = char>
unsigned int binary_data_save_rs( B *& aBuffer, const T * aValue, unsigned int aValueNumber )
{
	T * buffer = (T*)aBuffer;
    memcpy( (void*)buffer, (const void*)aValue, sizeof(T)*aValueNumber );
	aBuffer = (B*)( buffer + aValueNumber );
	return sizeof(T)*aValueNumber;
}

template<class T, class B = char>
T * binary_data_skip( B *& aBuffer, unsigned int aValueNumber = 1 )
{
	T * buffer = (T*)aBuffer;
	aBuffer = (B*)( buffer + aValueNumber );
	return buffer;
}

template<class T, class B = char>
void binary_data_save_object( B *& aBuffer, T & aObject )
{
	aBuffer = (B*)((char*)aBuffer + aObject.Save( aBuffer ));
}


template<class T, class B = char>
T * binary_data_get( B * aBuffer, unsigned int aShift = 0 )
{
	return (T*)( (char*)aBuffer + aShift );
}


#endif /* __BinaryData_hpp__ */
