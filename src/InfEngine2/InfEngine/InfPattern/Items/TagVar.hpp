#ifndef __InfPatternItemsTagVar_hpp__
#define __InfPatternItemsTagVar_hpp__

#include "Base.hpp"

#include "../../Vars/Id.hpp"

namespace InfPatternItems
{
	/** Тэг DL, представляющий значение переменной. */
	class TagVar : public Base
	{
	public:
		TagVar()
			{ vType = itVar; }

	public:
		/** Установка данных. */
		void Set( Vars::Id aVarId )
			{ vVarId = aVarId; }

		Vars::Id GetVarId() const
			{ return vVarId; }

	public:
		unsigned int GetNeedMemorySize() const
			{ return sizeof( uint32_t ); }

		unsigned int Save( void * aBuffer ) const
		{
			*((uint32_t*)aBuffer) = vVarId.serialized();
			return sizeof( uint32_t );
		}

	private:
		/** Идентификатор переменной. */
		Vars::Id vVarId;
	};

	/** Манипулятор для переменной. */
	class TagVarManipulator
	{
	public:
		TagVarManipulator( const void * aBuffer = nullptr ):
			vBuffer( static_cast<const char *>( aBuffer ) ) {}

	public:
		/** Инициализация. */
		void Init( const void * aBuffer )
			{ vBuffer = static_cast<const char *>( aBuffer ); }

	public:
		/** Получение идентификатора переменной. */
		Vars::Id GetVarId() const
		{
			Vars::Id VarsId;
			VarsId.load( *((uint32_t*)vBuffer) );
			return VarsId;
		}

	private:
		/** Данные элемента. */
		const char * vBuffer = nullptr;
	} ;
}

#endif /** __InfPatternItemsTagVar_hpp__ */
