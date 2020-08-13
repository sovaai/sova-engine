#ifndef __InfPatternItemsBaseCondition_hpp__
#define __InfPatternItemsBaseCondition_hpp__

#include "Base.hpp"

#include "../../Vars/Id.hpp"

// Значение уникального, всегда верного, сравнения.
#define InfConditionAny "COMPARE_IS_ALWAYS_SUCCESSFULL"
#define InfConditionAnyShort "CIAS"

/** Тип условия. **/
typedef enum
{
	InfConditionExist,		// Условие существования ( не равенства пустоте ) значения переменной.
	InfConditionDontExist,	// Условие не существования ( равенства пустоте ) значения переменной.
	InfConditionEqual,		// Условие равенства значения переменной с заданной строкой.
	InfConditionNotEqual,	// Условие не равенства значения переменной и заданной строки.
	InfConditionAnyValue,	// Условие верное всегда.
	InfConditionFunction,	// Условие, определяемое функцией.
	InfConditionUndefined	// Значение для инициализации.
} InfBaseConditionType;

namespace InfPatternItems
{
	/**
	 *  Базовое атомарное условие.
	 */
	class BaseCondition: public Base
	{
	public:
		/** Конструктор. */
		BaseCondition()
			{ vType = itCondition; }

	public:
		/**
		 *  Установка идентификатора переменной.
		 * @param aVarId - идентификатор переменной.
		 */
		void SetVarId( Vars::Id aVarId )
			{ vVarId = aVarId; }

		/**
		 *  Установка идентификатора условия.
		 * @param aConditionId - идентификатор условия.
		 */
		void SetConditionId( unsigned int aConditionId )
			{ vConditionId = aConditionId; }

	public:
		/** Получение идентификатора переменной. */
		Vars::Id GetVarId() const
			{ return vVarId; }

		/** Получение идентификатора условия. */
		unsigned int GetConditionId() const
			{ return vConditionId; }

	public:
		unsigned int GetNeedMemorySize() const
			{ return sizeof( uint32_t ); }

		unsigned int Save( void * aBuffer ) const
		{
			// Проверка аргументов.
			if( !aBuffer )
				return 0;

			*((uint32_t*)aBuffer) = vConditionId;

			return sizeof( uint32_t );
		}


	private:
		unsigned int vConditionId = -1;

		Vars::Id vVarId;
	};

	class BaseConditionManipulator
	{
	public:
		BaseConditionManipulator( const void * aBuffer ):
			vBuffer( aBuffer ) {}

	public:
		void Init( const void * aBuffer )
			{ vBuffer = aBuffer; }

	public:
		unsigned int GetConditionId() const
			{ return *static_cast<const uint32_t*>(vBuffer); }

	private:
		const void * vBuffer;
	};
}

#endif /** __InfPatternItemsBaseCondition_hpp__ */
