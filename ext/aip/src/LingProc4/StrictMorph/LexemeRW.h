/**
 * @file   LexemeRW.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 21 14:22:48 2008
 * 
 * @brief  Lexical element: a set of all word forms of a single word (read/write variant).
 * 
 * 
 */
#ifndef _LEXEMERW_H_
#define _LEXEMERW_H_

#include <_include/_string.h>

#include "Lexeme.h"

template<typename ReferenceType>
class CommonReferenceRW : public CommonReference<ReferenceType>
{
	typedef CommonReference<ReferenceType> Base;
	
public:
        CommonReferenceRW(bool isDummy = false) : Base()
	{
		bzero(Base::data, sizeof(Base::data));
		if (isDummy)
		{
			clearListReference();
			setReference( CommonReference<ReferenceType>::dummy() );
		}
	}
	~CommonReferenceRW() { }

public:
	/** 
	 * Sets the list reference mark (a reference becomes a list reference).
	 * 
	 */
	void setListReference()
	{
		( *(reinterpret_cast<ReferenceType *>(Base::data)) ) |=
			(1 << ((sizeof(ReferenceType) << 3) - 1));
	}

	/** 
	 * Clears the list reference mark (a reference becomes a single target reference).
	 * 
	 */
	void clearListReference()
	{
		( *(reinterpret_cast<ReferenceType *>(Base::data)) ) &=
			((1 << ((sizeof(ReferenceType) << 3) - 1)) - 1);
	}

	/** 
	 * Sets the reference itself.
	 * 
	 * @param ref A reference to set
	 */
	void setReference(ReferenceType ref)
	{
		int isList = Base::isListReference();
		*(reinterpret_cast<ReferenceType *>(Base::data)) = ref;
		if (isList)
			setListReference();
		else
			clearListReference();
	}
};

class LexemeRW : public Lexeme
{
public:
        LexemeRW() : Lexeme()
	{
		bzero(data, sizeof(data));
	}
	~LexemeRW() { }

public:
	/** 
	 * Returns either grammatical description id or a list id of them (read/write variant).
	 * 
	 * 
	 * @return CommonReferenceRW to a grammatical description id
	 */
	CommonReferenceRW<LexGramDescrId> *getGramDescrIdRW()
	{
		return reinterpret_cast<CommonReferenceRW<LexGramDescrId> *>
			( const_cast<CommonReference<LexGramDescrId> *>(getGramDescrId()) );
	}

	/** 
	 * Returns either stem id or a list id of them (read/write variant)..
	 * 
	 * 
	 * @return CommonReferenceRW to a stem id
	 */
	CommonReferenceRW<StemId> *getStemIdRW()
	{
		return reinterpret_cast<CommonReferenceRW<StemId> *>
			( const_cast<CommonReference<StemId> *>(getStemIdReference()) );
	}

	/** 
	 * Sets the new common reference to the lex grammar description id (or a list of them).
	 * 
	 * @param id An id to set
	 */
	void setGramDescrId(CommonReferenceRW<LexGramDescrId> id)
	{
		*( reinterpret_cast<CommonReference<LexGramDescrId> *>(data +
								       sizeof(CommonReferenceRW<StemId>)) ) = id;
	}

	/** 
	 * Sets the new common to the stem id (or a list of them).
	 * 
	 * @param id An id to set
	 */
	void setStemId(CommonReferenceRW<StemId> id)
	{
		*( reinterpret_cast<CommonReference<StemId> *>(data) ) = id;
	}

	/** 
	 * Sets the lexeme usage attributes.
	 * 
	 * @param caps Attributes to set
	 */
	void setUsageAttributes(StrictLexOrthVariant::UsageAttribs attr)
	{
		*( reinterpret_cast<uint8_t *>( data                                    +
						sizeof(CommonReference<LexGramDescrId>) +
						sizeof(CommonReference<StemId>) ) ) = static_cast<uint8_t>(attr);
	}

	/** 
	 * Sets the current lexeme as main with no orthographic variants.
	 * 
	 */
	void setOrthVariantMainOnly()
	{
		setOrthVariant(0x00);
	}

	/** 
	 * Sets the current lexeme as main with some more orthographic variants.
	 * 
	 */
	void setOrthVariantMainInList()
	{
		setOrthVariant(0x01);
	}

	/** 
	 * Sets the current lexeme as supplement orthographic variant.
	 * 
	 */
	void setOrthVariantSupplement()
	{
		setOrthVariant(0x03);
	}

private:
	void setOrthVariant(uint8_t var)
	{
		uint8_t value = *( data +
				   sizeof(CommonReference<LexGramDescrId>) +
				   sizeof(CommonReference<StemId>)         +
				   sizeof(uint8_t) );
		value &= 0xFC;
		var   &= 0x03;
		value |= var;
		*( data +
		   sizeof(CommonReference<LexGramDescrId>) +
		   sizeof(CommonReference<StemId>)         +
		   sizeof(uint8_t) ) = value;
	}
};

#endif /* _LEXEMERW_H_ */

