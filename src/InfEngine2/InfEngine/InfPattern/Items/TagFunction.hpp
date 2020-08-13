#ifndef __InfPatternItemsTagFunction_hpp__
#define __InfPatternItemsTagFunction_hpp__

#include "Base.hpp"
#include "lib/aptl/avector.h"
#include "Array.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление элемента "функция"
	 */

	enum TagFunctionOptions
	{
		TFO_INDEXED = 1,
		TFO_MULTIPLE = 2,
	};

	class TagFunction: public Base
	{
	public:
		class Argument;

	public:
		/** Конструктор классаю */
		TagFunction()
			{ vType = itFunction; }

		/**
		 *  Установить идентификатор функции.
		 * @param aFuncId - идентификатор, присвоенный функции при компиляции списка функций.
		 */
		void Set( unsigned int aFuncId )
			{ vFuncId = aFuncId; }

		/**
		 *  Задать аргументы функции.
		 * @param aArgs - аргументы функции.
		 * @param aArgsCount - количество аргументов.
		 */
		void SetArgs( Argument ** aArgs, unsigned int aArgsCount )
		{
			vArgs = aArgs;
			vArgsCount = aArgsCount;
		}

	public:
		unsigned int GetNeedMemorySize() const;

		unsigned int Save( void * aBuffer ) const;

	public:
		/** Возвращает идентификатор функции. */
		unsigned int GetId() const
			{ return vFuncId; }

		/**
		 *  Устанвить идентификатор тэга-функции.
		 * @param aTagFuncId - идентификатор данного тэга-функции.
		 */
		void SetTagId( unsigned int aTagFuncId )
			{ vTagFuncId = aTagFuncId; }

		/**
		 *  Установить возможность кэшировать тэг-функцию.
		 * @param aIsMultiple - возможность кэширования тэга-функции.
		 */
		void SetMultiple( bool aIsMultiple )
			{ vIsMultiple = aIsMultiple; }

		/** Возвращает True, если тэг-функция может принимать различные значения при одинаковых условиях раскрытия. */
		bool IsMultiple() const
			{ return vIsMultiple; }

		/** Возвращает количество аргументов у тэга-функции. */
		unsigned int GetArgsCount() const
			{ return vArgsCount; }

		/**
		 *  Возвращает заданный аргумент тэга-функции.
		 * @param aNumber - номер аргумента функции.
		 */
		const Argument * GetArg( unsigned int aNumber ) const
			{ return aNumber < vArgsCount ? vArgs[aNumber] : nullptr; }

	private:
		/** Идентификатор, прсвоенный функции при компиляции списка функций. */
		unsigned int vFuncId = -1;

		/** Идентификатор, присвоенный данному тэгу в индексе тэгов-функций. */
		unsigned int vTagFuncId = -1;

		/** Аргументы функции. */
		Argument ** vArgs = nullptr;

		/** Количество аргументов. */
		unsigned int vArgsCount = 0;

		/** Возможность кэширования тэга-функции. */
		bool vIsMultiple = false;
	};

	class TagFunction::Argument: public Base
	{
	public:
		/** Конструктор класса */
		Argument()
			{ vType = itFunctionArg; }

		/**
		 *  Задать составляющие аргумент элементы.
		 * @param aItems - составные элементы аргумента.
		 * @param aItemsCount - количество элементов.
		 */
		void SetItems( Base ** aItems, unsigned int aItemsCount )
		{
			vItems = aItems;
			vItemsCount = aItemsCount;
		}

	public:
		unsigned int GetNeedMemorySize() const
			{ return Array( vItems, vItemsCount ).GetNeedMemorySize(); }

		unsigned int Save( void * aBuffer ) const
			{ return Array( vItems, vItemsCount ).Save( aBuffer ); }

	public:
		/** Возвращает количество элементов, из которых состоит аргумент. */
		unsigned int GetItemsCount() const
			{ return vItemsCount; }

		/**
		 *  Возвращает указатель на элемент.
		 * @param item_n - номер элемента.
		 */
        InfPatternItems::Base * GetItem( unsigned int item_n ) const
			{ return item_n < vItemsCount ? vItems[item_n] : nullptr; }

		Array GetItems() const { return { vItems, vItemsCount }; }

	public:
		Base ** begin() const { return vItems; }
		Base ** end() const { return vItems ? vItems + vItemsCount : nullptr; }

	private:
		/** Элементы аргумента. */
		Base ** vItems = nullptr;

		/** Количество элементов аргумента. */
		unsigned int vItemsCount = 0;
	};

	/**
	 * Манипулятор элемента "функция"
	 */
	class TagFunctionManipulator
	{
	public:
		TagFunctionManipulator( const char * aBuffer = nullptr )
			{ SetBuffer( aBuffer ); }

		/**
		 * Инициализировать манипулятор.
		 * @param aBuffer - буфер.
		 */
		void SetBuffer( const char * aBuffer )
		{
			vBuffer = aBuffer;
			vArguments.Set( aBuffer ? aBuffer + 1 + sizeof(uint32_t) : nullptr );
		}

		/** Возвращает True, если тэг-функция сохранён в индексе. */
		bool SavedInIndex() const
			{ return vBuffer ? *vBuffer & TFO_INDEXED : false; }

		/** Возвращает признак того, что тэг-функция может раскрыватсья в разные значения при одинаковых условиях. */
		bool IsMultiple() const
			{ return vBuffer ? *vBuffer & TFO_MULTIPLE : false; }

		/** Возвращает идентификатор функции. */
		unsigned int GetId() const
			{ return vBuffer ? *reinterpret_cast<const uint32_t*>(vBuffer + 1) : 0; }

		/** Возвращает идентификатор тэга-функции. */
		unsigned int GetTagId() const
			{ return vBuffer ? *reinterpret_cast<const uint32_t*>(vBuffer + 1) : 0; }

		/** Возвращает количество разобранных аргументов. */
		unsigned int GetArgumentsCount() const
			{ return vBuffer ? vArguments.GetItemsNum() : 0; }

		/**
		 *  Возвращает указатель на аргумент.
		 * @param arg_n - номер аргумента функции.
		 */
		const void * GetArgument( unsigned int arg_n ) const
			{ return vBuffer && arg_n < GetArgumentsCount() ? vArguments.GetItem(arg_n) : nullptr; }

	private:
		const char * vBuffer = nullptr;

		ArrayManipulator vArguments;
	};


	/**
	 * Манипулятор элемента "Аргумент функции"
	 */
	typedef ArrayManipulator TagFunctionArgumentManipulator;
}

#endif
