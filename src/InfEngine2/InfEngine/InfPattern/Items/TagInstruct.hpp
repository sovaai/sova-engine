#ifndef __InfPatternItemsTagInstruct_hpp__
#define __InfPatternItemsTagInstruct_hpp__

#include "Array.hpp"

#include <InfEngine2/InfEngine/Vars/Id.hpp>

namespace InfPatternItems
{
	/** Представление элемента тэга - инструкции к изменению значения переменной. */
	class Instruct: public Base
	{
	public:
		Instruct()
			{ vType = itInstruct; }

	public:
		/** Установка данных. */
		void Set( Vars::Id aVarId, Array * aItems )
		{
			vVarId = aVarId;
			vItems = aItems;
		}

	public:
		unsigned int GetNeedMemorySize() const
			{ return sizeof( uint32_t ) + vItems->GetNeedMemorySize(); }

		unsigned int Save( void * aBuffer ) const
		{
			// Сохранение данных.
			char* ptr = static_cast<char*>( aBuffer );

			// Идентификатор переменной.
			*((uint32_t*)ptr) = vVarId.serialized();
			ptr += sizeof( uint32_t );

			// Сохранение значения.
			ptr += vItems->Save( ptr );

			return ptr - static_cast<char*>( aBuffer );
		}

	private:
		/** Идентификатор переменной. **/
		Vars::Id vVarId = {};

		/** Массив элементов значения. **/
		Array * vItems = nullptr;
	};

	/** Манипулятор для инструкции к изменению данных сессии. */
	class InstructManipulator
	{
		public:
			InstructManipulator( const void * aBuffer = nullptr ):
				vBuffer( static_cast<const char *>( aBuffer ) ) {}

		public:
			/** Получение идентификатора переменной. */
			Vars::Id GetVarId() const
			{
				Vars::Id VarsId;
				VarsId.load( *((uint32_t*)vBuffer) );
				return VarsId;
			}

			/** Получение массива элементов. */
			InfPatternItems::ArrayManipulator GetItems() const
				{ return InfPatternItems::ArrayManipulator( vBuffer ? (char*)((uint32_t*)vBuffer+1) : nullptr ); }

		private:
			/** Данные элемента. */
			const char * vBuffer = nullptr;
	};
}

#endif /** __InfPatternItemsTagInstruct_hpp__ */
