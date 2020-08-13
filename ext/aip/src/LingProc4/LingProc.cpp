
#include <stdarg.h>

#include <_include/cc_compat.h>

#include "LingProc.h"
#include "LingProcData.h"

LingProc::LingProc() :                          \
    shadow( 0 ),
    runtimeFlags( defaultRuntimeFlags ),
    runtimeFlagsAdjusted( false ),
    runtimeParamsLoaded( false ),
    enabledMorphs()
{
    LingProcData::Create( shadow );
}

LingProc::LingProc( const LingProc &src )
{
    shadow = 0;
    this->operator=( src );
}

LingProc &LingProc::operator=( const LingProc &src )
{
    LingProcData::CreateCopy( shadow, src.shadow );

    runtimeFlags         = src.runtimeFlags;
    runtimeFlagsAdjusted = src.runtimeFlagsAdjusted;
    runtimeParamsLoaded  = src.runtimeParamsLoaded;
    enabledMorphs        = src.enabledMorphs;

    return *this;
}

LingProc::~LingProc()
{
    LingProcData::Release( shadow );
}

LingProcErrors LingProc::AttachStrictMorph( const char *path, LangCodes lang )
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;

    fstorage *fs = fstorage_create();
    if ( unlikely(fs == 0) )
        return LP_ERROR_ENOMEM;

    fstorage_connect_config fscc;
    memset( &fscc, 0, sizeof(fscc) );
    fscc.pid = FSTORAGE_PID_STRICT_MORPH;

    if( unlikely(fstorage_connect_shared( fs, path, &fscc ) != FSTORAGE_OK) )
    {
        fstorage_close( fs );
        fstorage_destroy( fs );
        return LP_ERROR_FS_FAILED;
    }

    LingProcErrors status = AttachStrictMorph( fs, lang );
    if ( unlikely(status != LP_OK) )
    {
        fstorage_close( fs );
        fstorage_destroy( fs );
        return status;
    }

    return shadow->InstallStrictMorphFS( fs );
}

LingProcErrors LingProc::AttachStrictMorph( fstorage *fs, LangCodes lang )
{
    return AttachStrictMorph( fs, 1, &lang );
}

static LingProcErrors attachDefaultStrictMorphData( LingProc &lp, fstorage *fs )
{
    /* Try to open each lang from the array */
    bool found = false;
    for ( int i = LingProcData::kAllLanguagesSize - 1; i >= 0; i-- )
    {
        LingProcErrors status = lp.AttachStrictMorph( fs, LingProcData::kAllLanguages[i] );
        if ( status == LP_OK )
            found = true;
        else if ( status != LP_ERROR_STRICT_DATA )
            return status;
    }

    /* Check if something is opened without an error */
    if ( unlikely(!found) )
        return LP_ERROR_STRICT_DATA;

    return LP_OK;
}

LingProcErrors LingProc::AttachStrictMorph( fstorage *fs, size_t langsListSize, const LangCodes *langsList )
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;
    if ( unlikely(fs == 0) )
        return LP_ERROR_EINVAL;
    if ( unlikely(langsListSize < 1) )
        return LP_ERROR_EINVAL;

    /* Process LNG_DEFAULT */
    if ( langsListSize == 1 && langsList[0] == LNG_DEFAULT )
        return attachDefaultStrictMorphData( *this, fs );

    if ( langsListSize == 1 && langsList[0] == LNG_ALL )
    {
        langsListSize = LingProcData::kAllLanguagesSize;
        langsList     = LingProcData::kAllLanguages;
    }

    /* Iterate over languages list */
    for ( size_t i = 0; i < langsListSize; i++ )
    {
        LangCodes lang = langsList[ i ];

        /* Check if the language is valid */
        if ( unlikely( lang == LNG_ALL     ||
                       lang == LNG_DEFAULT ||
                       lang == LNG_INVALID ||
                       lang == LNG_UNKNOWN ) )
            return LP_ERROR_UNSUPPORTED_LANG;

        /* Check if there is a strict morph data for the language given in the fstorage provided */
        const LanguageInfo *langFullInfo = LanguageInfo::getLanguageInfo( lang );
        if ( unlikely(langFullInfo == 0) )
            return LP_ERROR_UNSUPPORTED_LANG;
        fstorage_section_id headerSection =
            LanguageInfo::getLPSectionStrict( langFullInfo->fsectionOffset );

        fstorage_section *section = fstorage_find_section(fs, headerSection);
        if ( unlikely(section == 0) )
            return LP_ERROR_STRICT_DATA;

        LingProcErrors status = shadow->newAttachedStrictMorph( lang, fs );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    return LP_OK;
}

void LingProc::DetachStrictMorphAll()
{
    if ( likely(shadow != 0) )
        shadow->detachStrictMorphs();
}

LingProcErrors LingProc::Create( const LingProcParams *params, size_t langsListSize, const LangCodes *langsList )
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_ENOMEM;

    return shadow->initialize( params, langsListSize, langsList, enabledMorphs );
}

LingProcErrors LingProc::Open( LingProcOpenModes mode, fstorage *fs, size_t langsListSize, const LangCodes *langsList )
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_ENOMEM;

    return shadow->open( mode,
                         fs,
                         langsListSize,
                         langsList,
                         runtimeFlags,
                         runtimeFlagsAdjusted,
                         runtimeParamsLoaded,
                         enabledMorphs );
}

LingProcErrors LingProc::Save( fstorage *fs, bool transport )
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_ENOMEM;

    return shadow->save( fs,
                         transport,
                         runtimeFlags,
                         runtimeFlagsAdjusted,
                         runtimeParamsLoaded,
                         enabledMorphs );
}

LingProcErrors LingProc::Flush()
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_ENOMEM;

    return shadow->flush();
}

LingProcErrors LingProc::Close()
{
    if ( likely(shadow != 0) )
        return LingProcData::Create( shadow );

    return LP_ERROR_ENOMEM;
}

uint32_t LingProc::CharSetTuneMode() const
{
    if ( unlikely(shadow == 0) )
        return 0;

    return shadow->getCharsetTuneMode();
}

const UCharSet *LingProc::GenericCharSet() const
{
    if ( unlikely(shadow == 0) )
        return 0;

    return shadow->getGenericCharset();
}

const UCharSet *LingProc::GetCharset( LangCodes lang ) const
{
    if ( unlikely(shadow == 0) )
        return 0;

    return shadow->getCharset( lang );
}

LingProcErrors LingProc::TuneCharSetSymbolAdd( LangCodes lang, uint32_t ch, UCharSet::flags flag )
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_ENOMEM;

    return shadow->charsetSymbolAdd( lang, ch, flag );
}

LingProcErrors LingProc::TuneCharSetSymbolClear( LangCodes lang, uint32_t ch, UCharSet::flags flag )
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_ENOMEM;

    return shadow->charsetSymbolClear( lang, ch, flag );
}

LingProcErrors LingProc::DumpLexid( LEXID lex, StringsReceiver &textReceiver )
{
    char *buf = textReceiver.requestBuffer( (sizeof(LEXID) * 2) + 1 );
    if ( unlikely(buf == 0) )
        return LP_ERROR_ENOMEM;

    size_t i, r;
    for ( i = 0, r = (sizeof(LEXID) << 3) - 4; i < sizeof(LEXID) * 2; i++, r -= 4 )
        buf[i] = digits[ ( lex >> r ) & 0xF ];
    buf[ i ] = '\0';
    textReceiver.commitString( i );

    return LP_OK;
}

LingProcErrors LingProc::RestoreLexid( const char *buf, size_t bufSize, LEXID &lex )
{
    if ( bufSize == static_cast<size_t>(-1) )
        bufSize = strlen( buf );

    size_t expectedBufSize = sizeof(LEXID) * 2;

    size_t i;
    for ( i = 0, lex = 0; i < bufSize; i++ )
    {
        lex <<= 4;
        char c = buf[i];
        if ( c >= '0' && c <= '9' )
            lex |= static_cast<LEXID>( c - '0' ) & 0xF;
        else if ( c >= 'A' && c <= 'F' )
            lex |= static_cast<LEXID>( c - 'A' + 10 ) & 0xF;
        else
            return LP_ERROR_EINVAL;
    }

    if ( unlikely(bufSize < expectedBufSize) )
        return LP_ERROR_NOT_ENOUGH_DATA;

    return LP_OK;
}

LingProcErrors LingProc::MorphNo( LangCodes lang, MorphTypes type, unsigned int &no ) const
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;

    return shadow->MorphNo( lang, type, no );
}

bool LingProc::IsCompoundModeOn( LangCodes lang ) const
{
    if ( unlikely(shadow == 0) )
        return false;

    return shadow->IsCompoundModeOn( lang );
}

bool LingProc::IsLangEnabled( LangCodes lang ) const
{
    if ( unlikely(shadow == 0) )
        return false;

    return shadow->IsLangEnabled( lang );
}

