/**
 * @file   LexGram.cpp
 * @author swizard <me@swizard.info>
 * @date   Mon Oct 26 10:06:18 2009
 *
 * @brief  StrictMorphGram/LexGram common API implementation.
 *
 *
 */

#include <_include/cc_compat.h>
#include <_include/_string.h>

#include "LexGramDescShadow.h"

LexGram::Refl LexGram::getRefl() const
{
        if ( unlikely(getShadow() == 0) )
                return LexGram::REFL_LAST;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getRefl();
}

LexGram::Transitivity LexGram::getTransitivity() const
{
        if ( unlikely(getShadow() == 0) )
                return LexGram::TRANSITIVITY_LAST;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getTransitivity();
}

LexGram::Aspect LexGram::getAspect() const
{
        if ( unlikely(getShadow() == 0) )
                return LexGram::ASPECT_LAST;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getAspect();
}

LexGram::Animate LexGram::getAnimate() const
{
        if ( unlikely(getShadow() == 0) )
                return LexGram::ANIMATE_LAST;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getAnimate();
}

LexGram::Number LexGram::getNumber() const
{
        if ( unlikely(getShadow() == 0) )
                return LexGram::NUMBER_LAST;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getNumber();
}

LexGram::Gender LexGram::getGender() const
{
        if ( unlikely(getShadow() == 0) )
                return LexGram::GENDER_LAST;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getGender();
}

LexGram::Suppl LexGram::getSuppl() const
{
        if ( unlikely(getShadow() == 0) )
                return LexGram::SUPPL_LAST;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getSuppl();
}

LexGram::Subtype LexGram::getSubtype() const
{
        if ( unlikely(getShadow() == 0) )
                return LexGram::SUBTYPE_LAST;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getSubtype();
}

LexGram::PartOfSpeech LexGram::getPartOfSpeech() const
{
        if ( unlikely(getShadow() == 0) )
                return LexGram::PART_LAST;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getPartOfSpeech();
}

const char *LexGram::getName( LexGram::PartOfSpeech value )
{
        switch ( value )
        {
        case PART_NOUN:
                return "Noun";
        case PART_ADJ:
                return "Adj";
        case PART_VERB:
                return "Verb";
        case PART_ADV:
                return "Adv";
        case PART_NUM:
                return "Num";
        case PART_PRON:
                return "Pron";
        case PART_PREP:
                return "Prep";
        case PART_CONJ:
                return "Conj";
        case PART_PARTICLE:
                return "Part";
        case PART_INTERJECTION:
                return "Interjection";
        case PART_ARTICLE:
                return "Article";
        case PART_UNDEF:
                return "Undef";
        default:
                return "";
        };
}

const char *LexGram::getName( LexGram::Refl value )
{
        switch ( value )
        {
        case REFL_REFL:
                return "Refl";
        default:
                return "";
        };
}

const char *LexGram::getName( LexGram::Transitivity value )
{
        switch ( value )
        {
        case TRANSITIVITY_TRANS:
                return "Trans";
        case TRANSITIVITY_INTRANS:
                return "Intrans";
        default:
                return "";
        };
}

const char *LexGram::getName( LexGram::Aspect value )
{
        switch ( value )
        {
        case ASPECT_IMPERF:
                return "Imperf";
        case ASPECT_PERF:
                return "Perf";
        case ASPECT_BOTH:
                return "Perf/Imperf";
        default:
                return "";
        };
}

const char *LexGram::getName( LexGram::Animate value )
{
        switch ( value )
        {
        case ANIMATE_ANIMATE:
                return "Animate";
        case ANIMATE_INANIMATE:
                return "Inanimate";
        case ANIMATE_BOTH:
                return "Animate/Inanimate";
        default:
                return "";
        };
}

const char *LexGram::getName( LexGram::Number value )
{
        switch ( value )
        {
        case NUMBER_SG:
                return "Sg";
        case NUMBER_PL:
                return "Pl";
        default:
                return "";
        };
}

const char *LexGram::getName( LexGram::Gender value )
{
        switch ( value )
        {
        case GENDER_MASC:
                return "Masc";
        case GENDER_FEM:
                return "Fem";
        case GENDER_NEUT:
                return "Neut";
        case GENDER_COMMON:
                return "Common";
        case GENDER_PLURAL:
                return "Plural";
        case GENDER_UNDEF:
                return "Undef";
        case GENDER_FEM_N_NEUT:
                return "Fem_Neut";
        case GENDER_MASC_N_NEUT:
                return "Masc_Neut";
        case GENDER_MASC_FEM_N_NEUT:
                return "Masc_Fem_Neut";
        default:
                return "";
        };
}

const char *LexGram::getName( LexGram::Suppl value )
{
        switch ( value )
        {
        case SUPPL_SHORTEN:
                return "Shorten";
        case SUPPL_SUBST:
                return "Subst";
        case SUPPL_ADJ:
                return "Adj";
        default:
                return "";
        };
}

const char *LexGram::getName( LexGram::Subtype value )
{
        switch ( value )
        {
        case SUBTYPE_SURNAME:
                return "Surname";
        case SUBTYPE_NAME:
                return "Name";
        case SUBTYPE_PATRONYMIC:
                return "Patronymic";
        case SUBTYPE_GEO:
                return "Geo";
        case SUBTYPE_PROPER:
                return "Proper";
        case SUBTYPE_ABBR:
                return "Abbr";
        case SUBTYPE_NUMER:
                return "Numer";
        case SUBTYPE_QUALITATIVE:
                return "Qualitative";
        case SUBTYPE_RELATIVE:
                return "Relative";
        case SUBTYPE_POSSESSIVE:
                return "Possessive";
        case SUBTYPE_IMPERSONAL:
                return "Impersonal";
        case SUBTYPE_FREQUENTATIVE:
                return "Frequentative";
        case SUBTYPE_PREDICATIVE:
                return "Predicative";
        case SUBTYPE_COMPARATIVE:
                return "Comparative";
        case SUBTYPE_PARENTHESIS:
                return "Parenthesis";
        case SUBTYPE_CARDINAL:
                return "Cardinal";
        case SUBTYPE_ORDINAL:
                return "Ordinal";
        case SUBTYPE_COLLECTIVE:
                return "Collective";
        case SUBTYPE_ADJECTIVE:
                return "Adjective";
        case SUBTYPE_PERSONAL:
                return "Personal";
        case SUBTYPE_REFL:
                return "Refl";
        case SUBTYPE_CONNECTIVE:
                return "Connective";
        case SUBTYPE_NEGATIVE:
                return "Negative";
        case SUBTYPE_UNDEF:
                return "Undef";
        case SUBTYPE_DEF:
                return "Def";
        default:
                return "";
        };
}

const char *LexGram::getName() const
{
        if ( unlikely(getShadow() == 0) )
                return 0;
        return reinterpret_cast<const LexGramDescShadow *>(getShadow())->getName();
}

static void concat( char **p, const char *name, const char *value, bool shortMode )
{
        if ( !shortMode )
        {
                for ( ; *name != '\0'; (**p) = (*name), (*p)++, name++ );
                (**p) = '='; (*p)++;
        }
        for ( ; *value != '\0'; (**p) = (*value), (*p)++, value++ );
        (**p) = ','; (*p)++;
}

const char *LexGram::describe( StrictAllocator &stringAllocator, bool shortMode ) const
{
        int bufferSize = 0;

        if ( getPartOfSpeech() != PART_LAST && getPartOfSpeech() != PART_NONE )
        { bufferSize += (shortMode ? -1 : strlen("PartOfSpeech"));
          bufferSize += strlen( LexGram::getName( getPartOfSpeech() ) ); bufferSize += 2; }
        if ( getSubtype() != SUBTYPE_LAST && getSubtype() != SUBTYPE_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Subtype"));
          bufferSize += strlen( LexGram::getName( getSubtype() ) ); bufferSize += 2; }
        if ( getSuppl() != SUPPL_LAST && getSuppl() != SUPPL_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Suppl"));
          bufferSize += strlen( LexGram::getName( getSuppl() ) ); bufferSize += 2; }
        if ( getGender() != GENDER_LAST && getGender() != GENDER_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Gender"));
          bufferSize += strlen( LexGram::getName( getGender() ) ); bufferSize += 2; }
        if ( getNumber() != NUMBER_LAST && getNumber() != NUMBER_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Number"));
          bufferSize += strlen( LexGram::getName( getNumber() ) ); bufferSize += 2; }
        if ( getAnimate() != ANIMATE_LAST && getAnimate() != ANIMATE_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Animate"));
          bufferSize += strlen( LexGram::getName( getAnimate() ) ); bufferSize += 2; }
        if ( getAspect() != ASPECT_LAST && getAspect() != ASPECT_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Aspect"));
          bufferSize += strlen( LexGram::getName( getAspect() ) ); bufferSize += 2; }
        if ( getTransitivity() != TRANSITIVITY_LAST && getTransitivity() != TRANSITIVITY_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Transitivity"));
          bufferSize += strlen( LexGram::getName( getTransitivity() ) ); bufferSize += 2; }
        if ( getRefl() != REFL_LAST && getRefl() != REFL_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Refl"));
          bufferSize += strlen( LexGram::getName( getRefl() ) ); bufferSize += 2; }

        char *buffer = reinterpret_cast<char *>( stringAllocator.alloc( bufferSize ) );
        if ( unlikely(buffer == 0) )
            return 0;

        char *p = buffer;

        if ( getPartOfSpeech() != PART_LAST && getPartOfSpeech() != PART_NONE )
                concat( &p, "PartOfSpeech", LexGram::getName( getPartOfSpeech() ), shortMode );
        if ( getSubtype() != SUBTYPE_LAST && getSubtype() != SUBTYPE_NONE )
                concat( &p, "Subtype", LexGram::getName( getSubtype() ), shortMode );
        if ( getSuppl() != SUPPL_LAST && getSuppl() != SUPPL_NONE )
                concat( &p, "Suppl", LexGram::getName( getSuppl() ), shortMode );
        if ( getGender() != GENDER_LAST && getGender() != GENDER_NONE )
                concat( &p, "Gender", LexGram::getName( getGender() ), shortMode );
        if ( getNumber() != NUMBER_LAST && getNumber() != NUMBER_NONE )
                concat( &p, "Number", LexGram::getName( getNumber() ), shortMode );
        if ( getAnimate() != ANIMATE_LAST && getAnimate() != ANIMATE_NONE )
                concat( &p, "Animate", LexGram::getName( getAnimate() ), shortMode );
        if ( getAspect() != ASPECT_LAST && getAspect() != ASPECT_NONE )
                concat( &p, "Aspect", LexGram::getName( getAspect() ), shortMode );
        if ( getTransitivity() != TRANSITIVITY_LAST && getTransitivity() != TRANSITIVITY_NONE )
                concat( &p, "Transitivity", LexGram::getName( getTransitivity() ), shortMode );
        if ( getRefl() != REFL_LAST && getRefl() != REFL_NONE )
                concat( &p, "Refl", LexGram::getName( getRefl() ), shortMode );

        buffer[ ( bufferSize == 0 ? 0 : bufferSize - 1 ) ] = '\0';

        return buffer;
}

