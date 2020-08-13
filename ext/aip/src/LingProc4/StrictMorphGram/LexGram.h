/**
 * @file   LexGram.h
 * @author swizard <me@swizard.info>
 * @date   Mon Oct 19 22:50:28 2009
 *
 * @brief  Lexeme grammatical description.
 *
 *
 */
#ifndef _LEXGRAM_H_
#define _LEXGRAM_H_

#include "Shadowed.h"

class SM_DLL LexGram : virtual public Shadowed
{
public:
    enum PartOfSpeech
    {
        PART_NONE         = 0,
        PART_NOUN         = 1,
        PART_ADJ          = 2,
        PART_VERB         = 3,
        PART_ADV          = 4,
        PART_NUM          = 5,
        PART_PRON         = 6,
        PART_PREP         = 10,
        PART_CONJ         = 11,
        PART_PARTICLE     = 12,
        PART_INTERJECTION = 13,
        PART_ARTICLE      = 14,
        PART_UNDEF        = 100,
        PART_LAST         = 1000
    };

    enum Subtype
    {
        SUBTYPE_NONE          = 0,
        SUBTYPE_SURNAME       = 10,
        SUBTYPE_NAME          = 11,
        SUBTYPE_PATRONYMIC    = 12,
        SUBTYPE_GEO           = 13,
        SUBTYPE_PROPER        = 14,
        SUBTYPE_ABBR	      = 15,
        SUBTYPE_NUMER	      = 16,
        SUBTYPE_QUALITATIVE   = 20,
        SUBTYPE_RELATIVE      = 21,
        SUBTYPE_POSSESSIVE    = 22,
        SUBTYPE_IMPERSONAL    = 30,
        SUBTYPE_FREQUENTATIVE = 31,
        SUBTYPE_PREDICATIVE   = 40,
        SUBTYPE_COMPARATIVE   = 41,
        SUBTYPE_PARENTHESIS   = 42,
        SUBTYPE_CARDINAL      = 50,
        SUBTYPE_ORDINAL       = 51,
        SUBTYPE_COLLECTIVE    = 52,
        SUBTYPE_ADJECTIVE     = 60,
        SUBTYPE_PERSONAL      = 61,
        SUBTYPE_REFL          = 62,
        SUBTYPE_CONNECTIVE    = 70,
        SUBTYPE_NEGATIVE      = 71,
        SUBTYPE_UNDEF         = 72,
        SUBTYPE_DEF           = 73,
        SUBTYPE_LAST          = 1000
    };

    enum Suppl
    {
        SUPPL_NONE    = 0,
        SUPPL_SHORTEN = 1,
        SUPPL_SUBST   = 2,
        SUPPL_ADJ     = 3,
        SUPPL_LAST    = 1000
    };

    enum Gender
    {
        GENDER_NONE            = 0,
        GENDER_MASC            = 1,
        GENDER_FEM             = 2,
        GENDER_NEUT            = 3,
        GENDER_COMMON          = 4,
        GENDER_PLURAL          = 5,
        GENDER_UNDEF           = 6,
        GENDER_MASC_N_NEUT     = 7,
        GENDER_FEM_N_NEUT      = 8,
        GENDER_MASC_FEM_N_NEUT = 9,
        GENDER_LAST            = 1000
    };

    enum Number
    {
        NUMBER_NONE = 0,
        NUMBER_SG   = 1,
        NUMBER_PL   = 2,
        NUMBER_LAST = 1000
    };

    enum Animate
    {
        ANIMATE_NONE      = 0,
        ANIMATE_ANIMATE   = 1,
        ANIMATE_INANIMATE = 2,
        ANIMATE_BOTH      = 3,
        ANIMATE_LAST      = 1000
    };

    enum Aspect
    {
        ASPECT_NONE   = 0,
        ASPECT_IMPERF = 1,
        ASPECT_PERF   = 2,
        ASPECT_BOTH   = 3,
        ASPECT_LAST   = 1000
    };

    enum Transitivity
    {
        TRANSITIVITY_NONE    = 0,
        TRANSITIVITY_TRANS   = 1,
        TRANSITIVITY_INTRANS = 2,
        TRANSITIVITY_LAST    = 1000
    };

    enum Refl
    {
        REFL_NONE = 0,
        REFL_REFL = 2,
        REFL_LAST = 1000
    };

public:
    Refl         getRefl()         const;
    Transitivity getTransitivity() const;
    Aspect       getAspect()       const;
    Animate      getAnimate()      const;
    Number       getNumber()       const;
    Gender       getGender()       const;
    Suppl        getSuppl()        const;
    Subtype      getSubtype()      const;
    PartOfSpeech getPartOfSpeech() const;

    static const char *getName( PartOfSpeech value );
    static const char *getName( Refl value );
    static const char *getName( Transitivity value );
    static const char *getName( Aspect value );
    static const char *getName( Animate value );
    static const char *getName( Number value );
    static const char *getName( Gender value );
    static const char *getName( Suppl value );
    static const char *getName( Subtype value );

    const char *getName() const;
    const char *describe( StrictAllocator &stringAllocator, bool shortMode = false ) const;

public:
    LexGram() : Shadowed() { }
    ~LexGram() { }
};

#endif /* _LEXGRAM_H_ */

