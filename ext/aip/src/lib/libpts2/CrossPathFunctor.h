/**
 * @file   CrossPathFunctor.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr 28 09:01:56 2008
 * 
 * @brief  A functor for tracking compound word parts.
 * 
 * 
 */
#ifndef _CROSSPATHFUNCTOR_H_
#define _CROSSPATHFUNCTOR_H_

struct CrossPathFunctor
{
	CrossPathFunctor() { }
	virtual ~CrossPathFunctor() { }

	virtual int nextPart(const uint8_t *part, const uint8_t *data, size_t dataSize, bool isExactMatch) = 0;
};

#endif /* _CROSSPATHFUNCTOR_H_ */

