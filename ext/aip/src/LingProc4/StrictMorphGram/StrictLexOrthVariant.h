/**
 * @file   StrictLexOrthVariant.h
 * @author swizard <me@swizard.info>
 * @date   Thu Oct 22 03:12:10 2009
 * 
 * @brief  Orthographic variant of a strict lexeme.
 * 
 * 
 */
#ifndef _STRICTLEXORTHVARIANT_H_
#define _STRICTLEXORTHVARIANT_H_

#include <stdlib.h>

#include "sm_dll.h"

struct SM_DLL StrictLexOrthVariant
{
    enum Type
    {
        MAIN_ONLY,
        MAIN_HAS_MORE,
        SUPPLEMENT,
        INVALID
    } type;
    
    enum UsageAttribs
    {
        USAGE_ATTR_NONE = 0,
        USAGE_ATTR_NEW  = 1,
        USAGE_ATTR_OLD  = 2,
            
        USAGE_ATTR_LAST = 3
    } usageAttribs;
    
    size_t lexNo; // Lexeme number of a current orth variant
    size_t orthVariant;
        
    static const char *getName( UsageAttribs attr );
        
    StrictLexOrthVariant() : type(INVALID), usageAttribs(USAGE_ATTR_LAST), lexNo(0), orthVariant(0) { }
};

#endif /* _STRICTLEXORTHVARIANT_H_ */

