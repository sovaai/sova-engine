#ifndef __InfPatternItemsEliStar_hpp__
#define __InfPatternItemsEliStar_hpp__

#include "Star.hpp"

namespace InfPatternItems
{
	/**
	 *  Ссылка на фрагмент запроса, соответствующий выделенной звездочке поискового шаблона с учетом
	 * специфики шаблонов эллипсисов.
	 */
	class EliStar : public Star
	{
	public:
		/** Конструктор. */
		EliStar()
			{ vType = itEliStar; }

	public:
		/** Привязка звездочки к одному из поисковых шаблонов из шаблона эллипсисов. */
		enum Binding
		{
			bUserRequest    = 0,    // Привязка к текущему запросу пользователя.
			bInfPreResponse = 1,    // Привязка к последнему ответу инфа.
			bUserPreRequest = 2     // Привязка к предыдущему запросу пользователя.
		};

	public:
		/**
		 *  Установка данных.
		 * @param aStarBinding - привязка звездочки.
		 * @param aStarNum - номер звездочки в поисковом шаблоне.
		 * @param aSuperStar - флаг суперзвездочки.
		 */
		void Set( Binding aStarBinding, char aStarNum, bool aSuperStar )
		{
			vBinding = aStarBinding;
			Star::Set( aStarNum, aSuperStar );
		}

		/**
		 *  Установка данных.
		 * @param aStarNum - порядковый номер звездочки.
		 * @param aSuperStar - флаг суперзвездочки.
		 */
		void Set( char aStarNum, bool aSuperStar )
			{ Set( bUserRequest, aStarNum, aSuperStar ); }

	public:
		/** Получение размера памяти, необходимого для сохранения данного элемента. */
		unsigned int GetNeedMemorySize() const
			{ return 1 + Star::GetNeedMemorySize(); }

		/**
		 *  Сохранение в буффер.
		 * @param aBuffer - буффер для сохранения звездочки.
		 * @param aBufferSize - размер буффера.
		 * @param aResultSize - размер сохраненной звездочки.
		 */
		unsigned int Save( char * aBuffer ) const
		{
			unsigned int size = Star::Save( aBuffer );
			aBuffer[size] = static_cast<char>( vBinding );
			return size + 1;
		}

	public:
		/** Привязка звездочки. */
		Binding vBinding = bUserRequest;
	};

	/**
	 *  Манипулятор для ссылка на фрагмент запроса, соответствующий выделенной звездочке поискового шаблона с учетом
	 * специфики шаблонов эллипсисов.
	 */
	class EliStarManipulator : public Star
	{
	public:
		using Binding = EliStar::Binding;

	public:
		/** Конструктор. */
		EliStarManipulator() : Star() {}

		/**
		 *  Конструктор.
		 * @param aBuffer - буффер, содержащий данные элемента.
		 */
		EliStarManipulator( const void * aBuffer ) : Star( aBuffer )
		{
			vBinding = static_cast<EliStar::Binding>( static_cast<const char*>( aBuffer )[2] );
		}

	public:
		/** Получение привязки звездочки. */
		EliStar::Binding GetBinding() const { return vBinding; }

	public:
		/** Привязка звездочки. */
		Binding vBinding = Binding::bUserRequest;
	};
}


#endif  /** __InfPatternItemsTagEliStar_hpp__ */
