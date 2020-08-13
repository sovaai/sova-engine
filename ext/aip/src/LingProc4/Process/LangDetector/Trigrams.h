#ifndef _TRIGRAMS_H_
#define _TRIGRAMS_H_

#include <unicode/utf8.h>

#include "LangDetector.h"

#include "../../UCharSet/UCharSet.h"

template< typename TrigramType,
          const TrigramType * getTrigram( const char *, unsigned int ) >
class TrigramLangDetector : public LangDetector
{
public:
    TrigramLangDetector( float _multiplier, float _probLowerLimit ) :
        LangDetector( _probLowerLimit ),
        multiplier( _multiplier ),
        freqsSum( 0.0 ),
        trigramsProcessed( 0 )
    {
    }
    
    LingProcErrors init( LangCodes /* lang */, const UCharSet &/* charset */ )
    {
        return LP_OK;
    }
    
    LingProcErrors reset()
    {
        freqsSum          = 0.0;
        trigramsProcessed = 0;
        
        return LP_OK;
    }
    
    LingProcErrors update( const char *word, size_t wordLength )
    {
        size_t wordOffset         = 0;
        size_t trigramStartOffset = 0;
        size_t charIndex          = 0;
        
        for ( ; wordOffset < wordLength ; )
        {
            UChar32 ch;
            
            U8_NEXT_UNSAFE( word, wordOffset, ch );
            if ( ch == 0 )
                break;
            charIndex++;
            
            if ( charIndex >= 3 )
            {
                const TrigramType *t = getTrigram( word + trigramStartOffset, wordOffset - trigramStartOffset );
                if ( t != 0 )
                    freqsSum += t->freq;

                trigramsProcessed++;
                U8_NEXT_UNSAFE( word, trigramStartOffset, ch );
            }
        }

        return LP_OK;
    }
    
    LingProcErrors finish()
    {
        return LP_OK;
    }
    
    float getProbability()
    {
        return freqsSum * multiplier;
    }

    LingProcErrors save( fstorage* /* fs */, fstorage_section_id /* secId */ )
    {
        return LP_OK;
    }
    
    LingProcErrors load( fstorage* /* fs */, fstorage_section_id /* secId */, const UCharSet &charset )
    {
        return init( charset.GetLang(), charset );
    }

protected:
    float  multiplier;
    float  freqsSum;
    size_t trigramsProcessed;
};

#endif /* _TRIGRAMS_H_ */

