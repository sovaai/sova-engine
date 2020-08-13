/**
 * @file   FlexionRW.h
 * @author swizard <me@swizard.info>
 * @date   Sun Jun  1 00:26:21 2008
 * 
 * @brief  The second (last) part of word form.
 * 
 * 
 */
#ifndef _FLEXIONRW_H_
#define _FLEXIONRW_H_

#include <_include/_string.h>

#include "Flexion.h"

class FlexionsArrayRW : public FlexionsArray
{
public:
        FlexionsArrayRW() : FlexionsArray() { }
	~FlexionsArrayRW() { }

public:
	/** 
	 * Sets the given flexion at the given index.
	 * 
	 * @param index An index of flexion to use
	 * @param flexion A flexion to set
	 */
	void setFlexion(size_t index, const char *flexion)
	{
		char *dst = const_cast<char *>( flexions + (index * maxFlexLength) );
		strncpy(dst, flexion, maxFlexLength - 1);
	}
};

class FlexDistribArrayRW : public FlexDistribArray
{
public:
        FlexDistribArrayRW() : FlexDistribArray() { }
	~FlexDistribArrayRW() { }

public:
	/** 
	 * Assigns the address of distributions array and flexions count for index calculating.
	 * 
	 * @param distributions An address with distributions to assign
	 * @param distribsCount A total flexion distributions count
	 * @param flexionsCount A flexions count to assign
	 */
	void assign(const FlexDistrib *distributions, size_t distribsCount, size_t flexionsCount)
	{
		FlexDistribArray::assign(distributions, flexionsCount);

		/* Mark all slots as blank */
		for (size_t i = 0; i < distribsCount; i++)
			for (size_t j = 0; j < flexionsCount; j++)
				setWordFormListIdx( static_cast<FlexDistribId>(i),
						    static_cast<FlexId>(j),
						    static_cast<WordFormListIdx>(-1) );
	}

	/** 
	 * Given the distribution id and a flexion id within it sets the word form list index
	 * that can be used with the help of WordFormsArray helper class.
	 * 
	 * @param distribId A distribution id
	 * @param flexId A flexion id within the distribution given
	 * @param listId A word form list index to set
	 */
	void setWordFormListIdx(FlexDistribId distribId, FlexId flexId, WordFormListIdx listId)
	{
		FlexDistrib *dst = const_cast<FlexDistrib *>(distributions);
		dst[ (distribId * flexionsCount) + flexId ] = listId;
	}
};

#endif /* _FLEXION_H_ */

