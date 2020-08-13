#ifndef __VoiceNumberParser_hpp__
#define __VoiceNumberParser_hpp__

#include "NumberParser.hpp"
#include "LongFloat.hpp"

class VoiceNumberParser: public NumberParser::CommonParser
{
public:            
    nlReturnCode Parse( LongFloat & aResult, const char * aStr, unsigned int aStrLen = 0 );
};

    
    
#endif // __VoiceNumberParser_hpp__