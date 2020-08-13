/**
 * @file   StrictWordFormsSet.h
 * @author swizard <me@swizard.info>
 * @date   Thu Oct 22 02:16:35 2009
 * 
 * @brief  A set of word forms for a lexeme.
 * 
 * 
 */
#ifndef _STRICTWORDFORMSSET_H_
#define _STRICTWORDFORMSSET_H_

#include "Functor.h"
#include "Shadowed.h"
#include "StrictWordForm.h"
#include "StrictMorphErrors.h"

class SM_DLL StrictWordFormsSet : public Shadowed
{  
public:
    typedef Functor<const StrictWordForm &, StrictMorphErrors> EnumerateFunctor;
    
public:
    StrictMorphErrors enumerate( EnumerateFunctor &functor ) const;
    
public:
    StrictWordFormsSet() : Shadowed() { }
    ~StrictWordFormsSet() { }
};

#endif /* _WORDFORMSSET_H_ */

