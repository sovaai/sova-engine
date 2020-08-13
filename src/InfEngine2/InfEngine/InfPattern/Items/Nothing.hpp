#ifndef __Nothing_hpp__
#define __Nothing_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление тэга nothing.
	 */
	class Nothing : public Base
	{
	public:
		Nothing( bool aSkipAuto = false ) { vType = itNothing; }

	public:
		unsigned int GetNeedMemorySize( ) const
			{ return sizeof( uint32_t ); }

		unsigned int Save( void * /*aBuffer*/ ) const
			{ return 0; }
	};
}

#endif /** __Nothing_hpp__ */
