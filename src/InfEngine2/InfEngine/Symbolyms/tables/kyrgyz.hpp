#ifndef SYMBOLYM_TABLE_KYRGYZ_HPP
#define SYMBOLYM_TABLE_KYRGYZ_HPP

#include "../SymbolymCommon.hpp"

namespace SymbolymCommon {
	namespace Tables {
		namespace Kyrgyz {
			static const char * group1[] = { "Ң", "ң" };	// => н
			static const char * group2[] = { "Ө", "ө" };	// => о
			static const char * group3[] = { "Ү", "ү" };	// => у

			static const Symbolym table[] = {
				{ "н", group1, sizeof( group1 ) / sizeof( group1[0] ) },
				{ "о", group2, sizeof( group2 ) / sizeof( group2[0] ) },
				{ "у", group3, sizeof( group3 ) / sizeof( group3[0] ) },
			};

			unsigned int table_size = sizeof( table ) / sizeof( Symbolym );
		}
	}
}

#endif //SYMBOLYM_TABLE_KYRGYZ_HPP
