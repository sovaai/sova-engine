#include "Text.hpp"

#include <cmath>

namespace InfPatternItems
{
	void InfPatternItems::Text::Set( const void * aBuffer )
	{
		vText = nullptr;
		vTextLength = 0;

		vNormalizedText = nullptr;
		vNormalizedTextLength = 0;

		vBuffer = static_cast<const char*>( aBuffer );
		vBufferSize = 0;

		vNeedMemorySize = 0;
	}

	void Text::Set( const void * aBuffer, unsigned int aBufferSize )
	{
		vText = nullptr;
		vTextLength = 0;

		vNormalizedText = nullptr;
		vNormalizedTextLength = 0;

		vBuffer = static_cast<const char*>( aBuffer );
		vBufferSize = aBufferSize;

		vNeedMemorySize = 0;
	}

	void Text::Set( const char * aText, unsigned int aTextLength )
	{
		vText = aText;
		if( vText )
			vTextLength = aTextLength;
		else
			vTextLength = 0;

		vBuffer = nullptr;
		vBufferSize = 0;

		vNeedMemorySize = 0;
	}

	InfEngineErrors Text::ApplySynonyms( SymbolymBaseRO & aSymbolymsBase, SynonymBaseRO & aSynonymsBase )
	{
		if( vBuffer )
			ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );
		
		// Подстановка символов.
        const char * tmp_text = nullptr;
		unsigned int tmp_text_length = 0;
		auto iee = aSymbolymsBase.ApplySymbolyms( GetTextString(), GetTextLength(), tmp_text, tmp_text_length );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		// Подстановка синонимов.
        if( INF_ENGINE_SUCCESS != ( iee = aSynonymsBase.ApplySynonyms( tmp_text, tmp_text_length, vNormalizedText, vNormalizedTextLength ) ) )
			ReturnWithTrace( iee );

		vNeedMemorySize = 0;

		return INF_ENGINE_SUCCESS;
	}

	unsigned int Text::GetNeedMemorySize() const
	{
		if( vNeedMemorySize )
			return vNeedMemorySize;
		else if( vBuffer )
		{
			if( vBufferSize )
				vNeedMemorySize = vBufferSize;
			else
			{
				vNeedMemorySize = 2 * sizeof( uint16_t ) + 1;
				vNeedMemorySize += std::ceil( (double)(
												  GetTextLength() +
												  GetNormalizedTextLength() +
												  2 ) / sizeof( uint32_t ) ) * sizeof( uint32_t );
			}

			return vNeedMemorySize;
		}
		else
		{
			vNeedMemorySize = 1; // Размер поля для хранения флага vStopWordsOnly.
			vNeedMemorySize += sizeof( uint16_t ); // Размер поля для хранения длины строки.
			vNeedMemorySize += vTextLength + 1; // Длина текстовой строки.

			vNeedMemorySize += sizeof( uint16_t ); // Размер поля для хранения длины нормализованной строки.
			vNeedMemorySize += vNormalizedTextLength + 1; // Длина нормализованной текстовой строки.

			vNeedMemorySize = std::ceil( ( (double)vNeedMemorySize ) / sizeof( uint32_t ) ) * sizeof( uint32_t );

			return vNeedMemorySize;
		}
	}

	unsigned int Text::Save( void * aBuffer ) const
	{
		// Проверка аргументов.
		if( !aBuffer )
			return 0;

		if( vBuffer )
		{
			memcpy( aBuffer, vBuffer, vNeedMemorySize );
			return vNeedMemorySize;
		}
		else
		{
			char * OriginalBuffer = static_cast<char*>( aBuffer );
			char * ptr = static_cast<char*>( aBuffer );

			// Сохранение длины текста.
			reinterpret_cast<uint16_t*>( ptr )[0] = vTextLength;
			ptr += sizeof( uint16_t );

			// Сохранение длины нормализованного текста.
			reinterpret_cast<uint16_t*>( ptr )[0] = vNormalizedTextLength;
			ptr += sizeof( uint16_t );

			// Сохранение текста.
			if( vTextLength )
				memcpy( ptr, vText, vTextLength );
			ptr += vTextLength;
			*( ptr++ ) = '\0';

			// Сохранение нормализованного текста.
			if( vNormalizedTextLength )
				memcpy( ptr, vNormalizedText, vNormalizedTextLength );
			ptr += vNormalizedTextLength;
			*( ptr++ ) = '\0';

			// Сохранение флага vStopWordsOnly.
			*( ptr++ ) = vStopWordsOnly;

			unsigned int Size = std::ceil( (double)(
											   1 +
											   2 * sizeof( uint16_t ) +
											   vTextLength + vNormalizedTextLength +
											   2 ) / sizeof( uint32_t ) ) * sizeof( uint32_t );
			ptr += Size - 1 - 2 * sizeof( uint16_t ) - vTextLength - vNormalizedTextLength - 2;

			return ptr - OriginalBuffer;
		}
	}
}
