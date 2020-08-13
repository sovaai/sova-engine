#ifndef __InfPatternItemIf_hpp__
#define __InfPatternItemIf_hpp__

#include "BaseCondition.hpp"
#include "Array.hpp"
#include "TagFunction.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление элемента условного оператора.
	 */
	class TagIf: public Base
	{
	public:
		TagIf( size_t aPos ) :
			Base( itIf, aPos ) {}

	public:
		/** Установка данных. */
		void Set( const TagFunction * aCondition, const TagFunction::Argument * aTrueItems, const TagFunction::Argument * aFalseItems )
		{
			vTrueItems = aTrueItems;
			vFalseItems = aFalseItems;
			vCondition = aCondition;
		}

	public:
		unsigned int GetNeedMemorySize() const;

		unsigned int Save( void * aBuffer ) const;

	public:
		/** Получение тела успешного варианта. */
		Array GetSuccessBody() const
			{ return vTrueItems ? vTrueItems->GetItems() : Array{}; }

		/** получение тела неуспешного варианта. */
		Array GetFailedBody() const
			{ return vFalseItems ? vFalseItems->GetItems() : Array{}; }

	private:
		/** Элементы утвердительного решения. */
		const TagFunction::Argument * vTrueItems = nullptr;

		/** Элементы отрицательного решения. */
		const TagFunction::Argument * vFalseItems = nullptr;

		/** Условие. */
		const TagFunction * vCondition = nullptr;
	};

	/**
	 *  Манипулятор тэга условного оператора.
	 */
	class TagIfManipulator
	{
	public:
		TagIfManipulator( const void * aBuffer = nullptr ):
			vBuffer( static_cast<const char *>( aBuffer ) ) {}

	public:
		/** Инициализация. */
		void Init( const void * aBuffer )
			{ vBuffer = static_cast<const char *>( aBuffer ); }

	public:
		/** Получение идентификатора условия. */
		const char * GetCondition() const
			{ return vBuffer ? vBuffer + *reinterpret_cast<const uint32_t*>(vBuffer + sizeof(uint32_t)) : nullptr; }

		/** Получение массива then-элементов. */
		const char * GetTrueItems() const
			{ return vBuffer ? vBuffer + *reinterpret_cast<const uint32_t*>(vBuffer + 2*sizeof(uint32_t)) : nullptr; }

		/** Получение массива else-элементов. */
		const char * GetFalseItems( ) const
			{ return vBuffer && *reinterpret_cast<const uint32_t*>(vBuffer + 3*sizeof(uint32_t)) ? vBuffer + *reinterpret_cast<const uint32_t*>(vBuffer + 3*sizeof(uint32_t)) : nullptr; }

	private:
		/** Данные элемента. */
		const char * vBuffer = nullptr;
	};
}

#endif /** __InfPatternItemIf_hpp__ */
