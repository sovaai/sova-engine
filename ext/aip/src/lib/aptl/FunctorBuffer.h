/**
 * @file   FunctorBuffer.h
 * @author swizard <me@swizard.info>
 * @date   Wed Nov 11 18:53:55 2009
 * 
 * @brief  An accumulating version of Functor class.
 * 
 * 
 */
#ifndef _FUNCTORBUFFER_H_
#define _FUNCTORBUFFER_H_

#include "avector.h"
#include "Functor.h"

/* The accumulating Functor version based on avector for common purposes */
template<typename FunctorValue, typename PoolValue>
class FunctorBuffer : public Functor<FunctorValue>, public avector<PoolValue>
{
public:
	FunctorBuffer() : Functor<FunctorValue>(), avector<PoolValue>() { }
	~FunctorBuffer() { }

public:
	virtual int apply(FunctorValue value)
	{
		avector<PoolValue>::push_back(value);
		if (avector<PoolValue>::no_memory())
			return 1;
		else
			return 0;
	}
};

#endif /* _FUNCTORBUFFER_H_ */

