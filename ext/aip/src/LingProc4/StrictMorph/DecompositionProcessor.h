/**
 * @file   DecompositionProcessor.h
 * @author swizard <me@swizard.info>
 * @date   Mon Aug 25 02:44:19 2008
 * 
 * @brief  A word decomposition processor used in lemmatization.
 * 
 * 
 */
#ifndef _DECOMPOSITIONPROCESSOR_H_
#define _DECOMPOSITIONPROCESSOR_H_

#include <lib/aptl/Functor.h>

#include "API/StrictMorph.h"

class DecompositionProcessor : public FunctorMarkAdapter<const StrictLex &>
{
private:
        enum Constants
        {
                chainsPoolSize = 1536
        };
        
public:
        DecompositionProcessor( StrictLex::LemmatizeFunctor &_target );
        ~DecompositionProcessor();
        
public:
        int apply( const StrictLex &lex );

        StrictMorphErrors getStatus() const { return status; }

private:
        int recurseCompound( const StrictLex *lex, size_t depth );
        bool checkBoundsAbsorbing( size_t startA, size_t endA, size_t startB, size_t endB ) const;
        
private:
        StrictMorphErrors              status;
        
        size_t currentPoolSize;
        size_t chainsPool[chainsPoolSize];
};

#endif /* _DECOMPOSITIONPROCESSOR_H_ */

