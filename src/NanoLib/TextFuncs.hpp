#ifndef __TextFuncs_hpp__
#define __TextFuncs_hpp__

#include <unicode/uchar.h>

#include <_include/_string.h>
#include <lib/aptl/avector.h>

#include "ReturnCode.hpp"

/**
 *  Функции для работы с текстом и символами.
 */
namespace TextFuncs
{
	/**
	 *  Понижение регистра текста.
	 * @param aString - текстовая строка.
	 * @param aStringLength - длина текстовой строки в байтах.
	 * @param aStringSize - размер строки в байтах.
	 * @param aStartPos - начальная позиция преобразования регистра.
	 * @param aEndPos - позиция окончания преобразования регистра. Если она равна 0, то преобразование производится до конца строки.
	 */
	inline nlReturnCode ToLower( char* aString, unsigned int aStringLength, unsigned int aStringSize = 0,
			unsigned int aStartPos = 0, unsigned int aEndPos = 0 );

	/**
	 *  Повышение регистра текста.
	 * @param aString - текстовая строка.
	 * @param aStringLength - длина текстовой строки в байтах.
	 * @param aStringSize - размер строки в байтах.
	 * @param aStartPos - начальная позиция преобразования регистра.
	 * @param aEndPos - позиция окончания преобразования регистра. Если она равна 0, то преобразование производится до конца строки.
	 */
	inline nlReturnCode ToUpper( char* aString, unsigned int aStringLength, unsigned int aStringSize = 0,
			unsigned int aStartPos = 0, unsigned int aEndPos = 0 );

	/**
	 *  Определение пробельного символа.
	 * @param aCh - символ, который нужно проверить.
	 */
	inline bool IsSpace( char aCh );

	inline bool IsAlpha( char aCh )
	{
		return ( ( 'a' <= aCh && aCh <= 'z' ) || ( 'A' <= aCh && aCh <= 'Z' ) ) ? true : false;
	}

	inline bool IsDigit( char aCh )
	{
		return ( '0' <= aCh && aCh <= '9' ) ? true : false;
	}

	inline bool IsAlNum( char aCh )
	{
		return IsAlpha( aCh ) | IsDigit( aCh );
	}

	inline bool IsAlNumUTF8( UChar32 aCh )
	{
		return u_isalnum( aCh );
	}

	/**
	 *  Очистка пробельных символов. Все последовательности пробельных символов заменяются на ' '.
	 * @param aString - строка с текстом.
	 * @param aStringLength - длина строки в байтах. Если длина равна -1, то считается, что строка заканчивается '\0'.
	 * @param aDeleteHeadSpace - флаг, показывающий, нужно ли удалять начальные пробелы в строке.
	 * @param aDeleteTailSpace - флаг, показывающий, нужно ли удалять завершающие пробелы в строке.
	 */
	inline unsigned int AllTrim( char* aString, unsigned int aStringLength = -1, bool aDeleteHeadSpace = true, bool aDeleteTailSpace = true,
								 bool aDeleteInsideSpace = false );

	/**
	 *  Очистка пробельных символов. Все последовательности пробельных символов заменяются на ' '. Завершающие пробельные символы удаляются.
	 * @param aString - строка с текстом.
	 * @param aStringLength - длина строки в байтах. Если длина равна -1, то считается, что строка заканчивается '\0'.
	 */
	inline unsigned int RightTrim( char* aString, unsigned int aStringLength = -1 )
	{
		return AllTrim( aString, aStringLength, false, true );
	}

	/**
	 *  Пропуск пробелов.
	 * @param aString - строка с текстом.
	 * @param aStartPos - начальная позиция.
	 * @param aStringLength - длина строки в байтах. Если длина равна -1, то считается, что строка заканчивается '\0'.
	 */
	inline unsigned int SkipSpaces( const char * aString, unsigned int aStartPos, unsigned int aStringLength = -1 );


	/**
	 *  Проверка строки на эквивалентность пустой строке с точностью до пробельных символов.
	 * @param aString - строка с текстом.
	 * @param aStringLength - длина строки в байтах. Если длина равна -1, то считается, что строка заканчивается '\0'.
	 */
	inline bool IsEmpty( const char * aString, unsigned int aStringLength = -1 );


	/** Поиск первого не заэскейпленного символа aSymbols в буффере. **/
	inline unsigned int FindSymbolTrue( const char * aBuffer, unsigned int aBufferSize, const char * aSymbols, unsigned int aStartPos );

	/** Поиск первого не заэскейпленного символа, не входящего в aSymbols, в буффере. **/
	inline unsigned int FindNotSymbolTrue( const char * aBuffer, unsigned int aBufferSize, const char * aSymbols, unsigned int aStartPos );

	inline unsigned int FindSymbol( const char * aBuffer, unsigned int aBufferSize, const char * aSymbols, unsigned int aStartPos );

	/** Нахождение первого эскейп символа, не описанного в aSymbols. **/
	inline unsigned int FindRestrictedSymbol( const char * aBuffer, unsigned int aBufferSize, const char * aSymbols, unsigned int aStartPos );

	/** Разэскейпливание. **/
	inline unsigned int RemoveEscapeSymbols( char* aBuffer, unsigned int aBufferLen );

	/** Прочитать число из строки. В случае ошибки возвращает 0 */
	inline int GetInteger( const char * aStr, unsigned int & aPos );
	inline int GetInteger( const char * aStr, unsigned int aStrLenght, unsigned int & aPos );

	/** Прочитать число из строки. В случае ошибки возвращает 0 */
	inline double GetFloat( const char * aStr, unsigned int & aPos );
	inline double GetFloat( const char * aStr, unsigned int aStrLenght, unsigned int & aPos );

	/** Прочитать положительное число из строки. В случае ошибки возвращает 0 */
	inline int GetUnsigned( const char * aStr, unsigned int & aPos );
	inline int GetUnsigned( const char * aStr, unsigned int aStrLenght, unsigned int & aPos );

	/** Прочитать часовой пояс в формате MSK{+|-}NUMBER[:NUMBER]. Указывается относительно Москвы. Возвращает значение в секундах. В случае ошибки возвращает 0. */
	inline int GetTimeZone( const char * aStr, unsigned int & aPos );
	inline int GetTimeZone( const char * aStr, unsigned int aStrLenght, unsigned int & aPos );

	/** Преобразовать строку в число. В случае ошибки возвращает 0 */
	inline long int ToInteger( const char * aStr, bool* aSuccess = nullptr );

	/** Разбирает строку с датой в формате ДД.ММ.ГГГГ или ДД.ММ */
	inline bool ParseDate( const char * date, unsigned int & day, unsigned int & month, unsigned int & year, bool & aDateIsShort );
	inline bool ParseDate( const char * date, unsigned int length, unsigned int & pos, unsigned int & day, unsigned int & month, unsigned int & year, bool & aDateIsShort, bool aMakeChecks = true, bool aSkipSpaces = false );

	/** Разбирает строку со временем в формате ЧЧ:ММ:СС или ЧЧ:ММ */
	inline bool ParseTime( const char * time, unsigned int & hour, unsigned int & minute, unsigned int & second, bool & aTimeIsShort );
	inline bool ParseTime( const char * time, unsigned int length, unsigned int & pos, unsigned int & hour, unsigned int & minute, unsigned int & second, bool & aTimeIsShort, bool aMakeChecks = true, bool aSkipSpaces = false );

	/**
	 *  Проверяет, являеются ли слова aWords префиксами строки aStr. Номер наиболее длинного из подходящих слов.
	 * @param aStr - строка.
	 * @param aStrLength - длина строки.
	 * @param aWords - список слов.
	 * @param aWordlengths - список длин слов.
	 * @param aWordsCount - количество слов в списке.
	 * @param aResult - список булевских значений, показывающих, является ли i-ое слово префиксом заданной строки.
	 */
	inline unsigned int ParseWord( const char * aStr, unsigned int aStrLength,
			const char ** aWords, const unsigned int * aWordLengths,
			unsigned int aWordsCount, bool * aResults );
}

inline nlReturnCode TextFuncs::ToLower( char* aString, unsigned int aStringLength, unsigned int aStringSize,
		unsigned int aStartPos, unsigned int aEndPos )
{
	// Проверка аргументов.
	if( !aString )
		return nlrcErrorInvArgs;

	// Нормализация аргументов.
	if( aEndPos == 0 || aEndPos > aStringLength )
		aEndPos = aStringLength;
	if( aStartPos >= aStringLength || aEndPos <= aStartPos )
		return nlrcSuccess;
	if( aStringSize <= aStringLength )
		aStringSize = aStringLength + 1;

	// Внутренний буффер для работы с текстом.
	static avector<char> TmpBuffer;

	// Выделение памяти.
	TmpBuffer.resize( ( aEndPos - aStartPos ) * 4 + 1 );
	if( TmpBuffer.no_memory() )
		return nlrcErrorNoFreeMemory;

	unsigned int srcOffset = aStartPos;
	unsigned int dstOffset = 0;

	for(; srcOffset < aEndPos; )
	{
		UChar32 ch;
		U8_NEXT_UNSAFE( aString, srcOffset, ch );

		ch = u_tolower( ch );

		if( dstOffset + U8_LENGTH( ch ) < TmpBuffer.size() )
		{
			U8_APPEND_UNSAFE( TmpBuffer.get_buffer(), dstOffset, ch );
		}
	}

	// Проверка размера строки.
	if( dstOffset > srcOffset - aStartPos )
	{
		if( aStringLength + dstOffset - ( srcOffset - aStartPos ) >= aStringSize )
			return nlrcErrorBufferTooSmall;
		else
			memmove( aString + aStartPos + dstOffset, aString + srcOffset, aStringLength - srcOffset + 1 );
	}
	memcpy( aString + aStartPos, TmpBuffer.get_buffer(), dstOffset );

	return nlrcSuccess;
}

inline nlReturnCode TextFuncs::ToUpper( char* aString, unsigned int aStringLength, unsigned int aStringSize,
		unsigned int aStartPos, unsigned int aEndPos )
{
	// Проверка аргументов.
	if( !aString )
		return nlrcErrorInvArgs;

	// Нормализация аргументов.
	if( aEndPos == 0 || aEndPos > aStringLength )
		aEndPos = aStringLength;
	if( aStartPos >= aStringLength || aEndPos <= aStartPos )
		return nlrcSuccess;
	if( aStringSize <= aStringLength )
		aStringSize = aStringLength + 1;

	// Внутренний буффер для работы с текстом.
	static avector<char> TmpBuffer;

	// Выделение памяти.
	TmpBuffer.resize( ( aEndPos - aStartPos ) * 4 + 1 );
	if( TmpBuffer.no_memory() )
		return nlrcErrorNoFreeMemory;

	unsigned int srcOffset = aStartPos;
	unsigned int dstOffset = 0;

	for(; srcOffset < aEndPos; )
	{
		UChar32 ch;
		U8_NEXT_UNSAFE( aString, srcOffset, ch );

		ch = u_toupper( ch );

		if( dstOffset + U8_LENGTH( ch ) < TmpBuffer.size() )
		{
			U8_APPEND_UNSAFE( TmpBuffer.get_buffer(), dstOffset, ch );
		}
	}

	// Проверка размера строки.
	if( dstOffset > srcOffset - aStartPos )
	{
		if( aStringLength + dstOffset - ( srcOffset - aStartPos ) >= aStringSize )
			return nlrcErrorBufferTooSmall;
		else
			memmove( aString + aStartPos + dstOffset, aString + srcOffset, aStringLength - srcOffset + 1 );
	}
	memcpy( aString + aStartPos, TmpBuffer.get_buffer(), dstOffset );

	return nlrcSuccess;
}

inline bool TextFuncs::IsSpace( char aCh )
{
	return ( aCh == ' ' || aCh == '\t' || aCh == '\n' || aCh == '\r' ) ? true : false;
}

inline unsigned int TextFuncs::AllTrim( char* aString, unsigned int aStringLength, bool aDeleteHeadSpace, bool aDeleteTailSpace, bool aDeleteInsideSpace ) {

	// Проверка аргументов.
	if( !aString && aStringLength )
		return 0;

	// Вычисление длины строки.
	if( aStringLength == static_cast<unsigned int>( -1 ) )
		aStringLength = strlen( aString );

	// Проход по всем символам в строке.
	int32_t LastPos = 0, CurrentPos = 0, DstPos = 0;
	while( static_cast<uint32_t>( CurrentPos ) < aStringLength ) {
		// Выделение символа UTF.
		UChar32 ch;
		U8_NEXT( aString, CurrentPos, (int32_t)aStringLength, ch );
		if( ch == U_SENTINEL ) {
			// Некорректный UTF.
			break;
		}

		// Проверка на пробельные символы.
		if( u_isspace( ch ) )
			continue;

		// Перемещение обработанных символов.
		if( CurrentPos != LastPos + U8_LENGTH( ch ) && ( DstPos == 0 ? !aDeleteHeadSpace : !aDeleteInsideSpace ) )
			U8_APPEND_UNSAFE( aString, DstPos, static_cast<UChar32>( ' ' ) );
		U8_APPEND_UNSAFE( aString, DstPos, ch );

		LastPos = CurrentPos;
	}

	if( CurrentPos != LastPos && !aDeleteTailSpace )
		U8_APPEND_UNSAFE( aString, DstPos, static_cast<UChar32>( ' ' ) );

	bzero( aString + DstPos, aStringLength - DstPos );

	return DstPos;
}

inline unsigned int TextFuncs::SkipSpaces( const char * aString, unsigned int aStartPos, unsigned int aStringLength )
{
	// Проверка аргументов.
	if( !aString && aStringLength )
		return aStringLength;
	else if( aStartPos >= aStringLength )
		return aStartPos;

	// Вычисление длины строки.
	if( aStringLength == (unsigned int) -1 )
		aStringLength = strlen( aString );

	// Проход по всем символам в строке.
	int32_t CurrentPos = aStartPos;
	while( (uint32_t)CurrentPos < aStringLength )
	{
		// Выделение символа UTF.
		UChar32 ch;
		U8_NEXT( aString, CurrentPos, (int32_t)aStringLength, ch );
		if( ch == U_SENTINEL )
		{
			// Некорректный UTF.
			return CurrentPos;
		}

		// Проверка на пробельные символы.
		if( !u_isspace( ch ) )
			return CurrentPos - U8_LENGTH( ch );
	}

	// Подавление предупреждения.
	return aStringLength;
}

inline bool TextFuncs::IsEmpty( const char * aString, unsigned int aStringLength )
{
	// Проверка аргументов.
	if( !aString && aStringLength )
		return true;

	// Вычисление длины строки.
	if( aStringLength == (unsigned int) -1 )
		aStringLength = strlen( aString );

	// Проход по всем символам в строке.
	int32_t CurrentPos = 0;
	while( (uint32_t)CurrentPos < aStringLength )
	{
		// Выделение символа UTF.
		UChar32 ch;
		U8_NEXT( aString, CurrentPos, (int32_t)aStringLength, ch );
		if( ch == U_SENTINEL )
		{
			// Некорректный UTF.
			return true;
		}

		// Проверка на пробельные символы.
		if( !u_isspace( ch ) )
			return false;
	}

	return false;
}

inline unsigned int TextFuncs::FindSymbol( const char * aBuffer, unsigned int aBufferSize, const char * aSymbols, unsigned int aStartPos )
{
	// Проверка аргументов.
	if( !aBuffer || !aSymbols )
		return aBufferSize;

	unsigned int res = aStartPos;
    for(; res < aBufferSize && !strchr( aSymbols, aBuffer[res] ); res++ );

	return res;
}

inline unsigned int TextFuncs::FindSymbolTrue( const char * aBuffer, unsigned int aBufferSize, const char * aSymbols, unsigned int aStartPos )
{
	// Проверка аргументов.
	if( !aBuffer || !aSymbols )
		return aBufferSize;

	unsigned int res = aStartPos;
	for(; res < aBufferSize && !strchr( aSymbols, aBuffer[res] ); res++ )
	{
		if( aBuffer[res] == '\\' )
			res++;
	}

	return res;
}

inline unsigned int TextFuncs::FindNotSymbolTrue( const char * aBuffer, unsigned int aBufferSize, const char * aSymbols, unsigned int aStartPos )
{
	// Проверка аргументов.
	if( !aBuffer || !aSymbols )
		return aBufferSize;

	unsigned int res = aStartPos;
	for(; res < aBufferSize && strchr( aSymbols, aBuffer[res] ); res++ )
	{
		if( aBuffer[res] == '\\' )
			res++;
	}

	return res;
}

inline unsigned int TextFuncs::FindRestrictedSymbol( const char * aBuffer, unsigned int aBufferSize, const char * aSymbols, unsigned int aStartPos )
{
	// Проверка аргументов.
	if( !aBuffer || !aSymbols )
		return aBufferSize;

	// Поиск символов '\'.
	const char * ptr = aBuffer + aStartPos;
	while( ptr )
	{
		ptr = strchr( ptr, '\\' );

		if( !ptr )
			return aBufferSize;

		unsigned int size = ptr - aBuffer;

		if( size >= aBufferSize )
			return aBufferSize;

		if( size == aBufferSize - 2 )
			return aBufferSize;

		if( !strchr( aSymbols, aBuffer[size + 1] ) )
			return size + 1;
		else
			ptr++;

		ptr++;
	}

	return aBufferSize;
}

inline unsigned int TextFuncs::RemoveEscapeSymbols( char* aBuffer, unsigned int aBufferLen )
{
	// Проверка аргументов.
	if( !aBuffer )
		return 0;

	unsigned int size = aBufferLen;

	for( unsigned int i = 0; i < size; i++ )
	{
		if( aBuffer[i] == '\\' )
		{
			memmove( aBuffer + i, aBuffer + i + 1, aBufferLen - i - 1 );
			size--;
		}
	}

	return size;
}

inline int TextFuncs::GetUnsigned( const char * aStr, unsigned int & aPos )
{
	return GetUnsigned( aStr, strlen( aStr ), aPos );
}

inline int TextFuncs::GetUnsigned( const char * aStr, unsigned int aStrLenght, unsigned int & aPos )
{
	if( !aStr )
		return 0;

	unsigned int Pos = aPos;

	Pos = TextFuncs::SkipSpaces( aStr, Pos, aStrLenght );
	if( Pos >= aStrLenght )
		return 0;

	int sum = 0;
	for(; Pos < aStrLenght && aStr[Pos]; ++Pos )
		if( TextFuncs::IsDigit( aStr[Pos] ) )
		{
			sum = sum * 10 + aStr[Pos] - '0';
		}
		else
			break;

	aPos = Pos;

	return sum;
}

inline double TextFuncs::GetFloat( const char * aStr, unsigned int & aPos )
{
	return GetFloat( aStr, strlen( aStr ), aPos );
}

inline double TextFuncs::GetFloat( const char * aStr, unsigned int aStrLenght, unsigned int & aPos )
{
	if( !aStr )
		return 0;

	unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLenght );
	bool is_negated = aStr[pos] == '-';
	if( aStr[pos] == '-' || aStr[pos] == '+' )
		pos = TextFuncs::SkipSpaces( aStr, pos + 1, aStrLenght );

	double res = GetUnsigned( aStr, aStrLenght, pos );
	if( !pos )
		return 0;

	if( aStr[pos] == '.' || aStr[pos] == ',' )
	{
		++pos;
		double base = 1;
		for(; pos < aStrLenght && aStr[pos]; ++pos )
			if( TextFuncs::IsDigit( aStr[pos] ) )
			{
				base /= 10.0;
				res += base * ( aStr[pos] - '0' );
			}
			else
				break;
	}

	aPos = pos;

	return is_negated ? -res : res;
}

inline int TextFuncs::GetInteger( const char * aStr, unsigned int & aPos )
{
	return GetInteger( aStr, strlen( aStr ), aPos );
}

inline int TextFuncs::GetInteger( const char * aStr, unsigned int aStrLenght, unsigned int & aPos )
{
	if( !aStr )
		return 0;

	unsigned int Pos = aPos;

	Pos = TextFuncs::SkipSpaces( aStr, Pos, aStrLenght );
	if( Pos >= aStrLenght )
		return 0;

	int sum = 0;
	bool negative = false;

	if( aStr[Pos] == '-' )
	{
		negative = true;
		++Pos;
	}
	else if( aStr[Pos] == '+' )
	{
		++Pos;
	}

	unsigned int End = Pos;
	sum = GetUnsigned( aStr, aStrLenght, End );
	if( End == Pos )
		return 0;

	aPos = End;

	return negative ? -sum : sum;
}

inline int TextFuncs::GetTimeZone( const char * aStr, unsigned int & aPos )
{
	return GetTimeZone( aStr, strlen( aStr ), aPos );
}

inline int TextFuncs::GetTimeZone( const char * aStr, unsigned int aStrLenght, unsigned int & aPos )
{
	unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLenght );
	if( pos >= aStrLenght )
		return 0;
	int in_seconds = 0;

	if( strncasecmp( aStr + pos, "MSK", 3 ) )
		return 0;
	pos += 3;

	if( aStr[pos] != '-' && aStr[pos] != '+' )
		return 0;
	bool is_negated = aStr[pos] == '-';
	++pos;
	if( !TextFuncs::IsDigit( aStr[pos] ) )
		return 0;

	unsigned int end = pos;
	int hours = TextFuncs::GetInteger( aStr, aStrLenght, end );
	if( pos == end || hours < 0 || hours > 23 )
		return 0;

	in_seconds = hours * 3600;
	pos = end;
	if( aStr[pos] == ':' )
	{
		++pos;
		if( !TextFuncs::IsDigit( aStr[pos] ) )
			return 0;

		end = pos;
		int minutes = TextFuncs::GetInteger( aStr, aStrLenght, end );
		if( pos == end || minutes < 0 || minutes > 59 )
			return 0;
		pos = end;

		in_seconds += minutes * 60;
	}

	if( is_negated )
		in_seconds *= -1;

	aPos = pos;

	return in_seconds;
}

inline long int TextFuncs::ToInteger( const char * aStr, bool* aSuccess )
{
	if( !aStr )
	{
		if( aSuccess )
			*aSuccess = false;
		return 0;
	}

	while( *aStr && IsSpace( *aStr ) )
		++aStr;

	long long sum = 0;
	bool negative = false;

	if( *aStr == '-' )
	{
		negative = true;
		++aStr;
	}
	else if( *aStr == '+' )
	{
		++aStr;
	}

	while( *aStr && IsSpace( *aStr ) )
		++aStr;

	while( *aStr && IsDigit( *aStr ) )
	{
		sum = sum * 10 + *aStr - '0';
		++aStr;
	}

	while( *aStr && IsSpace( *aStr ) )
		++aStr;

	if( *aStr )
	{
		if( aSuccess )
			*aSuccess = false;
		return 0;
	}

	if( aSuccess )
		*aSuccess = true;
	return negative ? -sum : sum;
}



inline bool TextFuncs::ParseDate( const char * date, unsigned int & day, unsigned int & month, unsigned int & year, bool & aDateIsShort )
{
	unsigned int pos = 0;
	bool res = ParseDate( date, strlen( date ), pos, day, month, year, aDateIsShort );
	if( res && date[pos] != '\0' )
		return false;
	return res;
}

inline bool TextFuncs::ParseDate( const char * date, unsigned int length, unsigned int & pos, unsigned int & day, unsigned int & month, unsigned int & year, bool & aDateIsShort, bool aMakeChecks, bool aSkipSpaces )
{
	if( !date )
		return false;

	day = 0;
	month = 0;
	year = 0;

	const char * ptr = date + pos;
	const char * cur = date + pos;

	for(; *cur && *cur >= '0' && *cur <= '9' && cur - ptr <= 2; ++cur )
		day = day * 10 + *cur - '0';

	if( aSkipSpaces )
		while( IsSpace( *cur ) )
			++cur;

	if( *cur != '.' || ( aMakeChecks && ( day < 1 || day > 31 ) ) )
		return false;
	ptr = ++cur;

	if( aSkipSpaces )
		while( IsSpace( *cur ) )
			++cur;

	for(; *cur && *cur >= '0' && *cur <= '9' && cur - ptr <= 2; ++cur )
		month = month * 10 + *cur - '0';

	if( aSkipSpaces )
		while( IsSpace( *cur ) )
			++cur;

	if( ( *cur != '.' && *cur != '\0' ) || ( aMakeChecks && ( month < 1 || month > 12 ) ) )
		return false;
	if( *cur == '\0' || TextFuncs::IsSpace( *cur ) )
	{
		pos = cur - date;
		aDateIsShort = true;
		return true;
	}
	ptr = ++cur;

	if( aSkipSpaces )
		while( IsSpace( *cur ) )
			++cur;

	for(; *cur && *cur >= '0' && *cur <= '9' && cur - ptr <= 4; ++cur )
		year = year * 10 + *cur - '0';

	pos = cur - date;

	aDateIsShort = false;

	return true;
}

inline bool TextFuncs::ParseTime( const char * time, unsigned int & hour, unsigned int & minute, unsigned int & second, bool & aTimeIsShort )
{
	unsigned int pos = 0;
	bool res = ParseTime( time, strlen( time ), pos, hour, minute, second, aTimeIsShort );
	if( res && time[pos] != '\0' )
		return false;
	return res;
}

inline bool TextFuncs::ParseTime( const char * time, unsigned int length, unsigned int & pos, unsigned int & hour, unsigned int & minute, unsigned int & second, bool & aTimeIsShort, bool aMakeChecks, bool aSkipSpaces )
{
	if( !time )
		return false;

	hour = 0;
	minute = 0;
	second = 0;

	const char * ptr = time + pos;
	const char * cur = time + pos;

	for(; *cur && *cur >= '0' && *cur <= '9' && cur - ptr <= 2; ++cur )
		hour = hour * 10 + *cur - '0';

	if( aSkipSpaces )
		while( IsSpace( *cur ) )
			++cur;

	if( *cur != ':' || ( aMakeChecks && hour > 23 ) )
		return false;
	ptr = ++cur;

	if( aSkipSpaces )
		while( IsSpace( *cur ) )
			++cur;

	for(; *cur && *cur >= '0' && *cur <= '9' && cur - ptr <= 2; ++cur )
		minute = minute * 10 + *cur - '0';

	if( aSkipSpaces )
		while( IsSpace( *cur ) )
			++cur;

	if( ( *cur != ':' && *cur != '\0' ) || ( aMakeChecks && minute > 59 ) )
		return false;
	if( *cur == '\0' || TextFuncs::IsSpace( *cur ) )
	{
		pos = cur - time;
		aTimeIsShort = true;
		return true;
	}
	ptr = ++cur;

	if( aSkipSpaces )
		while( IsSpace( *cur ) )
			++cur;

	for(; *cur && *cur >= '0' && *cur <= '9' && cur - ptr <= 2; ++cur )
		second = second * 10 + *cur - '0';
	if( aMakeChecks && second > 59 )
		return false;

	pos = cur - time;

	aTimeIsShort = false;

	return true;
}

inline unsigned int TextFuncs::ParseWord( const char * aStr, unsigned int aStrLength,
		const char ** aWords, const unsigned int * aWordLengths,
		unsigned int aWordsCount, bool * aResults )
{
	// Результат функции.
	unsigned int ResNum = -1;

	// Количество слов, являющихся префиксом заданной строки в текущий момент.
	unsigned int Matched = aWordsCount;
	memset( aResults, true, aWordsCount );

	// Цикл по символам строки пока есть хотя бы одно совпадение.
	for( unsigned int char_n = 0; char_n < aStrLength && Matched; ++char_n )
	{
		// Цикл по словам пока есть хотя бы одно совпадение..
		for( unsigned int word_n = 0; word_n < aWordsCount && Matched; ++word_n )
		{
			// Если слово ещё не проверено до конца и подходит.
			if( char_n < aWordLengths[word_n] && aResults[word_n] )
			{
				// Если очередной символ не совпал.
				if( aStr[char_n] != aWords[word_n][char_n] )
				{
					// Помечаем слово, как не являющееся префиксом заданной строки.
					aResults[word_n] = false;
					--Matched;
				}
				// Если совпал последний символ слова.
				else if( char_n + 1 == aWordLengths[word_n] )
				{
					// Результат функции - символ, следующий за совпавшим префиксом.
					ResNum = word_n;
					--Matched;
				}
			}
		}
	}

	return ResNum;
}

#endif	/** __TextFuncs_hpp__ */
