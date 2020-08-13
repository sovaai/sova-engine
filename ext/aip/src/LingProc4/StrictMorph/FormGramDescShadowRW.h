/**
 * @file   FormGramDescShadowRW.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 14 18:04:04 2008
 * 
 * @brief  A single form grammatical description (read/write variant).
 * 
 * 
 */
#ifndef _FORMGRAMDESCSHADOWRW_H_
#define _FORMGRAMDESCSHADOWRW_H_

#include <_include/_string.h>

#include "FormGramDescShadow.h"

class FormGramDescShadowRW : public FormGramDescShadow
{
public:
        FormGramDescShadowRW() : FormGramDescShadow()
	{
		bzero(data, sizeof(data));
	}
	~FormGramDescShadowRW() { }

public:
	/**
	 * Sets the Animate sign.
	 *
	 * @param value A Animate sign to set
	 */
	void setAnimate(FormGram::Animate value)
	{
		data[0] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Case sign.
	 *
	 * @param value A Case sign to set
	 */
	void setCase(FormGram::Case value)
	{
		data[1] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Degree sign.
	 *
	 * @param value A Degree sign to set
	 */
	void setDegree(FormGram::Degree value)
	{
		data[2] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Gender sign.
	 *
	 * @param value A Gender sign to set
	 */
	void setGender(FormGram::Gender value)
	{
		data[3] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Mood sign.
	 *
	 * @param value A Mood sign to set
	 */
	void setMood(FormGram::Mood value)
	{
		data[4] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Number sign.
	 *
	 * @param value A Number sign to set
	 */
	void setNumber(FormGram::Number value)
	{
		data[5] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Person sign.
	 *
	 * @param value A Person sign to set
	 */
	void setPerson(FormGram::Person value)
	{
		data[6] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Refl sign.
	 *
	 * @param value A Refl sign to set
	 */
	void setRefl(FormGram::Refl value)
	{
		data[7] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Short sign.
	 *
	 * @param value A Short sign to set
	 */
	void setShort(FormGram::Short value)
	{
		data[8] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Tense sign.
	 *
	 * @param value A Tense sign to set
	 */
	void setTense(FormGram::Tense value)
	{
		data[9] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Verbform sign.
	 *
	 * @param value A Verbform sign to set
	 */
	void setVerbform(FormGram::Verbform value)
	{
		data[10] = static_cast<uint8_t>(value);
	}

	/**
	 * Sets the Mstform sign.
	 *
	 * @param value A Mstform sign to set
	 */
	void setMstform(FormGram::Mstform value)
	{
		data[11] = static_cast<uint8_t>(value);
	}
};

#endif /* _FORMGRAMDESCSHADOWRW_H_ */

