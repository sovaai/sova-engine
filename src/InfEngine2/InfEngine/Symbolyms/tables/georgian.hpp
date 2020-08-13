#ifndef SYMBOLYM_TABLE_GEORGIAN_HPP
#define SYMBOLYM_TABLE_GEORGIAN_HPP

#include "../SymbolymCommon.hpp"

namespace SymbolymCommon {
	namespace Tables {
		namespace Georgian {
			static const char * group1[]	= { "Ⴀ" };
			static const char * group2[]	= { "Ⴁ" };
			static const char * group3[]	= { "Ⴂ" };
			static const char * group4[]	= { "Ⴃ" };
			static const char * group5[]	= { "Ⴄ" };
			static const char * group6[]	= { "Ⴅ" };
			static const char * group7[]	= { "Ⴆ" };
			static const char * group8[]	= { "Ⴇ" };
			static const char * group9[]	= { "Ⴈ" };
			static const char * group10[]	= { "Ⴉ" };
			static const char * group11[]	= { "Ⴊ" };
			static const char * group12[]	= { "Ⴋ" };
			static const char * group13[]	= { "Ⴌ" };
			static const char * group14[]	= { "Ⴍ" };
			static const char * group15[]	= { "Ⴎ" };
			static const char * group16[]	= { "Ⴏ" };
			static const char * group17[]	= { "Ⴐ" };
			static const char * group18[]	= { "Ⴑ" };
			static const char * group19[]	= { "Ⴒ" };
			static const char * group20[]	= { "Ⴓ" };
			static const char * group21[]	= { "Ⴔ" };
			static const char * group22[]	= { "Ⴕ" };
			static const char * group23[]	= { "Ⴖ" };
			static const char * group24[]	= { "Ⴗ" };
			static const char * group25[]	= { "Ⴘ" };
			static const char * group26[]	= { "Ⴙ" };
			static const char * group27[]	= { "Ⴚ" };
			static const char * group28[]	= { "Ⴛ" };
			static const char * group29[]	= { "Ⴜ" };
			static const char * group30[]	= { "Ⴝ" };
			static const char * group31[]	= { "Ⴞ" };
			static const char * group32[]	= { "Ⴟ" };
			static const char * group33[]	= { "Ⴠ" };
			static const char * group34[]	= { "Ⴡ" };
			static const char * group35[]	= { "Ⴢ" };
			static const char * group36[]	= { "Ⴣ" };
			static const char * group37[]	= { "Ⴤ" };
			static const char * group38[]	= { "Ⴥ" };

			static const Symbolym table[] = {
				{ "ა",	group1,		sizeof( group1	) / sizeof( group1[0]	) },
				{ "ბ",	group2,		sizeof( group2	) / sizeof( group2[0]	) },
				{ "გ",	group3,		sizeof( group3	) / sizeof( group3[0]	) },
				{ "დ",	group4,		sizeof( group4	) / sizeof( group4[0]	) },
				{ "ე",	group5,		sizeof( group5	) / sizeof( group5[0]	) },
				{ "ვ",	group6,		sizeof( group6	) / sizeof( group6[0]	) },
				{ "ზ",	group7,		sizeof( group7	) / sizeof( group7[0]	) },
				{ "თ",	group8,		sizeof( group8	) / sizeof( group8[0]	) },
				{ "ი",	group9,		sizeof( group9	) / sizeof( group9[0]	) },
				{ "კ",	group10,	sizeof( group10	) / sizeof( group10[0]	) },
				{ "ლ",	group11,	sizeof( group11	) / sizeof( group11[0]	) },
				{ "მ",	group12,	sizeof( group12	) / sizeof( group12[0]	) },
				{ "ნ",	group13,	sizeof( group13	) / sizeof( group13[0]	) },
				{ "ო",	group14,	sizeof( group14	) / sizeof( group14[0]	) },
				{ "პ",	group15,	sizeof( group15	) / sizeof( group15[0]	) },
				{ "ჟ",	group16,	sizeof( group16	) / sizeof( group16[0]	) },
				{ "რ",	group17,	sizeof( group17	) / sizeof( group17[0]	) },
				{ "ს",	group18,	sizeof( group18	) / sizeof( group18[0]	) },
				{ "ტ",	group19,	sizeof( group19	) / sizeof( group19[0]	) },
				{ "უ",	group20,	sizeof( group20	) / sizeof( group20[0]	) },
				{ "ფ",	group21,	sizeof( group21	) / sizeof( group21[0]	) },
				{ "ქ",	group22,	sizeof( group22	) / sizeof( group22[0]	) },
				{ "ღ",	group23,	sizeof( group23	) / sizeof( group23[0]	) },
				{ "ყ",	group24,	sizeof( group24	) / sizeof( group24[0]	) },
				{ "შ",	group25,	sizeof( group25	) / sizeof( group25[0]	) },
				{ "ჩ",	group26,	sizeof( group26	) / sizeof( group26[0]	) },
				{ "ც",	group27,	sizeof( group27	) / sizeof( group27[0]	) },
				{ "ძ",	group28,	sizeof( group28	) / sizeof( group28[0]	) },
				{ "წ",	group29,	sizeof( group29	) / sizeof( group29[0]	) },
				{ "ჭ",	group30,	sizeof( group30	) / sizeof( group30[0]	) },
				{ "ხ",	group31,	sizeof( group31	) / sizeof( group31[0]	) },
				{ "ჯ",	group32,	sizeof( group32	) / sizeof( group32[0]	) },
				{ "ჰ",	group33,	sizeof( group33	) / sizeof( group33[0]	) },
				{ "ჱ",	group34,	sizeof( group34	) / sizeof( group34[0]	) },
				{ "ჲ",	group35,	sizeof( group35	) / sizeof( group35[0]	) },
				{ "ჳ",	group36,	sizeof( group36	) / sizeof( group36[0]	) },
				{ "ჴ",	group37,	sizeof( group37	) / sizeof( group37[0]	) },
				{ "ჵ",	group38,	sizeof( group38	) / sizeof( group38[0]	) }
			};

			unsigned int table_size = sizeof( table ) / sizeof( Symbolym );
		}
	}
}

#endif // SYMBOLYM_TABLE_GEORGIAN_HPP
