/**
 * @file   Flexion.h
 * @author swizard <me@swizard.info>
 * @date   Thu May 15 18:51:57 2008
 * 
 * @brief  The second (last) part of word form.
 * 
 * 
 */
#ifndef _FLEXION_H_
#define _FLEXION_H_

#include <sys/types.h>

#include <_include/cc_compat.h>
#include <_include/_inttype.h>

#include <lib/aptl/OpenAddressingDict.h>
#include <unicode/utf8.h>

#include "WordForm.h"

typedef uint16_t            FlexId;
typedef uint16_t            FlexDistribId;
typedef WordFormListIdx     FlexDistrib;

/** 
 * This is a helper class.
 * Given a plain memory chunk of 'const char *' treats it as an array of flexions with
 * 32 bytes for each flexion.
 * 
 */
class FlexionsArray
{
public:
	enum Constants
        {
                maxFlexLength = 32
        };
	 
public:
        FlexionsArray() :
                flexions( 0 ),
                flexionsCount( 0 ),
                maxCompoundLength( 0 ),
                standardTable(),
                compositeTable() { }
	~FlexionsArray() { }

public:
	/** 
	 * Assigns the address with flexions array.
	 * 
	 * @param flexions An address with flexions to assign
	 * @param flexionsCount A flexions count to assign
	 *
	 * @return zero if successfull, non-zero if not enough memory
	 */
	int assign(const char *flexions, size_t flexionsCount)
	{
		this->flexions      = flexions;
		this->flexionsCount = flexionsCount;

		/* Create the prefix tree */
		for (register size_t i = 0; i < flexionsCount; i++)
		{
                        /* Determine the flexion type */
                        const char      *value      = getFlexion(i);
                        bool             isCompound = false;
                        size_t           length     = 0;
                        for ( ;; )
                        {
                                uint32_t ch;
                                U8_NEXT_UNSAFE( value, length, ch );
                                if ( ch == static_cast<uint32_t>( '\0' ) )
                                        break;
                                if ( ch == static_cast<uint32_t>( 0x2021 ) ) // #\DOUBLE_DAGGER
                                {
                                        isCompound = true;
                                        /* Modify maximum compound flexion length if need to */
                                        if ( length > maxCompoundLength )
                                                maxCompoundLength = length;
                                        break;
                                }
                        }
                        
                        /* Add the flexion to the appropriate table */
                        if ( isCompound )
                        {
                                CompositeFlexionsHash::ResultStatus status =
                                        compositeTable.access( value, static_cast<FlexId>(i) );
                                if ( unlikely(status != CompositeFlexionsHash::RESULT_STATUS_SUCCESS) )
                                        return 1;
                        }
                        else
                        {
                                FlexionsHash::ResultStatus status =
                                        standardTable.access( value, static_cast<FlexId>(i) );
                                if ( unlikely(status != FlexionsHash::RESULT_STATUS_SUCCESS) )
                                        return 1;
                        }
		}

		return 0;
	}
	
	/** 
	 * Returns the array previously assigned.
	 * 
	 * 
	 * @return An array of flexions
	 */
	const char *revoke() const
	{
		return flexions;
	}

	/** 
	 * Returns a flexion for an index given.
	 * 
	 * @param index An index of flexion to return
	 * 
	 * @return A flexion requested
	 */
	const char *getFlexion(size_t index) const
	{
		return flexions + (index * maxFlexLength);
	}

	/** 
	 * Returns a flexion index by its symbol representation.
	 * 
	 * @param flexion A flexion to return an index for
	 * 
	 * @return A flexion index or (FlexId)(-1) if no such flexion within the array
	 */
	FlexId getIndex(const char *flexion) const
	{
		FlexId *index = 0;
		if ( standardTable.get(flexion, &index) == FlexionsHash::RESULT_STATUS_SUCCESS )
			return *index;
		return static_cast<FlexId>(-1);
	}
        
        /** 
	 * Returns a compound flexion index by its symbol representation.
	 * 
	 * @param flexion A compound flexion to return an index for
	 * 
	 * @return A compound flexion index or (FlexId)(-1) if no such flexion within the array
	 */
	FlexId getCompoundIndex(const char *flexion) const
	{
		FlexId *index = 0;
		if ( compositeTable.get(flexion, &index) == CompositeFlexionsHash::RESULT_STATUS_SUCCESS )
			return *index;
		return static_cast<FlexId>(-1);
	}

        /** 
         * Returns the maximum length of a compound flexion.
         * 
         * 
         * @return The compound flexion max length
         */
        size_t getMaxCompoundLength() const { return maxCompoundLength; }
        
private:
        /* A custom hasher for composite flexions: same as
         * OpenAddressingHasher<const char *>, but with different stop condition
         */
        struct CompositeFlexionHasher
        {
                uint32_t getHash(const char *key) const
                {
                        /* A standard djb2 hash function for ASCIIZ strings */
                        uint32_t hash = 5381;
                        for ( ; *key != '\0' &&
                                      !( static_cast<uint8_t>(key[0]) == 0xE2 &&
                                         static_cast<uint8_t>(key[1]) == 0x80 &&
                                         static_cast<uint8_t>(key[2]) == 0xA1 ); key++ )
                        {
                                int c = *key;
                                hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
                        }
                        return hash;
                }
        };
        
        /* A custom comparator for composite flexions: same as
         * KeyEqualComparator<const char *>, but with different stop condition
         */
        struct CompositeFlexionComparator
        {
                bool areKeysEqual(const char *firstKey, const char *secondKey) const
                {
                        for (;;)
                        {
                                char first  = ( ( static_cast<uint8_t>(firstKey[0]) == 0xE2 &&
                                                  static_cast<uint8_t>(firstKey[1]) == 0x80 &&
                                                  static_cast<uint8_t>(firstKey[2]) == 0xA1 ) ? '\0' : *firstKey );
                                char second  = ( ( static_cast<uint8_t>(secondKey[0]) == 0xE2 &&
                                                   static_cast<uint8_t>(secondKey[1]) == 0x80 &&
                                                   static_cast<uint8_t>(secondKey[2]) == 0xA1 ) ? '\0' : *secondKey );

                                if ( first != second )
                                        return false;
                                if ( first == '\0' )
                                        break;

                                firstKey++;
                                secondKey++;
                        }
                        return true;
                }
        };

        typedef OpenAddressingDict< const char *, FlexId, 1024, 0, 1> FlexionsHash;
        typedef OpenAddressingDict< const char *,
                                    FlexId,
                                    1024,
                                    0,
                                    1,
                                    CompositeFlexionComparator,
                                    CompositeFlexionHasher > CompositeFlexionsHash;
        
protected:
	const char            *flexions;
	size_t                 flexionsCount;
        size_t                 maxCompoundLength;
        FlexionsHash           standardTable;
        CompositeFlexionsHash  compositeTable;
};

/** 
 * This is a helper class.
 * Given a plain memory chunk of 'const FlexDistrib *' treats it as an two-dimension array of
 * flexion distributions with FlexDistribId and FlexId as indexes [i,j].
 * 
 */
class FlexDistribArray
{
public:
        FlexDistribArray() : distributions(0), flexionsCount(0) { }
	~FlexDistribArray() { }

public:
	/** 
	 * Assigns the address of distributions array and flexions count for index calculating.
	 * 
	 * @param distributions An address with distributions to assign
	 * @param flexionsCount A flexions count to assign
	 */
	void assign(const FlexDistrib *distributions, size_t flexionsCount)
	{
		this->distributions = distributions;
		this->flexionsCount = flexionsCount;
	}
	
	/** 
	 * Returns the array previously assigned.
	 * 
	 * 
	 * @return An array of flexions distributions
	 */
	const FlexDistrib *revoke() const
	{
		return distributions;
	}

	/** 
	 * Given the distribution id and a flexion id within it returns the word form list index
	 * that can be used with the help of WordFormsArray helper class.
	 * 
	 * @param distribId A distribution id
	 * @param flexId A flexion id within the distribution given
	 * 
	 * @return The word form list index or (WordFormListIdx)(-1) if blank.
	 */
	WordFormListIdx getWordFormListIdx(FlexDistribId distribId, FlexId flexId) const
	{
		return distributions[ static_cast<size_t>(distribId) * flexionsCount + flexId ];
	}

protected:
	const FlexDistrib *distributions;
	size_t             flexionsCount;
};

#endif /* _FLEXION_H_ */

