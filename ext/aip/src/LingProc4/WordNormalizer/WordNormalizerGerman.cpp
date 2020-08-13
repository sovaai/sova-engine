
#include <_include/cc_compat.h>
#include <_include/_inttype.h>

#include "WordNormalizerGerman.h"

struct char_to_string_t
{
    uint32_t    ch;
    const char *map;
};

static char_to_string_t ligatures_mappings[] =
{
    { 0x0000C4, "AE" }, // LATIN CAPITAL LETTER A WITH DIAERESIS
    { 0x0000D6, "OE" }, // LATIN CAPITAL LETTER O WITH DIAERESIS
    { 0x0000DC, "UE" }, // LATIN CAPITAL LETTER U WITH DIAERESIS
    { 0x0000DF, "ss" }, // LATIN SMALL LETTER SHARP S
    { 0x0000E4, "ae" }, // LATIN SMALL LETTER A WITH DIAERESIS
    { 0x0000F6, "oe" }, // LATIN SMALL LETTER O WITH DIAERESIS
    { 0x0000FC, "ue" }  // LATIN SMALL LETTER U WITH DIAERESIS
};

GermanSymbolCopier::GermanSymbolCopier() :
    table(),
    mappings()
{
    for ( size_t i = 0; i < sizeof(ligatures_mappings) / sizeof(char_to_string_t); i++ )
    {
        mappings.push_back( ligatures_mappings[i].map );
        uint32_t offset = static_cast<uint32_t>( mappings.size() );
        table.setValue( ligatures_mappings[i].ch, offset );
    }
}

WordNormalizerResult GermanSymbolCopier::copySymbol( uint32_t  ch,
                                                     char     *outBuffer,
                                                     size_t    /* outBufferSize */,
                                                     size_t   &outBufferIdx,
                                                     uint32_t  flags )
{
    if ( flags & MORPH_FLAG_EXPAND_LIGATURES )
    {
        uint32_t offset = table.getValue( ch );
        if ( offset != 0 )
        {
            const char *map = mappings[ offset - 1 ];
            
            for ( size_t i = 0; ; )
            {
                U8_NEXT_UNSAFE( map, i, ch );
                if ( ch == static_cast<uint32_t>( '\0' ) )
                    break;
                U8_APPEND_UNSAFE( outBuffer, outBufferIdx, ch );
            }

            return NORMALIZE_SUCCESS;
        }
    }

    U8_APPEND_UNSAFE( outBuffer, outBufferIdx, ch );
    
    return NORMALIZE_SUCCESS;
}


