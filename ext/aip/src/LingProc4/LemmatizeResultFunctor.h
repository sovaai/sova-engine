/**
 * @file   LemmatizeResultFunctor.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr  7 19:25:58 2008
 * 
 * @brief  The functor interface for lemmatization results and its wrappers.
 * 
 * 
 */
#ifndef _LEMMATIZERESULTFUNCTOR_H_
#define _LEMMATIZERESULTFUNCTOR_H_

#include <lib/aptl/avector.h>

#include "LexID.h"
#include "LingProcErrors.h"

namespace LPStrict
{
    class Info;
}

/* A functor that should receive a current lemmatization result on each invocation */
class LemmatizeResultFunctor
{
public:
    /** 
     * Standard empty constructor.
     * 
     */
    LemmatizeResultFunctor() { }
    /** 
     * Standard empty destructor.
     * 
     */
    virtual ~LemmatizeResultFunctor() { }

public:
    /** 
     * Called by a lemmatizer just before to begin lemmatizing.
     * 
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors reset() = 0;

    /** 
     * Called by a lemmatizer when it creates next lemma from a word.
     * 
     * @param lexid The current LEXID
     * @param compoundNumber The number of a lexid within a compound word
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors nextLemma( LEXID lexid, size_t compoundNumber, const LPStrict::Info *info ) = 0;
};

struct LemmatizeBufferEntry
{
    LEXID  lex;
    size_t compoundNumber;
};

/* A lemmatization results collector (based on GrowingPool) */
class LemmatizeResultBuffer : public LemmatizeResultFunctor, public avector<LemmatizeBufferEntry>
{
public:
    /** 
     * Standard empty constructor with all inheritance propagation.
     * 
     */
    LemmatizeResultBuffer() : LemmatizeResultFunctor(), avector<LemmatizeBufferEntry>() { }
    /** 
     * Standard empty destructor with all inheritance propagation.
     * 
     */
    ~LemmatizeResultBuffer() { }

public:
    /** 
     * Called by a lemmatizer just before to begin lemmatizing.
     * 
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors reset()
    {
        avector<LemmatizeBufferEntry>::clear();
        return LP_OK;
    }

    /** 
     * Called by a lemmatizer when it creates next lemma from a word.
     * 
     * @param lexid The current LEXID
     * @param compoundNumber The number of a lexid within a compound word
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors nextLemma(LEXID lexid, size_t compoundNumber, const LPStrict::Info * /* info */)
    {
        LemmatizeBufferEntry *entry = avector<LemmatizeBufferEntry>::grow();
        if ( entry == 0 )
            return LP_ERROR_ENOMEM;
        entry->lex            = lexid;
        entry->compoundNumber = compoundNumber;
        return LP_OK;
    }
};

/* A lemmatization results collector with given storage target */
class LemmatizeResultTarget : public LemmatizeResultFunctor
{
public:
    /** 
     * Standard empty constructor with all inheritance propagation.
     * 
     */
    LemmatizeResultTarget( LemmatizeBufferEntry *_array, size_t _arraySize ) :
        LemmatizeResultFunctor(), array( _array ), arraySize( _arraySize ), index( 0 ) { }
    /** 
     * Standard empty destructor with all inheritance propagation.
     * 
     */
    ~LemmatizeResultTarget() { }

public:
    /** 
     * Called by a lemmatizer just before to begin lemmatizing.
     * 
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors reset()
    {
        index = 0;
        return LP_OK;
    }

    /** 
     * Called by a lemmatizer when it creates next lemma from a word.
     * 
     * @param lexid The current LEXID
     * @param compoundNumber The number of a lexid within a compound word
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors nextLemma( LEXID lexid, size_t compoundNumber, const LPStrict::Info * /* info */ )
    {
        if ( index < arraySize )
        {
            array[index].lex            = lexid;
            array[index].compoundNumber = compoundNumber;
        }
        index++;
        return LP_OK;
    }

public:
    LemmatizeBufferEntry *array;
    size_t                arraySize;
    size_t                index;
};

class LemmatizeResultSimpleTarget : public LemmatizeResultFunctor
{
public:
    /** 
     * Standard empty constructor with all inheritance propagation.
     * 
     */
    LemmatizeResultSimpleTarget( LEXID *_array, size_t _arraySize ) :
        LemmatizeResultFunctor(), array( _array ), arraySize( _arraySize ), index( 0 ) { }
    /** 
     * Standard empty destructor with all inheritance propagation.
     * 
     */
    ~LemmatizeResultSimpleTarget() { }

public:
    /** 
     * Called by a lemmatizer just before to begin lemmatizing.
     * 
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors reset()
    {
        index = 0;
        return LP_OK;
    }

    /** 
     * Called by a lemmatizer when it creates next lemma from a word.
     * 
     * @param lexid The current LEXID
     * @param compoundNumber The number of a lexid within a compound word
     * 
     * @return LP_OK if successfull and an error otherwise
     */
    virtual LingProcErrors nextLemma( LEXID lexid, size_t compoundNumber, const LPStrict::Info * /* info */ )
    {
        if ( compoundNumber == 0 )
        {
            if ( index < arraySize )
                array[index] = lexid;
            index++;
        }
        return LP_OK;
    }

public:
    LEXID  *array;
    size_t  arraySize;
    size_t  index;
};

#endif /* _LEMMATIZERESULTFUNCTOR_H_ */

