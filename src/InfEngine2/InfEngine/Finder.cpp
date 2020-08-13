#include "Finder.hpp"
#include <ClientLib/Protocol.hpp>

InfEngineErrors BFinder::CachedMatchingResults::AlreadyMatched( unsigned int aMainId, unsigned int aSubId, MatchingResult & aResult ) const
{
	// Получение идентификатора строки словаря/шаблон-вопроса, выданного хранилищем шаблонов.
	unsigned int aId = GetStringId( aMainId, aSubId );

	aResult = MR_NOT_MATCHED;

	// Поиск в хэше результатов сопоставления заданной строки/вопроса.
	Cache::const_iterator iter = vCache.find( aId );

	// Строка ранее не сопоставлялась.
	if( vCache.end() == iter )
		return INF_ENGINE_SUCCESS;

	// Если ещё не все варианты сопоставления строки были рассмотрены.
	if( iter->second.vMainId == aMainId && iter->second.vSubId == aSubId )
	{
		aResult = MR_NOT_MATCHED;
	}
	// Если сопоставление строки/вопроса завершилось успехом.
	else if( MR_SUCCESS == iter->second.vMatchingResult )
	{
		aResult = MR_SUCCESS;
	}
	// Если сопоставление строки/вопроса завершилось неуспехом.
	else if( MR_FAIL == iter->second.vMatchingResult )
	{
		aResult = MR_FAIL;
	}
	else return INF_ENGINE_ERROR_INTERNAL;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::CachedMatchingResults::TryMatch( unsigned int aMainId, unsigned int aSubId )
{
	// Получение идентификатора строки словаря/шаблон-вопроса, выданного хранилищем шаблонов.
	unsigned int aId = GetStringId( aMainId, aSubId );

	try
	{
		if( vCache.find( aId ) == vCache.end() )
			vCache[aId] = CacheNode( MR_FAIL, static_cast<unsigned int>( -1 ), aMainId, aSubId );
	}
	catch( ... )
	{
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::CachedMatchingResults::CopyMatch( unsigned int aMainId, unsigned int aSubId, unsigned int & debug_cnt, unsigned int aCondWeight )
{

	// Получение идентификатора строки словаря/шаблон-вопроса, выданного хранилищем шаблонов.
	unsigned int aId = GetStringId( aMainId, aSubId );

	// Поиск в хэше результатов сопоставления заданной строки/вопроса.
	Cache::const_iterator iter = vCache.find( aId );

	// Строка/вопрос ранее не сопоставлялась.
	if( vCache.end() == iter )
		return INF_ENGINE_ERROR_INTERNAL;

	// Если сопоставление строки/вопроса завершилось неуспехом.
	if( MR_SUCCESS != iter->second.vMatchingResult )
		return INF_ENGINE_ERROR_INTERNAL;

	// Копирование ранее найденных результатов сопоставления строки/вопроса.
	return CopyFrom( iter->second.vMatchN, aMainId, aSubId, debug_cnt, aCondWeight );
}

InfEngineErrors BFinder::CachedMatchingResults::AddToCache( unsigned int aId, unsigned int aPos, unsigned int aMainId, unsigned int aSubId )
{
	try
	{
		// Поиск записи в хэше.
		Cache::iterator iter = vCache.find( aId );
		if( iter == vCache.end() )
			return INF_ENGINE_ERROR_INTERNAL;

		// Если это первый вариант успешного сопоставления данного шаюлон-вопроса / строки словаря.
		if( MR_FAIL == iter->second.vMatchingResult )
			vCache[aId] = CacheNode( MR_SUCCESS, aPos, aMainId, aSubId );
		// Если это уже не первый вариант успешного сопоставления данного шаюлон-вопроса / строки словаря.
		else if( MR_SUCCESS == iter->second.vMatchingResult )
			iter->second.vMatchN.push_back( aPos );
	}
	catch( ... )
	{
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	}

	return INF_ENGINE_SUCCESS;
}

// Функция сравнения для сортировки кандидатов по идентификатору.
inline int BFinderCandidatCmpById( const void * aFirst, const void * aSecond )
{
	BFinder::Candidat * a = *( (BFinder::Candidat**)aFirst );
	BFinder::Candidat * b = *( (BFinder::Candidat**)aSecond );
	return a->vPatternId < b->vPatternId ?
						   -1 :
						   a->vPatternId > b->vPatternId ?
		   1 :
		   a->vWeight > b->vWeight ?
		   -1 :
		   a->vWeight < b->vWeight ?
		   1 : 0;
}

// Функция сравнения для сортировки кандидатов по весу.
inline int InfBFinderCandidatesCmpByWeigth( const void * pa, const void * pb )
{
	BFinder::Candidat * a = *( (BFinder::Candidat**)pa );
	BFinder::Candidat * b = *( (BFinder::Candidat**)pb );
	return a->vWeight < b->vWeight ?
						1 :
						a->vWeight > b->vWeight ?
		   -1 :
		   a->vPatternId < b->vPatternId ?
						   1 :
						   a->vPatternId > b->vPatternId ?
		   -1 :
		   0;
}

// Копирование кандидата.
inline BFinder::Candidat * CreateCandidatCopy( nMemoryAllocator & aAllocator, BFinder::Candidat & aCandidat )
{
	// Выделение памяти.
	BFinder::Candidat * cand = static_cast<BFinder::Candidat*>( aAllocator.Allocate( sizeof( BFinder::Candidat ) ) );
	if( !cand )
		return nullptr;
	new ( (nMemoryAllocatorElementPtr*)cand ) BFinder::Candidat;

	// Копирование данных.
	cand->vPatternId = aCandidat.vPatternId;
	cand->vQuestionId = aCandidat.vQuestionId;

	// Выделение памяти для копирования массива указателей на элементы.
	cand->vItems = static_cast<BFinder::Candidat::Item*>( aAllocator.Allocate( sizeof( BFinder::Candidat::Item ) * aCandidat.vItemsNumber ) );
	if( !cand->vItems )
		return nullptr;
	memset( cand->vItems, 0, sizeof( BFinder::Candidat::Item ) * aCandidat.vItemsNumber );
	for( unsigned int i = 0; i < aCandidat.vItemsNumber; i++ )
		new ( (nMemoryAllocatorElementPtr*)( cand->vItems + i ) ) BFinder::Candidat::Item;

	// Копирование массива указателей на элементы.
	memcpy( cand->vItems, aCandidat.vItems, sizeof( BFinder::Candidat::Item ) * aCandidat.vItemsNumber );

	// Копирование данных.
	cand->vItemsNumber = aCandidat.vItemsNumber;
	cand->vWeight = aCandidat.vWeight;

	return cand;
}

// Создание нового кандидата.

inline BFinder::Candidat * CreateNewCandidat( nMemoryAllocator & aAllocator, unsigned int aSize )
{
	// Выделение памяти.
	BFinder::Candidat * cand = static_cast<BFinder::Candidat*>( aAllocator.Allocate( sizeof( BFinder::Candidat ) ) );
	if( !cand )
		return nullptr;
	new ( (nMemoryAllocatorElementPtr*)cand ) BFinder::Candidat;

	// Выделение памяти для массива ссылок на элементы.
	cand->vItems = (BFinder::Candidat::Item*)aAllocator.Allocate( sizeof( BFinder::Candidat::Item ) * aSize );
	if( !cand->vItems )
		return nullptr;
	memset( cand->vItems, 0, sizeof( BFinder::Candidat::Item ) * aSize );
	for( unsigned int i = 0; i < aSize; i++ )
		new ( (nMemoryAllocatorElementPtr*)( cand->vItems + i ) ) BFinder::Candidat::Item;
	cand->vItemsNumber = aSize;

	return cand;
}

bool cmp_dict_matches( const BFinder::DictsMatchingResults::DictMatchInfo & a, const BFinder::DictsMatchingResults::DictMatchInfo & b )
{
	if( a.vDictId == b.vDictId )
	{
		if( a.vBeginPos.GetPos() == b.vBeginPos.GetPos() )
		{
			if( a.vEndPos.GetPos() == b.vEndPos.GetPos() )
				return a.vLineId < b.vLineId;
			else
				return a.vEndPos.GetPos() < b.vEndPos.GetPos();
		}
		else
			return a.vBeginPos.GetPos() < b.vBeginPos.GetPos();
	}

	return a.vDictId < b.vDictId;
}

bool cmp_crossdict_matches( const BFinder::DictsMatchingResults::DictMatchInfo & a, const BFinder::DictsMatchingResults::DictMatchInfo & b )
{
	if( a.vBeginPos.GetPos() == b.vBeginPos.GetPos() )
	{
		if( a.vEndPos.GetPos() == b.vEndPos.GetPos() )
			return a.vLineId < b.vLineId;
		else
			return a.vEndPos.GetPos() < b.vEndPos.GetPos();
	}

	return a.vBeginPos.GetPos() < b.vBeginPos.GetPos();
}

InfEngineErrors BFinder::DictsMatchingResults::GetMatch( unsigned int aDictId, DictMatch & aDictMatch ) const
{
	// Поиск результатов среди ранее полученных.
	if( aDictId < vDisclosedIndexCeilSize.size() && vDisclosedIndexCeilSize[aDictId] )
	{
		aDictMatch = DictMatch( vDisclosedIndexCeilSize[aDictId], vDisclosedMatches.get_buffer() + vDisclosedIndex[aDictId] );
		return INF_ENGINE_SUCCESS;
	}

	// Объединение вхождений в заданный словарь с вхождениями в его подсловари.

	// Количество вхождений в заданный словарь.
	unsigned int matches_count = aDictId < vIndexCeilSize.size() ? vIndexCeilSize[aDictId] : 0;

	// Загрузка заданного словаря из базы.
	InfDictWrapManipulator dict_manip;
	InfEngineErrors iee = vDLData->GetDictById( aDictId, dict_manip );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	// Проверка наличия подсловарей у заданного словаря.
	if( dict_manip.GetSubdictsNumber() )
	{
		// Подсчёт количества вхождений в подсловари.
		for( unsigned int subdict_n = 0; subdict_n < dict_manip.GetSubdictsNumber(); ++subdict_n )
		{
			unsigned int subdict_id = dict_manip.GetSubdictID( subdict_n );
			matches_count +=  subdict_id < vIndexCeilSize.size() ? vIndexCeilSize[subdict_id] : 0;
		}
	}

	if( !matches_count )
	{
		aDictMatch = DictMatch( 0, nullptr );

		return INF_ENGINE_SUCCESS;
	}

	// Выделение памяти под общий список вхождений.
	unsigned int begin = vDisclosedMatches.size();
	vDisclosedMatches.resize( begin + matches_count );
	if( vDisclosedMatches.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Копирование вхождений в заданный словарь.
	unsigned int pos = begin;
	if( aDictId < vIndex.size() )
	{
		for( unsigned int match_n = 0; match_n < vIndexCeilSize[aDictId]; ++match_n )
			vDisclosedMatches[pos++] = *( vIndex[aDictId][match_n] );
	}

	unsigned int lines_shift = dict_manip.GetSelfLinesNum();

	// Копирование вхождений в подсловари заданного словаря.
	for( unsigned int subdict_n = 0; subdict_n < dict_manip.GetSubdictsNumber(); ++subdict_n )
	{
		unsigned int subdict_id = dict_manip.GetSubdictID( subdict_n );

		if( subdict_id < vIndex.size() )
		{
			for( unsigned int match_n = 0; match_n < vIndexCeilSize[subdict_id]; ++match_n )
			{
				vDisclosedMatches[pos] = *( vIndex[subdict_id][match_n] );
				vDisclosedMatches[pos].vDictId = aDictId;
				vDisclosedMatches[pos].vLineId += lines_shift;
				++pos;
			}
		}

		// Загрузка подсловаря.
		InfDictWrapManipulator subdict_manip;
		if( INF_ENGINE_SUCCESS != ( iee = vDLData->GetDictById( subdict_id, subdict_manip ) ) )
			ReturnWithTrace( iee );

		// Поправка нумерации строк, учитывающая подсловари.
		lines_shift += subdict_manip.GetSelfLinesNum();
	}

	// Сортировка вхождений по левым позициям.
	std::sort( vDisclosedMatches.get_buffer() + begin, vDisclosedMatches.get_buffer() + vDisclosedMatches.size(), cmp_crossdict_matches );

	// Сохранение полученных результатов.
	if( aDictId >= vDisclosedIndex.size() )
	{
		unsigned int prev_size = vDisclosedIndex.size();
		vDisclosedIndex.resize( aDictId + 1 );
		vDisclosedIndexCeilSize.resize( aDictId + 1 );
		if( vDisclosedIndex.no_memory() || vDisclosedIndexCeilSize.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		memset( vDisclosedIndex.get_buffer() + prev_size, 0, vDisclosedIndex.size() - prev_size );
		memset( vDisclosedIndexCeilSize.get_buffer() + prev_size, 0, vDisclosedIndexCeilSize.size() - prev_size );
	}

	vDisclosedIndex[aDictId] = begin;
	vDisclosedIndexCeilSize[aDictId] = matches_count;

	aDictMatch = DictMatch( vDisclosedIndexCeilSize[aDictId], vDisclosedMatches.get_buffer() + vDisclosedIndex[aDictId] );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::DictsMatchingResults::GetDictInfo( unsigned int aDictId, const DictInfo* & aDictInfo ) const
{
	aDictInfo = nullptr;

	// Поиск результатов среди ранее полученных.
	if( aDictId < vDisclosedDictInfo.size() && vDisclosedDictInfo[aDictId].vDisclosed )
	{
		aDictInfo = vDisclosedDictInfo[aDictId].IsEmpty() ? nullptr : &vDisclosedDictInfo[aDictId];
		return INF_ENGINE_SUCCESS;
	}

	// Увеличение размера списка при неоходимости.
	if( vDisclosedDictInfo.size() <= aDictId )
	{
		unsigned int prev_size = vDisclosedDictInfo.size();
		vDisclosedDictInfo.resize( aDictId + 1 );
		if( vDisclosedDictInfo.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	// Копирование информации о самом словаре.
	if( aDictId < vDictInfo.size() )
		vDisclosedDictInfo[aDictId] = vDictInfo[aDictId];

	// Загрузка словаря из базы.
	InfDictWrapManipulator dict_manip;
	InfEngineErrors iee = vDLData->GetDictById( aDictId, dict_manip );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	// Проверка наличия у текущего словаря подсловарей.
	if( dict_manip.GetSubdictsNumber() )
	{
		DictInfo & dict_info = vDisclosedDictInfo[aDictId];

		// Добавление информации о подсловарях в информацию о словаре.
		for( unsigned int subdict_n = 0; subdict_n < dict_manip.GetSubdictsNumber(); ++subdict_n )
		{
			// Получение идентификатора подсловаря.
			unsigned int subdict_id = dict_manip.GetSubdictID( subdict_n );

			// Проверка наличия вхождений в подсловаре.
			if( subdict_id < vIndexCeilSize.size() && vIndexCeilSize[subdict_id] )
			{
				// Корректировка информации о текущем словаре на основе информации о его подсловаре.
				const DictInfo & subdict_info = vDictInfo[subdict_id];

				// Минимальная длина вхождения в словах.
				dict_info.vMinMatchLen = std::min<unsigned int>( dict_info.vMinMatchLen, subdict_info.vMinMatchLen );
				// Максимальная длина вхождения в словах.
				dict_info.vMaxMatchLen = std::max<unsigned int>( dict_info.vMaxMatchLen, subdict_info.vMaxMatchLen );

				// Самая левая позиция начала вхождения.
				dict_info.vMostLeftBeginPos = std::min<unsigned int>( dict_info.vMostLeftBeginPos, subdict_info.vMostLeftBeginPos );
				// Самая правая позиция начала вхождения
				dict_info.vMostRightBeginPos = std::max<unsigned int>( dict_info.vMostRightBeginPos, subdict_info.vMostRightBeginPos );

				// Самая левая позиция конца вхождения (номер последнего слова вхождения, а не следующего за ним).
				dict_info.vMostLeftEndPos = std::min<unsigned int>( dict_info.vMostLeftEndPos, subdict_info.vMostLeftEndPos );
				// Самая правая позиция конца вхождения (номер последнего слова вхождения, а не следующего за ним).
				dict_info.vMostRightEndPos = std::max<unsigned int>( dict_info.vMostRightEndPos, subdict_info.vMostRightEndPos );

				// Максимальный вес вхождения словаря.
				dict_info.vMaxWeight = std::max<unsigned int>( dict_info.vMaxWeight, subdict_info.vMaxWeight );
			}
		}
	}
	else
	{
		// У текущего словаря нет подсловарей. Уточнять суммарную информацию о нём не нужно.
		// do nothing
	}

	// Установка флага раскрытия.
	vDisclosedDictInfo[aDictId].vDisclosed = true;
	aDictInfo = vDisclosedDictInfo[aDictId].IsEmpty() ? nullptr : &vDisclosedDictInfo[aDictId];

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::DictsMatchingResults::BuildIndex( nMemoryAllocator & aMemoryAllocator )
{
	// Очистка контейнеров для хранения результатов выполнения запросов к методу GetMatch.
	vDisclosedMatches.clear();
	vDisclosedIndex.clear();
	vDisclosedIndexCeilSize.clear();
	vDisclosedDictInfo.clear();

	// Если не найдино ни одного вхождения
	if( !vDictMatches.size() )
	{
		Reset();
		return INF_ENGINE_SUCCESS;
	}

	// Сортировка найденных вхождений по номерам словарей, позициям и номерам строк.
	std::sort( vDictMatches.get_buffer(), vDictMatches.get_buffer() + vDictMatches.size(), cmp_dict_matches );

	// Вделение памяти для индекса.
	unsigned int max_dict_id = 0;
	for( unsigned int match_n = 0; match_n < vDictMatches.size(); ++match_n )
		max_dict_id = max_dict_id < vDictMatches[match_n].vDictId ? vDictMatches[match_n].vDictId : max_dict_id;

	vIndex.resize( max_dict_id + 1 );
	vIndexCeilSize.resize( max_dict_id + 1 );
	vDictInfo.clear();
	vDictInfo.resize( max_dict_id + 1 );
	if( vIndex.no_memory() || vIndexCeilSize.no_memory() || vDictInfo.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	memset( vIndex.get_buffer(), 0, vIndex.size() * sizeof( DictMatchInfo * * ) );
	memset( vIndexCeilSize.get_buffer(), 0, vIndexCeilSize.size() * sizeof( unsigned int ) );

	// Копирование данных в индекc.
	// Начало списка вхождений, относящихся к одному словарю.
	unsigned int begin = 0;
	// Конец списка вхождений, относящихся к одному словарю.
	unsigned int end = 0;
	while( begin < vDictMatches.size() )
	{
		// Определение границ списка вхождений, относящихся к одному словарю.
		end = begin + 1;
		while( end < vDictMatches.size() && vDictMatches[begin].vDictId == vDictMatches[end].vDictId )
			++end;

		// Идентификатор словаря, группа вхождений которого обрабатывается.
		unsigned int dict_id = vDictMatches[begin].vDictId;

		// Выделение памяти для списка вхождений.
		vIndexCeilSize[dict_id] = end - begin;
		vIndex[dict_id] = nAllocateObjects( aMemoryAllocator, DictMatchInfo*, vIndexCeilSize[dict_id] );
		if( !vIndex[dict_id] )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;

		// Суммарная информация о всех вхождениях данного словаря.
		DictInfo & dict_info = vDictInfo[dict_id];

		// Копирование списка вхождений.
		for( unsigned int match_n = begin; match_n < end; ++match_n )
		{
			// Информация о данном вхождении данного словаря.
			DictMatchInfo* & match_info = vIndex[dict_id][match_n - begin];
			match_info = &( vDictMatches[match_n] );

			// Вычисление самой левой позиции начала для вхождений данного словаря.
			dict_info.vMostLeftBeginPos = std::min<unsigned int>( dict_info.vMostLeftBeginPos, match_info->vBeginPos.GetPos() );
			// Вычисление самой правой позиции начала для вхождений данного словаря.
			dict_info.vMostRightBeginPos = std::max<unsigned int>( dict_info.vMostRightBeginPos, match_info->vBeginPos.GetPos() );

			// Вычисление самой левой позиции начала для вхождений данного словаря.
			dict_info.vMostLeftEndPos = std::min<unsigned int>( dict_info.vMostLeftEndPos, match_info->vEndPos.GetPos() );
			// Вычисление самой правой позиции начала для вхождений данного словаря.
			dict_info.vMostRightEndPos = std::max<unsigned int>( dict_info.vMostLeftEndPos, match_info->vEndPos.GetPos() );

			// Вычисление минимальной длины вхождения в словах.
			dict_info.vMinMatchLen = std::min<unsigned int>( dict_info.vMinMatchLen, match_info->vEndPos.GetPos() - match_info->vBeginPos.GetPos() + 1 );
			// Вычисление максимальной длины вхождения в словах.
			dict_info.vMaxMatchLen = match_info->vStars ? static_cast<unsigned int>( -1 ) : std::max<unsigned int>( dict_info.vMaxMatchLen, match_info->vEndPos.GetPos() - match_info->vBeginPos.GetPos() + 1 );

			// Вычисление наибольшего веса для вхождений данного словаря.
			dict_info.vMaxWeight = std::max( dict_info.vMaxWeight, match_info->vWeight );
			; //printf( "\nDBG: Index for dict #%i:  add entry %i\n", dict_id, vIndex[dict_id][match_n-begin] );
		}

		// Переход к следующей группе.
		begin = end;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::DictsMatchingResults::CreateByJoining( const DictsMatchingResults ** aResults, const DLDataRO ** aDLDataRO, unsigned int aCount,
																InfDictWrapManipulator::JoiningMethod aJoiningMethod,
																nMemoryAllocator & aMemoryAllocator )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Проверка аргументов.
	if( !aResults || !aDLDataRO )
		return INF_ENGINE_ERROR_INV_ARGS;

	// Очистка объетка.
	Reset();

	switch( aJoiningMethod )
	{
	case InfDictWrapManipulator::jmAddiction:
		// Вхождения из всех комплектов результатов последовательно объединяются.

		// Добавление вхождений из баз разных уровней.
		for( unsigned int res_n = 0; res_n < aCount; ++res_n )
		{
			// Добавление вхождений из очередного комплекта результатов.
			for( unsigned int match_n = 0; match_n < aResults[res_n]->vDictMatches.size(); ++match_n )
			{
				// Получаем очередное вхождение.
				BFinder::DictsMatchingResults::DictMatchInfo match_info = aResults[res_n]->vDictMatches[match_n];

				// Корректировка номер строки текущего вхождения.
				if( res_n )
				{
					// Получаем манипулятор на словарь с уровнем базы выше данного словаря.
					InfDictWrapManipulator dict_manip;
					if( INF_ENGINE_SUCCESS == ( iee = aDLDataRO[res_n - 1]->GetDictById( match_info.vDictId, dict_manip ) ) )
					{
						// Корректируем номер строки текущего вхождения.
						match_info.vLineId += dict_manip.GetNum();
					}
					else if( INF_ENGINE_WARN_UNSUCCESS == iee )
					{
						// Данный словарь уникален, номер строки корректировать не нужно.
					}
					else
						return iee;
				}

				// Добавляем текущее вхождение.
				vDictMatches.push_back( match_info );
				if( vDictMatches.no_memory() )
					return INF_ENGINE_ERROR_NOFREE_MEMORY;
			}
		}
		break;

	case InfDictWrapManipulator::jmSubstitution:
		{
			static avector<unsigned int> dict_is_joined;
			dict_is_joined.clear();

			// Добавление вхождений из баз разных уровней.
			for( int res_n = aCount - 1; res_n >= 0; --res_n )
			{
				// Добавление вхождений из очередного комплекта результатов.
				for( unsigned int match_n = 0; match_n < aResults[res_n]->vDictMatches.size(); ++match_n )
				{
					// Получаем очередное вхождение.
					BFinder::DictsMatchingResults::DictMatchInfo match_info = aResults[res_n]->vDictMatches[match_n];

					// Првоеряем, не был ли словарь с таким идентификатором добавлен ранее.
					if( match_info.vDictId < dict_is_joined.size() &&
						( dict_is_joined[match_info.vDictId] != static_cast<unsigned int>( -1 ) && dict_is_joined[match_info.vDictId] > res_n ) )
					{
						// Словарь с таким идентификатором уже бы добавлен ранее и вытесняет все остальные.
						continue;
					}
					else
					{
						// Првоеряем, не был ли словарь с таким идентификатором переопеделён в пользовательской базе.
						bool defined = false;
						int lvl_n = res_n + 1;
						for( int lvl_n = res_n + 1; lvl_n < aCount; ++lvl_n )
						{
							InfDictWrapManipulator dict_manip;
							if( aDLDataRO[lvl_n]->IsDictExisted( match_info.vDictId ) )
							{
								defined = true;
								break;
							}
						}

						// Делаем пометку, что словарь с таким идентификатором был добавлен.
						if( dict_is_joined.size() <= match_info.vDictId )
						{
							unsigned int last_size = dict_is_joined.size();
							dict_is_joined.resize( match_info.vDictId + 1 );
							if( dict_is_joined.no_memory() )
								return INF_ENGINE_ERROR_NOFREE_MEMORY;
							memset( dict_is_joined.get_buffer() + last_size, 0xff, sizeof( unsigned int ) * ( dict_is_joined.size() - last_size ) );
						}
						dict_is_joined[match_info.vDictId] = defined ? lvl_n : res_n;

						// Добавляем текущее вхождение.
						if( !defined )
						{
							vDictMatches.push_back( match_info );
							if( vDictMatches.no_memory() )
								return INF_ENGINE_ERROR_NOFREE_MEMORY;
						}
					}
				}
			}
			break;
		}

	default:
		return INF_ENGINE_ERROR_INV_DATA;
	}

	// Индексация результатов поиска.
	return BuildIndex( aMemoryAllocator );
}

unsigned int BFinder::DictsMatchingResults::GetStringId( unsigned int aDictId, unsigned int aLineId ) const
{
	// Загрузка текущей строки словаря из базы.
	InfDictWrapManipulator dict_wrap_manip;
	vDLData->GetDictById( aDictId, dict_wrap_manip );
	InfDictStringManipulator dict_string_manip;
	dict_wrap_manip.GetString( aLineId, dict_string_manip, false );
	return dict_string_manip.GetId();
}

InfEngineErrors BFinder::DictsMatchingResults::CopyFrom( const MatchNList & aPos, unsigned int aDictId, unsigned int aLineId, unsigned int & debug_cnt, unsigned int aCondWeight )
{
	debug_cnt = 0;
	for( MatchNList::const_iterator match_n = aPos.begin(); match_n != aPos.end(); ++match_n )
	{
		// Добавление ранее найденного вхождения и установка в нём координатов нового словаря.
        DictMatchInfo * info = vDictMatches.grow();
        if( !info )
            return INF_ENGINE_ERROR_NOFREE_MEMORY;
        *info = vDictMatches[*match_n];
		vDictMatches.back().vDictId = aDictId;
		vDictMatches.back().vLineId = aLineId;
		++debug_cnt;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::DictsMatchingResults::AddMatch( unsigned int aDictId, unsigned int aLineId,
														 NDocImage::Iterator aBeginPos, NDocImage::Iterator aEndPos,
														 unsigned int aStarBefore, unsigned int aStarAfter, bool aStars, unsigned int aWeight )
{
	// Получение идентификатора строки словаря, выданного хранилищем шаблонов.
	unsigned int aId = GetStringId( aDictId, aLineId );

	// Добавление информации о новом найденном вхождении.
	DictMatchInfo * dict_match_info = vDictMatches.grow();
	if( !dict_match_info )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Копирование идентификатора словаря.
	dict_match_info->vDictId = aDictId;

	// Копирование номера строки словаря.
	dict_match_info->vLineId = aLineId;

	// Копирование идентификатор, выданного хранилищем шаблонов.
	dict_match_info->vId = aId;

	// Копирование итератора, указывающего на начало элемента.
	dict_match_info->vBeginPos  = aBeginPos;

	// Копирование итератора, указывающего на конец элемента.
	dict_match_info->vEndPos = aEndPos;

	// Копирование флага наличия звёздочек или суперзвёздочек в начале строки.
	dict_match_info->vStarBefore = aStarBefore;

	// Копирование флага наличия звёздочек или суперзвёздочек в конце строки.
	dict_match_info->vStarAfter = aStarAfter;

	// Копирование флага наличия звёздочек или суперзвёздочек.
	dict_match_info->vStars = aStars;

	// Копирование веса совпадения.
	dict_match_info->vWeight = aWeight;

	return AddToCache( aId, vDictMatches.size() - 1, aDictId, aLineId );
}

void BFinder::DictsMatchingResults::Reset()
{
	CachedMatchingResults::Reset();

	vDictMatches.clear();
	vIndex.clear();
	vIndexCeilSize.clear();

	vDictInfo.clear();

	vDisclosedMatches.clear();
	vDisclosedIndex.clear();
	vDisclosedIndexCeilSize.clear();
	vDisclosedDictInfo.clear();
}


InfEngineErrors BFinder::QuestionMatchingResults::CopyFrom( const MatchNList & aPos, unsigned int aPatternId, unsigned int aQuestionId, unsigned int & debug_cnt, unsigned int aCondWeight )
{
	debug_cnt = 0;
	for( MatchNList::const_iterator cand_n = aPos.begin(); cand_n != aPos.end(); ++cand_n )	
	{
		// Создание копии кандидата.
		Candidat * copy_cand = nAllocateObject( vAllocator, Candidat );
		new( (nMemoryAllocatorElementPtr*)copy_cand )Candidat;
		*copy_cand = *vCandidats[*cand_n];

		// Привязка копии к другому шаблон-вопросу.
		copy_cand->vPatternId = aPatternId;
		copy_cand->vQuestionId = aQuestionId;
		copy_cand->vWeight = copy_cand->vSelfWeight | aCondWeight | GetQuestionWeight( aPatternId, aQuestionId );

		// Добавление ранее найденного вхождения и установка в нём коорлинатов нового словаря.
		vCandidats.push_back( copy_cand );
		if( vCandidats.no_memory() )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;

		++debug_cnt;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::QuestionMatchingResults::BuildIndex()
{
	if( vCandidats.size() != 0 )
	{
		// Сортировка результатов по весу.
		vCandidats.qsort( BFinderCandidatCmpById );

		// Удаление повторных шаблонов с меньшим весом.
		for( unsigned int i = 1; i < vCandidats.size(); i++ )
		{
			if( vCandidats[i]->vPatternId == vCandidats[i - 1]->vPatternId )
			{
				vCandidats.remove( i );
				i--;
			}
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::QuestionMatchingResults::SortByWeight()
{
	// Сортировка результатов по весу.
	vCandidats.qsort( InfBFinderCandidatesCmpByWeigth );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::QuestionMatchingResults::AddCandidat( Candidat * aCandidat )
{
	// Получение идентификатора строки словаря, выданного хранилищем шаблонов.
	unsigned int aId = GetStringId( aCandidat->vPatternId, aCandidat->vQuestionId );

	// Добавление очередного кандидата в список.
	vCandidats.push_back( aCandidat );
	if( vCandidats.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Кэширование.
	return AddToCache( aId, vCandidats.size() - 1, aCandidat->vPatternId, aCandidat->vQuestionId );
}


void BFinder::Reset()
{
	vRuntimeAllocator.Reset();

	vDictMatcher.Reset();
	vPatternMatcher.Reset();

	vDocMessage.ResetAll();
	vDocImage.Reset();

	vResults.Reset();
	vAnswers.clear();

	vConditionsCheckCache.clear();

	vDictsMatchingResults.Reset();
}

InfEngineErrors BFinder::MatchPattern( InfPatternItems::QuestionRO & aQuestion, const PatternMatcher & aPatternMatcher, const MatchedTerm * aMatchedTerms,
									   unsigned int aTermsNumber, const DictsMatchingResults & aDictsMatchingResults, avector<Candidat*> & aCandidats, unsigned int aCondWeight, bool & aIsBestMatch )
{
	aCandidats.clear();
	aIsBestMatch = false;
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Проверка аргументов.
	if( !aMatchedTerms || !aTermsNumber )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Выделение памяти под подготавливаемого кандидата.
	static avector<Candidat::Item> vCandidat;
	vCandidat.clear();

	vCandidat.resize( aQuestion.GetItemsNum() );
	if( vCandidat.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Состояния сопоставления словарей.
	static avector<unsigned int> dict_states;
	static avector<DictsMatchingResults::DictMatch> dict_matches;
	static avector<bool> dict_has_empty_variant;

	dict_states.resize( aQuestion.GetItemsNum() );
	if( dict_states.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	dict_matches.resize( aQuestion.GetItemsNum() );
	if( dict_matches.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	dict_has_empty_variant.resize( aQuestion.GetItemsNum() );
	if( dict_has_empty_variant.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Инициализация состояний совпадения словарей
	unsigned int max_summary_dicts_weight = 0;
	for( unsigned int item_n = 0; item_n < aQuestion.GetItemsNum(); ++item_n )
	{
		if( aQuestion.GetItemType( item_n ) == InfPatternItems::itDict )
		{
			InfPatternItems::TagDictManipulator dict( aQuestion.GetItem( item_n ) );

			// Проверка наличия вхождений у данного словаря.
			if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetMatch( dict.GetDictID(), dict_matches[item_n] ) ) )
				ReturnWithTrace( iee );

			if( dict_matches[item_n].GetMatchesNumber() )
			{
				dict_states[item_n] = 0;
				dict_has_empty_variant[item_n] = dict.HasEmptyVariant();
				max_summary_dicts_weight += InfPatternItems::DT_LCDICT == dict.GetDictType() ? ( aDictsMatchingResults.GetMaxWeight( dict.GetDictID() ) + 1 ) >> 1 : aDictsMatchingResults.GetMaxWeight( dict.GetDictID() );
			}
			else if( dict.HasEmptyVariant() )
			{
				dict_states[item_n] = 0;
				dict_has_empty_variant[item_n] = true;
			}
			else
				// Необходимый словарь не подошёл.
				return INF_ENGINE_WARN_UNSUCCESS;
		}
		else
		{
			dict_states[item_n] = static_cast<unsigned int>( -1 );
			dict_has_empty_variant[item_n] = false;
		}
	}

	// Элемент входной фразы, до которого сопоставление прошло успешно.
	NDocImage::Iterator iter = aPatternMatcher.GetNDocImage().First();
	++iter;

	// Номер текущего сопоставленного на предварительном этапе обязательного термина.
	unsigned int cur_term_n = 0;
	// Номер текущего сопоставляемого элемента шаблон-вопроса.
	unsigned int item_n = 0;
	// Указатель на элемент, для которого ещё не определён конец вхождения.
	Candidat::Item * waiting_for_end = nullptr;

	// Сопоставление шаблон-вопроса.
	bool step_back = false;
	while( item_n < aQuestion.GetItemsNum() )
	{		
		switch( aQuestion.GetItemType( item_n ) )
		{
		case InfPatternItems::itText:
			{
				InfPatternItems::Text text( aQuestion.GetItem( item_n ) );

				// Игнорирование стоп-символов в шаблон-вопросе.
				if( text.IfStopWordsOnly() )
				{
					// Сохраняем в результате тип успешно сопоставленного элемента шаблон-вопроса.
					vCandidat[item_n].vType = Candidat::Item::InlineDict;
					// Сохраняем в результате начало и конец совпадения элемента в виде пустого отрезка.
					vCandidat[item_n].vBeginPos = aPatternMatcher.GetNDocImage().Last();
					vCandidat[item_n].vEndPos = aPatternMatcher.GetNDocImage().Last();
					--vCandidat[item_n].vEndPos;
					vCandidat[item_n].vMostLeftNextBegin = iter;

					// Переход к следующему элементу шаблон-вопроса.
					break;
				}

				if( item_n != aMatchedTerms[cur_term_n].vAttrs->GetPos() )
				{
					// Обязательный термин по какой-то причине не был сопоставлен на предварительном этапе.
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
				}

				// Проверка начала элемента.
				if( iter > aMatchedTerms[cur_term_n].vEntry->vCoords.vFirstPos )
				{
					// Не удаётся сопоставить.
					step_back = true;
					vCandidat[item_n].Reset();
				}
				// Проверка звёздочек перед элементом.
				else if( waiting_for_end )
				{
					// Установка конца предыдущей звёздочки.
					waiting_for_end->vEndPos = aMatchedTerms[cur_term_n].vEntry->vCoords.vFirstPos;
					--( waiting_for_end->vEndPos );

					vCandidat[item_n].vWaitingForEnd = waiting_for_end;
					waiting_for_end = nullptr;
				}
				else if( iter != aMatchedTerms[cur_term_n].vEntry->vCoords.vFirstPos )
				{
					// Не удаётся сопоставить.
					step_back = true;
					vCandidat[item_n].Reset();
				}

				// Если шаблон-вопрос заканчивается обязательным элементом.
				if( item_n == aQuestion.GetItemsNum() - 1 )
				{
					// Конец последнего обязательного элемента не совпадает с концом входной фразы.
					if( aPatternMatcher.GetNDocImage().Last().GetPos() - aMatchedTerms[cur_term_n].vEntry->vCoords.vLastPos.GetPos() != 1 )
					{
						// Не удаётся сопоставить.
						step_back = true;
						vCandidat[item_n].Reset();
					}
				}

				if( !step_back )
				{
					// Сохраняем в результате тип успешно сопоставленного элемента шаблон-вопроса.
					vCandidat[item_n].vType =  Candidat::Item::Text;
					// Сохраняем в результате начало и конец совпадения элемента.
					vCandidat[item_n].vBeginPos = aMatchedTerms[cur_term_n].vEntry->vCoords.vFirstPos;
					vCandidat[item_n].vEndPos = aMatchedTerms[cur_term_n].vEntry->vCoords.vLastPos;

					// Переходим к следующему сопоставленному на предварительном этапе обязательному термину.
					++cur_term_n;

					// Продвигаемся вперёд по входной фразе.
					iter = vCandidat[item_n].vEndPos;
					++iter;
					vCandidat[item_n].vMostLeftNextBegin = iter;
				}

				break;
			}

		case InfPatternItems::itDictInline:
			{
				// Пустой инлайн-словарь без совпадения
				InfPatternItems::TagDictInlineManipulator inline_dict( aQuestion.GetItem( item_n ) );

				// Если данный инлай-словарь не был сопоставлен на предварительном этапе.
				if( cur_term_n >= aTermsNumber || item_n != aMatchedTerms[cur_term_n].vAttrs->GetPos() )
				{
					if( !inline_dict.HasEmpty() )
					{
						// Обязательный термин по какой-то причине не был сопоставлен на предварительном этапе.
						ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
					}

					// Сохраняем в результете тип успешно сопоставленного элемента шаблон-вопроса.
					vCandidat[item_n].vType = Candidat::Item::InlineDict;
					// Сохраняем в результате начало и конец совпадения элемента в виде пустого отрезка.
					vCandidat[item_n].vBeginPos = aPatternMatcher.GetNDocImage().Last();
					vCandidat[item_n].vEndPos = aPatternMatcher.GetNDocImage().Last();
					--vCandidat[item_n].vEndPos;
					vCandidat[item_n].vMostLeftNextBegin = iter;
					break;
				}

				// Проверка начала элемента.
				if( iter > aMatchedTerms[cur_term_n].vEntry->vCoords.vFirstPos )
				{
					// Не удаётся сопоставить.
					step_back = true;
					vCandidat[item_n].Reset();
				}
				// Проверка звёздочек перед элементом.
				else if( waiting_for_end )
				{
					// Установка конца предыдущей звёздочки.
					waiting_for_end->vEndPos = aMatchedTerms[cur_term_n].vEntry->vCoords.vFirstPos;
					--( waiting_for_end->vEndPos );
					vCandidat[item_n].vWaitingForEnd = waiting_for_end;
					waiting_for_end = nullptr;
				}
				else if( iter != aMatchedTerms[cur_term_n].vEntry->vCoords.vFirstPos )
				{
					// Не удаётся сопоставить.
					step_back = true;
					vCandidat[item_n].Reset();
				}

				// Если шаблон-вопрос заканчивается обязательным элементом.
				if( item_n == aQuestion.GetItemsNum() - 1 )
				{
					// Конец последнего обязательного элемента не совпадает с концом входной фразы.
					if( aPatternMatcher.GetNDocImage().Last().GetPos() - aMatchedTerms[cur_term_n].vEntry->vCoords.vLastPos.GetPos() != 1 )
					{
						// Не удаётся сопоставить.
						step_back = true;
						vCandidat[item_n].Reset();
					}
				}

				if( !step_back )
				{

					// Сохраняем в результате тип успешно сопоставленного элемента шаблон-вопроса.
					vCandidat[item_n].vType = Candidat::Item::InlineDict;
					// Сохраняем в результате начало и конец совпадения элемента.
					vCandidat[item_n].vBeginPos = aMatchedTerms[cur_term_n].vEntry->vCoords.vFirstPos;
					vCandidat[item_n].vEndPos = aMatchedTerms[cur_term_n].vEntry->vCoords.vLastPos;					

					// Переходим к следующему сопоставленному на предварительном этапе обязательному термину.
					++cur_term_n;

					// Продвигаемся вперёд по входной фразе.
					iter = vCandidat[item_n].vEndPos;
					++iter;
					vCandidat[item_n].vMostLeftNextBegin = iter;
				}

				break;
			}

		case InfPatternItems::itStar:
		case InfPatternItems::itEliStar:
			{
				// Установка конца предыдущей звёздочки.
				if( waiting_for_end )
				{
					waiting_for_end->vEndPos = iter;
					--( waiting_for_end->vEndPos );

					vCandidat[item_n].vWaitingForEnd = waiting_for_end;
					waiting_for_end = nullptr;
				}

				InfPatternItems::Star star( aQuestion.GetItem( item_n ) );
				// Для сопоставления суперзвёздочки необходимо хотя бы одно слово.
				if( star.IsSuperStar() && aPatternMatcher.GetNDocImage().Last().GetPos() - iter.GetPos() < 1 )
				{
					// Не удаётся сопоставить.
					step_back = true;
					vCandidat[item_n].Reset();
					break;
				}

				// Сохраняем в результате тип успешно сопоставленного элемента шаблон-вопроса.
				vCandidat[item_n].vType = star.IsSuperStar() ? Candidat::Item::SuperStar : Candidat::Item::Star;
				// Сохраняем в результате начало совпадения элемента.
				// Конец совпадения будет сохранён при сопоставлении следующего элемента.
				vCandidat[item_n].vBeginPos = iter;

				// Запоминаем элемент, требующий определения конца вхождения.
				waiting_for_end = &( vCandidat[item_n] );
				vCandidat[item_n].vStarAfter = true;

				// Делаем поправку в текущем положении
				if( star.IsSuperStar() )
					++iter;
				vCandidat[item_n].vMostLeftNextBegin = iter;
				break;
			}

		case InfPatternItems::itDict:
			{
				// Сопоставление пустого варианта.
				if( dict_states[item_n] == dict_matches[item_n].GetMatchesNumber() )
				{
					// Сохраняем в результете тип успешно сопоставленного элемента шаблон-вопроса.
					vCandidat[item_n].vType = Candidat::Item::InlineDict;
					// Сохраняем в результате начало и конец совпадения элемента в виде пустого отрезка.
					vCandidat[item_n].vBeginPos = aPatternMatcher.GetNDocImage().Last();
					vCandidat[item_n].vEndPos = aPatternMatcher.GetNDocImage().Last();
					--vCandidat[item_n].vEndPos;
					vCandidat[item_n].vMostLeftNextBegin = iter;
					break;
				}

				const DictsMatchingResults::DictMatchInfo * dict_match_info = dict_matches[item_n].GetMatch( dict_states[item_n] );

				// Проверка начала элемента.
				if( iter > dict_match_info->vBeginPos )
				{
					// Не удаётся сопоставить.
					step_back = true;
					vCandidat[item_n].vWaitingForEnd = waiting_for_end;
					break;
				}

				// Установка конца предыдущей звёздочки.
				else if( waiting_for_end )
				{
					// Установка конца предыдущей звёздочки.
					waiting_for_end->vEndPos = dict_match_info->vBeginPos;
					--( waiting_for_end->vEndPos );
					vCandidat[item_n].vWaitingForEnd = waiting_for_end;
					waiting_for_end = nullptr;
				}
				else if( iter != dict_match_info->vBeginPos && !dict_match_info->vStarBefore )
				{
					// Не удаётся сопоставить.
					step_back = true;					
					vCandidat[item_n].Reset();
				}

				// Если шаблон-вопрос заканчивается словарём.
				if( item_n == aQuestion.GetItemsNum() - 1 )
				{
					// Конец последнего элемента не совпадает с концом входной фразы.
					if( !dict_match_info->vStarAfter && aPatternMatcher.GetNDocImage().Last().GetPos() - dict_match_info->vEndPos.GetPos() != 1 )
					{
						// Не удаётся сопоставить.
						step_back = true;
						vCandidat[item_n].Reset();
					}
				}

				if( !step_back )
				{
					// Загружаем тэг, для проверки типа словаря.
					InfPatternItems::TagDictManipulator dict_manip( aQuestion.GetItem( item_n ) );

					// Сохраняем в результате тип успешно сопоставленного элемента шаблон-вопроса.
					vCandidat[item_n].vType = dict_manip.GetDictType() == InfPatternItems::DT_LCDICT ? Candidat::Item::LCDict : Candidat::Item::Dict;
					// Сохраняем в результате начало и конец совпадения элемента.
					if( !dict_match_info->vStarBefore )
						vCandidat[item_n].vBeginPos = dict_match_info->vBeginPos;
					else
					{
						if( item_n && vCandidat[item_n].vWaitingForEnd )
							vCandidat[item_n].vBeginPos = dict_match_info->vBeginPos;
						else
							vCandidat[item_n].vBeginPos = iter;
					}
					vCandidat[item_n].vEndPos = dict_match_info->vEndPos;					

					// Сохраняем результате идентификатор словаря и номер строки.
					vCandidat[item_n].vDictId = dict_match_info->vDictId;
					vCandidat[item_n].vLineId = dict_match_info->vLineId;
					vCandidat[item_n].vWeight = dict_match_info->vWeight;

					vCandidat[item_n].vStars = dict_match_info->vStars;
					vCandidat[item_n].vStarStart = dict_match_info->vStarBefore;
					vCandidat[item_n].vStarFinish = dict_match_info->vStarAfter;

					// Запоминаем элемент, требующий определения конца вхождения.
					if( dict_match_info->vStarAfter )
					{
						waiting_for_end = &( vCandidat[item_n] );
						vCandidat[item_n].vStarAfter = true;
					}

					// Продвигаемся вперёд по входной фразе.
					iter = vCandidat[item_n].vEndPos;
					++iter;
					vCandidat[item_n].vMostLeftNextBegin = iter;
				}

				break;
			}

		default:
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}

		// Все элементы шаблон-вопроса сопоставлены.
		if( !step_back && item_n + 1 == aQuestion.GetItemsNum() )
		{
			// Установка конца последней звёздочки.
			if( waiting_for_end )
			{
				// Установка конца предыдущей звёздочки.
				waiting_for_end->vEndPos = aPatternMatcher.GetNDocImage().Last();
				--( waiting_for_end->vEndPos );
			}

			// Была ли входная фраза сопоставлена до самого конца.
			if( waiting_for_end || aPatternMatcher.GetNDocImage().Last().GetPos() <= iter.GetPos() )
			{
				// Выделение памяти для кандидата.
				Candidat * candidat = nAllocateObject( vRuntimeAllocator, Candidat );
				if( !candidat )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				new( (nMemoryAllocatorElementPtr*)candidat )Candidat;

				// Заполнение информации о кандидате.
				// Идентификатор шаблона.
				candidat->vPatternId = aMatchedTerms[0].vAttrs->GetId();
				// Номер шаблон-вопроса.
				candidat->vQuestionId = aMatchedTerms[0].vAttrs->GetSubId();
				// Элементы кандидата.

				candidat->vItems = nAllocateObjects( vRuntimeAllocator, Candidat::Item, vCandidat.size() );
				if( !candidat->vItems )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				for( unsigned int itm_n = 0; itm_n < vCandidat.size(); ++itm_n )
					candidat->vItems[itm_n] = vCandidat[itm_n];
				// Количество элементов в кандидате.
				candidat->vItemsNumber = vCandidat.size();

				// Вес кандидата.
				// TODO: вычисление веса.
				unsigned int terms_weight = 0;
				unsigned int dicts_weight = 0;
				candidat->vSelfWeight = CalculateCandidatWeight( candidat, terms_weight, dicts_weight );
				candidat->vWeight = candidat->vSelfWeight |  aCondWeight | aQuestion.GetWeight();
				aCandidats.push_back( candidat );
				if( aCandidats.no_memory() )
					ReturnWithError( INF_ENGINE_ERROR_CANT_CONNECT, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				// Данное совпадение имеет максимально возможный вес для данного шаблон-вопроса.
				if( terms_weight + dicts_weight == aQuestion.GetTermsWeight() + max_summary_dicts_weight )
					aIsBestMatch = true;
			}
			else
			{
				// Сопоставление завершилось неудачей.
			}

			step_back = true;
		}

		// Откатываемся, используя ещё не рассмотренные варианты сопоставления словарей.
		if( step_back )						
		{
			waiting_for_end = nullptr;
			// Поиск нерассмотренных вариантов вхождения словарей.
			while( item_n )
			{
				// Если текущий элемент является словарём.
				if( dict_states[item_n] != static_cast<unsigned int>( -1 ) )
				{
					// Если у текущего словаря ещё остались нерасмотренные варианты.
					if( ( dict_states[item_n] + 1 < dict_matches[item_n].GetMatchesNumber() ) ||
						( dict_has_empty_variant[item_n] && dict_states[item_n] < dict_matches[item_n].GetMatchesNumber() ) )
					{
						// Преходим к рассмотрению следующего  варианта вхождения словаря.
						++dict_states[item_n];
						waiting_for_end = vCandidat[item_n].vWaitingForEnd;
						break;
					}
					// В словаре закончились состояния, сбрасываем счётчик вариантов для него.
					dict_states[item_n] = 0;
				}
				else
				{
					// Если текущий элемент был сопоставлен с реальным термином.
					if( cur_term_n && item_n == aMatchedTerms[cur_term_n - 1].vAttrs->GetPos() )
					{
						// Уменьшаем счётчик сопоставленных реальных терминов.
						--cur_term_n;
					}
				}

				// Откатываемся на один элемент назад.
				vCandidat[item_n].Reset();
				--item_n;

				// Корректируем положение конца рассмотренной части входной фразы.
				if( item_n )
					iter = vCandidat[item_n-1].vMostLeftNextBegin;
				else				
					iter = vCandidat[item_n].vBeginPos;
			}

			// Откатились до первого элемента.
			if( !item_n )
			{
				// Если текущий элемент является словарём.
				if( dict_states[item_n] != static_cast<unsigned int>( -1 ) )
				{
					// Если у текущего словаря ещё остались нерасмотренные варианты.
					if( ( dict_states[item_n] + 1 < dict_matches[item_n].GetMatchesNumber() ) ||
						( dict_has_empty_variant[item_n] && dict_states[item_n] < dict_matches[item_n].GetMatchesNumber() ) )
					{
						// Преходим к рассмотрению следующего варианта вхождения словаря.
						++dict_states[item_n];
						// Начинаем с начала входной фразы.
						iter = aPatternMatcher.GetNDocImage().First();
						++iter;
					}
					else
						// Все возможные вариатны сопоставления были рассмотрены.
						break;
				}
				else
					// Все возможные вариатны сопоставления были рассмотрены.
					break;
			}
			step_back = false;
		}
		else
			// Переход к следующему элементу шаблон-вопроса.
			++item_n;
	}

	// Сопоставление завершилось неудачей.
	if( !aCandidats.size() )
		return INF_ENGINE_WARN_UNSUCCESS;

	return INF_ENGINE_SUCCESS;
}

unsigned int BFinder::CalculateCandidatWeight( BFinder::Candidat* & aCandidat, unsigned int & aTermsWeight, unsigned int  & aDictsWeight )
{
	aTermsWeight = 0;
	aDictsWeight = 0;
	unsigned int weight = 0;
	bool star = false;

	for( unsigned int item_n = 0; item_n < aCandidat->vItemsNumber; ++item_n )
	{
		switch( aCandidat->vItems[item_n].vType )
		{
		case Candidat::Item::Text:
			weight += 2 * ( aCandidat->vItems[item_n].vEndPos.GetPos() - aCandidat->vItems[item_n].vBeginPos.GetPos() + 1 );
			aTermsWeight += 2 * ( aCandidat->vItems[item_n].vEndPos.GetPos() - aCandidat->vItems[item_n].vBeginPos.GetPos() + 1 );
			break;

		case Candidat::Item::InlineDict:
			if( aCandidat->vItems[item_n].vEndPos >= aCandidat->vItems[item_n].vBeginPos )
			{
				weight += 2 * ( aCandidat->vItems[item_n].vEndPos.GetPos() - aCandidat->vItems[item_n].vBeginPos.GetPos() + 1 );
				aTermsWeight += 2 * ( aCandidat->vItems[item_n].vEndPos.GetPos() - aCandidat->vItems[item_n].vBeginPos.GetPos() + 1 );
			}
			break;

		case Candidat::Item::Dict:
			weight += aCandidat->vItems[item_n].vWeight;
			star |= aCandidat->vItems[item_n].vStars;
			aDictsWeight += aCandidat->vItems[item_n].vWeight;
			break;

		case Candidat::Item::LCDict:
			weight += ( aCandidat->vItems[item_n].vWeight + 1 ) >> 1;
			star |= aCandidat->vItems[item_n].vStars;
			aDictsWeight += ( aCandidat->vItems[item_n].vWeight + 1 ) >> 1;
			break;

		case Candidat::Item::Star:
			star = true;
			break;
		case Candidat::Item::SuperStar:
			weight += 1;
			star = true;
			break;
		}
	}

	weight =  ( star ? 0 : 1 << 30 ) |  ( ( weight & 0xFFFF ) << 14 );

	return weight;
}

InfEngineErrors BFinder::ProcessPatternTerms( const DLDataRO & aPatternsData, const InfConditionsRegistry & aConditionsRegistry,
											  const Session & aSession, PatternMatcher & aPatternMatcher,
											  const DictsMatchingResults & aDictsMatchingResults, bool aQstFlag,
											  RequestStat & aRequestStat, bool aUserBase )
{
	vTSIterators.clear();
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Подготовка итераторов для поиска совпадений.
	vTSIterators.resize( aPatternMatcher.GetTermsNum() );
	if( vTSIterators.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Инициализация итераторов.
	bool variants_exists = false;
	unsigned int first_term_n = 0;
	for( unsigned int term_n = 0; term_n < vTSIterators.size(); ++term_n )
	{
		// Проверка наличия атрибутов в базе.
		if( INF_ENGINE_SUCCESS != ( iee = aPatternMatcher.SearchAttrsByN( term_n, vTSIterators[term_n] ) ) )
		{
			if( INF_ENGINE_WARN_UNSUCCESS )
				return INF_ENGINE_SUCCESS;
			else
				ReturnWithTrace( iee );
		}

		if( !variants_exists )
		{
			while( vTSIterators[term_n].Item() && !vTSIterators[term_n].Item()->GetFlagFirstTerm() )
				vTSIterators[term_n].Next();
			variants_exists |= !vTSIterators[term_n].IsEnd() && vTSIterators[term_n].Item()->GetFlagFirstTerm();
		}

		// Если для данного термина атрибутов не нашлось, то считаем первым термином следующий.
		if( !variants_exists )
		{
			vTSIterators[term_n].SetEnd();
			++first_term_n;
		}

		if( vTSIterators[term_n].Item() )
		{
			TermsStorageRO::PersonIdIterator tmp;
			aPatternMatcher.SearchAttrsByN( term_n, tmp );
		}
	}

	// Флаг отката.
	bool step_back = false;

	// Последний удачно сопоставленный термин.
	unsigned int last_term_n = static_cast<unsigned int>( -1 );
	// Текущий проверяемый на совпадение термин.
	unsigned int cur_term_n = first_term_n + 1;

	// Список сопоставленных терминов.
	static avector<MatchedTerm> matched_terms;

	// Текущий шаблон.
	InfPatternItems::QuestionRO question;

	// Поправка веса шаблона при проверке условий шаблона.
	unsigned int cond_weight = 0;

	// Идентификатор шаблона и шаблон-вопроса, которые не нужно обрабатывать
	unsigned int skip_id = static_cast<unsigned int>( -1 );
	unsigned int skip_sub_id = static_cast<unsigned int>( -1 );

	// Номер последнего загруженного шаблона.
	unsigned int prev_pattern_id = static_cast<unsigned int>( -1 );
	// Номер последнего загруженного шаблон-вопроса.
	unsigned int prev_question_id = static_cast<unsigned int>( -1 );

	// Номер последнего загруженного шаблона, прошедшего проверку всех эвристик.
	unsigned int prev_deep_pattern_id = static_cast<unsigned int>( -1 );
	// Номер последнего загруженного шаблон-вопроса, прошедшего проверку всех эвристик.
	unsigned int prev_deep_question_id = static_cast<unsigned int>( -1 );

	// Номер последнего подошедшего шаблона.
	unsigned int prev_acc_pattern_id = static_cast<unsigned int>( -1 );
	// Номер последнего подошедшего шаблон-вопроса.
	unsigned int prev_acc_question_id = static_cast<unsigned int>( -1 );

	unsigned int copy_match_cnt = 0;
	unsigned int add_match_cnt = 0;

	vResults.SetDLData( &aPatternsData );

	// Отбор возможных совпадений.
	while( variants_exists )
	{
		// Если ещё ни одного термина не сопоставлено.
		while( static_cast<unsigned int>( -1 ) == last_term_n && first_term_n < vTSIterators.size() )
		{
			// Поиск первого вхождения.
			const TermsStorage::TermAttrsRO * first_attrs = vTSIterators[first_term_n].Item();
			// Для данного вхождения кончились нерасмотренные атрибуты.
			if( !first_attrs )
			{
				// Переходим к следующему вхождению.
				++first_term_n;
				// Больше вхождений нет, все варианты были рассмотрены.
				if( first_term_n >= vTSIterators.size() )
					variants_exists = false;
				continue;
			}

			// Подсчёт количества обработанных на поиске шаблон-вопросов.
			if( prev_pattern_id != vTSIterators[first_term_n].Item()->GetId() || prev_question_id != vTSIterators[first_term_n].Item()->GetSubId() )
			{
				++aRequestStat.vQst;
				if( aUserBase )
					++aRequestStat.ex.counter_request_userbase_patterns_all_index;
				else
					++aRequestStat.ex.counter_request_mainbase_patterns_all_index;

				prev_pattern_id = vTSIterators[first_term_n].Item()->GetId();
				prev_question_id = vTSIterators[first_term_n].Item()->GetSubId();
			}

			if( aQstFlag )
			{
				// Проверка условий шаблона.
				if( INF_ENGINE_SUCCESS != ( iee = CheckConditions( aPatternsData, aConditionsRegistry, first_attrs->GetId(), aSession, cond_weight ) ) )
				{
					if( INF_ENGINE_WARN_UNSUCCESS == iee )
					{
						// Переход к следующим атрибутам.
						vTSIterators[first_term_n].Next();
						continue;
					}
					else
					{
						ReturnWithTrace( iee );
					}
				}
			}

			// Поиск результатов проверки шаблон-вопроса в кэше.
			if( aQstFlag )
			{
				unsigned int cur_id = vTSIterators[first_term_n].Item()->GetId();
				unsigned int cur_subid = vTSIterators[first_term_n].Item()->GetSubId();

				// Проверка, сопоставлся ли такой шаблон-вопрос.
				DictsMatchingResults::MatchingResult mr;
				if( INF_ENGINE_SUCCESS != ( iee = vResults.AlreadyMatched( cur_id, cur_subid, mr ) ) )
				{
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
				}

				switch( mr )
				{
				// Этот шаблон-вопрос раньше не встречалася.
				case DictsMatchingResults::MR_NOT_MATCHED:
					{
						// Инициализация попытки сопоставления шаблон-вопроса.
						if( INF_ENGINE_SUCCESS != ( iee = vResults.TryMatch( cur_id, cur_subid ) ) )
						{
							if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
								ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
							ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
						}

						break;
					}


				// Этот шаблон-вопрос уже сопоставлялся с текущей фразой пользователя.
				case DictsMatchingResults::MR_SUCCESS:
				case DictsMatchingResults::MR_FAIL:
					{
						// Этот шаблон-вопрос уже был успешно сопоставлен с текущей фразой пользователя.
						if( DictsMatchingResults::MR_SUCCESS == mr )
						{
							// Копирование ранее полученных результатов.
							unsigned int debug_cnt = 0;
							if( INF_ENGINE_SUCCESS != ( iee = vResults.CopyMatch( cur_id, cur_subid, debug_cnt, cond_weight ) ) )
							{
								if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
									ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
								ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
							}
							else
								copy_match_cnt += debug_cnt;
						}
						// Эта строка не подходит для текущей фразы пользователя.
						else if( DictsMatchingResults::MR_FAIL == mr )
						{
							// Игнорируем данный шаблон-вопрос.
						}


						// Переход к следующимему шаблон-вопросу.
						first_attrs = vTSIterators[first_term_n].Next();
						do
						{
							// Для данного вхождения ещё есть нерасмотренные атрибуты.
							if( first_attrs )
							{
								if( first_attrs->GetId() != cur_id || first_attrs->GetSubId() != cur_subid )
									break;
								else
									first_attrs = vTSIterators[first_term_n].Next();
							}
							else
							{
								// Переходим к следующему вхождению.
								++first_term_n;
								// Больше вхождений нет, все варианты были рассмотрены.
								if( first_term_n >= vTSIterators.size() )
									variants_exists = false;
								else
									first_attrs = vTSIterators[first_term_n].Item();
							}
						} while( variants_exists );

						continue;
					}

				default:
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Unknown matching result type: %u", mr );
				}
			}

			// Найдены атрибуты, которые могут находиться в начале фразы.
			if( first_attrs->GetFlagFirstTerm() && ( skip_id != first_attrs->GetId() || skip_sub_id != first_attrs->GetSubId() ) )
			{
				// Проверка суперзвёздочек перед первым термином.
				if( aPatternMatcher.GetFoundTerm( first_term_n )->vCoords.vFirstPos.GetPos() > first_attrs->GetStarShiftBefore() )
				{
					// Если шаблон-вопрос начинается обязательным элементом.
					if( !vTSIterators[first_term_n].Item()->GetFlagStarBefore() &&
						!vTSIterators[first_term_n].Item()->GetFlagDictBefore() &&
						!vTSIterators[first_term_n].Item()->GetFlagEmptyDictBefore() &&
						aPatternMatcher.GetFoundTerm( first_term_n )->vCoords.vFirstPos.GetPos() - aPatternMatcher.GetNDocImage().First().GetPos() != 1 )
					{
						// Сопоставить строку словаря входной фразе не удалось, т.к. начало первого обязательного элемента не совпадает с началом входной фразы.
						// do nothing
					}
					else
					{
						// Загрузка шаблон-вопроса для проверки.
						if( aQstFlag )
							question = aPatternsData.GetQuestionString( vTSIterators[first_term_n].Item()->GetId(), vTSIterators[first_term_n].Item()->GetSubId() );
						else
							question = aPatternsData.GetThatString( vTSIterators[first_term_n].Item()->GetId() );

						// Проверка наличия вхождений у всех словарей в данном шаблон-вопросе.
						bool all_dicts_have_matches = true;
						bool positions_order = true;
						unsigned int prev_dict_n = static_cast<unsigned int>( -1 );
						for( unsigned int item_n = 0; item_n < question.GetItemsNum(); ++item_n )
						{
							// Если текущий элемент шаблон-вопроса - словарь.
							if( InfPatternItems::itDict == question.GetItemType( item_n ) )
							{
								// Загрузка текущего словаря.
								InfPatternItems::TagDictManipulator dict( question.GetItem( item_n ) );

								// Если данный словарь может иметь пустое вхождение, то пропускаем его.
								if( dict.HasEmptyVariant() )
									continue;

								// Проверка наличия вхождений у данного словаря.
								BFinder::DictsMatchingResults::DictMatch dict_match;

								if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetMatch( dict.GetDictID(), dict_match ) ) )
									ReturnWithTrace( iee );
								if( !dict_match.GetMatchesNumber() )
								{
									all_dicts_have_matches = false;
									break;
								}

								// Если это не первый словарь в шаблон-вопросе.
								if( static_cast<unsigned int>( -1 ) != prev_dict_n )
								{
									// Загрузка предыдущего словаря.
									InfPatternItems::TagDictManipulator prev_dict( question.GetItem( prev_dict_n ) );

									// Загрузка суммарной информации о вхождениях текущего и предыдущего словарей.
									const DictsMatchingResults::DictInfo * dict_info;
									const DictsMatchingResults::DictInfo * prev_info;

									if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetDictInfo( dict.GetDictID(), dict_info ) ) )
										ReturnWithTrace( iee );
									if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetDictInfo( prev_dict.GetDictID(), prev_info ) ) )
										ReturnWithTrace( iee );

									if( !dict_info || !prev_info )
										ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
									if( dict_info )
									{
										// Проверка упорядоченности вхождений.
										if( prev_info->vMostLeftBeginPos >= dict_info->vMostRightEndPos ||
											prev_info->vMostLeftEndPos >= dict_info->vMostRightEndPos )
										{
											positions_order = false;
											break;
										}

										prev_dict_n = item_n;
									}

								}
							}
						}

						// Если в данном шаблон-вопросе есть вхождения у всех словарей и их порядок непротиворечив.
						if( all_dicts_have_matches && positions_order )
						{
							// Вычислеям расстояние в словах от начала входной фразы до начала текущего термина.
							unsigned int length = aPatternMatcher.GetFoundTerm( first_term_n )->vCoords.vFirstPos.GetPos() - 1;

							// Суммируем минимальную необходмую и максимальную допустимую длину вхождений словарей между началом шаблон-вопроса и текущим термином.
							unsigned int min_len = 0;
							unsigned int max_len = 0;
							for( unsigned int item_n = 0; item_n < first_attrs->GetPos(); ++item_n )
							{
								// Если текущий элемент шаблон-вопроса - словарь.
								if( InfPatternItems::itDict == question.GetItemType( item_n ) )
								{
									// Загрузка текущего словаря.
									InfPatternItems::TagDictManipulator dict( question.GetItem( item_n ) );
									// Загрузка суммарной информации о вхождениях текущего словарей.
									const DictsMatchingResults::DictInfo * dict_info;
									if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetDictInfo( dict.GetDictID(), dict_info ) ) )
										ReturnWithTrace( iee );

									if( !dict_info && !dict.HasEmptyVariant() )
										ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
									if( dict_info )
									{
										min_len += dict.HasEmptyVariant() ? 0 : dict_info->vMinMatchLen;
										if( static_cast<unsigned int>( -1 ) != max_len )
											max_len = static_cast<unsigned int>( -1 ) == dict_info->vMaxMatchLen ? static_cast<unsigned int>( -1 ) : max_len + dict_info->vMaxMatchLen;
									}
								}
								// Если текущий элемент шаблон-вопроса - (супер)звёздочка.
								else if( InfPatternItems::itStar == question.GetItemType( item_n ) )
									max_len = static_cast<unsigned int>( -1 );
							}

							if( length >= min_len && length <= max_len )
							{
								// Первый термин сопоставлен успешно.
								last_term_n = first_term_n;
								cur_term_n = last_term_n + 1;
								break;
							}


						}
					}
				}
			}
			// Переход к следующим атрибутам.

			vTSIterators[first_term_n].Next();
		}
		// Пытаемся сопоставить очередной термин.
		if( static_cast<unsigned int>( -1 ) != last_term_n && cur_term_n < vTSIterators.size() )
		{
			const TermsStorage::TermAttrsRO * last_attrs = vTSIterators[last_term_n].Item();
			const TermsStorage::TermAttrsRO * cur_attrs = vTSIterators[cur_term_n].Item();

			// Поиск атрибутов для текущего вхождения, принадлежащих текущему шаблон-вопросу.
			cur_attrs = vTSIterators[cur_term_n].BinarySearch( last_attrs->GetId(), last_attrs->GetSubId() );

			if( !cur_attrs )
			{
				// Переход к следующему вхождению.
				++cur_term_n;
			}
			else
			{
				// Проверка последнего и текущего вхождений на пересечение.
				if( aPatternMatcher.GetFoundTerm( cur_term_n )->vCoords.vFirstPos <= aPatternMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos )
				{
					// Текущее вхождение не может следовать за последним удачно сопоставленным.
					// Переход к следующему вхождению.
					vTSIterators[cur_term_n].Next();
					if( vTSIterators[cur_term_n].IsEnd() )
						++cur_term_n;
				}
				else
				{
					if( vTSIterators[cur_term_n].IsEnd() )
					{
						// Для текущего вхождения больше нет атрибутов.
						// Пререход к следующему вхождению.
						++cur_term_n;
					}
					else
					{
						// Найдены атрибуты, принадлежащие текущему шаблон-вопросу.

						// Поиск в текущем шаблон-вопросе атрибутов, следующих сразу за последними удачно сопоставленными.
						while( !vTSIterators[cur_term_n].IsEnd() &&
							   cur_attrs->GetId() == last_attrs->GetId() &&
							   cur_attrs->GetSubId() == last_attrs->GetSubId() &&
							   ( ( cur_attrs->GetFlagRealTerm() && last_attrs->GetFlagRealTerm() && cur_attrs->GetRealTermPos() <= last_attrs->GetRealTermPos() ) ||
								 ( ( !cur_attrs->GetFlagRealTerm() || !last_attrs->GetFlagRealTerm() ) && cur_attrs->GetPos() <= last_attrs->GetPos() )
								  )
							 )
						{
							cur_attrs = vTSIterators[cur_term_n].Next();
						}

						// Найденный обязательный термин должен следовать в шаблоне сразу за предыдущим найденным.
						if( !vTSIterators[cur_term_n].IsEnd() &&
							cur_attrs->GetId() == last_attrs->GetId() &&
							cur_attrs->GetSubId() == last_attrs->GetSubId() &&
							( ( cur_attrs->GetFlagRealTerm() && last_attrs->GetFlagRealTerm() && cur_attrs->GetRealTermPos() == 1 + last_attrs->GetRealTermPos() ) ||
							  ( cur_attrs->GetFlagRealTerm() && !last_attrs->GetFlagRealTerm()  && cur_attrs->GetPos() > last_attrs->GetPos() && cur_attrs->GetRealTermPos() == last_attrs->GetRealTermPos() ) ||
							  ( !cur_attrs->GetFlagRealTerm() && last_attrs->GetFlagRealTerm()  && cur_attrs->GetPos() > last_attrs->GetPos() && cur_attrs->GetRealTermPos() == 1 + last_attrs->GetRealTermPos() ) ||
							  ( !cur_attrs->GetFlagRealTerm() && !last_attrs->GetFlagRealTerm() && cur_attrs->GetPos() > last_attrs->GetPos() && cur_attrs->GetRealTermPos() == last_attrs->GetRealTermPos() )
							   )
						  )
						{
							// Идущие подряд в шаблон-вопросе термины должны в сопоставлемой фразе тоже идти подряд.
							if( cur_attrs->GetPos() > last_attrs->GetPos() + 1 ||
								aPatternMatcher.GetFoundTerm( cur_term_n )->vCoords.vFirstPos.GetPos() == aPatternMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos.GetPos() + 1 )
							{
								// Подходящие атрибуты найдены.

								// Проверка суперзвёздочек между текущим термином и предыдущим.
								if( aPatternMatcher.GetFoundTerm( cur_term_n )->vCoords.vFirstPos.GetPos() -
									aPatternMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos.GetPos() >
									vTSIterators[cur_term_n].Item()->GetStarShiftBefore() )
								{
									// Вычислеям расстояние в словах между текущим термином и предыдущим термином.
									unsigned int length = aPatternMatcher.GetFoundTerm( cur_term_n )->vCoords.vFirstPos.GetPos() -
														  aPatternMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos.GetPos() - 1;

									// Суммируем минимальную необходмую и максимальную допустимую длину вхождений словарей между текущим термином и предыдущим.
									unsigned int min_len = 0;
									unsigned int max_len = 0;
									for( unsigned int item_n = last_attrs->GetPos() + 1; item_n < cur_attrs->GetPos(); ++item_n )
									{
										// Если текущий элемент шаблон-вопроса - словарь.
										if( InfPatternItems::itDict == question.GetItemType( item_n ) )
										{
											// Загрузка текущего словаря.
											InfPatternItems::TagDictManipulator dict( question.GetItem( item_n ) );
											// Загрузка суммарной информации о вхождениях текущего словарей.
											const DictsMatchingResults::DictInfo * dict_info;
											if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetDictInfo( dict.GetDictID(), dict_info ) ) )
												ReturnWithTrace( iee );

											if( !dict_info && !dict.HasEmptyVariant() )
												ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
											if( dict_info )
											{
												min_len += dict.HasEmptyVariant() ? 0 : dict_info->vMinMatchLen;
												if( static_cast<unsigned int>( -1 ) != max_len )
													max_len = static_cast<unsigned int>( -1 ) == dict_info->vMaxMatchLen ? static_cast<unsigned int>( -1 ) : max_len + dict_info->vMaxMatchLen;
											}
										}
										// Если текущий элемент шаблон-вопроса - (супер)звёздочка.
										else if( InfPatternItems::itStar == question.GetItemType( item_n ) )
											max_len = static_cast<unsigned int>( -1 );
									}

									if( length >= min_len && length <= max_len )
									{
										// Помечаем текущий термин как последний успешно сопоставленный.
										last_term_n = cur_term_n;
										++cur_term_n;
									}
									else
									{
										// Не согласовать словари между текущим термином и предыдущим.
										// Переход к следующему набору атрибутов для текущего вхождения.
										vTSIterators[cur_term_n].Next();
									}
								}
								else
								{
									// Не хватает терминов для суперзвёздочек.
									// Переход к следующему набору атрибутов для текущего вхождения.
									vTSIterators[cur_term_n].Next();
								}
							}
							else
							{
								// В шаблон-вопросе термины стоят подряд, но в сопоставляемой фразе между ними присутствуют слова.
								// Переход к следующему набору атрибутов для текущего вхождения.
								vTSIterators[cur_term_n].Next();
							}
						}
						else
						{
							// Переход к следующему вхождению.
							vTSIterators[cur_term_n].SetEnd();
							++cur_term_n;
						}
					}
				}

			}
		}		
		
		// Если был сопоставлен последний термин.
		if( cur_term_n >= vTSIterators.size() )
		{
			if( last_term_n != static_cast<unsigned int>( -1 ) )
			{
				if( vTSIterators[last_term_n].Item()->GetFlagLastTerm() )
				{
					// Последний удачно сопоставленный термин является последним в строке словаря.
					// Проверка суперзвёздочек после послденего термина.
					if( aPatternMatcher.GetNDocImage().Size() - aPatternMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos.GetPos() >
						vTSIterators[last_term_n].Item()->GetStarShiftAfter() )
					{
						// Вычислеям расстояние в словах между текущим термином и предыдущим термином.
						unsigned int length = aPatternMatcher.GetNDocImage().Size() - aPatternMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos.GetPos() - 2;

						// Суммируем минимальную необходмую и максимальную допустимую длину вхождений словарей между текущим термином и концом шаблона.
						unsigned int min_len = 0;
						unsigned int max_len = 0;
						for( unsigned int item_n = vTSIterators[last_term_n].Item()->GetPos() + 1; item_n < question.GetItemsNum(); ++item_n )
						{
							// Если текущий элемент шаблон-вопроса - словарь.
							if( InfPatternItems::itDict == question.GetItemType( item_n ) )
							{
								// Загрузка текущего словаря.
								InfPatternItems::TagDictManipulator dict( question.GetItem( item_n ) );
								// Загрузка суммарной информации о вхождениях текущего словарей.
								const DictsMatchingResults::DictInfo * dict_info;
								if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetDictInfo( dict.GetDictID(), dict_info ) ) )
									ReturnWithTrace( iee );
								if( !dict_info && !dict.HasEmptyVariant() )
									ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
								if( dict_info )
								{
									min_len += dict.HasEmptyVariant() ? 0 : dict_info->vMinMatchLen;
									if( static_cast<unsigned int>( -1 ) != max_len )
										max_len = static_cast<unsigned int>( -1 ) == dict_info->vMaxMatchLen ? static_cast<unsigned int>( -1 ) : max_len + dict_info->vMaxMatchLen;
								}
							}
							// Если текущий элемент шаблон-вопроса - (супер)звёздочка.
							else if( InfPatternItems::itStar == question.GetItemType( item_n ) )
								max_len = static_cast<unsigned int>( -1 );
						}

						if( length >= min_len && length <= max_len )
						{
							// Если шаблон-вопрос заканчивается обязательным элементом.
							if( !vTSIterators[last_term_n].Item()->GetFlagStarAfter() &&
								!vTSIterators[last_term_n].Item()->GetFlagDictAfter() &&
								!vTSIterators[last_term_n].Item()->GetFlagEmptyDictAfter() &&
								aPatternMatcher.GetNDocImage().Last().GetPos() - aPatternMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos.GetPos() != 1 )
							{
								// Сопоставить строку словаря входной фразе не удалось, т.к. конец последнего обязательного элемента не совпадает с концом входной фразы.
								// do nothing
							}
							else
							{
								static avector<Candidat*> candidats;
								candidats.clear();
								InfPatternItems::QuestionRO question;

								// Подготовка списка сопоставленных терминов.
								matched_terms.clear();
								matched_terms.reserve( vTSIterators.size() );
								if( matched_terms.no_memory() )
									ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
								for( unsigned int term_n = 0; term_n < vTSIterators.size(); ++term_n )
								{
									if( vTSIterators[term_n].Item() )
										matched_terms.push_back( MatchedTerm( vTSIterators[term_n].Item(), aPatternMatcher.GetFoundTerm( term_n ) ) );
								}

								// Сопоставление обязательных терминов прошло успешно, проводим полное сопоставление шаблон-вопроса.
								if( aQstFlag )
									// Загрузка шаблон-вопроса для проверки.
									question = aPatternsData.GetQuestionString( vTSIterators[first_term_n].Item()->GetId(), vTSIterators[last_term_n].Item()->GetSubId() );
								else
									// Загрузка шаблон-вопроса для проверки.
									question = aPatternsData.GetThatString( vTSIterators[first_term_n].Item()->GetId() );

								// Подсчёт количества обработанных на поиске шаблон-вопросов, прошедших проверку всех эвристик.
								if( prev_deep_pattern_id != vTSIterators[first_term_n].Item()->GetId() || prev_deep_question_id != vTSIterators[first_term_n].Item()->GetSubId() )
								{
									++aRequestStat.vQstDeep;
									if( aUserBase )
										++aRequestStat.ex.counter_request_userbase_patterns_intricate_index;
									else
										++aRequestStat.ex.counter_request_mainbase_patterns_intricate_index;

									prev_deep_pattern_id = vTSIterators[first_term_n].Item()->GetId();
									prev_deep_question_id = vTSIterators[first_term_n].Item()->GetSubId();
								}

								bool is_best_match = false;	
								if( INF_ENGINE_SUCCESS == ( iee = MatchPattern( question, aPatternMatcher, matched_terms.get_buffer(), matched_terms.size(), aDictsMatchingResults, candidats, cond_weight, is_best_match ) ) )
								{
									// Подсчёт количества подошедших шаблон-вопросов.
									if( prev_acc_pattern_id != vTSIterators[first_term_n].Item()->GetId() || prev_acc_question_id != vTSIterators[first_term_n].Item()->GetSubId() )
									{
										prev_acc_pattern_id = vTSIterators[first_term_n].Item()->GetId();
										prev_acc_question_id = vTSIterators[first_term_n].Item()->GetSubId();
									}

									if( !candidats.size() )
										ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

									if( aQstFlag )
									{
										// Добавляем информацию об успешно сопоставленном шаблоне в список результатов.
										for( unsigned int cand_n = 0; cand_n < candidats.size(); ++cand_n )
										{
											++add_match_cnt;
											if( INF_ENGINE_SUCCESS != ( iee = vResults.AddCandidat( candidats[cand_n] ) ) )
												ReturnWithTrace( iee );
											++aRequestStat.vQstAcc;
											if( aUserBase )
												++aRequestStat.ex.counter_request_userbase_patterns_accepted_index;
											else
												++aRequestStat.ex.counter_request_mainbase_patterns_accepted_index;
										}

										// Совпадение имеет максимально возможный вес для данного шаблон-вопроса.
										if( is_best_match )
										{
											// Сохраняем данные для пропуска данного шаблон-вопроса.
											skip_id = vTSIterators[last_term_n].Item()->GetId();
											skip_sub_id = vTSIterators[last_term_n].Item()->GetSubId();

											// Сбрасываем текущее состояние.
											for( unsigned int n = first_term_n + 1; n < cur_term_n; ++n )
												if( INF_ENGINE_SUCCESS != ( iee = aPatternMatcher.SearchAttrsByN( n, vTSIterators[n] ) ) )
												{
													if( INF_ENGINE_WARN_UNSUCCESS )
														ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
													else
														ReturnWithTrace( iee );
												}

											cur_term_n = static_cast<unsigned int>( -1 );
											last_term_n = static_cast<unsigned int>( -1 );
											// Начинаем сопоставление с нового шаблон-вопроса.
											continue;
										}
									}
									else
									{
										// Добавляем информацию об успешно сопоставленной that-строке.
										if( INF_ENGINE_SUCCESS != ( iee = vThatMatchingResults.SetValid( vTSIterators[last_term_n].Item()->GetId() ) ) )
										{
											if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
												ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

											ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
										}
									}
								}
								else
								{
									if( INF_ENGINE_WARN_UNSUCCESS == iee )
									{
										// Сопоставить шаблон-вопрос входной фразе не удалось.

										if( !aQstFlag )
										{
											// Добавляем информацию о неуспешно сопоставленной that-строке.
											if( INF_ENGINE_SUCCESS != ( iee = vThatMatchingResults.SetInvalid( vTSIterators[last_term_n].Item()->GetId() ) ) )
											{
												if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
													ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

												ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
											}
										}

										; //printf( "\nDBG:      STOP 4\n");
									}
									else
										ReturnWithTrace( iee );
								}
							}
						}
						else
						{
							// Сопоставить строку словаря входной фразе не удалось.
							// do nothing
						}
					}
					else
					{
						// Сопоставить строку словаря входной фразе не удалось из-за суперзвёздочек после последнего термина.
						// do nothing
					}
				}
				else
				{
					// Последний удачно сопоставленный термин не является последним в строке шаблона.
					// do nothing
				}


				step_back = true;
			}
		}

		// "Откат" для сопоставления с использованием другого набора найденных атрибутов.
		if( step_back )
		{
			while( cur_term_n > first_term_n && vTSIterators[cur_term_n - 1].IsEnd() )
			{
				if( INF_ENGINE_SUCCESS != ( iee = aPatternMatcher.SearchAttrsByN( cur_term_n - 1, vTSIterators[cur_term_n - 1] ) ) )
				{
					if( INF_ENGINE_WARN_UNSUCCESS )
						ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
					else
						ReturnWithTrace( iee );
				}
				--cur_term_n;
			}

			if( cur_term_n > first_term_n )
				--cur_term_n;

			// Откатились до первого термина.
			if( cur_term_n == first_term_n )
			{
				last_term_n = static_cast<unsigned int>( -1 );
				if( !vTSIterators[cur_term_n].IsEnd() )
				{
					// Переход к следующему набору атрибутов.
					vTSIterators[cur_term_n].Next();
				}
				else
				{
					// Переход к следующему вхождению.
					++first_term_n;
				}
			}
			else
			{
				// Переход к следующему набору атрибутов.
				vTSIterators[cur_term_n].Next();
				// Поиск последнего удачно сопоставленного термина.
				last_term_n = cur_term_n - 1;
				while( last_term_n > first_term_n && vTSIterators[last_term_n].IsEnd() )
					--last_term_n;
			}

			step_back = false;
		}
	}

	return INF_ENGINE_SUCCESS;
}


InfEngineErrors BFinder::ProcessDictsTerms( DictMatcher & aDictMatcher, DictsMatchingResults & aDictsMatchingResults, const DLDataRO & aDLData )
{
	unsigned int copy_match_cnt = 0;
	unsigned int add_match_cnt = 0;

	vTSIterators.clear();
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Подготовка итераторов для поиска совпадений.
	vTSIterators.resize( aDictMatcher.GetTermsNum() );
	if( vTSIterators.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Инициализация итераторов.
	bool variants_exists = false;
	unsigned int first_term_n = 0;
	for( unsigned int term_n = 0; term_n < vTSIterators.size(); ++term_n )
	{
		// Проверка наличия атрибутов в базе.
		unsigned int term_id = aDictMatcher.GetTermId( term_n );
		if( INF_ENGINE_SUCCESS != ( iee = aDictMatcher.SearchAttrsByN( term_n, vTSIterators[term_n] ) ) )
		{
			if( INF_ENGINE_WARN_UNSUCCESS )
				return INF_ENGINE_SUCCESS;
			else
				ReturnWithTrace( iee );
		}

		if( !variants_exists )
		{
			while( vTSIterators[term_n].Item() && !vTSIterators[term_n].Item()->GetFlagFirstTerm() )
				vTSIterators[term_n].Next();
			variants_exists |= !vTSIterators[term_n].IsEnd() && vTSIterators[term_n].Item()->GetFlagFirstTerm();
		}

		// Если для данного термина атрибутов не нашлось, то считаем первым термином следующий.
		if( !variants_exists )
			++first_term_n;
	}
	// Последний удачно сопоставленный термин.
	unsigned int last_term_n = static_cast<unsigned int>( -1 );
	// Текущий проверяемый на совпадение термин.
	unsigned int cur_term_n = first_term_n + 1;

	// Поиск совпадений.
	while( variants_exists )
	{
		// Если ещё ни одного термина не сопоставлено.
		while( static_cast<unsigned int>( -1 ) == last_term_n && first_term_n < vTSIterators.size() )
		{
			// Поиск первого вхождения.
			const TermsStorage::TermAttrsRO * first_attrs = vTSIterators[first_term_n].Item();
			// Для данного вхождения кончились нерасмотренные атрибуты.
			if( !first_attrs )
			{
				// Переходим к следующему вхождению.
				++first_term_n;
				// Больше вхождений нет, все варианты были рассмотрены.
				if( first_term_n >= vTSIterators.size() )
					variants_exists = false;
				continue;
			}

			// Проверка, сопоставлялась ли такая строка ранее.
			DictsMatchingResults::MatchingResult mr;
			if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.AlreadyMatched( first_attrs->GetId(), first_attrs->GetSubId(), mr ) ) )
			{
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
			}

			switch( mr )
			{
			// Эта строка раньше не встречалась.
			case DictsMatchingResults::MR_NOT_MATCHED:
				{
					// Найдены атрибуты, которые могут находиться в начале фразы.
					if( first_attrs->GetFlagFirstTerm() )
					{
						// Проверка суперзвёздочек перед первым термином.
						if( aDictMatcher.GetFoundTerm( first_term_n )->vCoords.vFirstPos.GetPos() > first_attrs->GetStarShiftBefore() )
						{
							// Первый термин сопоставлен успешно.
							last_term_n = first_term_n;
							cur_term_n = last_term_n + 1;

							// Инициализация попытки сопоставления строки.
							if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.TryMatch( first_attrs->GetId(), first_attrs->GetSubId() ) ) )
							{
								if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
									ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
								ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
							}
						}
						else
						{
							// Переход к следующим атрибутам.
							vTSIterators[first_term_n].Next();
						}
					}
					else
					{
						// Переход к следующим атрибутам.
						vTSIterators[first_term_n].Next();
					}

					break;
				}

			// Эта строка уже была успешно сопоставлена с текущей фразой пользователя.
			case DictsMatchingResults::MR_SUCCESS:
				{
					// Копирование ранее полученных результатов.
					unsigned int debug_cnt = 0;
					if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.CopyMatch( first_attrs->GetId(), first_attrs->GetSubId(), debug_cnt, 0 ) ) )
					{
						if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
					}
					else
						copy_match_cnt += debug_cnt;

					// Переход к следующим атрибутам.
					vTSIterators[first_term_n].Next();
					break;
				}

			// Эта строка не подходит для текущей фразы пользователя.
			case DictsMatchingResults::MR_FAIL:
				// Переход к следующим атрибутам.
				vTSIterators[first_term_n].Next();
				break;

			default:
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Unknown matching result type: %u", mr );
			}
		}

		// Пытаемся сопоставить очередной термин.
		if( static_cast<unsigned int>( -1 ) != last_term_n && cur_term_n < vTSIterators.size() )
		{
			// Проверка последнего и текущего вхождений на пересечение.
			if( aDictMatcher.GetFoundTerm( cur_term_n )->vCoords.vFirstPos <= aDictMatcher.GetFoundTerm( last_term_n )->vCoords.vFirstPos )
			{
				// Текущее вхождение не может следовать за последним удачно сопоставленным.
				// Переход к следующему вхождению.
				vTSIterators[cur_term_n].SetEnd();
				++cur_term_n;
			}
			else
			{
				if( vTSIterators[cur_term_n].IsEnd() )
				{
					// Для текущего вхождения больше нет атрибутов.
					// Пререход к следующему вхождению.
					++cur_term_n;
				}
				else
				{
					const TermsStorage::TermAttrsRO * last_attrs = vTSIterators[last_term_n].Item();
					const TermsStorage::TermAttrsRO * cur_attrs = vTSIterators[cur_term_n].Item();

					// Поиск атрибутов для текущего вхождения, принадлежащих текущему шаблон-вопросу.
					while( !vTSIterators[cur_term_n].IsEnd() &&
						   ( cur_attrs->GetId() < last_attrs->GetId() ||
							 ( cur_attrs->GetId() == last_attrs->GetId() && cur_attrs->GetSubId() < last_attrs->GetSubId() )
						   )
						 )
					{
						cur_attrs = vTSIterators[cur_term_n].Next();
					}

					if( !vTSIterators[cur_term_n].IsEnd() &&
						cur_attrs->GetId() == last_attrs->GetId() &&
						cur_attrs->GetSubId() == last_attrs->GetSubId() )
					{
						// Найдены атрибуты, принадлежащие текущему шаблон-вопросу.

						// Поиск в текущем шаблон-вопросе атрибутов, следующих сразу за последними удачно сопоставленными.
						while( !vTSIterators[cur_term_n].IsEnd() &&
							   cur_attrs->GetId() == last_attrs->GetId() &&
							   cur_attrs->GetSubId() == last_attrs->GetSubId() &&
							   cur_attrs->GetPos() <= last_attrs->GetPos() &&
							   cur_attrs->GetRealTermPos() < last_attrs->GetRealTermPos() )
						{
							cur_attrs = vTSIterators[cur_term_n].Next();
						}
						
						if( !vTSIterators[cur_term_n].IsEnd() &&
							cur_attrs->GetId() == last_attrs->GetId() &&
							cur_attrs->GetSubId() == last_attrs->GetSubId() &&
							cur_attrs->GetPos() > last_attrs->GetPos() &&
							( cur_attrs->GetRealTermPos() == last_attrs->GetRealTermPos() ||
								( cur_attrs->GetRealTermPos() == 1 + last_attrs->GetRealTermPos() && last_attrs->GetFlagRealTerm() ) ) )
						{
							// Подходящие атрибуты найдены.

							// Проверка суперзвёздочек между текущим термином и предыдущим.
							if( aDictMatcher.GetFoundTerm( cur_term_n )->vCoords.vFirstPos.GetPos() -									
								aDictMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos.GetPos() >
								vTSIterators[cur_term_n].Item()->GetStarShiftBefore() -
								vTSIterators[last_term_n].Item()->GetStarShiftBefore() )
							{
								// Проверка следования терминов сразу друг за другом или наличия звёздочки или суперзвёздочки между ними.
								if( vTSIterators[last_term_n].Item()->GetFlagStarAfter() ||
									vTSIterators[cur_term_n].Item()->GetFlagStarBefore() ||
									aDictMatcher.GetFoundTerm( cur_term_n )->vCoords.vFirstPos.GetPos() -
									aDictMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos.GetPos() == 1 )

								{
									// Помечаем текущий термин как последний успешно сопоставленный.
									last_term_n = cur_term_n;
									++cur_term_n;
								}
								else
								{
									// Найденные вхождения располагаются не друг за другом, но при этом между ними нет звёздочек.
									// Переход к следующему набору атрибутов для текущего вхождения.
									vTSIterators[cur_term_n].Next();
								}
							}
							else
							{
								// Не хватает терминов для суперзвёздочек.
								// Переход к следующему набору атрибутов для текущего вхождения.
								vTSIterators[cur_term_n].Next();
							}

						}
						else
						{
							// Не найдены атрибуты, следующие в текущем шаблон-вопросе сразу за последними удачно сопоставленными.

							// Переход к следующему вхождению.
							vTSIterators[cur_term_n].SetEnd();
							++cur_term_n;
						}
					}
					else
					{
						// Переход к следующему вхождению.
						vTSIterators[cur_term_n].SetEnd();
						++cur_term_n;
					}
				}
			}
		}

		// Если был сопоставлен последний термин.
		if( cur_term_n >= vTSIterators.size() )
		{
			if( last_term_n != static_cast<unsigned int>( -1 ) )
			{
				if( vTSIterators[last_term_n].Item()->GetFlagLastTerm() )
				{
					// Последний удачно сопоставленный термин является последним в строке словаря.

					// Проверка суперзвёздочек после послденего термина.
					if( aDictMatcher.GetNDocImage().Size() - aDictMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos.GetPos() - 1 >
						vTSIterators[last_term_n].Item()->GetStarShiftAfter() )
					{
						// Загружаем словарь.
						InfDictWrapManipulator dict_manip;
						if( INF_ENGINE_SUCCESS != ( iee = aDLData.GetDictById( vTSIterators[first_term_n].Item()->GetId(), dict_manip ) ) )
							ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
						// Загружаем подошедшую строку.
						InfDictStringManipulator dict_string;
						if( INF_ENGINE_SUCCESS != ( iee = dict_manip.GetString( 0, dict_string ) ) )
							ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
						// Загружаем шаблон подошеджей строки.
						PatternsStorage::Pattern dict_string_pattern = aDLData.GetPatternFromStorage( dict_string.GetId() ); 
						
						// Определение наличия звёздочек.
						bool star_before = false;
						bool star_after = false;
						bool stars = false;
						for( unsigned int item_n = 0; item_n < dict_string_pattern.GetItemsNum(); ++item_n )
							if( InfPatternItems::itStar == dict_string_pattern.GetItemType( item_n ) )
							{
								stars = true;
								if( item_n < vTSIterators[first_term_n].Item()->GetPos() )
									star_before = true;
								if( item_n > vTSIterators[last_term_n].Item()->GetPos() )
									star_after = true;
							}

						// Вес совпадения.
						unsigned int terms_weight = 0;
						unsigned int superstar_cnt = vTSIterators[first_term_n].Item()->GetStarShiftBefore() + vTSIterators[first_term_n].Item()->GetStarShiftAfter();
						for( unsigned int iter_n = first_term_n; iter_n <= last_term_n; ++iter_n )
							if( vTSIterators[iter_n].Item() )
								terms_weight += aDictMatcher.GetFoundTerm( iter_n )->vCoords.vLastPos.GetPos() - aDictMatcher.GetFoundTerm( iter_n )->vCoords.vFirstPos.GetPos() + 1;

						terms_weight = 2 * terms_weight + superstar_cnt;
						
						// Сопоставление прошло успешно, добавляем результат в списк совпадений словарей.
						if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.AddMatch( vTSIterators[first_term_n].Item()->GetId(),
																						  vTSIterators[first_term_n].Item()->GetSubId(),
																						  aDictMatcher.GetFoundTerm( first_term_n )->vCoords.vFirstPos,
																						  aDictMatcher.GetFoundTerm( last_term_n )->vCoords.vLastPos,
																						  star_before,
																						  star_after,
																						  stars,
																						  terms_weight ) ) )
						{
							if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
								ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
							ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
						}
						else
							++add_match_cnt;
					}
					else
					{
						// Сопоставить строку словаря входной фразе не удалось из-за суперзвёздочек после последнего термина.
						// do nothing
					}
				}
				else
				{
					// Последний удачно сопоставленный термин не является последним в строке словаря.
					// do nothing
				}

				// "Откат" для сопоставления с использованием другого набора найденных атрибутов.
				while( cur_term_n > first_term_n && vTSIterators[cur_term_n - 1].IsEnd() )
				{
					if( INF_ENGINE_SUCCESS != ( iee = aDictMatcher.SearchAttrsByN( cur_term_n - 1, vTSIterators[cur_term_n - 1] ) ) )
					{
						if( INF_ENGINE_WARN_UNSUCCESS )
							ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
						else
							ReturnWithTrace( iee );
					}
					--cur_term_n;
				}
				
				if( cur_term_n > first_term_n )
					--cur_term_n;
				
				// Откатились до первого термина.
				if( cur_term_n == first_term_n )
				{
					last_term_n = static_cast<unsigned int>( -1 );
					if( !vTSIterators[cur_term_n].IsEnd() )
					{
						// Переход к следующему набору атрибутов.
						vTSIterators[cur_term_n].Next();
					}
					else
					{
						// Переход к следующему вхождению.
						++first_term_n;
					}
				}
				else
				{
					// Переход к следующему набору атрибутов.
					vTSIterators[cur_term_n].Next();
					last_term_n = cur_term_n - 1;
					while( last_term_n > first_term_n && vTSIterators[last_term_n].IsEnd() )
						--last_term_n;
				}
			}
		}
	}

	// Индексация найденных результатов.
	if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.BuildIndex( vRuntimeAllocator ) ) )
	{
		if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	}	
	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::CheckConditions( const DLDataRO & aPatternsData, const InfConditionsRegistry & aConditionsRegistry,
										  unsigned int aPatternId, const Session & aSession, unsigned int & aExtWeight )
{
	aExtWeight = 0;

	// Получение шаблона.
	static InfPatternRO pattern;
	aPatternsData.GetPattern( aPatternId, pattern );

	if( pattern.HasThatString() )
	{
		if( !vThatMatchingResults.IsValidPattern( aPatternId ) )
			return INF_ENGINE_WARN_UNSUCCESS;
		aExtWeight |= 1 << 31;
	}

	for( unsigned int i = 0; i < pattern.GetConditionsNum(); i++ )
	{
		// Получение манипулятора с шаблон-условием.
		InfPatternItems::ConditionRO condition = pattern.GetCondition( i );

		// Проверка кэша.
		unsigned int ExtPos = floor( ( (double)condition.GetConditionId() ) / 4 );
		unsigned int IntPos = condition.GetConditionId() % 4;
		if( vConditionsCheckCache.size() <= ExtPos )
		{
			unsigned int oldsize = vConditionsCheckCache.size();

			// Выделение памяти для кэша.
			vConditionsCheckCache.resize( ExtPos + 1 );
			if( vConditionsCheckCache.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Обнуление выделенной памяти.
			bzero( vConditionsCheckCache.get_buffer() + oldsize, sizeof( char ) * ( ExtPos + 1 - oldsize ) );
		}

		if( vConditionsCheckCache[ExtPos] & ( 2 << ( IntPos * 2 ) ) )
		{
			// Условие уже было проверено.
			if( ( vConditionsCheckCache[ExtPos] & ( 1 << ( IntPos * 2 ) ) ) == 0 )
				return INF_ENGINE_WARN_UNSUCCESS;
		}
		else
		{
			// Проверка шаблон-условия.
			bool Check = condition.Check( aConditionsRegistry, aSession );

			// Сохранение результата в кэш.
			vConditionsCheckCache[ExtPos] |= ( 2 << ( IntPos * 2 ) );

			if( !Check )
				return INF_ENGINE_WARN_UNSUCCESS;
			else
				vConditionsCheckCache[ExtPos] |= ( 1 << ( IntPos * 2 ) );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::SearchThats( IndexBaseRO & aIndexTBase, const DLDataRO & aPatternsData,
									  const NanoLib::NameIndex & aInfPersonRegistry, const InfConditionsRegistry & aConditionsRegistry,
									  DocImage & aThatString, const Session & aSession, DictsMatchingResults & aDictsMatchingResults,
									  RequestStat & aRequestStat, bool aUserBase )
{
	TimeStamp tm_check;

	// Очистка списка шаблонов, которые подошли под условия, накладываемые собственными шаблон-that'ами.
	vThatMatchingResults.Reset();

	// Проверка существования соответствующей индексной базы.
	if( !aIndexTBase.IsReady() )
	{
		// Результат поиска - пустота.
		return INF_ENGINE_SUCCESS;
	}

	RequestStat rstat;

	// Поиск в шаблонах.
	InfEngineErrors iee = MatchThatPattern( aIndexTBase, aPatternsData, aConditionsRegistry, aInfPersonRegistry, aThatString, aSession, aDictsMatchingResults, aRequestStat, aUserBase );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Проход по безтермовым шаблонам.
	iee = SearchPatternsWithoutTerms( aThatString, aSession, aPatternsData, aInfPersonRegistry, aIndexTBase, aDictsMatchingResults, aConditionsRegistry, false, rstat, aUserBase );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	vThatMatchingResults.BuildIndex();

	// Время поиска по that-строкам.
	aRequestStat.vThatBaseTm += tm_check.Interval();
	if( aUserBase )
		aRequestStat.ex.timer_request_userbase_that_match = tm_check.Interval();
	else
		aRequestStat.ex.timer_request_mainbase_that_match = tm_check.Interval();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::MatchThatPattern( IndexBaseRO & aIndexTBase, const DLDataRO & aPatternsData,
										   const InfConditionsRegistry & aConditionsRegistry,  const NanoLib::NameIndex & aInfPersonRegistry,
										   DocImage & aRequest, const Session & aSession, DictsMatchingResults & aDictsMatchingResults,
										   RequestStat & aRequestStat, bool aUserBase )
{
	// Очистка матчера.
	vPatternMatcher.Reset();

	// Проверка наличия базы терминов для шаблонов-that.
	if( !aIndexTBase.IsReady() )
		return INF_ENGINE_SUCCESS;

	// Поиск терминов шаблонов.

	// Получение идентификатор InfPerson.
	unsigned int InfPersonLength;
	const char * InfPerson = aSession.GetValue( InfEngineVarInfPerson ).AsText().GetValue( InfPersonLength );
	if( InfPerson )
	{
		const unsigned int * id = aInfPersonRegistry.Search( InfPerson, InfPersonLength );
		if( !id )
			vPatternMatcher.SetInfPerson( 0 );
		else
			vPatternMatcher.SetInfPerson( *id );
	}
	else
		vPatternMatcher.SetInfPerson( 0 );

	// Поиск терминов по базе.
	TimeStamp tm_check;
	vPatternMatcher.SetTermsStorage( &aIndexTBase.GetTermsStorage() );
	InfEngineErrors iee = aIndexTBase.Check( aRequest, vPatternMatcher );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	// Время поиска по дереву терминов that-строк.
	aRequestStat.vTermsCheckThatTm += tm_check.Interval();
	if( aUserBase )
		aRequestStat.ex.timer_request_userbase_terms_that_match = tm_check.Interval();
	else
		aRequestStat.ex.timer_request_mainbase_terms_that_match = tm_check.Interval();

	RequestStat rstat;
	if( INF_ENGINE_SUCCESS != ( iee = ProcessPatternTerms( aPatternsData, aConditionsRegistry, aSession, vPatternMatcher, aDictsMatchingResults, false, rstat, aUserBase ) ) )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::SearchPatterns( IndexBaseRO & aIndexQBase, const DLDataRO & aPatternsData,
										 const NanoLib::NameIndex & aInfPersonRegistry, const InfConditionsRegistry & aConditionsRegistry,
										 DocImage & aRequest, const Session & aSession, DictsMatchingResults & aDictsMatchingResults,
										 RequestStat & aRequestStat, bool aUserBase )
{
	// Очистка матчера.
	vPatternMatcher.Reset();

	// Поиск терминов шаблонов.

	// Получение идентификатор InfPerson.
	unsigned int InfPersonLength;
	const char * InfPerson = aSession.GetValue( InfEngineVarInfPerson ).AsText().GetValue( InfPersonLength );
	if( InfPerson )
	{
		const unsigned int * id = aInfPersonRegistry.Search( InfPerson, InfPersonLength );
		if( !id )
			vPatternMatcher.SetInfPerson( 0 );
		else
			vPatternMatcher.SetInfPerson( *id );
	}
	else
		vPatternMatcher.SetInfPerson( 0 );

	// Поиск терминов по базе.
	vPatternMatcher.SetTermsStorage( &( aIndexQBase.GetTermsStorage() ) );

	// Время начала процедуры поиска терминов в префиксном дереве.
	TimeStamp tm_check;
	InfEngineErrors iee = aIndexQBase.Check( aRequest, vPatternMatcher );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	// Время окончания процедуры поиска терминов в префиксном дереве.
	aRequestStat.vTermsCheckTime += tm_check.Interval();
	if( aUserBase )
		aRequestStat.ex.timer_request_userbase_terms_patterns = tm_check.Interval();
	else
		aRequestStat.ex.timer_request_mainbase_terms_patterns = tm_check.Interval();
	if( INF_ENGINE_SUCCESS != ( iee = ProcessPatternTerms( aPatternsData, aConditionsRegistry, aSession, vPatternMatcher, aDictsMatchingResults, true, aRequestStat, aUserBase ) ) )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::SearchQuestions( IndexBaseRO & aIndexQBase, const DLDataRO & aPatternsData,
										  const NanoLib::NameIndex & aInfPersonRegistry,
										  const InfConditionsRegistry & aConditionsRegistry, DocImage & aRequest,
										  const Session & aSession, DictsMatchingResults & aDictsMatchingResults,
										  RequestStat & aRequestStat, bool aUserBase )
{
	// Очистка старых данных.
	vResults.Reset();

	// Поиск в шаблонах.

	// Время начала процедуры поиска.
	TimeStamp tm_start;
	// Поиск.
	InfEngineErrors iee = SearchPatterns( aIndexQBase, aPatternsData, aInfPersonRegistry, aConditionsRegistry, aRequest, aSession, aDictsMatchingResults, aRequestStat, aUserBase );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	// Время окончания процедуры поиска.
	aRequestStat.vQstTime = tm_start.Interval();
	if( aUserBase )
		aRequestStat.ex.timer_request_userbase_patterns_index = tm_start.Interval();
	else
		aRequestStat.ex.timer_request_mainbase_patterns_index = tm_start.Interval();

	// Время начала процедуры поиска.
	TimeStamp tm_pwt_start;
	// Поиск среди безтермовых шаблонов.
	iee = SearchPatternsWithoutTerms( aRequest, aSession, aPatternsData, aInfPersonRegistry, aIndexQBase, aDictsMatchingResults, aConditionsRegistry, true, aRequestStat, aUserBase );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	// Время окончания процедуры поиска.
	aRequestStat.vPWTQstTime = tm_pwt_start.Interval();
	if( aUserBase )
		aRequestStat.ex.timer_request_userbase_patterns_fuzzy = tm_pwt_start.Interval();
	else
		aRequestStat.ex.timer_request_mainbase_patterns_fuzzy = tm_pwt_start.Interval();


	// Сортировка кандидатов по идентификаторам и удаление повторных шаблонов с меньшим весом.
	vResults.BuildIndex();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::MatchPatternWT( const NDocImage & aRequest, InfPatternItems::QuestionRO & aQuestion, const DLDataRO & aPatternsData,
										 const DictsMatchingResults & aDictsMatchingResults, avector<Candidat*> & aCandidats, unsigned int aCondWeight,
										 RequestStat & aRequestStat, bool aUserBase )
{	
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	aCandidats.clear();

	// Итератор для навигации по входной фразе.
	NDocImage::Iterator iter = aRequest.First();
	++iter;

	// Выделение памяти под подготавливаемого кандидата.
	static avector<Candidat::Item> vCandidat;
	vCandidat.clear();
	vCandidat.resize( aQuestion.GetItemsNum() );
	if( vCandidat.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Указатель на элемент, для которого ещё не определён конец вхождения.
	Candidat::Item * waiting_for_end = nullptr;

	// Состояния сопоставления словарей.
	static avector<unsigned int> dict_states;
	static avector<DictsMatchingResults::DictMatch> dict_matches;
	static avector<bool> dict_has_empty_variant;

	dict_states.resize( aQuestion.GetItemsNum() );
	if( dict_states.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	dict_matches.resize( aQuestion.GetItemsNum() );
	if( dict_matches.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	dict_has_empty_variant.resize( aQuestion.GetItemsNum() );
	if( dict_has_empty_variant.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Инициализация состояний совпадения словарей
	unsigned int prev_dict_n = static_cast<unsigned int>( -1 );
	for( unsigned int item_n = 0; item_n < aQuestion.GetItemsNum(); ++item_n )
	{
		if( aQuestion.GetItemType( item_n ) == InfPatternItems::itDict )
		{
			InfPatternItems::TagDictManipulator dict( aQuestion.GetItem( item_n ) );

			dict_has_empty_variant[item_n] = dict.HasEmptyVariant();

			// Проверка наличия вхождений у данного словаря.
			if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetMatch( dict.GetDictID(), dict_matches[item_n] ) ) )
				ReturnWithTrace( iee );

			if( dict_matches[item_n].GetMatchesNumber() )
			{	
				dict_states[item_n] = 0;

				// Если это обязательный словарь и он не является первым в шаблон-вопросе.
				if( !dict.HasEmptyVariant() && static_cast<unsigned int>( -1 ) != prev_dict_n )
				{
					// Загрузка предыдущего словаря.
					InfPatternItems::TagDictManipulator prev_dict( aQuestion.GetItem( prev_dict_n ) );

					// Загрузка суммарной информации о вхождениях текущего и предыдущего словарей.
					const DictsMatchingResults::DictInfo * dict_info;
					if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetDictInfo( dict.GetDictID(), dict_info ) ) )
						ReturnWithTrace( iee );
					const DictsMatchingResults::DictInfo * prev_info;
					if( INF_ENGINE_SUCCESS != ( iee = aDictsMatchingResults.GetDictInfo( prev_dict.GetDictID(), prev_info ) ) )
						ReturnWithTrace( iee );

					if( !dict_info || !prev_info )
						ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

					if( dict_info )
					{
						// Проверка упорядоченности вхождений.
						if( prev_info->vMostLeftBeginPos >= dict_info->vMostRightEndPos ||
							prev_info->vMostLeftEndPos >= dict_info->vMostRightEndPos )
						{
							// Не выполнено необходимое условия упорядоченности вхождений словарей.
							return INF_ENGINE_WARN_UNSUCCESS;
						}

						prev_dict_n = item_n;
					}
				}
			}
			else if( dict.HasEmptyVariant() )
			{
				dict_states[item_n] = 0;
			}
			else
				// Необходимый словарь не подошёл.
				return INF_ENGINE_WARN_UNSUCCESS;
		}
		else
		{
			dict_states[item_n] = static_cast<unsigned int>( -1 );
			dict_has_empty_variant[item_n] = false;
		}
	}

	++aRequestStat.vPWTQstDeep;
	if( aUserBase )
		++aRequestStat.ex.counter_request_userbase_patterns_intricate_fuzzy;
	else
		++aRequestStat.ex.counter_request_mainbase_patterns_intricate_fuzzy;

	// Сопоставление шаблон-вопроса.
	bool step_back = false;
	unsigned int item_n = 0;
	while( item_n < aQuestion.GetItemsNum() )
	{
		switch( aQuestion.GetItemType( item_n ) )
		{
		case InfPatternItems::itText:
			{
				// Безтермовый шаблон может содержать только звёздочки, суперзвёздочки и пустые inline-словари.
				InfPatternItems::Text text( aQuestion.GetItem( item_n ) );
				if( !text.IfStopWordsOnly() )
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
			}

		case InfPatternItems::itStar:
		case InfPatternItems::itEliStar:
			{
				// Установка конца предыдущей звёздочки.
				if( waiting_for_end )
				{
					waiting_for_end->vEndPos = iter;
					--( waiting_for_end->vEndPos );

					vCandidat[item_n].vWaitingForEnd = waiting_for_end;
					waiting_for_end = nullptr;
				}

				InfPatternItems::Star star( aQuestion.GetItem( item_n ) );
				// Для сопоставления суперзвёздочки необходимм хотя бы одно слово.
				if( star.IsSuperStar() && aRequest.Last().GetPos() - iter.GetPos() < 1 )
				{
					// Не удаётся сопоставить.
					step_back = true;
					vCandidat[item_n].Reset();
					break;
				}


				// Сохраняем в результате тип успешно сопоставленного элемента шаблон-вопроса.
				vCandidat[item_n].vType = star.IsSuperStar() ? Candidat::Item::SuperStar : Candidat::Item::Star;
				// Сохраняем в результате начало совпадения элемента.
				// Конец совпадения будет сохранён при сопоставлении следующего элемента.
				vCandidat[item_n].vBeginPos = iter;

				// Запоминаем элемент, требующий определения конца вхождения.
				waiting_for_end = &( vCandidat[item_n] );
				vCandidat[item_n].vStarAfter = true;

				// Делаем поправку в текущем положении
				if( star.IsSuperStar() )
					++iter;
				
				vCandidat[item_n].vMostLeftNextBegin = iter;

				break;
			}

		case InfPatternItems::itDict:
			{
				// Сопоставление пустого варианта.
				if( dict_states[item_n] == dict_matches[item_n].GetMatchesNumber() )
				{
					// Сохраняем в результете тип успешно сопоставленного элемента шаблон-вопроса.
					vCandidat[item_n].vType = Candidat::Item::InlineDict;
					// Сохраняем в результате начало и конец совпадения элемента в виде пустого отрезка.
					vCandidat[item_n].vBeginPos = aRequest.Last();
					vCandidat[item_n].vEndPos = aRequest.Last();
					--vCandidat[item_n].vEndPos;
					vCandidat[item_n].vMostLeftNextBegin = iter;
					break;
				}

				const DictsMatchingResults::DictMatchInfo * dict_match_info = dict_matches[item_n].GetMatch( dict_states[item_n] );

				if( iter > dict_match_info->vBeginPos )
				{
					// Не удаётся сопоставить.
					step_back = true;
					vCandidat[item_n].vWaitingForEnd = waiting_for_end;
					break;
				}

				// Установка конца предыдущей звёздочки.
				if( waiting_for_end )
				{
					// Установка конца предыдущей звёздочки.
					waiting_for_end->vEndPos = dict_match_info->vBeginPos;
					--( waiting_for_end->vEndPos );

					vCandidat[item_n].vWaitingForEnd = waiting_for_end;
					waiting_for_end = nullptr;
				}
				else if( iter != dict_match_info->vBeginPos && !dict_match_info->vStarBefore )
				{
					// Не удаётся сопоставить.
					step_back = true;
					vCandidat[item_n].Reset();
				}

				// Если шаблон-вопрос заканчивается словарём.
				if( item_n == aQuestion.GetItemsNum() - 1 )
				{
					// Конец последнего элемента не совпадает с концом входной фразы.
					if( !dict_match_info->vStarAfter && aRequest.Last().GetPos() - dict_match_info->vEndPos.GetPos() != 1 )
					{
						// Не удаётся сопоставить.
						step_back = true;
						vCandidat[item_n].Reset();
					}
				}

				if( !step_back )
				{
					// Загружаем тэг, для проверки типа словаря.
					InfPatternItems::TagDictManipulator dict_manip( aQuestion.GetItem( item_n ) );

					// Сохраняем в результате тип успешно сопоставленного элемента шаблон-вопроса.
					vCandidat[item_n].vType = dict_manip.GetDictType() == InfPatternItems::DT_LCDICT ? Candidat::Item::LCDict : Candidat::Item::Dict;
					// Сохраняем в результате начало и конец совпадения элемента.
					if( !dict_match_info->vStarBefore )
						vCandidat[item_n].vBeginPos = dict_match_info->vBeginPos;
					else
					{
						if( item_n && vCandidat[item_n].vWaitingForEnd )
							vCandidat[item_n].vBeginPos = dict_match_info->vBeginPos;
						else
							vCandidat[item_n].vBeginPos = iter;
					}
					vCandidat[item_n].vEndPos = dict_match_info->vEndPos;

					// Сохраняем результате идентификатор словаря и номер строки.
					vCandidat[item_n].vDictId = dict_match_info->vDictId;
					vCandidat[item_n].vLineId = dict_match_info->vLineId;
					vCandidat[item_n].vWeight = dict_match_info->vWeight;

					vCandidat[item_n].vStars = dict_match_info->vStars;
					vCandidat[item_n].vStarStart = dict_match_info->vStarBefore;
					vCandidat[item_n].vStarFinish = dict_match_info->vStarAfter;

					// Запоминаем элемент, требующий определения конца вхождения.
					if( dict_match_info->vStarAfter )
					{
						waiting_for_end = &( vCandidat[item_n] );
						vCandidat[item_n].vStarAfter = true;
					}

					// Продвигаемся вперёд по входной фразе.
					iter = vCandidat[item_n].vEndPos;
					++iter;
					
					vCandidat[item_n].vMostLeftNextBegin = iter;
				}

				break;
			}

		case InfPatternItems::itDictInline:
			{
				InfPatternItems::TagDictInlineManipulator inline_dict( aQuestion.GetItem( item_n ) );

				// Безтермовый шаблон может содержать только звёздочки, суперзвёздочки и пустые inline-словари.
				if( !inline_dict.HasEmpty() )
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

				// Сохраняем в результете тип успешно сопоставленного элемента шаблон-вопроса.
				vCandidat[item_n].vType = Candidat::Item::InlineDict;
				// Сохраняем в результате начало и конец совпадения элемента в виде пустого отрезка.
				vCandidat[item_n].vBeginPos = aRequest.Last();
				vCandidat[item_n].vEndPos = aRequest.Last();
				--vCandidat[item_n].vEndPos;
				vCandidat[item_n].vMostLeftNextBegin = iter;

				break;
			}

		default:
			// Безтермовый шаблон может содержать только звёздочки, суперзвёздочки и пустые inline-словари.
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}

		// Если был успешно сопоставлен последний элемент шаблон-вопроса.
		if( !step_back && item_n + 1 == aQuestion.GetItemsNum() )
		{
			// Установка конца последней звёздочки.
			if( waiting_for_end )
			{
				// Установка конца предыдущей звёздочки.
				waiting_for_end->vEndPos = aRequest.Last();
				--( waiting_for_end->vEndPos );
			}
			// Была ли входная фраза сопоставлена до самого конца.
			if( waiting_for_end || aRequest.Last().GetPos() <= iter.GetPos() )
			{

				// Выделение памяти для кандидата.
				Candidat * candidat = nAllocateObject( vRuntimeAllocator, Candidat );
				if( !candidat )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				new( (nMemoryAllocatorElementPtr*)candidat )Candidat;

				// Заполнение информации о кандидате.
				candidat->vItems = nAllocateObjects( vRuntimeAllocator, Candidat::Item, vCandidat.size() );
				if( !candidat->vItems )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				for( unsigned int item_n = 0; item_n < vCandidat.size(); ++item_n )
					candidat->vItems[item_n] = vCandidat[item_n];
				// Количество элементов в кандидате.
				candidat->vItemsNumber = vCandidat.size();

				// Вес кандидата.
				// TODO: вычисление веса.
				unsigned int terms_weight = 0;
				unsigned int dicts_weight = 0;
				candidat->vSelfWeight = CalculateCandidatWeight( candidat, terms_weight, dicts_weight );
				candidat->vWeight = candidat->vSelfWeight |  aCondWeight | aQuestion.GetWeight();

				aCandidats.push_back( candidat );
				if( aCandidats.no_memory() )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				// Подсчет подошедших шаблон-вопросов.
				++aRequestStat.vPWTQstAcc;
				if( aUserBase )
					++aRequestStat.ex.counter_request_userbase_patterns_accepted_fuzzy;
				else
					++aRequestStat.ex.counter_request_mainbase_patterns_accepted_fuzzy;
			}
			else
			{
				// Сопоставление завершилось неудачей.
			}

			step_back = true;
		}

		// Откатываемся, используя ещё не рассмотренные варианты сопоставления словарей.
		if( step_back )
		{
			waiting_for_end = nullptr;
			// Поиск нерассмотренных вариантов вхождения словарей.
			while( item_n )
			{
				// Если текущий элемент является словарём.
				if( dict_states[item_n] != static_cast<unsigned int>( -1 ) )
				{
					// Если у текущего словаря ещё остались нерасмотренные варианты.
					if( ( dict_states[item_n] + 1 < dict_matches[item_n].GetMatchesNumber() ) ||
						( dict_has_empty_variant[item_n] && dict_states[item_n] < dict_matches[item_n].GetMatchesNumber() ) )
					{
						// Преходим к рассмотрению следующего  варианта вхождения словаря.
						++dict_states[item_n];
						waiting_for_end = vCandidat[item_n].vWaitingForEnd;
						break;
					}
					else
						// Начинаем сопоставлять этот словарь заново.
						dict_states[item_n] = 0;
				}

				// Откатываемся на один элемент назад.
				vCandidat[item_n].Reset();
				--item_n;			

				if( item_n )
					iter = vCandidat[item_n-1].vMostLeftNextBegin;
				else
					iter = vCandidat[item_n].vBeginPos;
					
			}

			// Откатились до первого элемента.
			if( !item_n )
			{
				// Если текущий элемент является словарём.
				if( dict_states[item_n] != static_cast<unsigned int>( -1 ) )
				{
					// Если у текущего словаря ещё остались нерасмотренные варианты.
					if( ( dict_states[item_n] + 1 < dict_matches[item_n].GetMatchesNumber() ) ||
						( dict_has_empty_variant[item_n] && dict_states[item_n] < dict_matches[item_n].GetMatchesNumber() ) )
					{
						// Преходим к рассмотрению следующего варианта вхождения словаря.
						++dict_states[item_n];
					}
					else
						// Все возможные вариатны сопоставления были рассмотрены.
						break;
				}
				else
					// Все возможные вариатны сопоставления были рассмотрены.
					break;

				// Начинаем сопоставление с начала входной фразы.
				iter = aRequest.First();
				++iter;
			}

			step_back = false;
		}
		else
			// Переход к следующему элементу шаблон-вопроса.
			++item_n;
	}

	if( !aCandidats.size() )
	{
		// Сопоставление завершилось неудачей.
		return INF_ENGINE_WARN_UNSUCCESS;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BFinder::SearchPatternsWithoutTerms( DocImage & aRequest, const Session & aSession,
													 const DLDataRO & aPatternsData, const NanoLib::NameIndex & aInfPersonRegistry,
													 const IndexBaseRO & aPatterns, DictsMatchingResults & aDictsMatchingResults,
													 const InfConditionsRegistry & aConditionsRegistry, bool aQstFlag,
													 RequestStat & aRequestStat, bool aUserBase )
{	
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	// Манипулятор шаблона и шаблон-вопроса.
	InfPatternItems::QuestionRO pqst;

	// Поправка веса шаблона при проверке условий шаблона.
	unsigned int cond_weight = 0;

	// Идентификатор инфа.
	unsigned int InfPersonLength;
	const char * InfPerson = aSession.GetValue( InfEngineVarInfPerson ).AsText().GetValue( InfPersonLength );
	unsigned int inf_person_id = 0;
	if( InfPerson )
	{
		const unsigned int * id = aInfPersonRegistry.Search( InfPerson, InfPersonLength );
		if( id )
			inf_person_id = *id;
	}

	// Обработка запроса пользователя.
	NDocImage ndoc_image;
	TermsDocImageIterator iter( aRequest, &aPatterns.GetStopDict(), false );
	if( INF_ENGINE_SUCCESS != ( iee = ndoc_image.Init( iter ) ) )
	{
		if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		else
			ReturnWithError( iee, INF_ENGINE_STRING_ERROR_INTERNAL );
	}

	// Определение интервала шаблонов с заданным InfPersonId.
	unsigned int left_n = 0;
	unsigned int right_n = aPatterns.GetNumberOfPWT();
	while( left_n < right_n )
	{
		unsigned int cur_n = ( left_n + right_n ) >> 1;
		if( aPatterns.GetInfPersonIdOfPWT( cur_n ) > inf_person_id )
			right_n = cur_n;
		else if( aPatterns.GetInfPersonIdOfPWT( cur_n ) < inf_person_id )
			left_n = cur_n + 1;
		else
		{
			left_n = cur_n;
			right_n = cur_n;
			while( left_n && aPatterns.GetInfPersonIdOfPWT( left_n - 1 ) == inf_person_id )
				--left_n;
			while( right_n <  aPatterns.GetNumberOfPWT() && aPatterns.GetInfPersonIdOfPWT( right_n ) == inf_person_id )
				++right_n;
			break;
		}
	}

	// Итератор для последовательного прохода по двум интревалам.
	struct IntervalRunner
	{
		IntervalRunner( unsigned int _a1, unsigned int _a2, unsigned int _b1 = 0, unsigned int _b2 = 0 )
		{
			a1 = _a1;
			a2 = _a2;
			b1 = _b1;
			b2 = _b2;
			if( a1 < a2 )
			{
				first_pass = true;
				pos = a1;
			}
			else
			{
				pos = b1;
				first_pass = false;
			}
		}

		inline bool IsFinished()
		{
			return !first_pass && pos >= b2;
		}

		inline unsigned int operator++()
		{
			if( first_pass )
			{
				if( pos + 1 < a2 )
					++pos;
				else
				{
					first_pass = false;
					pos = b1;
				}
			}
			else
				++pos;

			return pos;
		}

		inline operator unsigned int() { return pos; }

		unsigned int a1, a2;
		unsigned int b1, b2;
		unsigned int pos;
		bool first_pass;
	};

	unsigned int last_id_without_person = 0;
	for( unsigned int i = 0; i < aPatterns.GetNumberOfPWT(); ++i )
		if( 0 == aPatterns.GetInfPersonIdOfPWT( i ) )
			++last_id_without_person;
		else
			break;

	unsigned int copy_match_cnt = 0;
	unsigned int add_match_cnt = 0;

	vResults.SetDLData( &aPatternsData );

	// Проход по всем безтерминным шаблонам.
	for( IntervalRunner i( left_n, right_n, 0, last_id_without_person ); !i.IsFinished(); ++i )
	{
		// Подсчёт количества обработанных на поиске шаблон-вопросов.
		++aRequestStat.vPWTQst;
		if( aUserBase )
			++aRequestStat.ex.counter_request_userbase_patterns_all_fuzzy;
		else
			++aRequestStat.ex.counter_request_mainbase_patterns_all_fuzzy;

		// Вычисление идентификатора шаблона и подшаблона.
		unsigned int id = aPatterns.GetIdOfPWT( i );
		unsigned int subid = aPatterns.GetSubIdOfPWT( i );

		// Проверка условий шаблона.
		if( aQstFlag )
		{
			if( INF_ENGINE_SUCCESS != ( iee = CheckConditions( aPatternsData, aConditionsRegistry, id, aSession, cond_weight ) ) )
			{
				if( INF_ENGINE_WARN_UNSUCCESS == iee )
					continue;
				ReturnWithTrace( iee );
			}
		}

		// Поиск результатов проверки шаблон-вопроса в кэше.
		if( aQstFlag )
		{
			// Проверка, сопоставлся лм такой шаблон-вопрос.
			DictsMatchingResults::MatchingResult mr;
			if( INF_ENGINE_SUCCESS != ( iee = vResults.AlreadyMatched( id, subid, mr ) ) )
			{
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
			}

			switch( mr )
			{
			// Этот шаблон-вопрос раньше не встречалася.
			case DictsMatchingResults::MR_NOT_MATCHED:
				{
					// Инициализация попытки сопоставления шаблон-вопроса.
					if( INF_ENGINE_SUCCESS != ( iee = vResults.TryMatch( id, subid ) ) )
					{
						if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
					}

					break;
				}

			// Этот шаблон-вопрос уже был успешно сопоставлен с текущей фразой пользователя.
			case DictsMatchingResults::MR_SUCCESS:
				{
					// Копирование ранее полученных результатов.
					unsigned int debug_cnt = 0;
					if( INF_ENGINE_SUCCESS != ( iee = vResults.CopyMatch( id, subid, debug_cnt, cond_weight ) ) )
					{
						if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
					}
					else
						copy_match_cnt += debug_cnt;

					// Переход к следующему шаблон-вопросу.
					continue;
				}

			// Эта строка не подходит для текущей фразы пользователя.
			case DictsMatchingResults::MR_FAIL:
				// Переход к следующему шаблон-вопросу.
				continue;

			default:
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Unknown matching result type: %u", mr );
			}
		}

		// Получение шаблон-вопроса.
		if( aQstFlag )
			pqst = aPatternsData.GetQuestionString( id, subid );
		else
			pqst = aPatternsData.GetThatString( id );

		static avector<Candidat*> candidats;
		candidats.clear();				
		if( INF_ENGINE_SUCCESS == ( iee = MatchPatternWT( ndoc_image, pqst, aPatternsData, aDictsMatchingResults, candidats, cond_weight, aRequestStat, aUserBase ) ) )
		{
			if( !candidats.size() )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

			if( aQstFlag )
			{
				// Добавляем информацию об успешно сопоставленном шаблоне в список результатов.
				for( unsigned int cand_n = 0; cand_n < candidats.size(); ++cand_n )
				{
					candidats[cand_n]->vPatternId = id;
					candidats[cand_n]->vQuestionId = subid;
					if( INF_ENGINE_SUCCESS != ( iee = vResults.AddCandidat( candidats[cand_n] ) ) )
						ReturnWithTrace( iee );
					++add_match_cnt;
				}
			}
			else
			{
				// Сохраняем информацию об успешно совпавшей that-строке.
				if( INF_ENGINE_SUCCESS != ( iee = vThatMatchingResults.SetValid( id ) ) )
				{
					if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
				}
			}
		}
		else
		{
			if( INF_ENGINE_WARN_UNSUCCESS == iee )
			{
				if( !aQstFlag )
				{
					// Сохраняем информацию несовпавшей that-строке.
					if( INF_ENGINE_SUCCESS != ( iee = vThatMatchingResults.SetInvalid( id ) ) )
					{
						if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
					}
				}
			}
			else
				ReturnWithTrace( iee );
		}
	}

	return INF_ENGINE_SUCCESS;
}
