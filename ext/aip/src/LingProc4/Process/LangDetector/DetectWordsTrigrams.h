#ifndef _DETECTWORDSTRIGRAMS_H_
#define _DETECTWORDSTRIGRAMS_H_

#include <_include/cc_compat.h>

#include "Trigrams.h"

#include "../../DetectWords.h"

template< typename TrigramType,
          const TrigramType * getTrigram( const char *, unsigned int ) >
class DetectWordsTrigramLangDetector : public TrigramLangDetector< TrigramType, getTrigram >,
                                       public DetectWords
{
private:
    typedef TrigramLangDetector< TrigramType, getTrigram > BaseDetector;
    
public:
    DetectWordsTrigramLangDetector( float _trigramsMultiplier, float _detectMatchCoeff, float _probLowerLimit ) :
        BaseDetector( _trigramsMultiplier, _probLowerLimit ),
        DetectWords(),
        wordsTotal( 0 ),
        wordsMatch( 0 ),
        charsetMatch( 0 ),
        detectMatchCoeff( _detectMatchCoeff )
    { }

public:
    LingProcErrors init( LangCodes lang, const UCharSet &charset )
    {
        LingProcErrors status = BaseDetector::init( lang, charset );
        if ( unlikely(status != LP_OK) )
            return status;
        
        return DetectWords::Init( lang, &charset );
    }
    
    LingProcErrors reset()
    {
        LingProcErrors status = BaseDetector::reset();
        if ( unlikely(status != LP_OK) )
            return status;
        
        wordsTotal   = 0;
        wordsMatch   = 0;
        charsetMatch = 0;

        return LP_OK;
    }
    
    LingProcErrors update( const char *word, size_t wordLength )
    {
        LingProcErrors status = BaseDetector::update( word, wordLength );
        if ( unlikely(status != LP_OK) )
            return status;
        
        int detect = DetectWords::CheckWord( word, wordLength );
        if ( unlikely(detect < 0) )
            return LP_ERROR_DETECTOR;
        if ( detect > 0 )
            charsetMatch++;
        if ( detect == 2 )
            wordsMatch++;

        wordsTotal++;
        
        return LP_OK;
    }
    
    LingProcErrors finish()
    {
        LingProcErrors status = BaseDetector::finish();
        if ( unlikely(status != LP_OK) )
            return status;
        
        return LP_OK;
    }
    
    float getProbability()
    {
        float baseProbability = BaseDetector::getProbability();

        return baseProbability * ( 1.0 + ( ( detectMatchCoeff * static_cast<float>(wordsMatch) ) / static_cast<float>(wordsTotal) ) );
    }

    LingProcErrors save( fstorage *fs, fstorage_section_id secId )
    {
        LingProcErrors status = BaseDetector::save( fs, secId );
        if ( unlikely(status != LP_OK) )
            return status;
        
        return DetectWords::Save( fs, secId );
    }
    
    LingProcErrors load( fstorage *fs, fstorage_section_id secId, const UCharSet &charset )
    {
        LingProcErrors status = BaseDetector::load( fs, secId, charset );
        if ( unlikely(status != LP_OK) )
            return status;
        
        return DetectWords::Load( fs, secId, &charset );
    }
    
protected:
    size_t wordsTotal;
    size_t wordsMatch;
    size_t charsetMatch;
    float  detectMatchCoeff;
};

#endif /* _DETECTWORDSTRIGRAMS_H_ */

