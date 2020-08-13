
#include <_include/cc_compat.h>
#include <unicode/utf8.h>

#include "DetectWords.h"
#include "data/detect_tables.h"
#include "LingProcErrors.h"

LingProcErrors DetectWords::Init( enum LangCodes lang, const UCharSet *charset )
{
    // getting detect table
    const char * const *detect_table = NULL;
    switch (lang)
    {
    case LNG_RUSSIAN:
        detect_table = detect_table_rus;
        break;
    case LNG_UKRAINIAN:
        detect_table = detect_table_ukr;
        break;
    case LNG_ENGLISH:
        detect_table = detect_table_eng;
        break;
    case LNG_EURO:
        detect_table = detect_table_eur;
        break;
    case LNG_GERMAN:
        detect_table = detect_table_deu;
        break;
    case LNG_FRENCH:
        detect_table = detect_table_fra;
        break;
    case LNG_SPANISH:
        detect_table = detect_table_esp;
        break;
    case LNG_VIETNAMESE:
        detect_table = detect_table_vie;
        break;
    default:
        return LP_ERROR_UNSUPPORTED_LANG;
    }
  
    return LingProcDict::Init( detect_table, charset );
}

int DetectWords::CheckWord( const char *word, size_t len ) const
/* returns:
   0 - not detected,
   1 - charset match (for words with 3 or more lang chars),
   2 - word match,
   LingProcErrors - on errors */
{
    if( charset == 0 )
        return LP_ERROR_INVALID_MODE;
    if ( word == 0 )
        return LP_ERROR_EINVAL;

    if ( len == 0 )
        return 0;

    size_t offset        = 0;
    size_t langCharCount = 0;

    for ( ; offset < len; )
    {
        uint32_t ch;
        U8_NEXT_UNSAFE( word, offset, ch );
        if ( ch == 0 )
        {
            len = offset - 1;
            break;
        }

        if ( !charset->IsWord( ch ) )
            return 0;
        if ( charset->IsWordIgnore( ch ) )
            continue;
        if ( !charset->IsLangChar( ch ) )
            return 0;
        
        langCharCount++;
    }

    // we DO NOT use ToLower() and ToLoPunct() to avoid extra copying
    // see CLingProc::LangDetectDoc()
    if ( ExactMatch( word, len ) > 0 )
        return 2;
    else if ( langCharCount >= 3 )
        return 1;
    
    return 0;
}

