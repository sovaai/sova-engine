/**
 * @file   FormGram.h
 * @author swizard <me@swizard.info>
 * @date   Tue Oct 20 01:39:51 2009
 * 
 * @brief  Word form grammatical description.
 * 
 * 
 */
#ifndef _FORMGRAM_H_
#define _FORMGRAM_H_

#include "Shadowed.h"

class SM_DLL FormGram : public Shadowed
{
public:
    enum Animate
    {
        ANIMATE_NONE      = 0,
        ANIMATE_ANIMATE   = 1,
        ANIMATE_INANIMATE = 2,
        ANIMATE_LAST      = 3
    };

    enum Case
    {
        CASE_NONE   = 0,
        CASE_NOM    = 1,
        CASE_GEN    = 2,
        CASE_DAT    = 3,
        CASE_ACC    = 4,
        CASE_INST   = 5,
        CASE_PREP   = 6,
        CASE_COMMON = 7,
        CASE_POSS   = 8,
        CASE_LAST   = 9
    };

    enum Degree
    {
        DEGREE_NONE  = 0,
        DEGREE_POS   = 1,
        DEGREE_COMP  = 2,
        DEGREE_SUPER = 3,
        DEGREE_LAST  = 4
    };

    enum Gender
    {
        GENDER_NONE = 0,
        GENDER_MASC = 1,
        GENDER_FEM  = 2,
        GENDER_NEUT = 3,
        GENDER_LAST = 4
    };
	
    enum Mood
    {
        MOOD_NONE        = 0,
        MOOD_INF         = 1,
        MOOD_INDICATIVE  = 2,
        MOOD_IMPER       = 3,
        MOOD_SUBJ        = 4,
        MOOD_ADV_PART    = 5,
        MOOD_PART_I      = 6,
        MOOD_PART_ACT    = 6,
        MOOD_PART_II     = 7,
        MOOD_PART_PASS   = 7,
        MOOD_OBSOLETE1   = 8, // to be deleted after fix of german data
        MOOD_OBSOLETE2   = 9, // to be deleted after fix of german data
        MOOD_LAST        = 10
    };

    enum Number
    {
        NUMBER_NONE = 0,
        NUMBER_SG   = 1,
        NUMBER_PL   = 2,
        NUMBER_LAST = 3
    };

    enum Person
    {
        PERSON_NONE = 0,
        PERSON_1ST  = 1,
        PERSON_2ND  = 2,
        PERSON_3RD  = 3,
        PERSON_LAST = 4
    };

    enum Refl
    {
        REFL_NONE     = 0,
        REFL_NOT_REFL = 1,
        REFL_REFL     = 2,
        REFL_LAST     = 3
    };

    enum Short
    {
        SHORT_NONE  = 0,
        SHORT_FULL  = 1,
        SHORT_SHORT = 2,
        SHORT_LAST  = 3
    };

    enum Tense
    {
        TENSE_NONE     = 0,
        TENSE_PRES     = 1,
        TENSE_PAST     = 2,
        TENSE_FUTUR    = 3,
        TENSE_LAST     = 4
    };

    enum Verbform
    {
        VERBFORM_NONE    = 0,
        VERBFORM_INF     = 1,
        VERBFORM_PRES1ST = 2,
        VERBFORM_PRES2ND = 3,
        VERBFORM_PRES3RD = 4,
        VERBFORM_PART1   = 5,
        VERBFORM_PAST    = 6,
        VERBFORM_PASTSG  = 7,
        VERBFORM_PASTPL  = 8,
        VERBFORM_PART2   = 9,
        VERBFORM_LAST    = 10
    };

    enum Mstform
    {
        MSTFORM_NONE     = 0,
        MSTFORM_DIRECT   = 1,
        MSTFORM_INDIRECT = 2,
        MSTFORM_ABS      = 3,
        MSTFORM_LAST     = 4
    };
    
public:
    Mstform  getMstform()  const;
    Verbform getVerbform() const;
    Tense    getTense()    const;
    Short    getShort()    const;
    Refl     getRefl()     const;
    Person   getPerson()   const;
    Number   getNumber()   const;
    Mood     getMood()     const;
    Gender   getGender()   const;
    Degree   getDegree()   const;
    Case     getCase()     const;
    Animate  getAnimate()  const;
    
    static const char *getName( Mstform value );
    static const char *getName( Verbform value );
    static const char *getName( Animate value );
    static const char *getName( Tense value );
    static const char *getName( Short value );
    static const char *getName( Refl value );
    static const char *getName( Person value );
    static const char *getName( Number value );
    static const char *getName( Mood value );
    static const char *getName( Gender value );
    static const char *getName( Degree value );
    static const char *getName( Case value );
    
    const char *getName() const;
    const char *describe( StrictAllocator &stringAllocator, bool shortMode = false ) const;
    
    typedef unsigned char ParadigmType;
    ParadigmType getParadigmType() const;
    
public:
    FormGram() : Shadowed() { }
    ~FormGram() { }
};

#endif /* _FORMGRAM_H_ */

