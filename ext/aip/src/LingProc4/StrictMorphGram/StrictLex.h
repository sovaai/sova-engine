/**
 * @file   StrictLex.h
 * @author swizard <me@swizard.info>
 * @date   Thu Oct 22 02:39:53 2009
 * 
 * @brief  Lexeme information.
 * 
 * 
 */
#ifndef _STRICTLEX_H_
#define _STRICTLEX_H_

#include <stdlib.h>

#include "Functor.h"
#include "Shadowed.h"
#include "LexGram.h"
#include "FormFilter.h"
#include "StrictWordForm.h"
#include "StrictMorphErrors.h"
#include "StrictWordFormsSet.h"
#include "StrictLexOrthVariant.h"

class SM_DLL StrictLex : public Shadowed
{  
public:
    enum DictCaps
    {
        CAPS_NO_CASE    = 0,
        CAPS_LOWER_CASE = 1,
        CAPS_TITLE_CASE = 2,
        CAPS_UPPER_CASE = 3,
        
        CAPS_LAST       = 4
    };
    
    enum SynthesisFlagsValues
    {
        SYNTH_STANDARD         = 0x00000000,
        SYNTH_WITH_BLANKS      = 0x00000001,
        SYNTH_WITH_EXTRA_FORMS = 0x00000002
    };
    
public:
    typedef Functor<const StrictLex &>                               LemmatizeFunctor;
    typedef Functor<const LexGram &, StrictMorphErrors>              GramEnumFunctor;
    typedef Functor<const StrictLexOrthVariant &, StrictMorphErrors> OrthEnumFunctor;
    typedef Functor<const StrictWordFormsSet &, StrictMorphErrors>   FormsSetFunctor;
    typedef Functor<const StrictLex &, StrictMorphErrors>            BuildFunctor;
    
    typedef unsigned int SynthesisFlags;

public:
    /* Functor for text retrieval */
    class TextFunctor : public Functor<const char *, StrictMorphErrors>
    {
    public:
        virtual char *allocateBuffer(size_t size) = 0;
    };

public:
    size_t    getLexNo()    const; // Lexeme number of a main orth variant
    DictCaps  getDictCaps() const;

    const StrictLexOrthVariant getVariant() const;
    const StrictLexOrthVariant getVariant( size_t orthVariant ) const;
    StrictMorphErrors queryVariants( OrthEnumFunctor &functor )  const;
    StrictMorphErrors queryLexGrams( GramEnumFunctor &functor )  const;
    StrictMorphErrors queryFormsSets( FormsSetFunctor &functor ) const;
    
    StrictMorphErrors synthesizeParadigm( StrictWordFormsSet::EnumerateFunctor &functor,
                                          SynthesisFlags                        flags,
                                          const FormFilter                     *filter ) const;

    StrictMorphErrors restoreText( TextFunctor &functor ) const;
    StrictMorphErrors restoreText( TextFunctor &functor, const StrictWordForm &customForm ) const;
        
public:
    const StrictLex *getLeftCompound()  const;
    size_t           getCompoundBound() const;
    unsigned int     getMorphNumber()   const;
    
public:
    template<typename MorphType>
    static StrictMorphErrors buildStrictLex( const MorphType &morphLink,
                                             BuildFunctor    &functor,
                                             size_t           lexNo,
                                             size_t           orthVariant );
    
    template<typename MorphType>
    static StrictMorphErrors buildStrictLex( const MorphType &morphLink,
                                             BuildFunctor    &functor,
                                             size_t           lexNo,
                                             size_t           orthVariant,
                                             size_t           formNo,
                                             size_t           formVariant );

    template<typename MorphType>
    static StrictMorphErrors restoreText( const MorphType &morphLink,
                                          TextFunctor     &functor,
                                          size_t           lexNo,
                                          size_t           orthVariant,
                                          size_t           formNo,
                                          size_t           formVariant );
    
public:
    StrictLex() : Shadowed() { }
    ~StrictLex() { }
};

#endif /* _STRICTLEX_H_ */

