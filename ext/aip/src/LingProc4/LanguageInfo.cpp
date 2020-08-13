/**
 * @file   LanguageInfo.cpp
 * @author swizard <me@swizard.info>
 * @date   Fri Jun  6 16:26:55 2008
 *
 * @brief  Language information tools (implementation).
 *
 *
 */

#include <_include/_string.h>
#include <_include/_inttype.h>

#include "LanguageInfo.h"

static const LanguageInfoDb< uint32_t > _table32[] =
{
    { LNG_UNKNOWN,    0x0000, 0x0000, 0x0000, 0x0000, 0x0100, "??", "Unknown",    3.00 },
    { LNG_ENGLISH,    0x0003, 0x0008, 0x0003, 0x0008, 0x0200, "en", "English",    2.43 },
    { LNG_RUSSIAN,    0x0004, 0x0009, 0x0004, 0x0009, 0x0300, "ru", "Russian",    4.50 },
    { LNG_GERMAN,     0x0005, 0x000A, 0x0005, 0x000A, 0x0400, "de", "German",     3.05 },
    { LNG_FRENCH,     0x0006, 0x000B, 0x0006, 0x000B, 0x0500, "fr", "French",     2.75 },
    { LNG_SPANISH,    0x0007, 0x000C, 0x0007, 0x000C, 0x0600, "es", "Spanish",    2.43 },
    { LNG_VIETNAMESE, 0x0007, 0x000C, 0x0007, 0x000C, 0x0600, "vi", "Vietnamese", 3.22 },
    { LNG_ARABIC,     0x0007, 0x000C, 0x0007, 0x000C, 0x0700, "ar", "Arabic",     3.00 },
    { LNG_CYRILLIC,   0x0007, 0x000C, 0x0007, 0x000C, 0x0700, "cy", "Cyrillic",   3.00 },
    { LNG_UKRAINIAN,  0x000D, 0x000E, 0x000D, 0x000E, 0x0800, "ua", "Ukrainian",  3.00 },
    { LNG_EURO,       0x000D, 0x000E, 0x000D, 0x000E, 0x0800, "eu", "European",   3.00 },

    { LNG_BULGARIAN,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "bg", "Bulgarian",  3.00 },
    { LNG_CZECH,      0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "cz", "Czech",      3.00 },
    { LNG_DANISH,     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "dk", "Danish",     3.00 },
    { LNG_FINNISH,    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "fi", "Finnish",    3.00 },
    { LNG_HUNGARIAN,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "hu", "Hungarian",  3.00 },
    { LNG_ITALIAN,    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "it", "Italian",    3.00 },
    { LNG_DUTCH,      0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "nl", "Dutch",      3.00 },
    { LNG_POLISH,     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "pl", "Polish",     3.00 },
    { LNG_SLOVAK,     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "sk", "Slovak",     3.00 },
    { LNG_SWEDISH,    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "se", "Swedish",    3.00 },
    { LNG_PORTUGUESE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "pt", "Portuguese", 3.00 },

    { LNG_GREEK,             0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "gr",  "Greek",              3.00 },
    { LNG_ROMANIAN,          0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "ro",  "Romanian",           3.00 },
    { LNG_CROATIAN,          0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "hr",  "Croatian",           3.00 },
    { LNG_SLOVENIAN,         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "sl",  "Slovenian",          3.00 },
    { LNG_SERBIAN_LATIN,     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "srl", "Serbian (Latin)",    3.00 },
    { LNG_BELARUSIAN,        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "be",  "Belarusian",         3.00 },
    { LNG_TAJIK,             0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "tg",  "Tajik",              3.00 },
    { LNG_MACEDONIAN,        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "mk",  "Macedonian (FYROM)", 3.00 },
    { LNG_KAZAK,             0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "kk",  "Kazakh",             3.00 },
    { LNG_KYRGYZ,            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "ky",  "Kyrgyz",             3.00 },
    { LNG_TURKMEN,           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "tk",  "Turkmen",            3.00 },
    { LNG_TATAR,             0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "tt",  "Tatar",              3.00 },
    { LNG_MONGOLIAN,         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "mn",  "Mongolian",          3.00 },
    { LNG_SERBIAN,           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "sr",  "Serbian (Cyrillic)", 3.00 },
    { LNG_CATALAN,           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "ca",  "Catalan",            3.00 },
    { LNG_ICELANDIC,         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "is",  "Icelandic",          3.00 },
    { LNG_NORWEGIAN,         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "nn",  "Norwegian",          3.00 },
    { LNG_NORWEGIAN_NYNORSK, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "nn",  "Norwegian",          3.00 },
    { LNG_ALBANIAN,          0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "sq",  "Albanian",           3.00 },
    { LNG_BASQUE,            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "eub", "Basque",             3.00 },
    { LNG_MALTESE,           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "mt",  "Maltese",            3.00 },
    { LNG_MALAY,             0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "ms",  "Malay",              3.00 },
    { LNG_LUXEMBOURGISH,     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "lb",  "Luxembourgish",      3.00 },
    { LNG_GREENLANDIC,       0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "kl",  "Greenlandic",        3.00 },
    { LNG_IRISH,             0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "ga",  "Irish",              3.00 },
    { LNG_TURKISH,           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "tr",  "Turkish",            3.00 },
    { LNG_HEBREW,            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "he",  "Hebrew",             3.00 },
    { LNG_ARABIC,            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "ar",  "Arabic",             3.00 },
    { LNG_PERSIAN,           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "fa",  "Persian (Farsi)",    3.00 },
    { LNG_ESTONIAN,          0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "et",  "Estonian",           3.00 },
    { LNG_LATVIAN,           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "lv",  "Latvian",            3.00 },
    { LNG_LITHUANIAN,        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "lt",  "Lithuanian",         3.00 },

    { LNG_OTHER,             0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "other", "Other",            3.00 },
    { LNG_INVALID,           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "",      "",                 3.00 }
};

static const LanguageInfoDb< uint64_t > _table64[] =
{
    { LNG_UNKNOWN,    0x0000, 0x0000, 0x0000, 0x0000, 0x0100, "??", "Unknown",    3.00 },
    { LNG_ENGLISH,    0x0003, 0x0004, 0x0003, 0x0004, 0x0200, "en", "English",    2.43 },
    { LNG_RUSSIAN,    0x0005, 0x0006, 0x0005, 0x0006, 0x0300, "ru", "Russian",    4.50 },
    { LNG_GERMAN,     0x0007, 0x0008, 0x0007, 0x0008, 0x0400, "de", "German",     3.05 },
    { LNG_FRENCH,     0x0009, 0x000A, 0x0009, 0x000A, 0x0500, "fr", "French",     2.75 },
    { LNG_SPANISH,    0x000B, 0x000C, 0x000B, 0x000C, 0x0600, "es", "Spanish",    2.43 },
    { LNG_VIETNAMESE, 0x000D, 0x000E, 0x000D, 0x000E, 0x0700, "vi", "Vietnamese", 3.22 },
    { LNG_UKRAINIAN,  0x000F, 0x0010, 0x000F, 0x0010, 0x0800, "ua", "Ukrainian",  3.00 },
    { LNG_ARABIC,     0x0011, 0x0012, 0x0011, 0x0012, 0x0900, "ar", "Arabic",     3.00 },
    { LNG_EURO,       0x0013, 0x0014, 0x0013, 0x0014, 0x0A00, "eu", "European",   3.00 },

    { LNG_BULGARIAN,  0x0015, 0x0016, 0x0015, 0x0016, 0x0000, "bg", "Bulgarian",  3.00 },
    { LNG_CZECH,      0x0017, 0x0018, 0x0017, 0x0018, 0x0000, "cz", "Czech",      3.00 },
    { LNG_DANISH,     0x0019, 0x001A, 0x0019, 0x001A, 0x0000, "dk", "Danish",     3.00 },
    { LNG_FINNISH,    0x001B, 0x001C, 0x001B, 0x001C, 0x0000, "fi", "Finnish",    3.00 },
    { LNG_HUNGARIAN,  0x001D, 0x001E, 0x001D, 0x001E, 0x0000, "hu", "Hungarian",  3.00 },
    { LNG_ITALIAN,    0x001F, 0x0020, 0x001F, 0x0020, 0x0000, "it", "Italian",    3.00 },
    { LNG_DUTCH,      0x0021, 0x0022, 0x0021, 0x0022, 0x0000, "nl", "Dutch",      3.00 },
    { LNG_POLISH,     0x0023, 0x0024, 0x0023, 0x0024, 0x0000, "pl", "Polish",     3.00 },
    { LNG_SLOVAK,     0x0025, 0x0026, 0x0025, 0x0026, 0x0000, "sk", "Slovak",     3.00 },
    { LNG_SWEDISH,    0x0027, 0x0028, 0x0027, 0x0028, 0x0000, "se", "Swedish",    3.00 },
    { LNG_PORTUGUESE, 0x0029, 0x002A, 0x0029, 0x002A, 0x0000, "pt", "Portuguese", 3.00 },

    { LNG_GREEK,             0x002B, 0x002C, 0x002B, 0x002C, 0x0000, "gr",  "Greek",              3.00 },
    { LNG_ROMANIAN,          0x002D, 0x002E, 0x002D, 0x002E, 0x0000, "ro",  "Romanian",           3.00 },
    { LNG_CROATIAN,          0x002F, 0x0030, 0x002F, 0x0030, 0x0000, "hr",  "Croatian",           3.00 },
    { LNG_SLOVENIAN,         0x0031, 0x0032, 0x0031, 0x0032, 0x0000, "sl",  "Slovenian",          3.00 },
    { LNG_SERBIAN_LATIN,     0x0033, 0x0034, 0x0033, 0x0034, 0x0000, "srl", "Serbian (Latin)",    3.00 },
    { LNG_BELARUSIAN,        0x0035, 0x0036, 0x0035, 0x0036, 0x0000, "be",  "Belarusian",         3.00 },
    { LNG_TAJIK,             0x0037, 0x0038, 0x0037, 0x0038, 0x0000, "tg",  "Tajik",              3.00 },
    { LNG_MACEDONIAN,        0x0039, 0x003A, 0x0039, 0x003A, 0x0000, "mk",  "Macedonian (FYROM)", 3.00 },
    { LNG_KAZAK,             0x003B, 0x003C, 0x003B, 0x003C, 0x0000, "kk",  "Kazakh",             3.00 },
    { LNG_KYRGYZ,            0x003D, 0x003E, 0x003D, 0x003E, 0x0000, "ky",  "Kyrgyz",             3.00 },
    { LNG_TURKMEN,           0x003F, 0x0040, 0x003F, 0x0040, 0x0000, "tk",  "Turkmen",            3.00 },
    { LNG_TATAR,             0x0041, 0x0042, 0x0041, 0x0042, 0x0000, "tt",  "Tatar",              3.00 },
    { LNG_MONGOLIAN,         0x0043, 0x0044, 0x0043, 0x0044, 0x0000, "mn",  "Mongolian",          3.00 },
    { LNG_SERBIAN,           0x0045, 0x0046, 0x0045, 0x0046, 0x0000, "sr",  "Serbian (Cyrillic)", 3.00 },
    { LNG_CATALAN,           0x0047, 0x0048, 0x0047, 0x0048, 0x0000, "ca",  "Catalan",            3.00 },
    { LNG_ICELANDIC,         0x0049, 0x004A, 0x0049, 0x004A, 0x0000, "is",  "Icelandic",          3.00 },
    { LNG_NORWEGIAN,         0x004B, 0x004C, 0x004B, 0x004C, 0x0000, "nn",  "Norwegian",          3.00 },
    { LNG_NORWEGIAN_NYNORSK, 0x004D, 0x004E, 0x004D, 0x004E, 0x0000, "nn",  "Norwegian",          3.00 },
    { LNG_ALBANIAN,          0x004F, 0x0050, 0x004F, 0x0050, 0x0000, "sq",  "Albanian",           3.00 },
    { LNG_BASQUE,            0x0051, 0x0052, 0x0051, 0x0052, 0x0000, "eub", "Basque",             3.00 },
    { LNG_MALTESE,           0x0053, 0x0054, 0x0053, 0x0054, 0x0000, "mt",  "Maltese",            3.00 },
    { LNG_MALAY,             0x0055, 0x0056, 0x0055, 0x0056, 0x0000, "ms",  "Malay",              3.00 },
    { LNG_LUXEMBOURGISH,     0x0057, 0x0058, 0x0057, 0x0058, 0x0000, "lb",  "Luxembourgish",      3.00 },
    { LNG_GREENLANDIC,       0x0059, 0x005A, 0x0059, 0x005A, 0x0000, "kl",  "Greenlandic",        3.00 },
    { LNG_IRISH,             0x005B, 0x005C, 0x005B, 0x005C, 0x0000, "ga",  "Irish",              3.00 },
    { LNG_TURKISH,           0x005D, 0x005E, 0x005D, 0x005E, 0x0000, "tr",  "Turkish",            3.00 },
    { LNG_HEBREW,            0x005F, 0x0060, 0x005F, 0x0060, 0x0000, "he",  "Hebrew",             3.00 },
    { LNG_PERSIAN,           0x0061, 0x0062, 0x0061, 0x0062, 0x0000, "fa",  "Persian (Farsi)",    3.00 },
    { LNG_ESTONIAN,          0x0063, 0x0064, 0x0063, 0x0064, 0x0000, "et",  "Estonian",           3.00 },
    { LNG_LATVIAN,           0x0065, 0x0066, 0x0065, 0x0066, 0x0000, "lv",  "Latvian",            3.00 },
    { LNG_LITHUANIAN,        0x0067, 0x0068, 0x0067, 0x0068, 0x0000, "lt",  "Lithuanian",         3.00 },

    { LNG_CYRILLIC,          0x0069, 0x0070, 0x0069, 0x0070, 0x0000, "cy",  "Cyrillic",           3.00 },
    { LNG_OTHER,             0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "other", "Other",            3.00 },
    { LNG_INVALID,           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, "",      "",                 3.00 }
};

template< typename LexidType > const LanguageInfoDb< LexidType > *_getTableEntry( int i );
template< > const LanguageInfoDb< uint32_t > *_getTableEntry< uint32_t >( int i ) { return _table32 + i; }
template< > const LanguageInfoDb< uint64_t > *_getTableEntry< uint64_t >( int i ) { return _table64 + i; }

template< typename LexidType >
const LanguageInfoDb< LexidType > *LanguageInfoDb< LexidType >::getLanguageInfo( LangCodes lang )
{
    for ( int i = 0; _getTableEntry< LexidType >(i)->language != LNG_INVALID; i++ )
        if ( _getTableEntry< LexidType >(i)->language == lang )
            return _getTableEntry< LexidType >(i);
    return 0;
}

template< typename LexidType >
const LanguageInfoDb< LexidType > *LanguageInfoDb< LexidType >::getLanguageInfo( const char *shortName )
{
    for ( int i = 0; _getTableEntry< LexidType >(i)->language != LNG_INVALID; i++ )
        if ( strcmp(_getTableEntry< LexidType >(i)->shortName, shortName) == 0 )
            return _getTableEntry< LexidType >(i);
    return 0;
}

template class LanguageInfoDb< uint32_t >;
template class LanguageInfoDb< uint64_t >;

