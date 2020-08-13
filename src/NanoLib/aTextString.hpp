#ifndef ATEXTSTRING_HPP
#define ATEXTSTRING_HPP

#include <sstream>
#include <cstring>
#include <cstdarg>

#include "ReturnCode.hpp"

#include <NanoLib/LogSystem.hpp>

/**
 *  Динамическая строка.
 */
class aTextString
{
public:
	/** Конструктор. */
	aTextString() noexcept {}

public:
	/** Запрет конструктора копирования. */
	aTextString( const aTextString & /*aString*/ ) = delete;

	/** Конструктор переноса. */
	aTextString( aTextString && aString ) noexcept { std::swap( vString, aString.vString ); }

public:
	/** Запрет конструктора копирования. */
	aTextString( const std::string & /*aString*/ ) = delete;

	/** Конструктор переноса. */
	aTextString( std::string && aString ) noexcept { std::swap( vString, aString ); }

public:
	/** Запрет небезопасного копирования. */
	aTextString & operator =( const aTextString & /*aString*/ ) = delete;

	/** Перенос строки. */
	aTextString & operator =( aTextString && aString ) noexcept { std::swap( vString, aString.vString ); return *this; }

public:
	/** Запрет небезопасного копирования. */
	aTextString & operator =( const std::string & /*aString*/ ) = delete;

	/** Перенос строки. */
	aTextString & operator =( std::string && aString ) noexcept { std::swap( vString, aString ); return *this; }

public:
	/** Доступ к элементу строки. */
	template<typename S>
	char & operator[]( S i ) { return vString[i]; }

	/** Константный доступ к элементу строки. */
	template<typename S>
	const char & operator[]( S i ) const { return vString[i]; }

public:
	/** Проверка на нулевую длину строки. */
	bool empty() const noexcept { return vString.empty(); }

public:
	/** Копирование буфера. */
	nlReturnCode assign( const char * aBuffer, size_t aSize ) noexcept
	{
		try
		{
			vString.assign( aBuffer, aSize );
		}
		catch( ... )
		{
			return nlrcErrorInvArgs;
		}
		return nlrcSuccess;
	}

	/** Копирование C-строки. */
	nlReturnCode assign( const char * aString ) noexcept { return assign( aString, aString ? std::strlen( aString ) : 0 ); }

	/** Копирование строки. */
	nlReturnCode assign( const aTextString & aString ) noexcept { return assign( aString.c_str(), aString.size() ); }

	/** Перенос строки. */
	nlReturnCode assign( aTextString && aString ) noexcept { std::swap( vString, aString.vString ); return nlrcSuccess; }

	/** Копирование std строки. */
	nlReturnCode assign( const std::string & aString ) noexcept { return assign( aString.c_str(), aString.size() ); }

	/** Перенос std строки. */
	nlReturnCode assign( std::string && aString ) noexcept { std::swap( vString, aString ); return nlrcSuccess; }

	/** Копирование данных в строку. */
	template<typename T>
	nlReturnCode assign( const T & obj ) noexcept
	{
		static std::ostringstream stream;
		try
		{
			stream.clear();
			stream.str("");
			stream << obj;
		}
		catch( ... )
		{
			return nlrcErrorNoFreeMemory;
		}
		return assign( stream.str() );
	}

public:
	/** Конкатенация буфера. */
	nlReturnCode append( const char * aBuffer, size_t aSize ) noexcept
	{
		try
		{
			vString.append( aBuffer, aSize );
		}
		catch( ... )
		{
			return nlrcErrorInvArgs;
		}
		return nlrcSuccess;
	}

	/** Конкатенация C-строки. */
	nlReturnCode append( const char * aString ) noexcept { return append( aString, std::strlen( aString ) ); }

	/** Конкатенация строки. */
	nlReturnCode append( const aTextString & aString ) noexcept { return append( aString.c_str(), aString.size() ); }
	nlReturnCode append( const std::string & aString ) noexcept { return append( aString.c_str(), aString.size() ); }

	/** Конкатенация данных. */
	template<typename T>
	nlReturnCode append( T obj ) noexcept
	{
		std::ostringstream stream;
		try
		{
			stream.clear();
			stream << obj;
		}
		catch( ... )
		{
			return nlrcErrorNoFreeMemory;
		}
		return append( stream.str() );
	}

public:
	/**
	 *  Печать данных строки в stdout.
	 * @param aFormat - формат записи.
	 */
	nlReturnCode Print( const char * aFormat, ...  ) noexcept
	{
		static const unsigned int BUF_SIZE = 1024;
		static char buf[BUF_SIZE];

		va_list ap;
		va_start( ap, aFormat );
		int len = vsnprintf( buf, BUF_SIZE, aFormat, ap );
		if( len < 0 )
			return nlrcUnsuccess;
		va_end( ap );

		return append( buf, len );
	}

public:
	/** Получение длины строки. */
	size_t size() const noexcept { return vString.size(); }

	/** Преобразование к типу size_t возвращает длину строки. */
	explicit operator size_t() const noexcept { return size(); }

	/** Преобразование к типу char *. */
	char * str() const noexcept { return const_cast<char*>( vString.c_str() ); }
	char * ToChar() const noexcept { return str(); }
	explicit operator char *() const noexcept { return str(); }

	/** Преобразование к типу const char *. */
	const char * c_str() const noexcept { return vString.c_str(); }
	const char * ToConstChar() const noexcept { return c_str(); }
	explicit operator const char *() const noexcept { return c_str(); }

	/** Преобразование к типу std::string. */
	std::string & std_str() noexcept { return vString; }
	explicit operator std::string() { return vString; }

	/** Перобразование к типу const std::string. */
	const std::string & c_std_str() const noexcept { return vString; }
	explicit operator const std::string() const { return vString; }

public:
	/** Изменение размера строки. */
	nlReturnCode resize( size_t asize ) noexcept
	{
		try
		{
			vString.resize( asize );
		}
		catch( ... )
		{
			return nlrcErrorNoFreeMemory;
		}
		return nlrcSuccess;
	}

public:
	/** Очистка строки. */
	void clear() noexcept { vString.clear(); }

public:
	/** Чтение строки из потока данных. */
	nlReturnCode ReadString( std::istream & stream ) noexcept
	{
		try
		{
			vString.clear();
			std::getline( stream, vString );
		}
		catch( ... )
		{
			return nlrcErrorNoFreeMemory;
		}

		if( !empty() )
			return nlrcSuccess;
		else if( stream.eof() )
			return nlrcEOF;
		else if( stream.fail() )
			return nlrcErrorRead;
		else
			return nlrcSuccess;
	}

	/** Чтение строки из файла. */
	nlReturnCode ReadString( FILE * file_handle ) noexcept
	{
		// Проверка аргументов.
		if( !file_handle )
			return nlrcErrorInvArgs;

		// Очистка старых данных.
		clear();

		// Проверка конца файла.
		if( feof( file_handle ) )
			return nlrcEOF;

		// Чтение из файла.
		while( 1 )
		{
			// Чтение очередного символа из файла.
			unsigned char sym = fgetc( file_handle );
			if( ferror( file_handle ) )
				return nlrcErrorRead;
			else if( feof( file_handle ) )
				return empty() ? nlrcEOF : nlrcSuccess;

			if( sym == '\n' )
				return nlrcSuccess;
			else if( sym == '\r' )
			{
				unsigned char sym = fgetc( file_handle );
				if( ferror( file_handle ) )
					return nlrcErrorRead;
				else if( feof( file_handle ) )
					return empty() ? nlrcEOF : nlrcSuccess;

				if( sym != '\n' )
					ungetc( sym, file_handle );

				return nlrcSuccess;
			}

			try
			{
				vString.push_back( sym );
			}
			catch( ... )
			{
				return nlrcErrorNoFreeMemory;
			}
		}
	}

public:
	/**
	 *  Поиск фрагмента текста в строке. Результат поиска - это позиция найденного фрагмета.
	 * @param aString - текст, который нужно найти в строке.
	 * @param pos - позиция начала поиска.
	 */
	size_t find( const char * aString, size_t astart = 0 ) const noexcept
	{
		return vString.find( aString, astart );
	}
	size_t find( const aTextString & aString, size_t astart = 0 ) const noexcept
	{
		return vString.find( aString.c_std_str(), astart );
	}
	size_t find( const std::string & aString, size_t astart = 0 ) const noexcept
	{
		return vString.find( aString, astart );
	}

private:
	/** Данные строки. */
	std::string vString;
};

inline std::ostream & operator << ( std::ostream & aStream, const aTextString & aString )
{
	return aStream << aString.c_std_str();
}

#endif	/** ATEXTSTRING_HPP */
