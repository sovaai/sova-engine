#ifndef INF_TERMS_HPP
#define INF_TERMS_HPP

#include <Filtration3/terms/FltTermsBase.h>

#include "InfTermAttrs.hpp"

/**
 *  Термин шаблона.
 */
class InfTerm
{
public:
	/** Получение указателя на термин. */
	const TERM * GetTerm() const { return vTerm.term; }


public:
	/** Получение идентификатора термина. */
	unsigned int GetId() const { return ( (InfTermAttrs*)term_attrs( vTerm.term ) )->GetId(); }

	/** Получение субидентификатора термина. */
	unsigned int GetSubId() const { return ( (InfTermAttrs*)term_attrs( vTerm.term ) )->GetSubId(); }

	/** Получение позиции термина в шаблоне. */
	unsigned int GetPos() const { return ( (InfTermAttrs*)term_attrs( vTerm.term ) )->GetPos(); }


public:
	/** Получение флага, показывающего является ли данный термин последним в шаблоне. */
	bool GetFlagLastTerm() const { return ( (InfTermAttrs*)term_attrs( vTerm.term ) )->GetFlagLastTerm(); }

	/** Получение флага, показывающего является ли данный термин первым не виртуальным термином в шаблон-вопросе. */
	bool GetFlagFirstRealTerm() const { return ( (InfTermAttrs*)term_attrs( vTerm.term ) )->GetFlagFirstRealTerm(); }

	/** Получение флага, показывающего, что перед термином идет звездочка. */
	bool GetFlagStarBefore() const { return ( (InfTermAttrs*)term_attrs( vTerm.term ) )->GetFlagStarBefore(); }

	/** Получение флага, показывающего, что после термина идет звездочка. */
	bool GetFlagStarAfter() const { return ( (InfTermAttrs*)term_attrs( vTerm.term ) )->GetFlagStarAfter(); }

	/** Получение флага, показывающего, что перед термином идет суперзвездочка. */
	bool GetFlagSuperStarBefore() const { return ( (InfTermAttrs*)term_attrs( vTerm.term ) )->GetFlagSuperStarBefore(); }

	/** Получение флага, показывающего, что после термина идет суперзвездочка. */
	bool GetFlagSuperStarAfter() const { return ( (InfTermAttrs*)term_attrs( vTerm.term ) )->GetFlagSuperStarAfter(); }


public:
	/** Получение позиции начала термина в словах. */
	const TermsDocImageIterator & GetTermBeginCoord() const { return vTerm.coords.first_pos; }

	/** Получение позиции конца термина в словах. */
	const TermsDocImageIterator & GetTermEndCoord() const { return vTerm.coords.last_pos; }

	/** Получение длины термина в словах. */
	size_t GetTermWordsLen() const
	{
		TermsDocImageIterator t = vTerm.coords.first_pos;
		size_t res = 1;
		for(; !( t == vTerm.coords.last_pos ); ++t )
			res++;
		return res;

	}


private:
	/** Ссылка на термин. */
	FltTermsMatcher::FoundTerm vTerm;
};

#endif /** INF_TERMS_HPP */
