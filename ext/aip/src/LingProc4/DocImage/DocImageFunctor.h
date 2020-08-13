#ifndef _DOCIMAGEFUNCTOR_H_
#define _DOCIMAGEFUNCTOR_H_

#include "DocImageTypes.h"

#include "../LemmatizeResultFunctor.h"
#include "../LPStrict.h"

class LemmatizeDocImageFunctor : public LemmatizeResultFunctor
{
private:
    // assignement operator, needed to avoid compiler's warning
    LemmatizeDocImageFunctor &operator=( const LemmatizeDocImageFunctor &)
    {
        assert(false);
        return *this;
    }

public:
    LemmatizeDocImageFunctor( DocImageTypes::ImageArray      &_imageArray,
                              DocImageTypes::WordsArray      &_wordsArray,
                              DocImageTypes::CompoundLexPool &_lexPool,
                              DocImageTypes::ChainsPool      &_chainsPool,
                              DocImageTypes::FormsSetsArray  &_imgFormsSets,
                              DocImageTypes::FormsSetsArray  &_cmpFormsSets,
                              uint16_t                        _attrs,
                              bool                            _forceForms ) :
        LemmatizeResultFunctor(),
        imageArray( _imageArray ),
        wordsArray( _wordsArray ),
        lexPool( _lexPool ),
        chainsPool( _chainsPool ),
        imgFormsSets( _imgFormsSets ),
        cmpFormsSets( _cmpFormsSets ),
        attrs( _attrs ),
        forceForms( _forceForms ),
        imageIndex( imageArray.size() ),
        chainIndex( chainsPool.size() ),
        chainOffset( lexPool.size() ),
        homoLengthOffset( chainOffset ),
        currentCompoundNumber( static_cast<size_t>(-1) ),
        currentChainLength( 0 ),
        currentChainsCount( 0 ),
        currentHomoChainLength( 0 ),
        wordImageLength( 0 )
    {
    }
    ~LemmatizeDocImageFunctor() { }

    LingProcErrors reset() { return LP_OK; }
        
    LingProcErrors storeForm( const LPStrict::Info *info, DocImageTypes::FormsSetsArray &target )
    {
        if ( !forceForms )
            return LP_OK;

        class LocalFunctor : public StrictLex::FormsSetFunctor
        {
        public:
            LocalFunctor( DocImageTypes::FormsSetsArray &_target ) : target( _target ) { }

            StrictMorphErrors apply( const StrictWordFormsSet &set )
            {
                target.push_back( set );
                if ( unlikely(target.no_memory()) )
                    return STRICT_ERROR_ALLOCATION_ERROR;
                
                return STRICT_OK;
            }
            
        private:
            DocImageTypes::FormsSetsArray &target;
        } functor( target );
        
        if ( info != 0 )
        {
            StrictMorphErrors status = info->formsSetsRetriever.retrieveFormsSets( functor );
            if ( unlikely(status != STRICT_OK) )
            {
                if ( status == STRICT_ERROR_ALLOCATION_ERROR )
                    return LP_ERROR_ENOMEM;
                return LP_ERROR_STRICT;
            }
        }
        
        return LP_OK;
    }
    
    LingProcErrors nextLemma( LEXID lexid, size_t compoundNumber, const LPStrict::Info *info )
    {
        /* A main homonym is coming -- store it in image */
        if ( compoundNumber == 0 )
        {
            /* A new main homonym is coming, save the last chain accumulated */
            LingProcErrors result = flushChain();
            if ( unlikely(result != LP_OK) )
                return result;
            
            imageArray.push_back(lexid);
            if ( unlikely(imageArray.no_memory()) )
                return LP_ERROR_ENOMEM;

            result = storeForm( info, imgFormsSets );
            if ( unlikely(result != LP_OK) )
                return result;

            wordImageLength++;
            return LP_OK;
        }

        /* A compound chain is coming -- store it in specific location */
        
        if ( currentCompoundNumber > compoundNumber )
        {
            /* A new chain is coming, save the last one accumulated */
            LingProcErrors result = flushChain();
            if ( unlikely(result != LP_OK) )
                return result;
            
            currentCompoundNumber = compoundNumber;
        }
        
        /* Check if the compound number has changed: a new decomposition chain position is encountered */
        if ( currentCompoundNumber != compoundNumber )
        {
            flushHomoChain();
            currentCompoundNumber = compoundNumber;
        }
        
        /* Allocate a homonym chain length cell if need to */
        if ( currentHomoChainLength == 0 )
        {
            lexPool.push_back(0);
            if ( unlikely(lexPool.no_memory()) )
                return LP_ERROR_ENOMEM;
        }

        /* Push the current homonym into the pool */
        lexPool.push_back(lexid);
        if ( unlikely(lexPool.no_memory()) )
            return LP_ERROR_ENOMEM;
        
        LingProcErrors result = storeForm( info, cmpFormsSets );
        if ( unlikely(result != LP_OK) )
            return result;
        
        currentHomoChainLength++;
        
        return LP_OK;
    }
    
    void flushHomoChain()
    {
        lexPool[homoLengthOffset]  = static_cast<LEXID>(currentHomoChainLength);
        
        homoLengthOffset          += currentHomoChainLength + 1;
        currentHomoChainLength     = 0;
        currentChainLength++;
    }
    
    LingProcErrors flushChain()
    {
        if ( currentHomoChainLength == 0 )
            return LP_OK;
        
        flushHomoChain();
        
        /* Create a new decomposition chain */
        DocImageTypes::DecompositionChain *chainCell = chainsPool.uninitialized_grow();
        if ( unlikely(chainCell == 0) )
            return LP_ERROR_ENOMEM;

        chainCell->chainOffset = chainOffset;
        chainCell->chainLength = currentChainLength;

        currentChainLength = 0;
        chainOffset        = homoLengthOffset;
        currentChainsCount++;

        return LP_OK;
    }
    
    LingProcErrors finish()
    {
        /* Flush a decomposition chain if any */
        LingProcErrors result = flushChain();
        if ( unlikely(result != LP_OK) )
            return result;
        
        if ( !wasLemmatizationPerformed() )
            return LP_OK;
        
        /* Append new word to words */
        DocImageTypes::DocImageWord *imgWord = wordsArray.uninitialized_grow();
        if ( unlikely(imgWord == 0) )
            return LP_ERROR_ENOMEM;

        imgWord->imgIndex    = static_cast<uint32_t>(imageIndex);
        imgWord->imgLength   = static_cast<uint16_t>(wordImageLength);
        imgWord->attrs       = attrs;
        imgWord->chainsIndex = static_cast<uint32_t>(chainIndex);
        imgWord->chainsCount = static_cast<uint32_t>(currentChainsCount);
        
        /* Clear the local counters */
        wordImageLength = 0;
        imageIndex      = imageArray.size();

        return LP_OK;
    }

    bool wasLemmatizationPerformed() const { return wordImageLength != 0 || currentChainsCount != 0 || currentChainLength != 0; }
    
private:
    DocImageTypes::ImageArray      &imageArray;
    DocImageTypes::WordsArray      &wordsArray;
    DocImageTypes::CompoundLexPool &lexPool;
    DocImageTypes::ChainsPool      &chainsPool;
    DocImageTypes::FormsSetsArray  &imgFormsSets;
    DocImageTypes::FormsSetsArray  &cmpFormsSets;
    uint16_t                        attrs;
    bool                            forceForms;

    size_t imageIndex;
    size_t chainIndex;
    size_t chainOffset;
    size_t homoLengthOffset;
    size_t currentCompoundNumber;
    size_t currentChainLength;
    size_t currentChainsCount;
    size_t currentHomoChainLength;
    size_t wordImageLength;
};

#endif /* _DOCIMAGEFUNCTOR_H_ */

