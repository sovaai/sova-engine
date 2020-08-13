#ifndef __InfPatternItemsArray_hpp__
#define __InfPatternItemsArray_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Массив DL элементов.
	 */
	class Array
	{
	public:
		/** Конструктор. */
		Array() {}

		/**
		 *  Конструктор.
		 * @param aItems - массив DL элементов.
		 * @param aItemsNum - число DL элементов в массиве.
		 */
		Array( Base ** aItems, unsigned int aItemsNum ) { Set( aItems, aItemsNum ); }

		/**
		 *  Конструктор.
		 * @param aArray - массив DL элементов.
		 */
		Array( const Array & aArray ) { Set( aArray.vItems, aArray.vItemsNum ); }

	public:
		/**
		 *  Установка элементов массива.
		 * @param aItems - массив DL элементов.
		 * @param aItemsNum - число DL элементов в массиве.
		 */
		virtual void Set( Base ** aItems, unsigned int aItemsNum );

		/**
		 *  Установка элементов массива.
		 * @param aArray - массив DL элементов.
		 */
		void Set( const Array & aArray ) { Set( aArray.vItems, aArray.vItemsNum ); }

	public:
		/** Получение количества DL элементов в массиве. */
		unsigned int GetItemsNumber() const { return vItemsNum; }

		/** Получение указателя на список DL элементов массива. */
		InfPatternItems::Base ** GetItems() const { return vItems; }

		/**
		 *  Получение aNum-ого DL элемента массива.
		 * @param aNum - номер DL элемента.
		 */
		InfPatternItems::Base * GetItem( unsigned int aNum ) const { return vItems[aNum]; }

		/**
		 *  Получение aNum-ого DL элемента массива.
		 * @param aNum - номер DL элемента.
		 */
		InfPatternItems::Base* operator[]( unsigned int aNum ) { return vItems[aNum]; }

		/** Проверка на непустоту. */
		explicit operator bool() const { return vItemsNum; }

	public:
		/** Получение итератора начала массива. */
		Base ** begin() const { return vItems; }
		/** Получение итератора конца массива. */
		Base ** end() const { return vItems ? vItems + vItemsNum : nullptr; }

	public:
		/** Вычисление объема памяти, необходимого для сохранения. */
		virtual unsigned int GetNeedMemorySize() const;

		/**
		 *  Сохранение массива в буфер.
		 * @param aBuffer - буфер для сохранения массива.
		 * @return - размер сохраненного массива.
		 */
		virtual unsigned int Save( void * aBuffer ) const;

	protected:
		/** Массив элементов строки шаблона. */
		Base ** vItems { nullptr };

		/** Количество элементов в массиве элементов строки шаблона. */
		unsigned int vItemsNum { 0 };

		/** Объем памяти, необходимый для сохранения массива. */
		mutable unsigned int vNeedMemorySize { 0 };
	};

	/**
	 *  Манипулятор массива DL элементов.
	 */
	class ArrayManipulator
	{
	public:
		/** Конструктор. */
		ArrayManipulator() {}

		/**
		 *  Конструктор.
		 * @param aBuffer - буфер с сохранненым массивом DL элементов.
		 */
		ArrayManipulator( const void * aBuffer ) { vBuffer = static_cast<const char*>( aBuffer ); }

		/**
		 *  Конструктор.
		 * @param aArrayManipulator - манипулятор сохраненным массивом DL элементов.
		 */
		ArrayManipulator( const ArrayManipulator & aArrayManipulator ) { Set( aArrayManipulator ); }

	public:
		/**
		 *  Установка данных.
		 * @param aBuffer - буфер с сохраненным массивом DL элементов.
		 */
		virtual void Set( const void * aBuffer ) { vBuffer = static_cast<const char*>( aBuffer ); }

		void Set( const ArrayManipulator & aArrayManipulator ) { vBuffer = aArrayManipulator.vBuffer; }

		/** Проверка, инициализирован ли манипулятор. */
		bool IsValid() const { return vBuffer != nullptr; }

	public:
		/** Получение количества элементов в массиве. */
		unsigned int GetItemsNum() const { return vBuffer ? *( static_cast<const uint32_t*>( vBuffer ) ) : 0; }

		/**
		 *  Получение типа i-того элемента массива.
		 * @param aNum - номер элеента массива.
		 */
		ItemType GetItemType( unsigned int aNum ) const
		{
			return vBuffer ? ( ItemType ) * ( static_cast<const char*>( vBuffer ) +
											  ( static_cast<const uint32_t*>( vBuffer ) )[aNum + 1] ) : itSpace;
		}

		/**
		 *  Получение i-того элемента массива.
		 * @param aNum - номер элемента массива.
		 */
		const void * GetItem( unsigned int aNum ) const
		{
			return vBuffer ? static_cast<const char*>( vBuffer ) + ( static_cast<const uint32_t*>( vBuffer ) )[aNum + 1] + 1 : nullptr;
		}

	protected:
		/** Данные массива. */
		const void * vBuffer { nullptr };
	};
}

#endif /** __InfPatternItemsArray_hpp__ */
