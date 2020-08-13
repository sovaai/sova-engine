/**
 * @file   Stem.h
 * @author swizard <me@swizard.info>
 * @date   Sat May 17 16:19:14 2008
 * 
 * @brief  The first part of a word form.
 * 
 * 
 */
#ifndef _STEM_H_
#define _STEM_H_

#include <sys/types.h>

#include <_include/_inttype.h>

#include <LingProc4/StrictMorphGram/StrictLex.h>

#include "StemCommon.h"
#include "Flexion.h"
#include "Lexeme.h"

inline uint8_t getStemsExtraFlags(const uint8_t *flags, size_t bitIdx)
{
        size_t octetIdx = bitIdx >> 3;
        return flags[octetIdx] & static_cast<uint8_t>(1 << (bitIdx - (octetIdx << 3)));
}

class StemRecordsArray
{
protected:
        enum ProtectedConstants
        {
                distribIdMask    = (1U << ((sizeof(FlexDistribId) << 3) - 2)) - 1,
                compositeBitMask = (1U << ((sizeof(FlexDistribId) << 3) - 2)),
                stopBitMask      = (1U << ((sizeof(FlexDistribId) << 3) - 1)),
                excludeBitIdx    = 0,
                restrictBitIdx   = 1
        };
        
public:
	enum PublicConstants
        {
                elementSize = sizeof(LexemeRef) + sizeof(FlexDistribId)
        };
	
public:
        StemRecordsArray(const uint8_t *_data, size_t _dataSize) :
 	        data(_data), recordsCount(_dataSize / elementSize) { }
	~StemRecordsArray() { }

protected:
        StemRecordsArray() : data(0), recordsCount(0) { }

public:
	/** 
	 * Returns the stem number within a single lexeme.
	 * 
	 * @param index An index within array
	 * 
	 * @return Stem number
	 */
	size_t getLexStemNumber(size_t index) const
	{
		return ( getLexRef(index) & 0xF8000000UL ) >> 27;
	}

	/** 
	 * Returns orthographical variant of lexeme.
	 * 
	 * @param index An index within array
	 * 
	 * @return Orthographical variant
	 */
	size_t getLexOrthNumber(size_t index) const 
	{
		return ( getLexRef(index) & 0x07800000UL ) >> 23;
	}

	/** 
	 * Returns dictionary capitalization of a lexeme.
	 * 
	 * @param index An index within array
	 * 
	 * @return Dictionary capitalization
	 */
        StrictLex::DictCaps getLexCaps(size_t index) const
	{
		return static_cast<StrictLex::DictCaps>( ( getLexRef(index) & 0x00600000UL ) >> 21 );
	}

	/** 
	 * Returns lexeme id.
	 * Should be equal to the index of a lexemes array.
	 * 
	 * @param index An index within array
	 * 
	 * @return Lexeme id
	 */
	LexemeId getLexId(size_t index) const
	{
		return ( getLexRef(index) & 0x001FFFFFUL );
	}

	/** 
	 * Returns the flexions distribution id.
	 * Should be equal to the index of flex distributions array.
	 * 
	 * @param index An index within array
	 * 
	 * @return Flexions distribution id
	 */
	FlexDistribId getFlexDistribId(size_t index) const
	{
		return ( *(reinterpret_cast<const FlexDistribId *>
                           (data + (sizeof(LexemeRef) * recordsCount)) + index) ) & distribIdMask;
	}

	/** 
	 * Checks the composite flexion bit of the stem.
	 * 
	 * @param index An index within array
	 * 
	 * @return Non-zero if bit is set or zero otherwise
	 */
	FlexDistribId checkCompositeBit(size_t index) const
	{
		return ( *(reinterpret_cast<const FlexDistribId *>
                           (data + (sizeof(LexemeRef) * recordsCount)) + index) ) & compositeBitMask;
	}
        
	/** 
	 * Checks the composite stop word bit of the stem.
	 * 
	 * @param index An index within array
	 * 
	 * @return Non-zero if bit is set or zero otherwise
	 */
	FlexDistribId checkStopBit(size_t index) const
	{
		return ( *(reinterpret_cast<const FlexDistribId *>
                           (data + (sizeof(LexemeRef) * recordsCount)) + index) ) & stopBitMask;
	}

        /** 
         * Checks the composite word rule exclusion bit. 
         * 
         * 
         * @return Non-zero if bit is set or zero otherwise
         */
        uint32_t checkExcludeBit() const
        {
                return getStemsExtraFlags( getStemsExtraFlagsPtr(), excludeBitIdx );
        }
        
        /** 
         * Checks the composite word membership restrict bit. 
         * 
         * 
         * @return Non-zero if bit is set or zero otherwise
         */
        uint32_t checkRestrictBit() const
        {
                return getStemsExtraFlags( getStemsExtraFlagsPtr(), restrictBitIdx );
        }
        
	/** 
	 * Returns the records count contained in the array.
	 * 
	 * 
	 * @return The records count
	 */
	size_t getRecordsCount() const { return recordsCount; }
        
public:
        void assign( StemRecordsArray &array ) const
        {
                array.data         = data;
                array.recordsCount = recordsCount;
        }

        const uint8_t *getDataPtr() const { return data; }
        
protected:
	LexemeRef getLexRef(size_t index) const
	{
		return *(reinterpret_cast<const LexemeRef *>(data) + index);
	}
        
        const uint8_t *getStemsExtraFlagsPtr() const
        {
                return data + ((sizeof(LexemeRef) + sizeof(FlexDistribId)) * recordsCount);
        }
        
protected:
	/* Logical data structure:
	 *
	 * Lexeme stem number:            5  bits
	 * Lexeme orthographical variant: 4  bits
	 * Lexeme capitalization:         2  bits
	 * Lexeme identificator:          21 bits
	 * ...
	 * -----------------------------
         * Composite flag:                1  bit
         * Stop word flag:                1  bit
	 * Flexions distribution id:      14 bits
	 * ...
         * -----------------------------
         * Extra: compound exclude flag   1  bit
         * Extra: compound restrict flag  1  bit
	 */
	const uint8_t *data;
	size_t         recordsCount;
};

#endif /* _STEM_H_ */

