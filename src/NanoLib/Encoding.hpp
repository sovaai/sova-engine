#ifndef ENCODING_HPP
#define ENCODING_HPP

#include <_include/CodePages.h>

#include "aTextString.hpp"

#include <InfEngine2/_Include/Errors.h>

namespace NanoLib
{
	enum class Encoding
	{
		UTF8,
		CP1251
	};

	inline nlReturnCode ConvertEncoding( aTextString & astring, Encoding afrom, Encoding ato )
	{
		if( afrom == ato )
			return nlrcSuccess;
		else if( afrom == Encoding::CP1251 && ato == Encoding::UTF8 )
		{
			// Выделение памяти для ковертации строки из cp1251 в utf8.
			unsigned int size = astring.size();
			if( astring.resize( astring.size() * 6 + 1 ) != nlrcSuccess )
				ReturnWithError( nlrcErrorNoFreeMemory, nlReturnCodeToString( nlrcErrorNoFreeMemory ) );
			bzero( astring.str() + size, astring.size() - size );

			// Преобразование из CP1251 в UTF-8.
			if( Convert2UTF_Ex( reinterpret_cast<unsigned char*>( astring.str() ), astring.size(), size, CP_CYRILLIC ) != (int)size )
				ReturnWithWarn( nlrcErrorInvArgs, "Can't convert string From CP1251 to UTF-8: %s", astring.c_str() );

			// Вычисление длины получившейся строки.
			astring.resize( std::strlen( astring.c_str() ) );
			
			return nlrcSuccess;
		} else
			ReturnWithError( nlrcErrorInvArgs, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
	}
}

#endif // ENCODING_HPP

