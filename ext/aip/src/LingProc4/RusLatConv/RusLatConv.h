#ifndef _RUSLATCONV_H_
#define _RUSLATCONV_H_

#include <assert.h>
#include <stdlib.h>

#include <unicode/utf8.h>

#include "../IndirectTable.h"
#include "../StringsReceiver.h"

enum RLCConversions
{
  RLC_NONE   = 0,
  RLC_TO_LAT = 1,
  RLC_TO_RUS = 2
};

enum RLCTables
{
  RLCT_NONE       = 0x0000,
  RLCT_RUS_TO_LAT = 0x0001,
  RLCT_DIG_TO_LAT = 0x0002,
  RLCT_LAT_TO_RUS = 0x0004,
  RLCT_DIG_TO_RUS = 0x0008,
  RLCT_ALL        = ( RLCT_RUS_TO_LAT |
                      RLCT_DIG_TO_LAT |
                      RLCT_LAT_TO_RUS |
                      RLCT_DIG_TO_RUS )
};

class RusLatConverter
{
public:
    RusLatConverter() { Init(); }
    ~RusLatConverter() { }

    enum tune_modes
    {
        tmStandard = 0x00000000u,
        tmNoDigits = 0x00004000u
    };
  
    LingProcErrors Init( unsigned tuneflags = tmStandard );

    unsigned int Detect( const char *word,
                         size_t      wlen = static_cast<size_t>(-1) );
  
    RLCConversions Check( const char     *word,
                          size_t          wlen                = static_cast<size_t>(-1),
                          RLCConversions  conflict_conversion = RLC_NONE,
                          RLCConversions  default_conversion  = RLC_NONE,
                          unsigned int    tables              = RLCT_ALL );

    void Convert( RLCConversions   conversion,
                  StringsReceiver &resultReceiver,
                  const char      *word,
                  size_t           wlen   = static_cast<size_t>(-1),
                  unsigned int     tables = RLCT_ALL );

    void Adjust( StringsReceiver &resultReceiver,
                 const char      *word,
                 size_t           wlen                = static_cast<size_t>(-1),
                 RLCConversions   conflict_conversion = RLC_NONE,
                 RLCConversions   default_conversion  = RLC_NONE )
    {
        Convert( Check( word, wlen, conflict_conversion, default_conversion ), resultReceiver, word, wlen );
    }

private:
    unsigned int DetectLoop( const char *word, size_t wlen, bool &to_lat, bool &to_rus, unsigned int tables );
  
protected:
    // copy prevention
    RusLatConverter( const RusLatConverter & ) { assert(false); abort(); }
    RusLatConverter &operator=( const RusLatConverter & ) { assert(false); abort(); return *this; }

protected:    
    // tables pointers
    IndirectTable uniqueChars;
    IndirectTable rusToLat;
    IndirectTable latToRus;
};

#endif /* _RUSLATCONV_H_ */

