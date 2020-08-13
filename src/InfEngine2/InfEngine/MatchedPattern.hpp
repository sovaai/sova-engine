#ifndef __MatchedPattern_hpp__
#define __MatchedPattern_hpp__

#include <InfEngine2/InfEngine/InfFunctions/TagFunctionsCache.hpp>
#include <InfEngine2/InfEngine/TLCSCache.hpp>
#include "Finder.hpp"


// Набора карт для обратных преобразований запроса псоле замены символов и синонимов.
typedef avector<const unsigned int*> RequestMap;

/**
 *  Класс для работы с уже подобранным шаблоном: порождение результата, добавление автоматических данных,
 * добавление необходимых инструкций и т.д.
 */
class MatchedPattern
{
public:
	/**
	 *  Конструктор.
	 */
	MatchedPattern() {}

public:
	/**
	 *  Генерация ответа по подобранному шаблон-ответу.
	 * @param aPatternAnswer - подобранный шаблон-ответ.
	 * @param aAnswer - контейнер для ответа.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors CreateAnswer( const InfPatternItems::AnswerRO & aPatternAnswer, iAnswer & aAnswer, const Session & aSession, bool aCheckIfCondition = false )
	{
		vSession = &aSession;
		vTmpAllocator.Reset();
		vSession->ResetTmp();
		aAnswer.Reset();
		auto iee = MakeResult( aPatternAnswer, aAnswer, 0, aCheckIfCondition );
		if( iee != INF_ENGINE_SUCCESS )
			aAnswer.Reset();
		return iee;
	}

public:
	/**
	 * @param aUserRequest -
	 * @param aUserRequestLength -
	 */
	void SetUserRequest( const char * aUserRequest, unsigned int aUserRequestLength )
	{
		vUserRequest = aUserRequest;
		vUserRequestLength = aUserRequestLength;
	}

public:
	/** Указатель на текущую сессию. */
	mutable const Session * vSession { nullptr };

	/**
	 *  Вычисление условных операторов уровня шаблона для заданного кондидата.
	 * @param aPattern - шаблон.
	 * @param aTLCS_CacheId - идентификатор, по которому доступны результаты в кэше УОУШ.
	 */
	InfEngineErrors TestTLCS( InfPatternRO & aPattern, unsigned int & aTLCS_CacheId ) const;

protected:

	/**
	 *  Вычисление ветки условных операторов уровня шаблона.
	 * @param aPattern - шаблон.
	 * @param aParentOk - результат проверки условния ролительской ветки.
	 * @param aFirstN - номер первого УОУШ в ветке.
	 * @param aCount - количество УОУШ в ветке.
	 * @param aPassed - вектор с результатами проверки УОУШ в веткке.
	 */
	InfEngineErrors TestTLCS( InfPatternRO & aPattern, bool aParentOk, unsigned int aFirstN, unsigned int & aCount, avector<bool> & aPassed ) const;


public:
	BFinder::Candidat * vMatchUserRequestData { nullptr };

	const DLDataRO * vPatternsData { nullptr };
	const DLDataRO * vDictsData { nullptr };
	const DLDataRO * vFunctionsData { nullptr };
	TagFunctionsCache * vTagFunctionsCache { nullptr };
	TLCSCache * vTLCSCache { nullptr };

	mutable Session * vRWSession { nullptr };

	const InfConditionsRegistry * vConditionsRegistry { nullptr };

	const char * vUserRequest { nullptr };
	unsigned int vUserRequestLength { 0 };

	const char * vNormalizedRequest { nullptr };
	unsigned int vNormalizedRequestLength { 0 };
	RequestMap * vRequestMap { nullptr };

	const BFinder::DictsMatchingResults * vDictsMatchingResults { nullptr };

protected:
	/**
	 *
	 * @param aPatternAnswer -
	 * @param aAnswer -
	 * @param aNestingLevel -
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors MakeResult( const InfPatternItems::ArrayManipulator & aPatternAnswer, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;

	/**
	 *  Вычисление значения условного оператора уровня шаблона.
	 * @param aTLCS - условный оператор уровня шаблона.
	 * @param aTrue - условный оператор уровня шаблона.
	 */
	InfEngineErrors InterpretTLCS( InfPatternItems::TLCS_RO aTLCS, bool & aTrue ) const;

	/**
	 *  Интерпретация элемента шаблон-ответа и добавления полученного результата к ответу.
	 * @param aPatternAnswer - шаблон-ответ, по которому нужно сгенерировать результат.
	 * @param aItemInd - номер элемента в шаблон-ответе.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretItem( const InfPatternItems::ArrayManipulator & aPatternAnswer, unsigned int aItemInd,
								   iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;
	/**
	 *  Интерпретация тэга переноса строки.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretBr( iAnswer & aAnswer ) const;

	/**
	 *  Интерпретация тэга nothing.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretTagNothing( iAnswer & aAnswer ) const { aAnswer.ForceNotEmpty( true ); return INF_ENGINE_SUCCESS; }

	/**
	 *  Интерпретация тэга disableautovars.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretTagDisableautovars( iAnswer & aAnswer ) const { aAnswer.DisableAutoVars( true ); return INF_ENGINE_SUCCESS; }

	/**
	 *  Интерпретация тэга отбрасывания ответа.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretBreak( iAnswer & aAnswer ) const { aAnswer.Reset(); return INF_ENGINE_SUCCESS; }
	/**
	 *  Интерпретация текстового элемента.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretText( const void * aPatternItem, iAnswer & aAnswer ) const;
	/**
	 *  Интерпретация текстового элемента.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aText - указатель на результат.
	 * @param aTextLength - длина результата.
	 */
	InfEngineErrors InterpretText( const void * aPatternItem, const char* & aText, unsigned int & aTextLength ) const;
	/**
	 *  Интерпретация пробельного элемента.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretSpace( const void * aPatternItem, iAnswer & aAnswer ) const;
	/**
	 *  Интерпретация пробельного элемента.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aText - указатель на результат.
	 * @param aTextLength - длина результата.
	 */
	InfEngineErrors InterpretSpace( const void * aPatternItem, const char* & aText, unsigned int & aTextLength ) const;
	/**
	 *  Интерпретация ссылки на подстроку запроса пользователя, соотнесенную с звездочкой в шаблон-вопросе.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aRequest - запрос пользователя.
	 * @param aRequestLength - длина запроса пользователя.
	 * @param aNormalizedRequest - запрос пользователя после замены синонимов на главные.
	 * @param aNormalizedRequestLength - длина запроса пользователя после замены синонимов на главные.
	 * @param aRequestMap - карта отображения нормализованного запроса в оригинальный.
	 * @param aMatchedData - данные кандидата.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretStar( const void * aPatternItem, const char * aRequest, unsigned int aRequestLength, const char * aNormalizedRequest,
								   unsigned int aNormalizedRequestLength, const RequestMap * aRequestMap, BFinder::Candidat * aMatchedData,
								   iAnswer & aAnswer, unsigned int aNestingLevel ) const;
	/**
	 *  Интерпретация ссылки на подстроку запроса пользователя, соотнесенную с звездочкой в шаблон-вопросе.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aRequest - запрос пользователя.
	 * @param aRequestLength - длина запроса пользователя.
	 * @param aNormalizedRequest - запрос пользователя после замены синонимов на главные.
	 * @param aNormalizedRequestLength - длина запроса пользователя после замены синонимов на главные.
	 * @param aRequestMap - карта отображения нормализованного запроса в оригинальный.
	 * @param aMatchedData - данные кандидата.
	 * @param aStar - указатель на результат.
	 * @param aStarLength - длина результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretStar( const void * aPatternItem, const char * aRequest, unsigned int aRequestLength,
								   const char * aNormalizedRequest, unsigned int aNormalizedRequestLength,
								   const RequestMap * aRequestMap, BFinder::Candidat * aMatchedData,
								   const char* & aStar, unsigned int & aStarLength, unsigned int aNestingLevel ) const;
	/**
	 *  Интерпретация ссылки на подстроку, соотнесенную с выделенным фрагментом шаблон-вопроса.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aRequest - запрос пользователя.
	 * @param aRequestLength - длина запроса пользователя.
	 * @param aNormalizedRequest - запрос пользователя после замены синонимов на главные.
	 * @param aNormalizedRequestLength - длина запроса пользователя после замены синонимов на главные.
	 * @param aRequestMap - карта отображения нормализованного запроса в оригинальный.
	 * @param aMatchedData - данные кандидата.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretReference( const void * aPatternItem, const char * aRequest, unsigned int aRequestLength, const char * aNormalizedRequest,
										unsigned int aNormalizedRequestLength, const RequestMap * aRequestMap, BFinder::Candidat * aMatchedData,
										iAnswer & aAnswer, unsigned int aNestingLevel ) const;
	/**
	 *  Интерпретация ссылки на подстроку, соотнесенную с выделенным фрагментом шаблон-вопроса.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aRequest - запрос пользователя.
	 * @param aRequestLength - длина запроса пользователя.
	 * @param aNormalizedRequest - запрос пользователя после замены синонимов на главные.
	 * @param aNormalizedRequestLength - длина запроса пользователя после замены синонимов на главные.
	 * @param aRequestMap - карта отображения нормализованного запроса в оригинальный.
	 * @param aMatchedData - данные кандидата.
	 * @param aReference - указатель на результат.
	 * @param aReferenceLength - длина результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretReference( const void * aPatternItem, const char * aRequest, unsigned int aRequestLength, const char * aNormalizedRequest,
										unsigned int aNormalizedRequestLength, const RequestMap * aRequestMap, BFinder::Candidat * aMatchedData,
										const char* & aReference, unsigned int & aReferenceLength, unsigned int aNestingLevel ) const;

	/**
	 *  Определение позиций начала и конца заданной референции во фразе пользователя.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aMatchedData - данные кандидата.
	 * @param aBegin - начало референции.
	 * @param aEnd - конец референции.
	 */
	InfEngineErrors GetReferenceBorders( const void * aPatternItem, BFinder::Candidat * aMatchedData, NDocImage::Iterator & aBegin, NDocImage::Iterator & aEnd ) const;

	/**
	 *  Интерпретация ссылки на словарь из шаблон-вопроса.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aRequest - запрос пользователя.
	 * @param aRequestLength - длина запроса пользователя.
	 * @param aNormalizedRequest - запрос пользователя после замены синонимов на главные.
	 * @param aNormalizedRequestLength - длина запроса пользователя после замены синонимов на главные.
	 * @param aRequestMap - карта отображения нормализованного запроса в оригинальный.
	 * @param aMatchedData - данные кандидата.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretDictRef(const void * aPatternItem, const char * aRequest, unsigned int aRequestLength, const char * aNormalizedRequest,
									  unsigned int aNormalizedRequestLength, const RequestMap * aRequestMap, BFinder::Candidat * aMatchedData, iAnswer & aAnswer ) const;
	/**
	 *  Интерпретация ссылки на словарь из шаблон-вопроса.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aRequest - запрос пользователя.
	 * @param aRequestLength - длина запроса пользователя.
	 * @param aRequestMap - карта отображения нормализованного запроса в оригинальный.
	 * @param aMatchedData - данные кандидата.
	 * @param aResult - указатель на результат.
	 * @param aResultLength - длина результата.
	 */
	InfEngineErrors InterpretDictRef(const void * aPatternItem, const char * aRequest, unsigned int aRequestLength, const RequestMap * aRequestMap,
									  BFinder::Candidat * aMatchedData, const char* & aResult, unsigned int & aResultLength ) const;

	/**
	 *  Интерпретация инструкции по изменению переменной.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretInstruct( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;
	/**
	 *  Интерпретация обращения к словарю.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretDict( const void * aPatternItem, iAnswer & aAnswer ) const;
	/**
	 *  Интерпретация словаря с заменой подчинённых строк на главные.
	 * @param aDictId - идентификатор словаря.
	 * @param aLineId - номер строки.
	 * @param aResult - строка с результатом интерпретации.
	 * @param aResultLength - длина строки с результатом.
	 */
	InfEngineErrors InterpretDict( unsigned int aDictId, unsigned int aLineId, const char* & aResult, unsigned int & aResultLength ) const;
	/**
	 *  Интерпретация строки словаря.
	 * @param aDictStringManip - манипулятор строки словаря.
	 * @param aResult - строка с результатом интерпретации.
	 * @param aResultLength - длина строки с результатом.
	 */
	InfEngineErrors InterpretDictString( const InfDictStringManipulator & aDictStringManip, const char* & aResult, unsigned int & aResultLength ) const;
	/**
	 *  Интерпретация тэга генерации ссылки на web страничку.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretHref( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;
	/**
	 *  Интерпретация динамического значения.
	 * @param aValuePattern - элементы динамического значения.
	 * @param aResultValue - контейнер для результата.
	 * @param aAnswer - контейнер для результата (используется для изменения значений переменных функциями).
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretValue( const InfPatternItems::ArrayManipulator & aValuePattern, aTextString & aResultValue, iAnswer & aAnswer,
									unsigned int aNestingLevel, bool aCheckIfCondition ) const;
	/**
	 *  Интерпретация кликабельной ссылки на ответ инфу.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretInf( const void * aPatternItem, iAnswer & aAnswer ) const;
	/**
	 *  Интерпретация значения переменной. В случае, если переменная яляется расширенной, то возможно добавление инструкции на изменение that_anchor.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretVar( const void * aPatternItem, iAnswer & aAnswer, bool aCheckIfCondition ) const;
	/**
	 *  Интерпретация значения переменной.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aVarValue - указатель на результат.
	 * @param aVarValueLength - длина результата.
	 */
	InfEngineErrors InterpretVar( const void * aPatternItem, const char* & aVarValue, unsigned int & aVarValueLength ) const;
	/**
	 *  Интерпретация inline-словаря.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretInlineDict( const void * aPatternItem, iAnswer & aAnswer ) const;
	/**
	 *  Интерпретация inline-словаря.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aResult - результат.
	 * @param aResultLength - длина результата.
	 */
	InfEngineErrors InterpretInlineDict( const void * aPatternItem, const char* & aResult ) const;
	/**
	 *  Интерпретация тэга открытия ссылки в окне браузера.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretOpenWindow( const void * aPatternItem, iAnswer & aAnswer ) const;
	/**
	 *  Интерпретация тэга включения шаблон-ответа другого шаблона в качестве элемента текущего шаблон-ответа.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности при шаблон-ответах,
	 *                       содержащих обращения к другим шаблонам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretGetAnswer( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;

	/**
	 *  Интерпретация оператора выбора.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretSwitch( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;

	/**
	 *  Интерпретация условного оператора.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretIf( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;

	/**
	 *  Интерпретация аргумента функции.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretFunctionArgument( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;

	/**
	 *  Интерпретация функции.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretFunction( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;

	/**
	 *  Интерпретация функции.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aText - указатель на результат.
	 * @param aTextLength - длина результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors InterpretFunction( const void * aPatternItem, const char* & aText, unsigned int & aTextLength, iAnswer & aAnswer,
									   unsigned int aNestingLevel, bool aCheckIfCondition ) const;

	/**
	 *  Интерпретация тэга, представленного в виде тэга-функции.
	 * @param aFunctionName - имя функции.
	 * @param aFunctionManipulator - манипулятор тэга-функции.
	 * @param aInterpretedArgs - развёрнутые аргументы тэга-функции.
	 * @param aFunctionResult - результат выполнения функции.
	 * @param aTextLength - длина результата выполнения фнукции.
	 * @param aContext - контекст для выполнения функции.
	 * @param aMatchedData - данные кандидата.
	 */
	InfEngineErrors InterpretTagLikeFunction( const char * aFunctionName, const InfPatternItems::TagFunctionManipulator & aFunctionManipulator,
											  avector<const char*> & aInterpretedArgs, const char* & aFunctionResult, unsigned int & aTextLength,
											  FunctionContext * aContext, BFinder::Candidat * aMatchedData ) const;

	/**
	 *  Интерпретация тэга DictsMatch, представленного в виде тэга-функции.
	 * @param aFunctionName - имя функции.
	 * @param aFunctionManipulator - манипулятор тэга-функции.
	 * @param aInterpretedArgs - развёрнутые аргументы тэга-функции.
	 * @param aFunctionResult - результат выполнения функции.
	 * @param aTextLength - длина результата выполнения фнукции.
	 * @param aContext - контекст для выполнения функции.
	 * @param aMatchedData - данные кандидата.
	 * @param aFirstLine - указатель на первую строку подошедшего словаря. nullptr, если строка не нужна.
	 * @param aNot - признак того, что обрабатывается инвертированная функция.
	 */
	InfEngineErrors InterpretTagDictsMatchLF( const char * aFunctionName, const InfPatternItems::TagFunctionManipulator & aFunctionManipulator,
											  avector<const char*> & aInterpretedArgs, const char* & aFunctionResult, unsigned int & aTextLength,
											  FunctionContext * aContext, BFinder::Candidat * aMatchedData, const char ** aFirstLine, bool aNot ) const;

	/**
	 *  Интерпретация тэга DictsNotMatch, представленного в виде тэга-функции.
	 * @param aFunctionName - имя функции.
	 * @param aFunctionManipulator - манипулятор тэга-функции.
	 * @param aInterpretedArgs - развёрнутые аргументы тэга-функции.
	 * @param aFunctionResult - результат выполнения функции.
	 * @param aTextLength - длина результата выполнения фнукции.
	 * @param aContext - контекст для выполнения функции.
	 * @param aMatchedData - данные кандидата.
	 * @param aFirstLine - указатель на первую строку подошедшего словаря. nullptr, если строка не нужна.
	 */
	InfEngineErrors InterpretTagDictsNotMatchLF( const char * aFunctionName, const InfPatternItems::TagFunctionManipulator & aFunctionManipulator,
												 avector<const char*> & aInterpretedArgs, const char* & aFunctionResult, unsigned int & aTextLength,
												 FunctionContext * aContext, BFinder::Candidat * aMatchedData ) const;

	/**
	 *  Интерпретация тэга DictsMatchExt, представленного в виде тэга-функции.
	 * @param aFunctionName - имя функции.
	 * @param aFunctionManipulator - манипулятор тэга-функции.
	 * @param aInterpretedArgs - развёрнутые аргументы тэга-функции.
	 * @param aFunctionResult - результат выполнения функции.
	 * @param aTextLength - длина результата выполнения фнукции.
	 * @param aContext - контекст для выполнения функции.
	 * @param aMatchedData - данные кандидата.
	 */
	InfEngineErrors InterpretTagDictsMatchExtLF( const char * aFunctionName, const InfPatternItems::TagFunctionManipulator & aFunctionManipulator,
												 avector<const char*> & aInterpretedArgs, const char* & aFunctionResult, unsigned int & aTextLength,
												 FunctionContext * aContext, BFinder::Candidat * aMatchedData ) const;


	/**
	 *  Интерпретация запроса RSS.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretRSS( const void * aPatternItem, iAnswer & aAnswer ) const;


	/**
	 *  Интерпретация тэга продолжения ответа.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretExtendAnswer( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel ) const;

	/**
	 *  Интерпретация тэга pre.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 */
	InfEngineErrors InterpretPre( const void * aPatternItem, iAnswer & aAnswer ) const;
	/**
	 *  Интерпретация тэга pre.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aText - указатель на результат.
	 * @param aTextLength - длина результата.
	 */
	InfEngineErrors InterpretPre( const void * aPatternItem, const char* & aText, unsigned int & aTextLength ) const;


protected:
	/** Аллокатор памяти для внутреннего использования. Очищается при каждой конвертации кандидата в ответ. */
	mutable nMemoryAllocator vTmpAllocator;
};

/**
 *  Подобранный вариант совпадения для шаблонов эллипсисов. Предназначен для финального манипулирования
 * подобранными данными.
 */
class EliMatchedPattern : public MatchedPattern
{
public:
	EliMatchedPattern() = default;

public:
	void SetInfPreResponse( const char * aInfPreResponse, unsigned int aInfPreResponseLength )
	{
		vInfPreResponse = aInfPreResponse;
		vInfPreResponseLength = aInfPreResponseLength;
	}

	void SetUserPreRequest( const char * aUserPreRequest, unsigned int aUserPreRequestLength )
	{
		vUserPreRequest = aUserPreRequest;
		vUserPreRequestLength = aUserPreRequestLength;
	}


public:
	/**
	 *  Генерация ответа по подобранному шаблон-ответу.
	 * @param aPatternAnswer - подобранный шаблон-ответ.
	 * @param aAnswer - контейнер для ответа.
	 */
	InfEngineErrors CreateAnswer( const Session & aSession, const InfPatternItems::ArrayManipulator & aPatternAnswer, iAnswer & aAnswer )
	{
		vSession = &aSession;
		vTmpAllocator.Reset();
		vSession->ResetTmp();
		return MakeResult( aPatternAnswer, aAnswer, 0, false );
	}


public:
	/** Данные подбора совпадения с предыдущим ответом инфа. */
	BFinder::Candidat * vMatchPreInfMatchData = nullptr;
	/** Данные подбора совпадения с предыдущим запросом пользователя. */
	BFinder::Candidat * vMatchPreUserMatchData = nullptr;


protected:
	/**
	 *  Генрация ответа на запрос пользователя на основании подобранного шаблона.
	 * @param aPatternAnswer - шаблон-ответ, по которому нужно сгенерировать результат.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 * @param aCheckIfCondition -
	 */
	InfEngineErrors MakeResult( const InfPatternItems::ArrayManipulator & aPatternAnswer, iAnswer & aAnswer, unsigned int aNestingLevel, bool aCheckIfCondition ) const;
	/**
	 *  Интерпретация ссылки на подстроку запроса пользователя, соотнесенную с звездочкой в шаблон-вопросе.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretEliStar( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel ) const;
	/**
	 *  Интерпретация ссылки на подстроку запроса пользователя, соотнесенную с звездочкой в шаблон-вопросе.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aStar - указатель на результат.
	 * @param aStarLength - длина результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretEliStar( const void * aPatternItem, const char* & aStar, unsigned int & aStarLength, unsigned int aNestingLevel ) const;
	/**
	 *  Интерпретация ссылки на подстроку, соотнесенную с выделенным фрагментом шаблон-вопроса.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aAnswer - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretEliReference( const void * aPatternItem, iAnswer & aAnswer, unsigned int aNestingLevel ) const;
	/**
	 *  Интерпретация ссылки на подстроку, соотнесенную с выделенным фрагментом шаблон-вопроса.
	 * @param aPatternItem - элемент шаблон-ответа.
	 * @param aReference - указатель на результат.
	 * @param aReferenceLength - длина результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretEliReference( const void * aPatternItem, const char* & aReference, unsigned int & aReferenceLength, unsigned int aNestingLevel ) const;
	/**
	 *  Интерпретация динамического значения.
	 * @param aValuePattern - элементы динамического значения.
	 * @param aResultValue - контейнер для результата.
	 * @param aNestingLevel - уровень вложенности. Используется для ограничения вложенности интерпретации
	 *                       элменетов шаблон-ответа, содержащих обращения к другим шаблон-ответам.
	 */
	InfEngineErrors InterpretValue( InfPatternItems::ArrayManipulator & aValuePattern, aTextString & aResultValue, unsigned int aNestingLevel ) const;


protected:
	/** Предыдущий ответ инфа. */
	const char * vInfPreResponse { nullptr };
	/** Длина предыдущего ответа инфа. */
	unsigned int vInfPreResponseLength { 0 };

	/** Предыдущий запрос пользователя. */
	const char * vUserPreRequest { nullptr };
	/** Длина предыдущего запроса пользователя. */
	unsigned int vUserPreRequestLength { 0 };
};

#endif  /** __MatchedPattern_hpp__ */
