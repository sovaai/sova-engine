
#include "LangDetector.h"
#include "Trigrams.h"
#include "DetectWordsTrigrams.h"

#include "trigrams_ar.gperf.h"
#include "trigrams_de.gperf.h"
#include "trigrams_en.gperf.h"
#include "trigrams_es.gperf.h"
#include "trigrams_fr.gperf.h"
#include "trigrams_ru.gperf.h"
#include "trigrams_vi.gperf.h"
#include "trigrams_ua.gperf.h"

LangDetector *LangDetector::factory( LangCodes lang )
{
    switch ( lang )
    {
    case LNG_VIETNAMESE:
        return new TrigramLangDetector< trigram_vi, get_trigram_vi >( 1.4, 0.01 );

    case LNG_ARABIC:
        return new TrigramLangDetector< trigram_ar, get_trigram_ar >( 1.4, 0.01 );
        
    case LNG_ENGLISH:
        return new DetectWordsTrigramLangDetector< trigram_en, get_trigram_en >( 0.9, 96.0, 0.01 );
        
    case LNG_RUSSIAN:
        return new DetectWordsTrigramLangDetector< trigram_ru, get_trigram_ru >( 1.0, 20.0, 0.01 );
        
    case LNG_GERMAN:
        return new DetectWordsTrigramLangDetector< trigram_de, get_trigram_de >( 1.0, 10.0, 0.01 );

    case LNG_FRENCH:
        return new DetectWordsTrigramLangDetector< trigram_fr, get_trigram_fr >( 1.0, 12.0, 0.01 );

    case LNG_SPANISH:
        return new DetectWordsTrigramLangDetector< trigram_es, get_trigram_es >( 0.9, 24.0, 0.01 );

    case LNG_UKRAINIAN:
        return new DetectWordsTrigramLangDetector< trigram_ua, get_trigram_ua >( 0.50, 80.0, 0.01 );

    default:
        return 0;
    };
}

