#include "IndexBase.hpp"

#include <algorithm>

#include "Terms/InfTermAttrs.hpp"

IndexBaseWR::IndexBaseWR( InfLingProc *& aLp, fstorage_section_id aSectionId, nMemoryAllocator & aMemoryAllocator ) :
	vLp( aLp ), vTBase( aSectionId ), vTermsStorageWR( aMemoryAllocator ), vExtSectionID( aSectionId+TERMS_SECTION_MAX_NUMBER ) {}

InfEngineErrors IndexBaseWR::Create()
{
	// Уничтожение индексной базы, если она была создана ранее.
	Destroy();

	// Создание базы терминов.
	enum FltTermsErrors ftres = vTBase.Create( vLp, TERMS_BASE_FLAGS_MAP_ERANGE_WARNING );
	if( ftres != TERMS_OK )
	{
		Destroy();

		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create FltTermsBase. Return code: %d", ftres );
	}

	// Инициализация таблицы стоп-символов.
	static bool init_stop_dict_once = false;
	if( !init_stop_dict_once )
	{
		init_stop_dict_once = true;
		if( TERMS_OK != ( ftres = vStopDict.init( *vLp ) ) )
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't initialize StopDict. Return code: %d", ftres );
	}

	return INF_ENGINE_SUCCESS;
}

void IndexBaseWR::Destroy()
{
	// Очистка базы терминов.
	vTBase.Close();

	// Очистка базы атрибутов терминов.
	vTermsStorageWR.Reset();

	// Очистка массива индексов безтерминных шаблонов.
	vIDWT.clear();
}

bool idwt_cmp( const IndexBaseWR::IDWT & a, const IndexBaseWR::IDWT & b )
{
	if( a.vInfPersonId == b.vInfPersonId )
	{
		if( a.vId == b.vId )
			return a.vSubId < b.vSubId;
		return a.vId < b.vId;
	}
	else
		return a.vInfPersonId < b.vInfPersonId;
}

InfEngineErrors IndexBaseWR::Save( fstorage * aFStorage )
{
	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Сохранение базы терминов.
	struct cf_terms_save_params params;
	memset( &params, 0, sizeof( cf_terms_save_params ) );
	enum FltTermsErrors ftres =  vTBase.Save( &params, aFStorage );
	if( ftres != TERMS_OK )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't save FltTermsBase. Return code: %d", ftres );

	fstorage_section* sectdata = fstorage_get_section( aFStorage, vExtSectionID );
	if( !sectdata )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Сохранение индекса безтерминных шаблонов.
	unsigned int memory = sizeof( uint32_t )*2 + vIDWT.size() * 3 * sizeof(uint32_t) + vTermsStorageWR.GetNeedMemorySize();

	int ret = fstorage_section_realloc( sectdata, memory );
	if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	char* base_ptr = static_cast<char*>(fstorage_section_get_all_data( sectdata ));
	if( !base_ptr )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

	// Сохранение размера секции.
	char* ptr = base_ptr;
	*((uint32_t*)ptr) = memory;
	ptr += sizeof( uint32_t );

	// Сортрировка безтерминных шаблонов по InfPersonId.
	std::sort( vIDWT.get_buffer(), vIDWT.get_buffer() + vIDWT.size(), idwt_cmp  );

	// Сохранение количества безтерминных шаблонов.
	*((uint32_t*)ptr) = vIDWT.size();
	ptr += sizeof( uint32_t );

	// Сохранение индекса.
	for( unsigned int idwt_n = 0; idwt_n < vIDWT.size(); ++idwt_n )
	{
		reinterpret_cast<uint32_t*>(ptr)[0] = vIDWT[idwt_n].vId;
		reinterpret_cast<uint32_t*>(ptr)[1] = vIDWT[idwt_n].vSubId;
		reinterpret_cast<uint32_t*>(ptr)[2] = vIDWT[idwt_n].vInfPersonId;

		ptr += 3 * sizeof(uint32_t);
	}

	// Сохранение базы атрибутов терминов.
	unsigned int used_memory = 0;
	InfEngineErrors iee = vTermsStorageWR.Save( ptr, memory - ( ptr - base_ptr ), used_memory );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );
	ptr += used_memory;

	// Проверка чексуммы.
	if( ptr - base_ptr != memory )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "%i bytes instead %i", ptr - base_ptr, memory );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors IndexBaseWR::PreparePatternString( InfPatternItems::QuestionWR & aPatternString ) const
{
	unsigned int terms_weight = 0;

	// Проставление флага StopWordsOnly для текстовых элементов и вычисление их веса.
	for( unsigned int pos = 0; pos < aPatternString.GetItemsNumber(); pos++ )
	{
		if( InfPatternItems::itText == aPatternString.GetItemType( pos ) )
		{
			// Получение текстового элемента.
			InfPatternItems::Text* text = static_cast<InfPatternItems::Text*>(aPatternString.GetItem( pos ));

			const char * normalized = text->GetNormalizedTextString();

			vTmpDocImage.Reset();

			// Подготовка текста.
			int ret = vTmpDocMessage.SetMessage( nullptr, 0, normalized, strlen( normalized ), true );
			if( ret != 0 )
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );
			vTmpDocMessage.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );

			// Лемматизация текста.
			LingProcErrors lpe = vLp->LemmatizeDoc( vTmpDocMessage, vTmpDocImage );
			if( lpe != LP_OK )
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't lemmatize DocImage. Return Code: %d", lpe );

			// Если текст состоит только из стоп-слов, отмечаем это.
			text->SetStopWordsOnly( true );
			for( unsigned int i = 0; i < vTmpDocImage.Size(); i++ )
				if( !vStopDict.isStop( vTmpDocImage.GetLex( i ) ) )
					text->SetStopWordsOnly( false );

			// Подсчёт количества слов в тексте, не считая стоп-символы.
			NDocImage ndoc_image;
			TermsDocImageIterator iter( vTmpDocImage, &vStopDict, false );
			InfEngineErrors iee = ndoc_image.Init( iter );
			if( iee != INF_ENGINE_SUCCESS )
			{
				if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				else
					ReturnWithError( iee, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
			if( ndoc_image.Size() > 2 )
				terms_weight += 2 * ( ndoc_image.Size() - 2 );
		}
		else if( InfPatternItems::itDictInline == aPatternString.GetItemType( pos ) )
		{
			// Получение inline-словаря.
			InfPatternItems::TagDictInline * item = static_cast<InfPatternItems::TagDictInline*>(aPatternString.GetItem( pos ));

			// Поиск в inline-словаре варианта с наибольшим весом.
			unsigned int max_weight = 0;

			for( unsigned int j = 0; j < item->GetNormalizedStringsNumber( ); j++ )
			{
				unsigned int term_id;
				bool term_is_new = false;

				// Подготовка текста.
				int ret = vTmpDocMessage.SetMessage( nullptr, 0, item->GetNormalizedString( j ), item->GetNormalizedStringLength( j ), true );
				if( ret != 0 )
					ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );
				vTmpDocMessage.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );

				// Лемматизация текста.
				LingProcErrors lpe = vLp->LemmatizeDoc( vTmpDocMessage, vTmpDocImage );
				if( lpe != LP_OK )
					ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't lemmatize DocImage. Return Code: %d", lpe );

				// Подсчёт количества слов в тексте, не считая стоп-символы.
				NDocImage ndoc_image;
				TermsDocImageIterator iter( vTmpDocImage, &vStopDict, false );
				InfEngineErrors iee = ndoc_image.Init( iter );
				if( iee != INF_ENGINE_SUCCESS )
				{
					if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					else
						ReturnWithError( iee, INF_ENGINE_STRING_ERROR_INTERNAL );
				}
				if( ndoc_image.Size() > 2 )
					max_weight = std::max( max_weight, 2 * ( ndoc_image.Size() - 2 ) );
			}
			terms_weight += max_weight;
		}
	}

	aPatternString.SetTermsWeight( terms_weight );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors IndexBaseWR::AddPatternString( const InfPatternItems::QuestionWR & aQst, unsigned int aId, unsigned int aSubId, unsigned int aInfPersonId )
{
	// Очистка аттрибутов.
	vAttrs.clear();
	vTermPos.clear();

	bool hasrealterm = false;
	bool hasnotrealterm = false;
	unsigned int indexpos = 0;

	// Проход по всем элементам строки шаблона.
	for( unsigned int pos = 0; pos < aQst.GetItemsNumber(); pos++ )
	{
		// Поиск текстового термина или inline-словаря.
		unsigned int superstars = 0;
		bool process = false;
		bool dict = false;
		bool stars = false;
		bool realterm = false;
		for(; pos < aQst.GetItemsNumber(); pos++ )
		{
			InfPatternItems::ItemType type = aQst.GetItemType( pos );
			if( type == InfPatternItems::itText )
			{
				// Текстовый элемент.
				realterm = !static_cast<InfPatternItems::Text*>(aQst.GetItem( pos ))->IfStopWordsOnly();
				break;
			}
			else if( type == InfPatternItems::itDict )
			{
				// Словарь сбрасывает флаг звездочки и поднимает флаг обязательного процессирования.
				process = true;
				dict = true;
				hasnotrealterm = true;
			}
			else if( type == InfPatternItems::itStar || type == InfPatternItems::itEliStar )
			{
				// Подсчет звездочек.
				stars = true;
				if( static_cast<InfPatternItems::Star*>(aQst.GetItem( pos ))->IsSuperStar() )
					superstars++;
				hasnotrealterm = true;
			}
			else if( type == InfPatternItems::itDictInline )
			{
				// Найдет inline-словарь.
				realterm = !static_cast<InfPatternItems::TagDictInline*>(aQst.GetItem( pos ))->HasEmptyVariant();
				hasnotrealterm |= !realterm;
				break;
			}
			else
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Invalid pattern-question's item. ItemType: ", type );
		}

		if( pos >= aQst.GetItemsNumber() )
		{
			// Термин не найден.

			// Корректировка найденных ранее терминов.
			if( !vAttrs.empty() )
			{
				// Флаг обязательного процессирования.
				if( process )
					vAttrs.back().SetFlagProcessAfter();

				// Звёздочки и суперзвёздочки после последнего термина.
				if( stars )
				{
					vAttrs.back().SetFlagStarAfter();
					vAttrs.back().SetStarShiftAfter( superstars );
				}
			}

			// Простановка флага последнего термина.
			for( unsigned int ind = vAttrs.size(); ind > 0; ind-- )
			{
				TermsStorage::TermAttrs & term = vAttrs[ind-1];
				term.SetFlagLastTerm();
				if( dict )
					term.SetFlagDictAfter();
				if( term.GetFlagRealTerm() )
					break;
			}

			// Выход из цикла поиска терминов.
			continue;
		}

		// Установка флага наличия реального термина в шаблоне.
		if( realterm )
			hasrealterm = true;

		// Выделение памяти под атрибуты найденного термина.
		TermsStorage::TermAttrs * attr = vAttrs.grow();
		if( !attr )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		attr->Reset();

		// Получение ссылки на атрибуты предыдущего термина.
		TermsStorage::TermAttrs * prev = vAttrs.size() > 1 ? &vAttrs[vAttrs.size()-2] : nullptr;

		vTermPos.push_back( pos );
		if( vTermPos.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Простановка идентификаторов.
		attr->SetId( aId );
		attr->SetSubId( aSubId );
		attr->SetPos( pos );
		// Проставление флага текстового термина.
		if( realterm )
		{
			attr->SetFlagRealTerm();
			attr->SetRealTermPos( indexpos );
			indexpos++;
		}
		else
			attr->SetRealTermPos( indexpos );

		// Простановка флага последнего термина.
		if( pos == aQst.GetItemsNumber()-1 )
		{
			for( unsigned int ind = vAttrs.size(); ind > 0; ind-- )
			{
				TermsStorage::TermAttrs & term = vAttrs[ind-1];
				term.SetFlagLastTerm();
				if( term.GetFlagRealTerm() )
					break;
			}
		}

		// Звёздочки и суперзвёздочки перед термином.
		if( stars )
		{
			attr->SetFlagStarBefore();
			attr->SetStarShiftBefore( superstars );

			// Звёздочки и суперзвёздочки после предыдущего термина.
			if( prev )
			{
				prev->SetFlagStarAfter();
				prev->SetStarShiftAfter( superstars );
			}
		}

		// Флага обязательного процессирования перед термином.
		if( process )
		{
			attr->SetFlagProcessBefore();
			// Флаг обязательного процессирования после предыдущего термина.
			if( prev )
				prev->SetFlagProcessAfter();
		}

		if( dict )
		{
			dict = false;
			attr->SetFlagDictBefore();
			if( prev )
				prev->SetFlagDictAfter();
		}


		// Проставление флага первого термина.
		if( !prev || ( prev->GetFlagFirstTerm() && !prev->GetFlagRealTerm() ) )
			attr->SetFlagFirstTerm();

		// Проставление флага пустого inline-словаря перед термином.
		if( pos &&
			( ( aQst.GetItemType( pos - 1 ) == InfPatternItems::itDictInline &&
				static_cast<InfPatternItems::TagDictInline*>(aQst.GetItem( pos - 1 ))->HasEmptyVariant() ) ||
			  ( aQst.GetItemType( pos - 1 ) == InfPatternItems::itDict &&
				static_cast<InfPatternItems::TagDict*>(aQst.GetItem( pos - 1 ))->HasEmptyVariant() ) ) )
			attr->SetFlagEmptyDictBefore();

		// Проставление флага пустого inline-словаря после термина.
		if( vAttrs.size() > 1 &&
			( ( aQst.GetItemType( pos ) == InfPatternItems::itDictInline &&
				(static_cast<InfPatternItems::TagDictInline*>(aQst.GetItem( pos )))->HasEmptyVariant() ) ||
			  ( aQst.GetItemType( pos ) == InfPatternItems::itDict &&
				(static_cast<InfPatternItems::TagDict*>(aQst.GetItem( pos )))->HasEmptyVariant() ) ) )
			vAttrs[vAttrs.size()-2].SetFlagEmptyDictAfter();

		// Проставление InfPerson.
		attr->SetInfPersonId( aInfPersonId );
	}

	// Добавление терминов в поисковую базу.
	if( !vAttrs.empty() )
	{
		// Установка атрибута "количества обязательных терминов после текущего".
		unsigned int rt_cnt = 0;
		for( unsigned int i = vAttrs.size(); i > 0; --i )
		{
			vAttrs[i-1].SetRealTermsAfter( rt_cnt );
			if( vAttrs[i-1].GetFlagRealTerm() )
				++rt_cnt;
		}

		for( unsigned int i = 0; i < vAttrs.size(); ++i )
		{
			TermsStorage::TermAttrs & attr = vAttrs[i];

			unsigned int pos = vTermPos[i];

			if( aQst.GetItemType( pos ) == InfPatternItems::itText )
			{
				// Получение текстового элемента.
				InfPatternItems::Text* text = static_cast<InfPatternItems::Text*>(aQst.GetItem( pos ));

				// Индексируем текст, только если в нём есть что-то кроме стоп-символов.
				if( !text->IfStopWordsOnly() )
				{
					unsigned int term_id;
					bool term_is_new = false;
					InfEngineErrors iee = vTermsStorageWR.AddTermAttrs( text->GetNormalizedTextString( ), text->GetNormalizedTextLength( ), attr, term_id, term_is_new );
					if( INF_ENGINE_SUCCESS != iee )
						ReturnWithTrace( iee );

					if( term_is_new )
					{
						InfTermAttrs term_attrs;
						term_attrs.SetId( term_id );
						enum FltTermsErrors ftres = vTBase.Add( &term_attrs.raw( ), text->GetNormalizedTextString( ) );
						if( ftres > 0 /* Warning */ )
						{
							return INF_ENGINE_ERROR_CANT_PARSE;
						}
						else if( ftres != TERMS_OK )
							ReturnWithError( INF_ENGINE_ERROR_FAULT,
											 "Can't add pattern term. Return Code: %d", ftres );
					}
				}
			}
			else if( aQst.GetItemType( pos ) == InfPatternItems::itDictInline )
			{
				// Получение inline-словаря.
				InfPatternItems::TagDictInline* item = static_cast<InfPatternItems::TagDictInline*>(aQst.GetItem( pos ));

				for( unsigned int j = 0; j < item->GetNormalizedStringsNumber( ); j++ )
				{
					unsigned int term_id;
					bool term_is_new = false;
					InfEngineErrors iee = vTermsStorageWR.AddTermAttrs( item->GetNormalizedString( j ),item->GetNormalizedStringLength( j ), attr, term_id, term_is_new );
					if( INF_ENGINE_SUCCESS != iee )
						ReturnWithTrace( iee );

					if( term_is_new )
					{
						InfTermAttrs term_attrs;
						term_attrs.SetId( term_id );
						enum FltTermsErrors ftres = vTBase.Add( &term_attrs.raw( ), item->GetNormalizedString( j ) );
						if( ftres > 0 )
							continue;
						else if( ftres != TERMS_OK )
							ReturnWithError( INF_ENGINE_ERROR_FAULT,
											 "Can't add pattern term. Return Code: %d", ftres );
					}
				}
			}
		}
	}

	// Обработка шаблонов без терминов и шаблонов содержащих только виртуальные термины.
	if( hasnotrealterm && ( vAttrs.empty() || !hasrealterm ) )
	{
		vIDWT.push_back( IndexBaseWR::IDWT( aId, aSubId, aInfPersonId ) );
		if( vIDWT.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	return INF_ENGINE_SUCCESS;
}

IndexBaseRO::IndexBaseRO( InfLingProc *& aLp, fstorage_section_id aSectionId ) :
	vLp( aLp ), vTBase( aSectionId ), vSectionId( aSectionId ), vExtSectionId( aSectionId+TERMS_SECTION_MAX_NUMBER ) {}

InfEngineErrors IndexBaseRO::Open( fstorage * aFStorage )
{
	// Проверка агрументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Очистка данных.
	Close();

	// Загрузка базы терминов.
	enum FltTermsErrors ftres = vTBase.Open( vLp, aFStorage, TERM_BASE_OPEN_RO_SHARED );
	if( ftres != TERMS_OK )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open FltTermsBase. Return Code: %d", ftres );
	}

	// Загрузка безтерминных шаблонов.
	fstorage_section* sectdata = fstorage_get_section( aFStorage, vExtSectionId );
	if( !sectdata )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );
	}

	// Получение указателя на память.
	const char * base_ptr = static_cast<const char *>(fstorage_section_get_all_data( sectdata ));
	if( !base_ptr )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get fstorage pointer." );
	}
	vIDWT = base_ptr;

	const char * term_attrs_base_ptr = reinterpret_cast<const char *>(base_ptr) + 2 * sizeof (uint32_t ) + 3 * reinterpret_cast<const uint32_t*>(base_ptr)[1] * sizeof(uint32_t);
	vTermsStorageRO.Open( term_attrs_base_ptr );

	// Загрузка таблицы стоп-символов.
	vStopDict.clear();
	StopDictEx::Status sdes = vStopDict.load( aFStorage, vSectionId + TERMS_SECTION_OFS_STOP_EX );
	if( StopDictEx::OK !=  sdes )
	{
		Close();
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't load stop table. Error %u", sdes );
	}

	vState = stReady;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors IndexBaseRO::Check( const DocImage& aRequest, FltTermsMatcher& aMatcher )
{
	// Проверка готовности базы для поиска.
	if( !IsReady() )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Поиск терминов в индексной базе.
	FltTermsErrors fte = vTBase.Check( &aMatcher, &aRequest );
	if( fte != TERMS_OK )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't check terms. Return Code: %d", fte );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors IndexBaseRO::LemmatizeDoc( DocText& aDocText, DocImage& aDocImage )
{
	// Лемматизация.
	LingProcErrors lpe = vLp->LemmatizeDoc( aDocText, aDocImage );
	if( lpe != LP_OK )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't lemmatize DocImage. Return Code: %d", lpe );

	return INF_ENGINE_SUCCESS;
}

void IndexBaseRO::Close()
{
	vTBase.Close();

	vIDWT = nullptr;

	vState = stNone;
}

InfIndexBaseRO::InfIndexBaseRO() :
	vIndexDBase( vLp, FSTORAGE_SECTION_INDEXER_INF_DICT ), vIndexQBase( vLp, FSTORAGE_SECTION_INDEXER_INF_QST ),
    vIndexTBase( vLp, FSTORAGE_SECTION_INDEXER_INF_THAT ), vSymbolymBase( FSTORAGE_SECTION_INDEXER_INF_SYMBOLYMS ),
    vSynonymBase( vLp, FSTORAGE_SECTION_INDEXER_INF_SYNONYMS ) {}

InfEngineErrors InfIndexBaseRO::Open( fstorage* aFStorage )
{
	// Проверка агрументов.

	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Закрытие индексной базы, если она было открыта.
	Close();

	// Открытие лингвистического процессора.
	vLp = new InfLingProc;
	if( vLp == nullptr )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	enum LingProcErrors lpres = vLp->Open( LP_MODE_RO, aFStorage );
	if( lpres != LP_OK )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open LingProc. Return Code: %d", lpres );
	}

	// Открытие индексных баз.
    InfEngineErrors iee;
    if( ( ( iee = vIndexDBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
            ( ( iee = vIndexQBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
            ( ( iee = vIndexTBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
            ( ( iee = vSymbolymBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
            ( ( iee = vSynonymBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) )
    {
        Close();

        ReturnWithTrace( iee );
    }

	return INF_ENGINE_SUCCESS;
}

void InfIndexBaseRO::Close()
{
	// Закрытие индексных баз.
	vIndexDBase.Close();
	vIndexQBase.Close();
	vIndexTBase.Close();
    vSymbolymBase.Close();
	vSynonymBase.Close( );

	// Закрытие лингвистического процессора.
	if( vLp )
	{
		vLp->Close();
		delete vLp;
	}
	vLp = nullptr;
}

InfEngineErrors InfIndexBaseRO::LemmatizeDoc( DocText& aDocText, DocImage& aDocImage ) const
{
	// Лемматизация.
	LingProcErrors lpe = vLp->LemmatizeDoc( aDocText, aDocImage );
	if( lpe != LP_OK )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't lemmatize DocImage. Return Code: %d", lpe );

	return INF_ENGINE_SUCCESS;
}

EllipsisIndexBaseRO::EllipsisIndexBaseRO() :
	vIndexFBase( vLp, FSTORAGE_SECTION_INDEXER_ELLIPSIS_FIRST ), vIndexSBase( vLp, FSTORAGE_SECTION_INDEXER_ELLIPSIS_SECOND ),
	vIndexTBase( vLp, FSTORAGE_SECTION_INDEXER_ELLIPSIS_THIRD ), vIndexDBase( vLp, FSTORAGE_SECTION_INDEXER_ELLIPSIS_DICT ),
	vSynonymBase( vLp, FSTORAGE_SECTION_INDEXER_INF_SYNONYMS ) {}

InfEngineErrors EllipsisIndexBaseRO::Open( fstorage* aFStorage )
{
	// Проверка агрументов.

	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Закрытие индексной базы, если она было открыта.
	Close();

	// Открытие лингвистического процессора.
	vLp = new InfLingProc;
	if( !vLp )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	enum LingProcErrors lpres = vLp->Open( LP_MODE_RO, aFStorage );
	if( lpres != LP_OK )
	{
		Close();

		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open LingProc. Return Code: %d", lpres );
	}

	// Открытие индексных баз.
	InfEngineErrors iee;
	if( ( ( iee = vIndexFBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vIndexSBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vIndexTBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vIndexDBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vSynonymBase.Open( aFStorage ) ) != INF_ENGINE_SUCCESS ) )
	{
		Close();

		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

void EllipsisIndexBaseRO::Close()
{
	// Закрытие индексных баз.
	vIndexFBase.Close();
	vIndexSBase.Close();
	vIndexTBase.Close();
	vIndexDBase.Close();
	vSynonymBase.Close( );

	// Закрытие лингвистического процессора.
	if( vLp )
	{
		vLp->Close();
		delete vLp;
	}
	vLp = nullptr;
}
