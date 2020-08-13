/**
 * @file   StrictWordForm.h
 * @author swizard <me@swizard.info>
 * @date   Thu Oct 22 02:09:29 2009
 * 
 * @brief  Word form information.
 * 
 * 
 */
#ifndef _STRICTWORDFORM_H_
#define _STRICTWORDFORM_H_

#include "Shadowed.h"
#include "FormGram.h"

class StrictLex;
class StrictMorph;
class APStrictMorph;
class SM_DLL StrictWordForm : public Shadowed
{
public:
    enum UsageAttribsValues
    {
        USAGE_ATTR_NONE  = 0x00,
        USAGE_ATTR_RARE  = 0x01,
        USAGE_ATTR_FORM2 = 0x02,
        USAGE_ATTR_CASE2 = 0x04,
        USAGE_ATTR_JO    = 0x08,
        USAGE_ATTR_EXTRA = 0x10
    };
    
    typedef unsigned char UsageAttribs;
    
public:
    template<typename MorphType> const FormGram  getFormGram( const MorphType &morphLink )     const;
    template<typename MorphType> const char     *getParadigmName( const MorphType &morphLink ) const;

    size_t       getFormNo()        const;
    size_t       getFormVariantNo() const;
    UsageAttribs getUsageAttribs()  const;

    static const char *getName( UsageAttribs attr );
    
public:
    StrictWordForm() : Shadowed() { }
    ~StrictWordForm() { }
};

#endif /* _STRICTWORDFORM_H_ */

