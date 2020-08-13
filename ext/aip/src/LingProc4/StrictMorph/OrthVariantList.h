/**
 * @file   OrthVariantList.h
 * @author swizard <me@swizard.info>
 * @date   Sun Jun 22 22:12:04 2008
 * 
 * @brief  A lexeme orthographic variants list manipulator class.
 * 
 * 
 */
#ifndef _ORTHVARIANTLIST_H_
#define _ORTHVARIANTLIST_H_

#include <sys/types.h>
#include <_include/cc_compat.h>

#include <lib/aptl/OpenAddressingDict.h>

#include "Lexeme.h"

typedef OpenAddressingDict<LexemeId, const LexemeId *, 2048, 0, 1> OrthVariantDict;

/* A list of orthographic variants */
class OrthVariantList :	public OrthVariantDict
{
public:
	OrthVariantList() :
		OrthVariantDict(),
		list( 0 ),
		listLength( 0 )
	{
	}
	~OrthVariantList() { }

public:
	/** 
	 * Assigns a lexeme orthographic variants list data.
	 * 
	 * @param list A list of variants
	 * @param listLength Total list length
	 * 
	 * @return zero if successfull, non-zero otherwise
	 */
	int assign( const LexemeId *list, size_t listLength )
	{
		this->list       = list;
		this->listLength = listLength;
		
		bool setNext = true;
		for (register size_t i = 0; i < listLength; i++)
		{
			if ( setNext )
			{
                                OrthVariantDict::ResultStatus result =
                                        OrthVariantDict::access( list[i], (list + i) );
				if ( unlikely(result != OrthVariantDict::RESULT_STATUS_SUCCESS) )
					return 1;
				setNext = false;
			}

			const CommonReference<LexemeId> *lexRef =
				reinterpret_cast<const CommonReference<LexemeId> *>(list + i);
			if ( lexRef->isListReference() )
				setNext = true;
		}

		return 0;
	}
	
	/** 
	 * Returns the array previously assigned.
	 * 
	 * 
	 * @return An array of lexeme ids
	 */
	const LexemeId *revoke()
	{
		return list;
	}

private:
	const LexemeId *list;
	size_t          listLength;
};

#endif /* _ORTHVARIANTLIST_H_ */

