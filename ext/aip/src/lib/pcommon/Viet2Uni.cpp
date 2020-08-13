
//=========================================================================
// CP1258->Unicode and Unicode->CP1258 recode table
//=========================================================================
#include <_include/Cp2Uni.h>

//-------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-------------------------------------------------------------------------

const char tabUcs2Viet_DefPg[256] = {
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
    '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?' };

//-------------------------------------------------------------------------

const char tabUcs2Viet_0x00Pg[256] = {
    '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F',
    '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
    '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F',
    '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F',
    '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4A', '\x4B', '\x4C', '\x4D', '\x4E', '\x4F',
    '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F',
    '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
    '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F',
    '\x3F', '\x81', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x8A', '\x3F', '\x3F', '\x8D', '\x8E', '\x8F',
    '\x90', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x9A', '\x3F', '\x3F', '\x9D', '\x9E', '\x3F',
    '\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
    '\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
    '\xC0', '\xC1', '\xC2', '\x3F', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\x3F', '\xCD', '\xCE', '\xCF',
    '\x3F', '\xD1', '\x3F', '\xD3', '\xD4', '\x3F', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\x3F', '\x3F', '\xDF',
    '\xE0', '\xE1', '\xE2', '\x3F', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\x3F', '\xED', '\xEE', '\xEF',
    '\x3F', '\xF1', '\x3F', '\xF3', '\xF4', '\x3F', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\x3F', '\x3F', '\xFF' };

//-------------------------------------------------------------------------

const char tabUcs2Viet_0x20Pg[256] = {
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x96', '\x97', '\x3F', '\x3F', '\x3F', '\x91', '\x92', '\x82', '\x3F', '\x93', '\x94', '\x84', '\x3F',
    '\x86', '\x87', '\x95', '\x3F', '\x3F', '\x3F', '\x85', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x89', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x8B', '\x9B', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\xFE', '\x80', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F' };

//-------------------------------------------------------------------------

const char tabUcs2Viet_0x01Pg[256] = {
    '\x3F', '\x3F', '\xC3', '\xE3', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\xD0', '\xF0', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x8C', '\x9C', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x9F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x83', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\xD5', '\xF5', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\xDD',
    '\xFD', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F' };

//-------------------------------------------------------------------------

const char tabUcs2Viet_0x02Pg[256] = {
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
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x88', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x98', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F' };

//-------------------------------------------------------------------------

const char tabUcs2Viet_0x21Pg[256] = {
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x99', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
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
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F' };

//-------------------------------------------------------------------------

const char tabUcs2Viet_0x03Pg[256] = {
    '\xCC', '\xEC', '\x3F', '\xDE', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\xD2', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\xF2', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
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
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F',
    '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F', '\x3F' };

//-------------------------------------------------------------------------

const char * const tabUcs2Viet[256] = {
    tabUcs2Viet_0x00Pg, tabUcs2Viet_0x01Pg, tabUcs2Viet_0x02Pg, tabUcs2Viet_0x03Pg,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_0x20Pg, tabUcs2Viet_0x21Pg, tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg ,
    tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg , tabUcs2Viet_DefPg  };

//-------------------------------------------------------------------------

const uint16_t tabViet2Ucs[256] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x008A, 0x2039, 0x0152, 0x008D, 0x008E, 0x008F,
    0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122, 0x009A, 0x203A, 0x0153, 0x009D, 0x009E, 0x0178,
    0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
    0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
    0x00C0, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x0300, 0x00CD, 0x00CE, 0x00CF,
    0x0110, 0x00D1, 0x0309, 0x00D3, 0x00D4, 0x01A0, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x01AF, 0x0303, 0x00DF,
    0x00E0, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x0301, 0x00ED, 0x00EE, 0x00EF,
    0x0111, 0x00F1, 0x0323, 0x00F3, 0x00F4, 0x01A1, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x01B0, 0x20AB, 0x00FF };

//-------------------------------------------------------------------------

const uint16_t ucsVectorsData[240] = {
    0x0041, 0x0303, 0x0061, 0x0303, 0x0041, 0x0309, 0x0061, 0x0309, 0x0041, 0x0323, 0x0061, 0x0323, 0x0102, 0x0301, 0x0103, 0x0301,
    0x0102, 0x0300, 0x0103, 0x0300, 0x0102, 0x0303, 0x0103, 0x0303, 0x0102, 0x0309, 0x0103, 0x0309, 0x0102, 0x0323, 0x0103, 0x0323,
    0x00C2, 0x0301, 0x00E2, 0x0301, 0x00C2, 0x0300, 0x00E2, 0x0300, 0x00C2, 0x0303, 0x00E2, 0x0303, 0x00C2, 0x0309, 0x00E2, 0x0309,
    0x00C2, 0x0323, 0x00E2, 0x0323, 0x0045, 0x0303, 0x0065, 0x0303, 0x0045, 0x0309, 0x0065, 0x0309, 0x0045, 0x0323, 0x0065, 0x0323,
    0x00CA, 0x0301, 0x00EA, 0x0301, 0x00CA, 0x0300, 0x00EA, 0x0300, 0x00CA, 0x0303, 0x00EA, 0x0303, 0x00CA, 0x0309, 0x00EA, 0x0309,
    0x00CA, 0x0323, 0x00EA, 0x0323, 0x0049, 0x0303, 0x0069, 0x0303, 0x0049, 0x0309, 0x0069, 0x0309, 0x0049, 0x0323, 0x0069, 0x0323,
    0x004F, 0x0303, 0x006F, 0x0303, 0x004F, 0x0309, 0x006F, 0x0309, 0x004F, 0x0323, 0x006F, 0x0323, 0x00D4, 0x0301, 0x00F4, 0x0301,
    0x00D4, 0x0300, 0x00F4, 0x0300, 0x00D4, 0x0303, 0x00F4, 0x0303, 0x00D4, 0x0309, 0x00F4, 0x0309, 0x00D4, 0x0323, 0x00F4, 0x0323,
    0x01A0, 0x0301, 0x01A1, 0x0301, 0x01A0, 0x0300, 0x01A1, 0x0300, 0x01A0, 0x0303, 0x01A1, 0x0303, 0x01A0, 0x0309, 0x01A1, 0x0309,
    0x01A0, 0x0323, 0x01A1, 0x0323, 0x0055, 0x0303, 0x0075, 0x0303, 0x0055, 0x0309, 0x0075, 0x0309, 0x0055, 0x0323, 0x0075, 0x0323,
    0x01AF, 0x0301, 0x01B0, 0x0301, 0x01AF, 0x0300, 0x01B0, 0x0300, 0x01AF, 0x0303, 0x01B0, 0x0303, 0x01AF, 0x0309, 0x01B0, 0x0309,
    0x01AF, 0x0323, 0x01B0, 0x0323, 0x0059, 0x0300, 0x0079, 0x0300, 0x0059, 0x0303, 0x0079, 0x0303, 0x0059, 0x0309, 0x0079, 0x0309,
    0x0059, 0x0323, 0x0079, 0x0323, 0x0049, 0x0300, 0x0069, 0x0300, 0x004F, 0x0300, 0x006F, 0x0300, 0x0059, 0x0301, 0x0079, 0x0301,
    0x0041, 0x0300, 0x0061, 0x0300, 0x0041, 0x0301, 0x0061, 0x0301, 0x0045, 0x0300, 0x0065, 0x0300, 0x0045, 0x0301, 0x0065, 0x0301,
    0x0049, 0x0301, 0x0069, 0x0301, 0x004F, 0x0301, 0x006F, 0x0301, 0x0055, 0x0300, 0x0075, 0x0300, 0x0055, 0x0301, 0x0075, 0x0301 };

//-------------------------------------------------------------------------

const ucs_vec_t ucsVectors[120] = {
    { 2, ucsVectorsData + 0 }  , { 2, ucsVectorsData + 2 }  , { 2, ucsVectorsData + 4 }  , { 2, ucsVectorsData + 6 }  ,
    { 2, ucsVectorsData + 8 }  , { 2, ucsVectorsData + 10 } , { 2, ucsVectorsData + 12 } , { 2, ucsVectorsData + 14 } ,
    { 2, ucsVectorsData + 16 } , { 2, ucsVectorsData + 18 } , { 2, ucsVectorsData + 20 } , { 2, ucsVectorsData + 22 } ,
    { 2, ucsVectorsData + 24 } , { 2, ucsVectorsData + 26 } , { 2, ucsVectorsData + 28 } , { 2, ucsVectorsData + 30 } ,
    { 2, ucsVectorsData + 32 } , { 2, ucsVectorsData + 34 } , { 2, ucsVectorsData + 36 } , { 2, ucsVectorsData + 38 } ,
    { 2, ucsVectorsData + 40 } , { 2, ucsVectorsData + 42 } , { 2, ucsVectorsData + 44 } , { 2, ucsVectorsData + 46 } ,
    { 2, ucsVectorsData + 48 } , { 2, ucsVectorsData + 50 } , { 2, ucsVectorsData + 52 } , { 2, ucsVectorsData + 54 } ,
    { 2, ucsVectorsData + 56 } , { 2, ucsVectorsData + 58 } , { 2, ucsVectorsData + 60 } , { 2, ucsVectorsData + 62 } ,
    { 2, ucsVectorsData + 64 } , { 2, ucsVectorsData + 66 } , { 2, ucsVectorsData + 68 } , { 2, ucsVectorsData + 70 } ,
    { 2, ucsVectorsData + 72 } , { 2, ucsVectorsData + 74 } , { 2, ucsVectorsData + 76 } , { 2, ucsVectorsData + 78 } ,
    { 2, ucsVectorsData + 80 } , { 2, ucsVectorsData + 82 } , { 2, ucsVectorsData + 84 } , { 2, ucsVectorsData + 86 } ,
    { 2, ucsVectorsData + 88 } , { 2, ucsVectorsData + 90 } , { 2, ucsVectorsData + 92 } , { 2, ucsVectorsData + 94 } ,
    { 2, ucsVectorsData + 96 } , { 2, ucsVectorsData + 98 } , { 2, ucsVectorsData + 100 }, { 2, ucsVectorsData + 102 },
    { 2, ucsVectorsData + 104 }, { 2, ucsVectorsData + 106 }, { 2, ucsVectorsData + 108 }, { 2, ucsVectorsData + 110 },
    { 2, ucsVectorsData + 112 }, { 2, ucsVectorsData + 114 }, { 2, ucsVectorsData + 116 }, { 2, ucsVectorsData + 118 },
    { 2, ucsVectorsData + 120 }, { 2, ucsVectorsData + 122 }, { 2, ucsVectorsData + 124 }, { 2, ucsVectorsData + 126 },
    { 2, ucsVectorsData + 128 }, { 2, ucsVectorsData + 130 }, { 2, ucsVectorsData + 132 }, { 2, ucsVectorsData + 134 },
    { 2, ucsVectorsData + 136 }, { 2, ucsVectorsData + 138 }, { 2, ucsVectorsData + 140 }, { 2, ucsVectorsData + 142 },
    { 2, ucsVectorsData + 144 }, { 2, ucsVectorsData + 146 }, { 2, ucsVectorsData + 148 }, { 2, ucsVectorsData + 150 },
    { 2, ucsVectorsData + 152 }, { 2, ucsVectorsData + 154 }, { 2, ucsVectorsData + 156 }, { 2, ucsVectorsData + 158 },
    { 2, ucsVectorsData + 160 }, { 2, ucsVectorsData + 162 }, { 2, ucsVectorsData + 164 }, { 2, ucsVectorsData + 166 },
    { 2, ucsVectorsData + 168 }, { 2, ucsVectorsData + 170 }, { 2, ucsVectorsData + 172 }, { 2, ucsVectorsData + 174 },
    { 2, ucsVectorsData + 176 }, { 2, ucsVectorsData + 178 }, { 2, ucsVectorsData + 180 }, { 2, ucsVectorsData + 182 },
    { 2, ucsVectorsData + 184 }, { 2, ucsVectorsData + 186 }, { 2, ucsVectorsData + 188 }, { 2, ucsVectorsData + 190 },
    { 2, ucsVectorsData + 192 }, { 2, ucsVectorsData + 194 }, { 2, ucsVectorsData + 196 }, { 2, ucsVectorsData + 198 },
    { 2, ucsVectorsData + 200 }, { 2, ucsVectorsData + 202 }, { 2, ucsVectorsData + 204 }, { 2, ucsVectorsData + 206 },
    { 2, ucsVectorsData + 208 }, { 2, ucsVectorsData + 210 }, { 2, ucsVectorsData + 212 }, { 2, ucsVectorsData + 214 },
    { 2, ucsVectorsData + 216 }, { 2, ucsVectorsData + 218 }, { 2, ucsVectorsData + 220 }, { 2, ucsVectorsData + 222 },
    { 2, ucsVectorsData + 224 }, { 2, ucsVectorsData + 226 }, { 2, ucsVectorsData + 228 }, { 2, ucsVectorsData + 230 },
    { 2, ucsVectorsData + 232 }, { 2, ucsVectorsData + 234 }, { 2, ucsVectorsData + 236 }, { 2, ucsVectorsData + 238 } };

//-------------------------------------------------------------------------

const ucs_vec_t * tabUcsVecViet_0x00Pg[256] = {
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    NULL            , NULL            , NULL            , NULL            ,
    ucsVectors + 104, ucsVectors + 106, NULL            , ucsVectors + 0  ,
    NULL            , NULL            , NULL            , NULL            ,
    ucsVectors + 108, ucsVectors + 110, NULL            , NULL            ,
    ucsVectors + 98 , ucsVectors + 112, NULL            , NULL            ,
    NULL            , NULL            , ucsVectors + 100, ucsVectors + 114,
    NULL            , ucsVectors + 48 , NULL            , NULL            ,
    NULL            , ucsVectors + 116, ucsVectors + 118, NULL            ,
    NULL            , ucsVectors + 102, NULL            , NULL            ,
    ucsVectors + 105, ucsVectors + 107, NULL            , ucsVectors + 1  ,
    NULL            , NULL            , NULL            , NULL            ,
    ucsVectors + 109, ucsVectors + 111, NULL            , NULL            ,
    ucsVectors + 99 , ucsVectors + 113, NULL            , NULL            ,
    NULL            , NULL            , ucsVectors + 101, ucsVectors + 115,
    NULL            , ucsVectors + 49 , NULL            , NULL            ,
    NULL            , ucsVectors + 117, ucsVectors + 119, NULL            ,
    NULL            , ucsVectors + 103, NULL            , NULL             };

//-------------------------------------------------------------------------

const ucs_vec_t * tabUcsVecViet_0x1EPg[256] = {
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    ucsVectors + 4 , ucsVectors + 5 , ucsVectors + 2 , ucsVectors + 3 ,
    ucsVectors + 16, ucsVectors + 17, ucsVectors + 18, ucsVectors + 19,
    ucsVectors + 22, ucsVectors + 23, ucsVectors + 20, ucsVectors + 21,
    ucsVectors + 24, ucsVectors + 25, ucsVectors + 6 , ucsVectors + 7 ,
    ucsVectors + 8 , ucsVectors + 9 , ucsVectors + 12, ucsVectors + 13,
    ucsVectors + 10, ucsVectors + 11, ucsVectors + 14, ucsVectors + 15,
    ucsVectors + 30, ucsVectors + 31, ucsVectors + 28, ucsVectors + 29,
    ucsVectors + 26, ucsVectors + 27, ucsVectors + 32, ucsVectors + 33,
    ucsVectors + 34, ucsVectors + 35, ucsVectors + 38, ucsVectors + 39,
    ucsVectors + 36, ucsVectors + 37, ucsVectors + 40, ucsVectors + 41,
    ucsVectors + 44, ucsVectors + 45, ucsVectors + 46, ucsVectors + 47,
    ucsVectors + 52, ucsVectors + 53, ucsVectors + 50, ucsVectors + 51,
    ucsVectors + 54, ucsVectors + 55, ucsVectors + 56, ucsVectors + 57,
    ucsVectors + 60, ucsVectors + 61, ucsVectors + 58, ucsVectors + 59,
    ucsVectors + 62, ucsVectors + 63, ucsVectors + 64, ucsVectors + 65,
    ucsVectors + 66, ucsVectors + 67, ucsVectors + 70, ucsVectors + 71,
    ucsVectors + 68, ucsVectors + 69, ucsVectors + 72, ucsVectors + 73,
    ucsVectors + 78, ucsVectors + 79, ucsVectors + 76, ucsVectors + 77,
    ucsVectors + 80, ucsVectors + 81, ucsVectors + 82, ucsVectors + 83,
    ucsVectors + 86, ucsVectors + 87, ucsVectors + 84, ucsVectors + 85,
    ucsVectors + 88, ucsVectors + 89, ucsVectors + 90, ucsVectors + 91,
    ucsVectors + 96, ucsVectors + 97, ucsVectors + 94, ucsVectors + 95,
    ucsVectors + 92, ucsVectors + 93, NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL            };

//-------------------------------------------------------------------------

const ucs_vec_t * tabUcsVecViet_0x01Pg[256] = {
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    ucsVectors + 42, ucsVectors + 43, NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    ucsVectors + 74, ucsVectors + 75, NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL           ,
    NULL           , NULL           , NULL           , NULL            };

//-------------------------------------------------------------------------

const ucs_vec_t * const * const tabUcsVecViet[256] = {
    tabUcsVecViet_0x00Pg, tabUcsVecViet_0x01Pg, NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , tabUcsVecViet_0x1EPg, NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                ,
    NULL                , NULL                , NULL                , NULL                 };

//-------------------------------------------------------------------------

uint16_t cp2UcCollapsingSeqViet( const unsigned char *chars, int charsCount, int *charsProcessed )
{
    uint16_t ch     = tabViet2Ucs[ chars[0] ];
    *charsProcessed = 1;
    
    for ( int i = 1; i < charsCount; i++, (*charsProcessed)++ )
    {
        uint16_t tone = tabViet2Ucs[ chars[i] ];
        
        switch ( tone )
        {
        case 0x0303:
            switch ( ch )
            {
            case 0x0079: ch = 0x1EF9; continue;
            case 0x0059: ch = 0x1EF8; continue;
            case 0x01B0: ch = 0x1EEF; continue;
            case 0x01AF: ch = 0x1EEE; continue;
            case 0x0075: ch = 0x0169; continue;
            case 0x0055: ch = 0x0168; continue;
            case 0x01A1: ch = 0x1EE1; continue;
            case 0x01A0: ch = 0x1EE0; continue;
            case 0x00F4: ch = 0x1ED7; continue;
            case 0x00D4: ch = 0x1ED6; continue;
            case 0x006F: ch = 0x00F5; continue;
            case 0x004F: ch = 0x00D5; continue;
            case 0x0069: ch = 0x0129; continue;
            case 0x0049: ch = 0x0128; continue;
            case 0x00EA: ch = 0x1EC5; continue;
            case 0x00CA: ch = 0x1EC4; continue;
            case 0x0065: ch = 0x1EBD; continue;
            case 0x0045: ch = 0x1EBC; continue;
            case 0x00E2: ch = 0x1EAB; continue;
            case 0x00C2: ch = 0x1EAA; continue;
            case 0x0103: ch = 0x1EB5; continue;
            case 0x0102: ch = 0x1EB4; continue;
            case 0x0061: ch = 0x00E3; continue;
            case 0x0041: ch = 0x00C3; continue;

            }
        case 0x0309:
            switch ( ch )
            {
            case 0x0079: ch = 0x1EF7; continue;
            case 0x0059: ch = 0x1EF6; continue;
            case 0x01B0: ch = 0x1EED; continue;
            case 0x01AF: ch = 0x1EEC; continue;
            case 0x0075: ch = 0x1EE7; continue;
            case 0x0055: ch = 0x1EE6; continue;
            case 0x01A1: ch = 0x1EDF; continue;
            case 0x01A0: ch = 0x1EDE; continue;
            case 0x00F4: ch = 0x1ED5; continue;
            case 0x00D4: ch = 0x1ED4; continue;
            case 0x006F: ch = 0x1ECF; continue;
            case 0x004F: ch = 0x1ECE; continue;
            case 0x0069: ch = 0x1EC9; continue;
            case 0x0049: ch = 0x1EC8; continue;
            case 0x00EA: ch = 0x1EC3; continue;
            case 0x00CA: ch = 0x1EC2; continue;
            case 0x0065: ch = 0x1EBB; continue;
            case 0x0045: ch = 0x1EBA; continue;
            case 0x00E2: ch = 0x1EA9; continue;
            case 0x00C2: ch = 0x1EA8; continue;
            case 0x0103: ch = 0x1EB3; continue;
            case 0x0102: ch = 0x1EB2; continue;
            case 0x0061: ch = 0x1EA3; continue;
            case 0x0041: ch = 0x1EA2; continue;

            }
        case 0x0323:
            switch ( ch )
            {
            case 0x0079: ch = 0x1EF5; continue;
            case 0x0059: ch = 0x1EF4; continue;
            case 0x01B0: ch = 0x1EF1; continue;
            case 0x01AF: ch = 0x1EF0; continue;
            case 0x0075: ch = 0x1EE5; continue;
            case 0x0055: ch = 0x1EE4; continue;
            case 0x01A1: ch = 0x1EE3; continue;
            case 0x01A0: ch = 0x1EE2; continue;
            case 0x00F4: ch = 0x1ED9; continue;
            case 0x00D4: ch = 0x1ED8; continue;
            case 0x006F: ch = 0x1ECD; continue;
            case 0x004F: ch = 0x1ECC; continue;
            case 0x0069: ch = 0x1ECB; continue;
            case 0x0049: ch = 0x1ECA; continue;
            case 0x00EA: ch = 0x1EC7; continue;
            case 0x00CA: ch = 0x1EC6; continue;
            case 0x0065: ch = 0x1EB9; continue;
            case 0x0045: ch = 0x1EB8; continue;
            case 0x00E2: ch = 0x1EAD; continue;
            case 0x00C2: ch = 0x1EAC; continue;
            case 0x0103: ch = 0x1EB7; continue;
            case 0x0102: ch = 0x1EB6; continue;
            case 0x0061: ch = 0x1EA1; continue;
            case 0x0041: ch = 0x1EA0; continue;

            }
        case 0x0306:
            switch ( ch )
            {
            case 0x1EA1: ch = 0x1EB7; continue;
            case 0x1EA0: ch = 0x1EB6; continue;
            case 0x0061: ch = 0x0103; continue;
            case 0x0041: ch = 0x0102; continue;

            }
        case 0x0301:
            switch ( ch )
            {
            case 0x0075: ch = 0x00FA; continue;
            case 0x0055: ch = 0x00DA; continue;
            case 0x006F: ch = 0x00F3; continue;
            case 0x004F: ch = 0x00D3; continue;
            case 0x0069: ch = 0x00ED; continue;
            case 0x0049: ch = 0x00CD; continue;
            case 0x0065: ch = 0x00E9; continue;
            case 0x0045: ch = 0x00C9; continue;
            case 0x0061: ch = 0x00E1; continue;
            case 0x0041: ch = 0x00C1; continue;
            case 0x0079: ch = 0x00FD; continue;
            case 0x0059: ch = 0x00DD; continue;
            case 0x01B0: ch = 0x1EE9; continue;
            case 0x01AF: ch = 0x1EE8; continue;
            case 0x01A1: ch = 0x1EDB; continue;
            case 0x01A0: ch = 0x1EDA; continue;
            case 0x00F4: ch = 0x1ED1; continue;
            case 0x00D4: ch = 0x1ED0; continue;
            case 0x00EA: ch = 0x1EBF; continue;
            case 0x00CA: ch = 0x1EBE; continue;
            case 0x00E2: ch = 0x1EA5; continue;
            case 0x00C2: ch = 0x1EA4; continue;
            case 0x0103: ch = 0x1EAF; continue;
            case 0x0102: ch = 0x1EAE; continue;

            }
        case 0x0300:
            switch ( ch )
            {
            case 0x0075: ch = 0x00F9; continue;
            case 0x0055: ch = 0x00D9; continue;
            case 0x0065: ch = 0x00E8; continue;
            case 0x0045: ch = 0x00C8; continue;
            case 0x0061: ch = 0x00E0; continue;
            case 0x0041: ch = 0x00C0; continue;
            case 0x006F: ch = 0x00F2; continue;
            case 0x004F: ch = 0x00D2; continue;
            case 0x0069: ch = 0x00EC; continue;
            case 0x0049: ch = 0x00CC; continue;
            case 0x0079: ch = 0x1EF3; continue;
            case 0x0059: ch = 0x1EF2; continue;
            case 0x01B0: ch = 0x1EEB; continue;
            case 0x01AF: ch = 0x1EEA; continue;
            case 0x01A1: ch = 0x1EDD; continue;
            case 0x01A0: ch = 0x1EDC; continue;
            case 0x00F4: ch = 0x1ED3; continue;
            case 0x00D4: ch = 0x1ED2; continue;
            case 0x00EA: ch = 0x1EC1; continue;
            case 0x00CA: ch = 0x1EC0; continue;
            case 0x00E2: ch = 0x1EA7; continue;
            case 0x00C2: ch = 0x1EA6; continue;
            case 0x0103: ch = 0x1EB1; continue;
            case 0x0102: ch = 0x1EB0; continue;

            }
        case 0x0302:
            switch ( ch )
            {
            case 0x1ECD: ch = 0x1ED9; continue;
            case 0x1ECC: ch = 0x1ED8; continue;
            case 0x1EB9: ch = 0x1EC7; continue;
            case 0x1EB8: ch = 0x1EC6; continue;
            case 0x1EA1: ch = 0x1EAD; continue;
            case 0x1EA0: ch = 0x1EAC; continue;

            }
        case 0x031B:
            switch ( ch )
            {
            case 0x0075: ch = 0x01B0; continue;
            case 0x0055: ch = 0x01AF; continue;
            case 0x006F: ch = 0x01A1; continue;
            case 0x004F: ch = 0x01A0; continue;

            }
        }
        break;
    }
    
    return ch;
}

//-------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//=========================================================================
