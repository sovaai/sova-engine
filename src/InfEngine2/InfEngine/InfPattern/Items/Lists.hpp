#ifndef INFPATTERN_LIST_HPP
#define INFPATTERN_LIST_HPP

#include "Empty.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление элемента языка DL: начало списка.
	 */
	using StartUList = Empty<itStartUList>;
	using StartOList = Empty<itStartOList>;

	/**
	 *  Представление элемента языка DL: элемент списка.
	 */
	using ListItem = Empty<itListItem>;

	/**
	 *  Представление элемента языка DL: конец списка.
	 */
	using EndList = Empty<itEndList>;
}

#endif // INFPATTERN_LIST_HPP
