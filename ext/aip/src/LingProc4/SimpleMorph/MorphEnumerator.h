/**
 * @file   MorphEnumerator.h
 * @author swizard <me@swizard.info>
 * @date   Tue Jun 10 15:43:11 2008
 * 
 * @brief  Functor object for morphology enumeration.
 * 
 * 
 */
#ifndef _MORPHENUMERATOR_H_
#define _MORPHENUMERATOR_H_

#include "../LexID.h"
#include "../LingProcErrors.h"

/* An enumerator functor object: inherit from it to enumerate and overlap its 'nextLexid' method */
class MorphEnumerator
{
public:
    MorphEnumerator() { }
    virtual ~MorphEnumerator() { }

    virtual LingProcErrors nextLexid( LEXID lexid ) = 0;
};

#endif /* _MORPHENUMERATOR_H_ */

