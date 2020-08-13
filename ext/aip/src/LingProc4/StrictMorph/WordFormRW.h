/**
 * @file   WordFormRW.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 14 20:19:24 2008
 * 
 * @brief  Word form -- grammatical form specific representation of a single word (read/write variant).
 * 
 * 
 */
#ifndef _WORDFORMRW_H_
#define _WORDFORMRW_H_

#include <_include/_string.h>

#include "WordForm.h"

class WordFormRW : public WordForm
{
public:
        WordFormRW() : WordForm()
	{
		bzero(data, sizeof(data));
	}
	~WordFormRW() { }

public:
	/** 
	 * Sets the paradigm form id.
	 * 
	 * @param id A paradigm form id
	 */
	void setParadigmFormId(ParadigmFormId id)
	{
		*( reinterpret_cast<ParadigmFormId *>(data) ) =
			(isLastInList() ? 0x8000 : 0x0000) | (id & 0xEFFF);
	}
	
	/** 
	 * Sets the paradigm form number.
	 * 
	 * @param id A paradigm form number
	 */
	void setFormNumber(ParadigmFormNumber number)
	{
		*( reinterpret_cast<ParadigmFormNumber *>(data                   +
							  sizeof(ParadigmFormId) +
							  sizeof(WordFormNumber) +
							  sizeof(WordFormAttribs)) ) = number;
	}
	
	/** 
	 * Sets the 'last in list' mark.
	 * 
	 */
	void setLastInListFlag()
	{
		*( reinterpret_cast<ParadigmFormId *>(data) ) |= 0x8000;
	}

	/** 
	 * Clears the 'last in list' mark.
	 * 
	 */
	void clearLastInListFlag()
	{
		*( reinterpret_cast<ParadigmFormId *>(data) ) &= 0xEFFF;
	}

	/** 
	 * Sets the word form number.
	 * 
	 * @param number A word form number
	 */
	void setNumber(WordFormNumber number)
	{
		*( reinterpret_cast<WordFormNumber *>(data                   +
						      sizeof(ParadigmFormId)) ) = number;
	}

	/** 
	 * Sets the word form attributes.
	 * 
	 * @param attribs Word form attributes
	 */
	void setAttribs(WordFormAttribs attribs)
	{
		*( reinterpret_cast<WordFormAttribs *>(data                   +
						       sizeof(ParadigmFormId) +
						       sizeof(WordFormNumber)) ) = attribs;
	}
        
        /** 
         * Sets the morphology internal form number.
         * 
         * @param morphNo Internal form number
         */
        void setInternalMorphNo(uint8_t morphNo)
        {
		*( reinterpret_cast<uint8_t *>(data                   +
                                               sizeof(ParadigmFormId) +
                                               sizeof(WordFormNumber) +
                                               sizeof(WordFormAttribs) +
                                               sizeof(ParadigmFormNumber) ) ) = morphNo;
        }
};

#endif /* _WORDFORMRW_H_ */

