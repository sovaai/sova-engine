/**
 * @file   NormalizeResultFunctor.h
 * @author swizard <me@swizard.info>
 * @date   Wed Nov 12 01:26:58 2008
 * 
 * @brief  The functor interface for normalization results and its wrappers.
 * 
 * 
 */
#ifndef _NORMALIZERESULTFUNCTOR_H_
#define _NORMALIZERESULTFUNCTOR_H_

#include "LexID.h"
#include "LingProcErrors.h"

/* A functor that should receive a current normalization result on each invocation */
class NormalizeResultFunctor
{
public:
        NormalizeResultFunctor() { }
        virtual ~NormalizeResultFunctor() { }
        
public:
	/** 
	 * Called by a normalizer just before to begin normalizing.
	 * 
	 * 
	 * @return LP_OK if successfull and an error otherwise
	 */
        virtual LingProcErrors reset() = 0;

        /** 
	 * Requests the client for a buffer to put the result into it before
	 * the nextResult call.
	 * 
	 * @param requestSize A size required to fit the result into the buffer
         * including the terminating '\0'
	 * 
	 * @return The buffer pointer or NULL if something failed
	 */
        virtual char *getResultBuffer(size_t requestSize) = 0;

        /** 
         * Called by a normalizer when word normalization is complete.
         * 
         * @param partialLexid Contains the original word capitalization and a morphology
         * @param result A normalized word
         * @param resultSize Normalized word size (without terminating '\0')
         * 
	 * @return LP_OK if successfull and an error otherwise
         */
        virtual LingProcErrors nextResult( LEXID       partialLexid,
                                           const char *result,
                                           size_t      resultSize ) = 0;
};

#endif /* _NORMALIZERESULTFUNCTOR_H_ */

