#ifndef __ExtFinder_hpp__
#define __ExtFinder_hpp__

#include "Finder.hpp"
#include "MatchedPattern.hpp"

/**
 *  Поисковик по индексной базе.
 */
class InfBFinder : public BFinder
{
public:
	/**
	 *  Конструктор поисковика шаблонов.
	 */
	InfBFinder() = default;

public:
	/** Очистка всех данных и подготовка к новому поиску. */
	void Reset();

public:
	/**
	 *  Поиск наилучшего ответа на вопрос.
	 * @param aRequest - строка запроса.
	 * @param aRequestLength - длина строки запроса.
	 * @param aSearchData - данные для поиска.
	 * @param aSession - сессиия.
	 * @param aAnswer - контейнер для результата.
	 * @param aAnswerWeight - вес подобранного ответа.
	 * @param aRequestStat - контейнер для сбора статистики.
	 * @param aCreateResult - флаг генерации ответа.
	 * @param aUserBase - флаг работы с пользовательской базой.
	 */
	InfEngineErrors Search( const char * aRequest, unsigned int aRequestLength, SearchData & aSearchData, Session & aSession,
							iAnswer & aAnswer, unsigned int & aAnswerWeight, RequestStat & aRequestStat, bool aCreateResult, bool aUserBase );

public:
	/**
	 *  Подготовка ответа по найденным кандидатам.
	 * @param aPatternsData - данные шаблонов.
	 * @param aDictsData - данные словарей.
	 * @param aFunctionsData - данные функций.
	 * @param aConditionsRegistry - реестр условий.
	 * @param aSession - сессия.
	 * @param aAnswer - контейнер для результата.
	 * @param aRequest - строка запроса.
	 * @param aRequestLength - длина строки запроса.
	 * @param aNormalizedReq - строка запроса после замены слов на главные синонимы.
	 * @param aNormalizedReqLength - длина строки сапроса после замены слов на главные синонимы.
	 * @param aReqestMap - карта отображения нормализованного запроса в оригинальный.
	 * @param aAnswerWeight - вес подобранного ответа.
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 * @param aDictsMatchingResults - результаты сопостовления словарей (используются для работы DL-функций).
	 */
	InfEngineErrors CreateAnswer( const DLDataRO & aPatternsData, const DLDataRO & aDictsData, const DLDataRO & aFunctionsData,
								  const InfConditionsRegistry & aConditionsRegistry, Session & aSession,
								  iAnswer & aAnswer, const char * aRequest, unsigned int aRequestLength,
								  const char * aNormalizedReq, unsigned int aNormalizedReqLength,
								  avector<const unsigned int*> & aRequestMap, unsigned int & aAnswerWeight,
								  RequestStat & aRequestStat, const DictsMatchingResults & aDictsMatchingResults );
	/**
	 *  Генерация ориентировочного числа вариантов, которые можно получить из данного шаблон-ответа. Если
	 * в результате получается 0, то из данного шаблон-ответа нельзя получить никаких вараинтов.
	 * @param aAnswer - данные шаблон-ответа.
	 * @param aSession - сессия.
	 * @param aPatternsData - данные шаблонов.
	 * @param aDictsData - данные словарей.
	 * @param aConditionsRegistry - реестр условий.
	 */
	unsigned int GenerateVariationNumber( const InfPatternItems::ArrayManipulator & aAnswer, const Session & aSession,
										  const DLDataRO & aPatternsData, const DLDataRO & aDictsData,
										  const InfConditionsRegistry & aConditionsRegistry );

protected:

	InfEngineErrors CreateAnswerFromEqualWeightCandidates( avector<AnswerData> & aAnswers, const std::vector<unsigned int> & aOrder,
														   unsigned int & aOrderN, const DLDataRO & aPatternsData,
														   const DLDataRO & aDictsData, const DLDataRO & aFunctionsData,
														   const InfConditionsRegistry & aConditionsRegistry, const Session & aSession,
														   iAnswer & aAnswer, RequestStat & aRequestStat, bool * aAnswerable );

protected:

	MatchedPattern vMatchedPattern{};

	/** Данные словарей. */
	DictsMatchingResults vMainDictsMatchingResults;

	TagFunctionsCache vTagFunctionsCache;

	/** Кэш результатов проверки условныхоператоров уровня шаблона. */
	TLCSCache vTLCSCache;
};

/**
 *  Поисковик по базе индексированных шаблонов, использующий дополнительную, кастомизирующую, базу.
 */
class InfBFinderExt : protected InfBFinder
{
public:
	/**
	 *  Виды логики выбора ответов с флагом UNKNOWN.
	 */
	enum class SkipUnknownLogic
	{
		Off     = 0, // Работает обычная логика выбора ответа. Флаг UNKNOWN игнорируется.
		Var     = 1, // Логика определяется значением переменной-флага SkipUnknown в пользовательской сессии.
		Force   = 2, // Всегда используется расширенная логика выбора ответа с флагом UNKNOWN.
	};

	/**
	 *  Конструктор поисковика шаблонов.
	 */
	InfBFinderExt() = default;

public:
	/** Очистка всех данных и подготовка к новому поиску. */
	void Reset();

public:
	/**
	 *  Поиск всех подходящих кандидатов.
	 * @param aRequest - строка запроса.
	 * @param aRequestSize - длина строки запроса.
	 * @param aMainSearchData - основные данные для поиска.
	 * @param aExtSearchData - дополнительные данные для поиска.
	 * @param aSession - сессия.
	 * @param aAnswer - контейнер для результата.
	 * @param aAnswerWeight - вес подобранного ответа.
	 * @param aAnswerFromUserBase - флаг, показывающий является ли ответ порожденным по шаблону из кастомизирующей базы или же из основной.
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 */
	InfEngineErrors Search( const char * aRequest, unsigned int aRequestSize, SearchData & aMainSearchData, SearchData & aExtSearchData, Session & aSession,
							iAnswer & aAnswer, unsigned int & aAnswerWeight, bool & aAnswerFromUserBase, RequestStat & aRequestStat );

	/**
	 *  Установка логики выбора ответов с флагом UNKNOWN.
	 * @param aSkipUnknownLogic - тип логики выбора ответа.
	 */
	void SetSkipUnknownLogic( SkipUnknownLogic aSkipUnknownLogic ) { vSkipUnknownLogic = aSkipUnknownLogic; }

	/**
	 *  Установка метода слияния словарей из основной и пользовательской базы.
	 * @param aJoiningMethod - метод слияния словарей.
	 */
	void SetDictMergeMethod( InfDictWrapManipulator::JoiningMethod aJoiningMethod ) { vDictsJoiningMethod = aJoiningMethod; }

	/**
	 *  Флаг, показывающий нужно ли использовать пользовательские словари для поиска по основной базе.
	 * @param aUseUserDictsInMainBaseSearch - true, если нужно использовать пользовательские словари для поиска по основной базе. Иначе - false.
	 */
	void SetDictMergeEffect( bool aUseUserDictsInMainBaseSearch ) { vUseUserDictsInMainBaseSearchDflt = aUseUserDictsInMainBaseSearch; }


private:
	/** Логика выбора ответов с флагом UNKNOWN. */
	SkipUnknownLogic vSkipUnknownLogic = SkipUnknownLogic::Off;

	/** Флаг, показывающий необходимость использовать пользовательские словари при поиске по основной базе. */
	bool vUseUserDictsInMainBaseSearch;
	bool vUseUserDictsInMainBaseSearchDflt = true;

	/** Метод объединения результатов поиска по ловарям основной и пользовательской базы. */
	InfDictWrapManipulator::JoiningMethod vDictsJoiningMethod;
	InfDictWrapManipulator::JoiningMethod vDictsJoiningMethodDflt = InfDictWrapManipulator::jmDefault;

	/** Вспомогательные объекты. */
	CDocMessage vDocMessageThat;
	DocImage vDocImageThat;
	DocImage vDocImageExt;

	/** Виртуальные термины словарей для that-шаблонов. */
	DictsMatchingResults vDictsDataThat;
	DictMatcher vDictMatcherThat;

	/** Результат поиска по пользовательским словарям. */
	DictsMatchingResults vUserDictsMatchingResults;

	/** Результат слияния результатов поиска по основным и пользовательским словарям. */
	DictsMatchingResults vJoinedDictsMatchingResults;
};


#endif /** __ExtFinder_hpp__ */
