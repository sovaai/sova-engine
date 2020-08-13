#include "Indexer.hpp"

#include <InfEngine2/InfEngine/Terms/InfTermAttrs.hpp>

BaseIndexer::BaseIndexer( fstorage_section_id aSectionId, nMemoryAllocator & aAllocator ):
	vBaseDict { aSectionId }, vDictTermsStorage { aAllocator },
vSynonymBaseWR { vLp, FSTORAGE_SECTION_INDEXER_INF_SYNONYMS },
vSynonymBaseRO { vLp, FSTORAGE_SECTION_INDEXER_INF_SYNONYMS },
vSymbolymBaseWR { FSTORAGE_SECTION_INDEXER_INF_SYMBOLYMS },
vSymbolymBaseRO { FSTORAGE_SECTION_INDEXER_INF_SYMBOLYMS }
{
	vSectionId = aSectionId;
}

InfEngineErrors BaseIndexer::Create()
{
	// Проверка статуса.
	if( vState != State::None )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_RECREATE );

	// Создание нового пустого лингвистического процессора.
	vLp = new InfLingProc;
	if( vLp == nullptr )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	enum LingProcErrors lpres = vLp->Create();
	if( lpres != LP_OK )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create LingProc. Return code: %d", lpres );

	// Создание базы терминов для соварей.
	enum FltTermsErrors ftres = vBaseDict.Create( vLp, TERMS_BASE_FLAGS_MAP_ERANGE_WARNING );
	if( ftres != TERMS_OK )
	{
		Destroy();

		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create FltTermsBase. Return code: %d", ftres );
	}

	// Создание базы синонимов.
	InfEngineErrors iee = vSynonymBaseWR.Create();
	if( iee != INF_ENGINE_SUCCESS )
	{
		Destroy();
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create SynonymBase. Return code: %d", iee );
	}
	
	// Создание базы заменяемых символов.
    if( INF_ENGINE_SUCCESS != ( iee = vSymbolymBaseWR.Create() ) )
	{
		Destroy();
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create SynonymBase. Return code: %d", iee );
	}

	return INF_ENGINE_SUCCESS;
}

void BaseIndexer::Destroy()
{
	// Очистка базы терминов.
	vBaseDict.Close();

	// Очистка базы терминов словарей.
	vDictTermsStorage.Reset();
		
	// Сброс базы синонимов.
	vSynonymBaseWR.Reset();
	
	// Очистка базы заменяемых символов.
	vSymbolymBaseWR.Reset();

	// Очистка лингвистического процессора.
	if( vLp )
	{
		vLp->Close();
		delete vLp;
	}
	vLp = nullptr;

	if( vFscmn_tmp )
	{
		fstorage_close( vFscmn_tmp );
		fstorage_destroy( vFscmn_tmp );
		vFscmn_tmp = nullptr;
	}

	vState = State::None;
}

InfEngineErrors BaseIndexer::FinalizeSynonyms()
{
	// Открытие fstorage.
	vFscmn_tmp = fstorage_create();
	if( !vFscmn_tmp )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't create fstorage." );

	fstorage_connect_config fscc;
	memset( &fscc, 0, sizeof( fstorage_connect_config ) );
	fscc.pid = FSTORAGE_PID_SYNONYMS_TMP;
	int ret = fstorage_trx_snapshot( vFscmn_tmp, FSTORAGE_TRX_READ_WRITE );
	if( ret != 0 )
	{
		fstorage_destroy( vFscmn_tmp );
		vFscmn_tmp = nullptr;

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't create RW snapshot. Return code: %d", ret );
	}

	// Сохранение временной базы синонимов.
	InfEngineErrors iee = vSynonymBaseWR.Save( vFscmn_tmp, true );
	if( iee != INF_ENGINE_SUCCESS )
	{
		fstorage_close( vFscmn_tmp );
		fstorage_destroy( vFscmn_tmp );
		vFscmn_tmp = nullptr;

		ReturnWithTrace( iee );
	}

	ret = fstorage_trx_snapshot( vFscmn_tmp, FSTORAGE_TRX_READ_ONLY );
	if( ret != 0 )
	{
		fstorage_close( vFscmn_tmp );
		fstorage_destroy( vFscmn_tmp );
		vFscmn_tmp = nullptr;

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't create RO snapshot. Return code: %d", ret );
	}

	if( INF_ENGINE_SUCCESS != ( iee = vSynonymBaseRO.Open( vFscmn_tmp ) ) )
	{
		fstorage_close( vFscmn_tmp );
		fstorage_destroy( vFscmn_tmp );
		vFscmn_tmp = nullptr;

		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BaseIndexer::FinalizeSymbolyms()
{
	// Открытие fstorage.
	vFscmn_tmp = fstorage_create();
	if( !vFscmn_tmp )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't create fstorage." );

	fstorage_connect_config fscc;
	memset( &fscc, 0, sizeof( fstorage_connect_config ) );
	fscc.pid = FSTORAGE_PID_SYMBOLYMS_TMP;
	int ret = fstorage_trx_snapshot( vFscmn_tmp, FSTORAGE_TRX_READ_WRITE );
	if( ret != 0 )
	{
		fstorage_destroy( vFscmn_tmp );
		vFscmn_tmp = nullptr;

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't create RW snapshot. Return code: %d", ret );
	}

	// Сохранение временной базы заменяемых символов.
    InfEngineErrors iee = vSymbolymBaseWR.Save( vFscmn_tmp );
	if( iee != INF_ENGINE_SUCCESS )
	{
		fstorage_close( vFscmn_tmp );
		fstorage_destroy( vFscmn_tmp );
		vFscmn_tmp = nullptr;

		ReturnWithTrace( iee );
	}

	ret = fstorage_trx_snapshot( vFscmn_tmp, FSTORAGE_TRX_READ_ONLY );
	if( ret != 0 )
	{
		fstorage_close( vFscmn_tmp );
		fstorage_destroy( vFscmn_tmp );
		vFscmn_tmp = nullptr;

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't create RO snapshot. Return code: %d", ret );
	}

	if( INF_ENGINE_SUCCESS != ( iee = vSymbolymBaseRO.Open( vFscmn_tmp ) ) )
	{
		fstorage_close( vFscmn_tmp );
		fstorage_destroy( vFscmn_tmp );
		vFscmn_tmp = nullptr;

		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BaseIndexer::BaseSave( fstorage * aFStorage )
{
	// Проверка статуса.
	if( vState != State::Ready )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "Indexer has not been created." );

	// Проверка параметров.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Сохранение лингвистического процессора.
	enum LingProcErrors lpres =  vLp->Save( aFStorage, false );
	if( lpres != LP_OK )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't save LingProc. Return code: %d", lpres );


	// Сохранение базы терминов словарей.
	struct cf_terms_save_params params;
	memset( &params, 0, sizeof( cf_terms_save_params ) );
	enum FltTermsErrors ftres =  vBaseDict.Save( &params, aFStorage );
	if( ftres != TERMS_OK )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't save FltTermsBase. Return code: %d", ftres );

	// @todo Нужно перевести словари на IndexBase.
	fstorage_section * sectdata = fstorage_get_section( aFStorage, vSectionId + TERMS_SECTION_MAX_NUMBER );
	if( !sectdata )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	unsigned int memory = sizeof( uint32_t ) * 2 + vDictTermsStorage.GetNeedMemorySize();

	int ret = fstorage_section_realloc( sectdata, memory );
	if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	char * base_ptr = static_cast<char*>( fstorage_section_get_all_data( sectdata ) );
	if( !base_ptr )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

	( (uint32_t*)base_ptr )[0] = memory;
	( (uint32_t*)base_ptr )[1] = 0;

	// База терминов словарей.
	unsigned int used = 0;
	InfEngineErrors iee = vDictTermsStorage.Save( base_ptr + 2 * sizeof( uint32_t ), memory - 2 * sizeof( uint32_t ), used );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	// Сохранение базы синонимов.
	if( INF_ENGINE_SUCCESS != ( iee = vSynonymBaseWR.Save( aFStorage ) ) )
		ReturnWithTrace( iee );
	
	// Сохранение базы заменяемых символов.
	if( INF_ENGINE_SUCCESS != ( iee = vSymbolymBaseWR.Save( aFStorage ) ) )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BaseIndexer::AddDict( const InfDict * aDict, const PatternsStorage & aDictPatternsStorage )
{
	// Проверка статуса.
	if( vState != State::Ready )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "Indexer is not created." );

	TermsStorage::TermAttrs * attrs = nullptr;
	static avector<TermsStorage::TermAttrs> attrs_list;

	for( size_t i = 0; i < aDict->GetStringsNum(); i++ )
	{
		attrs_list.clear();

		const PatternsStorage::Pattern items = aDictPatternsStorage.GetPatternString( aDict->GetStringID( i ) );

		unsigned int counter = 0;
		unsigned int sstar_before_num = 0;
		unsigned int sstar_after_num = 0;
		bool star_before = false;
		bool star_after = false;
		bool first_term = true;
		bool is_real_term = true;

		// Поиск первого текстового элемента.
		size_t j = 0;
		for( j = 0; j < items.GetItemsNum(); j++ )
		{
			// Если найден текстовый элемент.
			if( InfPatternItems::itText == items.GetItemType( j ) ||
				InfPatternItems::itDictInline == items.GetItemType( j ) )
				break;

			// Подсчёт количества суперзвёздочек перед термином.
			if( items.GetItemType( j ) == InfPatternItems::itStar || items.GetItemType( j ) == InfPatternItems::itEliStar )
			{
				star_before = true;
				InfPatternItems::Star star( (const char*)items.GetItem( j ) );
				if( star.IsSuperStar() )
					++sstar_before_num;
			}
		}

		// Если такого элемента не найдено.
		if( j == items.GetItemsNum() )
			continue;

		// Циклический поиск элементов.
		while( j < items.GetItemsNum() )
		{
			// Поиск следующего элемента.
			size_t next = 0;
			sstar_after_num = 0;
			star_after = false;
			for( next = j + 1; next < items.GetItemsNum(); next++ )
			{
				// Если найден текстовый элемент.
				if( InfPatternItems::itText == items.GetItemType( next ) ||
					InfPatternItems::itDictInline == items.GetItemType( next ) )
					break;

				// Подсчёт количества суперзвёздочек после термина.
				if( items.GetItemType( next ) == InfPatternItems::itStar || items.GetItemType( next ) == InfPatternItems::itEliStar )
				{
					star_after = true;
					InfPatternItems::Star star( (const char*)items.GetItem( next ) );
					if( star.IsSuperStar() )
						++sstar_after_num;
				}
			}

			// Добавление атрибутов.
			attrs = attrs_list.grow();
			if( !attrs )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			attrs->Reset();

			if( InfPatternItems::itText == items.GetItemType( j ) )
			{
				const InfPatternItems::Text item( items.GetItem( j ) );
				is_real_term = !item.IfStopWordsOnly();
			}
			else if( InfPatternItems::itDictInline == items.GetItemType( j ) )
			{
				const InfPatternItems::TagDictInlineManipulator item( items.GetItem( j ) );
				is_real_term = !item.HasEmpty();
			}

			// Идентификатор словаря.
			attrs->SetId( aDict->GetID() );
			// Номер строки в словаре.
			attrs->SetSubId( i );
			// Позиция в строке.
			attrs->SetPos( j );
			// Порядковый номер обязательного термина.
			attrs->SetRealTermPos( counter );
			// Флаг обязательного термина.
			if( is_real_term )
				attrs->SetFlagRealTerm();

			// Суперзвёздочки до и после термина.
			attrs->SetStarShiftBefore( sstar_before_num );
			attrs->SetStarShiftAfter( sstar_after_num );
			if( sstar_before_num )
				attrs->SetFlagSuperStarBefore();
			if( sstar_after_num )
				attrs->SetFlagSuperStarAfter();

			// Звёздочки до и после термина.
			if( star_before )
				attrs->SetFlagStarBefore();
			if( star_after )
				attrs->SetFlagStarAfter();

			// Признак первого термина в строке.
			if( first_term )
			{
				attrs->SetFlagFirstTerm();
				if( is_real_term )
					first_term = false;
			}

			// Признак последнего термина в строке.
			if( next == items.GetItemsNum() )
			{
				for( int attr_n = attrs_list.size() - 1; attr_n >= 0; --attr_n )
				{
					attrs_list[attr_n].SetFlagLastTerm();
					if( attrs_list[attr_n].GetFlagRealTerm() )
						break;
				}
			}

			sstar_before_num = sstar_after_num;
			star_before = star_after;

			// Инкрементация счётчика реальных терминов.
			if( InfPatternItems::itText == items.GetItemType( j ) )
			{
				counter++;
			}
			else if( InfPatternItems::itDictInline == items.GetItemType( j ) )
			{
				const InfPatternItems::TagDictInlineManipulator item( items.GetItem( j ) );
				if( !item.HasEmpty() )
					counter++;
			}

			j = next;
		}

		// Индексация терминов.
		if( !attrs_list.size() )
		{
			// В строке словаря нет ни одного обязательного элемента.
		}
		else
		{
			// Добавление информации о супер-звёздочках.
			for( unsigned int attr_n = 1; attr_n < attrs_list.size(); ++attr_n )
				attrs_list[attr_n].SetStarShiftBefore( attrs_list[attr_n].GetStarShiftBefore() + attrs_list[attr_n-1].GetStarShiftBefore() );
			
			for( unsigned int attr_n = attrs_list.size()-1; attr_n > 0;--attr_n )
				attrs_list[attr_n-1].SetStarShiftAfter( attrs_list[attr_n-1].GetStarShiftAfter() + attrs_list[attr_n].GetStarShiftAfter() );
			
			for( unsigned int attr_n = 0; attr_n < attrs_list.size(); ++attr_n )
			{
				attrs = &attrs_list[attr_n];

				if( InfPatternItems::itText == items.GetItemType( attrs->GetPos() ) )
				{
					// Индексация текстового элемента.
					InfPatternItems::Text text( items.GetItem( attrs->GetPos() ) );

					unsigned int term_id;
					bool term_is_new;
					InfEngineErrors iee = vDictTermsStorage.AddTermAttrs( text.GetNormalizedTextString(), text.GetNormalizedTextLength(), *attrs, term_id, term_is_new  );
					if( INF_ENGINE_SUCCESS != iee )
						ReturnWithTrace( iee );

					// Если термин встречен впервые.
					if( term_is_new )
					{
						InfTermAttrs atrrs_id;
						atrrs_id.SetId( term_id );

						enum FltTermsErrors ftres = vBaseDict.Add( &atrrs_id.raw(), text.GetNormalizedTextString() );
						if( ftres != TERMS_OK && ftres != TERMS_WARN_EMPTY )
							ReturnWithError( INF_ENGINE_ERROR_ADD_TERM, "Can't add dict term( %s ). Return Code: %d", text.GetTextString(), ftres );
					}
				}
				else if( InfPatternItems::itDictInline == items.GetItemType( attrs->GetPos() ) )
				{
					// Индексация элемента inline-словаря.
					const InfPatternItems::TagDictInlineManipulator item( items.GetItem( attrs->GetPos() ) );

					for( unsigned int inl_n = 0; inl_n < item.GetNormalizedStringsNumber(); inl_n++ )
					{
						unsigned int term_id;
						bool term_is_new = false;
						InfEngineErrors iee = vDictTermsStorage.AddTermAttrs( item.GetNormalizedString( inl_n ), strlen( item.GetNormalizedString( inl_n ) ), *attrs, term_id, term_is_new );
						if( INF_ENGINE_SUCCESS != iee )
							ReturnWithTrace( iee );

						if( term_is_new )
						{
							InfTermAttrs term_attrs;
							term_attrs.SetId( term_id );
							enum FltTermsErrors ftres = vBaseDict.Add( &term_attrs.raw(), item.GetNormalizedString( inl_n ) );
							if( ftres > 0 )
								continue;
							else if( ftres != TERMS_OK )
								ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't add dict term. Return Code: %d", ftres );
						}
					}
				}
			}
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BaseIndexer::PreparePattern( const IndexBaseWR & aIndexBase, InfPattern & aPattern ) const
{
	// Обработка шаблона.
	for( unsigned int i = 0; i < aPattern.GetQuestionNumber(); i++ )
	{
		InfEngineErrors iee = aIndexBase.PreparePatternString( aPattern.GetQuestion( i ) );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	InfPatternItems::QuestionWR * that = aPattern.GetThat();
	if( that )
	{
		InfEngineErrors iee = aIndexBase.PreparePatternString( *that );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfIndexer::InfIndexer( nMemoryAllocator & aMemoryAllocator ) :
	BaseIndexer( FSTORAGE_SECTION_INDEXER_INF_DICT, aMemoryAllocator ),
	vQBase( vLp, FSTORAGE_SECTION_INDEXER_INF_QST, aMemoryAllocator ),
	vTBase( vLp, FSTORAGE_SECTION_INDEXER_INF_THAT, aMemoryAllocator ) {}

InfEngineErrors InfIndexer::Create()
{
	/** Проверка состояния. */

	InfEngineErrors iee;
	if( ( iee = BaseIndexer::Create() ) != INF_ENGINE_SUCCESS ||
		( iee = vQBase.Create() ) != INF_ENGINE_SUCCESS ||
		( iee = vTBase.Create() ) != INF_ENGINE_SUCCESS )
	{
		Destroy();

		ReturnWithTrace( iee );
	}

	vState = State::Ready;

	return INF_ENGINE_SUCCESS;
}

void InfIndexer::Destroy()
{
	vQBase.Destroy();
	vTBase.Destroy();

	BaseIndexer::Destroy();
}

InfEngineErrors InfIndexer::Save( fstorage * aFStorage )
{
	InfEngineErrors iee;
	if( ( iee = BaseIndexer::BaseSave( aFStorage ) ) != INF_ENGINE_SUCCESS ||
		( iee = vQBase.Save( aFStorage ) ) != INF_ENGINE_SUCCESS ||
		( iee = vTBase.Save( aFStorage ) ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfIndexer::AddPattern( const InfPattern & aPattern, unsigned int aPatternId )
{
	InfEngineErrors iee;

	// Проверка статуса.
	if( vState != State::Ready )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "Indexer is not created." );

	// Обработка шаблона.
	for( size_t i = 0; i < aPattern.GetQuestionNumber(); i++ )
	{
		iee = vQBase.AddPatternString( aPattern.GetQuestion( i ), aPatternId, i, aPattern.GetInfPerson() );
		if( iee != INF_ENGINE_SUCCESS )
		{
			if( iee != INF_ENGINE_ERROR_CANT_PARSE )
				ReturnWithTrace( iee );
			else
				return iee;
		}
	}

	InfPatternItems::QuestionWR * that = aPattern.GetThat();
	if( that )
	{
		iee = vTBase.AddPatternString( *that, aPatternId, 0, aPattern.GetInfPerson() );
		if( iee != INF_ENGINE_SUCCESS )
		{
			if( iee != INF_ENGINE_ERROR_CANT_PARSE )
				ReturnWithTrace( iee );
			else
				return iee;
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfIndexer::AddSynonymGroup( const SynonymParser::SynonymGroup & aSynGroup, unsigned int aSynGroupN,
											 const char * aSynDictName, unsigned int aSynDictNameLen, const char* & aErrorMsg )
{
	InfEngineErrors iee = vSynonymBaseWR.AddSynonymGroup( aSynGroup, aSynGroupN, aSynDictName, aSynDictNameLen );
	if( iee != INF_ENGINE_SUCCESS )
	{
		if( iee == INF_ENGINE_WARN_SYNONYM_DUPLICATE )
		{
			aErrorMsg = vSynonymBaseWR.GetErrorMsg();
			return INF_ENGINE_WARN_SYNONYM_DUPLICATE;
		}
		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfIndexer::AddSymbolymGroup( const SynonymParser::SynonymGroup & aSymGroup, unsigned int aSymGroupN, const char* & aErrorMsg )
{
    InfEngineErrors iee = vSymbolymBaseWR.AddSymbolymGroup( aSymGroup, aSymGroupN  );
	if( iee != INF_ENGINE_SUCCESS )
	{
		if( iee == INF_ENGINE_WARN_SYNONYM_DUPLICATE )
		{
			aErrorMsg = vSymbolymBaseWR.GetErrorMsg();
			return INF_ENGINE_WARN_SYNONYM_DUPLICATE;
		}
		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

#define SetErrorAdv( aVerbose, aFormat,  ... ) \
	{ \
		if( aVerbose ) \
		{ \
			printf( "\n [FAILED] " ); \
			printf( aFormat, __VA_ARGS__ ); \
			printf( "\n\n" ); \
		} \
	\
		LogError( aFormat, __VA_ARGS__ ); \
	\
		LogInfo( "   %s", __PRETTY_FUNCTION__ ); \
		LogInfo( "     File: %s [ %d ]", __FILE__, __LINE__ ); \
	}


EllipsisIndexer::EllipsisIndexer( nMemoryAllocator & aMemoryAllocator ) :
	BaseIndexer( FSTORAGE_SECTION_INDEXER_ELLIPSIS_DICT, aMemoryAllocator ),
	vFBase( vLp, FSTORAGE_SECTION_INDEXER_ELLIPSIS_FIRST, aMemoryAllocator ),
	vSBase( vLp, FSTORAGE_SECTION_INDEXER_ELLIPSIS_SECOND, aMemoryAllocator ),
	vTBase( vLp, FSTORAGE_SECTION_INDEXER_ELLIPSIS_THIRD, aMemoryAllocator )
{
	// По умолчанию, считаем, что имеем дело с шаблонами эллипсисов первого типа.
	vMode = etFirst;
}

InfEngineErrors EllipsisIndexer::Create()
{
	// Уничтожение индексатора, если он был ранее создан.
	Destroy();

	InfEngineErrors iee;
	// Создание нужных объектов.
	if( ( ( iee = BaseIndexer::Create() ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vFBase.Create() ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vSBase.Create() ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vTBase.Create() ) != INF_ENGINE_SUCCESS ) )
	{
		Destroy();

		ReturnWithTrace( iee );
	}

	// Переход в состояние готовности к индексации.
	vState = State::Ready;

	return INF_ENGINE_SUCCESS;
}

void EllipsisIndexer::Destroy()
{
	// Уничтожение индексной базы для шаблонов эллипсисов первого типа.
	vFBase.Destroy();

	// Уничтожение индексной базы для шаблонов эллипсисов второго типа.
	vSBase.Destroy();

	// Уничтожение индексной базы для шаблонов эллипсисов третьего типа.
	vTBase.Destroy();

	// Уничтожение всех унаследованных объектов.
	BaseIndexer::Destroy();
}

InfEngineErrors EllipsisIndexer::Save( fstorage * aFStorage )
{
	InfEngineErrors iee;
	if( ( ( iee = BaseIndexer::BaseSave( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vFBase.Save( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vSBase.Save( aFStorage ) ) != INF_ENGINE_SUCCESS ) ||
		( ( iee = vTBase.Save( aFStorage ) ) != INF_ENGINE_SUCCESS ) )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors EllipsisIndexer::AddPattern( const InfPattern & aPattern, unsigned int aPatternId )
{
	// Проверка статуса.
	if( vState != State::Ready )
	{
		SetErrorAdv( 0, "%s.", "Indexer is not created" );

		return INF_ENGINE_ERROR_STATE;
	}

	// Обработка шаблона.
	for( unsigned int i = 0; i < aPattern.GetQuestionNumber(); i++ )
	{
		InfEngineErrors iee = INF_ENGINE_SUCCESS;
		switch( vMode )
		{
		case etFirst:
			iee = vFBase.AddPatternString( aPattern.GetQuestion( i ), aPatternId, i, aPattern.GetInfPerson() );
			break;
		case etSecond:
			iee = vSBase.AddPatternString( aPattern.GetQuestion( i ), aPatternId, i, aPattern.GetInfPerson() );
			break;
		case etThird:
			iee = vTBase.AddPatternString( aPattern.GetQuestion( i ), aPatternId, i, aPattern.GetInfPerson() );
			break;
		}
		if( iee != INF_ENGINE_SUCCESS )
		{
			if( iee != INF_ENGINE_ERROR_CANT_PARSE )
				ReturnWithTrace( iee );

			return iee;
		}
	}

	return INF_ENGINE_SUCCESS;
}
