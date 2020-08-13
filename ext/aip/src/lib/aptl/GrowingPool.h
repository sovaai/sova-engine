/**
 * @file   GrowingPool.h
 * @author swizard <me@swizard.info>
 * @date   Wed Feb 13 18:38:15 2008
 * 
 * @brief  A simple template structure for the growing array accumulator.
 * 
 * 
 */
#ifndef _GROWINGPOOL_H_
#define _GROWINGPOOL_H_

#ifdef _MSC_VER
    #pragma warning(disable:4786) // identifier was truncated to '255' characters in the debug information
#endif

#include <stdlib.h>

#include <_include/_inttype.h>

/** 
 * A growing array structure.
 *
 * Template parameters:
 *  T:                           the array type. I.e. GrowingPool<double> operates 'double *array' pool.
 *  initialSize (optional):      the size to allocate during the very first array access.
 *  growingIncrement (optional): the amount to add to allocation size each time it should be increased.
 *  growingPower (optional):     the increase coefficient: binary shifts the current allocation size and
 *                               adds the result to it (alloc + (alloc >> growingPower)). For example,
 *                               growingPower=0 doubles the allocation size at each increasing,
 *                               growingPower=1 appends a half of current allocation size,
 *                               growingPower=2 appends a quater of current allocation size and so on.
 *
 * The formula for size growing: newAlloc = alloc + growingIncrement + (alloc >> growingPower).
 * 
 */
template< typename T, int initialSize = 16, int growingIncrement = 16, int growingPower = (sizeof(size_t) << 3) - 1 >
struct GrowingPool
{
public:
        GrowingPool() : array( 0 ), size( 0 ), alloc( 0 ) { }
	~GrowingPool()
	{
		clear();
	}

public:
	/** 
	 * Grows the array by one.
	 * 
	 */
	T *grow()
	{
		if ( setSize( size + 1 ) )
			return 0;
		return &array[ size - 1 ];
	}
	/** 
	 * Sets the new size for the array.
	 * 
	 * @param newSize A new size to set
	 *
	 * @return zero if successfull and non-zero if no memory left
	 */
	int setSize( size_t newSize )
	{
		/* Check overflow */
		while ( newSize > alloc )
		{
			size_t  newAlloc = (alloc == 0 ?
					    initialSize : alloc + growingIncrement + ( alloc >> growingPower ));
			T      *buffer   = static_cast<T *>( realloc( array, (newAlloc * sizeof(T)) ) );

			/* Check if no memory left */
			if (buffer == 0)
				return 1;

			alloc = newAlloc;
			array = buffer;
		}
		size = newSize;

		return 0;
	}
	/** 
	 * Returns the current size of the buffer.
	 * 
	 * 
	 * @return the array current size (total number of elements)
	 */
	size_t getSize() const
	{
		return size;
	}
	/** 
	 * Rewinds the elements counter to zero but does not free the memory.
	 * 
	 */
	void reset()
	{
		size = 0;
	}
	/** 
	 * Clears array and frees the memory.
	 * 
	 */
	void clear()
	{
		reset();
		if ( alloc > 0 && array != 0 )
			free(array);
		array = 0;
		alloc = 0;
	}
	/** 
	 * Returns the array itself.
	 * 
	 * 
	 * @return the array of elements
	 */
	T *getArray()
	{
		return array;
	}
	/** 
	 * Returns the element at the given index.
	 * 
	 * @param index The index of element
	 * 
	 * @return the element at the given index
	 */
	T &operator[](size_t index)
	{
		return array[index];
	}
	/** 
	 * Returns the array itself (constant version).
	 * 
	 * 
	 * @return the array of elements
	 */
	const T *getArray() const
	{
		return array;
	}
	/** 
	 * Returns the element at the given index (constant version).
	 * 
	 * @param index The index of element
	 * 
	 * @return the element at the given index
	 */
	const T &operator[](size_t index) const
	{
		return array[index];
	}

protected:
	T      *array;
	size_t  size;
	size_t  alloc;
};

#endif /* _GROWINGPOOL_H_ */

