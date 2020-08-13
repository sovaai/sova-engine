#ifndef __InfPatternItemsReference_hpp__
#define __InfPatternItemsReference_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Ссылка на фрагмент запроса, соответствующий выделенному фрагменту поискового шаблона.
	 */
	class Reference: public Base
	{
	public:
		/** Конструктор. */
		Reference()
			{ vType = itReference; }

	public:
		/**
		 *  Установка данных.
		 * @param aRefNum - номер выделенного фрагмента поискового шаблона, на который указывает ссылка.
		 */
		void Set( unsigned int aRefNum )
			{ vRefNum = aRefNum; }

	public:
		unsigned int GetNeedMemorySize() const
			{ return sizeof( uint32_t ); }

		unsigned int Save( void * aBuffer ) const
		{
			*((uint32_t*)aBuffer) = vRefNum;
			return sizeof( uint32_t );
		}

	protected:
		/** Номер референции. */
		unsigned int vRefNum = 0;
	};

	/**
	 *  Манипулятор для ссылки на фрагмент запроса, соответствующий выделенному фрагменту поискового шаблона.
	 */
	class ReferenceManipulator
	{
	public:
		/** Конструктор. */
		ReferenceManipulator():
			vBuffer( nullptr ) {}

		/**
		 *  Конструктор.
		 * @param aBuffer - буффер, содержащий данные элемента.
		 */
		ReferenceManipulator( const void* aBuffer ):
			vBuffer( static_cast<const char *>(aBuffer) ) {}

	public:
		/**
		 *  Инициализация.
		 * @param aBuffer - ссылка на буффер, содержащий сохраненный элемент.
		 */
		void Init( const void* aBuffer )
			{ vBuffer = static_cast<const char *>(aBuffer); }

	public:
		/** Получение номера референции. */
		unsigned int GetRefNum() const
			{ return *((uint32_t*)vBuffer); }

	protected:
		/** Данные элемента. */
		const char * vBuffer = nullptr;
	};
}

#endif  /* __InfPatternItemsReference_hpp__ */
