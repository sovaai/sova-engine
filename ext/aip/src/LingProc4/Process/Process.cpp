
#include <_include/cc_compat.h>

#include "../LingProc.h"
#include "../LingProcData.h"

LingProcErrors LingProc::LemmatizeDoc( DocText &docText, DocImage &docImage, unsigned int morphFlags )
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;

    // ensure word break
    if ( !docText.checkWordBreak() )
    {
        LingProcErrors status = WordBreakDoc( docText );
        if ( unlikely(status != LP_OK) )
            return status;
    }
    
    return shadow->LemmatizeDoc( docText, docImage, morphFlags, runtimeFlags, enabledMorphs );
}

struct ProcessFunctor
{
    virtual int process( LPMorphInterface *i, const char *word, size_t wlen, unsigned int morphFlags ) = 0;
    virtual LingProcErrors reset() = 0;
};

class LemmatizeProcessFunctor : public ProcessFunctor
{
public:
    LemmatizeProcessFunctor( LemmatizeResultFunctor &_functor ) : ProcessFunctor(), functor( _functor ) { }
    int process( LPMorphInterface *i, const char *word, size_t wlen, unsigned int morphFlags )
    {
        return i->lemmatize( functor, word, wlen, morphFlags );
    }
    LingProcErrors reset() { return functor.reset(); }

private:
    LemmatizeResultFunctor &functor;
};

class StemmatizeProcessFunctor : public ProcessFunctor
{
public:
    StemmatizeProcessFunctor( StemmatizeResultFunctor &_functor ) : ProcessFunctor(), functor( _functor ) { }
    int process( LPMorphInterface *i, const char *word, size_t wlen, unsigned int morphFlags )
    {
        return i->stemmatize( functor, word, wlen, morphFlags );
    }
    LingProcErrors reset() { return functor.reset(); }
    
private:
    StemmatizeResultFunctor &functor;
};

class NormalizeProcessFunctor : public ProcessFunctor
{
public:
    NormalizeProcessFunctor( NormalizeResultFunctor &_functor ) : ProcessFunctor(), functor( _functor ) { }
    int process( LPMorphInterface *i, const char *word, size_t wlen, unsigned int morphFlags )
    {
        return i->normalize( functor, word, wlen, morphFlags );
    }
    LingProcErrors reset() { return functor.reset(); }

private:
    NormalizeResultFunctor &functor;
};

inline LingProcErrors maybeRunWordFilter( LingProc::WordFilter **wordFilters,
                                          MorphTypes             morph,
                                          char                  *buffer,
                                          size_t                 bufferSize,
                                          const char            *word,
                                          size_t                 wlen,
                                          const char           *&wordFixed,
                                          size_t                &wlenFixed )
{    
    wordFixed = word;
    wlenFixed = wlen;
    
    LingProc::WordFilter *wf = LingProcData::GetFilterMorph( wordFilters, morph );
    if ( wf != 0 )
    {
        wordFixed = buffer;

        return wf->filter( word, wlen, buffer, bufferSize, wlenFixed );
    }
    
    return LP_OK;
}

inline LingProcErrors SimpleProcess( ProcessFunctor  &functor,
                                     LingProcData    *shadow,
                                     size_t           langsListSize,
                                     const LangCodes *langsList,
                                     const char      *word,
                                     size_t           wlen,
                                     unsigned int     morphFlags,
                                     uint64_t         runtimeFlags,
                                     const LingProc::EnabledMorphsSet &runtimeEnabledMorphs )
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;
    if ( unlikely(langsListSize != 0 && langsList == 0) )
        return LP_ERROR_EINVAL;
    if ( unlikely(langsListSize == 0 && langsList != 0) )
        return LP_OK;

    LingProc::EnabledLangs langs;
    LingProcErrors status = shadow->MakeEnabledLangs( langsListSize, langsList, langs );
    if ( unlikely(status != LP_OK) )
        return status;

    status = functor.reset();
    if ( unlikely(status != LP_OK) )
        return status;

    return shadow->ProcessWord( functor, word, wlen, morphFlags, langs, LingProc::EnabledMorphsSet().setAll(), runtimeFlags, runtimeEnabledMorphs );
}                                     

LingProcErrors LingProc::LemmatizeWordEx(  LemmatizeResultFunctor &functor,
                                           const char             *word,
                                           size_t                  wlen,
                                           unsigned int            morphFlags,
                                           size_t                  langsListSize,
                                           const LangCodes        *langsList )
{
    LemmatizeProcessFunctor pFunctor( functor );
    return SimpleProcess( pFunctor, shadow, langsListSize, langsList, word, wlen, morphFlags, runtimeFlags, enabledMorphs );
}
    
LingProcErrors LingProc::StemmatizeWordEx( StemmatizeResultFunctor &functor,
                                           const char              *word,
                                           size_t                   wlen,
                                           unsigned int             morphFlags,
                                           size_t                   langsListSize,
                                           const LangCodes         *langsList )
{
    StemmatizeProcessFunctor pFunctor( functor );
    return SimpleProcess( pFunctor, shadow, langsListSize, langsList, word, wlen, morphFlags, runtimeFlags, enabledMorphs );
}
    
LingProcErrors LingProc::NormalizeWord( NormalizeResultFunctor &functor,
                                        const char             *word,
                                        size_t                  wlen,
                                        size_t                  langsListSize,
                                        const LangCodes        *langsList )
{
    NormalizeProcessFunctor pFunctor( functor );
    return SimpleProcess( pFunctor, shadow, langsListSize, langsList, word, wlen, MORPH_FLAG_USE_DEFAULT, runtimeFlags, enabledMorphs );
}

LingProcErrors LingProc::LemmatizeWordWithMorph( LemmatizeResultFunctor &functor,
                                                 const EnabledMorphsSet &enabledMorphs,
                                                 const char             *word,
                                                 size_t                  wlen )
{
    LemmatizeProcessFunctor pFunctor( functor );
    return SimpleProcess( pFunctor, shadow, 0, 0, word, wlen, MORPH_FLAG_USE_DEFAULT, runtimeFlags, enabledMorphs );
}
    
LingProcErrors LingProc::LemmatizeWordExWithMorph( LemmatizeResultFunctor &functor,
                                                   const EnabledMorphsSet &enabledMorphs,
                                                   const char             *word,
                                                   size_t                  wlen,
                                                   unsigned int            morphFlags )
{
    LemmatizeProcessFunctor pFunctor( functor );
    return SimpleProcess( pFunctor, shadow, 0, 0, word, wlen, morphFlags, runtimeFlags, enabledMorphs );
}
    
LingProcErrors LingProc::StemmatizeWordWithMorph( StemmatizeResultFunctor &functor,
                                                  const EnabledMorphsSet  &enabledMorphs,
                                                  const char              *word,
                                                  size_t                  wlen )
{
    StemmatizeProcessFunctor pFunctor( functor );
    return SimpleProcess( pFunctor, shadow, 0, 0, word, wlen, MORPH_FLAG_USE_DEFAULT, runtimeFlags, enabledMorphs );
}
    
LingProcErrors LingProc::NormalizeWordWithMorph( NormalizeResultFunctor &functor,
                                                 const EnabledMorphsSet &enabledMorphs,
                                                 const char             *word,
                                                 size_t                 wlen )
{
    NormalizeProcessFunctor pFunctor( functor );
    return SimpleProcess( pFunctor, shadow, 0, 0, word, wlen, MORPH_FLAG_USE_DEFAULT, runtimeFlags, enabledMorphs );
}
   
LingProcErrors LingProc::LemmatizeStemEx( LemmatizeResultFunctor &functor,
                                          LEXID                   partialLexid,
                                          const char             *stem,
                                          unsigned int            morphFlags )
{
    BufferReceiver< lpMaxWordLength + 1> buffer;
    
    LingProcErrors status = LexText( partialLexid, stem, buffer );
    if ( unlikely(status != LP_OK) )
        return status;

    class FilterFunctor : public LemmatizeResultFunctor
    {
    public:
        FilterFunctor( LemmatizeResultFunctor &_target, LEXID _filter ) : target( _target ), filter( _filter ) { }

	LingProcErrors reset() { return target.reset(); }
        
	LingProcErrors nextLemma( LEXID lexid, size_t compoundNumber, const LPStrict::Info *info )
        {
            if ( LexId(lexid) == LexId(filter) )
                return target.nextLemma( lexid, compoundNumber, info );
            return LP_OK;
        }
        
    private:
        LemmatizeResultFunctor &target;
        LEXID                   filter;
    } filterFunctor( functor, partialLexid );

    
    uint64_t oldRuntimeFlags = runtimeFlags;
    ClearFlags( fRusLatMask );
    status = LemmatizeWordExWithMorph( ( LexId(partialLexid) == 0 ? functor : filterFunctor ),
                                       EnabledMorphsSet().clear().set( LexNoMrph(partialLexid) ),
                                       buffer.getBuffer(),
                                       buffer.size(),
                                       morphFlags );
    SetFlags( oldRuntimeFlags );
    return status; 
}

LingProcErrors LingProc::StemmatizeLexid( StemmatizeResultFunctor &functor, LEXID lex )
{
    BufferReceiver< lpMaxWordLength + 1> buffer;
    
    LingProcErrors status = LexText( lex, buffer );
    if ( unlikely(status != LP_OK) )
        return status;

    class FilterFunctor : public StemmatizeResultFunctor
    {
    public:
        FilterFunctor( StemmatizeResultFunctor &_target, LEXID _filter ) : target( _target ), filter( _filter ) { }
        
	LingProcErrors reset() { return target.reset(); }

	char *getStemBuffer( size_t stemSize ) { return target.getStemBuffer( stemSize ); }
        
	LingProcErrors nextStem( LEXID partialLexid, size_t compoundNumber, const char *stem, size_t stemLength, const LPStrict::Info *info )
        {
            if ( LexId(partialLexid) == 0 || LexId(partialLexid) == LexId(filter) )
                return target.nextStem( partialLexid, compoundNumber, stem, stemLength, info );
            return LP_OK;
        }        
        
    private:
        StemmatizeResultFunctor &target;
        LEXID                    filter;
    } filterFunctor( functor, lex );
    
    uint64_t oldRuntimeFlags = runtimeFlags;
    ClearFlags( fRusLatMask );
    status = StemmatizeWordWithMorph( filterFunctor,
                                      EnabledMorphsSet().clear().set( LexNoMrph(lex) ),
                                      buffer.getBuffer(),
                                      buffer.size() );
    SetFlags( oldRuntimeFlags );
    return status; 
}

LingProcErrors LingProc::LexText( LEXID lex, StringsReceiver &textReceiver, const BinaryData *additionalData ) const
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;

    return shadow->LexText( lex, textReceiver, additionalData );
}

LingProcErrors LingProcData::LexText( LEXID lex, StringsReceiver &textReceiver, const BinaryData *additionalData ) const
{
    char *buffer = textReceiver.requestBuffer( lpMaxWordLength + 1 );
    if ( unlikely(buffer == 0) )
        return LP_ERROR_ENOMEM;

    if ( unlikely(lex == LEXNONE || lex == LEXINVALID) )
    {
        buffer[0] = '\0';
        textReceiver.commitString( 1 );
        return LP_OK;
    }

    size_t morphNo = LexNoMrph( lex );
    
    if ( morphs[ morphNo ].mp != 0 )
    {
        if ( LexIdForm(lex) == 0 && morphs[ morphNo ].mp->type() == MORPH_TYPE_STRICT )
            lex = _LexSetDictForm( lex );

        int result = morphs[ morphNo ].mp->getLexText( lex, buffer, lpMaxWordLength + 1, additionalData );
        if ( unlikely(result < 0) )
            return static_cast<LingProcErrors>(result);
        textReceiver.commitString( static_cast<size_t>(result) );
        return LP_OK;
    }
    else
    {
        buffer[0] = '\0';
        textReceiver.commitString( 1 );
        return LP_ERROR_ERANGE;
    }
}

LingProcErrors LingProc::LexText( LEXID partialLex, const char *stem, StringsReceiver &textReceiver, const BinaryData *additionalData ) const
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;

    return shadow->LexText( partialLex, stem, textReceiver, additionalData );
}

LingProcErrors LingProcData::LexText( LEXID partialLex, const char *stem, StringsReceiver &textReceiver, const BinaryData *additionalData ) const
{
    char *buffer = textReceiver.requestBuffer( lpMaxWordLength + 1 );
    if ( unlikely(buffer == 0) )
        return LP_ERROR_ENOMEM;

    if ( unlikely(partialLex == LEXINVALID) )
    {
        buffer[0] = '\0';
        textReceiver.commitString( 1 );
        return LP_OK;
    }

    size_t morphNo = LexNoMrph( partialLex );
    
    if ( morphs[ morphNo ].mp != 0 )
    {
        if ( LexIdForm(partialLex) == 0 && morphs[ morphNo ].mp->type() == MORPH_TYPE_STRICT )
            partialLex = _LexSetDictForm( partialLex );

        int result = morphs[ morphNo ].mp->getLexText( partialLex, stem, buffer, lpMaxWordLength + 1, additionalData );
        if ( unlikely(result < 0) )
            return static_cast<LingProcErrors>(result);
        textReceiver.commitString( static_cast<size_t>(result) );
        return LP_OK;
    }
    else
    {
        buffer[0] = '\0';
        textReceiver.commitString( 1 );
        return LP_ERROR_ERANGE;
    }
}

MorphTypes LingProc::LexMorphType( LEXID lex ) const
{
    if ( unlikely(shadow == 0) )
        return MORPH_TYPE_INVALID;
    
    return shadow->LexMorphType( lex );
}

LangCodes LingProc::LexLang( LEXID lex ) const
{
    if ( unlikely(shadow == 0) )
        return LNG_UNKNOWN;
    
    return shadow->LexLang( lex );
}

MorphTypes LingProcData::LexMorphType( LEXID lex ) const
{
    if ( unlikely(lex == LEXNONE || lex == LEXINVALID) )
        return MORPH_TYPE_INVALID;

    size_t morphNo = LexNoMrph( lex );
    if ( unlikely(morphNo >= lpMaxMorphsNum) )
        return MORPH_TYPE_INVALID;

    if ( morphs[ morphNo ].mp != 0 )
        return morphs[ morphNo ].mp->type();

    return MORPH_TYPE_INVALID;
}

LangCodes LingProcData::LexLang( LEXID lex ) const
{
    if ( unlikely(lex == LEXNONE || lex == LEXINVALID) )
        return LNG_UNKNOWN;

    size_t morphNo = LexNoMrph( lex );
    if ( unlikely(morphNo >= lpMaxMorphsNum) )
        return LNG_UNKNOWN;

    if ( morphs[ morphNo ].mp != 0 )
        return morphs[ morphNo ].mp->getLang();

    return LNG_UNKNOWN;
}

LingProcErrors LingProcData::ProcessWord( ProcessFunctor                   &functor,
                                          const char                       *word,
                                          size_t                            wlen,
                                          unsigned int                      morphFlags,
                                          const LingProc::EnabledLangs     &enabledLangs,
                                          const LingProc::EnabledMorphsSet &enabledMorphs,
                                          uint64_t                          runtimeFlags,
                                          const LingProc::EnabledMorphsSet &runtimeEnabledMorphs )
{
    int res = 0;

    /* Do no rus-lat conversion on punctuation and numbers */

    /* Process punctuation */
    if ( runtimeEnabledMorphs.check( MORPHNO_PUNCT ) && enabledMorphs.check( MORPHNO_PUNCT ) )
    {
        res = functor.process( &morphPunct, word, wlen, morphFlags );
        if ( res > 0 || res == LP_ENUM_STOP )
            return LP_OK;
    }

    /* Process numbers */
    if ( runtimeEnabledMorphs.check( MORPHNO_NUM ) && enabledMorphs.check( MORPHNO_NUM ) )
    {
        res = functor.process( &morphNumeric, word, wlen, morphFlags );
        if ( res > 0 || res == LP_ENUM_STOP )
            return LP_OK;
    }

    /* Rus-lat conversions */
    BufferReceiver< lpMaxWordLength + 1 > rlconvBuffer;
    if ( runtimeFlags & ( LingProc::fRusLatConv    |
                          LingProc::fRusLatTRusLat |
                          LingProc::fRusLatTDigLat |
                          LingProc::fRusLatTLatRus |
                          LingProc::fRusLatTDigRus ) )
    {
        RLCConversions conflictConversion = RLC_NONE;
        RLCConversions defaultConversion  = RLC_NONE;
        
        unsigned int rlctTables = RLCT_ALL;
        bool         noDetect   = false;
        
        if ( runtimeFlags & LingProc::fRusLatConv )
        {
            /* Deprecated API support */
            if ( runtimeFlags & LingProc::fRusLatConvLat )
            {
                defaultConversion  = RLC_TO_LAT;
                conflictConversion = RLC_TO_LAT;
            }
            else if ( runtimeFlags & LingProc::fRusLatConvRus )
            {
                defaultConversion  = RLC_TO_RUS;
                conflictConversion = RLC_TO_RUS;
            }
        }
        else
        {
            /* New API support */
            rlctTables = RLCT_NONE;

            if ( runtimeFlags & LingProc::fRusLatTRusLat )
                rlctTables |= RLCT_RUS_TO_LAT;
            if ( runtimeFlags & LingProc::fRusLatTDigLat )
                rlctTables |= RLCT_DIG_TO_LAT;
            if ( runtimeFlags & LingProc::fRusLatTLatRus )
                rlctTables |= RLCT_LAT_TO_RUS;
            if ( runtimeFlags & LingProc::fRusLatTDigRus )
                rlctTables |= RLCT_DIG_TO_RUS;
            
            if ( runtimeFlags & LingProc::fRusLatDRusLat )
                defaultConversion = RLC_TO_LAT;
            else if ( runtimeFlags & LingProc::fRusLatDLatRus )
                defaultConversion = RLC_TO_RUS;
            
            if ( runtimeFlags & LingProc::fRusLatCRusLat )
                conflictConversion = RLC_TO_LAT;
            else if ( runtimeFlags & LingProc::fRusLatCLatRus )
                conflictConversion = RLC_TO_RUS;

            if ( runtimeFlags & LingProc::fRusLatNoDetect )
                noDetect = true;
        }

        RLCConversions conv = RLC_NONE;
        
        if ( noDetect )
            conv = defaultConversion;
        else
            conv = ruslatConv.Check( word,
                                     wlen,
                                     conflictConversion,
                                     defaultConversion,
                                     rlctTables );

        if ( conv != RLC_NONE )
        {
            // run conversion
            ruslatConv.Convert( conv, rlconvBuffer, word, wlen, rlctTables );
            
            // creating local copy of word
            word = rlconvBuffer.getBuffer();
            wlen = rlconvBuffer.size();
        }
    }

    // languages
    bool oneAccepted = false;
    for ( size_t langIndex = 1; langIndex < langs.size(); langIndex++ )
    {
        if ( !enabledLangs.check( langIndex ) )
            continue;

        res = ProcessWordLang( langIndex,
                               functor,
                               word,
                               wlen,
                               morphFlags,
                               enabledMorphs,
                               runtimeEnabledMorphs );
        if ( unlikely(res < 0) )
        {
            if ( res == LP_ENUM_STOP )
                return LP_OK;
            else
                return static_cast<enum LingProcErrors>(res);
        }
        else if ( res > 0 && !oneAccepted )
            oneAccepted = true;

        if ( (runtimeFlags & LingProc::fTillFirst) && res > 0 )
            break;
    }

    // hierogliphs
    if ( (!oneAccepted || (runtimeFlags & LingProc::fForceHierHomo)) &&
         (runtimeEnabledMorphs.check( MORPHNO_HIER ) && enabledMorphs.check( MORPHNO_HIER )) )
    {
        char            buffer[ lpMaxWordLength + 1];
        const char     *wordFixed;
        size_t          wlenFixed;
        LingProcErrors  status;

        status = maybeRunWordFilter( langs[ MORPHNO_HIER ].wordFilters,
                                     MORPH_TYPE_HIER,
                                     buffer,
                                     lpMaxWordLength + 1,
                                     word,
                                     wlen,
                                     wordFixed,
                                     wlenFixed );
        if ( unlikely(status != LP_OK) )
            return status;
        
        langs[ MORPHNO_HIER ].dirty = true;
        res = functor.process( langs[ MORPHNO_HIER ].fuzzy,
                               wordFixed,
                               wlenFixed,
                               morphFlags );
        if ( unlikely(res < 0) )
        {
            if ( res == LP_ENUM_STOP )
                return LP_OK;
            else
                return static_cast<enum LingProcErrors>(res);
        }
    }

    return LP_OK;
}

int LingProcData::ProcessWordLang( size_t                            langIndex,
                                   ProcessFunctor                   &functor,
                                   const char                       *word,
                                   size_t                            wlen,
                                   unsigned int                      morphFlags,
                                   const LingProc::EnabledMorphsSet &enabledMorphs,
                                   const LingProc::EnabledMorphsSet &runtimeEnabledMorphs )
{
    int res  = 0;
    int res2 = 0;    
    
    char            buffer[ lpMaxWordLength + 1];
    const char     *wordFixed;
    size_t          wlenFixed;
    LingProcErrors  status;
    
    // strict
    if ( runtimeEnabledMorphs.check( langs[ langIndex ].strictInternalNo ) && enabledMorphs.check( langs[ langIndex ].strictInternalNo ))
    {
        status = maybeRunWordFilter( langs[ langIndex ].wordFilters,
                                     MORPH_TYPE_STRICT,
                                     buffer,
                                     lpMaxWordLength + 1,
                                     word,
                                     wlen,
                                     wordFixed,
                                     wlenFixed );
        if ( unlikely(status != LP_OK) )
            return (int)status;
        
        res = functor.process( langs[ langIndex].strict, wordFixed, wlenFixed, morphFlags );
    }    
    if ( res != 0 )
        return res;

    // fuzzy
    if (runtimeEnabledMorphs.check( langs[ langIndex ].fuzzyInternalNo ) && enabledMorphs.check( langs[ langIndex ].fuzzyInternalNo ))
    {
        status = maybeRunWordFilter( langs[ langIndex ].wordFilters,
                                     MORPH_TYPE_FUZZY,
                                     buffer,
                                     lpMaxWordLength + 1,
                                     word,
                                     wlen,
                                     wordFixed,
                                     wlenFixed );
        if ( unlikely(status != LP_OK) )
            return (int)status;
        
        res = functor.process( langs[ langIndex].fuzzy, wordFixed, wlenFixed, morphFlags );
    }
    if ( res != 0 )
    {
        langs[ langIndex ].dirty = true;
        return res;
    }

    // tilde
    if (runtimeEnabledMorphs.check( langs[ langIndex ].tildeInternalNo ) && enabledMorphs.check( langs[ langIndex ].tildeInternalNo ))
    {
        status = maybeRunWordFilter( langs[ langIndex ].wordFilters,
                                     MORPH_TYPE_TILDE,
                                     buffer,
                                     lpMaxWordLength + 1,
                                     word,
                                     wlen,
                                     wordFixed,
                                     wlenFixed );
        if ( unlikely(status != LP_OK) )
            return (int)status;
        
        res = functor.process( langs[ langIndex].tilde, wordFixed, wlenFixed, morphFlags );
    }
    if ( res < 0 )
        return res;
    else if ( res != 0 && (morphFlags & MORPH_FLAG_PROCESS_TILDE))
        langs[ langIndex ].dirty = true;
    
    // tilde_rev
    if (runtimeEnabledMorphs.check( langs[ langIndex ].tildeRevInternalNo ) && enabledMorphs.check( langs[ langIndex ].tildeRevInternalNo ))
    {
        status = maybeRunWordFilter( langs[ langIndex ].wordFilters,
                                     MORPH_TYPE_TILDE_REV,
                                     buffer,
                                     lpMaxWordLength + 1,
                                     word,
                                     wlen,
                                     wordFixed,
                                     wlenFixed );
        if ( unlikely(status != LP_OK) )
            return (int)status;
        
        res2 = functor.process( langs[ langIndex].tildeRev, wordFixed, wlenFixed, morphFlags );
    }
    if ( res2 < 0 )
        return res2;
    else if ( res2 != 0 && (morphFlags & MORPH_FLAG_PROCESS_TILDE) )
        langs[ langIndex ].dirty = true;
    
    return ( res2 > 0 ? res2 : res );
}

LingProcErrors LingProcData::LemmatizeDoc( DocText                          &docText,
                                           DocImage                         &docImage,
                                           unsigned int                      morphFlags,
                                           uint64_t                          runtimeFlags,
                                           const LingProc::EnabledMorphsSet &runtimeEnabledMorphs )
{
    // reset image
    docImage.Reset();
    docImage.SetDocText( &docText );

    LingProcErrors status;
        
    // lang detection
    if ( (runtimeFlags & LingProc::fDetectLang) && docText.checkUseLangDetector() )
    {
        if ( !docText.checkLangDetect() )
        {
            // process lang detection
            status = LangDetectDocReal( docText );
            if ( unlikely(status != LP_OK) )
                return status;
        }
        
        docImage.ConvertLanguages( docText );
    }
    else
        docImage.AssignPresetLanguages( docText );

    // create lang list for lemmatization
    LingProc::EnabledLangs enabledLangs;
    
    if ( runtimeFlags & LingProc::fUseDetectedLangs || runtimeFlags & LingProc::fDetectLang )
    {
        enabledLangs.set( getLangIndex( docImage.LangPrim() ) );
        enabledLangs.set( getLangIndex( docImage.LangSupp() ) );
    }
    else
        enabledLangs.setAll();

    size_t docSize = docText.WordsCount();
    
    if ( unlikely(docSize == 0) )
        return LP_OK;
        
    // initial memory reservation
    status = docImage.PrepareReserve( docText );
    if ( unlikely(status != LP_OK) )
        return status;
        
    // lemmatization    
    for ( size_t nword = 0; nword < docSize; nword++ )
    {
        /* lemmatize new word */
        LemmatizeDocImageFunctor dFunctor =
            docImage.MakeLemmatizeFunctor( docText.WordAttrs( nword ), ( runtimeFlags & LingProc::fDocImageForms ? true : false ) );
        LemmatizeProcessFunctor pFunctor( dFunctor );

        status = ProcessWord( pFunctor,
                              docText.WordText( nword ),
                              docText.WordLength( nword ),
                              morphFlags,
                              enabledLangs,
                              LingProc::EnabledMorphsSet().setAll(),
                              runtimeFlags, 
                              runtimeEnabledMorphs );
        if ( unlikely(status != LP_OK) )
            return status;
        
        if ( unlikely(!dFunctor.wasLemmatizationPerformed()) ) /* lemmatization failed */
        {
            status = dFunctor.nextLemma( LEXNONE, 0, 0 );
            if ( unlikely(status != LP_OK && status != LP_ENUM_STOP) )
                return status;
        }

        /* Finish the word processing */
        status = dFunctor.finish();
        if ( unlikely(status != LP_OK) )
            return status;
    }

    // convert markup
    status = docImage.ConvertMarkup( docText );
    if ( unlikely(status != LP_OK) )
        return status;
    
    return LP_OK;
}

LingProcErrors LingProc::SetWordFilter( LangCodes lang, MorphTypes morph, WordFilter *filter )
{
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;
    
    return shadow->SetWordFilter( lang, morph, filter );
}

LingProcErrors LingProcData::SetWordFilter( LangCodes lang, MorphTypes morph, LingProc::WordFilter *filter )
{
    LingProcErrors status;
    
    if ( morph == MORPH_TYPE_ALL )
    {
        status = SetWordFilter( lang, MORPH_TYPE_HIER, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        status = SetWordFilter( lang, MORPH_TYPE_FUZZY, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        status = SetWordFilter( lang, MORPH_TYPE_STRICT, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        status = SetWordFilter( lang, MORPH_TYPE_TILDE, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        status = SetWordFilter( lang, MORPH_TYPE_TILDE_REV, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        
        return LP_OK;
    }
    
    if ( morph == MORPH_TYPE_TILDE_BOTH )
    {
        status = SetWordFilter( lang, MORPH_TYPE_TILDE, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        status = SetWordFilter( lang, MORPH_TYPE_TILDE_REV, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        
        return LP_OK;
    }
    
    if ( lang == LNG_ALL )
    {
        status = SetWordFilter( LNG_ENGLISH, morph, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        status = SetWordFilter( LNG_RUSSIAN, morph, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        status = SetWordFilter( LNG_GERMAN, morph, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        status = SetWordFilter( LNG_FRENCH, morph, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        status = SetWordFilter( LNG_SPANISH, morph, filter );
        if ( unlikely(status != LP_OK) )
            return status;
        
        return LP_OK;
    }
    
    if ( lang == LNG_DEFAULT )
    {
        for ( size_t i = 0; i < langs.size(); i++ )
        {
            status = SetWordFilter( langs[ i ].lang, morph, filter );
            if ( unlikely(status != LP_OK) )
                return status;
        }
        
        return LP_OK;
    }

    size_t langIndex = ( morph == MORPH_TYPE_HIER ? 0 : getLangIndex( lang ) );
    if ( unlikely(langIndex == static_cast<size_t>(-1)) )
        return LP_ERROR_UNSUPPORTED_LANG;

    size_t morphIndex = LingProcData::GetFilterMorphIndex( morph );
    if ( unlikely(morphIndex == static_cast<size_t>(-1)) )
        return LP_ERROR_UNSUPPORTED_MORPH;

    langs[ langIndex ].wordFilters[ morphIndex ] = filter;
    
    return LP_OK;
}

