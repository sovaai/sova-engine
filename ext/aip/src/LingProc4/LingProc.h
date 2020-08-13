#ifndef _LINGPROC_H_
#define _LINGPROC_H_

#include <stdlib.h>

#include <_include/_string.h>
#include <_include/_inttype.h>

#include "Constants.h"
#include "LanguageInfo.h"
#include "LingProcErrors.h"
#include "UCharSet/UCharSet.h"
#include "DocText/DocText.h"
#include "DocText/DocTextWordBreaker.h"
#include "DocImage/DocImage.h"
#include "SimpleMorph/LPMorphCommon.h"
#include "StrictMorphGram/StrictDictInfo.h"

#include "StringsReceiver.h"
#include "LemmatizeResultFunctor.h"
#include "StemmatizeResultFunctor.h"
#include "NormalizeResultFunctor.h"

class LingProcData;
struct LingProcParams;

enum LingProcOpenModes
{
    LP_MODE_CLOSED = 0,
    LP_MODE_CREATE,
    LP_MODE_RO,
    LP_MODE_RW
};

class LingProc
{
public:
    /* RUNTIME FLAGS */
    static const uint64_t fDetectLang       = 0x00000100; /* Use language detection */
    static const uint64_t fUseDetectedLangs = 0x00000200; /* Use detected langs for lemmatization;
                                                             ignored (considered as set) if fDetectLang is set */
    static const uint64_t fTillFirst        = 0x00000400; /* Do lemmatization till first match */
    static const uint64_t fForceHierHomo    = 0x00000800; /* Always generate hieroglyph homonym */
    static const uint64_t fRusLatConv       = 0x00001000; /* Proceed Rus/Lat symbols conversion */
    static const uint64_t fRusLatConvLat    = 0x00002000; /* Force 2Lat coversion (default) */
    static const uint64_t fRusLatConvRus    = 0x00004000; /* Force 2Rus coversion (default) */

    static const uint64_t fRusLatTRusLat    = 0x00008000; /* Turn on cyrillic to latin conversion table */
    static const uint64_t fRusLatTDigLat    = 0x00010000; /* Turn on digits to latin conversion table */
    static const uint64_t fRusLatTLatRus    = 0x00020000; /* Turn on latin to cyrillic conversion table */
    static const uint64_t fRusLatTDigRus    = 0x00040000; /* Turn on digits to cyrillic conversion table */

    static const uint64_t fRusLatDRusLat    = 0x00080000; /* Default conversion: cyrillic to latin */
    static const uint64_t fRusLatDLatRus    = 0x00100000; /* Default conversion: latin to cyrillic */
    static const uint64_t fRusLatCRusLat    = 0x00200000; /* Conflict conversion: cyrillic to latin */
    static const uint64_t fRusLatCLatRus    = 0x00400000; /* Conflict conversion: latin to cyrillic */

    static const uint64_t fRusLatNoDetect   = 0x00800000; /* Turn off conversion detection, default convertion is used */

    static const uint64_t fDocImageForms    = 0x01000000; /* Turn on word forms accumulations in DocImage when lemmatizating */

    static const uint64_t fEnclosingApos    = 0x02000000; /* Always treat word enclosing apostrophes as punctuation (parallel to quotation marks) */

    static const uint64_t fRusLatMask       = ( fRusLatConv    | fRusLatConvLat | fRusLatConvRus |
                                                fRusLatTRusLat | fRusLatTDigLat | fRusLatTLatRus | fRusLatTDigRus |
                                                fRusLatDRusLat | fRusLatDLatRus | fRusLatCRusLat | fRusLatCLatRus |
                                                fRusLatNoDetect );

    static const uint64_t defaultRuntimeFlags = fTillFirst;

    /* Used in DocText (DocTextWord) and DocImage */
    static const uint16_t fWordHasSpaceBefore  = 0x0001;
    static const uint16_t fWordHasSpaceAfter   = 0x0002;
    static const uint16_t fWordIsLast          = 0x0004;
    static const uint16_t fWordHasNewLineAfter = 0x0008;
    static const uint16_t fWordIsLastInLine    = 0x0010;
    static const uint16_t fWordIsLastInPara    = 0x0020;
    static const uint16_t fWordIsLastInPhrase  = 0x0040;

    typedef BinarySet< lpMaxLangNum >   EnabledLangs;
    typedef BinarySet< lpMaxMorphsNum > EnabledMorphsSet;
    typedef EnabledMorphsSet            DisabledMorphsSet;

    // DocText processing
    enum ParaBreakMode
    {
        PARA_BREAK_FIT_LINE,
        PARA_BREAK_SMART_DETECT
    };

public:
    /* construction */
    LingProc();
    LingProc( const LingProc &src );
    LingProc &operator=( const LingProc &src );
    virtual ~LingProc();

    bool IsValid() const { return shadow != 0; }

    // strict morph data, should be attached before create or open call
    LingProcErrors AttachStrictMorph( const char *path, LangCodes lang );
    LingProcErrors AttachStrictMorph( fstorage *fs, LangCodes lang );
    LingProcErrors AttachStrictMorph( fstorage *fs, size_t langsListSize, const LangCodes *langsList );
    void           DetachStrictMorphAll();

    // open/create - basic (list of languages accepted)
    LingProcErrors Create()
    {
        LangCodes lang = LNG_ALL;
        return Create( 0, 1, &lang );
    }

    LingProcErrors Create( const LingProcParams *params )
    {
        LangCodes lang = LNG_ALL;
        return Create( params, 1, &lang );
    }

    LingProcErrors Open( LingProcOpenModes mode, fstorage *fs )
    {
        LangCodes lang = LNG_DEFAULT;
        return Open( mode, fs, 1, &lang );
    }

    LingProcErrors Create( const LingProcParams *params, size_t langsListSize, const LangCodes *langsList );
    LingProcErrors Open( LingProcOpenModes mode, fstorage *fs, size_t langsListSize, const LangCodes *langsList );
    LingProcErrors Save( fstorage *fs, bool transport );
    LingProcErrors Flush();
    LingProcErrors Close();

public:
    struct WordFilter
    {
        virtual LingProcErrors filter( const char *word,
                                       size_t      wordSize,
                                       char       *resultBuffer,
                                       size_t      resultBufferSize,
                                       size_t     &resultSize ) = 0;
    };

public:
    // fills DocText with words and lines using standard breaker
    LingProcErrors WordBreakDoc( DocText &docText ) const;
    // fills DocText with words and lines using custom user-provided breaker
    LingProcErrors WordBreakDoc( DocText &docText, DocTextWordBreaker &breaker ) const;
    // does not fill DocText, but invokes user functor for each word found using standard breaker
    LingProcErrors WordBreakDoc( DocText &docText, DocTextWordBreaker::WordReceiver &wordReceiver ) const;

    LingProcErrors ParagraphBreakDoc( DocText &docText, ParaBreakMode mode ) const;
    LingProcErrors PhraseBreakDoc( DocText &docText ) const;
    LingProcErrors LangDetectDoc( DocText &docText ) const;
    LingProcErrors LemmatizeDoc( DocText &docText, DocImage &docImage, unsigned int morphFlags = MORPH_FLAG_USE_DEFAULT );

    LingProcErrors ConvertMarkup( const DocText &docText, DocImage &docImage ) const { return docImage.ConvertMarkup( docText ); }

    LingProcErrors LemmatizeWord(  LemmatizeResultFunctor &functor,
                                   const char             *word,
                                   size_t                  wlen          = static_cast<size_t>(-1),
                                   size_t                  langsListSize = 0,
                                   const LangCodes        *langsList     = 0 )
    {
        return LemmatizeWordEx( functor, word, wlen, MORPH_FLAG_USE_DEFAULT, langsListSize, langsList );
    }

    LingProcErrors LemmatizeWordEx(  LemmatizeResultFunctor &functor,
                                     const char             *word,
                                     size_t                  wlen          = static_cast<size_t>(-1),
                                     unsigned int            morphFlags    = MORPH_FLAG_USE_DEFAULT,
                                     size_t                  langsListSize = 0,
                                     const LangCodes        *langsList     = 0 );

    LingProcErrors StemmatizeWord( StemmatizeResultFunctor &functor,
                                   const char              *word,
                                   size_t                   wlen          = static_cast<size_t>(-1),
                                   size_t                   langsListSize = 0,
                                   const LangCodes         *langsList     = 0 )
    {
        return StemmatizeWordEx( functor, word, wlen, MORPH_FLAG_USE_DEFAULT, langsListSize, langsList );
    }

    LingProcErrors StemmatizeWordEx( StemmatizeResultFunctor &functor,
                                     const char              *word,
                                     size_t                   wlen          = static_cast<size_t>(-1),
                                     unsigned int             morphFlags    = MORPH_FLAG_USE_DEFAULT,
                                     size_t                   langsListSize = 0,
                                     const LangCodes         *langsList     = 0 );

    LingProcErrors NormalizeWord( NormalizeResultFunctor &functor,
                                  const char             *word,
                                  size_t                  wlen          = static_cast<size_t>(-1),
                                  size_t                  langsListSize = 0,
                                  const LangCodes        *langsList     = 0 );

    LingProcErrors LemmatizeWordWithMorph( LemmatizeResultFunctor &functor,
                                           const EnabledMorphsSet &enabledMorphs,
                                           const char             *word,
                                           size_t                  wlen = static_cast<size_t>(-1) );

    LingProcErrors LemmatizeWordExWithMorph( LemmatizeResultFunctor &functor,
                                             const EnabledMorphsSet &enabledMorphs,
                                             const char             *word,
                                             size_t                  wlen = static_cast<size_t>(-1),
                                             unsigned int            morphFlags = MORPH_FLAG_USE_DEFAULT );

    LingProcErrors StemmatizeWordWithMorph( StemmatizeResultFunctor &functor,
                                            const EnabledMorphsSet  &enabledMorphs,
                                            const char              *word,
                                            size_t                   wlen = static_cast<size_t>(-1) );

    LingProcErrors NormalizeWordWithMorph( NormalizeResultFunctor &functor,
                                           const EnabledMorphsSet &enabledMorphs,
                                           const char             *word,
                                           size_t                  wlen = static_cast<size_t>(-1) );

    LingProcErrors LemmatizeStem( LemmatizeResultFunctor &functor,
                                  LEXID                   partialLexid,
                                  const char             *stem )
    {
        return LemmatizeStemEx( functor, partialLexid, stem, MORPH_FLAG_USE_DEFAULT );
    }

    LingProcErrors LemmatizeStemEx( LemmatizeResultFunctor &functor,
                                    LEXID                   partialLexid,
                                    const char             *stem,
                                    unsigned int            morphFlags );

    LingProcErrors StemmatizeLexid( StemmatizeResultFunctor &functor, LEXID lex );


    LingProcErrors LexText( LEXID lex, StringsReceiver &textReceiver, const BinaryData *additionalData = 0 ) const;
    LingProcErrors LexText( LEXID partialLex, const char *stem, StringsReceiver &textReceiver, const BinaryData *additionalData = 0 ) const;
    LingProcErrors LexTextNormalForm( LEXID lex, StringsReceiver &textReceiver ) const
    {
        return LexText( _LexSetDictForm( lex ), textReceiver );
    }
    LingProcErrors LexTextNormalForm( LEXID partialLex, const char *stem, StringsReceiver &textReceiver ) const
    {
        return LexText( _LexSetDictForm(partialLex), stem, textReceiver );
    }
    LingProcErrors LexTextNormalized( LEXID lex, StringsReceiver &textReceiver ) const
    {
        return LexTextNormalForm( LexSetNoCase(lex), textReceiver );
    }
    LingProcErrors LexTextNormalized( LEXID partialLex, const char *stem, StringsReceiver &textReceiver ) const
    {
        return LexTextNormalForm( LexSetNoCase(partialLex), stem, textReceiver );
    }
    LingProcErrors LexTextLower( LEXID lex, StringsReceiver &textReceiver ) const
    {
        return LexTextNormalForm( LexSetLower(lex), textReceiver );
    }
    LingProcErrors LexTextLower( LEXID partialLex, const char *stem, StringsReceiver &textReceiver ) const
    {
        return LexTextNormalForm( LexSetLower(partialLex), stem, textReceiver );
    }
    MorphTypes            LexMorphType( LEXID lex ) const;
    LangCodes             LexLang( LEXID lex ) const;
    LingProcErrors        SetWordFilter( LangCodes lang, MorphTypes morph, WordFilter *filter );
    LingProcErrors        MorphNo( LangCodes lang, MorphTypes type, unsigned int &no ) const;
    bool                  IsCompoundModeOn( LangCodes lang ) const;
    bool                  IsLangEnabled( LangCodes lang ) const;
    LingProcErrors        resolveLPStrict( LEXID lex, LPStrict::Resolver &resolver ) const;
    const StrictDictInfo *getStrictDictInfo( LangCodes lang ) const;
    static LingProcErrors DumpLexid( LEXID lex, StringsReceiver &textReceiver );
    static LingProcErrors RestoreLexid( const char *buf, size_t bufSize, LEXID &lex );

public:
    // runtime flags (independent for each copy of LP)
    bool RuntimeParamsLoaded() const { return runtimeParamsLoaded; }

    uint64_t Flags() const { return runtimeFlags; }

    uint64_t SetFlags( uint64_t mask )
    {
        uint64_t oldFlags    = runtimeFlags;
        runtimeFlags         = mask;
        runtimeFlagsAdjusted = true;
        return oldFlags;
    }

    uint64_t AddFlags( uint64_t mask )
    {
        uint64_t oldFlags     = runtimeFlags;
        runtimeFlags         |= mask;
        runtimeFlagsAdjusted  = true;
        return oldFlags;
    }

    uint64_t ClearFlags( uint64_t mask )
    {
        uint64_t oldFlags     = runtimeFlags;
        runtimeFlags         &= ~mask;
        runtimeFlagsAdjusted  = true;
        return oldFlags;
    }

    // creation params access - read only
    uint32_t CharSetTuneMode() const;

    // properties and sub objects
    const UCharSet *GenericCharSet() const;
    const UCharSet *GetCharset( LangCodes lang ) const;

    // add specific flag to charset symbol properties (use LNG_UNKNOWN for generic charset)
    LingProcErrors TuneCharSetSymbolAdd( LangCodes lang, uint32_t ch, UCharSet::flags flag );

    // clears specific flag to charset symbol properties (use LNG_UNKNOWN for generic charset)
    LingProcErrors TuneCharSetSymbolClear( LangCodes lang, uint32_t ch, UCharSet::flags flag );

protected:
    /* Actual data is shared between all LP copies; to use LP in multithread mode
       we need to add Clone() functionality */
    LingProcData     *shadow;

    // config contains settings that may differ for different LP copies
    uint64_t          runtimeFlags;
    bool              runtimeFlagsAdjusted;
    bool              runtimeParamsLoaded;
    EnabledMorphsSet  enabledMorphs;
};

struct LingProcParams
{
    MorphTypes                  morphType;          /* MORPH_TYPE_FUZZY or MORPH_TYPE_STRICT;
                                                     * zero if both types needed */
    unsigned int                morphFlags;         /* Morph creation flags; also see below */
    bool                        useMorphFlags;      /* If is true moprh_flags is used to create
                                                     * morphologies, else default LP moprh_flags
                                                     * is used */
    unsigned int                morphMinStemLength; /* Unless zero, overrides default morph
                                                       'minStemLength' value */
    unsigned int                morphMinCompoundStemLength; /* Unless zero, overrides default morph
                                                               'minCompoundStemLength' value */
    uint32_t                    charsetTuneMode;    /* All charsets tune mode */
    unsigned int                ruslatConvTuneMode; /* All rus-lat converter tune mode */
    LingProc::DisabledMorphsSet disabledMorphs;     /* Disabled morphologies */

    LingProcParams() :
        morphType( MORPH_TYPE_INVALID ),
        morphFlags( 0 ),
        useMorphFlags( false ),
        morphMinStemLength( 0 ),
        morphMinCompoundStemLength( 0 ),
        charsetTuneMode( 0 ),
        ruslatConvTuneMode( 0 ),
        disabledMorphs()
    {
    }
};

#endif /* _LINGPROC_H_ */

