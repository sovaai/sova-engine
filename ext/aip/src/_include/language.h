#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__

#define LG_UNKNOWN    0x0000
#define LG_INVALID    0xFFFF
#define LG_BULGARIAN  0x0402
#define LG_CZECH      0x0405
#define LG_DANISH     0x0406
#define LG_GERMAN     0x0407
#define LG_ENGLISH_US 0x0409
#define LG_SPANISH    0x040A
#define LG_FINNISH    0x040B
#define LG_FRENCH     0x040C
#define LG_HUNGARIAN  0x040E
#define LG_ITALIAN    0x0410
#define LG_DUTCH      0x0413
#define LG_POLISH     0x0415
#define LG_RUSSIAN    0x0419
#define LG_SLOVAK     0x041B
#define LG_SWEDISH    0x041D
#define LG_UKRAINIAN  0x0422
#define LG_GEORGIAN   0x0423
#define LG_ENGLISH_UK 0x0809
#define LG_PORTUGUESE 0x0816

#define LG_GREEK             0x0408
#define LG_ROMANIAN          0x0418
#define LG_CROATIAN          0x041A
#define LG_SLOVENIAN         0x0424
#define LG_SERBIAN_LATIN     0x081A
#define LG_BELARUSIAN        0x0423
#define LG_TAJIK             0x0428
#define LG_MACEDONIAN        0x042F
#define LG_KAZAK             0x043F
#define LG_KYRGYZ            0x0440
#define LG_TURKMEN           0x0442
#define LG_TATAR             0x0444
#define LG_MONGOLIAN         0x0450
#define LG_SERBIAN           0x0C1A
#define LG_CATALAN           0x0403
#define LG_ICELANDIC         0x040F
#define LG_NORWEGIAN         0x0414
#define LG_NORWEGIAN_NYNORSK 0x0814
#define LG_ALBANIAN          0x041C
#define LG_BASQUE            0x042D
#define LG_MALTESE           0x043A
#define LG_MALAY             0x043e
#define LG_LUXEMBOURGISH     0x046E
#define LG_GREENLANDIC       0x046F
#define LG_IRISH             0x083C
#define LG_TURKISH           0x041F
#define LG_HEBREW            0x040D
#define LG_ARABIC            0x0401
#define LG_PERSIAN           0x0429
#define LG_ESTONIAN          0x0425
#define LG_LATVIAN           0x0426
#define LG_LITHUANIAN        0x0427
#define LG_VIETNAMESE        0x042A

#define LG_ENGLISH    LG_ENGLISH_US

#define LG_OTHER      0xFFFD
#define LG_EURO       0xFFFE
#define LG_HIER_UTF   0xFFFC
#define LG_CYRILLIC   0xFFFB

enum LangCodes
{
  LNG_UNKNOWN = LG_UNKNOWN,
  LNG_INVALID = LG_INVALID,
  LNG_BULGARIAN = LG_BULGARIAN,
  LNG_CZECH = LG_CZECH,
  LNG_DANISH = LG_DANISH,
  LNG_GERMAN = LG_GERMAN,
  LNG_ENGLISH_US = LG_ENGLISH_US,
  LNG_SPANISH = LG_SPANISH,
  LNG_FINNISH = LG_FINNISH,
  LNG_FRENCH = LG_FRENCH,
  LNG_HUNGARIAN = LG_HUNGARIAN,
  LNG_ITALIAN = LG_ITALIAN,
  LNG_DUTCH = LG_DUTCH,
  LNG_POLISH = LG_POLISH,
  LNG_RUSSIAN = LG_RUSSIAN,
  LNG_SLOVAK = LG_SLOVAK,
  LNG_SWEDISH = LG_SWEDISH,
  LNG_ENGLISH_UK = LG_ENGLISH_UK,
  LNG_PORTUGUESE = LG_PORTUGUESE,
  LNG_UKRAINIAN = LG_UKRAINIAN,

  LNG_GEORGIAN = LG_GEORGIAN,
  
  LNG_GREEK = LG_GREEK,
  LNG_ROMANIAN = LG_ROMANIAN,
  LNG_CROATIAN = LG_CROATIAN,
  LNG_SLOVENIAN = LG_SLOVENIAN,
  LNG_SERBIAN_LATIN = LG_SERBIAN_LATIN,
  LNG_BELARUSIAN = LG_BELARUSIAN,
  LNG_TAJIK = LG_TAJIK,
  LNG_MACEDONIAN = LG_MACEDONIAN,
  LNG_KAZAK = LG_KAZAK,
  LNG_KYRGYZ = LG_KYRGYZ,
  LNG_TURKMEN = LG_TURKMEN,
  LNG_TATAR = LG_TATAR,
  LNG_MONGOLIAN = LG_MONGOLIAN,
  LNG_SERBIAN = LG_SERBIAN,
  LNG_CATALAN = LG_CATALAN,
  LNG_ICELANDIC = LG_ICELANDIC,
  LNG_NORWEGIAN = LG_NORWEGIAN,
  LNG_NORWEGIAN_NYNORSK = LG_NORWEGIAN_NYNORSK,
  LNG_ALBANIAN = LG_ALBANIAN,
  LNG_BASQUE = LG_BASQUE,
  LNG_MALTESE = LG_MALTESE,
  LNG_MALAY = LG_MALAY,
  LNG_LUXEMBOURGISH = LG_LUXEMBOURGISH,
  LNG_GREENLANDIC = LG_GREENLANDIC,
  LNG_IRISH = LG_IRISH,
  LNG_TURKISH = LG_TURKISH,
  LNG_HEBREW = LG_HEBREW,
  LNG_ARABIC = LG_ARABIC,
  LNG_PERSIAN = LG_PERSIAN,
  LNG_ESTONIAN = LG_ESTONIAN,
  LNG_LATVIAN = LG_LATVIAN,
  LNG_LITHUANIAN = LG_LITHUANIAN,
  LNG_VIETNAMESE = LG_VIETNAMESE,
  
  LNG_ENGLISH      = LNG_ENGLISH_US,
  LNG_OTHER        = LG_OTHER,
  LNG_EURO         = LG_EURO,
  LNG_HIER_UTF     = LG_HIER_UTF,
  LNG_CYRILLIC     = LG_CYRILLIC,

  LNG_ALL          = -1,
  LNG_DEFAULT      = -2
};

/* Codepages - standard */
#define CP_UNKNOWN    0
#define CP_INVALID    0xFFFF
#define CP_ASCII      20127
#define CP_UNICODE    1200 
#define CP_LATIN2     1250 /*Central Europe*/
#define CP_CYRILLIC   1251
#define CP_LATIN1     1252
#define CP_GREEK      1253
#define CP_TURKISH    1254
#define CP_HEBREW     1255
#define CP_ARABIC     1256
#define CP_BALTIC     1257
#define CP_VIETNAMESE 1258

/* Codepages - cyrillic nonstandard */
#define CP_CYRILLIC_MAC 10007
#define CP_OEM_RUSSIAN    866
#define CP_KOI8R        20866
#define CP_KOI8U        21866
#define CP_ISO8859_5    28595

/* Codepages - Unicode nonstandard */
#define CP_EUC_JP       20932
#define CP_UTF7         65000
#define CP_UTF8         65001
#define CP_HIER_UTF    0xFFFE

#define CP_UTF16         1200  /* little endian */
#define CP_UTF32         12000 /* little endian */
 
#define CP_UCS2          1299  /* little endian; two byte subset of utf-16; covers Basic Multilingual Plane */

enum CodePages
{
    CPG_UNKNOWN = CP_UNKNOWN,
    CPG_INVALID = CP_INVALID,

    CPG_ASCII = CP_ASCII,
    CPG_UNICODE = CP_UNICODE, 
    CPG_LATIN2 = CP_LATIN2, /*Central Europe*/
    CPG_CYRILLIC = CP_CYRILLIC,
    CPG_LATIN1 = CP_LATIN1,
    CPG_GREEK = CP_GREEK,
    CPG_TURKISH = CP_TURKISH,
    CPG_HEBREW = CP_HEBREW,
    CPG_ARABIC = CP_ARABIC,
    CPG_BALTIC = CP_BALTIC,
    CPG_VIETNAMESE = CP_VIETNAMESE,

    /* Codepages - cyrillic nonstandard */
    CPG_CYRILLIC_MAC = CP_CYRILLIC_MAC,
    CPG_OEM_RUSSIAN = CP_OEM_RUSSIAN,
    CPG_KOI8R = CP_KOI8R,
    CPG_KOI8U = CP_KOI8U,
    CPG_ISO8859_5 = CP_ISO8859_5,

    /* Special codepage for handling unicode as single-byte encoding */
    CPG_HIER_UTF = CP_HIER_UTF,

    /* Codepages - Unicode nonstandard */
    CPG_EUC_JP = CP_EUC_JP,
    CPG_UTF7 = CP_UTF7,
    CPG_UTF8 = CP_UTF8,

    CPG_UTF16 = CP_UTF16,
    CPG_UTF32 = CP_UTF32,
    CPG_UCS2 = CP_UCS2
};

#endif /*__LANGUAGE_H__*/
