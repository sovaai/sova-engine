#ifndef __PatternsStorage_hpp__
#define __PatternsStorage_hpp__

#include <map>
#include <vector>
#include <NanoLib/nMemoryAllocator.hpp>
#include <InfEngine2/_Include/FStorage.hpp>
#include "InfPattern/Items/Array.hpp"

/**
 *  Хранилище шаблон строк.
 */
class PatternsStorage
{
public:
	/**
	 *  Манипулятор внутренним представлением шаблон-строки в хранилище.
	 */
	struct Pattern : public InfPatternItems::ArrayManipulator
	{
	public:
		/**
		 *  Конструктор.
		 * @param aBuffer - буфер с сохраненной шаблон-строкой.
		 */
		Pattern( const void * aBuffer = nullptr ) { Set( aBuffer ); }

	private:
		/**
		 *  Установка данных.
		 * @param aBuffer - буфер с сохраненной шаблон-строкой.
		 */
		void Set( const void * aBuffer )
		{
			if( aBuffer )
			{
				vNeedMemorySize = *static_cast<const uint32_t*>( aBuffer );
				ArrayManipulator::Set( static_cast<const char*>( aBuffer ) + sizeof( uint32_t ) );
			}
			else
			{
				vNeedMemorySize = sizeof( uint32_t );
				ArrayManipulator::Set( nullptr );
			}
		}

	public:
		/** Получение размера памяти, необходимого для сохранения шаблон-строки. */
		unsigned int GetNeedMemorySize() const { return vNeedMemorySize; }

		/**
		 *  Сохранение шаблон-строки в буфер.
		 * @param aBuffer - буфер.
		 */
		unsigned int Save( void * aBuffer ) const
		{
			*static_cast<uint32_t*>( aBuffer ) = vNeedMemorySize;
			memcpy( static_cast<char*>( aBuffer ) + sizeof( uint32_t ), vBuffer, vNeedMemorySize - sizeof( uint32_t ) );
			return vNeedMemorySize;
		}

	public:
		/**
		 *  Сравнение двух шаблон-строк.
		 * @param aPattern - шаблон-строка.
		 */
		int operator ==( const Pattern & aPattern ) const
		{
			if( vNeedMemorySize < aPattern.vNeedMemorySize )
				return -1;
			else if( vNeedMemorySize > aPattern.vNeedMemorySize )
				return 1;
			else
				return memcmp( vBuffer, aPattern.vBuffer, vNeedMemorySize - sizeof( uint32_t ) );
		}

		/**
		 *  Сравнение шаблон-строк.
		 * @param aPattern - шаблон-строка.
		 */
		bool operator<( const Pattern & aPattern ) const { return ( *this == aPattern ) < 0; }

		/**
		 * Сравнение шаблон-строк.
		 * @param aPattern - шаблон-строка.
		 */
		bool operator>( const Pattern & aPattern ) const { return ( *this == aPattern ) > 0; }

	public:
		/** Объем памяти, необходимый для сохранения шаблон-строки. */
		unsigned int vNeedMemorySize = sizeof( uint32_t );
	};

public:
	/** Конструктор. */
	PatternsStorage() { Reset(); }

public:
	/**
	 *  Сохранение хранилища шаблонов в fstorage.
	 * @param aFStorage - открытый fstorage.
	 */
	InfEngineErrors Save( fstorage & aFStorage ) const;

	/**
	 *  Загрузка хранилища из fstorage в режиме только для чтения.
	 * @param aFStorage - открытый fstorage с сохренным хранилищем шаблонов.
	 */
	InfEngineErrors Open( fstorage & aFStorage );

	/** Сброс хранилища шаблонов и инициализация его в редиме чтения и записи. */
	void Reset()
	{
		vPatterns.clear();
		vIndex.clear();
		vMemoryAllocator.Reset();
		vBuffer = nullptr;
	}

public:
	/**
	 *  Добавляет шаблон-строки в хранилище и возвращает его идентификатор.
	 * @param aPatternString - шаблон-строка.
	 * @param aId - присвоенный шаблон-строке идентификатор.
	 */
	InfEngineErrors AddPatternString( const InfPatternItems::Array & aPatternString, unsigned int & aId );

	/**
	 *  Получение шаблон-строки по идентификатору.
	 * @param aId - идентификатор, выданный шаблон-строке при добавлении в хранилище.
	 */
	const Pattern GetPatternString( unsigned int aId ) const
	{
		return vBuffer ? Pattern( reinterpret_cast<const char*>( vBuffer ) + GetShift( aId ) ) : *( vPatterns[aId] );
	}

	/**
	 *  Получение шаблон-строки по идентификатору.
	 * @param aId - идентификатор, выданный шаблон-строке при добавлении в хранилище.
	 */
	const Pattern operator[]( unsigned int aId ) const { return GetPatternString( aId ); }

protected:
	/** Получение количества шаблон-строк в хранилище. */
	unsigned int GetPattrensNum() const { return vBuffer ? reinterpret_cast<const uint32_t*>( vBuffer )[1] : vPatterns.size(); }

	/**
	 *  Ищет шаблон-строку в хранилище и возвращает ее идентификатор. В случае неудачи возвращает код INF_ENGINE_WARN_UNSUCCESS.
	 * @param aPattern - шаблон.
	 * @param aId - присвоенный шаблону идентификатор.
	 */
	InfEngineErrors SearchPatternString( const Pattern & aPattern, unsigned int & aId ) const;

	/**
	 *  Получение смещения к шаблону по его идентификатору.
	 * @param aId - идентификатор, выданный шаблону при добавлении в хранилище.
	 */
	unsigned int GetShift( unsigned int aId ) const { return reinterpret_cast<const uint32_t*>( vBuffer )[2 + aId]; }

protected:
	struct CmpWrap
	{
		bool operator()( const Pattern * aFirst, const Pattern * aSecond ) const { return *aFirst < *aSecond; }
	};

	/** Список шаблонов в хранилище. */
	std::vector<Pattern*> vPatterns;

	typedef std::map<const Pattern*, unsigned int, CmpWrap> PatternsIndex;
	/** Индекс шаблонов, построенный при помощи класса сравнения CmpWrap. */
	PatternsIndex vIndex;

	/** Менеджер памяти. */
	nMemoryAllocator vMemoryAllocator;

	/** Буфер для временных данных. */
	std::vector<char> vTmpString;

	/** Указатель на сохранённое в памяти хранилище. */
	const void * vBuffer { nullptr };
};

#endif /** __PatternsStorage_hpp__ */
