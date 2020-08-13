#ifndef SYMBOLYM_TABLE_UKRAINIAN_HPP
#define SYMBOLYM_TABLE_UKRAINIAN_HPP

#include "../SymbolymCommon.hpp"

namespace SymbolymCommon {
	namespace Tables {
		namespace Ukraine {
			static const char * group1[] = { "Ї", "ї", "І", "і" };	// => i
			static const char * group2[] = { "Ґ", "ґ" };			// => г
			static const char * group3[] = { "Є", "є" };			// => э

			static const Symbolym table[] = {
				{ "i", group1, sizeof( group1 ) / sizeof( group1[0] ) },
				{ "г", group2, sizeof( group2 ) / sizeof( group2[0] ) },
				{ "э", group3, sizeof( group3 ) / sizeof( group3[0] ) },
			};

			unsigned int table_size = sizeof( table ) / sizeof( Symbolym );
		}
	}
}

#endif //SYMBOLYM_TABLE_UKRAINIAN_HPP
