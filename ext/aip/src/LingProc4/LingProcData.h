#ifndef _LINGPROCDATA_H_
#define _LINGPROCDATA_H_

#include <assert.h>
#include <stdlib.h>

#include <_include/language.h>
#include <_include/cc_compat.h>

#include <lib/fstorage/fstorage.h>
#include <lib/fstorage/fstorage_id.h>

#include "UCharSet/UCharSetFS.h"
#include "RusLatConv/RusLatConvFS.h"
#include "PhraseBreak/PhraseBreakerDict.h"

#include "SimpleMorph/LPMorphInterface.h"
#include "SimpleMorph/LPMorphPunct.h"
#include "SimpleMorph/LPMorphNumber.h"
#include "SimpleMorph/LPMorphFuzzy.h"
#include "SimpleMorph/LPMorphStrict.h"
#include "SimpleMorph/LPMorphTilde.h"
#include "SimpleMorph/LPMorphTildeRev.h"

#include "Process/LangDetector/LangDetector.h"

#include "LingProc.h"

#define LINGPROC_DEFAULT_MORPH_FLAGS MORPH_FLAG_USE_DEFAULT

#define LINGPROC_FS_HEADER_FIXED_SIZE (sizeof(uint32_t) * 6 + sizeof(uint16_t) * 1)

#define LINGPROC_FS_MAGIC                     0xB0013896u
#define LINGPROC_FS_WORK_FORMAT_VERSION       2
#define LINGPROC_FS_WORK_FORMAT_VARIANT       0

/* fstorage sections */
#define LINGPROC_FS_SEC_HEADER     (FSTORAGE_SECTION_LINGPROC)
#define LINGPROC_FS_SEC_RUSLAT     (FSTORAGE_SECTION_LINGPROC + 0x10)
#define LINGPROC_FS_SEC_PBREAKER   (FSTORAGE_SECTION_LINGPROC + 0x20)

struct ProcessFunctor;

class LingProcData
{
private:
    /*  copy prevention */
    LingProcData( const LingProcData & ) { assert(false); abort(); }
    LingProcData &operator=( const LingProcData & ) { assert(false); abort(); return *this; }

public:
    static const LangCodes  kAllLanguages[];
    static const int        kAllLanguagesSize;

    LingProcData() :
        refCount( 1 ),
        openMode( LP_MODE_CLOSED ),
        morphType( MORPH_TYPE_DEFAULT ),
        morphFlags( 0 ),
        charsetTuneMode( 0 ),
        morphMinStemLength( 0 ),
        morphMinCompoundStemLength( 0 ),
        storedRuntimeFlags( 0 ),
        storedRuntimeParamsLoaded( false ),
        disabledMorphs(),
        storedLangs(),
        strictFs( 0 ),
        attachedStrictMorphs(),
        strictLangs(),
        genericCharset( LNG_UNKNOWN ),
        morphPunct(),
        morphNumeric(),
        ruslatConv(),
        pbreakerData( 0 ),
        pbreakerDataInternal(),
        pbreakerDataIsExternal( false ),
        langs(),
        morphs()
    {
        pbreakerData = &pbreakerDataInternal;
    }

    ~LingProcData()
    {
        UninstallStrictMorphFS();
    }

    LingProcErrors InstallStrictMorphFS( fstorage *strictFs )
    {
        LingProcErrors status = UninstallStrictMorphFS();
        if ( unlikely(status != LP_OK) )
            return status;

        this->strictFs = strictFs;
        return LP_OK;
    }

    LingProcErrors UninstallStrictMorphFS()
    {
        if ( unlikely(strictFs != 0) )
        {
            int status = fstorage_close( strictFs );
            fstorage_destroy( strictFs );
            strictFs = 0;

            if ( unlikely(status != 0) )
                return LP_ERROR_STRICT_DATA_FS;
        }

        return LP_OK;
    }

    const LPMorphInterface *getMorph( unsigned int morphNo ) const
    {
        if ( morphNo < lpMaxMorphsNum )
            return morphs[ morphNo ].mp;
        return 0;
    }

    bool isClosed() const { return openMode == LP_MODE_CLOSED; }

public:
    struct AttachedStrictMorph
    {
        LangCodes  lang;
        fstorage  *fs;
    };

    const AttachedStrictMorph *findAttachedStrictMorph( LangCodes lang ) const
    {
        for ( size_t i = 0; i < attachedStrictMorphs.size(); i++ )
            if ( attachedStrictMorphs[ i ].lang == lang )
                return &attachedStrictMorphs[ i ];
        return 0;
    }

    LingProcErrors newAttachedStrictMorph( LangCodes lang, fstorage *fs )
    {
        if ( unlikely(attachedStrictMorphs.size() >= lpMaxLangNum) )
            return LP_ERROR_TOO_MANY_LANGS;
        if ( unlikely(findAttachedStrictMorph( lang ) != 0) )
            return LP_ERROR_UNSUPPORTED_LANG;
        AttachedStrictMorph *sm = attachedStrictMorphs.grow();
        if ( unlikely(sm == 0) )
            return LP_ERROR_ENOMEM;
        sm->lang = lang;
        sm->fs   = fs;
        return LP_OK;
    }

    void detachStrictMorphs()
    {
        attachedStrictMorphs.clear();
        UninstallStrictMorphFS();
    }

    LingProcErrors initialize( const LingProcParams       *params,
                               size_t                      langsListSize,
                               const LangCodes            *langsList,
                               LingProc::EnabledMorphsSet &runtimeEnabledMorphs );
    LingProcErrors open( LingProcOpenModes           mode,
                         fstorage                   *fs,
                         size_t                      langsListSize,
                         const LangCodes            *langsList,
                         uint64_t                   &runtimeFlags,
                         bool                       &runtimeFlagsAdjusted,
                         bool                       &runtimeParamsLoaded,
                         LingProc::EnabledMorphsSet &runtimeEnabledMorphs );
    LingProcErrors save( fstorage                   *fs,
                         bool                        transport,
                         uint64_t                   &runtimeFlags,
                         bool                       &runtimeFlagsAdjusted,
                         bool                       &runtimeParamsLoaded,
                         LingProc::EnabledMorphsSet &runtimeEnabledMorphs );

    LingProcErrors flush();

public:
    void           WordBreakDocPrepare( DocText &docText ) const;
    LingProcErrors WordBreakDocFinish( DocText &docText ) const;
    LingProcErrors ParagraphBreakDocLine( DocText &docText ) const;
    LingProcErrors ParagraphBreakDocSmart( DocText &docText ) const;
    LingProcErrors PhraseBreakDoc( DocText &docText ) const;
    LingProcErrors LangDetectDoc( DocText &docText, uint64_t runtimeFlags ) const;
    LingProcErrors LangDetectDocReal( DocText &docText ) const;
    LingProcErrors LemmatizeDoc( DocText &docText,
                                 DocImage &docImage,
                                 unsigned int morphFlags,
                                 uint64_t runtimeFlags,
                                 const LingProc::EnabledMorphsSet &runtimeEnabledMorphs );

    LingProcErrors LexText( LEXID lex, StringsReceiver &textReceiver, const BinaryData *additionalData ) const;
    LingProcErrors LexText( LEXID partialLex, const char *stem, StringsReceiver &textReceiver, const BinaryData *additionalData ) const;
    LingProcErrors SetWordFilter( LangCodes lang, MorphTypes morph, LingProc::WordFilter *filter );
    LingProcErrors MorphNo( LangCodes lang, MorphTypes type, unsigned int &no ) const;
    MorphTypes     LexMorphType( LEXID lex ) const;
    LangCodes      LexLang( LEXID lex ) const;

    bool IsCompoundModeOn( LangCodes lang ) const;

    bool IsLangEnabled( LangCodes lang ) const
    {
        return ( getLangIndex( lang ) == static_cast<size_t>( -1 ) ? false : true );
    }

public:
    static LingProcErrors Create( LingProcData *&pt)
    {
        Release( pt );
        pt = new LingProcData;
        if ( unlikely(pt == 0) )
            return LP_ERROR_ENOMEM;

        return LP_OK;
    }

    static void CreateCopy( LingProcData *&dest, LingProcData * const &src )
    {
        Release( dest );

        dest = src;

        if ( unlikely(dest != 0) )
            dest->refCount++;
    }

    static void Release( LingProcData *&pt )
    {
        if ( pt != 0 && --(pt->refCount) == 0 )
            delete pt;

        pt = 0;
    }

protected:
    UCharSet *locateCharset( LangCodes lang, bool genericAsLngUnknown = false )
    {
        if ( lang == LNG_UNKNOWN )
            return ( genericAsLngUnknown ? &genericCharset : 0 );

        size_t idx = getLangIndex( lang );
        if ( unlikely(idx == static_cast<size_t>(-1)) )
            return 0;
        
        return langs[ idx ].charset;
    }
    
public:
    uint32_t getCharsetTuneMode() const { return charsetTuneMode; }
    const UCharSet *getGenericCharset() const { return &genericCharset; }
    const UCharSet *getCharset( LangCodes lang ) const
    {
        return const_cast<LingProcData *>( this )->locateCharset( lang );
    }

    LingProcErrors charsetSymbolAdd( LangCodes lang, uint32_t ch, UCharSet::flags flag )
    {
        UCharSet *charset = locateCharset( lang, true );
        if ( unlikely(charset == 0) )
            return LP_ERROR_UNSUPPORTED_LANG;

        charset->tunePropsAdd( ch, flag );
        
        return LP_OK;
    }

    LingProcErrors charsetSymbolClear( LangCodes lang, uint32_t ch, UCharSet::flags flag )
    {
        UCharSet *charset = locateCharset( lang, true );
        if ( unlikely(charset == 0) )
            return LP_ERROR_UNSUPPORTED_LANG;

        charset->tunePropsClear( ch, flag );
        
        return LP_OK;
    }
    
    LingProcErrors setExternalPhraseBreakerData( PhraseBreakerData &data );

private:
    size_t refCount;

protected:
    struct LangData;

    LingProcErrors initializeCommon();
    LingProcErrors initializeLang( LangCodes lang );
    LingProcErrors initializeMorph( LangCodes lang, MorphTypes morphType );
    LingProcErrors initializeMorphCommonTop( LangCodes             lang,
                                             MorphTypes            morphType,
                                             LPMorphInterface    *&morph,
                                             LangData            *&ld );
    LingProcErrors initializeMorphCommonBottom( LangCodes            lang,
                                                MorphTypes           morphType,
                                                LPMorphInterface    *morph,
                                                LangData            *ld );
    LingProcErrors openLang( LingProcOpenModes mode, LangCodes lang, fstorage *fs );
    LingProcErrors openMorph( LingProcOpenModes mode, LangCodes lang, fstorage *fs, MorphTypes morphType );
    void           verifyEnabledMorphs( LingProc::EnabledMorphsSet &runtimeEnabledMorphs );
    LingProcErrors DeserializeHeader( fstorage *fs );
    LingProcErrors SerializeHeader( fstorage *fs ) const;

    size_t getLangIndex( LangCodes lang ) const
    {
        for ( size_t i = 0; i < langs.size(); i++ )
            if ( langs[i].lang == lang )
                return i;
        return static_cast<size_t>( -1 );
    }

    LingProcErrors obtainLangIndex( LangCodes lang, size_t &index );

    bool checkLang( LangCodes lang, const avector<LangCodes> &vec ) const
    {
        for ( size_t i = 0; i < vec.size(); i++ )
            if ( vec[i] == lang )
                return true;
        return false;
    }

public:
    LingProcErrors MakeEnabledLangs( size_t                  langsListSize,
                                     const LangCodes        *langsList,
                                     LingProc::EnabledLangs &langs ) const
    {
        if ( langsListSize == 0 || ( langsListSize == 1 && langsList[ 0 ] == LNG_ALL ) )
            langs.setAll();
        else
            for ( size_t i = 0; i < langsListSize; i++ )
            {
                size_t langIndex = getLangIndex( langsList[ i ] );
                if ( unlikely(langIndex == static_cast<size_t>(-1)) )
                    return LP_ERROR_UNSUPPORTED_LANG;
                langs.set( langIndex );
            }
        return LP_OK;
    }

    LingProcErrors ProcessWord( ProcessFunctor                   &functor,
                                const char                       *word,
                                size_t                            wlen,
                                unsigned int                      morphFlags,
                                const LingProc::EnabledLangs     &enabledLangs,
                                const LingProc::EnabledMorphsSet &enabledMorphs,
                                uint64_t                          runtimeFlags,
                                const LingProc::EnabledMorphsSet &runtimeEnabledMorphs );

protected:
    int ProcessWordLang( size_t                            langIndex,
                         ProcessFunctor                   &functor,
                         const char                       *word,
                         size_t                            wlen,
                         unsigned int                      morphFlags,
                         const LingProc::EnabledMorphsSet &enabledMorphs,
                         const LingProc::EnabledMorphsSet &runtimeEnabledMorphs );

public:
    static size_t GetFilterMorphIndex( MorphTypes morph )
    {
        switch ( morph )
        {
        case MORPH_TYPE_HIER:      return 0;
        case MORPH_TYPE_FUZZY:     return 1;
        case MORPH_TYPE_STRICT:    return 2;
        case MORPH_TYPE_TILDE:     return 3;
        case MORPH_TYPE_TILDE_REV: return 4;

        default:                   return -1;
        }
    }

    static LingProc::WordFilter *GetFilterMorph( LingProc::WordFilter **wordFilters, MorphTypes morph )
    {
        size_t index = GetFilterMorphIndex( morph );
        if ( index == static_cast<size_t>(-1) )
            return 0;
        return wordFilters[ index ];
    }

protected:
    LingProcOpenModes openMode;
    MorphTypes        morphType;
    uint32_t          morphFlags;
    uint32_t          charsetTuneMode;

    unsigned int morphMinStemLength;
    unsigned int morphMinCompoundStemLength;

    /* Stored runtime params */
    uint64_t          storedRuntimeFlags;
    bool              storedRuntimeParamsLoaded;

    // disabled morphs
    LingProc::DisabledMorphsSet disabledMorphs;
    avector<LangCodes>          storedLangs;

    fstorage          *strictFs;

    /* Attached strict morphs table */
    avector<AttachedStrictMorph> attachedStrictMorphs;
    avector<LangCodes>           strictLangs;

    /* Generic charset */
    UCharSetFS    genericCharset;

    /* Generic morphologies */
    LPMorphPunct  morphPunct;
    LPMorphNumber morphNumeric;

    // rus-lat converter
    RusLatConverterFS  ruslatConv;

    // phrase breaker
    PhraseBreakerData *pbreakerData;
    PhraseBreakerData  pbreakerDataInternal;
    bool               pbreakerDataIsExternal;

    // languages
    struct LangData
    {
        LangCodes             lang;
        UCharSetFS           *charset;
        LangDetector         *detector;
        LPMorphFuzzy         *fuzzy;
        unsigned int          fuzzyInternalNo;
        LPMorphInterface     *strict;
        unsigned int          strictInternalNo;
        LPMorphTilde         *tilde;
        unsigned int          tildeInternalNo;
        LPMorphTildeRev      *tildeRev;
        unsigned int          tildeRevInternalNo;

        LingProc::WordFilter *wordFilters[5];
        bool                  dirty;

        LangData() :
            lang( LNG_UNKNOWN ),
            charset( 0 ),
            detector( 0 ),
            fuzzy( 0 ),
            fuzzyInternalNo( MORPHNO_CTRL ),
            strict( 0 ),
            strictInternalNo( MORPHNO_CTRL ),
            tilde( 0 ),
            tildeInternalNo( MORPHNO_CTRL ),
            tildeRev( 0 ),
            tildeRevInternalNo( MORPHNO_CTRL ),
            dirty( false )
        {
            memset( &wordFilters, 0, sizeof(LingProc::WordFilter *) * 5 );
        }

        ~LangData() { drop(); }

        void drop()
        {
            if ( charset != 0 && lang != LNG_UNKNOWN )
                delete charset;
            if ( detector != 0 )
                delete detector;
            if ( fuzzy != 0 )
                delete fuzzy;
            if ( strict != 0 )
                delete strict;
            if ( tilde != 0 )
                delete tilde;
            if ( tildeRev != 0 )
                delete tildeRev;
            charset  = 0;
            detector = 0;
            fuzzy    = 0;
            strict   = 0;
            tilde    = 0;
            tildeRev = 0;
        }
    };
    avector<LangData> langs;

    // morphs reference table
    struct MorphRef
    {
        MorphRef() : mp( 0 ), langIndex( static_cast<size_t>(-1) ) { }

        LPMorphInterface *mp;
        size_t            langIndex;
    } morphs[ lpMaxMorphsNum ];
};

#endif /* _LINGPROCDATA_H_ */

