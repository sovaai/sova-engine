
#include "WordNormalizerInterface.h"
#include "WordNormalizer.h"
#include "WordNormalizer/WordNormalizerDefault.h"
#include "WordNormalizer/WordNormalizerMorphPunct.h"
#include "WordNormalizer/WordNormalizerMorphFuzzy.h"
#include "WordNormalizer/WordNormalizerMorphStrict.h"
#include "WordNormalizer/WordNormalizerMorphTilde.h"
#include "WordNormalizer/WordNormalizerGerman.h"
#include "WordNormalizer/WordNormalizerArabic.h"
#include "WordNormalizer/WordNormalizerEuro.h"

WordNormalizer *WordNormalizer::factory( LangCodes lang, MorphTypes morph )
{
    if ( lang == LNG_UNKNOWN && morph == MORPH_TYPE_ALL ) // generic default normalizer
        return new CustomWordNormalizer< DefaultWordNormalizerPolicy >();
    
    if ( morph == MORPH_TYPE_PUNCT )
        return new CustomWordNormalizer< PunctWordNormalizerPolicy >();

    if ( lang == LNG_EURO )
    {
        if ( morph == MORPH_TYPE_FUZZY || morph == MORPH_TYPE_HIER )
            return new CustomWordNormalizer< FuzzyEuroWordNormalizerPolicy >();

        if ( morph == MORPH_TYPE_STRICT )
            return new CustomWordNormalizer< StrictEuroWordNormalizerPolicy >();
    }
    
    if ( lang == LNG_GERMAN )
    {
        if ( morph == MORPH_TYPE_FUZZY || morph == MORPH_TYPE_HIER )
            return new CustomWordNormalizer< FuzzyGermanWordNormalizerPolicy >();

        if ( morph == MORPH_TYPE_STRICT )
            return new CustomWordNormalizer< StrictGermanWordNormalizerPolicy >();
    }
    
    if ( lang == LNG_ARABIC )
    {
        if ( morph == MORPH_TYPE_FUZZY || morph == MORPH_TYPE_HIER )
            return new CustomWordNormalizer< FuzzyArabicWordNormalizerPolicy >();

        if ( morph == MORPH_TYPE_STRICT )
            return new CustomWordNormalizer< StrictArabicWordNormalizerPolicy >();
    }
    
    if ( morph == MORPH_TYPE_FUZZY || morph == MORPH_TYPE_HIER )
        return new CustomWordNormalizer< FuzzyWordNormalizerPolicy >();

    if ( morph == MORPH_TYPE_STRICT )
        return new CustomWordNormalizer< StrictWordNormalizerPolicy >();

    if ( morph == MORPH_TYPE_TILDE || morph == MORPH_TYPE_TILDE_REV )
        return new CustomWordNormalizer< TildeWordNormalizerPolicy >();
    
    return 0;
}

