/**
 * @file   LPMorphTildeRev.h
 * @author swizard <me@swizard.info>
 * @date   Mon Aug 31 20:21:07 2009
 * 
 * @brief  Reverse word behaviour for LPMorphTilde.
 * 
 * 
 */
#ifndef _LPMORPHTILDEREV_H_
#define _LPMORPHTILDEREV_H_

#include <_include/_string.h>

#include "LPMorphTilde.h"

class TildeCustomReversed
{
protected:
    /* Tilde morphology constants */
    static const MorphTypes MORPH_TYPE = MORPH_TYPE_TILDE_REV;
        
    static const uint32_t TILDEMORPH_MAX_WORD_LEN                = 4096;
    static const uint32_t TILDEMORPH_MIN_STEM_LEN                = 2;
    static const uint32_t TILDEMORPH_VERSION                     = 1;
    static const uint32_t TILDEMORPH_VARIANT                     = 1;
    static const int      TILDEMORPH_SPEC_SUFFIX                 = 255;

    static const size_t   TILDEMORPH_FS_HEADER_SIZE              = (sizeof(uint32_t) * 11);
    static const uint32_t TILDEMORPH_FS_MAGIC                    = 0xF3E68B55U;
    static const uint32_t TILDEMORPH_FS_WORK_FORMAT_VERSION      = 2;
    static const uint32_t TILDEMORPH_FS_WORK_FORMAT_VARIANT      = 1;
    static const uint32_t TILDEMORPH_FS_TRANSPORT_FORMAT_VERSION = 1;
    static const uint32_t TILDEMORPH_FS_TRANSPORT_FORMAT_VARIANT = 1;

    static const int      TILDEMORPH_FS_SEC_STEMS                = 1;
        
protected:
    const char *preprocessWord( const char *word, size_t wordLength )
    {
        if ( wordLength == static_cast<size_t>(-1) )
            for ( wordLength = 0; word[wordLength] != '\0'; wordLength++ );
                
        if ( wordLength == 0 )
            return word;

        size_t tmpOffset = 0;

        for ( size_t offset = wordLength; offset > 0; )
        {
            uint32_t ch;
            U8_PREV_UNSAFE( word, offset, ch );
            U8_APPEND_UNSAFE( buffer, tmpOffset, ch );
        }
        buffer[ tmpOffset ] = '\0';
        
        return buffer;
    }
        
    void postprocessWord( char *buffer, size_t totalLen )
    {
        char   tmpBuffer[ TILDEMORPH_MAX_WORD_LEN ];
        size_t tmpOffset = 0;

        for ( size_t offset = totalLen - 1; offset > 0; )
        {
            uint32_t ch;
            U8_PREV_UNSAFE( buffer, offset, ch );
            U8_APPEND_UNSAFE( tmpBuffer, tmpOffset, ch );
        }
        memcpy( buffer, tmpBuffer, tmpOffset );
    }

private:
    char buffer[ TILDEMORPH_MAX_WORD_LEN ];
};

typedef LPMorphTildeImpl< TildeCustomReversed > LPMorphTildeRev;

#endif /* _LPMORPHTILDEREV_H_ */

