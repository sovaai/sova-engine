/**
 * @file   LexGramDescShadow.h
 * @author swizard <me@swizard.info>
 * @date   Sat May 17 20:39:28 2008
 * 
 * @brief  A lexeme grammatical description.
 * 
 * 
 */
#ifndef _LEXGRAMDESCSHADOW_H_
#define _LEXGRAMDESCSHADOW_H_

#include <sys/types.h>
#include <_include/_inttype.h>

#include <LingProc4/StrictMorphGram/LexGram.h>

typedef uint16_t LexGramDescrId;

class LexGramDescShadow
{
public:
	enum Constants
        {
                totalDataSize = 64,
                descOffset    = 24
        };
	
public:
	/** 
	 * Returns the part of speech sign.
	 * 
	 * 
	 * @return A part of speech
	 */
	LexGram::PartOfSpeech getPartOfSpeech() const
	{
		return static_cast<LexGram::PartOfSpeech>(data[0]);
	}
        /** 
         * Returns the reflextion sign.
         * 
         * 
         * @return A reflexion
         */
        LexGram::Refl getRefl() const
        {
                return static_cast<LexGram::Refl>(data[1]);
        }
        /** 
         * Returns the transitivity sign.
         * 
         * 
         * @return A transitivity
         */
        LexGram::Transitivity getTransitivity() const
        {
                return static_cast<LexGram::Transitivity>(data[2]);
        }
        /** 
         * Returns the aspect sign.
         * 
         * 
         * @return An aspect
         */
        LexGram::Aspect getAspect() const
        {
                return static_cast<LexGram::Aspect>(data[3]);
        }
        /** 
         * Returns the animate sign.
         * 
         * 
         * @return An animate
         */
        LexGram::Animate getAnimate() const
        {
                return static_cast<LexGram::Animate>(data[4]);
        }
        /** 
         * Returns the number sign.
         * 
         * 
         * @return A number
         */
        LexGram::Number getNumber() const
        {
                return static_cast<LexGram::Number>(data[5]);
        }
        /** 
         * Returns the gender sign.
         * 
         * 
         * @return A gender
         */
        LexGram::Gender getGender() const
        {
                return static_cast<LexGram::Gender>(data[6]);
        }
        /** 
         * Returns the suppl sign.
         * 
         * 
         * @return A suppl
         */
        LexGram::Suppl getSuppl() const
        {
                return static_cast<LexGram::Suppl>(data[7]);
        }
        /** 
         * Returns the subtype sign.
         * 
         * 
         * @return A subtype
         */
        LexGram::Subtype getSubtype() const
        {
                return static_cast<LexGram::Subtype>(data[8]);
        }
        /** 
         * Returns the full gram description string.
         * 
         * 
         * @return A description string
         */
        const char *getName() const
        {
                return reinterpret_cast<const char *>(data + descOffset);
        }
	
protected:
	LexGramDescShadow() { }
	~LexGramDescShadow() { }

protected:
	/* Logical data structure:
	 *
	 * Part of speech sign: 1 octet (PartOfSpeech)
         * Refl sign:           1 octet (Refl)
         * Transitivity sign:   1 octet (Transitivity)
         * Aspect sign:         1 octet (Aspect)
         * Animate sign:        1 octet (Animate)
         * Number sign:         1 octet (Number)
         * Gender sign:         1 octet (Gender)
         * Suppl sign:          1 octet (Suppl)
         * Subtype sign:        1 octet (Subtype)
         * <empty>:             ..till offset 'descOffset'
	 * Gram name:           (rest of 64) octets (ASCIIZ)
	 */
	uint8_t data[ totalDataSize ];
};

#endif /* _LEXGRAMDESCSHADOW_H_ */

