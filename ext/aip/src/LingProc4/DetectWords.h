#ifndef _DETECTWORDS_H_
#define _DETECTWORDS_H_

#include <stdlib.h>

#include "LingProcDict.h"

class DetectWords : public LingProcDict
{
public:
    DetectWords() : LingProcDict() { }
    ~DetectWords() { }

    LingProcErrors Init( LangCodes lang, const UCharSet *charset );

    /* returns 0 - not detected, 1 - charset match, 2 - word match,
       enum LingProcErrors - on errors */
    int CheckWord( const char *word, size_t len = static_cast<size_t>(-1) ) const;

private:
    // copy prevention
    DetectWords( const DetectWords &src ) : LingProcDict( src ) { }
    DetectWords &operator=( const DetectWords & ) { assert(false); abort(); return *this; }
};

#endif /* _DETECTWORDS_H_ */

