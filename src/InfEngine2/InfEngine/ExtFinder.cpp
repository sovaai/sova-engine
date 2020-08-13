#include "ExtFinder.hpp"

#include "InfPattern/Items/TagExtendAnswer.hpp"

using namespace InfPatternItems;


void InfBFinder::Reset( )
{
	BFinder::Reset( );

	vMainDictsMatchingResults.Reset( );
}

InfEngineErrors InfBFinder::Search( const char * aRequest, unsigned int aRequestLength, SearchData & aSearchData, Session & aSession,
									iAnswer & aAnswer, unsigned int & aAnswerWeight, RequestStat & aRequestStat, bool aCreateResult, bool aUserBase )
{
	// Время начала процедуры поиска.
	TimeStamp tm_start;

	// Очистка временных данных.
	Reset( );

	// Применение процедуры канонизации UTF-8 к обрабатываемой фразе.
	int ret = vCanonizationDocMessage.SetMessage( nullptr, 0, aRequest, aRequestLength, true );
	if( ret != 0 )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );
	aRequestLength = vCanonizationDocMessage.TextLength();
	char * tmp_buf = nAllocateObjects( vRuntimeAllocator, char, aRequestLength + 1 );
	if( !tmp_buf )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	strncpy( tmp_buf, vCanonizationDocMessage.Text(), aRequestLength );
	tmp_buf[aRequestLength] = '\0';
	aRequest = tmp_buf;

	// Подстановка главных синонимов в запрос пользователя.
	avector<const unsigned int*> request_map;
	request_map.push_back( nullptr );
	if( request_map.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	request_map.push_back( nullptr );
	if( request_map.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	
	const char * normalized_request = nullptr;
	unsigned int normalized_request_size = 0;

	if( aSearchData.vSymbolymBase || aSearchData.vSynonymBase  )
	{
		const char * current_request = aRequest;
		unsigned int current_request_length = aRequestLength;
						
		// Замена символов.
		if( aSearchData.vSymbolymBase )
		{
            const char * tmp_text = nullptr;
			unsigned int tmp_text_length = 0;
			InfEngineErrors iee = aSearchData.vSymbolymBase->ApplySymbolyms( current_request, current_request_length, 
																			 tmp_text, tmp_text_length, request_map[0] );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			current_request = tmp_text;
			current_request_length = tmp_text_length;
		}
		
		// Поиск синонимов.
		if( aSearchData.vSynonymBase )
		{
            const char * tmp_text = nullptr;
			unsigned int tmp_text_length = 0;
			InfEngineErrors iee = aSearchData.vSynonymBase->ApplySynonyms( current_request, current_request_length,
																		   tmp_text, tmp_text_length, request_map[1] );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			current_request = tmp_text;
			current_request_length = tmp_text_length;
		}
		
		normalized_request = current_request;
		normalized_request_size = current_request_length;
	}
	else
	{
		normalized_request = aRequest;
		normalized_request_size = aRequestLength;
	}

	// Увеличение размеров кэша тэгов-функций при необходимости.
	if( vTagFunctionsCache.GetSize( ) < aSearchData.vDLData.GetFuncTagRegistry( ).CountRO( ) )
		vTagFunctionsCache.SetSize( aSearchData.vDLData.GetFuncTagRegistry( ).CountRO( ) );

	// Очистка кэша тэгов-функций.
	vTagFunctionsCache.Clear( );

	// Проверка аргументов.
	if( !normalized_request )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка статуса базы терминов шаблон-вопросов.
	if( !aSearchData.vIndexQBase.IsReady( ) )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Сброс реестра условий.
	//    aSearchData.vDLData.GetConditionsRegistry().ResetChecks();

	// Поиск терминов шаблон-that'ов, если их индексная база предоставлена.
	if( aSearchData.vIndexTBase && aSearchData.vIndexTBase->IsReady( ) )
	{
		// Получаем предыдущий запрос пользователя.
		unsigned int InfPrevResponseLength;
		const char * InfPrevValue = aSession.GetValue( InfEngineVarInfPrevResponse ).AsText().GetValue( InfPrevResponseLength );

		if( InfPrevValue )
		{
			// Подготовка reply- для поиска.
			ret = vDocMessage.SetMessage( nullptr, 0, InfPrevValue, InfPrevResponseLength, true );
			if( ret != 0 )
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );

			// Установка языка.
			vDocMessage.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );


			// Поиск словарей входящих в reply-, если словарная индексная база существует.
			if( aSearchData.vIndexDBase && aSearchData.vIndexDBase->IsReady( ) )
			{
				// Лемматизация reply-.
				InfEngineErrors iee = aSearchData.vIndexDBase->LemmatizeDoc( vDocMessage, vDocImage );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

				// Поиск терминов словарей, входящих в reply-.
				TimeStamp tm_check;
				iee = aSearchData.vIndexDBase->Check( vDocImage, vDictMatcher );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );
				// Время поиска reply- в дереве терминов словарей.
				aRequestStat.vTermsCheckDictThatTm += tm_check.Interval();
				if( aUserBase )
					aRequestStat.ex.timer_request_userbase_terms_that_dicts = tm_check.Interval();
				else
					aRequestStat.ex.timer_request_mainbase_terms_that_dicts = tm_check.Interval();

				// Поиск словарей, входящих в reply- и создание для каждого из них псевдо термина.
//				iee = ProcessDictsTerms( vDictMatcher, vDictsMatchingResults, aSearchData.vDLData );
//				if( iee != INF_ENGINE_SUCCESS )
//					ReturnWithTrace( iee );
				// Время поиска reply- в словарях.
				aRequestStat.vThatDictTm += tm_check.Interval();
				if( aUserBase )
					aRequestStat.ex.timer_request_userbase_that_dicts = tm_check.Interval();
				else
					aRequestStat.ex.timer_request_mainbase_that_dicts = tm_check.Interval();
			}
			else
			{
				// Лемматизация reply-.
				InfEngineErrors iee = aSearchData.vIndexTBase->LemmatizeDoc( vDocMessage, vDocImage );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );
			}

			// Поиск в шаблонах-that'ах.
			InfEngineErrors iee = SearchThats( *aSearchData.vIndexTBase, aSearchData.vDLData,
											   aSearchData.vInfPersonRegistry,
											   aSearchData.vDLData.GetConditionsRegistry( ), vDocImage, aSession,
											   vDictsMatchingResults, aRequestStat, aUserBase );

			if( iee != INF_ENGINE_SUCCESS )
			{
				LogError( "ERROR INFO: user request: \"%s\"", aRequest );
				ReturnWithTrace( iee );
			}
		}
	}

	// Подготовка строки запроса.

	// Сброс старой информации.
	vDocImage.Reset( );

	// Подготовка запроса для поиска.
	ret = vDocMessage.SetMessage( nullptr, 0, normalized_request, normalized_request_size, true );
	if( ret != 0 )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );
	vDocMessage.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );

	InfEngineErrors iee = aSearchData.vIndexQBase.LemmatizeDoc( vDocMessage, vDocImage );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Поиск вхождений словарей в строку запроса.
	vDictMatcher.Reset( );
	vDictsMatchingResults.Reset();

	// Поиск по словарям.
	if( aSearchData.vIndexDBase && aSearchData.vIndexDBase->IsReady( ) )
	{
		vDictMatcher.SetTermsStorage( &(aSearchData.vIndexDBase->GetTermsStorage() ) );

		TimeStamp tm_check;
		iee = aSearchData.vIndexDBase->Check( vDocImage, vDictMatcher );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		// Время поиска по дереву терминов в словарях.
		aRequestStat.vTermsCheckDictTm += tm_check.Interval();
		if( aUserBase )
			aRequestStat.ex.timer_request_userbase_terms_dicts = tm_check.Interval();
		else
			aRequestStat.ex.timer_request_mainbase_terms_dicts = tm_check.Interval();

		vDictsMatchingResults.SetDLData( &(aSearchData.vDLData) );
		iee = ProcessDictsTerms( vDictMatcher, vDictsMatchingResults, aSearchData.vDLData );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		// Время поиска по словарям.
		aRequestStat.vDictTm += tm_check.Interval();
		aRequestStat.ex.timer_request_allbases_dicts += tm_check.Interval();
	}
	
	// Поиск в шаблон-вопросах.
	iee = SearchQuestions( aSearchData.vIndexQBase, aSearchData.vDLData, aSearchData.vInfPersonRegistry,
						   aSearchData.vDLData.GetConditionsRegistry( ), vDocImage, aSession, vDictsMatchingResults,
						   aRequestStat, aUserBase );
	if( iee != INF_ENGINE_SUCCESS )
	{
		LogError( "ERROR INFO: user request: \"%s\"", aRequest );
		ReturnWithTrace( iee );
	}

	// Вычисление времени выполнения поиска по основной базе.
	aRequestStat.vTime = tm_start.Interval();
	if( aUserBase )
		aRequestStat.ex.timer_request_userbase_all = tm_start.Interval();
	else
		aRequestStat.ex.timer_request_mainbase_all = tm_start.Interval();

	// Подготовка  результата.
	if( aCreateResult )
	{
		vTLCSCache.Clear();
		TimeStamp answer_tm;
		iee = CreateAnswer( aSearchData.vDLData, aSearchData.vDLData, aSearchData.vDLData,
							aSearchData.vDLData.GetConditionsRegistry( ), aSession,
							aAnswer, aRequest, strlen( aRequest ), normalized_request, normalized_request_size,
							request_map, aAnswerWeight, aRequestStat, vDictsMatchingResults );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			return iee;
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		// Время генерации ответа из основной базы.
		aRequestStat.vAnsBaseTm = answer_tm.Interval();
		aRequestStat.vAnsTm = aRequestStat.vAnsBaseTm;
	}

	// Вычисление общего времени поиска по деревьям терминов FltTermsBase.
	aRequestStat.vTermsCheckTime = aRequestStat.vTermsCheckUserTm + aRequestStat.vTermsCheckBaseTm +
								   aRequestStat.vTermsCheckDictTm + aRequestStat.vTermsCheckThatTm + aRequestStat.vTermsCheckDictThatTm;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfBFinder::CreateAnswerFromEqualWeightCandidates( avector<AnswerData> & aAnswers, const std::vector<unsigned int> & aOrder,
																   unsigned int & aOrderN, const DLDataRO & aPatternsData,
																   const DLDataRO & aDictsData, const DLDataRO & aFunctionsData,
																   const InfConditionsRegistry & aConditionsRegistry, const Session & aSession,
																   iAnswer & aAnswer, RequestStat & aRequestStat, bool * aAnswerable = nullptr )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Пытаемся построить ответ из имеющихся равновесных кандидатов.
	for( unsigned int ord_n = 0; ord_n < aOrder.size( ); ++ord_n )
	{
		aAnswer.Reset();

		unsigned int ans_n = aOrder[ord_n];

		// Вычисление предварительной длины ответа.
		unsigned int num = GenerateVariationNumber( aAnswers[ans_n].vAnswer, aSession, aPatternsData, aDictsData, aConditionsRegistry );
		// Если по данному шаблон-ответу не получается построить ответ, пропускаем его.
		if( num == 0 )
			continue;

		// Порождение результата.
		aAnswer.Reset( );

		vMatchedPattern.vMatchUserRequestData = aAnswers[ans_n].vCandidat;

		// Получение шаблон-вопроса, связанного с кандидатом.
		InfPatternItems::QuestionRO question = aPatternsData.GetQuestionString( vMatchedPattern.vMatchUserRequestData->vPatternId, vMatchedPattern.vMatchUserRequestData->vQuestionId );

		// Обработка совпавших udict и sudict словарей.
		bool udict_fail = false;
		for( unsigned int item_n = 0; item_n < question.GetItemsNum( ); ++item_n )
		{
			// Если очередной элемент кандидата совпал со словарём.
			if( question.GetItemType( item_n ) == InfPatternItems::itDict &&
				// ... и это было не пустое совпадение.
				Candidat::Item::InlineDict != vMatchedPattern.vMatchUserRequestData->vItems[item_n].vType )
			{
				// Проверяем, имеет ли данный словарь тип udict или sudict.
				InfPatternItems::TagDictManipulator tagdict( question.GetItem( item_n ) );
				if( tagdict.GetDictType( ) == InfPatternItems::DT_UDICT || tagdict.GetDictType( ) == InfPatternItems::DT_SUDICT )
				{
					InfDictWrapManipulator dict;
					InfEngineErrors iee = aDictsData.GetDictById( tagdict.GetDictID( ), dict );
					if( iee != INF_ENGINE_SUCCESS )
						ReturnWithTrace( iee );

					// Проверяем, не было ли найденное слово использовано ранее.
					bool already_used = false;
					if( INF_ENGINE_SUCCESS != ( iee = aSession.GetDictsStates( ).IsUsed( tagdict.GetDictID( ), tagdict.GetDictType( ),
																						 vMatchedPattern.vMatchUserRequestData->vItems[item_n].vLineId, already_used ) ) )
						ReturnWithTrace( iee );

					if( already_used )
					{
						udict_fail = true;
						break;
					}

					// Удаляем совпавшую строку из словаря.
					if( INF_ENGINE_SUCCESS != ( iee = aSession.GetDictsStates( ).RemoveLineIdFromDict( tagdict.GetDictID( ), tagdict.GetDictType( ), dict.GetNum( ),
																									   vMatchedPattern.vMatchUserRequestData->vItems[item_n].vLineId ) ) )
						ReturnWithTrace( iee );
				}
			}
			else if( question.GetItemType( item_n ) == InfPatternItems::itInstruct )
			{
				// Проверка на попытку изменения readonly переменных.
				InfPatternItems::InstructManipulator instruct{ question.GetItem( item_n ) };
				if( aPatternsData.GetVarsRegistry().GetProperties( instruct.GetVarId() ) > Vars::Options::ReadOnly )
				{
					LogDebug( "ERROR: Readonly variable in instruction." );
					udict_fail = true;
				}
			}
		}

		// Если проверка udict и sudict словарей провалена.
		if( udict_fail )
		{
			// Сбрасываем сделанные в ходе обработки кандидата изменения в словарях udict и sudict.
			aSession.GetDictsStates( ).RollbackLastChanges( );
			continue;
		}

		// Подсчёт попыток создания ответа инфа.
		++aRequestStat.vAnsAttempt;
		++aRequestStat.ex.counter_response_allbases_patterns;

		// Создание ответа инфа.
		iee = vMatchedPattern.CreateAnswer( aAnswers[ans_n].vAnswer, aAnswer, aSession );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
		{
			// Сбрасываем сделанные в ходе обработки кандидата изменения в словарях udict и sudict.
			aSession.GetDictsStates( ).RollbackLastChanges( );
			continue;
		}
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		if( !aAnswer.Empty( ) )
		{
			if( aAnswerable )
			{
				// Сбрасываем сделанные в ходе обработки кандидата изменения в словарях udict и sudict.
				aSession.GetDictsStates( ).RollbackLastChanges( );

				// Делаем отметку, что для данного шаблон-ответа можно успешно построить ответ.
				aAnswerable[ord_n] = true;
			}
			else
			{
				// Завершаем нам первом успешно построенном ответе.
				aOrderN = ord_n;
				aAnswer.SetAnswerId( aAnswers[ans_n].vCandidat->vPatternId, aAnswers[ans_n].vAnswerId );
				break;
			}
		}
		else
		{
			// Сбрасываем сделанные в ходе обработки кандидата изменения в словарях udict и sudict.
			aSession.GetDictsStates( ).RollbackLastChanges( );
		}

	}

	// Если построить ответ инфа не удалось.
	if( aAnswer.Empty( ) )
		// Сбрасываем сделанные в ходе обработки кандидата изменения в словарях udict и sudict.
		aSession.GetDictsStates( ).RollbackLastChanges( );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfBFinder::CreateAnswer( const DLDataRO & aPatternsData, const DLDataRO & aDictsData, const DLDataRO & aFunctionsData,
										  const InfConditionsRegistry & aConditionsRegistry, Session & aSession,
										  iAnswer & aAnswer, const char * aRequest, unsigned int aRequestLength,
										  const char * aNormalizedReq, unsigned int aNormalizedReqLength,
										  avector<const unsigned int*> & aRequestMap, unsigned int & aAnswerWeight,
										  RequestStat & aRequestStat, const DictsMatchingResults & aDictsMatchingResults )
{
	// Вес ответа.
	static char weight[64];

	// Подготовка кандидатов.
	InfEngineErrors iee = vResults.SortByWeight();
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	// Копия сессии на случай обработки шаблон-ответов с поднятым флагом LUniq.
	Session fake_session( aSession );
	// Сессия будет скопирована полностью только при необходимости и только один раз.
	bool session_was_copied = false;

	// Сброс данных.
	aAnswer.Reset( );

	aAnswerWeight = 0;

	vMatchedPattern.vConditionsRegistry = &aConditionsRegistry;
	vMatchedPattern.vDictsData = &aDictsData;
	vMatchedPattern.vFunctionsData = &aFunctionsData;
	vMatchedPattern.vPatternsData = &aPatternsData;
	vMatchedPattern.vSession = &aSession;
	vMatchedPattern.vUserRequest = aRequest;
	vMatchedPattern.vUserRequestLength = strlen( aRequest );
	vMatchedPattern.vTagFunctionsCache = &vTagFunctionsCache;
	vMatchedPattern.vTLCSCache = &vTLCSCache;
	vMatchedPattern.vNormalizedRequest = aNormalizedReq;
	vMatchedPattern.vNormalizedRequestLength = aNormalizedReqLength;
	vMatchedPattern.vRequestMap = &aRequestMap;
	vMatchedPattern.vDictsMatchingResults = &aDictsMatchingResults;

	// Проход по элементам кандидата.
	for( unsigned int i = 0; i < vResults.GetCandNumber(); i++ )
	{
		// Сброс массива кандидатов.
		vAnswers.clear( );
		vLUniqAnswers.clear();
		vLastAnswers.clear();

		// Получение кандидата.
		BFinder::Candidat * cand_base = vResults.GetCandidat(i);
		if( cand_base == nullptr )
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get search candidat. Candidat is nullptr." );

		// Выборка всех шаблон-ответов из равновесных шаблонов.
		unsigned int k = i;
		while( k < vResults.GetCandNumber() )
		{
			// Получение кандидата.
			BFinder::Candidat * cand = vResults.GetCandidat(k);
			if( cand == nullptr )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get search candidat. Candidat is nullptr." );

			// Сравнение по весу.
			if( k > i && cand->vWeight != cand_base->vWeight )
				break;

			// Получение шаблона, связанного с кандидатом.
			static InfPatternRO pattern;
			aPatternsData.GetPattern( cand->vPatternId, pattern );

			// Вычисление условных операторов уровня шаблона.
			unsigned int tlcs_cache_id = 0;
			// Установка кандидата.
			vMatchedPattern.vMatchUserRequestData = cand;
			if( INF_ENGINE_SUCCESS != ( iee = vMatchedPattern.TestTLCS( pattern, tlcs_cache_id ) ) )
				ReturnWithTrace( iee );

			// Выборка шаблон-ответов.
			for( unsigned int j = 0; j < pattern.GetAnswersNum( ); j++ )
			{
				auto answer = pattern.GetAnswer( j );

				// Проверка выполнения условных операторов уровня шаблона.
				if( answer.HasTLCS() && !vTLCSCache.Ok( tlcs_cache_id, answer.GetTLCS() ) )
					continue;

				// Проверка на флаг mUniq.
				if( ( answer.IsUniq() || answer.IsLUniq() ) && aSession.GetAnswerFlag( cand->vPatternId, j ) )
				{
					// Сохраняем в отдельный список использованные шаблон-ответы с модификатором LUniq.
					if( answer.IsLUniq() )
					{
						vLUniqAnswers.push_back( AnswerData( answer, cand, pattern.GetId( ), pattern.GetInstrLabel(), j ) );
						if( vLUniqAnswers.no_memory() )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					}

					continue;
				}

				// Проверка на модификатор mLast (шаблон-ответы, имеющие одновременно модификатор mLast и поднятый флаг mLUniq попадают в список vLUniqAnswers).
				if( answer.IsLast() )
				{
					vLastAnswers.push_back( AnswerData( answer, cand, pattern.GetId( ), pattern.GetInstrLabel(), j ) );
					if( vLastAnswers.no_memory() )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
				else
				{
					vAnswers.push_back( AnswerData( answer, cand, pattern.GetId( ), pattern.GetInstrLabel(), j ) );
					if( vAnswers.no_memory() )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
			}
			k++;
		}
		i = k - 1;

		// Указатель на срабовшего кандидата.
		const AnswerData * pAnswer = nullptr;

		if( !vAnswers.empty( ) )
		{

			// Создаём упорядоченный массив номеров.
			std::vector<unsigned int> order;
			try
			{
				for( unsigned int ord_n = 0; ord_n < vAnswers.size( ); ++ord_n )
					order.push_back( ord_n );
			}
			catch( ... )
			{
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			// Перемешиваем шаблоны.
			std::random_shuffle( order.begin(), order.end(), []( int i ) { return std::rand()%i; } );

			// Сортировка по частичному весу.
			std::stable_sort( order.begin(), order.end(), [this]( unsigned int a, unsigned int b ){ return vAnswers[a].vAnswer.GetWeightModifier() > vAnswers[b].vAnswer.GetWeightModifier(); } );

			// Пытаемся построить ответы.
			unsigned int ind = 0;
			if( INF_ENGINE_SUCCESS != ( iee = CreateAnswerFromEqualWeightCandidates( vAnswers, order, ind, aPatternsData, aDictsData,
																					 aFunctionsData, aConditionsRegistry, aSession,
																					 aAnswer, aRequestStat ) ) )
				ReturnWithTrace( iee );

			if( !aAnswer.Empty( ) )
				pAnswer = &vAnswers[order[ind]];
		}

		// Проверка результата создания ответа инфа.
		if( aAnswer.Empty( ) )
		{
			// Попытка построить ответ, сбросив у группы шаблон-ответов флаг LUniq.
			if( vLUniqAnswers.size() )
			{


				// Создаём упорядоченный массив номеров сработавших ранее шаблон-впросов с флагом LUniq.
				std::vector<unsigned int> order;
				try
				{
					for( unsigned int ord_n = 0; ord_n < vLUniqAnswers.size( ); ++ord_n )
						order.push_back( ord_n );
				}
				catch( ... )
				{
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}

				// Выделение низкоприоритетных шаблонов.
				std::sort( order.begin(), order.end(), [this]( unsigned int a, unsigned int b ){ return vLUniqAnswers[a].vAnswer.IsLast() < vLUniqAnswers[b].vAnswer.IsLast(); } );

				auto low_priority_pointer = std::find_if( order.begin(), order.end(), [this]( unsigned int a ) { return vLUniqAnswers[a].vAnswer.IsLast(); } );

				// Перемешиваем не низкоприоритетные шаблоны.
				std::random_shuffle( order.begin(), low_priority_pointer, []( int i ) { return std::rand()%i; } );
				// Сортировка по частичному весу.
				std::stable_sort( order.begin(), low_priority_pointer, [this]( unsigned int a, unsigned int b ){ return vLUniqAnswers[a].vAnswer.GetWeightModifier() > vLUniqAnswers[b].vAnswer.GetWeightModifier(); } );

				// Перемешиваем низкоприоритетные шаблоны.
				std::random_shuffle( low_priority_pointer, order.end(), []( int i ) { return std::rand()%i; } );

				// Пытаемся построить ответы из использованных ранее шаблон-ответов с флагом LUniq.
				unsigned int ind = 0;
				if( INF_ENGINE_SUCCESS != ( iee = CreateAnswerFromEqualWeightCandidates( vLUniqAnswers, order, ind, aPatternsData, aDictsData,
																						 aFunctionsData, aConditionsRegistry, aSession,
																						 aAnswer, aRequestStat ) ) )
					ReturnWithTrace( iee );

				// Удалось построить ответ с помощью шаблон-вопроса с флагом LUniq.
				if( !aAnswer.Empty( ) )
				{
					// Определяем группу шаблон-ответов с флагом LUniq, для которых следует сбросить флаг и сбрасываем его.

					// Выделяем группу шаблон-вопросов, принадлежащих тому же шаблону, для которого только что был успешно построен ответ.
					// Шаблон-вопрос, из которого был построен этот ответ, в группу не входит.
					unsigned int ans_n = ind + 1;
					for(; ans_n < order.size(); ++ans_n )
						if( vLUniqAnswers[order[ans_n-1]].vCandidat->vPatternId != vLUniqAnswers[order[ans_n]].vCandidat->vPatternId  )
							break;
						else
							order[ans_n-ind-1] = order[ans_n];
					order.resize( ans_n - ind - 1 );

					// Массив флагов. Поднятый флаг означает, что из данного шаблон-ответа можно успешно построить ответ.
					avector<bool> answerable;
					answerable.resize( order.size() );
					if( answerable.no_memory() )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					memset( answerable.get_buffer(), 0, sizeof(bool) * order.size() );

					// Проверяем, из каких шаблон-ответов из выделенной группы могут быть построены ответы.
					unsigned int fake_ind = 0;

					// Копирование сессии.
					if( !session_was_copied )
					{
						// Сохранение копируемой сессии во временный буффер.
						unsigned int need_memory = aSession.GetNeedMemorySize();
						char * buffer = nAllocateObjects( vRuntimeAllocator, char, need_memory );
						if( !buffer )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						if( INF_ENGINE_SUCCESS != ( iee = aSession.Save( buffer ) ) )
							ReturnWithTrace( iee );

						// Инициализация текущей сессии из буфера.
						if( INF_ENGINE_SUCCESS != ( iee = fake_session.Load( buffer, need_memory ) ) )
							ReturnWithTrace( iee );

						session_was_copied = true;
					}

					iAnswer fake_answer( vRuntimeAllocator, false );
					if( INF_ENGINE_SUCCESS != ( iee = CreateAnswerFromEqualWeightCandidates( vLUniqAnswers, order, fake_ind, aPatternsData, aDictsData,
																							 aFunctionsData, aConditionsRegistry, fake_session,
																							 fake_answer, aRequestStat, answerable.get_buffer() ) ) )
						ReturnWithTrace( iee );

					// Шаблон-ответы с флагом LUniq, из которых можно успешно построить ответ, помечаем как неиспользованные.
					for( ans_n = 0; ans_n < order.size(); ++ans_n )
						if( answerable[ans_n] )
							aSession.UnsetAnswerFlag( vLUniqAnswers[order[ans_n]].vCandidat->vPatternId, vLUniqAnswers[order[ans_n]].vAnswerId );
				}

				if( !aAnswer.Empty( ) )
					pAnswer = &vLUniqAnswers[order[ind]];
			}
		}

		// Проверка результата создания ответа инфа.
		if( aAnswer.Empty( ) )
		{
			// Попытка построить ответ из шаблон-ответов с модификатором mLast.
			if( vLastAnswers.size() )
			{
				// Создаём упорядоченный массив номеров.
				std::vector<unsigned int> order;
				try
				{
					for( unsigned int ord_n = 0; ord_n < vLastAnswers.size( ); ++ord_n )
						order.push_back( ord_n );
				}
				catch( ... )
				{
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}

				// Перемешиваем шаблоны.
				std::random_shuffle( order.begin(), order.end(), []( int i ) { return std::rand()%i; } );

				// Сортировка по частичному весу.
				std::stable_sort( order.begin(), order.end(), [this]( unsigned int a, unsigned int b ){ return vLastAnswers[a].vAnswer.GetWeightModifier() > vLastAnswers[b].vAnswer.GetWeightModifier(); } );


				// Пытаемся построить ответы.
				unsigned int ind = 0;
				if( INF_ENGINE_SUCCESS != ( iee = CreateAnswerFromEqualWeightCandidates( vLastAnswers, order, ind, aPatternsData, aDictsData,
																						 aFunctionsData, aConditionsRegistry, aSession,
																						 aAnswer, aRequestStat ) ) )
					ReturnWithTrace( iee );

				if( !aAnswer.Empty( ) )
					pAnswer = &vLastAnswers[order[ind]];
			}
		}

		// Пререходим к попытке построить ответ из шаблонов с меньшим весом.
		if( aAnswer.Empty( ) )
			continue;

		// Поиск завершился успехом.

		// Сохранение веса подобранного шаблона.
		snprintf( weight, 63, "%u", pAnswer->vCandidat->vWeight );
		InfEngineErrors iee = const_cast<Session *>( &aSession )->SetTextValue( InfEngineVarMatchWeight, weight, strlen( weight ) );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		if( aAnswer.IsAutoVarsEnabled() )
		{
			// Добавление к ответу инструкции изменения переменной MatchWeight.
			aAnswer.AddInstruct( InfEngineVarMatchWeight, weight, strlen( weight ), true );

			// Добавление reply- инструкции.
			static aTextString vTmp;
			vTmp.clear( );
			InfEngineErrors iee = aAnswer.ToString( vTmp );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithError( iee, "Can't make \"reply-\" instruct: %d", iee );

			// Добавление инструкции.
			if( aAnswer.AddInstruct( InfEngineVarInfPrevResponse, vTmp.ToConstChar( ), vTmp.size( ), true ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Добавление query- инструкции.
			if( aAnswer.AddInstruct( InfEngineVarUserPrevRequest, aRequest, aRequestLength, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Добавление query-- инструкции.
			unsigned int UserPrevRequestLength;
			const char * UserPrevRequest = aSession.GetValue( InfEngineVarUserPrevRequest ).AsText().GetValue( UserPrevRequestLength );
			if( aAnswer.AddInstruct( InfEngineVarUserPrevPrevRequest, UserPrevRequest, UserPrevRequestLength, true ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			bool thatanchor = false;
			bool lastmark = false;
			bool lastmark2 = false;
			bool acvinfo = false;
			bool lastservice = false;
			bool lastset1 = false;
			bool lastset2 = false;
			bool lastset3 = false;
			bool lastset4{ false }, lastset5{ false }, lastset6{ false };

			for( unsigned int i = 0; i < aAnswer.Size( ); i++ )
			{
				if( aAnswer[i].vType == iAnswer::Item::Type::Instruct )
				{
					if( InfEngineVarThatAnchor == aAnswer[i].vItem.vInstruct.vVarId )
						thatanchor = true;
					else if( InfEngineVarLastMark == aAnswer[i].vItem.vInstruct.vVarId )
						lastmark = true;
					else if( InfEngineVarLastMark2 == aAnswer[i].vItem.vInstruct.vVarId )
						lastmark2 = true;
					else if( InfEngineVarAcvInfo == aAnswer[i].vItem.vInstruct.vVarId )
						acvinfo = true;
					else if( InfEngineVarLastService == aAnswer[i].vItem.vInstruct.vVarId )
						lastservice = true;
					else if( InfEngineVarLastSet1 == aAnswer[i].vItem.vInstruct.vVarId )
						lastset1 = true;
					else if( InfEngineVarLastSet2 == aAnswer[i].vItem.vInstruct.vVarId )
						lastset2 = true;
					else if( InfEngineVarLastSet3 == aAnswer[i].vItem.vInstruct.vVarId )
						lastset3 = true;
					else if( InfEngineVarLastSet4 == aAnswer[i].vItem.vInstruct.vVarId )
						lastset4 = true;
					else if( InfEngineVarLastSet5 == aAnswer[i].vItem.vInstruct.vVarId )
						lastset5 = true;
					else if( InfEngineVarLastSet6 == aAnswer[i].vItem.vInstruct.vVarId )
						lastset6 = true;
				}
			}

			// Создание инструкция для обнуления автоматических переменных.
			if( !thatanchor )
			{
				if( aAnswer.AddInstruct( InfEngineVarThatAnchor, "", 0, false ) != INF_ENGINE_SUCCESS )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			if( !lastmark && aAnswer.AddInstruct( InfEngineVarLastMark, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			if( !lastmark2 && aAnswer.AddInstruct( InfEngineVarLastMark2, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			if( !acvinfo && aAnswer.AddInstruct( InfEngineVarAcvInfo, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			if( !lastservice && aAnswer.AddInstruct( InfEngineVarLastService, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			if( !lastset1 && aAnswer.AddInstruct( InfEngineVarLastSet1, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			if( !lastset2 && aAnswer.AddInstruct( InfEngineVarLastSet2, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			if( !lastset3 && aAnswer.AddInstruct( InfEngineVarLastSet3, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			if( !lastset4 && aAnswer.AddInstruct( InfEngineVarLastSet4, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			if( !lastset5 && aAnswer.AddInstruct( InfEngineVarLastSet5, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			if( !lastset6 && aAnswer.AddInstruct( InfEngineVarLastSet6, "", 0, false ) != INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}

		// Простановка автоматической переменной, содержащей идентификатор сработавшего шаблона.
		if( aAnswer.AddNullTermInstruct( InfEngineVarAutoReplyId, pAnswer->vReplyId, true ) != INF_ENGINE_SUCCESS )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Простановка автоматической переменной, содержащей метку --Label сработавшего шаблона.
		if( aAnswer.AddNullTermInstruct( InfEngineVarAutoReplyLabel, pAnswer->vInstrLabel, true ) != INF_ENGINE_SUCCESS )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );


		// Простановка веса подобранного ответа.
		aAnswerWeight = pAnswer->vCandidat->vWeight;

		return INF_ENGINE_SUCCESS;
	}

	return INF_ENGINE_WARN_UNSUCCESS;
}

unsigned int InfBFinder::GenerateVariationNumber( const InfPatternItems::ArrayManipulator & aAnswer, const Session & aSession,
												  const DLDataRO & aPatternsData, const DLDataRO & aDictsData,
												  const InfConditionsRegistry & aConditionsRegistry )
{
	// Подсчет ориентировочного числа вариантов, порождаемых данным шаблон-ответом.
	unsigned int number = 0;

	for( unsigned int i = 0; i < aAnswer.GetItemsNum( ); i++ )
	{
		InfPatternItems::ItemType type = aAnswer.GetItemType( i );
		if( type == InfPatternItems::itInstruct )
		{}
		else if( type == InfPatternItems::itText ||
				 type == InfPatternItems::itInf ||
				 type == InfPatternItems::itBr ||
				 type == InfPatternItems::itHref ||
				 type == InfPatternItems::itGetAnswer ||
				 type == InfPatternItems::itTagRSS ||
				 type == InfPatternItems::itDictRef ||
				 type == InfPatternItems::itPre ||
				 type == InfPatternItems::itNothing )
		{
			if( !number )
				number = 1;
		}
		else if( type == InfPatternItems::itBreak )
		{
			return 0;
		}
		else if( type == InfPatternItems::itExtendAnswer )
		{
			InfPatternItems::TagExtendAnswerManipulator tag;
			tag.SetBuffer( aAnswer.GetItem( i ) );
			if( tag.GetCount() )
			{
				if( number )
					number *= tag.GetCount();
				else
					number = 1;
			}
		}
		else if( type == InfPatternItems::itDict )
		{
			InfPatternItems::TagDictManipulator tagdict( aAnswer.GetItem( i ) );
			InfDictWrapManipulator dict;

			InfEngineErrors iee = aDictsData.GetDictById( tagdict.GetDictID( ), dict );
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
				iee = aDictsData.GetDictById( tagdict.GetDictID( ), dict );
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			unsigned int dict_num = dict.GetNum( );
			unsigned int dict_variation = dict.GetVariantsNumber( );
			if( tagdict.GetDictType( ) == InfPatternItems::DT_UDICT )
			{
				// Приблизительная оценка количества оставшихся в словаре вариантов.
				dict_variation = dict.GetVariantsNumber( ) * aSession.GetDictsStates( ).GetRemainingAmount( tagdict.GetDictID( ), dict_num ) / dict.GetNum();
				if( !dict_variation )
					aSession.GetDictsStates( ).ResetDictState( tagdict.GetDictID( ) );
				dict_variation = dict.GetVariantsNumber( ) * aSession.GetDictsStates( ).GetRemainingAmount( tagdict.GetDictID( ), dict_num ) / dict.GetNum();
				if( !dict_variation )
					return 0;
			}
			else if( tagdict.GetDictType( ) == InfPatternItems::DT_SUDICT )
			{
				// Приблизительная оценка количества оставшихся в словаре вариантов.
				dict_variation = dict.GetVariantsNumber( ) * aSession.GetDictsStates( ).GetRemainingAmount( tagdict.GetDictID( ), dict_num ) / dict.GetNum();
				if( !dict_variation )
					return 0;
			}
			else if( tagdict.GetDictType( ) == InfPatternItems::DT_LCDICT )
				dict_variation = ( dict_variation + 1 ) >> 1;

			if( !number )
				number = dict_variation;
			else
				number *= dict_variation;
		}
		else if( type == InfPatternItems::itVar )
		{
			InfPatternItems::TagVarManipulator tagvar( aAnswer.GetItem( i ) );

			if( aSession.GetValue( tagvar.GetVarId( ) ) && !number )
				number = 1;
		}
		else if( type == InfPatternItems::itStar || type == InfPatternItems::itEliStar )
		{
			if( !number )
				number = 1;
		}
		else if( type == InfPatternItems::itReference || type == InfPatternItems::itEliReference )
		{
			if( !number )
				number = 1;
		}
		else if( type == InfPatternItems::itDictInline )
		{
			InfPatternItems::TagDictInlineManipulator tagdict( aAnswer.GetItem( i ) );

			if( !number )
				number = tagdict.GetStringsNumber( );
			else
				number *= tagdict.GetStringsNumber( );
		}
		else if( type == InfPatternItems::itFunction )
		{
			InfPatternItems::TagFunctionManipulator FuncManip( (const char *)aAnswer.GetItem( i ) );
			if( FuncManip.SavedInIndex( ) )
				FuncManip.SetBuffer( aPatternsData.GetFuncTagRegistry( ).GetTagData( FuncManip.GetTagId( ) ) );

			unsigned int arg_var;
			unsigned int variants = 1;

			for( unsigned int arg_n = 0; arg_n < FuncManip.GetArgumentsCount( ); ++arg_n )
			{
				arg_var = GenerateVariationNumber( FuncManip.GetArgument( arg_n ), aSession, aPatternsData, aDictsData, aConditionsRegistry );
				if( arg_var )
					variants *= arg_var;
			}
			number += variants;
		}
		else if( type == InfPatternItems::itIf )
		{
			InfPatternItems::TagIfManipulator IfManip;

			IfManip.Init( aAnswer.GetItem( i ) );
			InfPatternItems::TagFunctionManipulator Condition( IfManip.GetCondition( ) );
			// Если тэг-функция, реализующая условие, может быть закэширован.
			if( Condition.SavedInIndex( ) && !Condition.IsMultiple( ) )
			{
				number += std::max( GenerateVariationNumber( IfManip.GetTrueItems( ), aSession, aPatternsData, aDictsData, aConditionsRegistry ),
									GenerateVariationNumber( IfManip.GetFalseItems( ), aSession, aPatternsData, aDictsData, aConditionsRegistry ) );
			}
			else
			{
				// Подсчитаем количество вариантов для каждой и зветок оператора If.
				number += GenerateVariationNumber( IfManip.GetTrueItems( ), aSession, aPatternsData, aDictsData, aConditionsRegistry );
				if( IfManip.GetFalseItems( ) )
					number += GenerateVariationNumber( IfManip.GetFalseItems( ), aSession, aPatternsData, aDictsData, aConditionsRegistry );
				else
					++number;
			}
		}
		else if( type == InfPatternItems::itSwitch )
		{
			// Манипулятор оператора switch.
			InfPatternItems::TagSwitchManipulator tag_switch( aAnswer.GetItem(i) );

			// Суммируем варианты раскрытия веток оператора switch.
			for( unsigned int case_n = 0; case_n < tag_switch.GetCaseNumber(); ++case_n )
				number += GenerateVariationNumber( tag_switch.GetCaseArg( case_n ), aSession, aPatternsData, aDictsData, aConditionsRegistry );
		}
	}

	return number;
}

void InfBFinderExt::Reset( )
{
	InfBFinder::Reset( );

	vDictMatcherThat.Reset( );

	// Вспомогательные объекты.
	vDocImageThat.Reset( );

	vDocImageExt.Reset();

	vDocMessageThat.ResetAll( );

	vDictsDataThat.Reset( );

	vUserDictsMatchingResults.Reset();
}

InfEngineErrors InfBFinderExt::Search( const char * aRequest, unsigned int aRequestSize, SearchData & aMainSearchData, SearchData & aExtSearchData, Session & aSession,
									   iAnswer & aAnswer, unsigned int & aAnswerWeight, bool & aAnswerFromUserBase, RequestStat & aRequestStat )
{
	if( !aRequest )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Ответ, полученный из основной базы.
	iAnswer main_answer( true );
	unsigned int main_answer_weight = 0;

	// Ответ, полученный из пользовательской базы.
	iAnswer user_answer( true );
	unsigned int user_answer_weight = 0;


	// Проверка статуса дополнительной базы терминов.
	if( !aExtSearchData.vIndexQBase.IsReady( ) )
	{
		aAnswerFromUserBase = false;

		// Производим поиск только по основной базе.
		return InfBFinder::Search( aRequest, aRequestSize, aMainSearchData, aSession, aAnswer, aAnswerWeight, aRequestStat, true, false );
	}

	// Время начала процедуры поиска.
	TimeStamp tm_start;

	// Очистка памяти.
	Reset( );
	
	// Применение процедуры канонизации UTF-8 к обрабатываемой фразе.
	int ret = vCanonizationDocMessage.SetMessage( nullptr, 0, aRequest, aRequestSize, true );
	if( ret != 0 )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );
	aRequestSize = vCanonizationDocMessage.TextLength();
	char * tmp_buf = nAllocateObjects( vRuntimeAllocator, char, aRequestSize + 1 );
	if( !tmp_buf )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	strncpy( tmp_buf, vCanonizationDocMessage.Text(), aRequestSize );
	tmp_buf[aRequestSize] = '\0';
	aRequest = tmp_buf;
	
	avector<const unsigned int*> request_map;
	request_map.push_back( nullptr );
	if( request_map.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	request_map.push_back( nullptr );
	if( request_map.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	
	const char * normalized_request = nullptr;
	unsigned int normalized_request_size = 0;
	const char * current_request = aRequest;
	unsigned int current_request_size = aRequestSize;
	
	// Замена символов в запросе пользователя.
	if( aMainSearchData.vSymbolymBase )
	{		
        const char * tmp_text = nullptr;
		unsigned int tmp_text_size = 0;
		InfEngineErrors iee = aMainSearchData.vSymbolymBase->ApplySymbolyms( current_request, current_request_size, tmp_text,
																			 tmp_text_size, request_map[0] );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		current_request = tmp_text;
		current_request_size = tmp_text_size;
	}	
	
	// Подстановка главных синонимов в запрос пользователя.
	if( aMainSearchData.vSynonymBase )
	{		
        const char * tmp_text = nullptr;
		unsigned int tmp_text_size = 0;
		InfEngineErrors iee = aMainSearchData.vSynonymBase->ApplySynonyms( current_request, current_request_size, tmp_text,
																		   tmp_text_size, request_map[1] );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		current_request = tmp_text;
		current_request_size = tmp_text_size;
	}
	
	normalized_request = current_request;
	normalized_request_size = current_request_size;

	// Проверка статуса  основной базы терминов.
	if( !aMainSearchData.vIndexQBase.IsReady( ) )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Поиск по дополнительной базе.

	// Подготовка запроса для поиска.
	ret = vDocMessage.SetMessage( nullptr, 0, normalized_request, normalized_request_size, true );
	if( ret != 0 )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "Can't create DocMessage. Return Code: %d", ret );
	vDocMessage.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );
    InfEngineErrors iee = aExtSearchData.vIndexQBase.LemmatizeDoc( vDocMessage, vDocImageExt );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Поиск по словарям основной базы.
	if( aMainSearchData.vIndexDBase && aMainSearchData.vIndexDBase->IsReady( ) )
	{
		TimeStamp tm_check;
		iee = aMainSearchData.vIndexDBase->LemmatizeDoc( vDocMessage, vDocImage );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		// Поиск вхождений словарей в строку запроса.
		vDictMatcher.SetTermsStorage( &(aMainSearchData.vIndexDBase->GetTermsStorage() ) );

		iee = aMainSearchData.vIndexDBase->Check( vDocImage, vDictMatcher );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		// Время поиска по дереву терминов для словарей.
		aRequestStat.vTermsCheckDictTm += tm_check.Interval();
		aRequestStat.ex.timer_request_mainbase_terms_dicts = tm_check.Interval();

		vMainDictsMatchingResults.SetDLData( &(aMainSearchData.vDLData) );
		iee = ProcessDictsTerms( vDictMatcher, vMainDictsMatchingResults, aMainSearchData.vDLData );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		// Время поиска по словарям.
		aRequestStat.vDictTm += tm_check.Interval();
		aRequestStat.ex.timer_request_allbases_dicts += tm_check.Interval();
	}

	// Поиск по словарям пользовательской базы.
	if( aExtSearchData.vIndexDBase && aExtSearchData.vIndexDBase->IsReady( ) )
	{
		TimeStamp tm_check;
		// Поиск вхождений словарей в строку запроса.
		vDictMatcher.SetTermsStorage( &(aExtSearchData.vIndexDBase->GetTermsStorage() ) );
		iee = aExtSearchData.vIndexDBase->Check( vDocImageExt, vDictMatcher );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		aRequestStat.ex.timer_request_userbase_terms_dicts = tm_check.Interval();

		vUserDictsMatchingResults.SetDLData( &(aMainSearchData.vDLData) );
		iee = ProcessDictsTerms( vDictMatcher, vUserDictsMatchingResults, aExtSearchData.vDLData );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		// Время поиска по словарям.
		aRequestStat.vDictTm += tm_check.Interval();
		aRequestStat.ex.timer_request_allbases_dicts += tm_check.Interval();
	}

	// Слияние результатов поиска по основным и пользовательским словарям.
	const DictsMatchingResults * matching_res[2] = { &vMainDictsMatchingResults, &vUserDictsMatchingResults };
	const DLDataRO * dl_data[2] = { &aMainSearchData.vDLData, &aExtSearchData.vDLData };
	vJoinedDictsMatchingResults.SetDLData( &(aMainSearchData.vDLData) );
	if( INF_ENGINE_SUCCESS != ( iee = vJoinedDictsMatchingResults.CreateByJoining( matching_res, dl_data, 2, vDictsJoiningMethod, vRuntimeAllocator ) ) )
	{
		ReturnWithTrace( iee );
	}

	// Поиск в шаблон-вопросах.
	iee = SearchQuestions( aExtSearchData.vIndexQBase, aExtSearchData.vDLData,
						   aExtSearchData.vInfPersonRegistry,
						   aExtSearchData.vDLData.GetConditionsRegistry( ), vDocImageExt, aSession,
						   vJoinedDictsMatchingResults, aRequestStat, true );
	if( iee != INF_ENGINE_SUCCESS )
	{
		LogError( "ERROR INFO: user request: \"%s\"", aRequest );
		ReturnWithTrace( iee );
	}

	// Вычисление времени выполнения поиска по пользовательской базе.
	aRequestStat.vUserTime = tm_start.Interval();
	aRequestStat.vTime = aRequestStat.vUserTime;
	aRequestStat.ex.timer_request_userbase_all = tm_start.Interval();

	// Определение значения флага InfEngineSkipUnknown.
	bool use_skip_unknown_logic = false;
	switch( vSkipUnknownLogic )
	{
	case SkipUnknownLogic::Off:
		use_skip_unknown_logic = false;
		break;

	case SkipUnknownLogic::Var:
		{
			const Vars::Vector::VarValue var_value = aSession.GetValue( InfEngineSkipUnknown );
			if( !var_value.IsNull() )
			{
				if( var_value.AsText().GetValue() && strlen( var_value.AsText().GetValue() ) )
					use_skip_unknown_logic = true;
			}
			break;
		}

	case SkipUnknownLogic::Force:
		use_skip_unknown_logic = true;
		break;
	}

	if( vResults.GetCandNumber() )
	{
		// Очистка кэша тэгов-функций.
		vTagFunctionsCache.Clear( );

		// Подготовка  результата.
		vTLCSCache.Clear();
		TimeStamp answer_tm;
		iee = CreateAnswer( aExtSearchData.vDLData, aExtSearchData.vDLData, aMainSearchData.vDLData,
							aExtSearchData.vDLData.GetConditionsRegistry( ), aSession,
							user_answer, aRequest, strlen( aRequest ), normalized_request, normalized_request_size,
							request_map, user_answer_weight, aRequestStat, vJoinedDictsMatchingResults );

		// Время генерации ответа из пользовательской базы.
		aRequestStat.vAnsUserTm = answer_tm.Interval();
		aRequestStat.vAnsTm = aRequestStat.vAnsUserTm;
		aRequestStat.ex.timer_response_userbase_all = answer_tm.Interval();

		if( iee == INF_ENGINE_SUCCESS )
		{
			aAnswerFromUserBase = true;

			// Вычисление общего времени поиска по деревьям терминов FltTermsBase.
			aRequestStat.vTermsCheckTime = aRequestStat.vTermsCheckUserTm + aRequestStat.vTermsCheckBaseTm +
										   aRequestStat.vTermsCheckDictTm + aRequestStat.vTermsCheckThatTm + aRequestStat.vTermsCheckDictThatTm;


			if( use_skip_unknown_logic )
			{
				vResults.Reset();
			}
			else
			{
				aAnswer = user_answer;
				aAnswerWeight = user_answer_weight;
				return INF_ENGINE_SUCCESS;
			}
		}
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		{
			ReturnWithTrace( iee );
		}
		else
			vResults.Reset();

	}

	// Поиск по основной базе.

	// Сброс времени выполнения поиска.
	tm_start = TimeStamp();

	// Сброс кэша условий.
	vConditionsCheckCache.clear( );

	// Поиск терминов шаблон-that'ов, если их индексная база предоставлена.
	if( aMainSearchData.vIndexTBase && aMainSearchData.vIndexTBase->IsReady( ) )
	{
		// Получение that-строки.
		unsigned int InfResponseLength;
		const char * InfResponse = aSession.GetValue( InfEngineVarInfPrevResponse ).AsText().GetValue( InfResponseLength );
		if( InfResponse )
		{
			// Подготовка запроса для поиска.
			int ret = vDocMessageThat.SetMessage( nullptr, 0, InfResponse, InfResponseLength, true );
			if( ret != 0 )
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );
			vDocMessageThat.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );
			InfEngineErrors iee = aMainSearchData.vIndexQBase.LemmatizeDoc( vDocMessageThat, vDocImageThat );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			if( aMainSearchData.vIndexDBase && aMainSearchData.vIndexDBase->IsReady( ) )
			{
				// Поиск вхождений словарей в that-строку.
				vDictMatcherThat.SetTermsStorage( &(aMainSearchData.vIndexDBase->GetTermsStorage() ) );
				TimeStamp tm_check;
                iee = aMainSearchData.vIndexDBase->Check( vDocImageThat, vDictMatcherThat );
                if( iee != INF_ENGINE_SUCCESS )
                    ReturnWithTrace( iee );
				aRequestStat.vTermsCheckTime += tm_check.Interval();
				// Время поиска reply- по дереву терминов для словарей.
				aRequestStat.vTermsCheckDictThatTm += tm_check.Interval();
				aRequestStat.ex.timer_request_mainbase_terms_that_dicts = tm_check.Interval();

				vDictsDataThat.Reset( );
				vDictsDataThat.SetDLData( &(aMainSearchData.vDLData) );
//				iee = ProcessDictsTerms( vDictMatcherThat, vDictsDataThat, aMainSearchData.vDLData );
//				if( iee != INF_ENGINE_SUCCESS )
//					ReturnWithTrace( iee );

				// Время поиска reply- в словарях.
				aRequestStat.vThatDictTm += tm_check.Interval();
				aRequestStat.ex.timer_request_mainbase_that_dicts = tm_check.Interval();
			}

			// Поиск в шаблонах-that'ах.
			iee = SearchThats( *aMainSearchData.vIndexTBase, aMainSearchData.vDLData, aMainSearchData.vInfPersonRegistry,
							   aMainSearchData.vDLData.GetConditionsRegistry( ), vDocImageThat, aSession, vDictsDataThat,
							   aRequestStat, false );
			if( iee != INF_ENGINE_SUCCESS )
			{
				LogError( "ERROR INFO: user request: \"%s\"", aRequest );
				ReturnWithTrace( iee );
			}
		}
	}

	iee = SearchQuestions( aMainSearchData.vIndexQBase, aMainSearchData.vDLData, aMainSearchData.vInfPersonRegistry,
						   aMainSearchData.vDLData.GetConditionsRegistry( ), vDocImage, aSession, vJoinedDictsMatchingResults,
						   aRequestStat, false );

	if( iee != INF_ENGINE_SUCCESS )
	{
		LogError( "ERROR INFO: user request: \"%s\"", aRequest );
		ReturnWithTrace( iee );
	}

	// Вычисление времени выполнения поиска по основной базе.
	aRequestStat.vBaseTime = tm_start.Interval();
	aRequestStat.vTime += tm_start.Interval();
	aRequestStat.ex.timer_request_mainbase_all = tm_start.Interval();

	// Очистка кэша тэгов-функций.
	vTagFunctionsCache.Clear( );
	// Подготовка  результата.
	vTLCSCache.Clear();
	TimeStamp answer_tm;
	iee = CreateAnswer( aMainSearchData.vDLData, aExtSearchData.vDLData, aMainSearchData.vDLData,
						aMainSearchData.vDLData.GetConditionsRegistry( ), aSession,
						main_answer, aRequest, strlen( aRequest ), normalized_request, normalized_request_size, request_map,
						main_answer_weight, aRequestStat, vJoinedDictsMatchingResults );

	if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );
	// Время генерации ответа из основной базы.
	aRequestStat.vAnsBaseTm = answer_tm.Interval();
	aRequestStat.vAnsTm += aRequestStat.vAnsBaseTm;
	aRequestStat.ex.timer_response_mainbase_all = answer_tm.Interval();

	if( use_skip_unknown_logic )
	{

		// Выбор между ответ из пользовательской и основной базы.
		if( !aAnswerFromUserBase )
		{
			aAnswer = main_answer;
			aAnswerWeight = main_answer_weight;
		}
		else
		{
			// Поиск метки unknown в ответе из пользовательской базы.
			bool main_answer_is_unknown = true;
			if( main_answer.Size() )
			{
				main_answer_is_unknown = false;
				const Vars::Registry & vars_reg = aMainSearchData.vDLData.GetVarsRegistry();
				for( unsigned int item_n = 0; item_n < main_answer.Size(); ++item_n )
				{
					const iAnswer::Item & item = main_answer[item_n];
					if( iAnswer::Item::Type::Instruct == item.vType )
					{
						const char * var_name = vars_reg.GetVarNameById( item.vItem.vInstruct.vVarId );
						if( !var_name )
							ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
						if( !strcasecmp( var_name, "unknown" ) )
						{
							char buf[1024];
							strncpy( buf, item.vItem.vInstruct.vValue, item.vItem.vInstruct.vValueLength );
							buf[item.vItem.vInstruct.vValueLength] = 0;
							if( item.vItem.vInstruct.vValueLength )
							{
								main_answer_is_unknown = true;
								break;
							}
						}
					}
				}
			}

			if( main_answer_is_unknown )
			{
				aAnswer = user_answer;
				aAnswerWeight = user_answer_weight;
				aAnswerFromUserBase = true;
			}
			else
			{
				aAnswer = main_answer;
				aAnswerWeight = main_answer_weight;
				aAnswerFromUserBase = false;
			}
		}
	}
	else
	{
		aAnswer = main_answer;
		aAnswerWeight = main_answer_weight;
		aAnswerFromUserBase = false;
	}

	// Вычисление общего времени поиска по деревьям терминов FltTermsBase.
	aRequestStat.vTermsCheckTime = aRequestStat.vTermsCheckUserTm + aRequestStat.vTermsCheckBaseTm +
								   aRequestStat.vTermsCheckDictTm + aRequestStat.vTermsCheckThatTm + aRequestStat.vTermsCheckDictThatTm;

	return INF_ENGINE_SUCCESS;
}
