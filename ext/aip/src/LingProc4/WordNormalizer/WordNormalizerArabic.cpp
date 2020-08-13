
#include <_include/cc_compat.h>
#include <_include/_inttype.h>

#include "WordNormalizerArabic.h"

static uint32_t ignored_chars[] =
{
    0x00000600 /* ARABIC_NUMBER_SIGN */,
    0x00000601 /* ARABIC_SIGN_SANAH */,
    0x00000602 /* ARABIC_FOOTNOTE_MARKER */,
    0x00000603 /* ARABIC_SIGN_SAFHA */,
    0x00000610 /* ARABIC_SIGN_SALLALLAHOU_ALAYHE_WASSALLAM */,
    0x00000611 /* ARABIC_SIGN_ALAYHE_ASSALLAM */,
    0x00000612 /* ARABIC_SIGN_RAHMATULLAH_ALAYHE */,
    0x00000613 /* ARABIC_SIGN_RADI_ALLAHOU_ANHU */,
    0x00000614 /* ARABIC_SIGN_TAKHALLUS */,
    0x00000615 /* ARABIC_SMALL_HIGH_TAH */,
    0x00000617 /* ARABIC_SMALL_HIGH_ZAIN */,
    0x00000618 /* ARABIC_SMALL_FATHA */,
    0x00000619 /* ARABIC_SMALL_DAMMA */,
    0x0000061A /* ARABIC_SMALL_KASRA */,
    0x00000640 /* ARABIC_TATWEEL */,
    0x0000064B /* ARABIC_FATHATAN */,
    0x0000064C /* ARABIC_DAMMATAN */,
    0x0000064D /* ARABIC_KASRATAN */,
    0x0000064E /* ARABIC_FATHA */,
    0x0000064F /* ARABIC_DAMMA */,
    0x00000650 /* ARABIC_KASRA */,
    0x00000651 /* ARABIC_SHADDA */,
    0x00000652 /* ARABIC_SUKUN */,
    0x00000653 /* ARABIC_MADDAH_ABOVE */,
    0x00000654 /* ARABIC_HAMZA_ABOVE */,
    0x00000655 /* ARABIC_HAMZA_BELOW */,
    0x0000066C /* ARABIC_THOUSANDS_SEPARATOR */,
    0x00000670 /* ARABIC_LETTER_SUPERSCRIPT_ALEF */,
    0x00000671 /* TODO: Add leter description */,
    0x000006A4 /* ARABIC_LETTER_VEH*/,
    0x000006D6 /* ARABIC_SMALL_HIGH_LIGATURE_SAD_WITH_LAM_WITH_ALEF_MAKSURA */,
    0x000006D7 /* ARABIC_SMALL_HIGH_LIGATURE_QAF_WITH_LAM_WITH_ALEF_MAKSURA */,
    0x000006D8 /* ARABIC_SMALL_HIGH_MEEM_INITIAL_FORM */,
    0x000006D9 /* ARABIC_SMALL_HIGH_LAM_ALEF */,
    0x000006DA /* ARABIC_SMALL_HIGH_JEEM */,
    0x000006DB /* ARABIC_SMALL_HIGH_THREE_DOTS */,
    0x000006DC /* ARABIC_SMALL_HIGH_SEEN */,
    0x000006DD /* ARABIC_END_OF_AYAH */,
    0x000006DF /* ARABIC_SMALL_HIGH_ROUNDED_ZERO */,
    0x000006E0 /* ARABIC_SMALL_HIGH_UPRIGHT_RECTANGULAR_ZERO */,
    0x000006E1 /* ARABIC_SMALL_HIGH_DOTLESS_HEAD_OF_KHAH */,
    0x000006E2 /* ARABIC_SMALL_HIGH_MEEM_ISOLATED_FORM */,
    0x000006E3 /* ARABIC_SMALL_LOW_SEEN */,
    0x000006E4 /* ARABIC_SMALL_HIGH_MADDA */,
    0x000006E7 /* ARABIC_SMALL_HIGH_YEH */,
    0x000006E8 /* ARABIC_SMALL_HIGH_NOON */,
    0x000006EA /* ARABIC_EMPTY_CENTRE_LOW_STOP */,
    0x000006EB /* ARABIC_EMPTY_CENTRE_HIGH_STOP */,
    0x000006EC /* ARABIC_ROUNDED_HIGH_STOP_WITH_FILLED_CENTRE */,
    0x000006ED /* ARABIC_SMALL_LOW_MEEM */,
    0x0000FBB2 /* UFBB2 */,
    0x0000FBB3 /* UFBB3 */,
    0x0000FBB4 /* UFBB4 */,
    0x0000FBB5 /* UFBB5 */,
    0x0000FBB6 /* UFBB6 */,
    0x0000FBB7 /* UFBB7 */,
    0x0000FBB8 /* UFBB8 */,
    0x0000FBB9 /* UFBB9 */,
    0x0000FBBA /* UFBBA */,
    0x0000FBBB /* UFBBB */,
    0x0000FBBC /* UFBBC */,
    0x0000FBBD /* UFBBD */,
    0x0000FBBE /* UFBBE */,
    0x0000FBBF /* UFBBF */,
    0x0000FBC0 /* UFBC0 */,
    0x0000FBC1 /* UFBC1 */
    //suppress pedagogical symbols
    // 0x0616, // early Persian diacritic
    // 0x0656, 0x0657, 0x0658, 0x0659, 0x065A, 0x065B, 0x065C, 0x065D, 0x065E, 0x065F,  // non-arabic diacritic (now is unchangeable)
    // check presentation forms
    // TODO: consider 0xFEFF and other presentation forms
};

static uint32_t single_mapping[][2] =
{
    { 0x00000606, 0x0000221B } /* ARABIC-INDIC_CUBE_ROOT -> CUBE_ROOT */,
    { 0x00000607, 0x0000221C } /* ARABIC-INDIC_FOURTH_ROOT -> FOURTH_ROOT */,
    { 0x00000609, 0x00002030 } /* ARABIC-INDIC_PER_MILLE_SIGN -> PER_MILLE_SIGN */,
    { 0x0000060A, 0x00002031 } /* ARABIC-INDIC_PER_TEN_THOUSAND_SIGN -> PER_TEN_THOUSAND_SIGN */,
    { 0x0000060C, 0x0000002C } /* ARABIC_COMMA -> COMMA */,
    { 0x0000060D, 0x0000002E } /* ARABIC_DATE_SEPARATOR -> FULL_STOP */,
    { 0x0000061B, 0x0000003B } /* ARABIC_SEMICOLON -> SEMICOLON */,
    { 0x0000061E, 0x0000002E } /* ARABIC_TRIPLE_DOT_PUNCTUATION_MARK -> FULL_STOP */,
    { 0x0000061F, 0x0000003F } /* ARABIC_QUESTION_MARK -> QUESTION_MARK */,
    { 0x00000622, 0x00000627 } /* ARABIC_LETTER_ALEF_WITH_MADDA_ABOVE -> ARABIC_LETTER_ALEF */,
    { 0x00000623, 0x00000627 } /* ARABIC_LETTER_ALEF_WITH_HAMZA_ABOVE -> ARABIC_LETTER_ALEF */,
    { 0x00000624, 0x00000648 } /* ARABIC_LETTER_WAW_WITH_HAMZA_ABOVE -> ARABIC_LETTER_WAW */,
    { 0x00000625, 0x00000627 } /* ARABIC_LETTER_ALEF_WITH_HAMZA_BELOW -> ARABIC_LETTER_ALEF */,
    { 0x00000626, 0x0000064A } /* ARABIC_LETTER_YEH_WITH_HAMZA_ABOVE -> ARABIC_LETTER_YEH */,
    { 0x00000629, 0x00000647 } /* ARABIC_LETTER_TEH_MARBUTA -> ARABIC_LETTER_HEH (TODO: make flag) */,
    { 0x00000649, 0x0000064A } /* ARABIC_LETTER_ALEF_MAKSURA -> ARABIC_LETTER_YEH (TODO: make flag) */,
    { 0x00000660, 0x00000030 } /* ARABIC-INDIC_DIGIT_ZERO -> DIGIT_ZERO */,
    { 0x00000661, 0x00000031 } /* ARABIC-INDIC_DIGIT_ONE -> DIGIT_ONE */,
    { 0x00000662, 0x00000032 } /* ARABIC-INDIC_DIGIT_TWO -> DIGIT_TWO */,
    { 0x00000663, 0x00000033 } /* ARABIC-INDIC_DIGIT_THREE -> DIGIT_THREE */,
    { 0x00000664, 0x00000034 } /* ARABIC-INDIC_DIGIT_FOUR -> DIGIT_FOUR */,
    { 0x00000665, 0x00000035 } /* ARABIC-INDIC_DIGIT_FIVE -> DIGIT_FIVE */,
    { 0x00000666, 0x00000036 } /* ARABIC-INDIC_DIGIT_SIX -> DIGIT_SIX */,
    { 0x00000667, 0x00000037 } /* ARABIC-INDIC_DIGIT_SEVEN -> DIGIT_SEVEN */,
    { 0x00000668, 0x00000038 } /* ARABIC-INDIC_DIGIT_EIGHT -> DIGIT_EIGHT */,
    { 0x00000669, 0x00000039 } /* ARABIC-INDIC_DIGIT_NINE -> DIGIT_NINE */,
    { 0x0000066A, 0x00000025 } /* ARABIC_PERCENT_SIGN -> PERCENT_SIGN */,
    { 0x0000066B, 0x0000002E } /* ARABIC_DECIMAL_SEPARATOR -> FULL_STOP */,
    { 0x0000066D, 0x0000002A } /* ARABIC_FIVE_POINTED_STAR -> ASTERISK */,
    { 0x0000067E, 0x00000628 } /* ARABIC_LETTER_PEH -> ARABIC_LETTER_BEH */,
    { 0x000006BE, 0x00000647 } /* ARABIC_LETTER_HEH_DOACHAHMEE(Urdu) -> ARABIC_LETTER_HEH */,
    { 0x000006D4, 0x0000002E } /* ARABIC_FULL_STOP -> FULL_STOP */,
    { 0x000006F0, 0x00000030 } /* EXTENDED_ARABIC-INDIC_DIGIT_ZERO -> DIGIT_ZERO */,
    { 0x000006F1, 0x00000031 } /* EXTENDED_ARABIC-INDIC_DIGIT_ONE -> DIGIT_ONE */,
    { 0x000006F2, 0x00000032 } /* EXTENDED_ARABIC-INDIC_DIGIT_TWO -> DIGIT_TWO */,
    { 0x000006F3, 0x00000033 } /* EXTENDED_ARABIC-INDIC_DIGIT_THREE -> DIGIT_THREE */,
    { 0x000006F4, 0x00000034 } /* EXTENDED_ARABIC-INDIC_DIGIT_FOUR -> DIGIT_FOUR */,
    { 0x000006F5, 0x00000035 } /* EXTENDED_ARABIC-INDIC_DIGIT_FIVE -> DIGIT_FIVE */,
    { 0x000006F6, 0x00000036 } /* EXTENDED_ARABIC-INDIC_DIGIT_SIX -> DIGIT_SIX */,
    { 0x000006F7, 0x00000037 } /* EXTENDED_ARABIC-INDIC_DIGIT_SEVEN -> DIGIT_SEVEN */,
    { 0x000006F8, 0x00000038 } /* EXTENDED_ARABIC-INDIC_DIGIT_EIGHT -> DIGIT_EIGHT */,
    { 0x000006F9, 0x00000039 } /* EXTENDED_ARABIC-INDIC_DIGIT_NINE -> DIGIT_NINE */
};

static uint32_t collapse_seq_chars[][2] =
{
    { 0x00000627, 1 } /* collapse sequence of 2 or more ARABIC_LETTER_ALEF */,
    { 0x00000648, 2 } /* collapse sequence of 3 or more ARABIC_LETTER_WAW */,
    { 0x0000064A, 2 } /* collapse sequence of 3 or more ARABIC_LETTER_YEH */
};

ArabicSymbolPostProcessor::ArabicSymbolPostProcessor() :
    table()
{
    for ( size_t i = 0; i < sizeof(single_mapping) / 2 / sizeof(uint32_t); i++ )
        table.setValue( single_mapping[i][0], single_mapping[i][1] );
}

WordNormalizerResult ArabicSymbolPostProcessor::postProcessSymbol( uint32_t       &ch,
                                                                   size_t          /* wordIdx */,
                                                                   const char*     /* word */,
                                                                   size_t          /* wordSize */,
                                                                   const UCharSet* /* charset */,
                                                                   uint32_t        /* flags */ )
{
    uint32_t map = table.getValue( ch );
    if ( map != 0 )
        ch = map;
    return NORMALIZE_SUCCESS;
}

ArabicSymbolCopier::ArabicSymbolCopier() :
    table(),
    seqs(),
    prevChar( 0 ),
    seqCount( 0 )
{
    for ( size_t i = 0; i < sizeof(ignored_chars) / sizeof(uint32_t); i++ )
        table.setValue( ignored_chars[i], 1 );

    for ( size_t i = 0; i < sizeof(collapse_seq_chars) / 2 / sizeof(uint32_t); i++ )
        seqs.setValue( collapse_seq_chars[i][0], collapse_seq_chars[i][1] );
}

WordNormalizerResult ArabicSymbolCopier::copySymbol( uint32_t  ch,
                                                     char     *outBuffer,
                                                     size_t    /* outBufferSize */,
                                                     size_t   &outBufferIdx,
                                                     uint32_t  flags )
{
    if ( table.getValue( ch ) == 0 )
    {
        if ( flags & MORPH_FLAG_ARABIC_SEQ_COLLAPSE )
        {
            if ( outBufferIdx == 0 )
            {
                prevChar = 0;
                seqCount = 0;
            }
            else if ( prevChar == ch )
            {
                seqCount++;

                uint32_t maxSeq = seqs.getValue( ch );
                if ( maxSeq != 0 )
                {
                    if ( seqCount == maxSeq )
                    {
                        for ( size_t i = 0; i < maxSeq - 1; i++ )
                        {
                            outBufferIdx -= U8_LENGTH( ch );
                        }
                    }

                    if ( seqCount >= maxSeq )
                    {
                        outBufferIdx -= U8_LENGTH( ch );
                    }
                }
            }
            else
                seqCount = 0;

            prevChar = ch;
        }

        U8_APPEND_UNSAFE( outBuffer, outBufferIdx, ch );
    }
    return NORMALIZE_SUCCESS;
}

