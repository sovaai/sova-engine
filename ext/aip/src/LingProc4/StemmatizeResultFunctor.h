// $Id: StemmatizeResultFunctor.h 20327 2011-03-02 12:57:42Z orlovdmal $
/**
 * @file   StemmatizeResultFunctor.h
 * @author swizard <me@swizard.info>
 * @date   Tue Apr  8 02:37:07 2008
 * 
 * @brief  The functor interface for stemmatization results and its wrappers.
 * 
 * 
 */
#ifndef _STEMMATIZERESULTFUNCTOR_H_
#define _STEMMATIZERESULTFUNCTOR_H_

#include <lib/aptl/avector.h>

#include "LexID.h"
#include "LingProcErrors.h"
#include "Constants.h"

namespace LPStrict
{
    class Info;
}

/* A functor that should receive a current stemmatization result on each invocation */
class StemmatizeResultFunctor
{
public:
    /** 
     * Standard empty constructor.
     * 
     */
    StemmatizeResultFunctor() { }
	
    /** 
     * Standard empty destructor.
     * 
     */
    virtual ~StemmatizeResultFunctor() { }

public:
    /** 
     * Called by a stemmatizer just before to begin stemmatizing.
     * 
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors reset() = 0;
	
    /** 
     * Requests the client for a buffer to put the stem into it before
     * the nextStem call.
     * 
     * @param stemSize A size required to fit the stem into the buffer
     * including the terminating '\0'
     * 
     * @return The buffer pointer or NULL if something failed
     */
    virtual char *getStemBuffer( size_t stemSize ) = 0;
	
    /** 
     * Called by a stemmatizer when it creates next stem from a word.
     * 
     * @param partialLexid A LEXID struct with stem partial information
     * @param compoundNumber The number of a lexid within a compound word
     * @param stem The stem itself
     * @param stemLength A length of the stem (not counting the terminating '\0')
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors nextStem( LEXID partialLexid, size_t compoundNumber, const char *stem, size_t stemLength, const LPStrict::Info *info ) = 0;
};

/* A functor that should receive a current stemmatization result on each invocation: with predefined getStemBuffer */
class StemmatizeResultCallback : public StemmatizeResultFunctor
{
public:
    /** 
     * Standard empty constructor with all inheritance propagation.
     * 
     */
    StemmatizeResultCallback() : StemmatizeResultFunctor() { }
	
    /** 
     * Standard empty destructor with all inheritance propagation.
     * 
     */
    ~StemmatizeResultCallback() { }

public:
    /** 
     * Called by a stemmatizer just before to begin stemmatizing.
     * 
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors reset() = 0;
	
    /** 
     * Requests the client for a buffer to put the stem into it before
     * the nextStem call.
     * 
     * @param stemSize A size required to fit the stem into the buffer
     * including the terminating '\0'
     * 
     * @return The buffer pointer or NULL if something failed
     */
    char *getStemBuffer( size_t stemSize )
    {
        if ( stemSize > sizeof(stemBuffer) )
            return static_cast<char *>( 0 );
        else
            return stemBuffer;
    }
	
    /** 
     * Called by a stemmatizer when it creates next stem from a word.
     * 
     * @param partialLexid A LEXID struct with stem partial information
     * @param compoundNumber The number of a lexid within a compound word
     * @param stem The stem itself
     * @param stemLength A length of the stem (not counting the terminating '\0')
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors nextStem( LEXID partialLexid, size_t compoundNumber, const char *stem, size_t stemLength, const LPStrict::Info * /* info */ ) = 0;

protected:
    char stemBuffer[ lpMaxWordLength + 1 ]; /**< The buffer for a stem */
};

/* A buffer element for stemmatization results collector */
struct StemInfo
{
	LEXID  partialLexid;
        size_t compoundNumber;
	char   stem[ lpMaxWordLength + 1 ];
	size_t stemLength;
}; 

/* A stemmatization results collector */
class StemmatizeResultGrowingBuffer : public StemmatizeResultFunctor, public avector<StemInfo>
{
public:
    /** 
     * Standard empty constructor with all inheritance propagation.
     * 
     */
    StemmatizeResultGrowingBuffer() : StemmatizeResultFunctor(), avector<StemInfo>(), bufAllocated( false ) { }
	
    /** 
     * Standard empty destructor with all inheritance propagation.
     * 
     */
    ~StemmatizeResultGrowingBuffer() { }

public:
    /** 
     * Requests the client for a buffer to put the stem into it before
     * the nextStem call.
     * 
     * @param stemSize A size required to fit the stem into the buffer
     * including the terminating '\0'
     * 
     * @return The buffer pointer or NULL if something failed
     */
    char *getStemBuffer( size_t stemSize )
    {
        if ( stemSize > lpMaxWordLength + 1 )
            return static_cast<char *>( 0 );
        if ( bufAllocated )
            return avector<StemInfo>::operator[]( avector<StemInfo>::size() - 1 ).stem;

        StemInfo *info = avector<StemInfo>::grow();
        if ( info == 0 )
            return static_cast<char *>( 0 );
     
        bufAllocated = true;
        
        return info->stem;
    }
	
    /** 
     * Called by a stemmatizer when it creates next stem from a word.
     * 
     * @param partialLexid A LEXID struct with stem partial information
     * @param compoundNumber The number of a lexid within a compound word
     * @param stem The stem itself
     * @param stemLength A length of the stem (not counting the terminating '\0')
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    LingProcErrors nextStem( LEXID partialLexid, size_t compoundNumber, const char * /* stem */, size_t stemLength, const LPStrict::Info * /* info */ )
    {        
        StemInfo &info = avector<StemInfo>::operator[]( avector<StemInfo>::size() - 1 );

        info.partialLexid   = partialLexid;
        info.compoundNumber = compoundNumber;
        info.stemLength     = stemLength;
        bufAllocated        = false;
			
        return LP_OK;
    }

    /** 
     * Rewinds the elements counter to zero but does not free the memory.
     * 
     */
    LingProcErrors reset()
    {
        avector<StemInfo>::clear();
        bufAllocated = false;
        
        return LP_OK;
    }

protected:
    bool bufAllocated;
};

class StemmatizeResultFixedBuffer : public StemmatizeResultFunctor
{
public:
    StemmatizeResultFixedBuffer() : StemmatizeResultFunctor(), size( 0 ) { }
    ~StemmatizeResultFixedBuffer() { }

    /** 
     * Requests the client for a buffer to put the stem into it before
     * the nextStem call.
     * 
     * @param stemSize A size required to fit the stem into the buffer
     * including the terminating '\0'
     * 
     * @return The buffer pointer or NULL if something failed
     */
    char *getStemBuffer(size_t stemSize)
    {
        if ( stemSize >  lpMaxWordLength + 1 ||
             size     >= lpMaxWordLength )
            return static_cast<char *>( 0 );
		
        return array[ size ].stem;
    }
	
    /** 
     * Called by a stemmatizer when it creates next stem from a word.
     * 
     * @param partialLexid A LEXID struct with stem partial information
     * @param compoundNumber The number of a lexid within a compound word
     * @param stem The stem itself
     * @param stemLength A length of the stem (not counting the terminating '\0')
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    LingProcErrors nextStem( LEXID partialLexid, size_t compoundNumber, const char * /* stem */, size_t stemLength, const LPStrict::Info * /* info */ )
    {
        array[ size ].partialLexid   = partialLexid;
        array[ size ].compoundNumber = compoundNumber;
        array[ size ].stemLength     = stemLength;
        size++;
		
        return LP_OK;
    }

    /** 
     * Rewinds the elements counter to zero but does not free the memory.
     * 
     */
    LingProcErrors reset()
    {
        size = 0;
        return LP_OK;
    }

    /** 
     * Returns the array itself.
     * 
     * 
     * @return the array of elements
     */
    StemInfo *getArray()
    {
        return array;
    }

    /** 
     * Returns the element at the given index.
     * 
     * @param index The index of element
     * 
     * @return the element at the given index
     */
    StemInfo &operator[](size_t index)
    {
        return array[ index ];
    }

    /** 
     * Returns the array itself (constant version).
     * 
     * 
     * @return the array of elements
     */
    const StemInfo *getArray() const
    {
        return array;
    }

    /** 
     * Returns the element at the given index (constant version).
     * 
     * @param index The index of element
     * 
     * @return the element at the given index
     */
    const StemInfo &operator[]( size_t index ) const
    {
        return array[ index ];
    }
	
	
public:
    StemInfo array[ lpMaxWordLength ];
    size_t   size;
};

#endif /* _STEMMATIZERESULTFUNCTOR_H_ */

