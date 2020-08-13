/**
 * @file   FormGramDescShadow.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 14 17:23:16 2008
 * 
 * @brief  A single form grammatical description.
 * 
 * 
 */
#ifndef _FORMGRAMDESCSHADOW_H_
#define _FORMGRAMDESCSHADOW_H_

#include <sys/types.h>
#include <_include/_inttype.h>

#include <LingProc4/StrictMorphGram/FormGram.h>

class FormGramDescShadow
{
protected:
	enum Constants
        {
                totalDataSize = 12
        };
	
public:
	/** 
	 * Returns the Animate sign.
	 * 
	 * 
	 * @return Animate sign
	 */
	FormGram::Animate getAnimate() const
	{
		return static_cast<FormGram::Animate>(data[0]);
	}

	/**
	 * Returns the Case sign.
	 *
	 *
	 * @return Case sign
	 */
	FormGram::Case getCase() const
	{
		return static_cast<FormGram::Case>(data[1]);
	}

	/**
	 * Returns the Degree sign.
	 *
	 *
	 * @return Degree sign
	 */
	FormGram::Degree getDegree() const
	{
		return static_cast<FormGram::Degree>(data[2]);
	}

	/**
	 * Returns the Gender sign.
	 *
	 *
	 * @return Gender sign
	 */
	FormGram::Gender getGender() const
	{
		return static_cast<FormGram::Gender>(data[3]);
	}

	/**
	 * Returns the Mood sign.
	 *
	 *
	 * @return Mood sign
	 */
	FormGram::Mood getMood() const
	{
		return static_cast<FormGram::Mood>(data[4]);
	}

	/**
	 * Returns the Number sign.
	 *
	 *
	 * @return Number sign
	 */
	FormGram::Number getNumber() const
	{
		return static_cast<FormGram::Number>(data[5]);
	}

	/**
	 * Returns the Person sign.
	 *
	 *
	 * @return Person sign
	 */
	FormGram::Person getPerson() const
	{
		return static_cast<FormGram::Person>(data[6]);
	}

	/**
	 * Returns the Refl sign.
	 *
	 *
	 * @return Refl sign
	 */
	FormGram::Refl getRefl() const
	{
		return static_cast<FormGram::Refl>(data[7]);
	}

	/**
	 * Returns the Short sign.
	 *
	 *
	 * @return Short sign
	 */
	FormGram::Short getShort() const
	{
		return static_cast<FormGram::Short>(data[8]);
	}

	/**
	 * Returns the Tense sign.
	 *
	 *
	 * @return Tense sign
	 */
	FormGram::Tense getTense() const
	{
		return static_cast<FormGram::Tense>(data[9]);
	}

	/**
	 * Returns the Verbform sign.
	 *
	 *
	 * @return Verbform sign
	 */
	FormGram::Verbform getVerbform() const
	{
		return static_cast<FormGram::Verbform>(data[10]);
	}

	/**
	 * Returns the Mstform sign.
	 *
	 *
	 * @return Mstform sign
	 */
	FormGram::Mstform getMstform() const
	{
		return static_cast<FormGram::Mstform>(data[11]);
	}
        
protected:
        FormGramDescShadow() { }
	~FormGramDescShadow() { }

protected:
	/* Logical data structure:
	 *
	 * Animate:  1 octet (Animate)
	 * Case:     1 octet (Case)
	 * Degree:   1 octet (Degree)
	 * Gender:   1 octet (Gender)
	 * Mood:     1 octet (Mood)
	 * Number:   1 octet (Number)
	 * Person:   1 octet (Person)
	 * Refl:     1 octet (Refl)
	 * Short:    1 octet (Short)
	 * Tense:    1 octet (Tense)
	 * Verbform: 1 octet (Verbform)
	 * Mstform:  1 octet (Mstform)
	 */
	uint8_t data[ totalDataSize ];
};

#endif /* _FORMGRAMDESCSHADOW_H_ */

