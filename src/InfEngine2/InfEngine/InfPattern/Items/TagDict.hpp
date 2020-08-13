#ifndef __InfPatternItemsTagDict_hpp__
#define __InfPatternItemsTagDict_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	// Тип словаря.
	enum DictType { DT_DICT = 0, DT_UDICT = 1, DT_SUDICT = 2, DT_LCDICT = 3 };

	/**
	 *  Представление элемента словаря.
	 */
	class TagDict: public Base
	{
	public:
		TagDict( enum DictType aDictType, bool aHasEmptyVariant )
			{ Init( aDictType, aHasEmptyVariant ); }

	public:
		/** Инициализация. */
		void Init( enum DictType aDictType, bool aHasEmptyVariant )
			{ vType = itDict; vDictType = aDictType; vHasEmptyVariant = aHasEmptyVariant; }

	public:
		/** Установка данных. */
		void Set( unsigned int aDictId )
			{ vDictId = aDictId; }

		/** Возвращает значени флага наличия у словаря пустого варианта сопоставления. */
		bool HasEmptyVariant() const
			{ return vHasEmptyVariant; }

	public:
		/** Получение размера памяти, необходимого для сохранения данного элемента. */
		unsigned int GetNeedMemorySize() const
			{ return 2*sizeof(uint32_t) + 1; }

		/** Сохранение элемента в буффер. */
		unsigned int Save( void * aBuffer ) const
		{
			// Сохранение данных.
			char * ptr = static_cast<char*>( aBuffer );

			// Сохранение значения.
			*((uint32_t*)ptr) = vDictId;
			ptr += sizeof( uint32_t );
			*((uint32_t*)ptr) = vDictType;
			ptr += sizeof( uint32_t );
			*((uint8_t*)ptr) = vHasEmptyVariant;
			ptr += sizeof( uint8_t );

			return ptr - static_cast<char*>( aBuffer );
		}

	public:
		/** Идентификатор словаря. */
		unsigned int vDictId = 0;

		/** Тип словаря. */
		enum DictType vDictType = DT_DICT;

		/** Флаг наличия у словаря пустого варианта сопоставления. */
		bool vHasEmptyVariant = false;
	};

	/**
	 *  Манипулятор элемента словаря.
	 */
	class TagDictManipulator
	{
	public:
		TagDictManipulator( const void * aBuffer = nullptr ):
			vBuffer( static_cast<const char *>( aBuffer ) ) {}

	public:
		/** Инициализация. */
		void Init( const void * aBuffer )
			{ vBuffer = static_cast<const char *>( aBuffer ); }

	public:
		/** Получение индекса словаря. */
		unsigned int GetDictID() const
			{ return *((uint32_t*)vBuffer); }

		/** Получение типа словаря. */
		DictType GetDictType() const
			{ return (DictType)((uint32_t*)vBuffer)[1]; }

		/** Получение флага наличия у словаря пустого варианта сопоставления. */
		bool HasEmptyVariant() const
			{ return *(vBuffer+2*sizeof(uint32_t)); }

	private:
		/** Данные элемента */
		const char * vBuffer = nullptr;
	};
}

#endif /** __InfPatternItemsTagDict_hpp__ */
