#include "Matcher.hpp"

#include <algorithm>

#include "InfTermAttrs.hpp"

bool found_cmp_by_id( const FltTermsMatcher::FoundTerm & a, const FltTermsMatcher::FoundTerm & b )
{
	return ( (InfTermAttrs*)term_attrs( a.term ) )->GetId() < ( (InfTermAttrs*)term_attrs( b.term ) )->GetId();
}

bool found_cmp_by_pos( const FltTermsMatcher::FoundTerm & a, const FltTermsMatcher::FoundTerm & b )
{
	if( a.coords.first_pos == b.coords.first_pos )
	{
		return a.coords.last_pos < b.coords.last_pos;
	}
	else
		return a.coords.first_pos < b.coords.first_pos;
}

enum FltTermsErrors CommonMatcher::FinalizeMatch( struct terms_di_info & cur_di_info )
{
	m_di = cur_di_info.di;
	m_di_size = cur_di_info.size;
	m_di_length = cur_di_info.length;
	m_di_lang_prim = cur_di_info.lang_prim;
	m_di_lang_supp = cur_di_info.lang_supp;

	// Заполнение струкур для навигации по результатам поиска.
	vNDocImage.Reset();
	if( m_found_terms.size() )
	{
		// Удаление дублей.
		std::sort( m_found_terms.get_buffer(), m_found_terms.get_buffer() + m_found_terms.size(), found_cmp_by_id );

		FoundTermsPool tmp;
		unsigned int uniq = 1;
		// Подсчёт уникальных терминов.
		for( unsigned int found_n = 1; found_n < m_found_terms.size(); ++found_n )
		{
			if( ( (InfTermAttrs*)term_attrs( m_found_terms[found_n].term ) )->GetId() !=
				( (InfTermAttrs*)term_attrs( m_found_terms[found_n - 1].term ) )->GetId() ||
				!( m_found_terms[found_n].coords.first_pos == m_found_terms[found_n - 1].coords.first_pos ) )
				++uniq;
		}
		// Копирование уникальных терминов.
		tmp.resize( uniq );
		if( tmp.no_memory() )
			return TERMS_ERROR_ENOMEM;
		tmp[0] = m_found_terms[0];
		uniq = 1;
		for( unsigned int found_n = 1; found_n < m_found_terms.size(); ++found_n )
		{
			if( ( (InfTermAttrs*)term_attrs( m_found_terms[found_n].term ) )->GetId() !=
				( (InfTermAttrs*)term_attrs( m_found_terms[found_n - 1].term ) )->GetId() ||
				!( m_found_terms[found_n].coords.first_pos == m_found_terms[found_n - 1].coords.first_pos ) )
				tmp[uniq++] = m_found_terms[found_n];
		}


		// Сортировка по положению во входной фразе.
		std::sort( tmp.get_buffer(), tmp.get_buffer() + tmp.size(), found_cmp_by_pos );

		// Копирование результата удаления дублей.
		m_found_terms.resize( tmp.size() );
		if( m_found_terms.no_memory() )
			return TERMS_ERROR_ENOMEM;
		memcpy( m_found_terms.get_buffer(), tmp.get_buffer(), tmp.size() * sizeof( FltTermsMatcher::FoundTerm ) );


		// Заполнение списка итераторов для быстрой навигации по фразе.
		InfEngineErrors iee = vNDocImage.Init( m_found_terms.back().coords.first_pos );
		if( INF_ENGINE_SUCCESS != iee )
		{
			if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
				return TERMS_ERROR_ENOMEM;
			return TERMS_ERROR_EFAULT;
		}

		// Заполнение списка результатов поиска терминов.
		vFoundTerms.resize( m_found_terms.size() );
		if( vFoundTerms.no_memory() )
			return TERMS_ERROR_ENOMEM;

		for( unsigned int found_n = 0; found_n < m_found_terms.size(); ++found_n )
		{
			unsigned int first_pos = 0;
			unsigned int last_pos = 0;

			// Поиск начала очередного термина во входной вразе.
			while( first_pos < vNDocImage.Size() && vNDocImage[first_pos] != m_found_terms[found_n].coords.first_pos )
				++first_pos;
			// Неизвестная ошибка.
			if( first_pos >= vNDocImage.Size() )
				return TERMS_ERROR_EFAULT;

			// Поиск конца очередного термина во входной вразе.
			last_pos = first_pos;
			while( last_pos < vNDocImage.Size() && vNDocImage[last_pos] != m_found_terms[found_n].coords.last_pos )
				++last_pos;
			// Неизвестная ошибка.
			if( last_pos >= vNDocImage.Size() )
				return TERMS_ERROR_EFAULT;

			// Сохранение информации о вхождении в новом формате.
			// Начало термина.
			vFoundTerms[found_n].vCoords.vFirstPos = vNDocImage[first_pos];
			// Конец термина.
			vFoundTerms[found_n].vCoords.vLastPos = vNDocImage[last_pos];
			// Термин.
			vFoundTerms[found_n].vTerm = m_found_terms[found_n].term;
			// Идентификатор термина для поиска его атрибутов в базе атрибутов.
			vFoundTerms[found_n].vTermId = ( (InfTermAttrs*)term_attrs( m_found_terms[found_n].term ) )->GetId();
		}
	}

	return TERMS_OK;
}

enum FltTermsErrors CommonMatcher::AddFound( const TERM * term, const TermsDocImageIterator & last_pos, TDATA_FLAGS_TYPE where, bool isHomo )
{
	// find first term position
	TermsDocImageIterator first_pos = last_pos;
	size_t term_pos = m_ro_base->GetTermLength( term );
	if( term_pos )
		--term_pos;
	while( !first_pos.IsHead() && term_pos )
	{
		--first_pos;
		--term_pos;
	}

	if( m_last_main != static_cast<size_t>( -1 ) )
	{
		if( ( first_pos == m_found_terms[m_last_main].coords.first_pos ) &&
			( last_pos  == m_found_terms[m_last_main].coords.last_pos )  &&
			( ( (InfTermAttrs*)term_attrs( term ) )->GetId() ==
			  ( (InfTermAttrs*)term_attrs( m_found_terms[m_last_main].term ) )->GetId() ) )
			return TERMS_OK;
	}

	FltTermsMatcher::FoundTerm * fterm = m_found_terms.grow();
	if( !fterm )
		return TERMS_ERROR_ENOMEM;
	fterm->term = term;
	fterm->where = where;
	fterm->coords.first_pos = first_pos;
	fterm->coords.last_pos = last_pos;
	fterm->deleted = false;

	m_last_main = m_found_terms.size() - 1;

	return TERMS_OK;
}
