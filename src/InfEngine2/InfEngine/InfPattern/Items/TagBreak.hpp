#ifndef __InfPatternItemsTagBreak_hpp__
#define __InfPatternItemsTagBreak_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление тэга отбрасывания ответа.
	 */
	class TagBreak: public Base
	{
	public:
		TagBreak()
			{ Init(); }

	public:
		/** Инициализация. */
		void Init()
			{ vType = itBreak; }

	public:
		unsigned int GetNeedMemorySize() const
			{ return 0; }

		unsigned int Save( void * aBuffer ) const
			{ return 0; }
	};

	/**
	 *  Манипулятор тэга отбрасывания ответа.
	 */
	class TagBreakManipulator
	{
	public:
		TagBreakManipulator( const void * aBuffer = nullptr ):
			vBuffer( static_cast<const char *>( aBuffer ) ) {}

	public:
		/** Инициализация. */
		void Init( const void * aBuffer )
			{ vBuffer = static_cast<const char *>( aBuffer ); }

	private:
		/** Данные элемента. */
		const char * vBuffer = nullptr;
	};
}

#endif /** __InfPatternItemsTagBreak_hpp__ */
