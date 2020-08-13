#ifndef __InfPatternItemsTagOpenWindow_hpp__
#define __InfPatternItemsTagOpenWindow_hpp__

#include "Base.hpp"
#include "Array.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление тэга открытия ссылки в новом окне.
	 */
	class TagOpenWindow: public Base
	{
	public:
		TagOpenWindow()
			{ vType = itOpenWindow; }

	public:
		/** Установка данных. */
		void Set( Base ** aURL, unsigned int aURLLen, int aType )
		{
			vURL = aURL;
			vURLLen = aURLLen;
			vOpenType = (wwwww)aType;
		}

	public:
		unsigned int GetNeedMemorySize() const
			{ return Array( vURL, vURLLen ).GetNeedMemorySize() + 1; }

		unsigned int Save( void * aBuffer ) const
		{
			// Сохранение данных.
			char* ptr = static_cast<char*>( aBuffer );

			// Сохранение типа открытия.
			*ptr = vOpenType;
			ptr++;

			// Сохранение URL.
			ptr += Array( vURL, vURLLen ).Save( ptr );

			return ptr - static_cast<char*>( aBuffer );
		}


		typedef enum
		{
			OPEN_IN_NEW_WINDOW      = 0,
			OPEN_IN_PARENT_WINDOW   = 1
		} wwwww;

	private:
		/** Элементы, из которых составляется адрес. */
		Base ** vURL = nullptr;

		/** Количество элементов в URL. */
		unsigned int vURLLen = 0;

		/** Тип открытия. */
		wwwww vOpenType = OPEN_IN_NEW_WINDOW;
	};

	/**
	 *  Манипулятор элемента ссылки.
	 */
	class TagOpenWindowManipulator
	{
	public:
		TagOpenWindowManipulator( const void * aBuffer = nullptr )
			{
				vBuffer = static_cast<const char *>( aBuffer );
				vURL.Set( vBuffer + 1 );
		}

	public:
		/** Получение ссылки на URL. */
		ArrayManipulator GetURL() const
			{ return vURL; }

		/** Получение типа открытия окна. */
		char GetOpenType() const
			{ return *vBuffer; }

	private:
		/** Данные элемента. */
		const char * vBuffer = nullptr;

		/** Элементы, составляющие URL. */
		ArrayManipulator vURL;
	};
}

#endif  /** __InfPatternItemsTagOpenWindow_hpp__ */
