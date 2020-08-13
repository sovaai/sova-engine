#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <string>
#include <vector>

enum class DLErrorId
{
	peToManyQuestions = 10000,
	peInvalidPatternString = 10001,
	pePatternWithoutQuestions = 10002,
	pePatternWithoutAnswers = 10003,
	peTextWithoutTerms = 10004,
	peQuestionIsEmpty = 10005,

	peQuestionInvalidTag = 10100,
	peThatInvalidTag = 10101,
	peAnswerInvalidTag = 10102,

	peReferenceWithoutStartPos = 10200,
	peReferenceWithoutEndPos = 10201,
	peReferenceIsEmpty = 10202,

	peDictWithoutOpenRoundBrace = 10300,
	peDictNameInvalid = 10301,
	peDictWithoutCloseRoundBrace = 10302,
	peDictWithoutCloseSquareBrace = 10303,
	peDictUnregistredName = 10304,
	peDictWithoutCloseTriangularBrace = 10305,

	peInlineDictWithoutEndDelimiter = 10400,
	peInlineDictEmptyVariant = 10401,
	peInlineDictRestrictedSymbol = 10402,

	peTextRestrictedSymbol = 10500,

	peReferenceInvalid = 10600,
	peDictRefWithoutCloseBrace = 10601,
	peDictRefWithoutCloseSquareBrace = 10602,
	peDictRefInvalidNumber = 10603,
	peDictRefCommaExpected = 10604,
	peDictRefInvalidType = 10605,
	peDictRefNumberIsTooBig = 10606,
	peDictRefNumberMayBeTooBig = 10607,

	peExternalServiceOpenRoundBrace = 10700,
	peExternalServiceCloseRoundBrace = 10701,
	peExternalServiceCloseSquareBrace = 10702,
	peExternalServiceArgumentEqual = 10703,
	peExternalServiceArgumentQuote = 10704,
	peExternalServiceArgumentDuplicate = 10705,
	peExternalServiceUnregistredService = 10706,
	peExternalServiceArgumentRestrictedTag = 10707,
	peExternalServiceArgumentOrder = 10708,
	peExternalServiceArgumentInvalid = 10709,
	peExternalServiceInvalidServiceName = 10710,

	peBrCloseSquareBrace = 11000,

	peVariableName = 11100,
	peVariableUnregistred = 11101,

	peInstructCloseSquareBrace = 11200,
	peInstructEqual = 11201,
	peInstructQuote = 11202,
	peInstructValue = 11203,
	peInstructReadOnlyVariable,

	peInfCloseSquareBrace = 11300,
	peInfValue = 11301,
	peInfInvalidRequest = 11302,
	peInfEmptyRequest = 11303,
	peInfOpenQuote = 11304,
	peInfCloseQuote = 11305,
	peInfOpenBrace = 11306,
	peInfCloseBrace = 11307,


	peHrefOpenRoundBrace = 11400,
	peHrefCloseRoundBrace = 11401,
	peHrefCloseSquareBrace = 11402,
	peHrefOpenCurlyBrace = 11403,
	peHrefCloseCurlyBrace = 11404,
	peHrefURL = 11405,
	peHrefTarget = 11406,
	peHrefLink = 11407,
	peMaybeCloseRoundBraceMissed = 11408,
	peMaybeInlineDictUsed = 11409,

	peOpenWindowOpenRoundBrace = 11500,
	peOpenWindowCloseRoundBrace = 11501,
	peOpenWindowCloseSquareBrace = 11502,
	peOpenWindowURL = 11503,
	peOpenWindowTarget = 11504,

	peIfOpenRoundBrace = 11600,
	peIfCloseRoundBrace = 11601,
	peIfCloseSquareBrace = 11602,
	peIfOpenCurlyBrace = 11603,
	peIfCloseCurlyBrace = 11604,
	peIfRestrictedTag = 11605,

	peStarCloseSquareBrace = 11700,

	peConditionVarNameInvalid = 11800,
	peConditionUnregistredVarName = 11801,
	peConditionInvalidValue = 11802,
	peConditionInvalid = 11803,

	peGoToOpenRoundBrace = 11900,
	peGoToCloseRoundBrace = 11902,
	peGoToCloseSquareBrace = 11903,
	peGoToInvalidPatternName = 11901,

	peFunctionInvalidArgType = 12001,
	peFunctionInvalidArgMin = 12002,
	peFunctionInvalidArgMax = 12003,
	peFunctionInvalidResType = 12004,
	peFunctionInvalidName = 12005,
	peFunctionInvalidFormat = 12006,
	peFunctionInvalidArgumentValue = 12007,
	peFunctionCloseBraketMissed = 12008,

	peParseUnexpectedEndOfLine = 13000,
	peParseUnexpectedCharacter = 13001,
	peParseSecondConditionArgExpected = 13002,
	peParseSecondConditionArgIsEmpty = 13003,
	peParseUnexpectedCharacterAfterCheckEmpty = 13004,

	peParseUnknownAliasResultType = 14000,
	peParseAliasNameExpected = 14001,
	peParseInvalidAliasName = 14002,
	peParseInvalidAliasArgName = 14003,
	peAliasOpenRoundBrace = 14004,
	peAliasCloseRoundBrace = 14005,
	peAliasCloseSquareBrace = 14006,
	peParseUnknownAliasArgType = 14007,
	peAliasCommaExpexted = 14008,
	peAliasValidValuesListIsEmpty = 14009,
	peAliasInvalidValue = 14010,
	peParseUnknownAliasArgName = 14011,
	peAliasInvalidArgMax = 14012,
	peAliasInvalidArgMin = 14013,
	peAliasInvalidArgValue = 14014,
	peFuncOrAliasInvalidName = 14015,
	peFuncOrAliasUnknownName = 14016,
	peFuncOrAliasArgOpenQuote = 14017,
	peFuncOrAliasArgCloseQuote = 14018,
	peFuncOrAliasArgComma = 14019,

	peRSSOpenRoundBrace = 15000,
	peRSSCloseRoundBrace = 15001,
	peRSSCloseSquareBrace = 15002,
	peRSSURLInvalid = 15003,
	peRSSAltInvalid = 15004,
	peRSSOffsetInvalid = 15005,
	peRSSShowTitleInvalid = 15006,
	peRSSShowLinkInvalid = 15007,
	peRSSUpdatePeriodInvalid = 15008,

	peBreakCloseSquareBrace = 16000,

	peExternAnswerOpenRoundBrace = 17000,
	peExternAnswerCloseRoundBrace = 17001,
	peExternAnswerCloseSquareBrace = 17002,
	peExternAnswerEmptyId = 17003,
	peExternAnswerEmpty = 17004,
	peExternAnswerUnregistredId = 17005,
	peExternAnswerOpenQuote = 17006,
	peExternAnswerCloseQuote = 17007,
	peExternAnswerComma = 17008,

	pePatternIdIsNotUnique = 18000,
	pePatternUnnecessarySpace = 19000,

	peTLCS_ElsifWithoutIf = 22000,
	peTLCS_ElseWithoutIf = 22001,
	peTLCS_EndifWithoutIf = 22002,
	peTLCS_EOLExpected = 22003,
	peTLCS_CaseWithoutSwitch = 22004,
	peTLCS_DefaultWithoutCase = 22005,
	peTLCS_CaseExpected = 22006,
	peTLCS_SwitchAfterSwitch = 22007,
	peTLCS_IncompleteOperatorIf = 22008,
	peTLCS_IncompleteOperatorSwitch = 22009,
	peTLCS_IncompleteOperator = 22010,

	peSwitchOpenRoundBrace = 23000,
	peSwitchCloseRoundBrace = 23001,
	peSwitchCloseSquareBrace = 23002,
	peSwitchBlockExpected = 23003,
	peDefaultBlockExpected = 23004,
	peSwitchTooManyArgs = 23005,

	peCaseOpenRoundBrace = 23100,
	peCaseCloseRoundBrace = 23101,
	peCaseCloseSquareBrace = 23102,
	peCaseOpenCurlyBrace = 23103,
	peCaseCloseCurlyBrace = 23104,
	peCaseTooManyArgs = 23105,

	peDefaultCloseSquareBrace = 23200,
	peDefaultOpenCurlyBrace = 23201,
	peDefaultCloseCurlyBrace = 23202,

	pePreOpenBrace = 24000,
	pePreCloseBrace = 24001,
	pePreEmpty = 24002,

	peInstrLabelIsEmpty = 25000,
	peInstrLabelIncorrectName = 25001,
	peInstrLabelIsNotUniq = 25002,
	peInstrLabelTooMach = 25003,
	peInstrLabelUnexpected = 25004,

	peNothingCloseSquareBrace = 26000,

	peDisableautovarsCloseSquareBrace = 27000,

	peInvalidModifier = 28000,

	peModifierFirstWithoutRequestEvent,
	peModifierThatAndEvent,
	peOrderAnswerBeforeQuestion = 29000,
	peOrderQuestionAfterAnswer = 29001,
	peOrderConditionAfterQuestion = 29002,
	peOrderQestionAfterThat = 29003,
	peOrderConditionAfterThat = 29004,
	peOrderThatAfterAnswer = 29005,
	peOrderThatBeforeQuestion = 29006,
	peOneThatOnly = 29007,


	peQuestionsAndEvents,
	peQuestionsLooksLikeEvent,

	peStartListCloseSquareBrace,
	peListItemCloseSquareBrace,
	peEndListCloseSquareBrace,
	peEndListToMuch,
	peDisbalanseIfLists,
	peDisbalanseSwicthLists,
};

/**
 *  Представление ошибки в текстовом виде.
 * @param aErrorDescription - Описание ошибки.
 */
inline const char * ConvertErrorDescriptionToString( DLErrorId aErrorId )
{
	switch( aErrorId ) {
		case DLErrorId::peToManyQuestions:
			return "Превышен предел числа шаблон-вопросов в одном шаблоне";
		case DLErrorId::peInvalidPatternString:
			return "Некорректная строка шаблона";
		case DLErrorId::pePatternWithoutQuestions:
			return "Шаблон без шаблон-вопросов";
		case DLErrorId::pePatternWithoutAnswers:
			return "Шаблон без шаблон-ответов";
		case DLErrorId::peTextWithoutTerms:
			return "Безтерменный элемент шаблон-вопроса";

		case DLErrorId::peQuestionInvalidTag:
			return "Некорректный тэг в шаблон-вопросе";
		case DLErrorId::peThatInvalidTag:
			return "Некорректный тэг в шаблон-that'е.";
		case DLErrorId::peAnswerInvalidTag:
			return "Некорректный тэг в шаблон-ответе.";
		case DLErrorId::peQuestionIsEmpty:
			return "Пустой шаблон-вопрос.";

		case DLErrorId::peReferenceWithoutStartPos:
			return "Референция без начала";
		case DLErrorId::peReferenceWithoutEndPos:
			return "Референция без конца";
		case DLErrorId::peReferenceIsEmpty:
			return "Пустая референция в шаблон-вопросе";

		case DLErrorId::peDictWithoutOpenRoundBrace:
			return "После тэга dict должна идти открывающаяся круглая скобка";
		case DLErrorId::peDictNameInvalid:
			return "Некорректное имя словаря";
		case DLErrorId::peDictWithoutCloseRoundBrace:
			return "В тэге dict пропущена закрывающаяся круглая скобка";
		case DLErrorId::peDictWithoutCloseSquareBrace:
			return "В тэге dict пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peDictUnregistredName:
			return "Неизвестный словарь";
		case DLErrorId::peDictWithoutCloseTriangularBrace:
			return "В тэге dict пропущена закрывающаяся треугольная скобка";

		case DLErrorId::peInlineDictWithoutEndDelimiter:
			return "Inline словарь без окончания";
		case DLErrorId::peInlineDictEmptyVariant:
			return "Пустой вариант в inline-словаре";
		case DLErrorId::peInlineDictRestrictedSymbol:
			return "Некорректный символ в inline-словаре";

		case DLErrorId::peTextRestrictedSymbol:
			return "Некорректный символ в текстовой строке";

		case DLErrorId::peReferenceInvalid:
			return "Некорректная референция";
		case DLErrorId::peDictRefWithoutCloseBrace:
			return "Пропущена закрывающая круглая скобка в описании ссылки на словарь";
		case DLErrorId::peDictRefWithoutCloseSquareBrace:
			return "Пропущена закрывающая квадратная скобка в описании ссылки на словарь";
		case DLErrorId::peDictRefInvalidNumber:
			return "Ошибка в указании номера ссылки на словарь";
		case DLErrorId::peDictRefCommaExpected:
			return "В описании ссылки на словарь пропущена запятая";
		case DLErrorId::peDictRefInvalidType:
			return "Ошибка в указании типа ссылки на словарь";
		case DLErrorId::peDictRefNumberIsTooBig:
			return "В ссылке на словарь указан слишком большой номер словаря";
		case DLErrorId::peDictRefNumberMayBeTooBig:
			return "Словарь с указанным в ссылке номером существует не во всех шаблон-вопросах данного шаблона";

		case DLErrorId::peExternalServiceOpenRoundBrace:
			return "В тэге ExternalService пропущена открывающаяся круглая скобка";
		case DLErrorId::peExternalServiceCloseRoundBrace:
			return "В тэге ExternalService пропущена закрывающаяся круглая скобка";
		case DLErrorId::peExternalServiceCloseSquareBrace:
			return "В тэге ExternalService пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peExternalServiceArgumentEqual:
			return "В тэге ExternalService после имени аргумента пропущен знак равенства";
		case DLErrorId::peExternalServiceArgumentQuote:
			return "В аргументе тэга ExternalService пропущена кавычка";
		case DLErrorId::peExternalServiceArgumentDuplicate:
			return "В тэге ExternalService дублируются аргументы";
		case DLErrorId::peExternalServiceUnregistredService:
			return "В тэге ExternalService указан неизвестный сервис";
		case DLErrorId::peExternalServiceArgumentRestrictedTag:
			return "В значении аргумента тэга ExternalService используется некорректный тэг";
		case DLErrorId::peExternalServiceArgumentOrder:
			return "В тэге ExternalService неверный порядок аргументов";
		case DLErrorId::peExternalServiceArgumentInvalid:
			return "Некорректный аргумент тэга ExternalService";
		case DLErrorId::peExternalServiceInvalidServiceName:
			return "В тэге ExternalService указан некорректный сервис";

		case DLErrorId::peBrCloseSquareBrace:
			return "В тэге Br пропущена закрывающаяся квадратная скобка";

		case DLErrorId::peVariableName:
			return "Некорректное имя переменной";
		case DLErrorId::peVariableUnregistred:
			return "Неизвестная переменная";

		case DLErrorId::peInstructCloseSquareBrace:
			return "В инструкции пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peInstructEqual:
			return "В инструкции пропущен знак равенства";
		case DLErrorId::peInstructQuote:
			return "В инструкции пропущена кавычка";
		case DLErrorId::peInstructValue:
			return "Некорректное значение в инструкции";
	case DLErrorId::peInstructReadOnlyVariable:
		return "Попытка изменения readonly переменной";

		case DLErrorId::peInfCloseSquareBrace:
			return "В тэге Inf пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peInfValue:
			return "Некорректное значение в тэге Inf";
		case DLErrorId::peInfInvalidRequest:
			return "Некорректный запрос в тэге Inf";
		case DLErrorId::peInfEmptyRequest:
			return "Пустой запрос в тэге Inf";
		case DLErrorId::peInfOpenQuote:
			return "Пропущена открывающаяся кавычка в запросе в тэге Inf";
		case DLErrorId::peInfCloseQuote:
			return "Пропущена закрывающаяся кавычка в запросе в тэге Inf";
		case DLErrorId::peInfOpenBrace:
			return "Пропущена открывающаяся фигурная скобка в тэге Inf";
		case DLErrorId::peInfCloseBrace:
			return "Пропущена закрывающаяся фигурная скобка в тэге Inf";

		case DLErrorId::peHrefOpenRoundBrace:
			return "В тэге Href пропущена открывающаяся круглая скобка";
		case DLErrorId::peHrefCloseRoundBrace:
			return "В тэге Href пропущена закрывающаяся круглая скобка";
		case DLErrorId::peHrefCloseSquareBrace:
			return "В тэге Href пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peHrefOpenCurlyBrace:
			return "В тэге Href пропущена открывающаяся фигурная скобка";
		case DLErrorId::peHrefCloseCurlyBrace:
			return "В тэге Href пропущена закрывающаяся фигурная скобка";
		case DLErrorId::peHrefURL:
			return "В тэге Href некорректное значение аргумента URL";
		case DLErrorId::peHrefTarget:
			return "В тэге Href некорректное значение аргумента Target";
		case DLErrorId::peHrefLink:
			return "В тэге Href некорректное значение аргумента Link";
		case DLErrorId::peMaybeCloseRoundBraceMissed:
			return "Возможно, в тэге Href пропущена закрывающаяся круглая скобка";
		case DLErrorId::peMaybeInlineDictUsed:
			return "Возможно, в тэге Href был использован inline-словарь, что является недопуститимым";

		case DLErrorId::peOpenWindowOpenRoundBrace:
			return "В тэге OpenWindow пропущена открывающаяся круглая скобка";
		case DLErrorId::peOpenWindowCloseRoundBrace:
			return "В тэге OpenWindow пропущена закрывающаяся круглая скобка";
		case DLErrorId::peOpenWindowCloseSquareBrace:
			return "В тэге OpenWindow пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peOpenWindowURL:
			return "В тэге OpenWindow некорректное значение аргумента URL";
		case DLErrorId::peOpenWindowTarget:
			return "В тэге OpenWindow некорректное значение аргумента Target";

		case DLErrorId::peIfOpenRoundBrace:
			return "В тэге If пропущена открывающаяся круглая скобка";
		case DLErrorId::peIfCloseRoundBrace:
			return "В тэге If пропущена закрывающаяся круглая скобка";
		case DLErrorId::peIfCloseSquareBrace:
			return "В тэге If пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peIfOpenCurlyBrace:
			return "В тэге If пропущена открывающаяся фигурная скобка";
		case DLErrorId::peIfCloseCurlyBrace:
			return "В тэге If пропущена закрывающаяся фигурная скобка";
		case DLErrorId::peIfRestrictedTag:
			return "Использование некорректного тэга в тэге If";

		case DLErrorId::peStarCloseSquareBrace:
			return "После звездочки пропущена закрывающаяся квадратная скобка";

		case DLErrorId::peConditionVarNameInvalid:
			return "Некорректное имя переменной в условии";
		case DLErrorId::peConditionUnregistredVarName:
			return "Неизвестная переменная в условии";
		case DLErrorId::peConditionInvalidValue:
			return "Некорректное значение в условии";
		case DLErrorId::peConditionInvalid:
			return "Некорректное условие";

		case DLErrorId::peGoToOpenRoundBrace:
			return "В тэге GetAnswer пропущена открывающаяся круглая скобка";
		case DLErrorId::peGoToCloseRoundBrace:
			return "В тэге GetAnswer пропущена закрывающаяся круглая скобка";
		case DLErrorId::peGoToCloseSquareBrace:
			return "В тэге GetAnswer пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peGoToInvalidPatternName:
			return "Некорректная метка в тэге GetAnswer";

		case DLErrorId::peFunctionInvalidArgType:
			return "В функцию передан аргумент несоответствующего типа";
		case DLErrorId::peFunctionInvalidArgMin:
			return "В функцию передано недостаточное количество аргументов";
		case DLErrorId::peFunctionInvalidArgMax:
			return "В функцию передано слишком много аргументов";
		case DLErrorId::peFunctionInvalidResType:
			return "Неверный тип функции";
		case DLErrorId::peFunctionInvalidName:
			return "Незвестная функция";
		case DLErrorId::peFunctionInvalidFormat:
			return "Неверный формат использования функции";
		case DLErrorId::peFunctionInvalidArgumentValue:
			return "Некорректный аргумент функции";
		case DLErrorId::peFunctionCloseBraketMissed:
			return "У тэга-функции пропущена закрывающая квадратная скобка";


		case DLErrorId::peParseUnexpectedEndOfLine:
			return "Неожиданный конец строки";
		case DLErrorId::peParseUnexpectedCharacter:
			return "Неожиданный символ";
		case DLErrorId::peParseSecondConditionArgExpected:
			return "Ожидается второй аргумент условия If";
		case DLErrorId::peParseSecondConditionArgIsEmpty:
			return "Второй аргумент условия If пуст";
		case DLErrorId::peParseUnexpectedCharacterAfterCheckEmpty:
			return "Неожиданный символ после проверки переменной на пустоту";

		case DLErrorId::peParseUnknownAliasResultType:
			return "Неизвестный тип результата алиаса";
		case DLErrorId::peParseAliasNameExpected:
			return "Ожидается имя алиаса";
		case DLErrorId::peParseInvalidAliasName:
			return "Имя алиаса содержит недопустимые символы";
		case DLErrorId::peParseInvalidAliasArgName:
			return "Имя аргумента алиаса содержит недопустимые символы";
		case DLErrorId::peAliasOpenRoundBrace:
			return "Пропущена открывающая скобка после имени алиаса";
		case DLErrorId::peAliasCloseRoundBrace:
			return "Пропущена закрывающая скобка после списка аргументов алиаса";
		case DLErrorId::peAliasCloseSquareBrace:
			return "Пропущена закрывающая квадратная скобка в определении аргументов алиаса";
		case DLErrorId::peParseUnknownAliasArgType:
			return "Неизвестный тип аргумента алиаса";
		case DLErrorId::peAliasCommaExpexted:
			return "Ожидается запятая";
		case DLErrorId::peAliasValidValuesListIsEmpty:
			return "Пустой список допустимых значений аргумента типа TEXT";
		case DLErrorId::peAliasInvalidValue:
			return "Ошибка разбора допустимого значения аргумента алиаса";
		case DLErrorId::peParseUnknownAliasArgName:
			return "Сылка на неизвестный аргумент в теле алиаса";
		case DLErrorId::peAliasInvalidArgMax:
			return "Алиасу передано слишком большое число аргументов";
		case DLErrorId::peAliasInvalidArgMin:
			return "Алиасу передано недостаточное число аргументов";
		case DLErrorId::peAliasInvalidArgValue:
			return "Значение переданного алиасу аргумента не является допустимым.";
		case DLErrorId::peFuncOrAliasInvalidName:
			return "Недопустимый формат имени функции или алиаса";
		case DLErrorId::peFuncOrAliasUnknownName:
			return "Неизвестное имя функции или алиаса";

		case DLErrorId::peFuncOrAliasArgOpenQuote:
			return "Пропущена открывающая кавычка перед аргументом алиаса или функции";
		case DLErrorId::peFuncOrAliasArgCloseQuote:
			return "Пропущена закрывающая кавычка после аргумента алиаса или функции";
		case DLErrorId::peFuncOrAliasArgComma:
			return "Пропущена запятая между аргументами алиаса или функции";

		case DLErrorId::peRSSOpenRoundBrace:
			return "В тэге RSS пропущена открывающаяся круглая скобка";
		case DLErrorId::peRSSCloseRoundBrace:
			return "В тэге RSS пропущена закрывающаяся круглая скобка";
		case DLErrorId::peRSSCloseSquareBrace:
			return "В тэге RSS пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peRSSURLInvalid:
			return "В тэге RSS некорректный первый аргумент(URL)";
		case DLErrorId::peRSSAltInvalid:
			return "В тэге RSS некорректный второй аргумент(Alt)";
		case DLErrorId::peRSSOffsetInvalid:
			return "В тэге RSS некорректный третий аргумент(Offset)";
		case DLErrorId::peRSSShowTitleInvalid:
			return "В тэге RSS некорректный четвертый аргумент(ShowTitle)";
		case DLErrorId::peRSSShowLinkInvalid:
			return "В тэге RSS некорректный пятый аргумент(ShowLink)";
		case DLErrorId::peRSSUpdatePeriodInvalid:
			return "В тэге RSS некорректный шестой аргумент(UpdatePeriod)";

		case DLErrorId::peBreakCloseSquareBrace:
			return "В тэге Break пропущена закрывающаяся квадратная скобка";

		case DLErrorId::peExternAnswerOpenRoundBrace:
			return "В тэге ExternAnswer пропущена открывающаяся круглая скобка";
		case DLErrorId::peExternAnswerCloseRoundBrace:
			return "В тэге ExternAnswer пропущена закрывающаяся круглая скобка";
		case DLErrorId::peExternAnswerCloseSquareBrace:
			return "В тэге ExternAnswer пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peExternAnswerEmptyId:
			return "В тэге ExternAnswer указан пустой идентификатор шаблона";
		case DLErrorId::peExternAnswerEmpty:
			return "В тэге ExternAnswer не указан ин один идентификатор шаблона";
		case DLErrorId::peExternAnswerUnregistredId:
			return "Тэг ExternAnswer ссылается на неизвестный шаблон";
		case DLErrorId::peExternAnswerComma:
			return "В тэге ExternAnswer пропущена запятая между аргументами";
		case DLErrorId::peExternAnswerOpenQuote:
			return "В тэге ExternAnswer пропущена открывающая кавычка";
		case DLErrorId::peExternAnswerCloseQuote:
			return "В тэге ExternAnswer пропущена закрывающая кавычка";

		case DLErrorId::pePatternIdIsNotUnique:
			return "Идентификатор данного шаблона уже используется в другом шаблоне";

		case DLErrorId::pePatternUnnecessarySpace:
			return "Возможно в начале строки перед служебным символом стоит лишний пробел";

		case DLErrorId::peTLCS_ElsifWithoutIf:
			return "Оператору elsif не предшествует не один оператор if";
		case DLErrorId::peTLCS_ElseWithoutIf:
			return "Оператору else не предшествует не один оператор if";
		case DLErrorId::peTLCS_EndifWithoutIf:
			return "Оператору endif не предшествует не один оператор if";
		case DLErrorId::peTLCS_EOLExpected:
			return "Недопустимый текст после условного оператора уровня шаблона";

		case DLErrorId::peTLCS_CaseWithoutSwitch:
			return "Оператору case не предшествует не один оператор switch";
		case DLErrorId::peTLCS_DefaultWithoutCase:
			return "Оператору default не предшествует не один оператор case";
		case DLErrorId::peTLCS_CaseExpected:
			return "После оператора switch ожидается оператор case";
		case DLErrorId::peTLCS_SwitchAfterSwitch:
			return "После оператора switch ожидается оператор case, а не switch";
		case DLErrorId::peTLCS_IncompleteOperatorIf:
			return "Незавершенный оператор if уровня шаблона";
		case DLErrorId::peTLCS_IncompleteOperatorSwitch:
			return "Незавершенный оператор switch уровня шаблона";
		case DLErrorId::peTLCS_IncompleteOperator:
			return "Незавершенный оператор уровня шаблона";

		case DLErrorId::pePreOpenBrace:
			return " В тэге pre пропущена открывающая фигурная скобка";
		case DLErrorId::pePreCloseBrace:
			return " В тэге pre пропущена закрывающая фигурная скобка";
		case DLErrorId::pePreEmpty:
			return " В тэге pre нет текста";

		case DLErrorId::peSwitchOpenRoundBrace:
			return "В тэге Switch пропущена открывающая круглая скобка";
		case DLErrorId::peSwitchCloseRoundBrace:
			return "В тэге Switch пропущена закрывающая круглая скобка";
		case DLErrorId::peSwitchCloseSquareBrace:
			return "В тэге Switch пропущена закрывающая квадратная скобка";
		case DLErrorId::peSwitchBlockExpected:
			return "В тэге Switch ожидается блок Case или Default";
		case DLErrorId::peDefaultBlockExpected:
			return "В тэге Switch не хватает блока Default";
		case DLErrorId::peSwitchTooManyArgs:
			return "В условии Switch можно использовать не более одного элемента типа EXTENDED";

		case DLErrorId::peCaseOpenRoundBrace:
			return "В тэге Case пропущена открывающая круглая скобка";
		case DLErrorId::peCaseCloseRoundBrace:
			return "В тэге Case пропущена закрывающая круглая скобка";
		case DLErrorId::peCaseCloseSquareBrace:
			return "В тэге Case пропущена закрывающая квадратная скобка";
		case DLErrorId::peCaseOpenCurlyBrace:
			return "В тэге Case пропущена открывающая фигурная скобка";
		case DLErrorId::peCaseCloseCurlyBrace:
			return "В тэге Case пропущена закрывающая фигурная скобка";
		case DLErrorId::peCaseTooManyArgs:
			return "В условии Case можно использовать не более одного элемента типа EXTENDED";

		case DLErrorId::peDefaultCloseSquareBrace:
			return "В тэге Default пропущена закрывающая квадратная скобка";
		case DLErrorId::peDefaultOpenCurlyBrace:
			return "В тэге Default пропущена открывающая фигурная скобка";
		case DLErrorId::peDefaultCloseCurlyBrace:
			return "В тэге Default пропущена закрывающая фигурная скобка";

		case DLErrorId::peInstrLabelIsEmpty:
			return "В инструкции --Label пропущено имя метки";
		case DLErrorId::peInstrLabelIncorrectName:
			return "Недопустимое имя метки в инструкции --Label";
		case DLErrorId::peInstrLabelIsNotUniq:
			return "Повторно использована одна и та же метка в инструкции --Label";
		case DLErrorId::peInstrLabelTooMach:
			return "Несколько инструкций --Label в одном шаблоне";
		case DLErrorId::peInstrLabelUnexpected:
			return "Инструкция --Label может находиться только в начале описания шаблона";

		case DLErrorId::peNothingCloseSquareBrace:
			return "В тэге Nothing пропущена закрывающаяся квадратная скобка";

		case DLErrorId::peDisableautovarsCloseSquareBrace:
			return "В тэге Disableautovars пропущена закрывающаяся квадратная скобка";

		case DLErrorId::peInvalidModifier:
			return "Комбинация модификаторов :luniq:last: в одном шаблон-вопросе недопустима";

		case DLErrorId::peModifierFirstWithoutRequestEvent:
			return "Модификатор first может использоваться только с событием REQUEST";

		case DLErrorId::peModifierThatAndEvent:
			return "that-строки не должны использоваться в шаблонах с событиями";

		case DLErrorId::peQuestionsAndEvents:
			return "Шаблон может содержать шаблон-вопросы только если в нем нет событий";

		case DLErrorId::peQuestionsLooksLikeEvent:
			return "Шаблон-вопрос похож на описание события";

		case DLErrorId::peOrderAnswerBeforeQuestion:
			return "Шаблон-ответы в описании шаблона не должны предшествовать шаблон-вопросам";
		case DLErrorId::peOrderQuestionAfterAnswer:
			return "Шаблон-вопросы в описании шаблона не должны следовать после шаблон-ответов";
		case DLErrorId::peOrderConditionAfterQuestion:
			return "Шаблон-условия в описании шаблона не должны следовать после шаблон-вопросов";
		case DLErrorId::peOrderQestionAfterThat:
			return "Шаблон-вопросы в описании шаблона не должны следовать после that-строки";
		case DLErrorId::peOrderConditionAfterThat:
			return "Шаблон-условия в описании шаблона не должны следовать после that-строки";
		case DLErrorId::peOrderThatAfterAnswer:
			return "that-строка в описании шаблона не должна следовать после шаблон-ответа";
		case DLErrorId::peOrderThatBeforeQuestion:
			return "that-строка в описании шаблона не должна предшествовать шаблон-вопросам";
		case DLErrorId::peOneThatOnly:
			return "В шаблоне не может быть больше одной that-строки";

		case DLErrorId::peStartListCloseSquareBrace:
			return "В тэге начала форматированнного списка пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peListItemCloseSquareBrace:
			return "В тэге элемента форматированного списка пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peEndListCloseSquareBrace:
			return "В тэге конца форматированного списка пропущена закрывающаяся квадратная скобка";
		case DLErrorId::peEndListToMuch:
			return "Лишний тэг завершения форматированного списка";
		case DLErrorId::peDisbalanseIfLists:
			return "Несбалансированные списки в условном операторе";
		case DLErrorId::peDisbalanseSwicthLists:
			return "Несбалансированные списки в операторе выбора";
	}

	return nullptr;
}

/** Описание и координаты ошибки разбора. */
struct ErrorReport {
	// Код ошибки.
	DLErrorId vCode{};
	
	// Имя файла, в котором произошла ошибка.
	std::string vFileName{};
	
	// Строка, в которой произошла ошибка.
	std::string vString{};
	// Позиция в строке, в которой произошла ошибка.
	unsigned int vPos{ 0 };
	
	std::string vPatternId{};
};

/** Список описаний и координат ошибок разбора. */
using ErrorReports = std::vector<ErrorReport>;

#endif // ERRORS_HPP

