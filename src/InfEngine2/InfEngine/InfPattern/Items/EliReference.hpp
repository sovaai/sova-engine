#ifndef __InfPatternItemsEliReference_hpp__
#define __InfPatternItemsEliReference_hpp__

#include "Reference.hpp"

namespace InfPatternItems
{
	/**
	 *  Ссылка на фрагмент запроса, соответствующий выделенному фрагменту поискового шаблона с учетом
	 * специфики шаблонов эллипсисов.
	 */
	class EliReference: public Reference
	{
	public:
		/** Конструктор. */
		EliReference(): Reference()
			{ vBinding = bUserRequest; vType = itEliReference; }

	public:
		/** Привязка ссылки к одному из поисковых шаблонов из шаблона эллипсисов. */
		typedef enum
		{
			/** Привязка к текущему запросу пользователя. */
			bUserRequest    = 0,
			/** Привязка к последнему ответу инфа. */
			bInfPreResponse = 1,
			/** Привязка к предыдущему запросу пользователя. */
			bUserPreRequest = 2
		} Binding;

	public:
		/**
		 *  Установка данных.
		 * @param aBinding - привязка ссылки.
		 * @param aRefNum - номер выделенного фрагмента поискового шаблона, на который указывает ссылка.
		 */
		void Set( Binding aBinding, unsigned int aRefNum )
			{ vRefNum = aRefNum; vBinding = aBinding; }

		/**
		 *  Установка данных.
		 * @param aRefNum - номер выделенного фрагмента поискового шаблона, на который указывает ссылка.
		 */
		void Set( unsigned int aRefNum )
			{ Set( bUserRequest, aRefNum ); }

	public:
		unsigned int GetNeedMemorySize() const
			{ return Reference::GetNeedMemorySize() + 1; }

		unsigned int Save( void * aBuffer ) const
		{
			// Сохранение базовой части ссылки.
			unsigned int size = Reference::Save( aBuffer );

			// Сохранение привязки.
			static_cast<char*>( aBuffer )[size] = static_cast<char>(vBinding);

			return size + 1;
		}

	protected:
		/** Привязка ссылки. */
		Binding vBinding;
	};

	/**
	 *  Манипулятор для ссылки на фрагмент запроса, соответствующий выделенному фрагменту поискового шаблона с учетом
	 * специфики шаблонов эллипсисов.
	 **/
	class EliReferenceManipulator: public ReferenceManipulator
	{
	public:
		/**
		 *  Конструктор.
		 * @param aBuffer - буффер, содержащий данные элемента.
		 */
		EliReferenceManipulator( const void * aBuffer = nullptr ):
			ReferenceManipulator( aBuffer ) {}

	public:
		/** Получение привязки ссылки. */
		EliReference::Binding GetBinding() const
			{ return static_cast<EliReference::Binding>(vBuffer[sizeof(uint32_t)]); }
	};
}

#endif  /* __InfPatternItemsEliReference_hpp__ */

