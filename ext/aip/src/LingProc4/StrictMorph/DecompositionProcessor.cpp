/**
 * @file   DecompositionProcessor.cpp
 * @author swizard <me@swizard.info>
 * @date   Mon Aug 25 02:51:45 2008
 * 
 * @brief  A word decomposition processor used in lemmatization (implementation).
 * 
 * 
 */
#include <_include/cc_compat.h>

#include "LexUnit.h"
#include "DecompositionProcessor.h"

DecompositionProcessor::DecompositionProcessor( StrictLex::LemmatizeFunctor &_target ) :
        FunctorMarkAdapter<const StrictLex &>( _target ),
        status( STRICT_OK ),
        currentPoolSize( 1 )
{
        chainsPool[0] = static_cast<size_t>(0);
}

DecompositionProcessor::~DecompositionProcessor()
{
}

int DecompositionProcessor::apply( const StrictLex &lex )
{
        /* Temporary install the new chain */
        int chainLength = recurseCompound( &lex, 0 );
        if ( unlikely(chainLength < 0) )
        {
                status = STRICT_ERROR_TOO_MANY_DECOMP_VARIANTS;
                return 1;
        }
        
        /* Terminate the chain with zero */
        chainsPool[currentPoolSize + chainLength] = static_cast<size_t>(0);
        chainLength++;
        
        /* Check for some bounds absorbing */

        /* Find all chains already accumulated and check them against the new one */
        bool areAbsorbed = false;
        size_t chainStart, chainEnd;
        for ( chainStart = 0, chainEnd = 1; chainEnd < currentPoolSize; chainEnd++ )
                if ( chainsPool[chainEnd] == static_cast<size_t>(0) )
                {
                        if ( checkBoundsAbsorbing( chainStart,
                                                   chainEnd,
                                                   currentPoolSize - 1,
                                                   currentPoolSize + chainLength - 1 ) )
                        {
                                areAbsorbed = true;
                                break;
                        }
                                
                        chainStart = chainEnd;
                }
                
        /* The new chain is absorbed by a previously added one: silently quit */
        if ( areAbsorbed )
                return 0;
        
        currentPoolSize += chainLength;
        return FunctorMarkAdapter<const StrictLex &>::apply( lex );
}

bool DecompositionProcessor::checkBoundsAbsorbing( size_t startA, size_t endA, size_t startB, size_t endB ) const
{
        bool    result           = false;
        bool    areStrictlyEqual = true;
        size_t *driver           = 0;
        
        /* Check if one chain can be decomposed by another */
        for (;;)
        {
                if ( startA >= endA || startB >= endB )
                        break;
                
                if ( chainsPool[startA] == chainsPool[startB] )
                {
                        /* Two bounds are equal, proceed with the next */
                        startA++;
                        startB++;
                        driver = 0;
                        result = true;
                        continue;
                }
            
                if ( driver == 0 )
                        driver = ( chainsPool[startA] < chainsPool[startB] ? &startA : &startB );

                (*driver)++;
                result           = false;
                areStrictlyEqual = false;
        }

        if ( areStrictlyEqual )
                return false;
        
        return result;
}

int DecompositionProcessor::recurseCompound( const StrictLex *lex, size_t depth )
{
        /* Error condition */
        if ( unlikely(currentPoolSize + depth + 1 >= chainsPoolSize) )
                return -1;
        
        /* Recursion termination conditions */
        if ( lex == 0 )
                return static_cast<int>( depth );
        
        const LexUnit *unit = reinterpret_cast<const LexUnit *>( lex->getShadow() );
        
        /* Sink into recursion, then install a chain */
        int length = recurseCompound( unit->leftCompound, depth + 1 );
        if ( unlikely(length < 0) )
                return length;
        
        chainsPool[ currentPoolSize + length - depth - 1] = unit->compoundBound;
        
        return length;
}

