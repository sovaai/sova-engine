
#include <_include/_string.h>
#include <_include/ntoh.h>
#include <LingProc4/ArabicMorph/libbuckwalter/LPArabicMorph.h>

#include "LingProcData.h"

const LangCodes  LingProcData::kAllLanguages[]  =   { LNG_ENGLISH
                                                    , LNG_RUSSIAN
                                                    , LNG_GERMAN
                                                    , LNG_FRENCH
                                                    , LNG_SPANISH
#ifdef FORCE_LEXID64
                                                    , LNG_ARABIC
                                                    , LNG_UKRAINIAN
#endif
                                                    };
const int       LingProcData::kAllLanguagesSize =   sizeof(LingProcData::kAllLanguages) / sizeof(LingProcData::kAllLanguages[0]);

static LPMorphInterface* strictMorphFactory(LangCodes lang)
{
    LPMorphInterface*   ret =   NULL;
    if (LNG_ARABIC == lang) {
        ret =   new TLPArabicMorph();
    } else {
        ret =   new LPMorphStrict();
    }
    return ret;
}

LingProcErrors LingProcData::obtainLangIndex( LangCodes lang, size_t &index )
{
    index = getLangIndex( lang );
    if ( likely(index != static_cast<size_t>(-1)) )
        return LP_OK;

    if ( unlikely(langs.size() >= lpMaxLangNum) )
        return LP_ERROR_TOO_MANY_LANGS;

    LangData *ld = langs.grow();
    if ( unlikely(ld == 0) )
        return LP_ERROR_ENOMEM;

    if ( lang == LNG_UNKNOWN )
        ld->charset = &genericCharset;
    else
    {
        ld->charset = new UCharSetFS();
        if ( unlikely(ld->charset == 0) )
            goto nomemory;
    }

    ld->fuzzy = new LPMorphFuzzy();
    if ( unlikely(ld->fuzzy == 0) )
        goto nomemory;

    if ( lang != LNG_UNKNOWN )
    {
        ld->tilde = new LPMorphTilde();
        if ( unlikely(ld->tilde == 0) )
            goto nomemory;

        ld->tildeRev = new LPMorphTildeRev();
        if ( unlikely(ld->tildeRev == 0) )
            goto nomemory;

        ld->strict = strictMorphFactory(lang);
        if ( unlikely(ld->strict == 0) )
            goto nomemory;

        ld->detector = LangDetector::factory( lang );
    }

    ld->lang = lang;
    index    = langs.size() - 1;

    return LP_OK;

nomemory:
    ld->drop();
    langs.pop_back();
    return LP_ERROR_ENOMEM;
}

LingProcErrors LingProcData::DeserializeHeader( fstorage *fs )
{
    // find section
    fstorage_section* sec = fstorage_find_section(fs, LINGPROC_FS_SEC_HEADER);
    if ( unlikely(sec == 0) )
        return LP_ERROR_FS_FAILED;

    // getting pointer
    uint32_t *pt = (uint32_t *)fstorage_section_get_all_data( sec );
    if ( unlikely(pt == 0) )
        return LP_ERROR_FS_FAILED;

    // checking section size
    if ( unlikely(fstorage_section_get_size( sec ) < LINGPROC_FS_HEADER_FIXED_SIZE) )
        return LP_ERROR_FS_FAILED;

    // getting data
    uint32_t magic               = be32toh(*pt++);
    uint32_t workFormatVersion   = be32toh(*pt++);
    /* uint32_t workFormatVariant   = be32toh(*pt++); */ pt++;

    morphType                    = static_cast<MorphTypes>( be32toh(*pt++) );
    morphFlags                   = be32toh(*pt++);
    charsetTuneMode              = be32toh(*pt++);

    // list of stored languages
    uint16_t *pt2      = reinterpret_cast<uint16_t *>( pt );
    int       langsNum = static_cast<int>( be16toh(*pt2++) );

    // check header size
    size_t sizeNeeded = LINGPROC_FS_HEADER_FIXED_SIZE + sizeof(uint16_t) * langsNum;
    if ( unlikely(fstorage_section_get_size( sec ) < sizeNeeded) )
        return LP_ERROR_FS_FAILED;

    storedLangs.clear();

    for ( ; langsNum > 0; langsNum-- )
    {
        storedLangs.push_back( static_cast<LangCodes>( be16toh(*pt2++) ) );
        if ( unlikely(storedLangs.no_memory()) )
            return LP_ERROR_ENOMEM;
    }

    // check versions
    if ( unlikely(magic != LINGPROC_FS_MAGIC) )
        return LP_ERROR_INVALID_FORMAT;

    if ( unlikely(workFormatVersion != LINGPROC_FS_WORK_FORMAT_VERSION) )
        return LP_ERROR_INVALID_FORMAT;

    // runtime params
    if ( sizeNeeded % sizeof(uint32_t) != 0 )
        sizeNeeded += sizeof(uint32_t) - (sizeNeeded % sizeof(uint32_t));
    sizeNeeded += sizeof(uint64_t);

    if ( unlikely(fstorage_section_get_size(sec) < sizeNeeded) )
        return LP_ERROR_FS_FAILED;

    uint8_t *pt3 = reinterpret_cast<uint8_t *>(pt2);
    while ( (pt3 - reinterpret_cast<uint8_t *>( fstorage_section_get_all_data(sec) )) % sizeof(uint32_t) != 0 )
        pt3++;

    uint64_t *pt4             = reinterpret_cast<uint64_t *>( pt3 );
    storedRuntimeFlags        = be64toh(*pt4++);
    storedRuntimeParamsLoaded = true;

    // strict langs
    strictLangs.clear();

    pt2 = reinterpret_cast<uint16_t *>( pt4 );

    sizeNeeded += sizeof(uint16_t);
    if ( unlikely(fstorage_section_get_size(sec) < sizeNeeded) )
        return LP_ERROR_FS_FAILED;

    // list of stored languages for strict morphology
    int strictLangsNum = static_cast<int>( be16toh(*pt2++) );

    sizeNeeded += sizeof(uint16_t) * strictLangsNum;
    if ( unlikely(fstorage_section_get_size(sec) < sizeNeeded) )
        return LP_ERROR_FS_FAILED;

    for ( ; strictLangsNum > 0; strictLangsNum-- )
    {
        strictLangs.push_back( static_cast<LangCodes>(be16toh(*pt2++)) );
        if ( unlikely(strictLangs.no_memory()) )
            return LP_ERROR_ENOMEM;
    }

    // disabled morphs
    disabledMorphs.clear();
    pt3 = reinterpret_cast<uint8_t *>( pt2 );

    sizeNeeded += lpMaxMorphsNum * sizeof(uint8_t);
    if ( unlikely(fstorage_section_get_size(sec) < sizeNeeded) )
        return LP_ERROR_FS_FAILED;

    for ( size_t i = 0; i < lpMaxMorphsNum; i++ )
    {
        uint8_t value = *pt3++;
        if ( value == 0 )
            disabledMorphs.unset(i);
        else
            disabledMorphs.set(i);
    }

    return LP_OK;
}

LingProcErrors LingProcData::SerializeHeader( fstorage *fs ) const
{
    // find or create section
    fstorage_section *sec = fstorage_find_section( fs, LINGPROC_FS_SEC_HEADER );
    if( unlikely(sec == 0) )
        sec = fstorage_section_create( fs, LINGPROC_FS_SEC_HEADER );
    if( unlikely(sec == 0) )
        return LP_ERROR_FS_FAILED;

    // allocating memory
    size_t headerSize = LINGPROC_FS_HEADER_FIXED_SIZE + sizeof(uint16_t) * storedLangs.size();

    // add memory for runtime params
    if ( headerSize % sizeof(uint32_t) != 0 )
        headerSize += sizeof(uint32_t) - (headerSize % sizeof(uint32_t));
    headerSize += sizeof(uint64_t) * 1;

    // add memory for strict morph languages
    headerSize += sizeof(uint16_t) + (strictLangs.size() * sizeof(uint16_t)) + (lpMaxMorphsNum * sizeof(uint8_t));

    if ( fstorage_section_get_size(sec) != headerSize )
    {
        if ( unlikely(fstorage_section_realloc(sec, headerSize) != FSTORAGE_OK) )
            return LP_ERROR_FS_FAILED;
    }

    // getting pointer
    uint32_t *pt = reinterpret_cast<uint32_t *>( fstorage_section_get_all_data(sec) );
    if ( unlikely(pt == 0) )
        return LP_ERROR_FS_FAILED;

    // storing data
    uint32_t magic               = LINGPROC_FS_MAGIC;
    uint32_t workFormatVersion   = LINGPROC_FS_WORK_FORMAT_VERSION;
    uint32_t workFormatVariant   = LINGPROC_FS_WORK_FORMAT_VARIANT;

    *pt++ = htobe32(magic);
    *pt++ = htobe32(workFormatVersion);
    *pt++ = htobe32(workFormatVariant);

    *pt++ = htobe32(morphType);
    *pt++ = htobe32(morphFlags);
    *pt++ = htobe32(charsetTuneMode);

    // list of stored languages
    uint16_t *pt2 = reinterpret_cast<uint16_t *>( pt );
    *pt2++ = htobe16( static_cast<uint16_t>(storedLangs.size()) );

    // to be absolutely sure
    if ( unlikely(reinterpret_cast<uint8_t *>(pt2) - reinterpret_cast<uint8_t *>(fstorage_section_get_all_data(sec)) != LINGPROC_FS_HEADER_FIXED_SIZE) )
        return LP_ERROR_EFAULT;

    size_t n;
    for ( n = 0; n < storedLangs.size(); n++ )
        *pt2++ = htobe16( static_cast<uint16_t>( storedLangs[n] ) );

    // runtime params
    uint8_t *pt3 = reinterpret_cast<uint8_t *>( pt2 );
    while ( (pt3 - reinterpret_cast<uint8_t *>(fstorage_section_get_all_data(sec))) % sizeof(uint32_t) != 0 )
        *pt3++ = 0;

    uint64_t *pt4 = reinterpret_cast<uint64_t *>( pt3 );
    *pt4++ = htobe64( storedRuntimeFlags );

    // strict langs
    pt2    = reinterpret_cast<uint16_t *>( pt4 );
    *pt2++ = htobe16( static_cast<uint16_t>(strictLangs.size()) );
    for ( n = 0; n < strictLangs.size(); n++ )
        *pt2++ = htobe16( static_cast<uint16_t>(strictLangs[n]) );

    // disabled morphs
    pt3 = reinterpret_cast<uint8_t *>( pt2 );
    for ( size_t i = 0; i < lpMaxMorphsNum; i++ )
    {
        uint8_t value = disabledMorphs.check(i) ? 1 : 0;
        *pt3++ = value;
    }

    return LP_OK;
}

LingProcErrors LingProcData::initializeCommon()
{
    LingProcErrors status;

    // Expand default morph flags
    if ( morphFlags & MORPH_FLAG_USE_DEFAULT )
        morphFlags = ( MORPH_FLAG_CASE_INSENSITIVE |
                       MORPH_FLAG_LWR_ACCENT       |
                       MORPH_FLAG_LWR_PUNCT        |
                       MORPH_FLAG_REM_IGNORE       |
                       MORPH_FLAG_NATIVE_ONLY );

    /* morph_create_params */
    LPMorphCreateParams morphParams;
    memset( &morphParams, 0, sizeof(morphParams) );

    morphParams.lang  = genericCharset.GetLang();
    morphParams.flags = morphFlags;

    /* punct */
    morphParams.type           = MORPH_TYPE_PUNCT;
    morphParams.internalNumber = MORPHNO_PUNCT;
    status = morphPunct.create( &genericCharset, &morphParams );
    if ( unlikely(status != LP_OK) )
        return status;

    morphs[ MORPHNO_PUNCT ].mp = &morphPunct;

    /* numeric */
    morphParams.type           = MORPH_TYPE_NUM;
    morphParams.internalNumber = MORPHNO_NUM;
    status = morphNumeric.create( &genericCharset, &morphParams );
    if ( unlikely(status != LP_OK) )
        return status;

    morphs[ MORPHNO_NUM ].mp = &morphNumeric;

    return LP_OK;
}

void LingProcData::verifyEnabledMorphs( LingProc::EnabledMorphsSet &runtimeEnabledMorphs )
{
    runtimeEnabledMorphs.clear();

    if ( !disabledMorphs.check( MORPHNO_HIER ) )
        runtimeEnabledMorphs.set( MORPHNO_HIER );
    if ( !disabledMorphs.check( MORPHNO_PUNCT ) )
        runtimeEnabledMorphs.set( MORPHNO_PUNCT );
    if ( !disabledMorphs.check( MORPHNO_NUM ) )
        runtimeEnabledMorphs.set( MORPHNO_NUM );

    for ( size_t i = 1; i < langs.size(); i++ )
    {
        const LangData &ld = langs[ i ];

        if ( ld.fuzzyInternalNo != MORPHNO_CTRL )
            runtimeEnabledMorphs.set( ld.fuzzyInternalNo );
        if ( ld.strictInternalNo != MORPHNO_CTRL )
            runtimeEnabledMorphs.set( ld.strictInternalNo );
        if ( ld.tildeInternalNo != MORPHNO_CTRL )
            runtimeEnabledMorphs.set( ld.tildeInternalNo );
        if ( ld.tildeRevInternalNo != MORPHNO_CTRL )
            runtimeEnabledMorphs.set( ld.tildeRevInternalNo );
    }
}

LingProcErrors LingProcData::initialize( const LingProcParams       *params,
                                         size_t                      langsListSize,
                                         const LangCodes            *langsList,
                                         LingProc::EnabledMorphsSet &runtimeEnabledMorphs )
{
    morphType = ( params == 0 ? MORPH_TYPE_DEFAULT : params->morphType );

    // default is fuzzy only
    if ( morphType == MORPH_TYPE_DEFAULT )
        morphType = MORPH_TYPE_FUZZY;

    if ( params != 0 && params->useMorphFlags )
        morphFlags = params->morphFlags;
    else
        morphFlags = MORPH_FLAG_USE_DEFAULT;

    if ( params == 0 )
    {
        charsetTuneMode = 0;
        morphMinStemLength = 0;
        morphMinCompoundStemLength = 0;
    }
    else
    {
        charsetTuneMode = params->charsetTuneMode;
        morphMinStemLength = params->morphMinStemLength;
        morphMinCompoundStemLength = params->morphMinCompoundStemLength;
    }

    // strict langs - copy from attached_strict_morphs table
    strictLangs.clear();
    for ( size_t i = 0; i < attachedStrictMorphs.size(); i++ )
        strictLangs.push_back( attachedStrictMorphs[i].lang );
    if ( unlikely(strictLangs.no_memory()) )
        return LP_ERROR_ENOMEM;

    // copy disabled morphs
    if ( params != 0 )
        disabledMorphs = params->disabledMorphs;

    LingProcErrors status = initializeCommon();
    if ( unlikely(status != LP_OK) )
        return status;

    // LNG_UNKNOWN: generic charset and hier morph
    status = initializeLang( LNG_UNKNOWN );
    if ( unlikely(status != LP_OK) )
        return status;

    // rus-lat converter
    status = ruslatConv.Init( params == 0 ? 0 : params->ruslatConvTuneMode );
    if ( unlikely(status != LP_OK) )
        return status;

    // phrase breaker
    if ( !pbreakerDataIsExternal && pbreakerData != 0 )
    {
        status = pbreakerData->Init( &genericCharset );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    // open langs
    if ( langsListSize == 1 && ( langsList[0] == LNG_ALL || langsList[0] == LNG_DEFAULT ) )
    {
        langsListSize = kAllLanguagesSize;
        langsList     = kAllLanguages;
    }

    for ( size_t i = 0; i < langsListSize; i++ )
    {
        LangCodes lang = langsList[ i ];

        if ( unlikely(lang == LNG_ALL || lang == LNG_DEFAULT) )
            return LP_ERROR_UNSUPPORTED_LANG;

        // check for morph types intersection
        const LanguageInfo *currentLangInfo = LanguageInfo::getLanguageInfo( lang );
        if ( unlikely(currentLangInfo == 0) )
            return LP_ERROR_UNSUPPORTED_LANG;

        for ( size_t j = 1; j < langs.size(); j++ )
        {
            const LangData &ld = langs[ j ];

            const LanguageInfo *langInfo = LanguageInfo::getLanguageInfo( ld.lang );
            if ( unlikely(langInfo == 0) )
                return LP_ERROR_UNSUPPORTED_LANG;

            switch ( morphType )
            {
            case MORPH_TYPE_FUZZY:
                if ( unlikely(langInfo->fuzzyNumber == currentLangInfo->fuzzyNumber) )
                    return LP_ERROR_MORPHS_COLLISION;
                break;

            case MORPH_TYPE_STRICT:
            case MORPH_TYPE_ALL:
                if ( unlikely(langInfo->strictNumber == currentLangInfo->strictNumber) )
                    return LP_ERROR_MORPHS_COLLISION;
                break;

            case MORPH_TYPE_TILDE:
            case MORPH_TYPE_TILDE_BOTH:
                if ( unlikely(langInfo->tildeNumber    == currentLangInfo->tildeNumber ||
                              langInfo->tildeRevNumber == currentLangInfo->tildeRevNumber ) )
                    return LP_ERROR_MORPHS_COLLISION;
                break;

            default:
                break;
            };
        }

        status = initializeLang( lang );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    verifyEnabledMorphs( runtimeEnabledMorphs );

    // open mode
    openMode = LP_MODE_CREATE;

    return LP_OK;
}

LingProcErrors LingProcData::initializeLang( LangCodes lang )
{
    LingProcErrors status;

    // find or create lang data
    size_t index;
    status = obtainLangIndex( lang, index );
    if ( unlikely(status != LP_OK) )
        return status;

    LangData &ld = langs[ index ];

    // tune hier charset if need to
    if ( lang == LNG_UNKNOWN && morphFlags & MORPH_FLAG_HIER_NATIVE_ONLY )
        charsetTuneMode |=  UCharSet::tmWordPartAsNative;
    else
        charsetTuneMode &= ~UCharSet::tmWordPartAsNative;

    // charset
    if ( lang == LNG_UNKNOWN )
    {
        if ( unlikely(ld.charset->Tune( charsetTuneMode ) != 0) )
            return LP_ERROR_EFAULT;
    }
    else if ( !ld.charset->IsValid() )
    {
        if ( unlikely(ld.charset->Init( lang ) != 0) )
            return LP_ERROR_ENOMEM;
        if ( unlikely(ld.charset->Tune( charsetTuneMode ) != 0) )
            return LP_ERROR_EFAULT;
    }

    // detect words
    if ( ( lang != LNG_UNKNOWN ) && ( ld.detector != 0 ) )
    {
        status = ld.detector->init( lang, *ld.charset );
        if ( unlikely(status != LP_OK) )
            return LP_OK;
    }

    // fuzzy or hier morph
    if ( lang == LNG_UNKNOWN )
    {
        status = initializeMorph( lang, MORPH_TYPE_HIER );
        if ( unlikely(status != LP_OK) )
            return status;
    }
    else if ( morphType == MORPH_TYPE_FUZZY || morphType == MORPH_TYPE_ALL )
    {
        status = initializeMorph( lang, MORPH_TYPE_FUZZY );
        if ( unlikely(status != LP_OK) )
            return status;
    }
    else if ( morphType == MORPH_TYPE_TILDE || morphType == MORPH_TYPE_TILDE_BOTH )
    {
        status = initializeMorph( lang, MORPH_TYPE_TILDE );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    if ( lang != LNG_UNKNOWN && ( morphType == MORPH_TYPE_TILDE_REV || morphType == MORPH_TYPE_TILDE_BOTH ) )
    {
        status = initializeMorph( lang, MORPH_TYPE_TILDE_REV );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    // strict morph
    if ( lang != LNG_UNKNOWN )
    {
        if ( morphType == MORPH_TYPE_STRICT )
        {
            status = initializeMorph( lang, MORPH_TYPE_STRICT );
                if ( unlikely(status != LP_OK) )
                    return status;
        }
        else if ( morphType == MORPH_TYPE_ALL && checkLang( lang, strictLangs ) )
        {
            status = initializeMorph( lang, MORPH_TYPE_STRICT );
                if ( unlikely(status != LP_OK) )
                    return status;
        }
    }

    return LP_OK;
}

LingProcErrors LingProcData::initializeMorphCommonTop( LangCodes             lang,
                                                       MorphTypes            morphType,
                                                       LPMorphInterface    *&morph,
                                                       LangData            *&ld )
{
    // find lang data
    size_t index = getLangIndex( lang );
    if ( unlikely(index == static_cast<size_t>(-1)) )
        return LP_ERROR_UNSUPPORTED_LANG;

    ld = &langs[ index ];

    if ( morphType == MORPH_TYPE_STRICT )
        morph = ld->strict;
    else if ( morphType == MORPH_TYPE_FUZZY || morphType == MORPH_TYPE_HIER )
        morph = ld->fuzzy;
    else if ( morphType == MORPH_TYPE_TILDE )
        morph = ld->tilde;
    else if ( morphType == MORPH_TYPE_TILDE_REV )
        morph = ld->tildeRev;
    else
        return LP_ERROR_UNSUPPORTED_MORPH;

    // find attached storage for strict morph
    LingProcErrors status;
    if ( morphType == MORPH_TYPE_STRICT )
    {
        bool found = false;
        for ( size_t i = 0; i < attachedStrictMorphs.size(); i++ )
        {
            if ( attachedStrictMorphs[i].lang == lang )
            {
                status = morph->FSAttachStrictMorph( attachedStrictMorphs[i].fs );
                if ( unlikely(status != LP_OK) )
                    return status;
                found = true;
                break;
            }
        }

        if ( !found )
            return LP_ERROR_STRICT_DATA;
    }

    return LP_OK;
}

LingProcErrors LingProcData::initializeMorphCommonBottom( LangCodes            lang,
                                                          MorphTypes           morphType,
                                                          LPMorphInterface    *morph,
                                                          LangData            *ld )
{
    // find lang data
    size_t index = getLangIndex( lang );
    if ( unlikely(index == static_cast<size_t>(-1)) )
        return LP_ERROR_UNSUPPORTED_LANG;

    unsigned int internalNo = morph->internalNumber();
    if ( unlikely(internalNo >= lpMaxMorphsNum || morphs[ internalNo ].mp) )
        return LP_ERROR_UNSUPPORTED_MORPH;

    switch ( morphType )
    {
    case MORPH_TYPE_STRICT:
        ld->strictInternalNo = internalNo;
        break;
    case MORPH_TYPE_TILDE:
        ld->tildeInternalNo = internalNo;
        break;
    case MORPH_TYPE_TILDE_REV:
        ld->tildeRevInternalNo = internalNo;
        break;
    default:
        ld->fuzzyInternalNo = internalNo;
        break;
    };

    morphs[ internalNo ].mp        = morph;
    morphs[ internalNo ].langIndex = index;

    return LP_OK;
}

LingProcErrors LingProcData::initializeMorph( LangCodes lang, MorphTypes morphType )
{
    // init morph interface
    LPMorphInterface *morph;
    LangData         *ld;

    LingProcErrors status = initializeMorphCommonTop( lang, morphType, morph, ld );
    if ( unlikely(status != LP_OK) )
        return status;

    // find lang description
    const LanguageInfo *langDesc = LanguageInfo::getLanguageInfo( lang );
    if ( unlikely(langDesc == 0) )
        return LP_ERROR_UNSUPPORTED_LANG;

    // morph create params
    LPMorphCreateParams morphParams;
    memset( &morphParams, 0, sizeof(morphParams) );

    morphParams.lang = lang;
    morphParams.type = morphType;

    switch ( morphType )
    {
    case MORPH_TYPE_STRICT:
        morphParams.internalNumber = langDesc->strictNumber;
        break;
    case MORPH_TYPE_HIER:
        morphParams.internalNumber = MORPHNO_HIER;
        break;
    case MORPH_TYPE_TILDE:
        morphParams.internalNumber = langDesc->tildeNumber;
        break;
    case MORPH_TYPE_TILDE_REV:
        morphParams.internalNumber = langDesc->tildeRevNumber;
        break;
    default:
        morphParams.internalNumber = langDesc->fuzzyNumber;
        break;
    };

    morphParams.flags = morphFlags;
    morphParams.minStemLength = morphMinStemLength;
    morphParams.minCompoundStemLength = morphMinCompoundStemLength;

    if ( morphType == MORPH_TYPE_HIER && !(morphFlags & MORPH_FLAG_HIER_NATIVE_ONLY) )
        morphParams.flags &= ~MORPH_FLAG_NATIVE_ONLY;

    status = morph->create( ld->charset, &morphParams );
    if ( unlikely(status != LP_OK) )
        return status;

    return initializeMorphCommonBottom( lang, morphType, morph, ld );
}

LingProcErrors LingProcData::open( LingProcOpenModes           mode,
                                   fstorage                   *fs,
                                   size_t                      langsListSize,
                                   const LangCodes            *langsList,
                                   uint64_t                   &runtimeFlags,
                                   bool                       &runtimeFlagsAdjusted,
                                   bool                       &runtimeParamsLoaded,
                                   LingProc::EnabledMorphsSet &runtimeEnabledMorphs )
{
    if ( unlikely(mode != LP_MODE_RW && mode != LP_MODE_RO) )
        return LP_ERROR_EINVAL;

    LingProcErrors status = DeserializeHeader( fs );
    if ( unlikely(status != LP_OK) )
        return status;

    // set stored runtimes params
    if ( storedRuntimeParamsLoaded )
    {
        runtimeParamsLoaded = true;
        if ( !runtimeFlagsAdjusted )
            runtimeFlags = storedRuntimeFlags;
    }

    // generic
    status = initializeCommon();
    if ( unlikely(status != LP_OK) )
        return status;

    // LNG_UNKNOWN: generic charset and hier morph
    status = openLang( mode, LNG_UNKNOWN, fs );
    if ( unlikely(status != LP_OK) )
        return status;

    // rus-lat converter
    if( mode == LP_MODE_RW )
    {
        status = ruslatConv.Load( fs, LINGPROC_FS_SEC_RUSLAT );
        if ( unlikely(status != LP_OK) )
            return status;
    }
    else if ( mode == LP_MODE_RO )
    {
        status = ruslatConv.Connect( fs, LINGPROC_FS_SEC_RUSLAT );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    // phrase breaker
    if ( !pbreakerDataIsExternal && pbreakerData != 0 )
    {
        status = pbreakerData->Load( fs,  LINGPROC_FS_SEC_PBREAKER, &genericCharset );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    // languages
    if ( langsListSize >= 1 )
    {

        if ( langsList[ 0 ] == LNG_DEFAULT )
        {
            langsListSize = storedLangs.size();
            langsList     = storedLangs.get_buffer();
        }
        else if ( langsList[ 0 ] == LNG_ALL )
        {
            langsListSize = kAllLanguagesSize;
            langsList     = kAllLanguages;
        }

        for ( size_t i = 0; i < langsListSize; i++ )
        {
            LangCodes lang = langsList[ i ];

            if ( unlikely(lang == LNG_ALL || lang == LNG_DEFAULT) )
                return LP_ERROR_UNSUPPORTED_LANG;

            if ( unlikely(lang == LNG_UNKNOWN) )
                continue;

            // check for morph types intersection
            const LanguageInfo *currentLangInfo = LanguageInfo::getLanguageInfo( lang );
            if ( unlikely(currentLangInfo == 0) )
                return LP_ERROR_UNSUPPORTED_LANG;

            for ( size_t j = 1; j < langs.size(); j++ )
            {
                const LangData &ld = langs[ j ];

                const LanguageInfo *langInfo = LanguageInfo::getLanguageInfo( ld.lang );
                if ( unlikely(langInfo == 0) )
                    return LP_ERROR_UNSUPPORTED_LANG;

                switch ( morphType )
                {
                case MORPH_TYPE_FUZZY:
                    if ( unlikely(langInfo->fuzzyNumber == currentLangInfo->fuzzyNumber) )
                        return LP_ERROR_MORPHS_COLLISION;
                    break;

                case MORPH_TYPE_STRICT:
                case MORPH_TYPE_ALL:
                    if ( unlikely(langInfo->strictNumber == currentLangInfo->strictNumber) )
                        return LP_ERROR_MORPHS_COLLISION;
                    break;

                case MORPH_TYPE_TILDE:
                case MORPH_TYPE_TILDE_BOTH:
                    if ( unlikely(langInfo->tildeNumber    == currentLangInfo->tildeNumber ||
                                  langInfo->tildeRevNumber == currentLangInfo->tildeRevNumber ) )
                        return LP_ERROR_MORPHS_COLLISION;
                    break;

                default:
                    break;
                };
            }

            status = openLang( mode, lang, fs );
            if ( unlikely(status != LP_OK) )
                return status;
        }
    }

    verifyEnabledMorphs( runtimeEnabledMorphs );

    // open mode
    openMode = mode;

    return LP_OK;
}

LingProcErrors LingProcData::openLang( LingProcOpenModes mode, LangCodes lang, fstorage *fs )
{
    if ( unlikely(mode != LP_MODE_RW && mode != LP_MODE_RO) )
        return LP_ERROR_EINVAL;

    // check if lang is stored in opened storage
    if ( unlikely(!(lang == LNG_UNKNOWN || checkLang( lang, storedLangs ))) )
        return LP_ERROR_UNSUPPORTED_LANG;

    // find lang description
    const LanguageInfo *langDesc = LanguageInfo::getLanguageInfo( lang );
    if ( unlikely(langDesc == 0) )
        return LP_ERROR_UNSUPPORTED_LANG;

    // find or create lang data
    size_t index;
    LingProcErrors status = obtainLangIndex( lang, index );
    if ( unlikely(status != LP_OK) )
        return status;

    LangData &ld = langs[ index ];

    // tune hier charset if need to
    if ( lang == LNG_UNKNOWN && morphFlags & MORPH_FLAG_HIER_NATIVE_ONLY )
        charsetTuneMode |=  UCharSet::tmWordPartAsNative;
    else
        charsetTuneMode &= ~UCharSet::tmWordPartAsNative;

    // charset
    if ( mode == LP_MODE_RW )
    {
        status = ld.charset->Load( fs, LanguageInfo::getLPSectionCharset( langDesc->fsectionOffset ) );
        if ( unlikely(status != LP_OK) )
            return status;
    }
    else
    {
        status = ld.charset->Connect( fs, LanguageInfo::getLPSectionCharset( langDesc->fsectionOffset ) );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    // detect words
    if ( ( lang != LNG_UNKNOWN ) && ( ld.detector != 0 ) )
    {
        status = ld.detector->load( fs, LanguageInfo::getLPSectionDetect( langDesc->fsectionOffset ), *ld.charset );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    // fuzzy or hier morph
    if ( lang == LNG_UNKNOWN )
    {
        status = openMorph( mode, lang, fs, MORPH_TYPE_HIER );
        if ( unlikely(status != LP_OK) )
            return status;
    }
    else if ( morphType == MORPH_TYPE_FUZZY || morphType == MORPH_TYPE_ALL )
    {
        status = openMorph( mode, lang, fs, MORPH_TYPE_FUZZY );
        if ( unlikely(status != LP_OK) )
            return status;
    }
    else if ( morphType == MORPH_TYPE_TILDE || morphType == MORPH_TYPE_TILDE_BOTH )
    {
        status = openMorph( mode, lang, fs, MORPH_TYPE_TILDE );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    if ( lang != LNG_UNKNOWN && ( morphType == MORPH_TYPE_TILDE_REV || morphType == MORPH_TYPE_TILDE_BOTH ) )
    {
        status = openMorph( mode, lang, fs, MORPH_TYPE_TILDE_REV );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    // strict morph
    if ( lang != LNG_UNKNOWN )
    {
        if ( morphType == MORPH_TYPE_STRICT )
        {
            status = openMorph( mode, lang, fs, MORPH_TYPE_STRICT );
                if ( unlikely(status != LP_OK) )
                    return status;
        }
        else if ( morphType == MORPH_TYPE_ALL && checkLang( lang, strictLangs ) )
        {
            status = openMorph( mode, lang, fs, MORPH_TYPE_STRICT );
                if ( unlikely(status != LP_OK) )
                    return status;
        }
    }

    return LP_OK;
}

LingProcErrors LingProcData::openMorph( LingProcOpenModes mode, LangCodes lang, fstorage *fs, MorphTypes morphType )
{
    // init morph interface
    LPMorphInterface *morph;
    LangData         *ld;

    LingProcErrors status = initializeMorphCommonTop( lang, morphType, morph, ld );
    if ( unlikely(status != LP_OK) )
        return status;

    // find lang description
    const LanguageInfo *langDesc = LanguageInfo::getLanguageInfo( lang );
    if ( unlikely(langDesc == 0) )
        return LP_ERROR_UNSUPPORTED_LANG;

    fstorage_section_id secId;

    switch ( morphType )
    {
    case MORPH_TYPE_STRICT:
        secId = LanguageInfo::getLPSectionStrict( langDesc->fsectionOffset );
        break;

    case MORPH_TYPE_FUZZY:
    case MORPH_TYPE_HIER:
        secId = LanguageInfo::getLPSectionFuzzy( langDesc->fsectionOffset );
        break;

    case MORPH_TYPE_TILDE:
        secId = LanguageInfo::getLPSectionTilde( langDesc->fsectionOffset );
        break;

    case MORPH_TYPE_TILDE_REV:
        secId = LanguageInfo::getLPSectionTildeRev( langDesc->fsectionOffset );
        break;

    default:
        return LP_ERROR_UNSUPPORTED_MORPH;
    };

    if ( mode == LP_MODE_RW )
    {
        status = morph->FSOpenRW( ld->charset, fs, secId );
        if ( unlikely(status != LP_OK) )
            return status;
    }
    else if ( mode == LP_MODE_RO )
    {
        status = morph->FSOpenRO( ld->charset, fs, secId );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    return initializeMorphCommonBottom( lang, morphType, morph, ld );
}

LingProcErrors LingProcData::save( fstorage                   *fs,
                                   bool                        /* transport */,
                                   uint64_t                   &runtimeFlags,
                                   bool                       &/* runtimeFlagsAdjusted */,
                                   bool                       &/* runtimeParamsLoaded */,
                                   LingProc::EnabledMorphsSet &/* runtimeEnabledMorphs */ )
{
    if ( unlikely(fs == 0) )
        return LP_ERROR_EINVAL;

    if ( unlikely(openMode != LP_MODE_CREATE && openMode != LP_MODE_RW) )
        return LP_ERROR_INVALID_MODE;

    // prepared stored langs list
    storedLangs.clear();
    for ( size_t n = 0; n < langs.size(); n++ )
    {
        storedLangs.push_back( langs[ n ].lang );
        if ( unlikely(storedLangs.no_memory()) )
            return LP_ERROR_ENOMEM;
    }

    // prepare stored runtime params
    storedRuntimeFlags = runtimeFlags;

    // saving header
    LingProcErrors status = SerializeHeader( fs );
    if ( unlikely(status != LP_OK) )
        return status;

    // saving rus-lat converter
    status = ruslatConv.Save( fs, LINGPROC_FS_SEC_RUSLAT );
    if ( unlikely(status != LP_OK) )
        return status;

    // saving phrase breaker
    status = pbreakerData->Save( fs, LINGPROC_FS_SEC_PBREAKER );
    if ( unlikely(status != LP_OK) )
        return status;

    // saving languages
    for ( size_t n = 0; n < langs.size(); n++ )
    {
        const LanguageInfo *langDesc = LanguageInfo::getLanguageInfo( langs[ n ].lang );
        if ( unlikely(langDesc == 0) )
            return LP_ERROR_UNSUPPORTED_LANG;

        // charset
        if ( langs[ n ].charset != 0 )
        {
            fstorage_section_id secId = LanguageInfo::getLPSectionCharset( langDesc->fsectionOffset );
            status = langs[ n ].charset->Save( fs, secId, false );
            if ( unlikely(status != LP_OK) )
                return status;
        }

        // fuzzy
        if ( langs[ n ].lang == LNG_UNKNOWN || morphType == MORPH_TYPE_FUZZY || morphType == MORPH_TYPE_ALL )
        {
            if ( likely(langs[ n ].fuzzy != 0) )
            {
                fstorage_section_id secId = LanguageInfo::getLPSectionFuzzy( langDesc->fsectionOffset );
                status = langs[ n ].fuzzy->FSSave( fs, secId, false );
                if ( unlikely(status != LP_OK) )
                    return status;
            }
            else
                return LP_ERROR_EFAULT;
        }

        // tilde
        if ( langs[ n ].lang != LNG_UNKNOWN && (morphType == MORPH_TYPE_TILDE || morphType == MORPH_TYPE_TILDE_BOTH) )
        {
            if ( likely(langs[ n ].tilde != 0) )
            {
                fstorage_section_id secId = LanguageInfo::getLPSectionTilde( langDesc->fsectionOffset );
                status = langs[ n ].tilde->FSSave( fs, secId, false );
                if ( unlikely(status != LP_OK) )
                    return status;
            }
            else
                return LP_ERROR_EFAULT;
        }

        // tilde_rev
        if ( langs[ n ].lang != LNG_UNKNOWN && (morphType == MORPH_TYPE_TILDE_REV || morphType == MORPH_TYPE_TILDE_BOTH) )
        {
            if ( likely(langs[ n ].tildeRev != 0) )
            {
                fstorage_section_id secId = LanguageInfo::getLPSectionTildeRev( langDesc->fsectionOffset );
                status = langs[ n ].tildeRev->FSSave( fs, secId, false );
                if ( unlikely(status != LP_OK) )
                    return status;
            }
            else
                return LP_ERROR_EFAULT;
        }

        // strict
        if ( langs[ n ].lang != LNG_UNKNOWN && (morphType == MORPH_TYPE_STRICT || morphType == MORPH_TYPE_ALL) )
        {
            if ( unlikely(langs[ n ].strict == 0) )
                return LP_ERROR_EFAULT;

            if ( morphType == MORPH_TYPE_STRICT || checkLang( langs[ n ].lang, strictLangs ) )
            {
                fstorage_section_id secId = LanguageInfo::getLPSectionStrict( langDesc->fsectionOffset );
                status = langs[ n ].strict->FSSave( fs, secId, false );
                if ( unlikely(status != LP_OK) )
                    return status;
            }
        }

        // detect words
        if ( langs[ n ].lang != LNG_UNKNOWN )
        {
            if ( likely(langs[ n ].detector != 0) )
            {
                fstorage_section_id secId = LanguageInfo::getLPSectionDetect( langDesc->fsectionOffset );
                status = langs[ n ].detector->save( fs, secId );
                if ( unlikely(status != LP_OK) )
                    return status;
            }
        }
    }

    return LP_OK;
}

LingProcErrors LingProcData::flush()
{
    LingProcErrors globalStatus = LP_OK;

    for ( size_t n = 0; n < langs.size(); n++ )
    {
        if ( langs[ n ].fuzzy != 0 )
        {
            LingProcErrors status = langs[ n ].fuzzy->flush();
            if ( unlikely(status != LP_OK && globalStatus == LP_OK) )
                globalStatus = status;
        }

        if ( langs[ n ].tilde != 0 )
        {
            LingProcErrors status = langs[ n ].tilde->flush();
            if ( unlikely(status != LP_OK && globalStatus == LP_OK) )
                globalStatus = status;
        }

        if ( langs[ n ].tildeRev != 0 )
        {
            LingProcErrors status = langs[ n ].tildeRev->flush();
            if ( unlikely(status != LP_OK && globalStatus == LP_OK) )
                globalStatus = status;
        }
    }

    return globalStatus;
}

LingProcErrors LingProcData::MorphNo( LangCodes lang, MorphTypes type, unsigned int &no ) const
{
    size_t langIndex = getLangIndex( lang );
    if ( unlikely(langIndex == static_cast<size_t>(-1)) )
        return LP_ERROR_UNSUPPORTED_LANG;

    switch ( type )
    {
    case MORPH_TYPE_FUZZY:
        no = langs[ langIndex ].fuzzyInternalNo;
        break;

    case MORPH_TYPE_STRICT:
        no = langs[ langIndex ].strictInternalNo;
        break;

    case MORPH_TYPE_TILDE:
        no = langs[ langIndex ].tildeInternalNo;
        break;

    case MORPH_TYPE_TILDE_REV:
        no = langs[ langIndex ].tildeRevInternalNo;
        break;

    default:
        return LP_ERROR_UNSUPPORTED_MORPH;
    };

    return LP_OK;
}

bool LingProcData::IsCompoundModeOn( LangCodes lang ) const
{
    size_t langIndex = getLangIndex( lang );
    if ( langIndex == static_cast<size_t>(-1) )
        return false;

    LPMorphInterface *strictMorph = langs[ langIndex ].strict;
    if ( strictMorph == 0 )
        return false;

    if ( strictMorph->info()->creationFlags & MORPH_FLAG_PROCESS_COMPOUNDS )
        return true;

    return false;
}

LingProcErrors LingProcData::setExternalPhraseBreakerData( PhraseBreakerData &data )
{
    pbreakerData           = &data;
    pbreakerDataIsExternal = true;

    return LP_OK;
}

