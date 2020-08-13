/**
 * @file   FormGram.cpp
 * @author swizard <me@swizard.info>
 * @date   Mon Oct 26 09:56:58 2009
 * 
 * @brief  StrictMorphGram/FormGram common API implementation.
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_string.h>

#include "Paradigm.h"
#include "FormGramDescShadow.h"

FormGram::Mstform FormGram::getMstform() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::MSTFORM_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getMstform();
}

FormGram::Verbform FormGram::getVerbform() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::VERBFORM_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getVerbform();
}

FormGram::Tense FormGram::getTense() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::TENSE_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getTense();
}

FormGram::Short FormGram::getShort() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::SHORT_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getShort();
}

FormGram::Refl FormGram::getRefl() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::REFL_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getRefl();
}

FormGram::Person FormGram::getPerson() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::PERSON_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getPerson();
}

FormGram::Number FormGram::getNumber() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::NUMBER_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getNumber();
}

FormGram::Mood FormGram::getMood() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::MOOD_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getMood();
}

FormGram::Gender FormGram::getGender() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::GENDER_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getGender();
}

FormGram::Degree FormGram::getDegree() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::DEGREE_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getDegree();
}

FormGram::Case FormGram::getCase() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::CASE_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getCase();
}

FormGram::Animate FormGram::getAnimate() const
{
        if ( unlikely(getShadow() == 0) )
                return FormGram::ANIMATE_LAST;
        return reinterpret_cast<const ParadigmForm *>(getShadow())->getDesc()->getAnimate();
}

const char *FormGram::getName( FormGram::Animate value )
{
        switch (value)
        {
        case ANIMATE_ANIMATE:
                return "Animate";
        case ANIMATE_INANIMATE:
                return "Inanimate";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Mstform value )
{
        switch (value)
        {
        case MSTFORM_DIRECT:
                return "Direct";
        case MSTFORM_INDIRECT:
                return "Indirect";
        case MSTFORM_ABS:
                return "Abs";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Verbform value )
{
        switch (value)
        {
        case VERBFORM_INF:
                return "Inf";
        case VERBFORM_PRES1ST:
                return "Pres1st";
        case VERBFORM_PRES2ND:
                return "Pres2nd";
        case VERBFORM_PRES3RD:
                return "Pres3rd";
        case VERBFORM_PART1:
                return "Part1";
        case VERBFORM_PAST:
                return "Past";
        case VERBFORM_PASTSG:
                return "Pastsg";
        case VERBFORM_PASTPL:
                return "Pastpl";
        case VERBFORM_PART2:
                return "Part2";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Tense value )
{
        switch (value)
        {
        case TENSE_PRES:
                return "Pres";
        case TENSE_PAST:
                return "Past";
        case TENSE_FUTUR:
                return "Futur";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Short value )
{
        switch (value)
        {
        case SHORT_FULL:
                return "Full";
        case SHORT_SHORT:
                return "Short";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Refl value )
{
        switch (value)
        {
        case REFL_NOT_REFL:
                return "NonRefl";
        case REFL_REFL:
                return "Refl";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Person value )
{
        switch (value)
        {
        case PERSON_1ST:
                return "1st";
        case PERSON_2ND:
                return "2nd";
        case PERSON_3RD:
                return "3rd";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Number value )
{
        switch (value)
        {
        case NUMBER_SG:
                return "Sg";
        case NUMBER_PL:
                return "Pl";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Mood value )
{
        switch (value)
        {
        case MOOD_INF:
                return "Inf";
        case MOOD_INDICATIVE:
                return "Indicative";
        case MOOD_IMPER:
                return "Imper";
        case MOOD_SUBJ:
                return "Subj";
        case MOOD_ADV_PART:
                return "AdvPart";
        case MOOD_PART_I:
                return "PartI";
        case MOOD_PART_II:
                return "PartII";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Gender value )
{
        switch (value)
        {
        case GENDER_MASC:
                return "Masc";
        case GENDER_FEM:
                return "Fem";
        case GENDER_NEUT:
                return "Neut";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Degree value )
{
        switch (value)
        {
        case DEGREE_POS:
                return "Pos";
        case DEGREE_COMP:
                return "Comp";
        case DEGREE_SUPER:
                return "Super";
        default:
                return "";
        };
}

const char *FormGram::getName( FormGram::Case value )
{
        switch (value)
        {
        case CASE_NOM:
                return "Nom";
        case CASE_GEN:
                return "Gen";
        case CASE_DAT:
                return "Dat";
        case CASE_ACC:
                return "Acc";
        case CASE_INST:
                return "Inst";
        case CASE_PREP:
                return "Prep";
        case CASE_COMMON:
                return "Common";
        case CASE_POSS:
                return "Poss";
        default:
                return "";
        };
}

const char *FormGram::getName() const
{
        if ( unlikely(getShadow() == 0) )
                return 0;

        return reinterpret_cast<const ParadigmForm *>(getShadow())->getFormName();
}

FormGram::ParadigmType FormGram::getParadigmType() const
{
        if ( unlikely(getShadow() == 0) )
                return static_cast<FormGram::ParadigmType>(-1);

        return static_cast<FormGram::ParadigmType>( reinterpret_cast<const ParadigmForm *>(getShadow())->getParadigmType() );
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

const char *FormGram::describe( StrictAllocator &stringAllocator, bool shortMode ) const
{
        int bufferSize = 0;

        if ( getAnimate() != ANIMATE_LAST && getAnimate() != ANIMATE_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Animate"));
          bufferSize += strlen(FormGram::getName( getAnimate() )); bufferSize += 2; }
        if ( getCase() != CASE_LAST && getCase() != CASE_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Case"));
          bufferSize += strlen(FormGram::getName( getCase() )); bufferSize += 2; }
        if ( getDegree() != DEGREE_LAST && getDegree() != DEGREE_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Degree"));
          bufferSize += strlen(FormGram::getName( getDegree() )); bufferSize += 2; }
        if ( getGender() != GENDER_LAST && getGender() != GENDER_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Gender"));
          bufferSize += strlen(FormGram::getName( getGender() )); bufferSize += 2; }
        if ( getMood() != MOOD_LAST && getMood() != MOOD_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Mood"));
          bufferSize += strlen(FormGram::getName( getMood() )); bufferSize += 2; }
        if ( getNumber() != NUMBER_LAST && getNumber() != NUMBER_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Number"));
          bufferSize += strlen(FormGram::getName( getNumber() )); bufferSize += 2; }
        if ( getPerson() != PERSON_LAST && getPerson() != PERSON_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Person"));
          bufferSize += strlen(FormGram::getName( getPerson() )); bufferSize += 2; }
        if ( getRefl() != REFL_LAST && getRefl() != REFL_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Refl"));
          bufferSize += strlen(FormGram::getName( getRefl() )); bufferSize += 2; }
        if ( getShort() != SHORT_LAST && getShort() != SHORT_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Short"));
          bufferSize += strlen(FormGram::getName( getShort() )); bufferSize += 2; }
        if ( getTense() != TENSE_LAST && getTense() != TENSE_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Tense"));
          bufferSize += strlen(FormGram::getName( getTense() )); bufferSize += 2; }
        if ( getVerbform() != VERBFORM_LAST && getVerbform() != VERBFORM_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Verbform"));
          bufferSize += strlen(FormGram::getName( getVerbform() )); bufferSize += 2; }
        if ( getMstform() != MSTFORM_LAST && getMstform() != MSTFORM_NONE )
        { bufferSize += (shortMode ? -1 : strlen("Mstform"));
          bufferSize += strlen(FormGram::getName( getMstform() )); bufferSize += 2; }

        char *buffer = reinterpret_cast<char *>( stringAllocator.alloc( bufferSize ) );
        if ( unlikely(buffer == 0) )
            return 0;

        char *p = buffer;

        if ( getAnimate() != ANIMATE_LAST && getAnimate() != ANIMATE_NONE )
                concat( &p, "Animate", FormGram::getName( getAnimate() ), shortMode );
        if ( getCase() != CASE_LAST && getCase() != CASE_NONE )
                concat( &p, "Case", FormGram::getName( getCase() ), shortMode );
        if ( getDegree() != DEGREE_LAST && getDegree() != DEGREE_NONE )
                concat( &p, "Degree", FormGram::getName( getDegree() ), shortMode );
        if ( getGender() != GENDER_LAST && getGender() != GENDER_NONE )
                concat( &p, "Gender", FormGram::getName( getGender() ), shortMode );
        if ( getMood() != MOOD_LAST && getMood() != MOOD_NONE )
                concat( &p, "Mood", FormGram::getName( getMood() ), shortMode );
        if ( getNumber() != NUMBER_LAST && getNumber() != NUMBER_NONE )
                concat( &p, "Number", FormGram::getName( getNumber() ), shortMode );
        if ( getPerson() != PERSON_LAST && getPerson() != PERSON_NONE )
                concat( &p, "Person", FormGram::getName( getPerson() ), shortMode );
        if ( getRefl() != REFL_LAST && getRefl() != REFL_NONE )
                concat( &p, "Refl", FormGram::getName( getRefl() ), shortMode );
        if ( getShort() != SHORT_LAST && getShort() != SHORT_NONE )
                concat( &p, "Short", FormGram::getName( getShort() ), shortMode );
        if ( getTense() != TENSE_LAST && getTense() != TENSE_NONE )
                concat( &p, "Tense", FormGram::getName( getTense() ), shortMode );
        if ( getVerbform() != VERBFORM_LAST && getVerbform() != VERBFORM_NONE )
                concat( &p, "Verbform", FormGram::getName( getVerbform() ), shortMode );
        if ( getMstform() != MSTFORM_LAST && getMstform() != MSTFORM_NONE )
                concat( &p, "Mstform", FormGram::getName( getMstform() ), shortMode );

        buffer[ ( bufferSize == 0 ? 0 : bufferSize - 1 ) ] = '\0';

        return buffer;
}

