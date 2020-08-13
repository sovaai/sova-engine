
#include "UCharSet.h"

#include "props.h"
#include "lang_de.h"
#include "lang_en.h"
#include "lang_es.h"
#include "lang_eu.h"
#include "lang_fr.h"
#include "lang_ru.h"
#include "lang_ua.h"
#include "lang_vi.h"
#include "lang_ar.h"
#include "lang_cy.h"
#include "lang_kz.h"
#include "lang_ge.h"
#include "lang_cz.h"
#include "lang_gr.h"
#include "lang_pl.h"

int UCharSet::Init( LangCodes lang )
{
    // load props
    for ( size_t i = 0; i < sizeof(char_props) / 2 / sizeof(uint32_t); i++ )
        charProps.setValue( char_props[i][0], char_props[i][1] );

    // load native and lower syms
    switch ( lang )
    {
    case LNG_GERMAN:
        for ( size_t i = 0; i < sizeof(native_chars_de) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_de[i], charProps.getValue( native_chars_de[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_de) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_de[i][0], lower_chars_de[i][1] );
        break;

    case LNG_ENGLISH:
        for ( size_t i = 0; i < sizeof(native_chars_en) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_en[i], charProps.getValue( native_chars_en[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_en) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_en[i][0], lower_chars_en[i][1] );
        break;

    case LNG_SPANISH:
        for ( size_t i = 0; i < sizeof(native_chars_es) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_es[i], charProps.getValue( native_chars_es[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_es) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_es[i][0], lower_chars_es[i][1] );
        break;

    case LNG_EURO:
        for ( size_t i = 0; i < sizeof(native_chars_eu) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_eu[i], charProps.getValue( native_chars_eu[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_eu) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_eu[i][0], lower_chars_eu[i][1] );
        break;

    case LNG_FRENCH:
        for ( size_t i = 0; i < sizeof(native_chars_fr) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_fr[i], charProps.getValue( native_chars_fr[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_fr) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_fr[i][0], lower_chars_fr[i][1] );
        break;

    case LNG_RUSSIAN:
        for ( size_t i = 0; i < sizeof(native_chars_ru) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_ru[i], charProps.getValue( native_chars_ru[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_ru) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_ru[i][0], lower_chars_ru[i][1] );
        break;

    case LNG_UKRAINIAN:
        for ( size_t i = 0; i < sizeof(native_chars_ua) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_ua[i], charProps.getValue( native_chars_ua[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_ua) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_ua[i][0], lower_chars_ua[i][1] );
        break;

    case LNG_VIETNAMESE:
        for ( size_t i = 0; i < sizeof(native_chars_vi) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_vi[i], charProps.getValue( native_chars_vi[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_vi) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_vi[i][0], lower_chars_vi[i][1] );
        break;

    case LNG_ARABIC:
        for ( size_t i = 0; i < sizeof(native_chars_ar) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_ar[i], charProps.getValue( native_chars_ar[i] ) | UCharSet::native );
        break;

    case LNG_CYRILLIC:
        for ( size_t i = 0; i < sizeof(native_chars_cy) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_cy[i], charProps.getValue( native_chars_cy[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_cy) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_cy[i][0], lower_chars_cy[i][1] );
        break;

    case LNG_CZECH:
        for ( size_t i = 0; i < sizeof(native_chars_cz) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_cz[i], charProps.getValue( native_chars_cz[i] ) | UCharSet::native );
        break;

	case LNG_KAZAK:
		for ( size_t i = 0; i < sizeof(native_chars_kz) / sizeof(uint32_t); i++ )
			charProps.setValue( native_chars_kz[i], charProps.getValue( native_chars_kz[i] ) | UCharSet::native );
		break;

	case LNG_GEORGIAN:
		for ( size_t i = 0; i < sizeof(native_chars_ge) / sizeof(uint32_t); i++ )
			charProps.setValue( native_chars_kz[i], charProps.getValue( native_chars_ge[i] ) | UCharSet::native );
        for ( size_t i = 0; i < sizeof(lower_chars_ge) / 2 / sizeof(uint32_t); i++ )
            charLower.setValue( lower_chars_ge[i][0], lower_chars_ge[i][1] );
		break;

	case LNG_GREEK:
        for ( size_t i = 0; i < sizeof(native_chars_gr) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_gr[i], charProps.getValue( native_chars_gr[i] ) | UCharSet::native );
        break;

    case LNG_POLISH:
        for ( size_t i = 0; i < sizeof(native_chars_pl) / sizeof(uint32_t); i++ )
            charProps.setValue( native_chars_pl[i], charProps.getValue( native_chars_pl[i] ) | UCharSet::native );
        break;

    case LNG_UNKNOWN:
        /* Ugly hack for user's happiness: do lower cyrillic chars for hiers */
        charLower.setValue( 0x00000401, 0x00000415 ); /* CYRILLIC_CAPITAL_LETTER_IO -> CYRILLIC_CAPITAL_LETTER_IE */
        charLower.setValue( 0x00000451, 0x00000435 ); /* CYRILLIC_SMALL_LETTER_IO -> CYRILLIC_SMALL_LETTER_IE */
        break;

    default: return -1;
    }

    /* Assume that a native char for a language is also a word part char */
    for ( size_t i = 0; i < sizeof(native_chars_de) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_de[i], charProps.getValue( native_chars_de[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_en) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_en[i], charProps.getValue( native_chars_en[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_es) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_es[i], charProps.getValue( native_chars_es[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_eu) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_eu[i], charProps.getValue( native_chars_eu[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_fr) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_fr[i], charProps.getValue( native_chars_fr[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_ru) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_ru[i], charProps.getValue( native_chars_ru[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_ua) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_ua[i], charProps.getValue( native_chars_ua[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_vi) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_vi[i], charProps.getValue( native_chars_vi[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_ar) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_ar[i], charProps.getValue( native_chars_ar[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_cy) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_cy[i], charProps.getValue( native_chars_cy[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_cz) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_cz[i], charProps.getValue( native_chars_cz[i] ) | UCharSet::wordpart );
	for ( size_t i = 0; i < sizeof(native_chars_kz) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_kz[i], charProps.getValue( native_chars_kz[i] ) | UCharSet::wordpart );
	for ( size_t i = 0; i < sizeof(native_chars_ge) / sizeof(uint32_t); i++ )
		charProps.setValue( native_chars_ge[i], charProps.getValue( native_chars_ge[i] ) | UCharSet::wordpart );
	for ( size_t i = 0; i < sizeof(native_chars_gr) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_gr[i], charProps.getValue( native_chars_gr[i] ) | UCharSet::wordpart );
    for ( size_t i = 0; i < sizeof(native_chars_pl) / sizeof(uint32_t); i++ )
        charProps.setValue( native_chars_pl[i], charProps.getValue( native_chars_pl[i] ) | UCharSet::wordpart );

    this->lang = lang;

    return 0;
}

static void markInternalOnly(IndirectTable& charProps, uint32_t letter)
{
    uint32_t    prevValue   =   charProps.getValue(letter);

    if ( (prevValue & UCharSet::wordpart) == 0 ) {
        return;
    }

    prevValue   =   prevValue | UCharSet::internal_wordpart;
    charProps.setValue(letter, prevValue);
}

int UCharSet::Tune( unsigned int tuneMode )
{
    // Restore default table data
    Init( lang );

    // Make all TWORD-marked symbols from ctype tables as native n language tables
    if ( tuneMode & tmWordPartAsNative )
    {
        class Marker : public IndirectTable::Enumerator
        {
        public:
            Marker( IndirectTable &_t ) : t( _t ) { }

            int apply( uint32_t wchar )
            {
                uint32_t flags = t.getValue( wchar );
                if ( flags & UCharSet::wordpart )
                    t.setValue( wchar, flags | UCharSet::native );

                return 0;
            }

        private:
            IndirectTable &t;
        } marker( charProps );

        charProps.enumerate( marker );
    }

    // Simple apostroph
    if ( tuneMode & tmApostrophOnlyPunct )
        charProps.setValue( 0x00000027, punct );

    // Right quotation mark
    if ( tuneMode & tmRQuotMarkPunct )
        charProps.setValue( 0x00002019, punct );

    // Stop apostroph lowering as long as they are different
    if ( charProps.getValue( 0x00000027 ) != charProps.getValue( 0x00002019 ) )
        charLower.setValue( 0x00002019, 0x00002019 );

    // Hyphen
    if ( tuneMode & tmHyphenWordPart )
        charProps.setValue( 0x0000002D, punct | wordpart | hyphen | native );

    // Tilde
    if ( tuneMode & tmTildeWordPart )
        charProps.setValue( 0x0000007E, punct | wordpart | symbol | native );

    // Space
    if ( tuneMode & tmSpaceNative )
        charProps.setValue( 0x00000020, space | blank | native );

    // Number sign
    if ( tuneMode & tmNumberSignWordPart )
        charProps.setValue( 0x00000023, punct | wordpart );

    // At sign
    if ( tuneMode & tmAtSignWordPart )
        charProps.setValue( 0x00000040, punct | wordpart );

    // Underline
    if ( tuneMode & tmUnderlineWordPart )
        charProps.setValue( 0x0000005F, punct | wordpart );
    
    if ( tuneMode & tmWordPartWoEdge ) {
        //Hyphen
        markInternalOnly(charProps, 0x0000002D);
        // Tilde
        markInternalOnly(charProps, 0x0000007E);
        // Simple apostroph
        markInternalOnly(charProps, 0x00000027);
        // Right quotation mark
        markInternalOnly(charProps, 0x00002019);
    }

    this->tuneMode = tuneMode;

    return 0;

}

const char *UCharSet::findSubstring(
    const char *string,
    size_t stringSize,
    const char *pattern,
    size_t patternSize,
    bool caseSensitive )
{
    UErrorCode status = U_ZERO_ERROR;

    utxtA = utext_openUTF8( utxtA,
                            string,
                            ( stringSize == static_cast<size_t>(-1) ? static_cast<int64_t>(-1) : static_cast<int64_t>(stringSize) ),
                            &status );
    if ( !U_SUCCESS(status) )
        return NULL;

    utxtB = utext_openUTF8( utxtB,
                            pattern,
                            ( patternSize == static_cast<size_t>(-1) ? static_cast<int64_t>(-1) : static_cast<int64_t>(patternSize) ),
                            &status );
    if ( !U_SUCCESS(status) )
        return NULL;

    uint64_t matchStart = 0;
    UChar32  chString   = utext_next32From( utxtA, 0 );
    UChar32  chPattern  = utext_next32From( utxtB, 0 );

    for ( ; chString >= 0; chString = utext_next32( utxtA ) )
    {
        if ( chPattern < 0 )
            return string + matchStart;

        for ( ;; )
        {
            bool patternFirst = (utext_getPreviousNativeIndex( utxtB ) == 0);

            if ( (chString == chPattern) ||
                 (!caseSensitive &&
                  ToLower( static_cast<uint32_t>(chString) ) ==
                  ToLower( static_cast<uint32_t>(chPattern) )) )
            {
                if ( patternFirst )
                    matchStart = utext_getPreviousNativeIndex( utxtA );
                chPattern = utext_next32( utxtB );
                break;
            }
            else if ( patternFirst )
                break;

            chPattern  = utext_next32From( utxtB, 0 );
            matchStart = 0;
        }
    }

    return 0;
}


