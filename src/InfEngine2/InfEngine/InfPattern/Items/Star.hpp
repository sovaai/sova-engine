#ifndef __InfPatternItemsStar_hpp__
#define __InfPatternItemsStar_hpp__

#include <strings.h>

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление элемента - звездочка.
	 */
	class Star : public Base
	{
	public:
		/** Конструктор. */
		Star( )
		{
			vType = itStar;
			bzero( vBuffer, 2 );
		}

		/**
		 *  Буфер с сохраненной звездочкой.
		 * @param aBuffer
		 */
		Star( const void * aBuffer )
		{
			vType = itStar;
			Set( aBuffer );
		}

	public:
		/**
		 *  Установка данных.
		 * @param aStarNum - порядковый номер звездочки.
		 * @param aSuperStar - флаг суперзвездочки.
		 */
		void Set( char aStarNum, bool aSuperStar )
		{
			vBuffer[0] = aStarNum;
			vBuffer[1] = aSuperStar ? 1 : 0;
		}

		/**
		 *  Загрузка сохраненной звездочки.
		 * @param aBuffer - буфер с сохраненной звездочкой.
		 */
		void Set( const void* aBuffer )
			{ memcpy( vBuffer, aBuffer, 2 ); }

	public:
		unsigned int GetNeedMemorySize( ) const
			{ return 2; }

		unsigned int Save( void * aBuffer ) const
		{
			memcpy( aBuffer, vBuffer, 2 );
			return 2;
		}

	public:
		/** Проверка на суперзвездочку. **/
		bool IsSuperStar( ) const
			{ return vBuffer[1]; }

		/** Получение номера звездочки. */
		unsigned int GetNum() const
			{ return vBuffer[0]; }

	protected:
		/** Данные тэга. */
		char vBuffer[2];
	};
};

#endif /** __InfPatternItemsStar_hpp__ */
