/**
 * @file   LexGramDescShadowRW.h
 * @author swizard <me@swizard.info>
 * @date   Sat May 17 20:57:10 2008
 * 
 * @brief  A lexeme grammatical description (read/write variant).
 * 
 * 
 */
#ifndef _LEXGRAMDESCSHADOWRW_H_
#define _LEXGRAMDESCSHADOWRW_H_

#include <_include/_string.h>

#include "LexGramDescShadow.h"

class LexGramDescShadowRW : public LexGramDescShadow
{
public:
        LexGramDescShadowRW() : LexGramDescShadow()
	{
		bzero(data, sizeof(data));
	}
	~LexGramDescShadowRW() { }

public:
	/** 
	 * Sets the part of speech sign.
	 * 
	 * @param value A part of speech
	 */
	void setPartOfSpeech(LexGram::PartOfSpeech value)
        {
                data[0] = static_cast<uint8_t>(value);
        }
        /** 
         * Sets the reflexion sign.
         * 
         * @param value A reflexion
         */
        void setRefl(LexGram::Refl value)
        {
                data[1] = static_cast<uint8_t>(value);
        }
        /** 
         * Sets the transitivity sign.
         * 
         * @param value A transitivity
         */
        void setTransitivity(LexGram::Transitivity value)
        {
                data[2] = static_cast<uint8_t>(value);
        }
        /** 
         * Sets the aspect sign.
         * 
         * @param value A aspect
         */
        void setAspect(LexGram::Aspect value)
        {
                data[3] = static_cast<uint8_t>(value);
        }
        /** 
         * Sets the animate sign.
         * 
         * @param value An animate
         */
        void setAnimate(LexGram::Animate value)
        {
                data[4] = static_cast<uint8_t>(value);
        }
        /** 
         * Sets the number sign.
         * 
         * @param value A number
         */
        void setNumber(LexGram::Number value)
        {
                data[5] = static_cast<uint8_t>(value);
        }
        /** 
         * Sets the gender sign.
         * 
         * @param value A gender
         */
        void setGender(LexGram::Gender value)
        {
                data[6] = static_cast<uint8_t>(value);
        }
        /** 
         * Sets the suppl sign.
         * 
         * @param value A suppl
         */
        void setSuppl(LexGram::Suppl value)
        {
                data[7] = static_cast<uint8_t>(value);
        }
        /** 
         * Sets the subtype sign.
         * 
         * @param value A subtype
         */
        void setSubtype(LexGram::Subtype value)
        {
                data[8] = static_cast<uint8_t>(value);
        }
	/** 
	 * Sets the full gram description string.
	 * 
	 * @param name A full gram description string to set
	 */
	void setName(const char *name)
	{
		strncpy( reinterpret_cast<char *>(data + descOffset),
                         name,
                         sizeof(data) - descOffset - 1 );
	}
};

#endif /* _LEXGRAMDESCSHADOWRW_H_ */

