//=========================================================================
// Win->Unicode and Unicode->Win recode table
//=========================================================================
#include <_include/Cp2Uni.h>

//-------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-------------------------------------------------------------------------

const uint16_t tabWin2Usc[256] =
    {
        0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 
        0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F, 
        0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 
        0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 
        0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 
        0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 
        0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 
        0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F, 
        0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021, 0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F, 
        0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x0098, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F, 
        0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7, 0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407, 
        0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7, 0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457, 
        0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 
        0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F, 
        0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F, 
        0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
    };

//-------------------------------------------------------------------------

const char tabUcs2Win_DefPg[256] =
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

const char tabUcs2Win_00Pg[256] =
    {
        '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', 
        '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', 
        '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F', 
        '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F', 
        '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4A', '\x4B', '\x4C', '\x4D', '\x4E', '\x4F', 
        '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F', 
        '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F', 
        '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x95', '\x96', '\x97', '\x98', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\xA0', '\x3F', '\x3F', '\x3F', '\xA4', '\x3F', '\xA6', '\xA7', '\x3F', '\xA9', '\x3F', '\xAB', '\xAC', '\xAD', '\xAE', '\x3F', 
        '\xB0', '\xB1', '\x3F', '\x3F', '\x3F', '\xB5', '\xB6', '\xB7', '\x3F', '\x3F', '\x3F', '\xBB', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x41', '\x41', '\x41', '\x41', '\x41', '\x41', '\x3F', '\x43', '\x45', '\x45', '\x45', '\x45', '\x49', '\x49', '\x49', '\x49', 
        '\x3F', '\x4E', '\x4F', '\x4F', '\x4F', '\x4F', '\x4F', '\x3F', '\x4F', '\x55', '\x55', '\x55', '\x55', '\x59', '\x3F', '\x3F', 
        '\x61', '\x61', '\x61', '\x61', '\x61', '\x61', '\x3F', '\x63', '\x65', '\x65', '\x65', '\x65', '\x69', '\x69', '\x69', '\x69', 
        '\x3F', '\x6E', '\x6F', '\x6F', '\x6F', '\x6F', '\x6F', '\x3F', '\x6F', '\x75', '\x75', '\x75', '\x75', '\x79', '\x3F', '\x79'
    };

extern const char tabUcs2Win_04Pg[256];

const char tabUcs2Win_04Pg[256] =
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
    };

const char tabUcs2Win_20Pg[256] =
    {
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x96', '\x97', '\x3F', '\x3F', '\x3F', '\x91', '\x92', '\x82', '\x3F', '\x93', '\x94', '\x84', '\x3F', 
        '\x86', '\x87', '\x95', '\x3F', '\x3F', '\x3F', '\x85', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x89', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x8B', '\x9B', '\x3F', '\x21', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x88', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F'
    };

const char tabUcs2Win_21Pg[256] =
    {
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\xB9', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x99', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3C', '\x5E', '\x3E', '\x76', '\x2D', '\xA6', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\xA6', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', 
        '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F'
    };

//-------------------------------------------------------------------------

const char* const tabUsc2Win[256] =
    {
        tabUcs2Win_00Pg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_04Pg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_20Pg, tabUcs2Win_21Pg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, 
        tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg, tabUcs2Win_DefPg
    };



//-------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//=========================================================================
