
#include <unicode/uchar.h>

#include "../LingProcErrors.h"

#include "ruslat.h"
#include "RusLatConv.h"

LingProcErrors RusLatConverter::Init( unsigned tuneflags /* = tmStandard */ )
{
    for ( size_t i = 0; i < sizeof(unique_lat) / sizeof(uint32_t); i++ )
        uniqueChars.setValue( unique_lat[i], RLC_TO_LAT );
    for ( size_t i = 0; i < sizeof(unique_rus) / sizeof(uint32_t); i++ )
        uniqueChars.setValue( unique_rus[i], RLC_TO_RUS );
    for ( size_t i = 0; i < sizeof(map_to_lat) / sizeof(uint32_t) / 2; i++ )
        rusToLat.setValue( map_to_lat[i][0], map_to_lat[i][1] );
    for ( size_t i = 0; i < sizeof(map_to_rus) / sizeof(uint32_t) / 2; i++ )
        latToRus.setValue( map_to_rus[i][0], map_to_rus[i][1] );

    if ( (tuneflags & tmNoDigits) != 0 )
    {
        rusToLat.clearValue( 0x00000030 );
        rusToLat.clearValue( 0x00000031 );
        rusToLat.clearValue( 0x00000034 );

        latToRus.clearValue( 0x00000030 );
        latToRus.clearValue( 0x00000033 );
        latToRus.clearValue( 0x00000036 );
    }
    
    return LP_OK;
}

unsigned int RusLatConverter::DetectLoop( const char   *word,
                                          size_t        wlen,
                                          bool         &to_lat,
                                          bool         &to_rus,
                                          unsigned int  tables )
{
    // check word
    unsigned int res = RLC_NONE;
    to_lat           = false;
    to_rus           = false;
    bool digits_only = true;

    for ( size_t offset = 0; offset < wlen; )
    {
        UChar32 ch;
        U8_NEXT_UNSAFE( word, offset, ch );
        if ( ch == static_cast<UChar32>('\0') )
            break;
        
        res |= uniqueChars.getValue( ch );
        
        if ( ((!u_isdigit( ch ) && tables & RLCT_RUS_TO_LAT) ||
              (u_isdigit( ch )  && tables & RLCT_DIG_TO_LAT)) && rusToLat.getValue( ch ) != 0 )
            to_lat = true;
        if ( ((!u_isdigit( ch ) && tables & RLCT_LAT_TO_RUS) ||
              (u_isdigit( ch )  && tables & RLCT_DIG_TO_RUS)) && latToRus.getValue( ch ) != 0 )
            to_rus = true;
        if ( !u_isdigit( ch ) )
            digits_only = false;
    }

    // no conversions for numbers (consisting of didgits only)
    if ( digits_only )
        return RLC_NONE;
    
    return res;
}

unsigned int RusLatConverter::Detect( const char *word,
                                      size_t      wlen /* = size_t(-1) */ )
{
    bool to_rus = false;
    bool to_lat = false;
    return DetectLoop( word, wlen, to_lat, to_rus, RLCT_ALL );
}

RLCConversions RusLatConverter::Check( const char     *word,
                                       size_t          wlen                /* = size_t(-1) */,
                                       RLCConversions  conflict_conversion /* = RLC_NONE   */,
                                       RLCConversions  default_conversion  /* = RLC_NONE   */,
                                       unsigned int    tables              /* = RLCT_ALL   */ )
{
    bool         to_rus = false;
    bool         to_lat = false;
    unsigned int res    = DetectLoop( word, wlen, to_lat, to_rus, tables );
    
    // adjust conversion type
    if ( res == (RLC_TO_RUS | RLC_TO_LAT) )
        res = conflict_conversion;
    else if ( res == RLC_NONE )
        res = default_conversion;

    // check if conversion is possible
    if ( res == RLC_TO_LAT && !to_lat )
        res = RLC_NONE;
    else if ( res == RLC_TO_RUS && !to_rus )
        res = RLC_NONE;

    return static_cast<RLCConversions>(res); 
}

void RusLatConverter::Convert( RLCConversions   conversion,
                               StringsReceiver &resultReceiver,
                               const char      *word,
                               size_t           wlen   /* = size_t(-1) */,
                               unsigned int     tables /* = RLCT_ALL   */ )
{
    IndirectTable *conv_table = 0;
    
    bool allow_digits  = false;
    bool allow_symbols = false;
    
    if ( conversion == RLC_TO_LAT )
    {
        conv_table = &rusToLat;
        if ( tables & RLCT_RUS_TO_LAT )
            allow_symbols = true;
        if ( tables & RLCT_DIG_TO_LAT )
            allow_digits  = true;
    }
    else if ( conversion == RLC_TO_RUS )
    {
        conv_table = &latToRus;
        if ( tables & RLCT_LAT_TO_RUS )
            allow_symbols = true;
        if ( tables & RLCT_DIG_TO_RUS )
            allow_digits  = true;
    }

    if ( conv_table == 0 )
        return;
    
    char *out_buffer= resultReceiver.requestBuffer( (wlen == static_cast<size_t>(-1) ? strlen( word ) : wlen) * 4 );
    if ( out_buffer == 0 )
        return;

    size_t offset = 0;
    
    for ( size_t srcOffset = 0; srcOffset < wlen; )
    {
        UChar32 ch = 0;
        U8_NEXT_UNSAFE( word, srcOffset, ch );
        if ( ch == static_cast<UChar32>( '\0' ) )
            break;
        
        if ( u_isdigit( ch ) )
        {
            if ( !allow_digits )
            {
                U8_APPEND_UNSAFE( out_buffer, offset, ch );
                continue;
            }
        }
        else
        {
            if ( !allow_symbols )
            {
                U8_APPEND_UNSAFE( out_buffer, offset, ch );
                continue;
            }
        }

        UChar32 map_ch = conv_table->getValue( ch );
        if ( map_ch == 0 )
        {
            U8_APPEND_UNSAFE( out_buffer, offset, ch );
        }
        else
        {
            U8_APPEND_UNSAFE( out_buffer, offset, map_ch );
        }        
    }

    resultReceiver.commitString( offset );
}

