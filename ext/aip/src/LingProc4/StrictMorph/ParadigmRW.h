/**
 * @file   ParadigmRW.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 14 18:11:55 2008
 * 
 * @brief  Paradigm -- ordered list of grammatical word forms (read/write variant).
 * 
 * 
 */
#ifndef _PARADIGMRW_H_
#define _PARADIGMRW_H_

#include <_include/_string.h>

#include "Paradigm.h"
#include "FormGramDescShadowRW.h"

class ParadigmRW : public Paradigm
{
public:
        ParadigmRW() : Paradigm()
	{
		bzero(data, sizeof(data));
	}
	~ParadigmRW() { }

public:
	/** 
	 * Sets the type of the paradigm.
	 * 
	 * @param type A type to set
	 */
	void setType(ParadigmType type)
	{
		*( reinterpret_cast<ParadigmType *>(data                       +
						    sizeof(ParadigmFormNumber) +
						    sizeof(ParadigmFormId)) ) = type;
	}

	/** 
	 * Sets the forms count within the current paradigm.
	 * 
	 * @param count A forms count to set
	 */
	void setFormsCount(ParadigmFormNumber count)
	{
		*( reinterpret_cast<ParadigmFormNumber *>(data) ) = count;
	}

	/** 
	 * Sets the reference (an array index) for the grammatical word description.
	 * 
	 * @param index A reference to set
	 */
	void setFormsIndex(ParadigmFormId index)
	{
		*( reinterpret_cast<ParadigmFormId *>(data                       +
						      sizeof(ParadigmFormNumber)) ) = index;
	}

	/** 
	 * Sets the paradigm name.
	 * 
	 * @param name A name to set
	 */
	void setTypeName(const char *name)
	{
		strncpy(reinterpret_cast<char *>(data + fixedDataSize),
			name,
			sizeof(data) - fixedDataSize - 1);
	}
};

class ParadigmFormRW : public ParadigmForm
{
public:
        ParadigmFormRW() : ParadigmForm()
	{
		bzero(data,  sizeof(data));
	}
	~ParadigmFormRW() { }

public:
	/** 
	 * Sets the form id.
	 * 
	 * @param id An id to set
	 */
	void setId(ParadigmFormId id)
	{
		*( reinterpret_cast<ParadigmFormId *>(data) ) = id;
	}

	/** 
	 * Sets the type of the paradigm which form belongs to.
	 * 
	 * @param type A paradigm type
	 */
	void setParadigmType(ParadigmType type)
	{
		*( reinterpret_cast<ParadigmType *>(data                       +
						    sizeof(ParadigmFormId)     +
						    sizeof(ParadigmFormNumber) +
						    sizeof(FormGramDescShadow)) ) = type;
	}
	
	/** 
	 * Sets form number within the paradigm.
	 * 
	 * @param number A form number
	 */
	void setFormNumber(ParadigmFormNumber number)
	{
		*( reinterpret_cast<ParadigmFormNumber *>(data                   +
							  sizeof(ParadigmFormId)) ) = number;
	}

	/** 
	 * Sets the form name.
	 * 
	 * @param name A name to set
	 */
	void setFormName(const char *name)
	{
		strncpy(reinterpret_cast<char *>(data + fixedDataSize),
			name,
			sizeof(data) - fixedDataSize - 1);
	}

	/** 
	 * Returns the FormGramDescShadow pointer for r/w operations.
	 * 
	 * 
	 * @return Form grammatical description
	 */
	FormGramDescShadowRW *getFormGramDesc()
	{
		return reinterpret_cast<FormGramDescShadowRW *>(data                       +
								sizeof(ParadigmFormId)     +
								sizeof(ParadigmFormNumber));
	}
};

#endif /* _PARADIGMRW_H_ */

