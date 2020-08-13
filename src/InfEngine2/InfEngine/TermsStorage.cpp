#include "TermsStorage.hpp"

#include <algorithm>

#include <NanoLib/LogSystem.hpp>

bool TermAttrsCmp( TermsStorage::TermAttrs * a, TermsStorage::TermAttrs * b )
{
	if( a->GetInfPersonId() == b->GetInfPersonId() )
	{
		if( a->GetId() == b->GetId() )
		{
			if( a->GetSubId() == b->GetSubId() )
			{
				if( a->GetPos() == b->GetPos() )
					return false;
				else
					return a->GetPos() < b->GetPos();
			}
			else
				return a->GetSubId() < b->GetSubId();
		}
		else
			return a->GetId() < b->GetId();
	}
	else
		return a->GetInfPersonId() < b->GetInfPersonId();
}


InfEngineErrors TermsStorageWR::AddTermAttrs( const char * aTerm, unsigned int aTermLength,
											  TermsStorage::TermAttrs & aTermAttrs, unsigned int & aTermId, bool & aTermIsNew )
{
	// Проверка состояния базы.
	if( NanoLib::NameIndex::stClosed == vTermIndex.GetState() )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Определение идентификатора темина.
	const unsigned int * term_id_ptr = vTermIndex.Search( aTerm, aTermLength );
	unsigned int term_id;
	if( term_id_ptr )
	{
		term_id = *term_id_ptr;
		aTermIsNew = false;
	}
	else
	{
		// Такой термин ещё не зарегестрирован. Добавляем его.
		NanoLib::NameIndex::ReturnCode rc = vTermIndex.AddName( aTerm, aTermLength, term_id );
		if( NanoLib::NameIndex::rcSuccess != rc )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Failed to register new term. Error %.", rc );

		// Увеличение массива списков атрибутов.
		TermAttrsList ** tal = vAttrs.grow();
		if( !tal )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		*tal = nAllocateObject( vMemoryAllocator, TermAttrsList );
		if( !*tal )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new( (nMemoryAllocatorElementPtr*)*tal )TermAttrsList;

		aTermIsNew = true;
	}

	// Проверка корректности идентификатора термина.
	if( term_id >= vAttrs.size() )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Копирование набора атрибутов.
	TermsStorage::TermAttrs * attrs = nAllocateObject( vMemoryAllocator, TermsStorage::TermAttrs );
	if( !attrs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new( (nMemoryAllocatorElementPtr*)attrs ) TermsStorage::TermAttrs( aTermAttrs );

	// Добавление набора атрибутов в базу атрибутов терминов.
	vAttrs[term_id]->push_back( attrs );
	if( vAttrs[term_id]->no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aTermId = term_id;

	return INF_ENGINE_SUCCESS;
}


unsigned int TermsStorageWR::GetNeedMemorySize() const
{
	unsigned int need_memory = 0;
	// Количество терминов.
	need_memory += sizeof( uint32_t );
	// Индекс отсортированых списков наборов атрибутов.
	need_memory += sizeof( uint32_t ) * vAttrs.size();

	// Атрибуты.
	unsigned int attrs_cnt = 0;
	for( unsigned int list_n = 0; list_n < vAttrs.size(); ++list_n )
	{
		// Количество наборов атрибутов в списке текущего термина.
		need_memory += sizeof( uint32_t );

		TermAttrsList & list = *vAttrs[list_n];
		attrs_cnt += list.size();

		// Копирование списка атрибутов во временный буфер для сортировки.
		vTermAttrsList.resize( list.size() );
		if( vTermAttrsList.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( vTermAttrsList.get_buffer(), list.get_buffer(), list.size() * sizeof( TermsStorage::TermAttrs* ) );
		// Сортировка.
		std::sort( vTermAttrsList.get_buffer(), vTermAttrsList.get_buffer() + vTermAttrsList.size(), TermAttrsCmp );

		// Подсчёт количества различных PersonId в списке.
		unsigned int last_person_id = static_cast<unsigned int>( -1 );
		unsigned int person_id_number = 0;
		for( unsigned int attr_n = 0; attr_n < vTermAttrsList.size(); ++attr_n )
			if( last_person_id != vTermAttrsList[attr_n]->GetInfPersonId() )
			{
				last_person_id = vTermAttrsList[attr_n]->GetInfPersonId();
				++person_id_number;
			}

		// Количество записей в индексе по PersonId данного термина.
		need_memory += sizeof( uint32_t );
		// Размер индекса по PersonId данного термина.
		need_memory += person_id_number * 3 * sizeof( uint32_t );

		// Список наборов атрибутов данного термина.
		need_memory += vAttrs[list_n]->size() * TermsStorage::ATTRS_SIZE;
		// Выравнивание в памяти.
		need_memory += need_memory & 3 ? 4 - ( need_memory & 3 ) : 0;
	}

	return need_memory;
}

InfEngineErrors TermsStorageWR::Save( char * aBuffer, unsigned int aBufferSize, unsigned int & aUsedMemory ) const
{
	// Проверка аргументов.
	if( !aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка размера буфера для сохранения.
	unsigned int memory = GetNeedMemorySize();
	if( memory > aBufferSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	char * ptr = aBuffer;
	// Сохранение количества терминов.
	*reinterpret_cast<uint32_t*>( ptr ) = vAttrs.size();
	ptr += sizeof( uint32_t );

	// Указатель на индекс отсортированых списков наборов атрибутов.
	uint32_t * shift = reinterpret_cast<uint32_t*>( ptr );
	ptr += sizeof( uint32_t ) * vAttrs.size();

	for( unsigned int list_n = 0; list_n < vAttrs.size(); ++list_n )
	{
		// Смещение к списку наборов атрибутов текущего термина.
		*shift = ptr - aBuffer;
		++shift;

		// Сохранение количества наборов атрибутов у текущего термина.
		TermAttrsList & list = *vAttrs[list_n];
		*reinterpret_cast<uint32_t*>( ptr ) = list.size();
		ptr += sizeof( uint32_t );

		// Копирование списка атрибутов во временный буфер для сортировки.
		vTermAttrsList.resize( list.size() );
		if( vTermAttrsList.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( vTermAttrsList.get_buffer(), list.get_buffer(), list.size() * sizeof( TermsStorage::TermAttrs* ) );
		// Сортировка.
		std::sort( vTermAttrsList.get_buffer(), vTermAttrsList.get_buffer() + vTermAttrsList.size(), TermAttrsCmp );

		// Подсчёт количества различных PersonId в списке.
		unsigned int last_person_id = static_cast<unsigned int>( -1 );
		unsigned int person_id_number = 0;
		for( unsigned int attr_n = 0; attr_n < vTermAttrsList.size(); ++attr_n )
			if( last_person_id != vTermAttrsList[attr_n]->GetInfPersonId() )
			{
				last_person_id = vTermAttrsList[attr_n]->GetInfPersonId();
				++person_id_number;
			}

		// Сохранение количества различных PersonId в списке.
		*reinterpret_cast<uint32_t*>( ptr ) = person_id_number;
		ptr += sizeof( uint32_t );
		last_person_id = static_cast<unsigned int>( -1 );
		// Индекс по PersonId.
		uint32_t * person_id_index = reinterpret_cast<uint32_t*>( ptr );
		ptr += person_id_number * 3 * sizeof( uint32_t );
		for( unsigned int attr_n = 0; attr_n < vTermAttrsList.size(); ++attr_n )
		{
			if( last_person_id != vTermAttrsList[attr_n]->GetInfPersonId() )
			{
				// Фиксация количества соответствующих предыдущему PerosnId атрибутов.
				if( static_cast<unsigned int>( -1 ) != last_person_id )
					++person_id_index;

				// Сохранение PersonId.
				*person_id_index = vTermAttrsList[attr_n]->GetInfPersonId();
				++person_id_index;
				// Сохраняем номер первого набора атрибутов, относящегося к данному PersonId.
				*person_id_index = attr_n;
				++person_id_index;
				// Инициализация количества соответствующих данному PerosnId атрибутов.
				*person_id_index = 0;

				last_person_id = vTermAttrsList[attr_n]->GetInfPersonId();
			}

			// Сохраняем очередной набор аттрибутов текущего термина.
			memcpy( ptr, vTermAttrsList[attr_n]->vBuffer, TermsStorage::ATTRS_SIZE );
			ptr += TermsStorage::ATTRS_SIZE;
			// Обновление количества соответствующих данному PerosnId атрибутов.
			++( *person_id_index );
		}

		// Выравнивание в памяти.
		ptr += ( ptr - aBuffer ) & 3 ? 4 - ( ( ptr - aBuffer ) & 3 ) : 0;
	}

	// Проверка корректности сохранениея.
	aUsedMemory = ptr - aBuffer;
	if( aUsedMemory != memory )
		ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_SAVING_RESULT );

	return INF_ENGINE_SUCCESS;
}

void TermsStorageWR::Reset()
{
	// Сброс индекса терминов.
	vTermIndex.Reset();
	vTermIndex.Create();

	// Очистка списка атрибутов терминов.
	for( unsigned int list_n = 0; list_n < vAttrs.size(); ++list_n )
		if( vAttrs[list_n] )
			vAttrs[list_n]->free_buffer();
	vAttrs.clear();
}


InfEngineErrors TermsStorageRO::SearchAttrs( unsigned int aTermId, CommonIterator & aIterator ) const
{
	// Поиск термина.
	if( aTermId > GetTermsCount() )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Инициализируем итератор адресом, по которому хранится информация о заданом термине.
	aIterator.Set( vBuffer + reinterpret_cast<const uint32_t*>( vBuffer + sizeof( uint32_t ) )[aTermId] );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors TermsStorageRO::SearchAttrs( unsigned int aTermId, unsigned int aPersonId, PersonIdIterator & aIterator ) const
{
	// Поиск термина.
	if( aTermId > GetTermsCount() )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Инициализируем итератор адресом, по которому хранится информация о заданом термине.
	aIterator.Set( vBuffer + reinterpret_cast<const uint32_t*>( vBuffer + sizeof( uint32_t ) )[aTermId], aPersonId );

	return INF_ENGINE_SUCCESS;
}

const TermsStorage::TermAttrsRO* TermsStorageRO::CommonIterator::Next()
{
	// Проверка текущего состояния.
	if( !vBuffer || !vItem )
		return nullptr;

	// Следующий набор атрибутов.
	do
	{
		vItem = ( vItem - GetFirst() ) / TermsStorage::ATTRS_SIZE + 1 >= GetAttrsNumber() ? nullptr : vItem + TermsStorage::ATTRS_SIZE;

		if( !vItem )
			return nullptr;

		// Новый набор атрибутов.
		vCurrent.Init( vItem );

	} while( !TermIsValid( &vCurrent ) );

	return &vCurrent;
}

void TermsStorageRO::PersonIdIterator::Set( const char * aBuffer, unsigned int aPersonId )
{
	vBuffer = aBuffer;
	vPersonId = aPersonId;
	vPersonIdNum = static_cast<unsigned int>( -1 );
	vItem = GetFirst();

	// Если для заданного PersonId записей не найдено, то используем PersonId = 0;
	if( !vItem )
	{
		vPersonId = 0;
		vPersonIdNum = static_cast<unsigned int>( -1 );
		vItem = GetFirst();
	}

	if( vItem )
		vCurrent.Init( vItem );
}

const char* TermsStorageRO::PersonIdIterator::GetFirst()
{
	vFirst = nullptr;

	if( vPersonIdNum == static_cast<unsigned int>( -1 ) )
	{
		// Поиск в индексе по PersonId.
		uint32_t left = 0;
		uint32_t right = GetPersonIdNumber();
		uint32_t cur_n;
		const uint32_t * person_id_index;
		while( left < right )
		{
			cur_n = ( left + right ) >> 1;
			person_id_index = reinterpret_cast<const uint32_t*>( vBuffer + 2 * sizeof( uint32_t ) + cur_n * 3 * sizeof( uint32_t ) );
			if( vPersonId == person_id_index[0] )
				break;
			else if( vPersonId < person_id_index[0] )
				right = cur_n;
			else
				left = cur_n + 1;
		}

		if( left < right )
			vPersonIdNum = cur_n;
	}

	// Запись для заданного PersonId не найдена.
	if( vPersonIdNum == static_cast<unsigned int>( -1 ) )
		return nullptr;

	vFirst = vBuffer +
			 2 * sizeof( uint32_t ) +
			 GetPersonIdNumber() * 3 * sizeof( uint32_t ) +
			 reinterpret_cast<const uint32_t*>( vBuffer +
												2 * sizeof( uint32_t ) +
												vPersonIdNum * 3 * sizeof( uint32_t ) )[1] * TermsStorage::ATTRS_SIZE;

	return vFirst;
}

const TermsStorage::TermAttrsRO* TermsStorageRO::PersonIdIterator::BinarySearch( unsigned int aId, unsigned int aSubId )
{
	if( IsEnd() )
		return nullptr;

	// Поиск начинается с текущего набора атрибутов.
	unsigned int left = ( vItem - vFirst ) / TermsStorage::ATTRS_SIZE;
	// Поиск идёт внутри набора атрибутов, соответствующего заданному PersonId.
	unsigned int right = reinterpret_cast<const uint32_t*>( vBuffer + 2 * sizeof( uint32_t ) + vPersonIdNum * 3 * sizeof( uint32_t ) )[2];

	static const char * item;
	static TermsStorage::TermAttrsRO current;
	static unsigned int cur;

	// Двоичный поиск атрибутов с заданными Id и SubId.
	while( left < right )
	{
		cur = ( left + right ) >> 1;
		item = vBuffer + 2 * sizeof( uint32_t ) + GetPersonIdNumber() * 3 * sizeof( uint32_t ) +
			   reinterpret_cast<const uint32_t*>( vBuffer +
												  2 * sizeof( uint32_t ) +
												  vPersonIdNum * 3 * sizeof( uint32_t ) )[1] * TermsStorage::ATTRS_SIZE +
			   cur * TermsStorage::ATTRS_SIZE;
		current.Init( item );

		if( current.GetId() == aId )
		{
			if( current.GetSubId() == aSubId )
			{
				// Найден один из наборов атрибутов с заданными Id и SubId.
				// Поиск самого первого набора атрибутов с заданными Id и SubId.
				while( cur > left )
				{
					item = vBuffer + 2 * sizeof( uint32_t ) + GetPersonIdNumber() * 3 * sizeof( uint32_t ) +
						   reinterpret_cast<const uint32_t*>( vBuffer +
															  2 * sizeof( uint32_t ) +
															  vPersonIdNum * 3 * sizeof( uint32_t ) )[1] * TermsStorage::ATTRS_SIZE +
						   ( cur - 1 ) * TermsStorage::ATTRS_SIZE;
					current.Init( item );

					// Переход к предыдущему набору атрибутов с заданными Id и SubId.
					if( current.GetSubId() == aSubId && current.GetId() == aId )
						--cur;
					else
						break;
				}
				// Найден самый первый набор атрибутов с заданными Id и SubId.
				vItem = vBuffer + 2 * sizeof( uint32_t ) + GetPersonIdNumber() * 3 * sizeof( uint32_t ) +
						reinterpret_cast<const uint32_t*>( vBuffer +
														   2 * sizeof( uint32_t ) +
														   vPersonIdNum * 3 * sizeof( uint32_t ) )[1] * TermsStorage::ATTRS_SIZE +
						cur * TermsStorage::ATTRS_SIZE;
				vCurrent.Init( vItem );

				return &vCurrent;
			}
			else if( current.GetSubId() < aSubId )
				left = cur + 1;
			else
				right = cur;
		}
		else if( current.GetId() < aId )
			left = cur + 1;
		else
			right = cur;
	}

	vItem = nullptr;
	return nullptr;
}

const TermsStorage::TermAttrsRO* TermsStorageRO::PersonIdIterator::Next()
{
	// Проверка текущего состояния.
	if( !vBuffer || !vItem )
		return nullptr;

	// Следующий набор атрибутов.
	vItem = ( vItem - vFirst ) / TermsStorage::ATTRS_SIZE + 1 >= GetAttrsNumber() ? nullptr : vItem + TermsStorage::ATTRS_SIZE;

	if( vItem )
	{
		// Атрибуты, относящиеся к заданному PersonId закончились.
		if( vPersonId != 0 )
		{
			TermsStorage::TermAttrsRO next( vItem );
			if( vCurrent.GetInfPersonId() != next.GetInfPersonId() )
			{
				// Далее используем PersonId = 0.
				vPersonId = 0;
				vPersonIdNum = static_cast<unsigned int>( -1 );

				vItem = GetFirst();
			}
		}
	}
	else
	{
		// Атрибуты, относящиеся к заданному PersonId закончились.
		if( vPersonId != 0 )
		{
			// Далее используем PersonId = 0.
			vPersonId = 0;
			vPersonIdNum = static_cast<unsigned int>( -1 );
			vItem = GetFirst();
		}
	}

	if( !vItem )
		return nullptr;

	// Новый набор атрибутов.
	vCurrent.Init( vItem );

	return &vCurrent;
}

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
*
*      class DictTermsFilter
*
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/


bool DictTermsFilter::IsAccepted( const TermsStorage::TermAttrsRO * aTermAttrsRO, const NDocImage::TermCoords & aCoords ) const
{
	unsigned int dict_id = aTermAttrsRO->GetId();
	NDocImage::Iterator first = aCoords.vFirstPos;
	if( !first.IsHead() )
		--first;
	TermsDocImageIterator last = aCoords.vLastPos;
	if( !last.IsEnd() )
		++last;

	return dict_id < vFilterMap.size()  ? vFilterMap[dict_id] && first.IsHead() && last.IsEnd()  : false;
}

InfEngineErrors DictTermsFilter::AddAcceptableDictId( unsigned int aDictId )
{
	vDictIDToAccept.push_back( aDictId );
	if( vDictIDToAccept.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	if( aDictId >= vFilterMap.size() )
	{
		unsigned int prev_size = vFilterMap.size();

		vFilterMap.resize( aDictId + 1 );
		if( vFilterMap.no_memory() )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;

		memset( vFilterMap.get_buffer() + prev_size, 0, aDictId + 1 - prev_size );
	}

	vFilterMap[aDictId] = true;

	return INF_ENGINE_SUCCESS;
}

void DictTermsFilter::Reset()
{
	for( size_t n = 0; n < vDictIDToAccept.size(); ++n )
		vFilterMap[vDictIDToAccept[n]] = false;

	vDictIDToAccept.clear();
}
