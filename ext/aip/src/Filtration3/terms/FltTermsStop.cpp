/* Copyright (C) 2001-2003 Alexey P. Ivanov, MSU, Dept. of Phys., Moscow
 *
 * PROJECT: CONTENT FILTRATION
 * NAME:    Temporary Semantic Stop-Dict
 *
 * COMPILER: MS VC++ 6.0, GCC     TARGET: Win32,FreeBSD-x86,Linux-x86
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 15.10.01: v 1.00 created by AlexIV
 *==========================================================================
 */

#include "LingProc4/LexID.h"

#include "FltTermsStopEx.h"

const char *stopWordsUtf[] = {
    "\x00",                   // NULL
    "\x01",                   // START OF HEADING
    "\x02",                   // START OF TEXT
    "\x03",                   // END OF TEXT
    "\x04",                   // END OF TRANSMISSION
    "\x05",                   // ENQUIRY
    "\x06",                   // ACKNOWLEDGE
    "\x07",                   // BELL
    "\x08",                   // BACKSPACE
    "\x09",                   // HORIZONTAL TABULATION
    "\x0A",                   // LINE FEED
    "\x0B",                   // VERTICAL TABULATION
    "\x0C",                   // FORM FEED
    "\x0D",                   // CARRIAGE RETURN
    "\x0E",                   // SHIFT OUT
    "\x0F",                   // SHIFT IN
    "\x10",                   // DATA LINK ESCAPE
    "\x11",                   // DEVICE CONTROL ONE
    "\x12",                   // DEVICE CONTROL TWO
    "\x13",                   // DEVICE CONTROL THREE
    "\x14",                   // DEVICE CONTROL FOUR
    "\x15",                   // NEGATIVE ACKNOWLEDGE
    "\x16",                   // SYNCHRONOUS IDLE
    "\x17",                   // END OF TRANSMISSION BLOCK
    "\x18",                   // CANCEL
    "\x19",                   // END OF MEDIUM
    "\x1A",                   // SUBSTITUTE
    "\x1B",                   // ESCAPE
    "\x1C",                   // FILE SEPARATOR
    "\x1D",                   // GROUP SEPARATOR
    "\x1E",                   // RECORD SEPARATOR
    "\x1F",                   // UNIT SEPARATOR
    "\x20",                   // SPACE
    "\x22",                   // QUOTATION MARK
    "\x27",                   // APOSTROPHE
    "\x28",                   // LEFT PARENTHESIS
    "\x29",                   // RIGHT PARENTHESIS
    "\x2C",                   // COMMA
    "\x2D",                   // HYPHEN-MINUS
    "\x2E",                   // FULL STOP
    "\x2F",                   // SOLIDUS
    "\x3A",                   // COLON
    "\x3B",                   // SEMICOLON
    "\x3C",                   // LESS-THAN SIGN
    "\x3E",                   // GREATER-THAN SIGN
    "\x5B",                   // LEFT SQUARE BRACKET
    "\x5C",                   // REVERSE SOLIDUS
    "\x5D",                   // RIGHT SQUARE BRACKET
    "\x5F",                   // LOW LINE
    "\x7B",                   // LEFT CURLY BRACKET
    "\x7D",                   // RIGHT CURLY BRACKET
    "\x7F",                   // DELETE
    "\xE2\x80\x9A",           // SINGLE LOW-9 QUOTATION MARK
    "\xE2\x80\x9E",           // DOUBLE LOW-9 QUOTATION MARK
    "\xE2\x80\xA6",           // HORIZONTAL ELLIPSIS
    "\xE2\x80\xA0",           // DAGGER
    "\xE2\x80\xA1",           // DOUBLE DAGGER
    "\xE2\x80\xB9",           // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
    "\xE2\x80\x99",           // RIGHT SINGLE QUOTATION MARK
    "\xE2\x80\x9C",           // LEFT DOUBLE QUOTATION MARK
    "\xE2\x80\x9D",           // RIGHT DOUBLE QUOTATION MARK
    "\xE2\x80\xA2",           // BULLET
    "\xE2\x80\x93",           // EN DASH
    "\xE2\x80\x94",           // EM DASH
    "\xF0\xA0\x80\x82",       // UNDEFINED
    "\xE2\x84\xA2",           // TRADE MARK SIGN
    "\xE2\x80\xBA",           // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
    "\xC2\xA0",               // NO-BREAK SPACE
    "\xC2\xA6",               // BROKEN BAR
    "\xC2\xA7",               // SECTION SIGN
    "\xC2\xA9",               // COPYRIGHT SIGN
    "\xC2\xAB",               // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    "\xC2\xAC",               // NOT SIGN
    "\xC2\xAD",               // SOFT HYPHEN
    "\xC2\xAE",               // REGISTERED SIGN
    "\xC2\xB0",               // DEGREE SIGN
    "\xC2\xB1",               // PLUS-MINUS SIGN
    "\xC2\xB6",               // PILCROW SIGN
    "\xC2\xB7",               // MIDDLE DOT
    "\xE2\x84\x96",           // NUMERO SIGN
    "\xC2\xBB"                // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
};

FltTermsErrors TermsStopDict::init( LingProc &lp )
{
    return mapStopStatus( addWords( stopWordsUtf, sizeof( stopWordsUtf ) / sizeof( const char * ), lp ) );
}

