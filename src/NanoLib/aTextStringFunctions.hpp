#ifndef ATEXTSTRINGFUNCTIONS_HPP
#define ATEXTSTRINGFUNCTIONS_HPP

#include "aTextString.hpp"
#include "TextFuncs.hpp"

/** Преобразование к нижнему регистру. */
inline nlReturnCode SetLower( std::string & astring ) noexcept
{
	size_t srcOffset = 0;
	size_t dstOffset = 0;

	for(; srcOffset < astring.size(); )
	{
		// Выделение символа utf8.
		UChar32 ch;
		U8_NEXT_UNSAFE( const_cast<char*>( astring.c_str() ), srcOffset, ch );

		// Перевод его в верхний регистр.
		ch = u_tolower( ch );

		// Проверка необходимости сдвига строки.
		unsigned int SymbolLength = U8_LENGTH( ch );
		if( srcOffset - dstOffset < SymbolLength )
		{
			// Увеличение памяти.
			try
			{
				astring.resize( astring.size() + (SymbolLength - srcOffset + dstOffset ) );
			}
			catch( ... )
			{
				return nlrcErrorNoFreeMemory;
			}

			// Перенос данных.
			memmove( const_cast<char*>( astring.c_str() ) + dstOffset + SymbolLength, astring.c_str() + srcOffset, astring.size() - dstOffset - SymbolLength );

			srcOffset = dstOffset + SymbolLength;
		}
		else if( srcOffset - dstOffset > SymbolLength )
		{
			// Перенос данных.
			memmove( const_cast<char*>( astring.c_str() ) + dstOffset + SymbolLength, astring.c_str() + srcOffset, astring.size() - srcOffset );

			// Корректировка размер буфера.
			try
			{
				astring.resize( astring.size() - srcOffset + dstOffset - SymbolLength );
			}
			catch( ... )
			{
				return nlrcErrorNoFreeMemory;
			}
		}

		// Замена символа на верхний регстр.
		U8_APPEND_UNSAFE( const_cast<char*>( astring.c_str() ), dstOffset, ch );
	}

	return nlrcSuccess;
}

inline nlReturnCode SetLower( aTextString & astring ) noexcept
{
	return SetLower( astring.std_str() );
}

/** Преобразование к верхнему регистру. */
inline nlReturnCode SetUpper( std::string & astring ) noexcept
{
	size_t srcOffset = 0;
	size_t dstOffset = 0;

	for(; srcOffset < astring.size(); )
	{
		// Выделение символа utf8.
		UChar32 ch;
		U8_NEXT_UNSAFE( const_cast<char*>( astring.c_str() ), srcOffset, ch );

		// Перевод его в верхний регистр.
		ch = u_toupper( ch );

		// Проверка необходимости сдвига строки.
		unsigned int SymbolLength = U8_LENGTH( ch );
		if( srcOffset - dstOffset < SymbolLength )
		{
			// Увеличение памяти.
			try
			{
				astring.resize( astring.size() + (SymbolLength - srcOffset + dstOffset ) );
			}
			catch( ... )
			{
				return nlrcErrorNoFreeMemory;
			}

			// Перенос данных.
			memmove( const_cast<char*>( astring.c_str() ) + dstOffset + SymbolLength, astring.c_str() + srcOffset, astring.size() - dstOffset - SymbolLength );

			srcOffset = dstOffset + SymbolLength;
		}
		else if( srcOffset - dstOffset > SymbolLength )
		{
			// Перенос данных.
			memmove( const_cast<char*>( astring.c_str() ) + dstOffset + SymbolLength, astring.c_str() + srcOffset, astring.size() - srcOffset );

			// Корректировка размер буфера.
			try
			{
				astring.resize( astring.size() - srcOffset + dstOffset - SymbolLength );
			}
			catch( ... )
			{
				return nlrcErrorNoFreeMemory;
			}
		}

		// Замена символа на верхний регстр.
		U8_APPEND_UNSAFE( const_cast<char*>( astring.c_str() ), dstOffset, ch );
	}

	return nlrcSuccess;
}

inline nlReturnCode SetUpper( aTextString & astring ) noexcept
{
	return SetUpper( astring.std_str() );
}

/**
 *  Очистка пробелов. Несколько идущих подряд пробелов превращаются в один. Начальный и завершающий пробелы удаляются.
 * @param astring - строка для очистки.
 * @param ahead - флаг, показывающий, нужно ли удалять начальные пробелы в строке.
 * @param atail - флаг, показывающий, нужно ли удалять завершающие пробелы в строке.
 */
inline void AllTrim( std::string & astring, bool ahead = true, bool atail = true ) noexcept
{
	try
	{
		astring.resize( TextFuncs::AllTrim( const_cast<char*>( astring.c_str() ), astring.size(), ahead, atail ) );
	}
	catch( ... )
	{

	}
}

inline void AllTrim( aTextString & astring, bool ahead = true, bool atail = true ) noexcept
{
	AllTrim( astring.std_str(), ahead, atail );
}

/** Удаление эскейп символов. */
inline void RemoveEscapeSymbols( std::string & astring ) noexcept
{
	try
	{
		astring.resize( TextFuncs::RemoveEscapeSymbols( const_cast<char*>( astring.c_str() ), astring.size() ) );
	}
	catch( ... )
	{

	}
}

inline void RemoveEscapeSymbols( aTextString & astring ) noexcept
{
	RemoveEscapeSymbols( astring.std_str() );
}

#endif // ATEXTSTRINGFUNCTIONS_HPP

