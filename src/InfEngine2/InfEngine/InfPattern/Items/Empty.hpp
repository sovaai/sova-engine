#ifndef INFPATTERN_ITEMS_EMPTY_HPP
#define INFPATTERN_ITEMS_EMPTY_HPP

#include "Base.hpp"

namespace InfPatternItems
{
	template<ItemType T>
	class Empty : public Base
	{
	public:
		/**
		 *  Конструктор элемента DL без дополнительных данных.
		 * @param aPos - позиция элемента в строке.
		 */
		Empty( size_t aPos ) : Base( T, aPos ) {}

	public:
		unsigned int GetNeedMemorySize() const { return 0; }

		unsigned int Save( void * aBuffer ) const { return 0; }
	};
}

#endif // INFPATTERN_ITEMS_EMPTY_HPP
