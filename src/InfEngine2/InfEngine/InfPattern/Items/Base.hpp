#ifndef __InfPatternItemBase_hpp__
#define __InfPatternItemBase_hpp__

#include "../ItemTypes.h"

#include <InfEngine2/_Include/BinaryData.hpp>

namespace InfPatternItems
{
	/**
	 *  Базовый класс элемента шабон-строки.
	 */
	class Base
	{
	public:
		Base() {}
		Base( ItemType aType ): vType( aType ) {}
		Base( ItemType aType, size_t aPos ): vType( aType ), vPos( aPos ) {}

	public:
		/** Получение типа элемента. */
		ItemType GetType( ) const
			{ return vType; }

	public:
		/** Получение размера памяти, необходимого для сохранения элемента. */
		virtual unsigned int GetNeedMemorySize( ) const = 0;

		/**
		 *  Сохранение элемента в буфер.
		 * @param aBuffer - буфер для сохранения элемента.
		 * @return - размер сохраненного элемента.
		 */
		virtual unsigned int Save( void * aBuffer ) const = 0;

		/**
		 *  Сохранение в буфер. После сохранения размер буфера уменьшается, а указатель передвигается на свобоное место.
		 * @param aBuffer - буфер для сохранения.
		 * @param aBufferSize - размер буфера для сохранения.
		 */
		virtual unsigned int Save( char *& aBuffer, unsigned int & aBufferSize ) const
		{
			unsigned int size = Save( aBuffer );
			aBuffer += size;
			aBufferSize -= size;
			return size;
		}

	public:
		unsigned int GetPos() const { return vPos; }

	protected:
		// Типа элемента.
		ItemType vType = itText;

		// Позиция начала элемента в DL строке.
		unsigned int vPos;
	};
};

#endif /** __InfPatternItemBase_hpp__ */
