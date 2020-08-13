#ifndef __Disableautovars_hpp__
#define __Disableautovars_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление тэга отключения автоматического изменения переменных.
	 */
	class Disableautovars : public Base
	{
	public:
		/** Конструктор. */
		Disableautovars( )
			{ vType = itDisableautovars; }

	public:
		unsigned int GetNeedMemorySize( ) const
			{ return 0; }

		unsigned int Save( void * aBuffer ) const
			{ return 0; }
	};
}


#endif /** __Disabelautovars_hpp__ */
