/**
 * @file   WordForm.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 14 19:52:27 2008
 * 
 * @brief  Word form -- grammatical form specific representation of a single word.
 * 
 * 
 */
#ifndef _WORDFORM_H_
#define _WORDFORM_H_

#include <sys/types.h>
#include <_include/_inttype.h>

#include "Paradigm.h"

typedef uint8_t  WordFormNumber;
typedef uint8_t  WordFormAttribs;
typedef uint16_t WordFormListIdx;

const WordFormNumber WORD_FORM_NUMBER_EMPTY = static_cast<WordFormNumber>(-1);

class WordForm
{
protected:
	enum Constants
        {
                totalDataSize = 8
        };
	
public:
	/** 
	 * Returns the grammatical form identificator.
	 * Should be an index within a ParadigmForm array.
	 * 
	 * 
	 * @return A grammatical form identificator
	 * @see ParadigmFormId
	 */
	ParadigmFormId getParadigmFormId() const
	{
		return *( reinterpret_cast<const ParadigmFormId *>(data) ) & 0x7FFF;
	}
	
	/** 
	 * Returns the form number within the paradigm.
	 * 
	 * 
	 * @return Form number
	 */
	ParadigmFormNumber getFormNumber() const
	{
		return *( reinterpret_cast<const ParadigmFormNumber *>(data                   +
								       sizeof(ParadigmFormId) +
								       sizeof(WordFormNumber) +
								       sizeof(WordFormAttribs)) );
	}
	
	/** 
	 * Checks if the current word form is last within a some list.
	 * 
	 * 
	 * @return Zero if not last, non-zero otherwise
	 */
	int isLastInList() const
	{
		return *( reinterpret_cast<const ParadigmFormId *>(data) ) & 0x8000;
	}

	/** 
	 * Returns the word form number within a grammatical form.
	 * 
	 * 
	 * @return Word form number
	 */
	WordFormNumber getFormVariantNumber() const
	{
		return *( reinterpret_cast<const WordFormNumber *>(data + sizeof(ParadigmFormId)) );
	}

	/** 
	 * Returns the word form attributes.
	 * 
	 * 
	 * @return Word form attributes.
	 */
	WordFormAttribs getAttribs() const
	{
		return *( reinterpret_cast<const WordFormAttribs *>(data                   +
								    sizeof(ParadigmFormId) +
								    sizeof(WordFormNumber)) );
	}
        
        /** 
         * Returns the morphology internal form number.
         * 
         * 
         * @return Internal form number.
         */
        uint8_t getInternalMorphNo() const
        {
		return *( reinterpret_cast<const uint8_t *>(data                   +
                                                            sizeof(ParadigmFormId) +
                                                            sizeof(WordFormNumber) +
                                                            sizeof(WordFormAttribs) +
                                                            sizeof(ParadigmFormNumber) ) );
        }
	
protected:
	WordForm() { }
	~WordForm() { }

protected:
	/* Logical data structure:
	 *
	 * List end flag:     1  bit
	 * Paradigm form id:  15 bits (ParadigmFormId)
	 * Number:            1  octet (WordFormNumber)
	 * Attibutes:         1  octet (WordFormAttribs)
	 * Paradigm form no:  2  octets (ParadigmFormNumber)
         * Internal morph no: 1  octet
	 * Reserved:          1  octet
	 */
	uint8_t data[ totalDataSize ];
};

class WordFormsArray
{
public:
        WordFormsArray() : array(0) { }
        WordFormsArray(const WordForm *_array) : array(_array) { }
	~WordFormsArray() { }

public:
	/** 
	 * Initializes the current buffer with a data provided.
	 * 
	 * @param array An array of WordForm's to use
	 */
	void assign(const WordForm *array)
	{
		this->array = array;
	}

	/** 
	 * Returns the WordForm from an array located at the index provided.
	 * 
	 * @param index An index of WordForm within an array.
	 * 
	 * @return WordForm requested or null form
	 */
	const WordForm *getAt(WordFormListIdx index) const
	{
		return array + index;
	}

	/** 
	 * Returns the next WordForm in a list (if exists) within an array.
	 * 
	 * @param form A current WordForm
	 * @param index A current index
	 * 
	 * @return A next WordForm if current one is not the last in list, zero otherwise
	 */
	const WordForm *getNext(const WordForm *form, WordFormListIdx index) const
	{
		return ( form->isLastInList() ? 0 : getAt(index) );
	}

	/** 
	 * Returns the list length (number of elements until end-of-list mark encountered).
	 * 
	 * 
	 * @return The list length
	 */
	size_t getListLength()
	{
		WordFormListIdx idx = 0;
		for ( const WordForm *e = getAt(idx); e != 0; idx++, e = getNext(e, idx) );
		return static_cast<size_t>(idx);
	}
	
protected:
	const WordForm *array;
};

#endif /* _WORDFORM_H_ */

