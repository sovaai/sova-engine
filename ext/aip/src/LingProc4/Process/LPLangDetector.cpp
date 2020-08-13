
#include <_include/cc_compat.h>

#include "../LingProc.h"
#include "../LingProcData.h"
#include <stdio.h>

LingProcErrors LingProc::LangDetectDoc( DocText &docText ) const
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
    
    return shadow->LangDetectDoc( docText, runtimeFlags );
}

LingProcErrors LingProcData::LangDetectDoc( DocText &docText, uint64_t runtimeFlags ) const
{
    // clear detect lang results
    docText.ResetFlagsLang();
    docText.ResetFlagsDetectedLang();

    LingProcErrors status;
        
    // lang detection and adjusting
    if ( (runtimeFlags & LingProc::fDetectLang) && docText.checkUseLangDetector() )
    {
        // process lang detection
        status = LangDetectDocReal( docText );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    // set and adjust langs
    docText.InstallDetectedLangs();        

    return LP_OK;
}

LingProcErrors LingProcData::LangDetectDocReal( DocText &docText ) const
{
    // getting generic charset
    const UCharSet *charset = getGenericCharset();

    // clear detect lang results
    docText.SetDetectedLangs( LNG_UNKNOWN, LNG_UNKNOWN, false );

    LingProcErrors status;

    // prepare lang detectors
    for ( size_t curLang = 1; curLang < langs.size(); curLang++ )
    {
        const LangData &ld = langs[ curLang ];

        if ( unlikely(ld.detector == 0) )
            continue;

        status = ld.detector->reset();
        if ( unlikely(status != LP_OK) )
            return status;
    }
    
    // lang detection
    for ( size_t curWord = 0; curWord < docText.WordsCount(); curWord++ )
    {
        // create local copy of a word in lower case; use generic charset for not
        // to create local copy for each language
        char   buf[ lpMaxWordLength + 1 ];
        size_t bufOffset = 0;

        const char *wordText = docText.WordText( curWord );
        size_t      wordLen  = docText.WordLength( curWord );
        size_t      offset   = 0;

        for ( ; offset < wordLen && bufOffset < lpMaxWordLength ; )
        {
            size_t ch;
            U8_NEXT_UNSAFE( wordText, offset, ch );

            if ( charset->IsWordIgnore( ch ) )
                continue;

            ch = charset->ToLower( ch );
            ch = charset->ToLoPunct( ch );

            U8_APPEND_UNSAFE( buf, bufOffset, ch );
        }
        buf[ bufOffset ] = '\0';
        
        // perform incremental detection
        for ( size_t curLang = 1; curLang < langs.size(); curLang++ )
        {
            const LangData &ld = langs[ curLang ];

            if ( unlikely(ld.detector == 0) )
                continue;
            
            status = ld.detector->update( buf, bufOffset );
            if ( unlikely(status != LP_OK) )
                return status;
        }
    }

    // finish lang detection and analyze results
    size_t primIndex = 0;
    float  primProb  = 0.0;
    size_t suppIndex = 0;
    float  suppProb  = 0.0;
    
    for ( size_t curLang = 1; curLang < langs.size(); curLang++ )
    {
        const LangData &ld = langs[ curLang ];

        if ( unlikely(ld.detector == 0) )
            continue;
        
        status = ld.detector->finish();
        if ( unlikely(status != LP_OK) )
            return status;

        float currentProb = ld.detector->getLangProbability();

        if ( currentProb > primProb )
        {
            suppIndex = primIndex;
            suppProb  = primProb;
            primIndex = curLang;
            primProb  = currentProb;
        }
        else if ( currentProb > suppProb )
        {
            suppIndex = curLang;
            suppProb  = currentProb;
        }
    }

    LangCodes langPrim = langs[ primIndex ].lang;
    LangCodes langSupp = langs[ suppIndex ].lang;

    // store results
    docText.SetDetectedLangs( langPrim, langSupp, true );

    return LP_OK;
}

