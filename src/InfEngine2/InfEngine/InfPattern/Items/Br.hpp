#ifndef __InfPatternItemsBr_hpp__
#define __InfPatternItemsBr_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление тэга переноса строки.
	 */
	class Br : public Base
	{
	public:
		/** Конструктор. */
		Br( )
			{ vType = itBr; }

	public:
		unsigned int GetNeedMemorySize( ) const
			{ return 0; }

		unsigned int Save( void * aBuffer ) const
			{ return 0; }
	};
}

#endif /** __InfPatternItemsBr_hpp__ */
