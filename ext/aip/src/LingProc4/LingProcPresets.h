#ifndef _LINGPROCPRESETS_H_
#define _LINGPROCPRESETS_H_

#include <_include/cc_compat.h>
#include <_include/_inttype.h>

#include "LingProc.h"
#include "UCharSet/UCharSet.h"
#include "RusLatConv/RusLatConv.h"

class LingProcSettings
{
protected: // prevent construction, use derived class
    LingProcSettings()
    {
        runtimeFlags = LingProc::defaultRuntimeFlags;

        langs[0]   = LNG_ALL;
        langsCount = 1;
    }

public:
    virtual ~LingProcSettings() {}

protected:
    /* PROTECTED DATA */
    // adjust only in the constructor of a derived class
    uint64_t       runtimeFlags;
    LingProcParams createParams;
    LangCodes      langs[ lpMaxLangNum ];
    size_t         langsCount;

public:
    uint64_t getRuntimeFlags()              const { return runtimeFlags; }
    const LangCodes *getLangs()             const { return langs; }
    size_t getLangsCount()                  const { return langsCount; }
    const LingProcParams *getCreateParams() const { return &createParams; }

    void setRuntimeFlags( uint64_t flags ) { runtimeFlags = flags; }
};

class LingProcPreset : public LingProc
{
public:
    LingProcPreset() : LingProc(), presetConfig( 0 ) { }
    virtual ~LingProcPreset() { }

    LingProcErrors Open( LingProcOpenModes mode, fstorage *fs )
    {
        if ( unlikely(presetConfig == 0) )
            return LP_ERROR_EFAULT;

        return LingProc::Open( mode, fs, presetConfig->getLangsCount(), presetConfig->getLangs() );
    }

    const LingProcSettings* Settings()const
    {
        return presetConfig;
    }

    LingProcErrors Create()
    {
        if ( unlikely(presetConfig == 0) )
            return LP_ERROR_EFAULT;

        return LingProc::Create( presetConfig->getCreateParams(), presetConfig->getLangsCount(), presetConfig->getLangs() );
    }

public:
    // low level access
    void AssignSettings( const LingProcSettings *settings )
    {
        presetConfig = settings;
        if ( likely(presetConfig != 0) )
            SetPresetFlags();
    }

private:
    // restrict access to basic open/create
    // the following functions should not be used with presets
    LingProcErrors Create( const LingProcParams *params );
    LingProcErrors Create( const LingProcParams *params, size_t langsListSize, const LangCodes *langsList );
    LingProcErrors Open( LingProcOpenModes mode, fstorage *fs, size_t langsListSize, const LangCodes *langsList );

private:
    void SetPresetFlags()
    {
        SetFlags( presetConfig->getRuntimeFlags() );
    }

private:
    const LingProcSettings *presetConfig;
};

template< typename SettingsType >
class LingProcPresetAdjusted : public LingProcPreset
{
public:
    LingProcPresetAdjusted() : LingProcPreset() { AssignSettings( &adjustedSettings ); }
    virtual ~LingProcPresetAdjusted() { }

protected:
    SettingsType adjustedSettings;
};

/**********************************************************
 * Presets                                                *
 **********************************************************/
class InfLingProcSettings : public LingProcSettings
{
public:
    InfLingProcSettings() : LingProcSettings()
    {
        runtimeFlags =
            //LingProc::fDetectLang        |
            LingProc::fUseDetectedLangs    |
            LingProc::fTillFirst           |
            LingProc::fForceHierHomo       |
            // LingProc::fRusLatConv       |
            //LingProc::fRusLatConvLat     |
            //LingProc::fRusLatConvRus     |
            0;

        createParams.morphType           = MORPH_TYPE_TILDE_BOTH;
        createParams.morphFlags         |= MORPH_FLAG_CASE_INSENSITIVE;
        createParams.morphFlags         |= MORPH_FLAG_LWR_ACCENT;
        createParams.morphFlags         |= MORPH_FLAG_LWR_PUNCT;
        createParams.morphFlags         |= MORPH_FLAG_REM_IGNORE;
        //createParams.morphFlags       |= MORPH_FLAG_NATIVE_ONLY;
        createParams.useMorphFlags       = true;

        createParams.charsetTuneMode     = UCharSet::tmApostrophPunct | UCharSet::tmTildeWordPart;
        createParams.ruslatConvTuneMode  = RusLatConverter::tmNoDigits;

        createParams.disabledMorphs.set( MORPHNO_NUM );

        langs[0]   = LNG_RUSSIAN;
        langsCount = 1;
    }
};

typedef LingProcPresetAdjusted<InfLingProcSettings> InfLingProc;
/*============================================================================*/
#endif /* _LINGPROCPRESETS_H_ */
