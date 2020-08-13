/**
 * @file   Paradigm.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 14 16:24:49 2008
 * 
 * @brief  Paradigm -- ordered list of grammatical word forms.
 * 
 * 
 */
#ifndef _PARADIGM_H_
#define _PARADIGM_H_

#include <sys/types.h>

#include <_include/_inttype.h>

#include "FormGramDescShadow.h"

typedef uint8_t  ParadigmType;
typedef uint16_t ParadigmFormNumber;

const ParadigmFormNumber PARADIGM_FORM_EMPTY = static_cast<ParadigmFormNumber>(-1);

/* Paradigm form identificator: ParadigmType + ParadigmFormNumber compound */
typedef uint16_t ParadigmFormId;

/* The paradigm itself */
class Paradigm
{
protected:
	enum Constants
        {
                totalDataSize = 32,
                fixedDataSize = sizeof(ParadigmFormNumber) + sizeof(ParadigmFormId) + sizeof(ParadigmType)
        };
	
public:
	/** 
	 * Returns the type of the paradigm.
	 * It should be equal to the element index within the paradigms array.
	 * 
	 * 
	 * @return Paradigm type
	 */
	ParadigmType getType() const
	{
		return *( reinterpret_cast<const ParadigmType *>(data                       +
								 sizeof(ParadigmFormNumber) +
								 sizeof(ParadigmFormId)) );
	}

	/** 
	 * Returns the forms count within the current paradigm.
	 * 
	 * 
	 * @return Forms count
	 */
	ParadigmFormNumber getFormsCount() const
	{
		return *( reinterpret_cast<const ParadigmFormNumber *>(data) );
	}

	/** 
	 * Returns the reference (array index) for the grammatical word description.
	 * 
	 * 
	 * @return An index in ParadigmForm's array
	 */	
	ParadigmFormId getFormsIndex() const
	{
		return *( reinterpret_cast<const ParadigmFormId *>(data + sizeof(ParadigmFormNumber)) );
	}

	/** 
	 * Returns the paradigm name.
	 * 
	 * 
	 * @return Paradigm name
	 */
	const char *getTypeName() const
	{
		return reinterpret_cast<const char *>( data + fixedDataSize );
	}
	
protected:
	Paradigm() { }
	~Paradigm() { }
	
protected:
	/* Logical data structure:
	 *
	 * Forms count:  2 octets (size_t)
	 * Forms index:  2 octets (ParadigmFormId)
	 * Type:         1 octet  (ParadigmType)
	 * Type name:   27 octets (ASCIIZ)
	 */
	uint8_t data[ totalDataSize ];
};

/* Grammatical forms of a word for a single paradigm type */
class ParadigmForm
{
protected:
	enum Constants
        {
                totalDataSize = 64,
                fixedDataSize =
		sizeof(ParadigmFormNumber) +
		sizeof(ParadigmFormId)     +
		sizeof(ParadigmType)       +
		sizeof(FormGramDescShadow)
        };

public:
	/** 
	 * Returns the form id.
	 * It should be equal to the element index within forms array.
	 * 
	 * 
	 * @return Form id
	 */
	ParadigmFormId getId() const
	{
		return *( reinterpret_cast<const ParadigmFormId *>(data) );
	}

	/** 
	 * Returns the type of the paradigm which current form belongs to.
	 * 
	 * 
	 * @return Paradigm type
	 */
	ParadigmType getParadigmType() const
	{
		return *( reinterpret_cast<const ParadigmType *>(data                       +
								 sizeof(ParadigmFormId)     +
								 sizeof(ParadigmFormNumber) +
								 sizeof(FormGramDescShadow)) );
	}

	/** 
	 * Returns the form number within the paradigm.
	 * 
	 * 
	 * @return Form number
	 */
	ParadigmFormNumber getFormNumber() const
	{
		return *( reinterpret_cast<const ParadigmFormNumber *>(data +
								       sizeof(ParadigmFormId)) );
	}

	/** 
	 * Returns the form name.
	 * 
	 * 
	 * @return Form name
	 */
	const char *getFormName() const
	{
		return reinterpret_cast<const char *>(data + fixedDataSize);
	}

        /** 
         * Returns the form description.
         * 
         * 
         * @return Form description
         */
        const FormGramDescShadow *getDesc() const
        {
                return reinterpret_cast<const FormGramDescShadow *>( data                       +
                                                                     sizeof(ParadigmFormId)     +
                                                                     sizeof(ParadigmFormNumber) );
        }
	
protected:
        ParadigmForm() { }
	~ParadigmForm() { }

protected:
	/* Logical data structure:
	 *
	 * Form id:                      2 octets (ParadigmFormId)
	 * Form number:                  2 octets (ParadigmFormNumber)
	 * Form grammatical description: sizeof(FormGramDescShadow) octets
	 * Type:                         1 octet  (ParadigmType)
	 * Form name:                    (rest of 64) octets (ASCIIZ)
	 */
	uint8_t             data[ totalDataSize ];
};

#endif /* _PARADIGM_H_ */

