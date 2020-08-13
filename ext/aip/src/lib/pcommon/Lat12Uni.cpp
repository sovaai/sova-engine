//=========================================================================
// Lat1->Unicode and Unicode->Lat1 recode table
//=========================================================================
#include <_include/Cp2Uni.h>

//-------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-------------------------------------------------------------------------
extern const char tabUcs2Lat1_DefPg[256];
extern const char tabUcs2Lat1_00Pg[256];
extern const char tabUcs2Lat1_01Pg[256];
extern const char tabUcs2Lat1_02Pg[256];
extern const char tabUcs2Lat1_20Pg[256];
extern const char tabUcs2Lat1_21Pg[256];

const uint16_t tabLat12Usc[256] =
    {
        0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F, 
        0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 
        0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 
        0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 
        0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 
        0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 
        0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F, 
        0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F, 
        0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178, 
        0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 
        0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 
        0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 
        0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, 
        0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 
        0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
    };

//-------------------------------------------------------------------------

const char tabUcs2Lat1_DefPg[256] =
    {
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 
        '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?'
    };
//-------------------------------------------------------------------------

const char tabUcs2Lat1_00Pg[256] =
    {
        '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', 
        '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', 
        '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F', 
        '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F', 
        '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4A', '\x4B', '\x4C', '\x4D', '\x4E', '\x4F', 
        '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F', 
        '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F', 
        '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F', 
        '\x3F', '\x81', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x8D', '\x3F', '\x8F', 
        '\x90', '\x3F', '\x3F', '\x3F', '\x3F', '\x95', '\x96', '\x97', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x9D', '\x3F', '\x3F', 
        '\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF', 
        '\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF', 
        '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF', 
        '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF', 
        '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF', 
        '\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'
    };

const char tabUcs2Lat1_20Pg[256] =
    {
        '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x2D', '\x2D', '\x3F', '\x96', '\x97', '\x3F', '\x3F', '\x3D', '\x91', '\x92', '\x82', '\x3F', '\x93', '\x94', '\x84', '\x3F', 
        '\x86', '\x87', '\x95', '\x3F', '\xB7', '\x3F', '\x85', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x89', '\x3F', '\x27', '\x3F', '\x3F', '\x60', '\x3F', '\x3F', '\x3F', '\x8B', '\x9B', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x2F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\xB0', '\x3F', '\x3F', '\x3F', '\x34', '\x35', '\x36', '\x37', '\x38', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x6E', 
        '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\xA2', '\x3F', '\x3F', '\xA3', '\x3F', '\x3F', '\x50', '\x3F', '\x3F', '\x3F', '\x3F', '\x80', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F'
    };

const char tabUcs2Lat1_01Pg[256] =
    {
        '\x41', '\x61', '\x41', '\x61', '\x41', '\x61', '\x43', '\x63', '\x43', '\x63', '\x43', '\x63', '\x43', '\x63', '\x44', '\x64', 
        '\xD0', '\x64', '\x45', '\x65', '\x45', '\x65', '\x45', '\x65', '\x45', '\x65', '\x45', '\x65', '\x47', '\x67', '\x47', '\x67', 
        '\x47', '\x67', '\x47', '\x67', '\x48', '\x68', '\x48', '\x68', '\x49', '\x69', '\x49', '\x69', '\x49', '\x69', '\x49', '\x69', 
        '\x49', '\x69', '\x3F', '\x3F', '\x4A', '\x6A', '\x4B', '\x6B', '\x3F', '\x4C', '\x6C', '\x4C', '\x6C', '\x4C', '\x6C', '\x3F', 
        '\x3F', '\x4C', '\x6C', '\x4E', '\x6E', '\x4E', '\x6E', '\x4E', '\x6E', '\x3F', '\x3F', '\x3F', '\x4F', '\x6F', '\x4F', '\x6F', 
        '\x4F', '\x6F', '\x8C', '\x9C', '\x52', '\x72', '\x52', '\x72', '\x52', '\x72', '\x53', '\x73', '\x53', '\x73', '\x53', '\x73', 
        '\x8A', '\x9A', '\x54', '\x74', '\x54', '\x74', '\x54', '\x74', '\x55', '\x75', '\x55', '\x75', '\x55', '\x75', '\x55', '\x75', 
        '\x55', '\x75', '\x55', '\x75', '\x57', '\x77', '\x59', '\x79', '\x9F', '\x5A', '\x7A', '\x5A', '\x7A', '\x8E', '\x9E', '\x3F', 
        '\x62', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\xD0', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x83', '\x83', '\x3F', '\x3F', '\x3F', '\x3F', '\x49', '\x3F', '\x3F', '\x6C', '\x3F', '\x3F', '\x3F', '\x3F', '\x4F', 
        '\x4F', '\x6F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x74', '\x3F', '\x3F', '\x54', '\x55', 
        '\x75', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x7A', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x7C', '\x3F', '\x3F', '\x21', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x41', '\x61', '\x49', 
        '\x69', '\x4F', '\x6F', '\x55', '\x75', '\x55', '\x75', '\x55', '\x75', '\x55', '\x75', '\x55', '\x75', '\x3F', '\x41', '\x61', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x47', '\x67', '\x47', '\x67', '\x4B', '\x6B', '\x4F', '\x6F', '\x4F', '\x6F', '\x3F', '\x3F', 
        '\x6A', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F'
    };

const char tabUcs2Lat1_02Pg[256] =
    {
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x67', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x27', '\x22', '\x3F', '\x27', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x5E', '\x3F', '\x88', '\x3F', '\x27', '\xAF', '\xB4', '\x60', '\x3F', '\x5F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\xB0', '\x3F', '\x98', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F'
    };



const char tabUcs2Lat1_21Pg[256] =
    {
        '\x3F', '\x3F', '\x43', '\x3F', '\x3F', '\x3F', '\x3F', '\x45', '\x3F', '\x3F', '\x67', '\x48', '\x48', '\x48', '\x68', '\x3F', 
        '\x49', '\x49', '\x4C', '\x6C', '\x3F', '\x4E', '\xb9', '\x3F', '\x50', '\x50', '\x51', '\x52', '\x52', '\x52', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x99', '\x3F', '\x5A', '\x3F', '\x3F', '\x3F', '\x5A', '\x3F', '\x4B', '\xC5', '\x42', '\x43', '\x65', '\x65', 
        '\x45', '\x46', '\x3F', '\x4D', '\x6F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F'
    };

//-------------------------------------------------------------------------
extern const char tabUcs2Win_04Pg[256]; /* =
    {
        '\x3F', '\xA8', '\x80', '\x81', '\xAA', '\xBD', '\xB2', '\xAF', '\xA3', '\x8A', '\x8C', '\x8E', '\x8D', '\x3F', '\xA1', '\x8F', 
        '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF', 
        '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF', 
        '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF', 
        '\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF', 
        '\x3F', '\xB8', '\x90', '\x83', '\xBA', '\xBE', '\xB3', '\xBF', '\xBC', '\x9A', '\x9C', '\x9E', '\x9D', '\x3F', '\xA2', '\x9F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\xA5', '\xB4', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F'
    };*/


const char* const tabUsc2Lat1[256] =
    {
        tabUcs2Lat1_00Pg,  tabUcs2Lat1_01Pg,  tabUcs2Lat1_02Pg,  tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_20Pg,  tabUcs2Lat1_21Pg,  tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg
    };



//-------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//=========================================================================
