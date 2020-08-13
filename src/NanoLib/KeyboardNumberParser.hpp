#ifndef __KeyboardNumberParser_hpp__
#define __KeyboardNumberParser_hpp__

#include "NumberParser.hpp"
#include "LongFloat.hpp"

class KeyboardNumberParser: public NumberParser::CommonParser
{
public:            
    nlReturnCode Parse( LongFloat & aResult, const char * aStr, unsigned int aStrLen = 0 );
};

    
    
#endif // __KeyboardNumberParser_hpp__