#ifndef _LANGDETECTOR_H_
#define _LANGDETECTOR_H_

#include <stdlib.h>

#include <_include/_inttype.h>
#include <_include/language.h>

#include <lib/fstorage/fstorage.h>

#include "../../LingProcErrors.h"

class UCharSet;

class LangDetector
{
protected:
    LangDetector( float _probLowerLimit ) : probLowerLimit( _probLowerLimit ) { }

public:
    virtual ~LangDetector() { }
    
public:
    static LangDetector *factory( LangCodes lang );

public:
    virtual LingProcErrors init( LangCodes lang, const UCharSet &charset ) = 0;
    virtual LingProcErrors reset() = 0;
    virtual LingProcErrors update( const char *word, size_t wordLength ) = 0;
    virtual LingProcErrors finish() = 0;

    virtual LingProcErrors save( fstorage *fs, fstorage_section_id secId ) = 0;
    virtual LingProcErrors load( fstorage *fs, fstorage_section_id secId, const UCharSet &charset ) = 0;

    float getLangProbability()
    {
        float prob = getProbability();
        if ( prob < probLowerLimit )
            return 0.0;
        return prob;
    }
    
protected:
    virtual float getProbability() = 0;
    
protected:
    float probLowerLimit;
};

#endif /* _LANGDETECTOR_H_ */

