/**
 * @file   Lexeme.h
 * @author swizard <me@swizard.info>
 * @date   Sat May 17 20:07:14 2008
 * 
 * @brief  Lexical element: a set of all word forms of a single word.
 * 
 * 
 */
#ifndef _LEXEME_H_
#define _LEXEME_H_

#include <sys/types.h>
#include <_include/_inttype.h>

#include <LingProc4/StrictMorphGram/StrictLexOrthVariant.h>

#include "StemCommon.h"
#include "LexGramDescShadow.h"

typedef uint32_t LexemeRef;
typedef uint32_t LexemeId;
typedef uint8_t  LexemeCaps;

const LexemeId dummyLexeme = (static_cast<LexemeId>(-1)) >> 1;

class Lexeme;
/** 
 * CommonReference: a single target or a list of targets reference.
 * If the most significant bit is non-zero, the reference considered as list reference.
 * Otherwise the reference considered as single target reference.
 * 
 */
template<typename ReferenceType>
class CommonReference
{
public:
	static ReferenceType dummy()
        {
                return static_cast<ReferenceType>(-1) & ((1U << ((sizeof(ReferenceType) << 3) - 1U)) - 1U);
        }
	
public:
	/** 
	 * Checks if the reference is a list reference.
	 * 
	 * 
	 * @return Non-zero if a list reference, zero if a single target reference
	 */
	int isListReference() const
	{
		return ( *(reinterpret_cast<const ReferenceType *>(data)) ) >>
			((sizeof(ReferenceType) << 3) - 1U);
	}

	/** 
	 * Returns a reference itself.
	 * 
	 * 
	 * @return A reference
	 */
	ReferenceType getReference() const
	{
		return ( *(reinterpret_cast<const ReferenceType *>(data)) ) &
			((1U << ((sizeof(ReferenceType) << 3) - 1U)) - 1U);
	}
	
protected:
	CommonReference() { }
	~CommonReference() { }

protected:
	uint8_t data[ sizeof(ReferenceType) ];
};

class Lexeme
{
protected:
	enum Constants
        {
                totalDataSize = 8
        };
public:
	/** 
	 * Returns either grammatical description id or a list id of them.
	 * 
	 * 
	 * @return CommonReference to a grammatical description id
	 */
	const CommonReference<LexGramDescrId> *getGramDescrId() const
	{
		return reinterpret_cast<const CommonReference<LexGramDescrId> *>(data +
										 sizeof(CommonReference<StemId>));
	}

	/** 
	 * Returns either stem id or a list id of them.
	 * 
	 * 
	 * @return CommonReference to a stem id
	 */
	const CommonReference<StemId> *getStemIdReference() const
	{
		return reinterpret_cast<const CommonReference<StemId> *>(data);
	}

	/** 
	 * Returns the usage attributes of a lexeme.
	 * 
	 * 
	 * @return A lexeme usage attributes
	 */
        StrictLexOrthVariant::UsageAttribs getUsageAttributes() const
	{
		return static_cast<StrictLexOrthVariant::UsageAttribs>
			( *( reinterpret_cast<const uint8_t *>( data                                    +
								sizeof(CommonReference<LexGramDescrId>) +
								sizeof(CommonReference<StemId>) ) ) );
	}

	/** 
	 * Checks if the current lexeme is a dummy lexeme.
	 * 
	 * 
	 * @return true if a dummy, false otherwise
	 */
	bool isDummy() const
	{
		return ( getGramDescrId()->getReference()     == CommonReference<LexGramDescrId>::dummy() &&
			 getStemIdReference()->getReference() == CommonReference<StemId>::dummy() );
	}

	/** 
	 * Checks if the current lexeme has orthographic variants.
	 * 
	 * 
	 * @return true if has, false otherwise
	 */
	bool hasOrthVariants() const
	{
		return (getOrthVariantValue() == 0x01);
	}

	/** 
	 * Checks if the current lexeme is the main orthographic variant with no other variants.
	 * 
	 * 
	 * @return true if is, false otherwise
	 */
	bool isMainOrthVariant() const
	{
		return (getOrthVariantValue() == 0x00);
	}

protected:
	Lexeme() { }
	~Lexeme() { }

private:
	uint8_t getOrthVariantValue() const
	{
		return *( data +
			  sizeof(CommonReference<LexGramDescrId>) +
			  sizeof(CommonReference<StemId>)         +
			  sizeof(uint8_t) ) & 0x03;
	}

protected:
	/* Logical data structure:
	 *
	 * Stem id / list id:                4 octets
	 * Grammar description id / list id: 2 octets
	 * Usage attribs:                    1 octet
	 * Orth variant:                     2 bits:
	 *                                     00 -- main variant, has no variants more
	 *                                     01 -- main variant, has more
	 *                                     11 -- supplement variant
	 * Reserved:                         6 bits
	 */
	uint8_t data[ totalDataSize ];
};

#endif /* _LEXEME_H_ */

