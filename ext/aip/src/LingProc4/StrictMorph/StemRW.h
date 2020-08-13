/**
 * @file   StemRW.h
 * @author swizard <me@swizard.info>
 * @date   Sat May 17 17:50:10 2008
 * 
 * @brief  The first part of a word form (read/write variant).
 * 
 * 
 */
#ifndef _STEMRW_H_
#define _STEMRW_H_

#include <_include/_string.h>

#include "Stem.h"

inline void setStemsExtraFlags(uint8_t *flags, size_t bitIdx)
{
        size_t octetIdx = bitIdx >> 3;
        flags[octetIdx] |= static_cast<uint8_t>(1 << (bitIdx - (octetIdx << 3)));
}

inline void clearStemsExtraFlags(uint8_t *flags, size_t bitIdx)
{
        size_t octetIdx = bitIdx >> 3;
        flags[octetIdx] &= ~( static_cast<uint8_t>(1 << (bitIdx - (octetIdx << 3))) );
}

class StemRecordsArrayRW : public StemRecordsArray
{
public:
        StemRecordsArrayRW(uint8_t *_data, size_t _dataSize) : StemRecordsArray(_data, _dataSize) {}
	~StemRecordsArrayRW() { }

public:
	/** 
	 * Sets the stem number within a single lexeme.
	 * 
	 * @param index An index within array
	 * @param number A stem number.
	 */
	void setLexStemNumber(size_t index, size_t number)
	{
		setLexRef( index, (getLexRef(index) & 0x07FFFFFFUL) | ((number & 0x0000001FUL) << 27) );
	}

	/** 
	 * Sets the orthographical variant of a lexeme.
	 * 
	 * @param index An index within array
	 * @param number An orthographical variant
	 */
	void setLexOrthNumber(size_t index, size_t number)
	{
		setLexRef( index, (getLexRef(index) & 0xF87FFFFFUL) | ((number & 0x0000000FUL) << 23) );
	}

	/** 
	 * Sets the dictionary capitalization of a lexeme.
	 * 
	 * @param index An index within array
	 * @param caps A dictionary capitalization
	 */
	void setLexCaps(size_t index, uint8_t caps)
	{
		setLexRef( index, (getLexRef(index) & 0xFF9FFFFFUL) | ((caps & 0x00000003UL) << 21) );
	}

	/** 
	 * Sets the lexeme id.
	 * 
	 * @param index An index within array
	 * @param id A lexeme id
	 */
	void setLexId(size_t index, LexemeId id)
	{
		setLexRef( index, (getLexRef(index) & 0xFFE00000UL) | (id & 0x001FFFFFUL) );
	}

	/** 
	 * Sets the flexions distribution id.
	 * 
	 * @param index An index within array
	 * @param id A flexions distribution id
	 */
	void setFlexDistribId(size_t index, FlexDistribId id)
	{
 		uint8_t *dataRW = const_cast<uint8_t *>(data);
                id &= distribIdMask;
		*( reinterpret_cast<FlexDistribId *>(dataRW + (sizeof(LexemeRef) * recordsCount)) + index) = id;
	}

	/** 
	 * Sets the composite flexion bit of the stem.
	 * 
	 * @param index An index within array
	 */
	void setCompositeBit(size_t index)
	{
 		uint8_t *dataRW = const_cast<uint8_t *>(data);
		*( reinterpret_cast<FlexDistribId *>
                   (dataRW + (sizeof(LexemeRef) * recordsCount)) + index ) |= compositeBitMask;
	}
        
	/** 
	 * Clears the composite flexion bit of the stem.
	 * 
	 * @param index An index within array
	 */
	void clearCompositeBit(size_t index)
	{
 		uint8_t *dataRW = const_cast<uint8_t *>(data);
		*( reinterpret_cast<FlexDistribId *>
                   (dataRW + (sizeof(LexemeRef) * recordsCount)) + index ) &= ~compositeBitMask;
	}

	/** 
	 * Sets the composite stop word bit of the stem.
	 * 
	 * @param index An index within array
	 */
	void setStopBit(size_t index)
	{
 		uint8_t *dataRW = const_cast<uint8_t *>(data);
		*( reinterpret_cast<FlexDistribId *>
                   (dataRW + (sizeof(LexemeRef) * recordsCount)) + index ) |= stopBitMask;
	}
        
	/** 
	 * Clears the composite stop word bit of the stem.
	 * 
	 * @param index An index within array
	 */
	void clearStopBit(size_t index)
	{
 		uint8_t *dataRW = const_cast<uint8_t *>(data);
		*( reinterpret_cast<FlexDistribId *>
                   (dataRW + (sizeof(LexemeRef) * recordsCount)) + index ) &= ~stopBitMask;
	}
        
        /** 
         * Sets the composite word rule exclusion bit. 
         * 
         */
        void setExcludeBit()
        {
 		uint8_t *dataRW = const_cast<uint8_t *>( getStemsExtraFlagsPtr() );
                setStemsExtraFlags( dataRW, excludeBitIdx );
        }

        /** 
         * Clears the composite word rule exclusion bit. 
         * 
         */
        void clearExcludeBit()
        {
 		uint8_t *dataRW = const_cast<uint8_t *>( getStemsExtraFlagsPtr() );
                clearStemsExtraFlags( dataRW, excludeBitIdx );
        }
        
        /** 
         * Sets the composite word membership restrict bit.
         * 
         */
        void setRestrictBit()
        {
 		uint8_t *dataRW = const_cast<uint8_t *>( getStemsExtraFlagsPtr() );
                setStemsExtraFlags( dataRW, restrictBitIdx );
        }

        /** 
         * Clears the composite word membership restrict bit.
         * 
         */
        void clearRestrictBit()
        {
 		uint8_t *dataRW = const_cast<uint8_t *>( getStemsExtraFlagsPtr() );
                clearStemsExtraFlags( dataRW, restrictBitIdx );
        }
        
protected:
	void setLexRef(size_t index, LexemeRef value)
	{
		uint8_t *dataRW = const_cast<uint8_t *>(data);
		*( reinterpret_cast<LexemeRef *>(dataRW) + index ) = value;
	}
};

#endif /* _STEMRW_H_ */

