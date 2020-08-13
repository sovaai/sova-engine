#ifndef INFENGINE_TERMS_MATCHER_HPP
#define INFENGINE_TERMS_MATCHER_HPP

#include <Filtration3/terms/FltTermsBase.h>

#include <InfEngine2/InfEngine/TermsStorage.hpp>

/**
 *  Матчер для разбора результатов поиска в базе терминов.
 */
class CommonMatcher : public FltTermsMatcher
{
public:
	struct FoundTerm
	{
		const TERM * vTerm { nullptr };
		unsigned int vTermId = -1;
		NDocImage::TermCoords vCoords;
	};

public:
	/**
	 *  Типы фильтров.
	 */
	enum FilterType {
		FT_DICT_FILTER = 0, // Используется функцией IsBelong.
	};

protected:
	avector<FoundTerm> vFoundTerms;

public:
	CommonMatcher() { Reset(); }

	void SetTermsStorage( const TermsStorageRO * aTermsStorage ) { vTermsStorage = aTermsStorage; }

public:
	void SetInfPerson( unsigned int aInfPersonId ) { vInfPersonId = aInfPersonId; }

	// Получение количества найденных терминов.
	size_t GetTermsNum() { return m_found_terms.size(); }

	/** Получение aNum-того термина. */
	FoundTerm * GetFoundTerm( size_t aNum ) { return aNum < vFoundTerms.size() ? &( vFoundTerms[aNum] ) : nullptr; }

	size_t GetTermLen( size_t i ) { return m_ro_base->GetTermLength( m_found_terms[i].term ); }

	enum FltTermsErrors SetFilter( TermsStorageRO::Iterator::Filter * aFilter, enum FilterType aFilterType )
	{
		unsigned int filter_n = aFilterType;
		if( vFilters.size() <= filter_n )
		{
			unsigned int prev_size = vFilters.size();
			vFilters.resize( filter_n + 1 );
			if( vFilters.no_memory() )
				return TERMS_ERROR_ENOMEM;
			for( unsigned int n = prev_size; n < filter_n; ++n )
				vFilters[n] = nullptr;
		}
		vFilters[filter_n] = aFilter;
		return TERMS_OK;
	}

	virtual enum FltTermsErrors AddFound( const TERM * term, const TermsDocImageIterator & last_pos, TDATA_FLAGS_TYPE where, bool isHomo = false );

public:
	unsigned int GetTermId( unsigned int aTermNum ) const { return vFoundTerms[aTermNum].vTermId; }

	InfEngineErrors SearchAttrsByN( unsigned int aTermN, TermsStorageRO::CommonIterator & aIterator )
	{
		if( vTermsStorage )
		{
			aIterator.InitFilters( vFilters.get_buffer(), vFilters.size(), &( vFoundTerms[aTermN].vCoords ) );
			return vTermsStorage->SearchAttrs( vFoundTerms[aTermN].vTermId, aIterator );
		}

		return INF_ENGINE_ERROR_INTERNAL;
	}

	InfEngineErrors SearchAttrsByN( unsigned int aTermN, TermsStorageRO::PersonIdIterator & aIterator )
	{
		if( vTermsStorage )
		{
			aIterator.InitFilters( vFilters.get_buffer(), vFilters.size(), &( vFoundTerms[aTermN].vCoords ) );
			return vTermsStorage->SearchAttrs( vFoundTerms[aTermN].vTermId, vInfPersonId, aIterator );
		}

		return INF_ENGINE_ERROR_INTERNAL;
	}

	const NDocImage & GetNDocImage() const { return vNDocImage; }

public:
	/** Очистка данных. */
	virtual void Reset()
	{
		FltTermsMatcher::Reset();
		vFoundTerms.clear();
	}

	/** Освобождение памяти. */
	virtual void Free() { FltTermsMatcher::Free(); }

private:
	/** Обработка результатов поиска. */
	virtual enum FltTermsErrors FinalizeMatch( struct terms_di_info & cur_di_info );

private:
	/** База атрибутов терминов. */
	const TermsStorageRO * vTermsStorage {nullptr};

	/** Структура для быстрой навигации по входной фразе. */
	NDocImage vNDocImage;

	/** Пользовательские фильтры для найденных терминов. */
	avector<TermsStorageRO::Iterator::Filter*> vFilters;

	/** Идентификатор инфа. */
	unsigned int vInfPersonId = -1;
};

using DictMatcher = CommonMatcher;
using PatternMatcher = CommonMatcher;

#endif /** INFENGINE_TERMS_MATCHER_HPP */
