#include "MatchedPattern.hpp"

#include "InfPattern/Items/TagExtendAnswer.hpp"
#include "InfPattern/Items/TagPre.hpp"
#include "InfFunctions/InternalFunctions/InternalFunction.hpp"

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;

void unmap( unsigned int & aBeginPos, unsigned int & aEndPos, const unsigned int * aMap )
{
	unsigned int begin = aMap ? aMap[aBeginPos] : aBeginPos;
	unsigned int end = aMap ? aMap[aEndPos] : aEndPos;
	
	aBeginPos = begin;
	aEndPos = end;
}

InfEngineErrors MatchedPattern::InterpretTLCS( InfPatternItems::TLCS_RO aTLCS, bool & aTrue ) const
{
	aTrue = false;

	// Контейнер для результата проверки.
	iAnswer answer( false );

	// Вычисление условия.
	InfEngineErrors iee = MakeResult( aTLCS, answer, 0, false );
	if( INF_ENGINE_SUCCESS != iee )
	{
		// Вычислить условие не удалось.
		if( INF_ENGINE_WARN_UNSUCCESS == iee )
			return INF_ENGINE_SUCCESS;

		ReturnWithTrace( iee );
	}

	// Сохраняем результаты проверки условия в строке.
	aTextString answer_string;
	if( INF_ENGINE_SUCCESS != ( iee = answer.ToString( answer_string ) ) )
		ReturnWithError( iee, "Failed to get Argument as String" );

	// Возвращаем результат.
	if( answer_string.size() )
		aTrue = true;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::TestTLCS( InfPatternRO & aPattern, bool aParentOk, unsigned int aFirstN, unsigned int & aCount, avector<bool> & aPassed ) const
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Количество обработанных УОУШ.
	aCount = 0;
	unsigned int n = aFirstN;
	for(; n < aPattern.GetTLCSNum(); ++n )
	{
		// Получение очередного условного оператора уровня шаблона.
		InfPatternItems::TLCS_RO tlcs = aPattern.GetTLCS( n );

		if( InfPatternItems::TLCS_WR::Type::If == tlcs.GetType() ||
			InfPatternItems::TLCS_WR::Type::Switch == tlcs.GetType() )
		{
			// Тип главного оператора в текущей ветке.
			InfPatternItems::TLCS_WR::Type block_type = tlcs.GetType();

			// Если главный оператор - Switch, то переходим к первому оператору Case.
			if( InfPatternItems::TLCS_WR::Type::Case == block_type )
			{
				if( n + 1 >= aPattern.GetTLCSNum() )
					ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "No Case after Switch" );
				tlcs = aPattern.GetTLCS( ++n );
				if( InfPatternItems::TLCS_WR::Type::Case != tlcs.GetType() )
					ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "No Case after Switch" );

				// Дале работа с блоками Case происходит аналогично работе с блоками If и Elsif.
			}

			// Признак того, что проверки всех предыдущих веток текущего блока If провалились.
			bool all_prev_failed = true;
			// Результат проверки последнего УОУШ текущего уровня.
			bool ok = false;
			// Вычисление результата проверки УОУШ.
			if( aParentOk )
			{
				if( INF_ENGINE_SUCCESS != ( iee = InterpretTLCS( tlcs, ok ) ) )
					ReturnWithTrace( iee );
			}
			// Сохранение результата проверки УОУШ.
			aPassed[n] = ok;
			if( ok )
				all_prev_failed = false;

			// Проход по всем веткам If одного уровня.
			++n;
			while( n < aPattern.GetTLCSNum() )
			{
				// Получение очередного условного оператора уровня шаблона.
				InfPatternItems::TLCS_RO tlcs = aPattern.GetTLCS( n );

				// Окончание блока If.
				if( InfPatternItems::TLCS_WR::Type::Endif == tlcs.GetType() )
				{
					if( InfPatternItems::TLCS_WR::Type::If == block_type )
						break;
					else
						ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Endswitch expected but Endif found" );
				}
				// Окончание блока Switch.
				if( InfPatternItems::TLCS_WR::Type::Endswitch == tlcs.GetType() )
				{
					if( InfPatternItems::TLCS_WR::Type::Switch == block_type )
						break;
					else
						ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Endif expected but Endswitch found" );
				}

				switch( tlcs.GetType() )
				{
				// If и Switch
				case InfPatternItems::TLCS_WR::Type::If:
				case InfPatternItems::TLCS_WR::Type::Switch:
					{
						// Вычисление вложенной ветки.
						unsigned int sub_branch_size = 0;
						if( INF_ENGINE_SUCCESS != ( iee = TestTLCS( aPattern, ok, n, sub_branch_size, aPassed ) ) )
							ReturnWithTrace( iee );

						// Пропуск вложенной ветки.
						n += sub_branch_size;
						break;
					}

				// Elseif и Case.
				case InfPatternItems::TLCS_WR::Type::Elsif:
				case InfPatternItems::TLCS_WR::Type::Case:
					{
						// Вычисление результата проверки УОУШ.
						if( aParentOk && all_prev_failed )
						{
							if( INF_ENGINE_SUCCESS != ( iee = InterpretTLCS( tlcs, ok ) ) )
								ReturnWithTrace( iee );
						}
						else
							ok = false;
						// Сохранение результата проверки УОУШ.
						aPassed[n] = ok;

						if( ok )
							all_prev_failed = false;
						++n;
						break;
					}

				// Else и Default
				case InfPatternItems::TLCS_WR::Type::Else:
				case InfPatternItems::TLCS_WR::Type::Default:
					// Вычисление результата проверки УОУШ.
					ok = aParentOk && all_prev_failed;
					aPassed[n] = ok;

					if( ok )
						all_prev_failed = false;
					++n;
					break;

				default:
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Unknown TLCS type: %i", tlcs.GetType() );
				}
			}
		}
		else
		{
			// Обрабатываемая ветка УОУШ закончилась.
			aCount = n - aFirstN;
			return INF_ENGINE_SUCCESS;
		}
	}

	if( n > aPattern.GetTLCSNum() )
		n = aPattern.GetTLCSNum();

	aCount = n - aFirstN;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::TestTLCS( InfPatternRO & aPattern, unsigned int & aTLCS_CacheId ) const
{
	if( !aPattern.GetTLCSNum() )	
	{
		aTLCS_CacheId = static_cast<unsigned int>(-1);
		return INF_ENGINE_SUCCESS;
	}
	
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Булевый вектор, i-ый элемент которого показывает результат проверки i-ого условного оператора уровня шаблона.
	avector<bool> tlcs_passed;
	tlcs_passed.resize( aPattern.GetTLCSNum() );
	if( tlcs_passed.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	// По умолчанию результаты вычисления всех УОУШ считаются отрицительными.
	memset( tlcs_passed.get_buffer(), 0, tlcs_passed.size() );

	unsigned int tlcs_count = 0;
	if( INF_ENGINE_SUCCESS != ( iee = TestTLCS( aPattern, true, 0, tlcs_count, tlcs_passed ) ) )
		ReturnWithTrace( iee );

	// Проверка корректности данных УОУШ.
	if( tlcs_count != aPattern.GetTLCSNum() )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Only %i TLCS tested from %i", tlcs_count, aPattern.GetTLCSNum() );

	// Кэширование результатов проверки.
	if( INF_ENGINE_SUCCESS != ( iee = vTLCSCache->Put( vMatchUserRequestData->vPatternId, vMatchUserRequestData->vQuestionId,
													   tlcs_passed.get_buffer(), tlcs_passed.size(), aTLCS_CacheId ) ) )
		ReturnWithTrace( iee );

	//TODO: Кэширование результатов проверки.

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::MakeResult( const InfPatternItems::ArrayManipulator & aPatternAnswer, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	// Перебор и интерпретация элементов шаблон-ответа.
	for( unsigned int i = 0; i < aPatternAnswer.GetItemsNum(); i++ )
	{
		// Интерпертация i-ого элемента шаблон-ответа.
		InfEngineErrors iee = InterpretItem( aPatternAnswer, i, aAnswer, aNestingLevel, aCheckIfCondition );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			return iee;
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	// Перенос приоритетных инструкций в конец ответа.
	InfEngineErrors iee = aAnswer.ProcessPriorityItems();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretItem( const InfPatternItems::ArrayManipulator & aPatternAnswer, unsigned int aItemInd, iAnswer & aAnswer,
											   unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	if( aItemInd >= aPatternAnswer.GetItemsNum() )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпертация i-ого элемента шаблон-ответа.
	switch( aPatternAnswer.GetItemType( aItemInd ) )
	{
		InfEngineErrors iee;

	case InfPatternItems::itFunction:
		// Интерпретация функции.
		if( ( iee = InterpretFunction( aPatternAnswer.GetItem( aItemInd ), aAnswer, aNestingLevel, aCheckIfCondition ) ) != INF_ENGINE_SUCCESS )
		{
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
				return iee;
			else
				ReturnWithTrace( iee );
		}
		break;

	case InfPatternItems::itInstruct:
		// Интерпретация инструкции по изменению переменной.
		if( ( iee = InterpretInstruct( aPatternAnswer.GetItem( aItemInd ), aAnswer, aNestingLevel, aCheckIfCondition ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itBr:
		// Интерпретация тэга переноса строки.
		if( ( iee = InterpretBr( aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
		case InfPatternItems::itStartUList:
			LogDebug( "MP: StartUList" );
			TryWithTrace( aAnswer.StartList( false ), INF_ENGINE_SUCCESS );
			break;
		case InfPatternItems::itStartOList:
			LogDebug( "MP: StartOList" );
			TryWithTrace( aAnswer.StartList( true ), INF_ENGINE_SUCCESS );
			break;
		case InfPatternItems::itListItem:
			LogDebug( "MP: ListItem" );
			TryWithTrace( aAnswer.AddListItem(), INF_ENGINE_SUCCESS );
			break;
		case InfPatternItems::itEndList:
			LogDebug( "MP: EndList" );
			TryWithTrace( aAnswer.FinishList(), INF_ENGINE_SUCCESS );
			break;
	case InfPatternItems::itNothing:
		if( ( iee = InterpretTagNothing( aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itDisableautovars:
		if( ( iee = InterpretTagDisableautovars( aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itBreak:
		// Интерпретация тэга отбрасывания ответа.
		if( ( iee = InterpretBreak( aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		return INF_ENGINE_WARN_UNSUCCESS;
		break;
	case InfPatternItems::itReference:
		// Интерпретация ссылки на подстроку, соотнесенную с выделенным фрагментом шаблон-вопроса.
		if( ( iee = InterpretReference( aPatternAnswer.GetItem( aItemInd ), vUserRequest, vUserRequestLength, vNormalizedRequest, vNormalizedRequestLength, vRequestMap,
										vMatchUserRequestData, aAnswer, aNestingLevel ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itDictRef:
		// Интерпретация ссылки на словарь из шаблон-ответа.
		if( ( iee = InterpretDictRef( aPatternAnswer.GetItem( aItemInd ), vUserRequest, vUserRequestLength,
									  vNormalizedRequest, vNormalizedRequestLength, vRequestMap,
									  vMatchUserRequestData, aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itDict:
		// Интерпретация обращения к словарю.
		if( ( iee = InterpretDict( aPatternAnswer.GetItem( aItemInd ), aAnswer ) ) != INF_ENGINE_SUCCESS )
		{
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
				return iee;
			else
				ReturnWithTrace( iee );
		}
		break;
	case InfPatternItems::itHref:
		// Интерпретация тэга генерации ссылки на web страничку.
		if( ( iee = InterpretHref( aPatternAnswer.GetItem( aItemInd ), aAnswer, aNestingLevel, aCheckIfCondition ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itIf:
		// Интерпретация условного оператора.
		if( ( iee = InterpretIf( aPatternAnswer.GetItem( aItemInd ), aAnswer, aNestingLevel, aCheckIfCondition ) ) != INF_ENGINE_SUCCESS )
		{
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
				return iee;
			else
				ReturnWithTrace( iee );
		}
		break;
	case InfPatternItems::itSwitch:
		// Интерпретация оператора выбора.
		if( ( iee = InterpretSwitch( aPatternAnswer.GetItem( aItemInd ), aAnswer, aNestingLevel, aCheckIfCondition ) ) != INF_ENGINE_SUCCESS )
		{
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
				return iee;
			else
				ReturnWithTrace( iee );
		}
		break;
	case InfPatternItems::itGetAnswer:
		/**
		 *  Интерпретация тэга включения шаблон-ответа другого шаблона в качестве элемента
		 * текущего шаблон-ответа.
		 */
		if( ( iee = InterpretGetAnswer( aPatternAnswer.GetItem( aItemInd ), aAnswer, aNestingLevel, aCheckIfCondition ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itInf:
		// Интерпретация кликабельной ссылки на ответ инфу.
		if( ( iee = InterpretInf( aPatternAnswer.GetItem( aItemInd ), aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itStar:
		// Интерпретация ссылки на подстроку запроса пользователя, соотнесенную с звездочкой в шаблон-вопросе.
		if( ( iee = InterpretStar( aPatternAnswer.GetItem( aItemInd ), vUserRequest, vUserRequestLength,
								   vNormalizedRequest, vNormalizedRequestLength, vRequestMap,
								   vMatchUserRequestData, aAnswer, aNestingLevel ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itVar:
		// Интерпретация значения переменной.
		if( ( iee = InterpretVar( aPatternAnswer.GetItem( aItemInd ), aAnswer, aCheckIfCondition ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itText:
		// Интерпретация текстового элемента.
		if( ( iee = InterpretText( aPatternAnswer.GetItem( aItemInd ), aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itSpace:
		// Интерпретация текстового элемента.
		if( ( iee = InterpretSpace( aPatternAnswer.GetItem( aItemInd ), aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itDictInline:
		// Интерпретация inline-словаря.
		if( ( iee = InterpretInlineDict( aPatternAnswer.GetItem( aItemInd ), aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itOpenWindow:
		// Интерпретация тэга открытия ссылки в окне браузера.
		if( ( iee = InterpretOpenWindow( aPatternAnswer.GetItem( aItemInd ), aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itFunctionArg:
	case InfPatternItems::itCondition:
	case InfPatternItems::itEliStar:
	case InfPatternItems::itEliReference:
		// Элементы, которые не могут быть частью шаблон-ответа.
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Invalid type of pattern-answer item: %d", aPatternAnswer.GetItemType( aItemInd ) );
	case InfPatternItems::itTagRSS:
		// Интерпретация запроса RSS.
		if( ( iee = InterpretRSS( aPatternAnswer.GetItem( aItemInd ), aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
	case InfPatternItems::itExtendAnswer:
		// Интерпретация тэга продолжения ответа.
		if( ( iee = InterpretExtendAnswer( aPatternAnswer.GetItem( aItemInd ), aAnswer, aNestingLevel ) ) != INF_ENGINE_SUCCESS )
		{
			if( INF_ENGINE_WARN_UNSUCCESS == iee )
				return iee;
			ReturnWithTrace( iee );
		}
		break;
	case InfPatternItems::itPre:
		// Интерпретация тэга неинтерпретируемого текста.
		if( ( iee = InterpretPre( aPatternAnswer.GetItem( aItemInd ), aAnswer ) ) != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;
		
		case InfPatternItems::itKeywordArgument:
		case InfPatternItems::itTLCS:
			break;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretBr( iAnswer & aAnswer ) const
{
	InfEngineErrors iee = aAnswer.AddBr();
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretText( const void * aPatternItem, iAnswer & aAnswer ) const
{
	const char * Text = nullptr;
	unsigned int TextLength = 0;

	// Получение данных.
	InfEngineErrors iee = InterpretText( aPatternItem, Text, TextLength );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = aAnswer.AddText( Text, TextLength, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretText( const void * aPatternItem, const char* & aText, unsigned int & aTextLength ) const
{
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация текстового элемента.
	InfPatternItems::Text text( aPatternItem );

	aText = text.GetTextString();
	aTextLength = text.GetTextLength();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretSpace( const void * aPatternItem, iAnswer & aAnswer ) const
{
	const char * Space = nullptr;
	unsigned int SpaceLength = 0;

	// Получение данных.
	InfEngineErrors iee = InterpretSpace( aPatternItem, Space, SpaceLength );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = aAnswer.AddSpace( Space, SpaceLength, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretSpace( const void * aPatternItem, const char* & aText, unsigned int & aTextLength ) const
{
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация текстового элемента.
	InfPatternItems::Space space( aPatternItem );

	aText = space.GetTextString();
	aTextLength = space.GetTextLength();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretStar( const void * aPatternItem, const char * aRequest, unsigned int aRequestLength, const char * aNormalizedRequest,
											   unsigned int aNormalizedRequestLength, const RequestMap * aRequestMap, BFinder::Candidat * aMatchedData,
											   iAnswer & aAnswer, unsigned int aNestingLevel ) const
{
	const char * Star = nullptr;
	unsigned int StarLength = 0;

	InfEngineErrors iee = InterpretStar( aPatternItem, aRequest, aRequestLength, aNormalizedRequest, aNormalizedRequestLength,
										 aRequestMap, aMatchedData, Star, StarLength, aNestingLevel );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = aAnswer.AddText( Star, StarLength, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretStar( const void * aPatternItem, const char * aRequest, unsigned int /*aRequestLength*/, const char* /*aNormalizedRequest*/,
											   unsigned int aNormalizedRequestLength, const RequestMap * aRequestMap, BFinder::Candidat * aMatchedData,
											   const char* & aStar, unsigned int & aStarLength, unsigned int aNestingLevel ) const
{
	// Проверка аргументов.
	if( !aPatternItem || !aRequest || !aMatchedData )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Ограничения вложенности при шаблон-ответах, содержащих обращения к другим шаблонам.
	if( aNestingLevel )
	{
		aStar = nullptr;
		aStarLength = 0;
		return INF_ENGINE_SUCCESS;
	}


	// Поиск элемента в шаблон-вопросе, на который ссылается звёздочка.
	InfPatternItems::Star star( aPatternItem );
	unsigned int star_n = star.GetNum();
	unsigned int item_n = 0;
	for( item_n = 0; item_n < aMatchedData->vItemsNumber; ++item_n )
	{
		if( aMatchedData->vItems[item_n].vType == BFinder::Candidat::Item::Star ||
			aMatchedData->vItems[item_n].vType == BFinder::Candidat::Item::SuperStar )
		{
			if( star_n == 1 )
				break;
			--star_n;
		}
	}

	// Элемент, на который ссылается звёздочка не найден.
	if( item_n >= aMatchedData->vItemsNumber )
	{
		aStar = nullptr;
		aStarLength = 0;
		return INF_ENGINE_SUCCESS;
	}

	unsigned int begin = 0;
	unsigned int end = 0;

	// Пустая звёздочка. Нучно вкючить в неё стоп-символы, если они есть.
	if( aMatchedData->vItems[item_n].vBeginPos > aMatchedData->vItems[item_n].vEndPos )
	{
		// Если слева от данной звёздочки находится тоже (супер) звёздочка, то данная звёздочка - пустая.
		if( item_n && ( aMatchedData->vItems[item_n - 1].vType == BFinder::Candidat::Item::Star ||
						aMatchedData->vItems[item_n - 1].vType == BFinder::Candidat::Item::SuperStar ) )
		{
			aStar = nullptr;
			aStarLength = 0;
			return INF_ENGINE_SUCCESS;
		}

		// Поиск стоящего перед звёздочкой непустого элемента.
		int begin_n = item_n - 1;
		while( begin_n >= 0 &&
			   ( aMatchedData->vItems[begin_n].vEndPos < aMatchedData->vItems[begin_n].vBeginPos  ||
				 // Текст из стоп-символов.
				 aMatchedData->vItems[begin_n].vEndPos.IsHead() ) )
			--begin_n;

		// Если все элементы перед звёздочкой пусты.
		if( begin_n < 0 )
			begin = 0;
		// Найден непустой элемент перед звёздочкой.
		else
			begin = aMatchedData->vItems[begin_n].vEndPos.GetDocTextWord()->Offset() + aMatchedData->vItems[begin_n].vEndPos.GetDocTextWord()->Length();

		// Поиск стоящего после звёздочки непустого элемента.
		int end_n = item_n + 1;
		while( end_n < aMatchedData->vItemsNumber &&
			   ( aMatchedData->vItems[end_n].vEndPos < aMatchedData->vItems[end_n].vBeginPos ||
				 // Текст из стоп-символов.
				 aMatchedData->vItems[end_n].vEndPos.IsHead() ) )
			++end_n;

		// Если все элементы после звёздочки пусты.
		if( end_n >= aMatchedData->vItemsNumber )
			end = vNormalizedRequestLength;
		// Найден непустой элемент после звёздочки.
		else
			end = aMatchedData->vItems[end_n].vBeginPos.GetDocTextWord()->Offset();
	}
	else
	{
		NDocImage::Iterator iter_begin = aMatchedData->vItems[item_n].vBeginPos;
		NDocImage::Iterator iter_end = aMatchedData->vItems[item_n].vEndPos;

		if( !iter_begin.IsHead() )
		{
			--iter_begin;
			if( !iter_begin.IsHead() )
				begin = iter_begin.GetDocTextWord()->Offset() + iter_begin.GetDocTextWord()->Length();
			else
				begin = 0;
		}
		else
			begin = iter_begin.GetDocTextWord()->Offset();

		if( !iter_end.IsEnd() )
		{
			++iter_end;
			if( !iter_end.IsEnd() )
				end = iter_end.GetDocTextWord()->Offset();
			else
				end = aNormalizedRequestLength;
		}
		else
			end = iter_end.GetDocTextWord()->Offset() + iter_end.GetDocTextWord()->Length();
	}
		
	if( end == (unsigned int)-1 )
		end = strlen( aRequest );
	if( aRequestMap ) 
	{
		for( unsigned int map_n = 0; map_n < aRequestMap->size(); ++map_n) 
		{
			const unsigned int * map = (*aRequestMap)[aRequestMap->size() - map_n - 1];
			unmap( begin, end, map );
		}
	}
	const char * ptr = aRequest + begin;
	unsigned int size = end - begin;

	// Исключение пробелов из значения.
	for(; size > 0 && *ptr == ' '; ptr++, size-- );

	// Удаление пробелов в конце строки.
	for(; size > 0 && ptr[size - 1] == ' '; size-- );

	aStar = ptr;
	aStarLength = size;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretReference( const void * aPatternItem, const char * aRequest, unsigned int aRequestLength, const char * aNormalizedRequest,
													unsigned int aNormalizedRequestLength, const RequestMap * aRequestMap, BFinder::Candidat * aMatchedData,
													iAnswer & aAnswer, unsigned int aNestingLevel ) const
{
	const char * Reference = nullptr;
	unsigned int ReferenceLength = 0;

	InfEngineErrors iee = InterpretReference( aPatternItem, aRequest, aRequestLength, aNormalizedRequest, aNormalizedRequestLength,
											  aRequestMap, aMatchedData, Reference, ReferenceLength, aNestingLevel );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = aAnswer.AddText( Reference, ReferenceLength, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretReference( const void * aPatternItem, const char * aRequest, unsigned int /*aRequestLength*/, const char* /*aNormalizedRequest*/,
													unsigned int aNormalizedRequestLength, const RequestMap * aRequestMap, BFinder::Candidat * aMatchedData,
													const char* & aReference, unsigned int & aReferenceLength, unsigned int aNestingLevel ) const
{
	// Ограничения вложенности при шаблон-ответах, содержащих обращения к другим шаблонам.
	if( aNestingLevel )
	{
		aReference = nullptr;
		aReferenceLength = 0;
		return INF_ENGINE_SUCCESS;
	}

	// Проверка аргументов.
	if( !aPatternItem || !aRequest || !aMatchedData || !vPatternsData )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация ссылки на подстроку, соотнесенную с выделенным фрагментом шаблон-вопроса.

	// Ограничения вложенности при шаблон-ответах, содержащих обращения к другим шаблонам.
	if( aNestingLevel )
		return INF_ENGINE_SUCCESS;

	// Преобразование данных.
	InfPatternItems::ReferenceManipulator ref( aPatternItem );

	// Получение нужного шаблон-вопроса.
	InfPatternItems::QuestionRO question = vPatternsData->GetQuestionString( aMatchedData->vPatternId, aMatchedData->vQuestionId );

	// Получение границ референции.
	unsigned int tmp_begin, tmp_end;
	question.GetReferece( ref.GetRefNum(), tmp_begin, tmp_end );
	
	// Проверка корректности референции.
	if( tmp_begin < 0 || tmp_end < 0 )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
	
	// Проверка существования референции.
	if( tmp_end == 0 )
	{
		aReference = nullptr;
		aReferenceLength = 0;
		return INF_ENGINE_SUCCESS;
	}
	
	int begin = tmp_begin;
	int end = tmp_end;

	unsigned int beginpos = 0, endpos = 0;

	// Тип первого элемента референции.
	BFinder::Candidat::Item::Type type_like = aMatchedData->vItems[begin].vType;

	// Если первый элемент в референции - словарь.
	if( BFinder::Candidat::Item::Dict == type_like ||
		BFinder::Candidat::Item::LCDict == type_like )
		type_like = aMatchedData->vItems[begin].vStarStart ? BFinder::Candidat::Item::Star : BFinder::Candidat::Item::Text;

	switch( type_like )
	{
	// Первый элемент в референции - текст.
	case BFinder::Candidat::Item::Text:
		{
			if( aMatchedData->vItems[begin].vEndPos < aMatchedData->vItems[begin].vBeginPos )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get matched text" );
			beginpos = aMatchedData->vItems[begin].vBeginPos.GetDocTextWord()->Offset();
			break;
		}

	// Первый элемент в референции - инлайн-словарь.
	case BFinder::Candidat::Item::InlineDict:
		{
			// В инлай-словаре сработал пустой вариант.
			if( aMatchedData->vItems[begin].vEndPos < aMatchedData->vItems[begin].vBeginPos )
			{
				// Поиск следующего за инлайн-словарём непустого элемента.
				while( begin < end &&
					   ( aMatchedData->vItems[begin].vEndPos < aMatchedData->vItems[begin].vBeginPos ||
						 // Текст из стоп-символов.
						 aMatchedData->vItems[begin].vEndPos.IsHead() ) )
					++begin;

				// Непустой элемент найден.
				if( begin < end )
				{
					beginpos = aMatchedData->vItems[begin].vBeginPos.GetDocTextWord()->Offset();
				}
				// Непустой элемент не найден.
				else
				{
					beginpos = aNormalizedRequestLength;
				}
			}
			// В инлай-словаре сработал НЕпустой вариант.
			else
			{
				beginpos = aMatchedData->vItems[begin].vBeginPos.GetDocTextWord()->Offset();
			}
			break;
		}

	// Первый элемент в референции - звёздочка или суперзвёздочка.
	case BFinder::Candidat::Item::Star:
	case BFinder::Candidat::Item::SuperStar:
		{
			// Поиск стоящего перед звёздочкой непустого элемента.
			--begin;
			while( begin >= 0 &&
				   ( aMatchedData->vItems[begin].vEndPos < aMatchedData->vItems[begin].vBeginPos  ||
					 // Текст из стоп-символов.
					 aMatchedData->vItems[begin].vEndPos.IsHead() ) )
				--begin;

			// Если все элементы перед звёздочкой пусты.
			if( begin < 0 )
			{
				beginpos = 0;
			}
			// Найден непустой элемент перед звёздочкой.
			else
			{
				beginpos = aMatchedData->vItems[begin].vEndPos.GetDocTextWord()->Offset() +
						   aMatchedData->vItems[begin].vEndPos.GetDocTextWord()->Length();
			}
			break;
		}
		case BFinder::Candidat::Item::Dict:
		case BFinder::Candidat::Item::LCDict:
			break;
	}

	// Если референция не пустая.
	if( beginpos != aNormalizedRequestLength )
	{
		if( 0 == end )
		{
			// Пустая референция.
			beginpos = aNormalizedRequestLength;
			endpos = aNormalizedRequestLength;
		}
		else
		{
			// Переход к последнему элементу в референции.
			--end;

			type_like = aMatchedData->vItems[end].vType;
			// Если последний элемент в референции - словарь.
			if( BFinder::Candidat::Item::Dict == type_like ||
				BFinder::Candidat::Item::LCDict == type_like )
				type_like = aMatchedData->vItems[end].vStarFinish ? BFinder::Candidat::Item::Star : BFinder::Candidat::Item::Text;

			switch( type_like )
			{
			// Последний элемент в референции - текст.
			case BFinder::Candidat::Item::Text:
				{
					if( aMatchedData->vItems[end].vEndPos < aMatchedData->vItems[end].vBeginPos )
						ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get matched text" );
					endpos = aMatchedData->vItems[end].vEndPos.GetDocTextWord()->Offset() +
							 aMatchedData->vItems[end].vEndPos.GetDocTextWord()->Length();
					break;
				}

			// Последний элемент в референции - инлайн-словарь.
			case BFinder::Candidat::Item::InlineDict:
				{
					// В инлай-словаре сработал пустой вариант.
					if( aMatchedData->vItems[end].vEndPos < aMatchedData->vItems[end].vBeginPos )
					{
						// Поиск следующего перед инлайн-словарём непустого элемента.
						while( end >= begin &&
							   ( aMatchedData->vItems[end].vEndPos < aMatchedData->vItems[end].vBeginPos ||
								 // Текст из стоп-символов.
								 aMatchedData->vItems[end].vEndPos.IsHead() ) )
						{
							--end;
						}

						// Непустой элемент найден.
						if( end >= begin )
						{
							endpos = aMatchedData->vItems[end].vEndPos.GetDocTextWord()->Offset() +
									 aMatchedData->vItems[end].vEndPos.GetDocTextWord()->Length();
						}
						// Непустой элемент не найден.
						else
						{
							endpos = aNormalizedRequestLength;
						}
					}
					// В инлай-словаре сработал НЕпустой вариант.
					else
					{
						endpos = aMatchedData->vItems[end].vEndPos.GetDocTextWord()->Offset() +
								 aMatchedData->vItems[end].vEndPos.GetDocTextWord()->Length();
					}
					break;
				}

			// Последний элемент в референции - звёздочка или суперзвёздочка.
			case BFinder::Candidat::Item::Star:
			case BFinder::Candidat::Item::SuperStar:
				{
					// Поиск стоящего после звёздочки непустого элемента.
					++end;
					while( end < aMatchedData->vItemsNumber &&
						   ( aMatchedData->vItems[end].vEndPos < aMatchedData->vItems[end].vBeginPos ||
							 // Текст из стоп-символов.
							 aMatchedData->vItems[end].vEndPos.IsHead() ) )
						++end;

					// Если все элементы после звёздочки пусты.
					if( end >= aMatchedData->vItemsNumber )
					{
						endpos = vNormalizedRequestLength;
					}
					// Найден непустой элемент после звёздочки.
					else
					{
						endpos = aMatchedData->vItems[end].vBeginPos.GetDocTextWord()->Offset();
					}
					break;
				}
				case BFinder::Candidat::Item::Dict:
				case BFinder::Candidat::Item::LCDict:
					break;
			}
		}
	}
	else
		endpos = aNormalizedRequestLength;
	
	if( aRequestMap ) 
	{
		for( unsigned int map_n = 0; map_n < aRequestMap->size(); ++map_n) 
		{
			const unsigned int * map = (*aRequestMap)[aRequestMap->size() - map_n - 1];
			unmap( beginpos, endpos, map );
		}
	}
	aReference = aRequest + beginpos;
	aReferenceLength = beginpos > endpos ? 0 : endpos - beginpos;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::GetReferenceBorders( const void * aPatternItem, BFinder::Candidat * aMatchedData, NDocImage::Iterator & aBegin, NDocImage::Iterator & aEnd ) const
{
	// Проверка аргументов.
	if( !aPatternItem || !aMatchedData || !vPatternsData )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Загрузка ссылки.
	InfPatternItems::ReferenceManipulator ref( aPatternItem );

	// Получение нужного шаблон-вопроса.
	InfPatternItems::QuestionRO question = vPatternsData->GetQuestionString( aMatchedData->vPatternId, aMatchedData->vQuestionId );

	// Получение границ референции.
	unsigned int tmp_begin, tmp_end;
	question.GetReferece( ref.GetRefNum(), tmp_begin, tmp_end );
	
	// Проверка корректности референции.
	if( tmp_begin < 0 || tmp_end < 0 )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
	
	// Проверка существования референции.
	if( tmp_end == 0 )
	{
		if( aMatchedData->vItemsNumber <= tmp_begin  )
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		aBegin = aMatchedData->vItems[tmp_begin].vBeginPos;
		aEnd = aMatchedData->vItems[tmp_begin].vBeginPos;		
		return INF_ENGINE_SUCCESS;
	}
	
	int begin = tmp_begin;
	int end = tmp_end;
	
	NDocImage::Iterator beginpos = aMatchedData->vItems[begin].vBeginPos;
	NDocImage::Iterator endpos = aMatchedData->vItems[end - 1].vEndPos;

	if( endpos < beginpos )
	{
		aBegin = beginpos;
		aEnd = endpos;
		return INF_ENGINE_SUCCESS;
	}

	// Тип первого элемента референции.
	BFinder::Candidat::Item::Type type_like = aMatchedData->vItems[begin].vType;

	// Если первый элемент в референции - словарь.
	if( BFinder::Candidat::Item::Dict == type_like ||
		BFinder::Candidat::Item::LCDict == type_like )
		type_like = aMatchedData->vItems[begin].vStarStart ? BFinder::Candidat::Item::Star : BFinder::Candidat::Item::Text;

	switch( type_like )
	{
	// Первый элемент в референции - текст.
	case BFinder::Candidat::Item::Text:
		{
			if( aMatchedData->vItems[begin].vEndPos < aMatchedData->vItems[begin].vBeginPos )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get matched text" );
			beginpos = aMatchedData->vItems[begin].vBeginPos;
			break;
		}

	// Первый элемент в референции - инлайн-словарь.
	case BFinder::Candidat::Item::InlineDict:
		{
			// В инлай-словаре сработал пустой вариант.
			if( aMatchedData->vItems[begin].vEndPos < aMatchedData->vItems[begin].vBeginPos )
			{
				// Поиск следующего за инлайн-словарём непустого элемента.
				while( begin < end &&
					   ( aMatchedData->vItems[begin].vEndPos < aMatchedData->vItems[begin].vBeginPos ||
						 // Текст из стоп-символов.
						 aMatchedData->vItems[begin].vEndPos.IsHead() ) )
					++begin;

				// Непустой элемент найден.
				if( begin < end )
				{
					beginpos = aMatchedData->vItems[begin].vBeginPos;
				}
				// Непустой элемент не найден.
				else
				{
					while( !beginpos.IsEnd() )
						++beginpos;
				}
			}
			// В инлай-словаре сработал НЕпустой вариант.
			else
			{
				beginpos = aMatchedData->vItems[begin].vBeginPos;
			}
			break;
		}

	// Первый элемент в референции - звёздочка или суперзвёздочка.
	case BFinder::Candidat::Item::Star:
	case BFinder::Candidat::Item::SuperStar:
		{
			// Поиск стоящего перед звёздочкой непустого элемента.
			--begin;
			while( begin >= 0 &&
				   ( aMatchedData->vItems[begin].vEndPos < aMatchedData->vItems[begin].vBeginPos  ||
					 // Текст из стоп-символов.
					 aMatchedData->vItems[begin].vEndPos.IsHead() ) )
				--begin;

			// Если все элементы перед звёздочкой пусты.
			if( begin < 0 )
				while( !beginpos.IsHead() )
					--beginpos;
			// Найден непустой элемент перед звёздочкой.
			else
			{
				beginpos = aMatchedData->vItems[begin].vEndPos;
				++beginpos;
			}
			break;
		}
		case BFinder::Candidat::Item::Dict:
		case BFinder::Candidat::Item::LCDict:
			break;
	}

	// Если референция не пустая.
	if( !beginpos.IsEnd() )
	{
		if( 0 == end )
		{
			// Пустая референция.
			while( !beginpos.IsEnd() )
				++beginpos;
			while( !endpos.IsEnd() )
				++endpos;
		}
		else
		{
			// Переход к последнему элементу в референции.
			--end;

			type_like = aMatchedData->vItems[end].vType;
			// Если последний элемент в референции - словарь.
			if( BFinder::Candidat::Item::Dict == type_like ||
				BFinder::Candidat::Item::LCDict == type_like )
				type_like = aMatchedData->vItems[end].vStarFinish ? BFinder::Candidat::Item::Star : BFinder::Candidat::Item::Text;

			switch( type_like )
			{
			// Последний элемент в референции - текст.
			case BFinder::Candidat::Item::Text:
				{
					if( aMatchedData->vItems[end].vEndPos < aMatchedData->vItems[end].vBeginPos )
						ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get matched text" );
					endpos = aMatchedData->vItems[end].vEndPos;
					break;
				}

			// Последний элемент в референции - инлайн-словарь.
			case BFinder::Candidat::Item::InlineDict:
				{
					// В инлай-словаре сработал пустой вариант.
					if( aMatchedData->vItems[end].vEndPos < aMatchedData->vItems[end].vBeginPos )
					{
						// Поиск следующего перед инлайн-словарём непустого элемента.
						while( end >= begin &&
							   ( aMatchedData->vItems[end].vEndPos < aMatchedData->vItems[end].vBeginPos ||
								 // Текст из стоп-символов.
								 aMatchedData->vItems[end].vEndPos.IsHead() ) )
						{
							--end;
						}

						// Непустой элемент найден.
						if( end >= begin )
						{
							endpos = aMatchedData->vItems[end].vEndPos;
						}
						// Непустой элемент не найден.
						else
						{
							while( !endpos.IsEnd() )
								++endpos;
						}
					}
					// В инлай-словаре сработал НЕпустой вариант.
					else
					{
						endpos = aMatchedData->vItems[end].vEndPos;
					}
					break;
				}

			// Последний элемент в референции - звёздочка или суперзвёздочка.
			case BFinder::Candidat::Item::Star:
			case BFinder::Candidat::Item::SuperStar:
				{
					// Поиск стоящего после звёздочки непустого элемента.
					++end;
					while( end < aMatchedData->vItemsNumber &&
						   ( aMatchedData->vItems[end].vEndPos < aMatchedData->vItems[end].vBeginPos ||
							 // Текст из стоп-символов.
							 aMatchedData->vItems[end].vEndPos.IsHead() ) )
						++end;

					// Если все элементы после звёздочки пусты.
					if( end >= aMatchedData->vItemsNumber )
					{
						while( !endpos.IsEnd() )
							++endpos;
					}
					// Найден непустой элемент после звёздочки.
					else
					{
						endpos = aMatchedData->vItems[end].vBeginPos;
						--endpos;
					}
					break;
				}
				case BFinder::Candidat::Item::Dict:
				case BFinder::Candidat::Item::LCDict:
					break;
			}
		}
	}
	else
	{
		while( !endpos.IsEnd() )
			++endpos;
	}

	if( beginpos.IsHead() )
		++beginpos;
	if( endpos.IsEnd() )
		--endpos;

	aBegin = beginpos;
	aEnd = endpos;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretDictRef( const void * aPatternItem, const char * aRequest, unsigned int aRequestLength, const char * aNormalizedRequest,
												  unsigned int aNormalizedRequestLength, const RequestMap * aRequestMap,
												  BFinder::Candidat * aMatchedData, iAnswer & aAnswer ) const
{
	const char * DictRef = nullptr;
	unsigned int DictRefLength = 0;

	InfEngineErrors iee = InterpretDictRef( aPatternItem, aRequest, aRequestLength, aRequestMap, aMatchedData, DictRef, DictRefLength );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = aAnswer.AddText( DictRef, DictRefLength, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretDictRef( const void * aPatternItem, const char * aRequest, unsigned int /*aRequestLength*/, const RequestMap * aRequestMap,
												  BFinder::Candidat * aMatchedData, const char* & aResult, unsigned int & aResultLength ) const
{
	// Проверка аргументов.
	if( !aPatternItem || !aRequest || !aMatchedData || !vPatternsData )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Загрузка манипулятора.
	InfPatternItems::DictRefManipulator dict_ref( aPatternItem );

	// Получение нужного шаблон-вопроса.
	InfPatternItems::QuestionRO question = vPatternsData->GetQuestionString( aMatchedData->vPatternId, aMatchedData->vQuestionId );

	// Поиск нужного словаря в шаблон-вопросе.
	unsigned int dict_num = 0;
	unsigned int item_n = 0;
	for(; item_n < question.GetItemsNum(); ++item_n )
		if( InfPatternItems::itDict == question.GetItemType( item_n ) )
		{
			++dict_num;
			// Словарь найден.
			if( dict_num == dict_ref.GetRefNum() )
				break;
		}
	// Если словарь не найден.
	if( item_n == question.GetItemsNum() )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't find dictionary #%i in question #%i in pattern #%i",
						 dict_ref.GetRefNum(), aMatchedData->vQuestionId,  aMatchedData->vPatternId );

	// Сторка с результатом.
	static aTextString res;
	res.clear();

	// Выбор способа интерпретации.
	switch( dict_ref.GetRefType() )
	{
	case InfPatternItems::DictRef::RT_NAME:
	case InfPatternItems::DictRef::RT_VALUE:
	case InfPatternItems::DictRef::RT_POS:
	case InfPatternItems::DictRef::RT_NORM:
		{
			// Получение тэга-словаря.
			InfPatternItems::TagDictManipulator tag_dict( question.GetItem( item_n ) );

			// Выбор результата.
			if( InfPatternItems::DictRef::RT_POS == dict_ref.GetRefType() )
			{
				// Получение номера сработавшей строки.
				nlReturnCode nlrc = res.assign( aMatchedData->vItems[item_n].vLineId );
				if( nlrcSuccess != nlrc )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			}
			else if( InfPatternItems::DictRef::RT_NAME == dict_ref.GetRefType() )
			{
				// Получение ссылки на нужный словарь.
				InfDictWrapManipulator dict_manip;
				InfEngineErrors iee = vDictsData->GetDictById( tag_dict.GetDictID(), dict_manip );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict %i manipulator by id: %d", tag_dict.GetDictID(), iee );

				// Получение имени словаря.
				nlReturnCode nlrc = res.assign( dict_manip.GetName() );
				if( nlrcSuccess != nlrc )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			else if( InfPatternItems::DictRef::RT_VALUE == dict_ref.GetRefType() ||
					 InfPatternItems::DictRef::RT_NORM == dict_ref.GetRefType() )
			{
				// Получение ссылки на нужный словарь.
				InfDictWrapManipulator dict_manip;
				InfEngineErrors iee = vDictsData->GetDictById( tag_dict.GetDictID(), dict_manip );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict %i manipulator by id: %d", tag_dict.GetDictID(), iee );

				// Получение сработавшей строки словаря.
				InfDictStringManipulator dict_string_manip;
				iee = dict_manip.GetString( aMatchedData->vItems[item_n].vLineId, dict_string_manip, InfPatternItems::DictRef::RT_NORM == dict_ref.GetRefType() );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict-element manipulator by id: %u. Error: %i",
									 aMatchedData->vItems[item_n].vLineId, iee );

				// Интерпретация строки словаря.
				const char * buf = nullptr;
				unsigned int buf_len = 0;
				if( INF_ENGINE_SUCCESS != ( iee = InterpretDictString( dict_string_manip, buf, buf_len ) ) )
				{
					if( INF_ENGINE_WARN_UNSUCCESS == iee )
					{
						aResult = nullptr;
						aResultLength = 0;
						return INF_ENGINE_WARN_UNSUCCESS;
					}
					ReturnWithTrace( iee );
				}

				nlReturnCode nlrc = res.assign( buf, buf_len );
				if( nlrcSuccess != nlrc )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			// Выделение памяти под результат.
			char * buffer  = nAllocateObjects( vTmpAllocator, char, res.size() + 1 );
			if( !buffer )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			strcpy( buffer, res.ToConstChar() );
			aResult = buffer;
			aResultLength = res.size();

			break;
		}

	case InfPatternItems::DictRef::RT_ORIGIN:
		{
			// Получение координат фрагмента исходной фразы, совпавшего со словарём.
			unsigned int beginpos = aMatchedData->vItems[item_n].vBeginPos.GetDocTextWord()->Offset();
			unsigned int endpos = aMatchedData->vItems[item_n].vEndPos.GetDocTextWord()->Offset() +
								   aMatchedData->vItems[item_n].vEndPos.GetDocTextWord()->Length();

			if( aRequestMap ) 
			{
				for( unsigned int map_n = 0; map_n < aRequestMap->size(); ++map_n) 
				{
					const unsigned int * map = (*aRequestMap)[aRequestMap->size() - map_n - 1];
					unmap( beginpos, endpos, map );
				}
			}
			aResult = aRequest + beginpos;
			aResultLength = beginpos > endpos ? 0 : endpos - beginpos;

			break;
		}

	default:
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	}

	// Получение шаблона по его идентификатору.
	return INF_ENGINE_SUCCESS;
}


InfEngineErrors MatchedPattern::InterpretInstruct( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация инструкции по изменению переменной.
	InfPatternItems::InstructManipulator instruct( aPatternItem );

	// Получение строки элементов, преставляющих значение, которое нужно присвоить переменной.
	aTextString Value;

	// Подготовка строки значения.
	InfEngineErrors iee = InterpretValue( instruct.GetItems(), Value, aAnswer, aNestingLevel, aCheckIfCondition );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	if( instruct.GetVarId().is_tmp() )
		return vSession->SetTmpValue( instruct.GetVarId(), Value.ToConstChar(), Value.size() );

	if( vRWSession )
	{
		iee = vRWSession->SetTextValue( instruct.GetVarId(), Value.ToConstChar(), Value.size() );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	iee = aAnswer.AddInstruct( instruct.GetVarId(), Value.ToConstChar(), Value.size(), true );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretDict( unsigned int aDictId, unsigned int aLineId, const char* & aResult, unsigned int & aResultLength ) const
{
	// Получение ссылки на нужный словарь.
	InfDictWrapManipulator DictManipulator;
	InfEngineErrors iee = vDictsData->GetDictById( aDictId, DictManipulator );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict %i manipulator by id: %d", aDictId, iee );

	// Получение нужно строки словаря.
	InfDictStringManipulator dictmanipulator;
	iee = DictManipulator.GetString( aLineId, dictmanipulator, true );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict-element manipulator by id: %u. Error: %u", aLineId, iee );

	// Заполнение результатов.
	if( INF_ENGINE_SUCCESS != ( iee = InterpretDictString( dictmanipulator, aResult, aResultLength ) ) )
	{
		if( INF_ENGINE_WARN_UNSUCCESS == iee )
		{
			aResult = nullptr;
			aResultLength = 0;
			return INF_ENGINE_WARN_UNSUCCESS;
		}
		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretDictString( const InfDictStringManipulator & aDictStringManip, const char* & aResult, unsigned int & aResultLength ) const
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	static aTextString res;
	res.clear();
	const char * Buffer = nullptr;
	unsigned int BufferLength = 0;
	
	const PatternsStorage::Pattern items( aDictStringManip.Get() );

	for( unsigned int item_n = 0; item_n < items.GetItemsNum(); ++item_n )
	{
		// Добавление пробела для продолжения результата.
		if( item_n && res.size() )
			if( nlrcSuccess != res.append( " ", 1 ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		
		switch( items.GetItemType( item_n ) )
		{
		case InfPatternItems::itText:
			{
				// Интерпретация текста.
				if( ( iee = InterpretText( items.GetItem( item_n ), Buffer, BufferLength ) ) != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

				// Добавление к результату.
				if( nlrcSuccess != res.append( Buffer, BufferLength ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				break;
			}

		case InfPatternItems::itDictInline:
			// Интерпретация inline-словаря.
			if( ( iee = InterpretInlineDict( items.GetItem( item_n ), Buffer ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			// Добавление к результату.
			if( nlrcSuccess != res.append( Buffer ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			break;

		case InfPatternItems::itStar:
			{
				// Интерпретация звёздочки.
				InfPatternItems::Star star( items.GetItem( item_n ) );
				if( star.IsSuperStar() )
				{
					// Добавление к результату.
					if( nlrcSuccess != res.append( "**", 2 ) )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
				else
				{
					// Добавление к результату.
					if( nlrcSuccess != res.append( "*", 1 ) )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
				break;
			}

		default:
			// Попытка интерпретации строки словаря, содержащей звёздочки.
			return INF_ENGINE_WARN_UNSUCCESS;
		}
	}

	// Полготовка результата.
	char * buf = nAllocateObjects( vTmpAllocator, char, res.size() + 1 );
	if( !buf )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	strcpy( buf, res.ToConstChar() );
	aResult = buf;
	aResultLength = res.size();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretDict( const void * aPatternItem, iAnswer & aAnswer ) const
{
	// Проверка аргументов.
	if( !aPatternItem || !vDictsData )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация обращения к словарю.
	InfPatternItems::TagDictManipulator dict( aPatternItem );

	// Получение ссылки на нужный словарь.
	InfDictWrapManipulator DictManipulator;
	InfEngineErrors iee = vDictsData->GetDictById( dict.GetDictID(), DictManipulator );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict %i manipulator by id: %d", dict.GetDictID(), iee );

	// Рандомизация.
	DictsStates & dicts_states = vSession->GetDictsStates();
	unsigned int target;
	switch( dict.GetDictType() )
	{
	// Простой словарь.
	case InfPatternItems::DT_DICT:
	// Словарь с пониженным приоритетом.
	case InfPatternItems::DT_LCDICT:
		target = rand() % DictManipulator.GetNum();
		break;

	// Словарь udict.
	case InfPatternItems::DT_UDICT:
		// Получаем очередной случайный номер строки.
		iee = dicts_states.GetRandomDictLineId( dict.GetDictID(), dict.GetDictType(), DictManipulator.GetNum(), target );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;

	// Словарь sudict.
	case InfPatternItems::DT_SUDICT:
		iee = dicts_states.GetRandomDictLineId( dict.GetDictID(), dict.GetDictType(), DictManipulator.GetNum(), target );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			target = static_cast<unsigned int>( -1 );
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		break;

	default:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	}

	const char * original = nullptr;
	unsigned int original_len = 0;
	if( static_cast<unsigned int>( -1 ) == target )
	{
		aAnswer.Reset();
		return INF_ENGINE_WARN_UNSUCCESS;
	}
	else
	{
		InfDictStringManipulator dictmanipulator;
		iee = DictManipulator.GetString( target, dictmanipulator );
		if( INF_ENGINE_SUCCESS != iee )
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict-element manipulator by id: %d", iee );

		if( INF_ENGINE_SUCCESS != ( iee = InterpretDictString( dictmanipulator, original, original_len ) ) )
		{
			if( INF_ENGINE_WARN_UNSUCCESS == iee )
			{
				aAnswer.Reset();
				return INF_ENGINE_WARN_UNSUCCESS;
			}
			ReturnWithTrace( iee );
		}
	}

	iee = aAnswer.AddNullTermText( original, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretHref( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация тэга генерации ссылки на web страничку.
	InfPatternItems::TagHrefManipulator href( aPatternItem );

	// Подготовка значения URL'а.
	aTextString URL;
	InfEngineErrors iee = InterpretValue( href.GetURL(), URL, aAnswer, aNestingLevel, aCheckIfCondition );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Подготовка значения ссылки.
	aTextString Link;
	iee = InterpretValue( href.GetLink(), Link, aAnswer, aNestingLevel, aCheckIfCondition );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	const char * Target = href.GetTarget();
	iee = aAnswer.AddHref( Link.ToConstChar(), Link.size(),
						   Target, Target ? strlen( Target ) : 0,
						   URL.ToConstChar(), URL.size(), true );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretValue( const InfPatternItems::ArrayManipulator & aValuePattern, aTextString & aResultValue,
												iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	InfEngineErrors iee;

	aResultValue.clear();

	for( unsigned int i = 0; i < aValuePattern.GetItemsNum(); i++ )
	{
		const char * Buffer = nullptr;
		unsigned int BufferLength = 0;

		switch( aValuePattern.GetItemType( i ) )
		{
		case InfPatternItems::itText:
			if( ( iee = InterpretText( aValuePattern.GetItem( i ), Buffer, BufferLength ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itPre:
			if( ( iee = InterpretPre( aValuePattern.GetItem( i ), Buffer, BufferLength ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itStar:
			if( ( iee = InterpretStar( aValuePattern.GetItem( i ), vUserRequest, vUserRequestLength,
									   vNormalizedRequest, vNormalizedRequestLength, vRequestMap,
									   vMatchUserRequestData, Buffer, BufferLength,
									   aNestingLevel ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itReference:
			if( ( iee = InterpretReference( aValuePattern.GetItem( i ), vUserRequest, vUserRequestLength, vNormalizedRequest, vNormalizedRequestLength, vRequestMap,
											vMatchUserRequestData, Buffer, BufferLength, aNestingLevel ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itDictRef:
			if( ( iee = InterpretDictRef( aValuePattern.GetItem( i ), vUserRequest, vUserRequestLength, vRequestMap,
										  vMatchUserRequestData, Buffer, BufferLength ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itVar:
			if( ( iee = InterpretVar( aValuePattern.GetItem( i ), Buffer, BufferLength ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itFunction:
			if( ( iee = InterpretFunction( aValuePattern.GetItem( i ), Buffer, BufferLength, aAnswer, aNestingLevel, aCheckIfCondition ) ) != INF_ENGINE_SUCCESS )
			{
				if( iee == INF_ENGINE_WARN_UNSUCCESS )
					return iee;
				else if( iee == INF_ENGINE_ERROR_DLF_SKIP_ANSWER )
				{
					aAnswer.Reset();
					return INF_ENGINE_WARN_UNSUCCESS;
				}
				else
					ReturnWithTrace( iee );
			}
			break;
		default:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		}

		if( !aResultValue.empty() && aResultValue.append( " ", 1 ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		if( aResultValue.append( Buffer, BufferLength ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretInf( const void * aPatternItem, iAnswer & aAnswer ) const {
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация кликабельной ссылки на ответ инфу.
	InfPatternItems::TagInfManipulator inf { aPatternItem };

	iAnswer tmpAnswer { false };

	aTextString String;
	auto iee = InterpretValue( inf.GetString(), String, tmpAnswer, 0, false );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return iee;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	tmpAnswer.Reset();

	aTextString RequestString;
	iee = InterpretValue( inf.GetRequest(), RequestString, tmpAnswer, 0, false );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return iee;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );


	iee = aAnswer.AddInf( String.ToConstChar(), String.size(), RequestString.ToConstChar(), RequestString.size(), false );
	switch( iee ) {
		case INF_ENGINE_SUCCESS:
			return INF_ENGINE_SUCCESS;
		case INF_ENGINE_ERROR_INV_ARGS:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
		case INF_ENGINE_ERROR_NOFREE_MEMORY:
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		default:
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretVar( const void * aPatternItem, iAnswer & aAnswer, bool aCheckIfCondition ) const
{
	const char * VarValue = nullptr;
	unsigned int VarValueLength = 0;

	// Получение манипулятора элемента.
	InfPatternItems::TagVarManipulator var( aPatternItem );

	// Получение значения переменной.
	const Vars::Vector::VarValue val = vSession->GetValue( var.GetVarId() );
	if( !val )
		return INF_ENGINE_SUCCESS;
	const Vars::Vector::VarText Value = val.AsText();
	if( !Value )
		return INF_ENGINE_SUCCESS;

	if( Value.GetProperties() > Vars::Options::Extended )
	{
		// Получение that_anchor.
		unsigned int ThatAnchorLength;
		const char * ThatAnchor = vSession->GetValue( InfEngineVarThatAnchor ).AsText().GetValue( ThatAnchorLength );

		// Проверка наличия вариантов значений.
		if( Value.GetNumberOfExtendedValues() == 0 )
			return INF_ENGINE_SUCCESS;

		// Выделение памяти под массив вариантов.
		unsigned int * Variants = static_cast<unsigned int*>( vTmpAllocator.Allocate( sizeof( unsigned int ) * Value.GetNumberOfExtendedValues() ) );
		if( !Variants )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		unsigned int VariantsNumber = 0;

		// Флаг использования вариантов с повышенным приоритетом.
		bool UsingThatAnchor = false;

		for( unsigned int i = 0; i < Value.GetNumberOfExtendedValues(); i++ )
		{
			// Получение значения.
			VarValue = Value.GetExtendedValue( i, VarValueLength );
			if( !VarValue || VarValueLength == 0 )
				continue;

			// Проверка условия.
			unsigned int ConditionLength;
			const char * Condition = Value.GetExtendedCondition( i, ConditionLength );

			if( ThatAnchor && ThatAnchorLength != 0 )
			{
				if( Condition && ConditionLength != 0 )
				{
					if( ThatAnchorLength == ConditionLength && !strncasecmp( Condition, ThatAnchor, ConditionLength ) )
					{
						if( !UsingThatAnchor )
						{
							VariantsNumber = 0;
							UsingThatAnchor = true;
						}

						Variants[VariantsNumber] = i;
						VariantsNumber++;
					}
				}
				else
				{
					if( !UsingThatAnchor )
					{
						Variants[VariantsNumber] = i;
						VariantsNumber++;
					}
				}
			}
			else
			{
				if( !Condition || ConditionLength == 0 )
				{
					if( !UsingThatAnchor )
					{
						Variants[VariantsNumber] = i;
						VariantsNumber++;
					}
				}
			}
		}

		// Выбор результата.
		if( VariantsNumber == 0 )
			return INF_ENGINE_SUCCESS;

		unsigned int Target = Variants[(unsigned int)( ( ( (double)rand() ) / ( RAND_MAX - 1.0 ) ) * VariantsNumber )];

		VarValue = Value.GetExtendedValue( Target, VarValueLength );
		InfEngineErrors iee = aAnswer.AddText( VarValue, VarValueLength, true );
		if( iee != INF_ENGINE_SUCCESS )
		{
			switch( iee )
			{
			case INF_ENGINE_SUCCESS:
				break;
			case INF_ENGINE_ERROR_INV_ARGS:
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
			case INF_ENGINE_ERROR_NOFREE_MEMORY:
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			default:
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
			}
		}

		if( !aCheckIfCondition )
		{

			// Добавление инструкции на установку переменной that_anchor.
			VarValue = Value.GetExtendedInstruct( Target, VarValueLength );
			if( VarValue && VarValueLength )
			{
				iee = aAnswer.AddInstruct( InfEngineVarThatAnchor, VarValue, VarValueLength, true );
				switch( iee )
				{
				case INF_ENGINE_SUCCESS:
					break;
				case INF_ENGINE_ERROR_INV_ARGS:
					ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
				case INF_ENGINE_ERROR_NOFREE_MEMORY:
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				default:
					ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
				}
			}

			// Добавление инструкции для отображения эмоции.
			VarValue = Value.GetExtendedEmotion( Target, VarValueLength );
			if( VarValue && VarValueLength )
			{
				iee = aAnswer.AddInstruct( InfEngineVarAnimation, VarValue, VarValueLength, true );
				switch( iee )
				{
				case INF_ENGINE_SUCCESS:
					break;
				case INF_ENGINE_ERROR_INV_ARGS:
					ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
				case INF_ENGINE_ERROR_NOFREE_MEMORY:
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				default:
					ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
				}
			}

		}
	}
	else
	{
		VarValue = Value.GetValue( VarValueLength );
		InfEngineErrors iee = aAnswer.AddText( VarValue, VarValueLength, true );
		switch( iee )
		{
		case INF_ENGINE_SUCCESS:
			return INF_ENGINE_SUCCESS;
		case INF_ENGINE_ERROR_INV_ARGS:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		case INF_ENGINE_ERROR_NOFREE_MEMORY:
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		default:
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretVar( const void * aPatternItem, const char* & aVarValue, unsigned int & aVarValueLength ) const
{
	// Проверка аргументов.
	if( !aPatternItem || !vSession )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	const char * VarValue = nullptr;
	unsigned int VarValueLength = 0;
	aVarValueLength = 0;

	// Интерпретация значения переменной.
	InfPatternItems::TagVarManipulator var( aPatternItem );

	// Получение значения переменной.
	const Vars::Vector::VarValue val = vSession->GetValue( var.GetVarId() );
	if( !val )
		return INF_ENGINE_SUCCESS;
	const Vars::Vector::VarText Value = val.AsText();
	if( !Value )
		return INF_ENGINE_SUCCESS;

	if( Value.GetProperties() > Vars::Options::Extended )
	{
		// Получение that_anchor.
		unsigned int ThatAnchorLength;
		const char * ThatAnchor = vSession->GetValue( InfEngineVarThatAnchor ).AsText().GetValue( ThatAnchorLength );

		// Проверка наличия вариантов значений.
		if( Value.GetNumberOfExtendedValues() == 0 )
			return INF_ENGINE_SUCCESS;

		// Выделение памяти под массив вариантов.
		unsigned int * Variants = static_cast<unsigned int*>( vTmpAllocator.Allocate( sizeof( unsigned int ) * Value.GetNumberOfExtendedValues() ) );
		if( !Variants )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		unsigned int VariantsNumber = 0;

		// Флаг использования вариантов с повышенным приоритетом.
		bool UsingThatAnchor = false;

		for( unsigned int i = 0; i < Value.GetNumberOfExtendedValues(); i++ )
		{
			// Получение значения.
			VarValue = Value.GetExtendedValue( i, VarValueLength );
			if( !VarValue || VarValueLength == 0 )
				continue;

			// Проверка условия.
			unsigned int ConditionLength;
			const char * Condition = Value.GetExtendedCondition( i, ConditionLength );

			if( ThatAnchor && ThatAnchorLength != 0 )
			{
				if( Condition && ConditionLength != 0 )
				{
					if( ThatAnchorLength == ConditionLength && !strncasecmp( Condition, ThatAnchor, ConditionLength ) )
					{
						if( !UsingThatAnchor )
						{
							VariantsNumber = 0;
							UsingThatAnchor = true;
						}

						Variants[VariantsNumber] = i;
						VariantsNumber++;
					}
				}
				else
				{
					if( !UsingThatAnchor )
					{
						Variants[VariantsNumber] = i;
						VariantsNumber++;
					}
				}
			}
			else
			{
				if( !Condition || ConditionLength == 0 )
				{
					if( !UsingThatAnchor )
					{
						Variants[VariantsNumber] = i;
						VariantsNumber++;
					}
				}
			}
		}

		// Выбор результата.
		if( VariantsNumber == 0 )
			return INF_ENGINE_SUCCESS;

		unsigned int Target = Variants[(unsigned int)( ( ( (double)rand() ) / ( RAND_MAX - 1.0 ) ) * VariantsNumber )];

		aVarValue = Value.GetExtendedValue( Target, aVarValueLength );
	}
	else
	{
		aVarValue = Value.GetValue( aVarValueLength );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretInlineDict( const void * aPatternItem, iAnswer & aAnswer ) const
{
	// Интерпретация inline-словаря.
	const char * res = nullptr;
	InfEngineErrors iee = InterpretInlineDict( aPatternItem, res );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	iee = aAnswer.AddNullTermText( res, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretInlineDict( const void * aPatternItem, const char* & aResult ) const
{
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация inline-словаря.
	InfPatternItems::TagDictInlineManipulator tdi( aPatternItem );

	aResult = tdi.GetString( rand() % tdi.GetStringsNumber() );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretOpenWindow( const void * aPatternItem, iAnswer & aAnswer ) const
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация тэга открытия ссылки в окне браузера.
	InfPatternItems::TagOpenWindowManipulator tag( aPatternItem );
	if( tag.GetURL().GetItemsNum() < 1 )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	iAnswer tmpAnswer( false );
	for( unsigned int i = 0; i < tag.GetURL().GetItemsNum(); i++ )
	{
		iee = InterpretFunctionArgument( tag.GetURL().GetItem( 0 ), tmpAnswer, 0, false );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			return iee;
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	// Сохранение интерпретированного аргумента в виде строки.
	aTextString AnswerString;
	iee = tmpAnswer.ToString( AnswerString );

	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( iee, "Failed to get Argument as String" );

	char * ArgString = nAllocateObjects( vTmpAllocator, char, AnswerString.size() + 1 );
	if( !ArgString )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	memcpy( ArgString, AnswerString.ToConstChar(), AnswerString.size() );
	ArgString[AnswerString.size()] = '\0';

	iee = aAnswer.AddOpenWindow( ArgString, strlen( ArgString ), tag.GetOpenType(), false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretGetAnswer( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	// Проверка аргументов.
	if( !aPatternItem || !vPatternsData )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация тэга включения шаблон-ответа другого шаблона в качестве элемента текущего шаблон-ответа.
	InfPatternItems::TagGetAnswerManipulator taggetanswer( aPatternItem );

	// Получение идентификатора метки.
	unsigned int labelid = taggetanswer.GetTargetId();

	static InfPatternRO pattern;
	InfEngineErrors iee = vPatternsData->GetPatternByLabelId( labelid, pattern );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( iee, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	unsigned int ans = rand() % pattern.GetAnswersNum();

	InfPatternItems::AnswerRO answer = pattern.GetAnswer( ans );

	// Обработка результата.
	if( aNestingLevel < 10 )
	{
		unsigned int originalsize = aAnswer.Size();
		iee = MakeResult( answer, aAnswer, aNestingLevel, aCheckIfCondition );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		for( unsigned int i = originalsize; i < aAnswer.Size(); i++ )
			aAnswer.SetPriority( i );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretSwitch( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	unsigned int AnswerSize = aAnswer.Size();

	// Манипулятор условного оператора.
	InfPatternItems::TagSwitchManipulator tag_switch( aPatternItem );

	// Вычисление аргумента switch.
	iAnswer intepreted_switch( false );
	iAnswer intepreted_case( false );
	intepreted_switch.Reset();
	InfEngineErrors iee = InterpretFunctionArgument( tag_switch.GetSwitchArg(), intepreted_switch, aNestingLevel, aCheckIfCondition );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return iee;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Преобразование результата вычисления аргумента Switch в строку.
	aTextString switch_arg_str;
	if( INF_ENGINE_SUCCESS != ( iee = intepreted_switch.ToString( switch_arg_str ) ) )
		ReturnWithError( iee, "Failed to get Switch argument as String" );
	AllTrim( switch_arg_str );

	const char * result_block = nullptr;

	for( unsigned int case_n = 0; case_n < tag_switch.GetCaseNumber(); ++case_n )
	{
		// Вычисление аргумента Case.
		intepreted_case.Reset();
		InfEngineErrors iee = InterpretFunctionArgument( tag_switch.GetCaseArg( case_n ), intepreted_case, aNestingLevel, aCheckIfCondition );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			return iee;
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		// Преобразование результата вычисления аргумента Case в строку.
		aTextString case_arg_str;
		if( INF_ENGINE_SUCCESS != ( iee = intepreted_case.ToString( case_arg_str ) ) )
			ReturnWithError( iee, "Failed to get Case argument as String" );
		AllTrim( case_arg_str );

		// Проверка выполнения условия.
		if( !strcmp( case_arg_str.ToConstChar(), switch_arg_str.ToConstChar() ) )
		{
			result_block = tag_switch.GetCaseBody( case_n );
			break;
		}
		else
			intepreted_case.Reset();
	}

	// Выбор блока default.
	if( !result_block )
		result_block = tag_switch.GetDefaultBody();

	// Добавляем к ответу инструкции по изменению переменных, сделанные при проверки условий.
	for( unsigned int item_n = 0; item_n < intepreted_switch.Size(); ++item_n )
	{
		const iAnswer::Item & item = intepreted_switch[item_n];
		if( item.vType == iAnswer::Item::Type::Instruct )
		{
			if( INF_ENGINE_SUCCESS != ( iee = aAnswer.AddInstruct( item.vItem.vInstruct.vVarId, item.vItem.vInstruct.vValue,
																   item.vItem.vInstruct.vValueLength, false ) ) )
				ReturnWithTrace( iee );
		}
	}
	for( unsigned int item_n = 0; item_n < intepreted_case.Size(); ++item_n )
	{
		const iAnswer::Item & item = intepreted_case[item_n];
		if( item.vType == iAnswer::Item::Type::Instruct )
		{
			if( INF_ENGINE_SUCCESS != ( iee = aAnswer.AddInstruct( item.vItem.vInstruct.vVarId, item.vItem.vInstruct.vValue,
																   item.vItem.vInstruct.vValueLength, false ) ) )
				ReturnWithTrace( iee );
		}
	}

	// Интерпретация выбранного в итоге блока.
	iee = InterpretFunctionArgument( result_block, aAnswer, aNestingLevel, aCheckIfCondition );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return iee;
	else if( iee != INF_ENGINE_SUCCESS )
	{
		aAnswer.PopBack( aAnswer.Size() - AnswerSize );
		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretIf( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация условного оператора.
	InfPatternItems::TagIfManipulator tagif( aPatternItem );

	// Проверка условия.
	static iAnswer cond_answer( false );
	cond_answer.Reset();
	InfEngineErrors iee = InterpretFunction( tagif.GetCondition(), cond_answer, aNestingLevel, true );
	if( iee != INF_ENGINE_SUCCESS )
	{
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			return iee;
		else
			ReturnWithTrace( iee );
	}

	// Сохраняем результаты проверки условия в строке.
	unsigned int AnswerSize = aAnswer.Size();
	aTextString AnswerString;
	iee = cond_answer.ToString( AnswerString );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( iee, "Failed to get Argument as String" );

	// Проверяем, чему равно условие.
	const char * branch = nullptr;
	if( AnswerString.size() )
		// Результат проверки - истина.
		branch = tagif.GetTrueItems();
	else
		// Результат проверки - ложь.
		branch = tagif.GetFalseItems();

	// Интерпретация выбранной ветви оператора If.
	if( branch )
	{
		// Добавляем к ответу инструкции по изменению переменных, сделанный при проверки условия.
		for( unsigned int item_n = 0; item_n < cond_answer.Size(); ++item_n )
		{
			const iAnswer::Item & item = cond_answer[item_n];
			if( item.vType == iAnswer::Item::Type::Instruct )
			{
				if( INF_ENGINE_SUCCESS != ( iee = aAnswer.AddInstruct( item.vItem.vInstruct.vVarId, item.vItem.vInstruct.vValue,
																	   item.vItem.vInstruct.vValueLength, false ) ) )
					ReturnWithTrace( iee );
			}
		}

		iee = InterpretFunctionArgument( branch, aAnswer, aNestingLevel, aCheckIfCondition );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			return iee;
		else if( iee != INF_ENGINE_SUCCESS )
		{
			aAnswer.PopBack( aAnswer.Size() - AnswerSize );
			ReturnWithTrace( iee );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretFunctionArgument( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	// Интерпретация аргументов.
	InfEngineErrors iee = MakeResult( InfPatternItems::TagFunctionArgumentManipulator( aPatternItem ), aAnswer, aNestingLevel, aCheckIfCondition );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return iee;
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Failed to recurse intepret Function Argument" );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretFunction( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	const char * Text = nullptr;
	unsigned int TextLength = 0;

	// Получение данных.
	InfEngineErrors iee = InterpretFunction( aPatternItem, Text, TextLength, aAnswer, aNestingLevel, aCheckIfCondition );
	if( iee == INF_ENGINE_WARN_UNSUCCESS || iee == INF_ENGINE_ERROR_DLF_SKIP_ANSWER )
	{
		aAnswer.Reset();
		return INF_ENGINE_WARN_UNSUCCESS;
	}
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	if( Text && TextLength )
	{
		iee = aAnswer.AddText( Text, TextLength, false );
		switch( iee )
		{
		case INF_ENGINE_SUCCESS:
			return INF_ENGINE_SUCCESS;
		case INF_ENGINE_ERROR_INV_ARGS:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
		case INF_ENGINE_ERROR_NOFREE_MEMORY:
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		default:
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretFunction( const void * aPatternItem, const char* & aText, unsigned int & aTextLength, iAnswer & aAnswer,
												   unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	// инициализация манипулятора для функции.
	InfPatternItems::TagFunctionManipulator FunctionManipulator;
	FunctionManipulator.SetBuffer( static_cast<const char*>( aPatternItem ) );

	unsigned int TagId = -1;

	if( FunctionManipulator.SavedInIndex() )
	{
		TagId = FunctionManipulator.GetTagId();

		if( !FunctionManipulator.IsMultiple() &&
			vTagFunctionsCache->GetValue( TagId, aText, aTextLength ) == INF_ENGINE_SUCCESS )
			return INF_ENGINE_SUCCESS;

		FunctionManipulator.SetBuffer( vPatternsData->GetFuncTagRegistry().GetTagData( TagId ) );
	}

	// Поиск функции среди зарегистрированных.
	const FunctionsRegistry & aFunctionsRegistry = vFunctionsData->GetFunctionsRegistry();
	const char * FunctionName = aFunctionsRegistry.GetFuncNameById( FunctionManipulator.GetId() );
	if( !FunctionName )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Failed to find Function by Id=%i", FunctionManipulator.GetId() );

	// Проверка корректности количества аргументов функции.
	const unsigned int * registred_arg_count = aFunctionsRegistry.GetFunctionArgCount( FunctionManipulator.GetId() );
	if( !registred_arg_count )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "In function %s: not registred ", FunctionName );

	if( FunctionManipulator.GetArgumentsCount() > *registred_arg_count && !aFunctionsRegistry.HasVarArgs( FunctionManipulator.GetId() ) )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "In function %s: %i parameters were given when only %i expected ", FunctionName,
						 FunctionManipulator.GetArgumentsCount(), FunctionManipulator.GetArgumentsCount() );



	// Интерпретация аргументов.
	avector<const char*> InterpretedArgs;

	for( unsigned int arg_n = 0; arg_n < FunctionManipulator.GetArgumentsCount(); ++arg_n )
	{
        // Строка - результат раскрытия аргумента.
        char * ArgString = nullptr;
        
		// Определение типа очередного агрумента.
		DLFunctionArgType ArgType = aFunctionsRegistry.GetFunctionArgInfo( FunctionManipulator.GetId(), arg_n )->type;

		// Получение очредного аргумента.
		const void * pArg = FunctionManipulator.GetArgument( arg_n );
		if( !pArg )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Failed to get Manipulator for Arg %i Function Id=%i", arg_n, FunctionManipulator.GetId() );
        
        // Подстановка идентификатора переменной на место аргумента-переменной.
        if( ArgType == DLFAT_VARIABLE )
        {
            // Получение аргумента-переменной, переданной "по сслыке".
            const void * pArg = FunctionManipulator.GetArgument( arg_n );
    
            // Инициализация манипулятора для аргумента.
            InfPatternItems::TagFunctionArgumentManipulator ArgManipulator( pArg );
    
            // В списке должен быть только один элемент - переменная.
            if( ArgManipulator.GetItemsNum() != 1 ||
                ArgManipulator.GetItemType( 0 ) != InfPatternItems::itVar )
                ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Some another argument found while variable expected" );
    
            // Представляем идентификатор переменной в виде строки.
            InfPatternItems::TagVarManipulator var( ArgManipulator.GetItem( 0 ) );
            aTextString var_id;         
            nlReturnCode nlrc = var_id.assign( var.GetVarId().serialized() );
            if( nlrcSuccess != nlrc )
                ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
            
            ArgString = nAllocateObjects( vTmpAllocator, char, var_id.size() + 1 );
            if( !ArgString )
                ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
    
            memcpy( ArgString, var_id.c_str(), var_id.size() );
            ArgString[var_id.size()] = '\0';
        }
        else
        {
            iAnswer tmpAnswer( false );
    
            InfEngineErrors iee = InterpretFunctionArgument( pArg, tmpAnswer, aNestingLevel, aCheckIfCondition );
            if( iee == INF_ENGINE_WARN_UNSUCCESS )
                return iee;
            else if( iee != INF_ENGINE_SUCCESS )
                ReturnWithTrace( iee );
    
            // Добавление инструкций из временного ответа в конец основного ответа.
            for( unsigned int item_n = 0; item_n < tmpAnswer.Size(); ++item_n )
            {
                const iAnswer::Item & item = tmpAnswer[item_n];
                if( item.vType == iAnswer::Item::Type::Instruct )
                    aAnswer.AddInstruct( item.vItem.vInstruct.vVarId, item.vItem.vInstruct.vValue, strlen( item.vItem.vInstruct.vValue ), true );
            }
    
            // Сохранение интерпретированного аргумента в виде строки.
            aTextString AnswerString;
            iee = tmpAnswer.ToString( AnswerString );
    
            if( iee != INF_ENGINE_SUCCESS )
                ReturnWithError( iee, "Failed to get Argument as String" );           
            
            ArgString = nAllocateObjects( vTmpAllocator, char, AnswerString.size() + 1 );
            if( !ArgString )
                ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
    
            memcpy( ArgString, AnswerString.ToConstChar(), AnswerString.size() );
            ArgString[AnswerString.size()] = '\0';
        }               

        InterpretedArgs.push_back( ArgString );
        if( InterpretedArgs.no_memory() )
            ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	// Результат выполнения тэга-функции.
	const char * FunctionResult;
	// Контекст, передаваемый в тэг-функцию.
	FunctionContext context( const_cast<Session*>( vSession ), &aAnswer );

	// Интерпретация тэга, представленного в виде тэга-функции.
	InfEngineErrors iee = InterpretTagLikeFunction( FunctionName, FunctionManipulator, InterpretedArgs, FunctionResult, aTextLength, &context, vMatchUserRequestData );

	// Если это простой тэг-функция.
	if( INF_ENGINE_WARN_UNSUCCESS == iee )
	{
		// Получение точки входа в функцию.
		FDLFucntion Function = aFunctionsRegistry.GetFunction( FunctionManipulator.GetId() );
		if( !Function )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Failed to call DL Function: function is not loaded" );

		// Добавление терминального аргумента.
		InterpretedArgs.push_back( nullptr );
		if( InterpretedArgs.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Выполнение функции.
		iee = Function( InterpretedArgs.get_buffer(), FunctionResult, aTextLength, &vTmpAllocator, &context );
	}

	// Разбор кода возврата функции.
	switch( iee )
	{
	/** Некорректное число аргументов. */
	case INF_ENGINE_ERROR_ARGC:
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Invalid internal data: Wrong arguments number in DL Function \"%s\" - %u",
						 vFunctionsData->GetFunctionsRegistry().GetFuncNameById( FunctionManipulator.GetId() ), InterpretedArgs.size() - 1 );

	/** Неполучилось выполнить функцию. */
	case INF_ENGINE_WARN_UNSUCCESS:
	/** Прекращение разбора ответа. */
	case INF_ENGINE_ERROR_DLF_SKIP_ANSWER:
		return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;

	case INF_ENGINE_SUCCESS:
		break;

	default:
		// Логгирование аргументов функции.
		const char ** pArgs = InterpretedArgs.get_buffer();
		unsigned int pos = 0;
		while( pArgs && *pArgs )
		{
			LogDebug( "Function argument[#%u]=|%s|", ++pos, *pArgs );
			pArgs++;
		}
		ReturnWithError( iee, "Failed to call DL Function; FuncId: %u; ErrorCode: %d", FunctionManipulator.GetId(), iee );
	}

	aText = FunctionResult;

	// Сохраняем вычисленное значение в кэше.
	if( TagId != static_cast<unsigned int>( -1 ) && !FunctionManipulator.IsMultiple() )
		vTagFunctionsCache->SetValue( TagId, aText, aTextLength );

	return INF_ENGINE_SUCCESS;

}

InfEngineErrors MatchedPattern::InterpretTagLikeFunction( const char * aFunctionName, const InfPatternItems::TagFunctionManipulator & aFunctionManipulator,
														  avector<const char*> & aInterpretedArgs, const char* & aFunctionResult, unsigned int & aTextLength,
														  FunctionContext * aContext, BFinder::Candidat * aMatchedData ) const
{
	InfEngineErrors iee = InterpretTagDictsMatchLF( aFunctionName, aFunctionManipulator, aInterpretedArgs, aFunctionResult, aTextLength, aContext, aMatchedData, nullptr, false );
	if( INF_ENGINE_SUCCESS == iee )
		return iee;
	if( INF_ENGINE_WARN_UNSUCCESS != iee )
		ReturnWithTrace( iee );

	iee = InterpretTagDictsNotMatchLF( aFunctionName, aFunctionManipulator, aInterpretedArgs, aFunctionResult, aTextLength, aContext, aMatchedData );
	if( INF_ENGINE_SUCCESS == iee )
		return iee;
	if( INF_ENGINE_WARN_UNSUCCESS != iee )
		ReturnWithTrace( iee );

	iee = InterpretTagDictsMatchExtLF( aFunctionName, aFunctionManipulator, aInterpretedArgs, aFunctionResult, aTextLength, aContext, aMatchedData );
	if( INF_ENGINE_SUCCESS == iee )
		return iee;
	if( INF_ENGINE_WARN_UNSUCCESS != iee )
		ReturnWithTrace( iee );

	// Данный тэг-функция не является самостоятельным тэгом.
	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors MatchedPattern::InterpretTagDictsMatchExtLF( const char * aFunctionName, const InfPatternItems::TagFunctionManipulator & aFunctionManipulator,
															 avector<const char*> & aInterpretedArgs, const char* & aFunctionResult, unsigned int & aTextLength,
															 FunctionContext * aContext, BFinder::Candidat * aMatchedData ) const
{
	// Первая строка подходящего словаря.
	const char * first_line = nullptr;
	// Проверка на вхождение фразы в заданные словари.
	InfEngineErrors iee = InterpretTagDictsMatchLF( aFunctionName, aFunctionManipulator, aInterpretedArgs, aFunctionResult, aTextLength, aContext, aMatchedData, &first_line, false );
	if( INF_ENGINE_WARN_UNSUCCESS == iee )
		return iee;
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	// Если вхождение в словарь не обнаружено.
	if( strcmp( CTRUE, aFunctionResult ) )
		return INF_ENGINE_SUCCESS;

	// Проверка результата поиска среди словарей.
	if( !first_line )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Проверка количества аргументов у тэга DictsMatchExt.
	if( aInterpretedArgs.size() < 2 )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Получение переменной.
	unsigned int var_name_len = strlen( aInterpretedArgs[1] );
	char * var_name = static_cast<char*>( vTmpAllocator.Allocate( var_name_len + 1 ) );
	if( !var_name )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	strcpy( var_name, aInterpretedArgs[1] );

	// Приведение имени переменной к нижнему регистру.
	if( nlrcSuccess != TextFuncs::ToLower( var_name, var_name_len ) )
		ReturnWithError( INF_ENGINE_ERROR_NANOLIB, INF_ENGINE_STRING_ERROR_INTERNAL );

	Vars::Id var_value_id = aContext->vSession->GetValueId( var_name, var_name_len );
	if( !var_value_id )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Записываем первую строку из подошедшего словаря в заданную переменную.
	unsigned int first_line_len = strlen( first_line );
	if( INF_ENGINE_SUCCESS != ( iee = aContext->vSession->SetTextValue( var_value_id, first_line, first_line_len ) ) )
		return iee;

	// Записываем в ответ информацию о изменении переменной в сессии.
	if( INF_ENGINE_SUCCESS != ( iee = aContext->vAnswer->AddInstruct( var_value_id, first_line, first_line_len, true ) ) )
	{
		switch( iee )
		{
		case INF_ENGINE_ERROR_INV_ARGS:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
		case INF_ENGINE_ERROR_NOFREE_MEMORY:
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		default:
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretTagDictsNotMatchLF( const char * aFunctionName, const InfPatternItems::TagFunctionManipulator & aFunctionManipulator,
															 avector<const char*> & aInterpretedArgs, const char* & aFunctionResult, unsigned int & aTextLength,
															 FunctionContext * aContext, BFinder::Candidat * aMatchedData ) const
{
	// Проверка на вхождение фразы в заданные словари.
	InfEngineErrors iee = InterpretTagDictsMatchLF( aFunctionName, aFunctionManipulator, aInterpretedArgs, aFunctionResult, aTextLength, aContext, aMatchedData, nullptr, true );
	if( INF_ENGINE_WARN_UNSUCCESS == iee )
		return iee;
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	// Инвертирование результата.
	if( !strcasecmp( CTRUE, aFunctionResult ) )
	{
		aFunctionResult = CFALSE;
		aTextLength = CFALSE_LEN;
	}
	else
	{
		aFunctionResult = CTRUE;
		aTextLength = CTRUE_LEN;
	}

	return INF_ENGINE_SUCCESS;

}

InfEngineErrors MatchedPattern::InterpretTagDictsMatchLF( const char * aFunctionName, const InfPatternItems::TagFunctionManipulator & aFunctionManipulator,
														  avector<const char*> & aInterpretedArgs, const char* & aFunctionResult, unsigned int & aTextLength,
														  FunctionContext * aContext, BFinder::Candidat * aMatchedData, const char ** aFirstLine, bool aNot ) const
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Проверка имени функции.
	if( aFirstLine )
	{
		*aFirstLine = nullptr;
		if( strcasecmp( "DictsMatchExt", aFunctionName ) )
			return INF_ENGINE_WARN_UNSUCCESS;
	}
	else
	{
		if( aNot )
		{
			if( strcasecmp( "DictsNotMatch", aFunctionName ) )
				return INF_ENGINE_WARN_UNSUCCESS;
		}
		else
		{
			if( strcasecmp( "DictsMatch", aFunctionName ) )
				return INF_ENGINE_WARN_UNSUCCESS;
		}
	}

	// Начало и конец вхождения проверяемой фразы.
	NDocImage::Iterator begin_pos;
	NDocImage::Iterator end_pos;

	// Получение первого аргумента.
	const void * pArg = aFunctionManipulator.GetArgument( 0 );
	if( !pArg )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Failed to get Manipulator for first argument of DictsMatch" );

	// Инициализация манипулятора для аргумента.
	InfPatternItems::TagFunctionArgumentManipulator ArgManipulator( pArg );

	// Получение списка элементов, из которых состоит аргумент.
	if( ArgManipulator.GetItemsNum() != 1 )
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "First argument of DictsMatch contains too much items: %i", ArgManipulator.GetItemsNum() );

	// Определение типа элемента.
	if( InfPatternItems::itStar == ArgManipulator.GetItemType( 0 ) )
	{
		// Поиск элемента в шаблон-вопросе, на который ссылается звёздочка.
		InfPatternItems::Star star( ArgManipulator.GetItem( 0 ) );
		unsigned int star_n = star.GetNum();
		unsigned int item_n = 0;
		for( item_n = 0; item_n < aMatchedData->vItemsNumber; ++item_n )
		{
			if( aMatchedData->vItems[item_n].vType == BFinder::Candidat::Item::Star ||
				aMatchedData->vItems[item_n].vType == BFinder::Candidat::Item::SuperStar )
			{
				if( star_n == 1 )
					break;
				--star_n;
			}
		}

		// Элемент, на который ссылается звёздочка не найден.
		if( item_n >= aMatchedData->vItemsNumber )
			ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "Too big star number: %i", star.GetNum() );

		begin_pos = aMatchedData->vItems[item_n].vBeginPos;
		end_pos = aMatchedData->vItems[item_n].vEndPos;
	}
	else if( InfPatternItems::itReference == ArgManipulator.GetItemType( 0 ) )
	{
		if( INF_ENGINE_SUCCESS != ( iee = GetReferenceBorders( ArgManipulator.GetItem( 0 ), aMatchedData, begin_pos, end_pos ) ) )
			ReturnWithTrace( iee );
	}
	else
		ReturnWithError( INF_ENGINE_WARN_UNSUCCESS, "First argument of DictsMatch contains item of invalid type: %u", ArgManipulator.GetItemType( 0 ) );


	for( unsigned int arg_n = 1 + ( aFirstLine ? 1 : 0 ); arg_n < aInterpretedArgs.size(); ++arg_n )
	{
		// Получение имени очередного словаря.
		const char * dict_name = aInterpretedArgs[arg_n];

		// Получение идентификатора очередного словаря.
		const unsigned int * dict_id = vDictsData->GetDictIdByName( dict_name, strlen( dict_name ) );
		if( dict_id )
		{
			// Получение списка успешных сопоставлений со словарём.
			BFinder::DictsMatchingResults::DictMatch dict_match;
			vDictsMatchingResults->GetMatch( *dict_id, dict_match );

			for( unsigned int match_n = 0; match_n < dict_match.GetMatchesNumber(); ++match_n )
			{
				// Получение очередного вхождения словаря.
				const BFinder::DictsMatchingResults::DictMatchInfo * match_info = dict_match.GetMatch( match_n );

				// Если подходящее вхождение найдено.
				if( ( match_info->vBeginPos == begin_pos || ( match_info->vBeginPos > begin_pos && match_info->vStarBefore ) ) &&
					( match_info->vEndPos == end_pos || ( match_info->vEndPos < end_pos && match_info->vStarAfter ) ) )
				{

					// Если это тэг DictsMatchExt.
					if( aFirstLine )
					{
						// Загружаем подошедший словарь.
						InfDictWrapManipulator dict_manip;
						if( INF_ENGINE_SUCCESS != ( iee = vDictsData->GetDictById( match_info->vDictId, dict_manip ) ) )
							ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

						// Выбираем из подошедшего словаря первую строку.
						if( dict_manip.GetNum() )
						{
							InfDictStringManipulator dict_string;
							if( INF_ENGINE_SUCCESS != ( iee = dict_manip.GetString( 0, dict_string ) ) )
								ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

							*aFirstLine = dict_string.GetOriginal();
						}
						else
							ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
					}

					aFunctionResult = CTRUE;
					aTextLength = CTRUE_LEN;
					return INF_ENGINE_SUCCESS;
				}
			}
		}
		else
			LogWarn( "Unknown dictionary \"%s\" in tag DictsMatch", dict_name );
	}

	// Фразу пользователя в заданных словарях найти не удалось.
	aFunctionResult = CFALSE;
	aTextLength = CFALSE_LEN;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretRSS( const void * aPatternItem, iAnswer & aAnswer ) const
{
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация тэга.
	InfPatternItems::TagRSSManipulator tag( aPatternItem );

	InfEngineErrors iee = aAnswer.AddRSS( tag.GetURL(), tag.GetURLLength(), tag.GetAlt(), tag.GetAltLength(), tag.GetOffset(),
										  tag.GetShowTitle(), tag.GetShowLink(), tag.GetUpdatePeriod(), false );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	else
		return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretExtendAnswer( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel ) const
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Проверка ограничения глубины вложенности.
	if( aNestingLevel > 2 )
		return INF_ENGINE_SUCCESS;

	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация тэга.
	InfPatternItems::TagExtendAnswerManipulator tag;
	tag.SetBuffer( aPatternItem );
	if( !tag.GetCount() )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Выбор случайного ответа из списка возможных.
	unsigned int * shuffled_patterns = nAllocateObjects( vTmpAllocator, unsigned int, tag.GetCount() );
	for( unsigned int pattern_n = 0; pattern_n < tag.GetCount(); ++pattern_n )
		shuffled_patterns[pattern_n] = pattern_n;
	for( unsigned int pattern_n = 0; pattern_n < tag.GetCount(); ++pattern_n )
	{
		std::swap( shuffled_patterns[pattern_n], shuffled_patterns[pattern_n + rand() % ( tag.GetCount() - pattern_n )] );
		unsigned int id = tag.GetId( shuffled_patterns[pattern_n] );
		InfPatternRO pattern_manip;
		if( INF_ENGINE_SUCCESS != ( iee = vPatternsData->GetPatternByPatternId( id, pattern_manip ) ) )
		{
			if( INF_ENGINE_WARN_UNSUCCESS == iee )
			{
				aAnswer.Reset();
				return iee;
			}
			ReturnWithTrace( iee );
		}

		unsigned int * shuffled_answers = nAllocateObjects( vTmpAllocator, unsigned int, pattern_manip.GetAnswersNum() );
		for( unsigned int answer_n = 0; answer_n < pattern_manip.GetAnswersNum(); ++answer_n )
			shuffled_answers[answer_n] = answer_n;
		for( unsigned int answer_n = 0; answer_n < pattern_manip.GetAnswersNum(); ++answer_n )
		{
			std::swap( shuffled_answers[answer_n], shuffled_answers[answer_n + rand() % ( pattern_manip.GetAnswersNum() - answer_n )] );
			InfPatternItems::AnswerRO answer = pattern_manip.GetAnswer( shuffled_answers[answer_n] );

			// Проверка условий уровня шаблона для данного шаблон-ответа.
			if( answer.HasTLCS() )
			{
				unsigned int tlcs_chache_id = 0;
				// Поиск результата в кэше.
				if( INF_ENGINE_WARN_UNSUCCESS == ( iee = vTLCSCache->Find( vMatchUserRequestData->vPatternId,
																		   vMatchUserRequestData->vQuestionId,
																		   tlcs_chache_id ) ) )
				{
					// Вычисление результа и добавление его в кэш.
					if( INF_ENGINE_SUCCESS != ( iee = TestTLCS( pattern_manip, tlcs_chache_id ) ) )
						ReturnWithTrace( iee );
				}
				else if( INF_ENGINE_SUCCESS != iee )
					ReturnWithTrace( iee );

				// Пропуск шаблон-ответа, если он не удовлетворяет условным операторам уровня шаблона.
				if( !vTLCSCache->Ok( tlcs_chache_id, answer.GetTLCS() ) )
					continue;
			}

			unsigned int answer_size = aAnswer.Size();
			if( INF_ENGINE_SUCCESS == ( iee = MakeResult( answer, aAnswer, aNestingLevel + 1, false ) ) )
			{
				for( unsigned int item_n = answer_size; item_n < aAnswer.Size(); ++item_n )
				{
					if( aAnswer[item_n].vType == iAnswer::Item::Type::Text ||
						aAnswer[item_n].vType == iAnswer::Item::Type::Href ||
						aAnswer[item_n].vType == iAnswer::Item::Type::Inf ||
						aAnswer[item_n].vType == iAnswer::Item::Type::OpenWindow ||
						aAnswer[item_n].vType == iAnswer::Item::Type::RSS )
						return INF_ENGINE_SUCCESS;
				}
			}
			if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors MatchedPattern::InterpretPre( const void * aPatternItem, iAnswer & aAnswer ) const
{
	const char * Text = nullptr;
	unsigned int TextLength = 0;

	// Получение данных.
	InfEngineErrors iee = InterpretPre( aPatternItem, Text, TextLength );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = aAnswer.AddText( Text, TextLength, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors MatchedPattern::InterpretPre( const void * aPatternItem, const char* & aText, unsigned int & aTextLength ) const
{
	// Проверка аргументов.
	if( !aPatternItem )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Интерпретация текстового элемента.
	InfPatternItems::TagPreManipulator pre( aPatternItem );

	aText = pre.GetText( aTextLength );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors EliMatchedPattern::MakeResult( const InfPatternItems::ArrayManipulator & aPatternAnswer, iAnswer & aAnswer,
											   unsigned int aNestingLevel, bool aCheckIfCondition ) const
{
	InfEngineErrors iee;

	// Перебор и интерпретация элементов шаблон-ответа.
	for( unsigned int i = 0; i < aPatternAnswer.GetItemsNum(); i++ )
	{
		switch( aPatternAnswer.GetItemType( i ) )
		{
		case InfPatternItems::itEliStar:
			iee = InterpretEliStar( aPatternAnswer.GetItem( i ), aAnswer, aNestingLevel );
			break;
		case InfPatternItems::itEliReference:
			iee = InterpretEliReference( aPatternAnswer.GetItem( i ), aAnswer, aNestingLevel );
			break;
		default:
			iee = InterpretItem( aPatternAnswer, i, aAnswer, aNestingLevel, aCheckIfCondition );
		}
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	// Перенос приоритетных инструкций в конец ответа.
	iee = aAnswer.ProcessPriorityItems();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors EliMatchedPattern::InterpretEliStar( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel ) const
{
	const char * Star = nullptr;
	unsigned int StarLength = 0;

	InfEngineErrors iee = InterpretEliStar( aPatternItem, Star, StarLength, aNestingLevel );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = aAnswer.AddText( Star, StarLength, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors EliMatchedPattern::InterpretEliStar( const void * aPatternItem, const char* & aStar, unsigned int & aStarLength, unsigned int aNestingLevel ) const
{
	// Проверка аргументов.
	if( !aPatternItem || !vUserRequest || !vInfPreResponse || !vUserPreRequest || !vMatchUserRequestData )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Ограничения вложенности при шаблон-ответах, содержащих обращения к другим шаблонам.
	if( aNestingLevel )
		return INF_ENGINE_SUCCESS;

	// Интерпретация ссылки на подстроку запроса пользователя, соотнесенную с звездочкой для эллипсисов.
	InfPatternItems::EliStarManipulator star( aPatternItem );

	// Выбор строки запроса, к которой относится звездочка.
	const char * Request = nullptr;
	unsigned int RequestLength = 0;
	BFinder::Candidat * MatchedData = nullptr;
	switch( star.GetBinding() )
	{
	case InfPatternItems::EliStar::bUserRequest:
		Request = vUserRequest;
		RequestLength = vUserRequestLength;
		MatchedData = vMatchUserRequestData;
		break;
	case InfPatternItems::EliStar::bInfPreResponse:
		Request = vInfPreResponse;
		RequestLength = vInfPreResponseLength;
		MatchedData = vMatchPreInfMatchData;
		break;
	case InfPatternItems::EliStar::bUserPreRequest:
		Request = vUserPreRequest;
		RequestLength = vUserPreRequestLength;
		MatchedData = vMatchPreUserMatchData;
		break;
	}
	InfEngineErrors iee = InterpretStar( aPatternItem, Request, RequestLength, Request, RequestLength, nullptr, MatchedData, aStar, aStarLength, aNestingLevel );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors EliMatchedPattern::InterpretEliReference( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel ) const
{
	const char * Reference = nullptr;
	unsigned int ReferenceLength = 0;

	InfEngineErrors iee = InterpretEliReference( aPatternItem, Reference, ReferenceLength, aNestingLevel );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = aAnswer.AddText( Reference, ReferenceLength, false );
	switch( iee )
	{
	case INF_ENGINE_SUCCESS:
		return INF_ENGINE_SUCCESS;
	case INF_ENGINE_ERROR_INV_ARGS:
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	default:
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
	}
}

InfEngineErrors EliMatchedPattern::InterpretEliReference( const void * aPatternItem, const char* & aReference,
														  unsigned int & aReferenceLength, unsigned int aNestingLevel ) const
{
	// Проверка аргументов.
	if( !aPatternItem || !vUserRequest || !vInfPreResponse || !vUserPreRequest || !vMatchUserRequestData )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Ограничения вложенности при шаблон-ответах, содержащих обращения к другим шаблонам.
	if( aNestingLevel )
		return INF_ENGINE_SUCCESS;

	InfPatternItems::EliReferenceManipulator reference( aPatternItem );

	// Выбор строки запроса, к которой относится звездочка.
	const char * Request = nullptr;
	unsigned int RequestLength = 0;
	BFinder::Candidat * MatchedData = nullptr;
	switch( reference.GetBinding() )
	{
	case InfPatternItems::EliReference::bUserRequest:
		Request = vUserRequest;
		RequestLength = vUserRequestLength;
		MatchedData = vMatchUserRequestData;

		//NormalizedRequest = vNormalizedRequest;
		//NormalizedRequestLength = vNormalizedRequestLength;
		//RequestMap = vRequestMap;
		break;
	case InfPatternItems::EliReference::bInfPreResponse:
		Request = vInfPreResponse;
		RequestLength = vInfPreResponseLength;
		MatchedData = vMatchPreInfMatchData;
		break;
	case InfPatternItems::EliReference::bUserPreRequest:
		Request = vUserPreRequest;
		RequestLength = vUserPreRequestLength;
		MatchedData = vMatchPreUserMatchData;
		break;
	}

	InfEngineErrors iee = InterpretReference( aPatternItem, Request, RequestLength, Request, RequestLength,
											  nullptr, MatchedData, aReference, aReferenceLength, aNestingLevel );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors EliMatchedPattern::InterpretValue( InfPatternItems::ArrayManipulator & aValuePattern, aTextString & aResultValue, unsigned int aNestingLevel ) const
{
	InfEngineErrors iee;

	aResultValue.clear();

	for( unsigned int i = 0; i < aValuePattern.GetItemsNum(); i++ )
	{
		const char * Buffer = nullptr;
		unsigned int BufferLength = 0;

		switch( aValuePattern.GetItemType( i ) )
		{
		case InfPatternItems::itText:
			if( ( iee = InterpretText( aValuePattern.GetItem( i ), Buffer, BufferLength ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itPre:
			if( ( iee = InterpretPre( aValuePattern.GetItem( i ), Buffer, BufferLength ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itStar:
			if( ( iee = InterpretStar( aValuePattern.GetItem( i ), vUserRequest, vUserRequestLength,
									   nullptr, 0, nullptr, vMatchUserRequestData, Buffer, BufferLength,
									   aNestingLevel ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itEliStar:
			if( ( iee = InterpretEliStar( aValuePattern.GetItem( i ), Buffer, BufferLength,
										  aNestingLevel ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itReference:
			if( ( iee = InterpretReference( aValuePattern.GetItem( i ), vUserRequest, vUserRequestLength, nullptr, 0, nullptr,
											vMatchUserRequestData, Buffer, BufferLength, aNestingLevel ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itDictRef:
			if( ( iee = InterpretDictRef( aValuePattern.GetItem( i ), vUserRequest, vUserRequestLength, nullptr,
										  vMatchUserRequestData, Buffer, BufferLength ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		case InfPatternItems::itEliReference:
			if( ( iee = InterpretEliReference( aValuePattern.GetItem( i ), Buffer, BufferLength, aNestingLevel ) ) != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			break;
		default:
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		}

		if( !aResultValue.empty() && aResultValue.append( " ", 1 ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		if( aResultValue.append( Buffer, BufferLength ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	return INF_ENGINE_SUCCESS;
}
