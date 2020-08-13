#ifndef _PROPS_H_
#define _PROPS_H_

const uint32_t char_props[477][2] = {
    /* C0 Controls and Basic Latin Range: 0000–007F */
    { 0x00000000, UCharSet::cntrl } /* Nul */,
    { 0x00000001, UCharSet::cntrl } /* Soh */,
    { 0x00000002, UCharSet::cntrl } /* Stx */,
    { 0x00000003, UCharSet::cntrl } /* Etx */,
    { 0x00000004, UCharSet::cntrl } /* Eot */,
    { 0x00000005, UCharSet::cntrl } /* Enq */,
    { 0x00000006, UCharSet::cntrl } /* Ack */,
    { 0x00000007, UCharSet::cntrl } /* Bel */,
    { 0x00000008, UCharSet::cntrl } /* Backspace */,
    { 0x00000009, UCharSet::space |
                  UCharSet::cntrl } /* Tab */,
    { 0x0000000A, UCharSet::space |
                  UCharSet::cntrl } /* Newline */,
    { 0x0000000B, UCharSet::space |
                  UCharSet::cntrl } /* Vt */,
    { 0x0000000C, UCharSet::space |
                  UCharSet::cntrl } /* Page */,
    { 0x0000000D, UCharSet::space |
                  UCharSet::cntrl } /* Return */,
    { 0x0000000E, UCharSet::cntrl } /* So */,
    { 0x0000000F, UCharSet::cntrl } /* Si */,
    { 0x00000010, UCharSet::cntrl } /* Dle */,
    { 0x00000011, UCharSet::cntrl } /* Dc1 */,
    { 0x00000012, UCharSet::cntrl } /* Dc2 */,
    { 0x00000013, UCharSet::cntrl } /* Dc3 */,
    { 0x00000014, UCharSet::cntrl } /* Dc4 */,
    { 0x00000015, UCharSet::cntrl } /* Nak */,
    { 0x00000016, UCharSet::cntrl } /* Syn */,
    { 0x00000017, UCharSet::cntrl } /* Etb */,
    { 0x00000018, UCharSet::cntrl } /* Can */,
    { 0x00000019, UCharSet::cntrl } /* Em */,
    { 0x0000001A, UCharSet::cntrl } /* Sub */,
    { 0x0000001B, UCharSet::cntrl } /* Esc */,
    { 0x0000001C, UCharSet::cntrl } /* Fs */,
    { 0x0000001D, UCharSet::cntrl } /* Gs */,
    { 0x0000001E, UCharSet::cntrl } /* Rs */,
    { 0x0000001F, UCharSet::cntrl } /* Us */,
    { 0x00000020, UCharSet::space |
                  UCharSet::blank } /* Space */,
    { 0x00000021, UCharSet::punct } /* EXCLAMATION_MARK */,
    { 0x00000022, UCharSet::punct } /* QUOTATION_MARK */,
    { 0x00000023, UCharSet::punct } /* NUMBER_SIGN */,
    { 0x00000024, UCharSet::symbol |
                  UCharSet::punct } /* DOLLAR_SIGN */,
    { 0x00000025, UCharSet::punct } /* PERCENT_SIGN */,
    { 0x00000026, UCharSet::punct } /* AMPERSAND */,
    { 0x00000027, UCharSet::punct |
                  UCharSet::wordpart } /* APOSTROPHE */,
    { 0x00000028, UCharSet::punct } /* LEFT_PARENTHESIS */,
    { 0x00000029, UCharSet::punct } /* RIGHT_PARENTHESIS */,
    { 0x0000002A, UCharSet::punct } /* ASTERISK */,
    { 0x0000002B, UCharSet::symbol |
                  UCharSet::punct } /* PLUS_SIGN */,
    { 0x0000002C, UCharSet::punct } /* COMMA */,
    { 0x0000002D, UCharSet::hyphen |
                  UCharSet::punct } /* HYPHEN-MINUS */,
    { 0x0000002E, UCharSet::punct } /* FULL_STOP */,
    { 0x0000002F, UCharSet::punct } /* SOLIDUS */,
    { 0x00000030, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_ZERO */,
    { 0x00000031, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_ONE */,
    { 0x00000032, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_TWO */,
    { 0x00000033, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_THREE */,
    { 0x00000034, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_FOUR */,
    { 0x00000035, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_FIVE */,
    { 0x00000036, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_SIX */,
    { 0x00000037, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_SEVEN */,
    { 0x00000038, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_EIGHT */,
    { 0x00000039, UCharSet::wordpart |
                  UCharSet::digit |
                  UCharSet::xdigit } /* DIGIT_NINE */,
    { 0x0000003A, UCharSet::punct } /* COLON */,
    { 0x0000003B, UCharSet::punct } /* SEMICOLON */,
    { 0x0000003C, UCharSet::symbol |
                  UCharSet::punct } /* LESS-THAN_SIGN */,
    { 0x0000003D, UCharSet::symbol |
                  UCharSet::punct } /* EQUALS_SIGN */,
    { 0x0000003E, UCharSet::symbol |
                  UCharSet::punct } /* GREATER-THAN_SIGN */,
    { 0x0000003F, UCharSet::punct } /* QUESTION_MARK */,
    { 0x00000040, UCharSet::punct } /* COMMERCIAL_AT */,
    { 0x00000041, UCharSet::wordpart |
                  UCharSet::upper |
                  UCharSet::xdigit } /* LATIN_CAPITAL_LETTER_A */,
    { 0x00000042, UCharSet::wordpart |
                  UCharSet::upper |
                  UCharSet::xdigit } /* LATIN_CAPITAL_LETTER_B */,
    { 0x00000043, UCharSet::wordpart |
                  UCharSet::upper |
                  UCharSet::xdigit } /* LATIN_CAPITAL_LETTER_C */,
    { 0x00000044, UCharSet::wordpart |
                  UCharSet::upper |
                  UCharSet::xdigit } /* LATIN_CAPITAL_LETTER_D */,
    { 0x00000045, UCharSet::wordpart |
                  UCharSet::upper |
                  UCharSet::xdigit } /* LATIN_CAPITAL_LETTER_E */,
    { 0x00000046, UCharSet::wordpart |
                  UCharSet::upper |
                  UCharSet::xdigit } /* LATIN_CAPITAL_LETTER_F */,
    { 0x00000047, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_G */,
    { 0x00000048, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_H */,
    { 0x00000049, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_I */,
    { 0x0000004A, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_J */,
    { 0x0000004B, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_K */,
    { 0x0000004C, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_L */,
    { 0x0000004D, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_M */,
    { 0x0000004E, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_N */,
    { 0x0000004F, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_O */,
    { 0x00000050, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_P */,
    { 0x00000051, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_Q */,
    { 0x00000052, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_R */,
    { 0x00000053, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_S */,
    { 0x00000054, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_T */,
    { 0x00000055, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_U */,
    { 0x00000056, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_V */,
    { 0x00000057, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_W */,
    { 0x00000058, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_X */,
    { 0x00000059, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_Y */,
    { 0x0000005A, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_Z */,
    { 0x0000005B, UCharSet::punct } /* LEFT_SQUARE_BRACKET */,
    { 0x0000005C, UCharSet::punct } /* REVERSE_SOLIDUS */,
    { 0x0000005D, UCharSet::punct } /* RIGHT_SQUARE_BRACKET */,
    { 0x0000005E, UCharSet::symbol |
                  UCharSet::punct } /* CIRCUMFLEX_ACCENT */,
    { 0x0000005F, UCharSet::punct } /* LOW_LINE */,
    { 0x00000060, UCharSet::symbol |
                  UCharSet::punct } /* GRAVE_ACCENT */,
    { 0x00000061, UCharSet::wordpart |
                  UCharSet::lower |
                  UCharSet::xdigit } /* LATIN_SMALL_LETTER_A */,
    { 0x00000062, UCharSet::wordpart |
                  UCharSet::lower |
                  UCharSet::xdigit } /* LATIN_SMALL_LETTER_B */,
    { 0x00000063, UCharSet::wordpart |
                  UCharSet::lower |
                  UCharSet::xdigit } /* LATIN_SMALL_LETTER_C */,
    { 0x00000064, UCharSet::wordpart |
                  UCharSet::lower |
                  UCharSet::xdigit } /* LATIN_SMALL_LETTER_D */,
    { 0x00000065, UCharSet::wordpart |
                  UCharSet::lower |
                  UCharSet::xdigit } /* LATIN_SMALL_LETTER_E */,
    { 0x00000066, UCharSet::wordpart |
                  UCharSet::lower |
                  UCharSet::xdigit } /* LATIN_SMALL_LETTER_F */,
    { 0x00000067, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_G */,
    { 0x00000068, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_H */,
    { 0x00000069, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_I */,
    { 0x0000006A, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_J */,
    { 0x0000006B, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_K */,
    { 0x0000006C, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_L */,
    { 0x0000006D, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_M */,
    { 0x0000006E, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_N */,
    { 0x0000006F, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_O */,
    { 0x00000070, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_P */,
    { 0x00000071, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_Q */,
    { 0x00000072, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_R */,
    { 0x00000073, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_S */,
    { 0x00000074, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_T */,
    { 0x00000075, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_U */,
    { 0x00000076, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_V */,
    { 0x00000077, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_W */,
    { 0x00000078, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_X */,
    { 0x00000079, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_Y */,
    { 0x0000007A, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_Z */,
    { 0x0000007B, UCharSet::punct } /* LEFT_CURLY_BRACKET */,
    { 0x0000007C, UCharSet::symbol |
                  UCharSet::punct } /* VERTICAL_LINE */,
    { 0x0000007D, UCharSet::punct } /* RIGHT_CURLY_BRACKET */,
    { 0x0000007E, UCharSet::symbol |
                  UCharSet::punct } /* TILDE */,
    { 0x0000007F, UCharSet::cntrl } /* Rubout */,
    /* C1 Controls and Latin-1 Supplement Range: 0080–00FF */
    { 0x000000A0, UCharSet::space |
                  UCharSet::blank } /* NO-BREAK_SPACE */,
    { 0x000000A1, UCharSet::punct } /* INVERTED_EXCLAMATION_MARK */,
    { 0x000000A2, UCharSet::symbol |
                  UCharSet::punct } /* CENT_SIGN */,
    { 0x000000A3, UCharSet::symbol |
                  UCharSet::punct } /* POUND_SIGN */,
    { 0x000000A4, UCharSet::symbol |
                  UCharSet::punct } /* CURRENCY_SIGN */,
    { 0x000000A5, UCharSet::symbol |
                  UCharSet::punct } /* YEN_SIGN */,
    { 0x000000A6, UCharSet::symbol |
                  UCharSet::punct } /* BROKEN_BAR */,
    { 0x000000A7, UCharSet::symbol |
                  UCharSet::punct } /* SECTION_SIGN */,
    { 0x000000A8, UCharSet::punct } /* DIAERESIS */,
    { 0x000000A9, UCharSet::symbol |
                  UCharSet::punct } /* COPYRIGHT_SIGN */,
    { 0x000000AA, UCharSet::symbol |
                  UCharSet::punct } /* FEMININE_ORDINAL_INDICATOR */,
    { 0x000000AB, UCharSet::punct } /* LEFT-POINTING_DOUBLE_ANGLE_QUOTATION_MARK */,
    { 0x000000AC, UCharSet::symbol |
                  UCharSet::punct } /* NOT_SIGN */,
    { 0x000000AD, UCharSet::ignore |
                  UCharSet::wordpart |
                  UCharSet::hyphen |
                  UCharSet::punct } /* SOFT_HYPHEN */,
    { 0x000000AE, UCharSet::symbol |
                  UCharSet::punct } /* REGISTERED_SIGN */,
    { 0x000000AF, UCharSet::punct } /* MACRON */,
    { 0x000000B0, UCharSet::symbol |
                  UCharSet::punct } /* DEGREE_SIGN */,
    { 0x000000B1, UCharSet::symbol |
                  UCharSet::punct } /* PLUS-MINUS_SIGN */,
    { 0x000000B2, UCharSet::symbol } /* SUPERSCRIPT_TWO */,
    { 0x000000B3, UCharSet::symbol } /* SUPERSCRIPT_THREE */,
    { 0x000000B4, UCharSet::symbol } /* ACUTE_ACCENT */,
    { 0x000000B5, UCharSet::symbol |
                  UCharSet::punct } /* MICRO_SIGN */,
    { 0x000000B6, UCharSet::symbol |
                  UCharSet::punct } /* PILCROW_SIGN */,
    { 0x000000B7, UCharSet::symbol |
                  UCharSet::punct } /* MIDDLE_DOT */,
    { 0x000000B8, UCharSet::symbol |
                  UCharSet::punct } /* CEDILLA */,
    { 0x000000B9, UCharSet::symbol } /* SUPERSCRIPT_ONE */,
    { 0x000000BA, UCharSet::symbol |
                  UCharSet::punct } /* MASCULINE_ORDINAL_INDICATOR */,
    { 0x000000BB, UCharSet::punct } /* RIGHT-POINTING_DOUBLE_ANGLE_QUOTATION_MARK */,
    { 0x000000BC, UCharSet::symbol } /* VULGAR_FRACTION_ONE_QUARTER */,
    { 0x000000BD, UCharSet::symbol } /* VULGAR_FRACTION_ONE_HALF */,
    { 0x000000BE, UCharSet::symbol } /* VULGAR_FRACTION_THREE_QUARTERS */,
    { 0x000000BF, UCharSet::punct } /* INVERTED_QUESTION_MARK */,
    { 0x000000C0, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_A_WITH_GRAVE */,
    { 0x000000C1, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_A_WITH_ACUTE */,
    { 0x000000C2, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_A_WITH_CIRCUMFLEX */,
    { 0x000000C3, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_A_WITH_TILDE */,
    { 0x000000C4, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_A_WITH_DIAERESIS */,
    { 0x000000C5, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_A_WITH_RING_ABOVE */,
    { 0x000000C6, UCharSet::wordpart |
                  UCharSet::upper |
                  UCharSet::ligature } /* LATIN_CAPITAL_LETTER_AE */,
    { 0x000000C7, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_C_WITH_CEDILLA */,
    { 0x000000C8, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_E_WITH_GRAVE */,
    { 0x000000C9, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_E_WITH_ACUTE */,
    { 0x000000CA, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_E_WITH_CIRCUMFLEX */,
    { 0x000000CB, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_E_WITH_DIAERESIS */,
    { 0x000000CC, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_I_WITH_GRAVE */,
    { 0x000000CD, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_I_WITH_ACUTE */,
    { 0x000000CE, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_I_WITH_CIRCUMFLEX */,
    { 0x000000CF, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_I_WITH_DIAERESIS */,
    { 0x000000D0, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_ETH */,
    { 0x000000D1, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_N_WITH_TILDE */,
    { 0x000000D2, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_O_WITH_GRAVE */,
    { 0x000000D3, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_O_WITH_ACUTE */,
    { 0x000000D4, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_O_WITH_CIRCUMFLEX */,
    { 0x000000D5, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_O_WITH_TILDE */,
    { 0x000000D6, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_O_WITH_DIAERESIS */,
    { 0x000000D7, UCharSet::symbol |
                  UCharSet::punct } /* MULTIPLICATION_SIGN */,
    { 0x000000D8, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_O_WITH_STROKE */,
    { 0x000000D9, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_U_WITH_GRAVE */,
    { 0x000000DA, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_U_WITH_ACUTE */,
    { 0x000000DB, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_U_WITH_CIRCUMFLEX */,
    { 0x000000DC, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_U_WITH_DIAERESIS */,
    { 0x000000DD, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_Y_WITH_ACUTE */,
    { 0x000000DE, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_THORN */,
    { 0x000000DF, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_SHARP_S */,
    { 0x000000E0, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_A_WITH_GRAVE */,
    { 0x000000E1, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_A_WITH_ACUTE */,
    { 0x000000E2, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_A_WITH_CIRCUMFLEX */,
    { 0x000000E3, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_A_WITH_TILDE */,
    { 0x000000E4, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_A_WITH_DIAERESIS */,
    { 0x000000E5, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_A_WITH_RING_ABOVE */,
    { 0x000000E6, UCharSet::wordpart |
                  UCharSet::lower |
                  UCharSet::ligature } /* LATIN_SMALL_LETTER_AE */,
    { 0x000000E7, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_C_WITH_CEDILLA */,
    { 0x000000E8, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_E_WITH_GRAVE */,
    { 0x000000E9, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_E_WITH_ACUTE */,
    { 0x000000EA, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_E_WITH_CIRCUMFLEX */,
    { 0x000000EB, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_E_WITH_DIAERESIS */,
    { 0x000000EC, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_I_WITH_GRAVE */,
    { 0x000000ED, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_I_WITH_ACUTE */,
    { 0x000000EE, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_I_WITH_CIRCUMFLEX */,
    { 0x000000EF, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_I_WITH_DIAERESIS */,
    { 0x000000F0, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_ETH */,
    { 0x000000F1, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_N_WITH_TILDE */,
    { 0x000000F2, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_O_WITH_GRAVE */,
    { 0x000000F3, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_O_WITH_ACUTE */,
    { 0x000000F4, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_O_WITH_CIRCUMFLEX */,
    { 0x000000F5, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_O_WITH_TILDE */,
    { 0x000000F6, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_O_WITH_DIAERESIS */,
    { 0x000000F7, UCharSet::symbol |
                  UCharSet::punct } /* DIVISION_SIGN */,
    { 0x000000F8, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_O_WITH_STROKE */,
    { 0x000000F9, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_U_WITH_GRAVE */,
    { 0x000000FA, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_U_WITH_ACUTE */,
    { 0x000000FB, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_U_WITH_CIRCUMFLEX */,
    { 0x000000FC, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_U_WITH_DIAERESIS */,
    { 0x000000FD, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_Y_WITH_ACUTE */,
    { 0x000000FE, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_THORN */,
    { 0x000000FF, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_Y_WITH_DIAERESIS */,
    /* Latin Extended-A Range: 0100–017F */
    { 0x00000100, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER A WITH MACRON */,
    { 0x00000101, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER A WITH MACRON */,
    { 0x00000102, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_A_WITH_BREVE */,
    { 0x00000103, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_A_WITH_BREVE */,
    { 0x00000104, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER A WITH OGONEK */,
    { 0x00000105, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER A WITH OGONEK */,
    { 0x00000106, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER C WITH ACUTE */,
    { 0x00000107, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN CAPITAL LETTER C WITH ACUTE */,
    { 0x00000108, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER C WITH CIRCUMFLEX */,
    { 0x00000109, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER C WITH CIRCUMFLEX */,
    { 0x0000010A, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER C WITH DOT ABOVE */,
    { 0x0000010B, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER C WITH DOT ABOVE */,
    { 0x0000010C, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER C WITH CARON */,
    { 0x0000010D, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER C WITH CARON */,
    { 0x0000010E, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER D WITH CARON */,
    { 0x0000010F, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER D WITH CARON */,
    { 0x00000110, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_D_WITH_STROKE */,
    { 0x00000111, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_D_WITH_STROKE */,
    { 0x00000112, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER E WITH MACRON */,
    { 0x00000113, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER E WITH MACRON */,
    { 0x00000114, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER E WITH BREVE */,
    { 0x00000115, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER E WITH BREVE */,
    { 0x00000116, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER E WITH DOT ABOVE */,
    { 0x00000117, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER E WITH DOT ABOVE */,
    { 0x00000118, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER E WITH OGONEK */,
    { 0x00000119, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER E WITH OGONEK */,
    { 0x0000011A, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER E WITH CARON */,
    { 0x0000011B, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER E WITH CARON */,
    { 0x0000011C, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER G WITH CIRCUMFLEX */,
    { 0x0000011D, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER G WITH CIRCUMFLEX */,
    { 0x0000011E, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER G WITH BREVE */,
    { 0x0000011F, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER G WITH BREVE */,
    { 0x00000120, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER G WITH DOT ABOVE */,
    { 0x00000121, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER G WITH DOT ABOVE */,
    { 0x00000122, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER G WITH CEDILLA */,
    { 0x00000123, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER G WITH CEDILLA */,
    { 0x00000124, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER H WITH CIRCUMFLEX */,
    { 0x00000125, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER H WITH CIRCUMFLEX */,
    { 0x00000126, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER H WITH STROKE */,
    { 0x00000127, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER H WITH STROKE */,
    { 0x00000128, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER I WITH TILDE */,
    { 0x00000129, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER I WITH TILDE */,
    { 0x0000012A, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER I WITH MACRON */,
    { 0x0000012B, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER I WITH MACRON */,
    { 0x0000012C, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER I WITH BREVE */,
    { 0x0000012D, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER I WITH BREVE */,
    { 0x0000012E, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER I WITH OGONEK */,
    { 0x0000012F, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER I WITH OGONEK */,
    { 0x00000130, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_I_WITH_DOT_ABOVE */,
    { 0x00000131, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_DOTLESS_I */,
    { 0x00000132, UCharSet::wordpart |
                  UCharSet::upper |
                  UCharSet::ligature } /* LATIN CAPITAL LIGATURE IJ */,
    { 0x00000133, UCharSet::wordpart |
                  UCharSet::lower |
                  UCharSet::ligature } /* LATIN CAPITAL LIGATURE IJ */,
    { 0x00000134, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER J WITH CIRCUMFLEX */,
    { 0x00000135, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER J WITH CIRCUMFLEX */,
    { 0x00000136, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER K WITH CEDILLA */,
    { 0x00000137, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER K WITH CEDILLA */,
    //{ 0x00000138, UCharSet::wordpart |
    //              UCharSet::lower } /* LATIN SMALL LETTER KRA */,
    { 0x00000139, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER L WITH ACUTE */,
    { 0x0000013A, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER L WITH ACUTE */,
    { 0x0000013B, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER L WITH CEDILLA */,
    { 0x0000013C, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER L WITH CEDILLA */,
    { 0x0000013D, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER L WITH CARON */,
    { 0x0000013E, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER L WITH CARON*/,
    { 0x0000013F, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER L WITH MIDDLE DOT */,
    { 0x00000140, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER L WITH MIDDLE DOT */,
    { 0x00000141, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER L WITH STROKE */,
    { 0x00000142, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER L WITH STROKE */,
    { 0x00000143, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER N WITH ACUTE */,
    { 0x00000144, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER N WITH ACUTE */,
    { 0x00000145, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER N WITH CEDILLA */,
    { 0x00000146, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER N WITH CEDILLA */,
    { 0x00000147, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER N WITH CARON */,
    { 0x00000148, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER N WITH CARON */,
    //{ 0x00000149, UCharSet::wordpart |
    //              UCharSet::lower } /* LATIN SMALL LETTER N PRECEDED BY APOSTROPHE */,
    { 0x0000014A, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER ENG */,
    { 0x0000014B, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER ENG */,
    { 0x0000014C, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER O WITH MACRON */,
    { 0x0000014D, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER O WITH MACRON */,
    { 0x0000014E, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER O WITH BREVE */,
    { 0x0000014F, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER O WITH BREVE */,
    { 0x00000150, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER O WITH DOUBLE ACUTE */,
    { 0x00000151, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER O WITH DOUBLE ACUTE */,
    { 0x00000152, UCharSet::wordpart |
                  UCharSet::upper |
                  UCharSet::ligature } /* LATIN_CAPITAL_LIGATURE_OE */,
    { 0x00000153, UCharSet::wordpart |
                  UCharSet::lower |
                  UCharSet::ligature } /* LATIN_SMALL_LIGATURE_OE */,
    { 0x00000154, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER R WITH ACUTE */,
    { 0x00000155, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER R WITH ACUTE */,
    { 0x00000156, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER R WITH CEDILLA */,
    { 0x00000157, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER R WITH CEDILLA */,
    { 0x00000158, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER R WITH CARON */,
    { 0x00000159, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER R WITH CARON */,
    { 0x0000015A, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER S WITH ACUTE */,
    { 0x0000015B, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER S WITH ACUTE */,
    { 0x0000015C, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER S WITH CIRCUMFLEX */,
    { 0x0000015D, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER S WITH CIRCUMFLEX */,
    { 0x0000015E, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER S WITH CEDILLA */,
    { 0x0000015F, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER S WITH CEDILLA */,
    { 0x00000160, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_S_WITH_CARON */,
    { 0x00000161, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_S_WITH_CARON */,
    { 0x00000162, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER T WITH CEDILLA */,
    { 0x00000163, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER T WITH CEDILLA */,
    { 0x00000164, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER T WITH CARON */,
    { 0x00000165, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER T WITH CARON */,
    { 0x00000166, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER T WITH STROKE */,
    { 0x00000167, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER T WITH STROKE */,
    { 0x00000168, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER U WITH TILDE */,
    { 0x00000169, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER U WITH TILDE */,
    { 0x0000016A, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER U WITH MACRON */,
    { 0x0000016B, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER U WITH MACRON */,
    { 0x0000016C, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER U WITH BREVE */,
    { 0x0000016D, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER U WITH BREVE */,
    { 0x0000016E, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER U WITH RING ABOVE */,
    { 0x0000016F, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER U WITH RING ABOVE */,
    { 0x00000170, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER U WITH DOUBLE ACUTE */,
    { 0x00000171, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER U WITH DOUBLE ACUTE */,
    { 0x00000172, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER U WITH OGONEK */,
    { 0x00000173, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER U WITH OGONEK */,
    { 0x00000174, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER W WITH CIRCUMFLEX */,
    { 0x00000175, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER W WITH CIRCUMFLEX */,
    { 0x00000176, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER Y WITH CIRCUMFLEX */,
    { 0x00000177, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER Y WITH CIRCUMFLEX */,
    { 0x00000178, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_CAPITAL_LETTER_Y_WITH_DIAERESIS */,
    { 0x00000179, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER Z WITH ACUTE */,
    { 0x0000017A, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER Z WITH ACUTE */,
    { 0x0000017B, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER Z WITH DOT ABOVE */,
    { 0x0000017C, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER Z WITH DOT ABOVE */,
    { 0x0000017D, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_Z_WITH_CARON */,
    { 0x0000017E, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_Z_WITH_CARON */,
    //{ 0x0000017F, UCharSet::wordpart |
    //              UCharSet::lower } /* LATIN SMALL LETTER LONG S */,
    /* Latin Extended-B Range: 0180–024F */
    { 0x00000192, UCharSet::symbol |
                  UCharSet::punct } /* LATIN_SMALL_LETTER_F_WITH_HOOK */,
    { 0x000001A0, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_O_WITH_HORN */,
    { 0x000001A1, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_O_WITH_HORN */,
    { 0x000001AF, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN_CAPITAL_LETTER_U_WITH_HORN */,
    { 0x000001B0, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN_SMALL_LETTER_U_WITH_HORN */,
    { 0x00000218, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER S WITH COMMA BELOW */,
    { 0x00000219, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER S WITH COMMA BELOW */,
    { 0x0000021A, UCharSet::wordpart |
                  UCharSet::upper } /* LATIN CAPITAL LETTER T WITH COMMA BELOW */,
    { 0x0000021B, UCharSet::wordpart |
                  UCharSet::lower } /* LATIN SMALL LETTER T WITH COMMA BELOW */,
    /* Spacing Modifier Letters Range: 02B0–02FF */
    { 0x000002C6, UCharSet::symbol |
                  UCharSet::punct } /* MODIFIER_LETTER_CIRCUMFLEX_ACCENT */,
    /* Combining Diacritical Marks Range: 0300–036F */
    { 0x00000300, UCharSet::wordpart |
                  UCharSet::upper } /* COMBINING_GRAVE_ACCENT */,
    { 0x00000301, UCharSet::wordpart |
                  UCharSet::lower } /* COMBINING_ACUTE_ACCENT */,
    { 0x00000303, UCharSet::wordpart |
                  UCharSet::upper } /* COMBINING_TILDE */,
    { 0x00000309, UCharSet::wordpart |
                  UCharSet::upper } /* COMBINING_HOOK_ABOVE */,
    { 0x00000323, UCharSet::wordpart |
                  UCharSet::lower } /* COMBINING_DOT_BELOW */,
    /* Cyrillic Range: 0400–04FF */
    { 0x00000401, UCharSet::accent |
                  UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_IO */,
    { 0x00000402, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_DJE */,
    { 0x00000403, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_GJE */,
    { 0x00000404, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_UKRAINIAN_IE */,
    { 0x00000405, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_DZE */,
    { 0x00000406, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_BYELORUSSIAN-UKRAINIAN_I */,
    { 0x00000407, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_YI */,
    { 0x00000408, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_JE */,
    { 0x00000409, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_LJE */,
    { 0x0000040A, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_NJE */,
    { 0x0000040B, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_TSHE */,
    { 0x0000040C, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_KJE */,
    { 0x0000040E, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_SHORT_U */,
    { 0x0000040F, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_DZHE */,
    { 0x00000410, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_A */,
    { 0x00000411, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_BE */,
    { 0x00000412, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_VE */,
    { 0x00000413, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_GHE */,
    { 0x00000414, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_DE */,
    { 0x00000415, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_IE */,
    { 0x00000416, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_ZHE */,
    { 0x00000417, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_ZE */,
    { 0x00000418, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_I */,
    { 0x00000419, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_SHORT_I */,
    { 0x0000041A, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_KA */,
    { 0x0000041B, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_EL */,
    { 0x0000041C, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_EM */,
    { 0x0000041D, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_EN */,
    { 0x0000041E, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_O */,
    { 0x0000041F, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_PE */,
    { 0x00000420, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_ER */,
    { 0x00000421, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_ES */,
    { 0x00000422, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_TE */,
    { 0x00000423, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_U */,
    { 0x00000424, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_EF */,
    { 0x00000425, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_HA */,
    { 0x00000426, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_TSE */,
    { 0x00000427, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_CHE */,
    { 0x00000428, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_SHA */,
    { 0x00000429, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_SHCHA */,
    { 0x0000042A, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_HARD_SIGN */,
    { 0x0000042B, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_YERU */,
    { 0x0000042C, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_SOFT_SIGN */,
    { 0x0000042D, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_E */,
    { 0x0000042E, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_YU */,
    { 0x0000042F, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_YA */,
    { 0x00000430, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_A */,
    { 0x00000431, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_BE */,
    { 0x00000432, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_VE */,
    { 0x00000433, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_GHE */,
    { 0x00000434, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_DE */,
    { 0x00000435, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_IE */,
    { 0x00000436, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_ZHE */,
    { 0x00000437, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_ZE */,
    { 0x00000438, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_I */,
    { 0x00000439, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_SHORT_I */,
    { 0x0000043A, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_KA */,
    { 0x0000043B, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_EL */,
    { 0x0000043C, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_EM */,
    { 0x0000043D, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_EN */,
    { 0x0000043E, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_O */,
    { 0x0000043F, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_PE */,
    { 0x00000440, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_ER */,
    { 0x00000441, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_ES */,
    { 0x00000442, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_TE */,
    { 0x00000443, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_U */,
    { 0x00000444, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_EF */,
    { 0x00000445, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_HA */,
    { 0x00000446, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_TSE */,
    { 0x00000447, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_CHE */,
    { 0x00000448, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_SHA */,
    { 0x00000449, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_SHCHA */,
    { 0x0000044A, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_HARD_SIGN */,
    { 0x0000044B, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_YERU */,
    { 0x0000044C, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_SOFT_SIGN */,
    { 0x0000044D, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_E */,
    { 0x0000044E, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_YU */,
    { 0x0000044F, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_YA */,
    { 0x00000451, UCharSet::accent |
                  UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_IO */,
    { 0x00000452, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_DJE */,
    { 0x00000453, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_GJE */,
    { 0x00000454, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_UKRAINIAN_IE */,
    { 0x00000455, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_DZE */,
    { 0x00000456, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_BYELORUSSIAN-UKRAINIAN_I */,
    { 0x00000457, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_YI */,
    { 0x00000458, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_JE */,
    { 0x00000459, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_LJE */,
    { 0x0000045A, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_NJE */,
    { 0x0000045B, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_TSHE */,
    { 0x0000045C, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_KJE */,
    { 0x0000045E, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_SHORT_U */,
    { 0x0000045F, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_DZHE */,
    { 0x00000490, UCharSet::wordpart |
                  UCharSet::upper } /* CYRILLIC_CAPITAL_LETTER_GHE_WITH_UPTURN */,
    { 0x00000491, UCharSet::wordpart |
                  UCharSet::lower } /* CYRILLIC_SMALL_LETTER_GHE_WITH_UPTURN */,
    /* General Punctuation Range: 2000–206F */
    { 0x00002013, UCharSet::punct } /* EN_DASH */,
    { 0x00002014, UCharSet::punct } /* EM_DASH */,
    { 0x00002018, UCharSet::punct } /* LEFT_SINGLE_QUOTATION_MARK */,
    { 0x00002019, UCharSet::punct |
                  UCharSet::wordpart } /* RIGHT_SINGLE_QUOTATION_MARK */,
    { 0x0000201A, UCharSet::punct } /* SINGLE_LOW-9_QUOTATION_MARK */,
    { 0x0000201C, UCharSet::punct } /* LEFT_DOUBLE_QUOTATION_MARK */,
    { 0x0000201D, UCharSet::punct } /* RIGHT_DOUBLE_QUOTATION_MARK */,
    { 0x0000201E, UCharSet::punct } /* DOUBLE_LOW-9_QUOTATION_MARK */,
    { 0x00002020, UCharSet::symbol |
                  UCharSet::punct } /* DAGGER */,
    { 0x00002021, UCharSet::symbol |
                  UCharSet::punct } /* DOUBLE_DAGGER */,
    { 0x00002022, UCharSet::punct } /* BULLET */,
    { 0x00002026, UCharSet::punct } /* HORIZONTAL_ELLIPSIS */,
    { 0x00002030, UCharSet::punct } /* PER_MILLE_SIGN */,
    { 0x00002039, UCharSet::punct } /* SINGLE_LEFT-POINTING_ANGLE_QUOTATION_MARK */,
    { 0x0000203A, UCharSet::punct } /* SINGLE_RIGHT-POINTING_ANGLE_QUOTATION_MARK */,
    /* Currency Symbols Range: 20A0–20CF */
    { 0x000020AB, UCharSet::symbol |
                  UCharSet::punct } /* DONG_SIGN */,
    { 0x000020AC, UCharSet::symbol |
                  UCharSet::punct } /* EURO_SIGN */,
    /* Letterlike Symbols Range: 2100–214F */
    { 0x00002116, UCharSet::punct } /* NUMERO_SIGN */,
    { 0x00002122, UCharSet::symbol |
                  UCharSet::punct } /* TRADE_MARK_SIGN */ };


#endif /* _PROPS_H_ */
