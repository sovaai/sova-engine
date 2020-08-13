#ifndef INF_PATTERN_HPP
#define INF_PATTERN_HPP

#include "InfPattern/Items.hpp"

/**
 *  Класс, представляющий шаблон в памяти.
 */
class InfPattern
{
public:
	/** Классы событий. */
	enum class Events
	{
		/** Событие запроса ответа инфа. */
		Request         = 0,
		/** Событие запроса установки сессии. */
		SetSession      = 1,
		/** Событие запроса обновления сессии. */
		UpdateSession   = 2
	};

public:
	/**
	 *  Установка внешнего идентификатора шаблона.
	 * @param aPatternId - текстовый внешний идентификатор шаблона.
	 * @param aPatternIdLength - длина внешнего идентификатора.
	 */
	void SetPatternId( const char * aPatternId, unsigned aPatternIdLength )
	{
		vPatternIdLength = aPatternIdLength;
		vPatternId = aPatternId;
	}

	/**
	 *  Установка метки (арагумента инструкции --Label).
	 * @param aInstrLabel - метка --Label.
	 * @param aInstrLabelLength - длина метки.
	 */
	void SetInstrLabel( const char * aInstrLabel, unsigned int aInstrLabelLength )
	{
		vInstrLabel = aInstrLabel;
		vInstrLabelLength = aInstrLabelLength;
	}

	/**
	 *  Установка шаблон-условий.
	 * @param aConditions - массив шаблон-условий.
	 * @param aConditionsNum - количество шаблон-условий.
	 */
	void SetConditions( InfPatternItems::ConditionRW ** aConditions, unsigned int aConditionsNum )
	{
		vConditionsNum = aConditionsNum;
		vConditions = aConditions;
	}

	/**
	 *  Установка шаблон-вопросов.
	 * @param aQuestions - массив шаблон-вопросов.
	 * @param aQuestionsNum - длина массива шаблон-вопросов.
	 */
	void SetQuestions( InfPatternItems::QuestionWR ** aQuestions, unsigned int aQuestionsNum )
	{
		vQuestionsNum = aQuestionsNum;
		vQuestions = aQuestions;
	}

	/**
	 *  Установка
	 * @param aTLCS - массив условных операторов уровня шаблона.
	 * @param aTLCSNum - длина массива условных операторов уровня шаблона.
	 */
	void SetTLCS( InfPatternItems::TLCS_WR ** aTLCS, unsigned int aTLCSNum )
	{
		vTLCSNum = aTLCSNum;
		vTLCS = aTLCS;
	}

	/**
	 *  Установка that-строки.
	 * @param aThat - that-строка.
	 */
	void SetThat( InfPatternItems::QuestionWR * aThat ) { vThat = aThat; }

	/**
	 *  Установка шаблон-ответов. При этом копирования данных не производится.
	 * @param aAnswers - массив шаблон-ответов.
	 * @param aAnswersNum - длина массива шаблон-ответов.
	 * @param aWaitingForResolving -
	 * @param aWaitingForResolvingNum -
	 */
	InfEngineErrors SetAnswers( const InfPatternItems::AnswerRW * aAnswers, unsigned int aAnswersNum,
								InfPatternItems::Base ** aWaitingForResolving, unsigned int aWaitingForResolvingNum )
	{
		// Проверка аргументов.
		if( !aAnswers && aAnswersNum != 0 )
			return INF_ENGINE_ERROR_INV_ARGS;

		// Копирование ссылок на данные.
		vAnswersNum = aAnswersNum;
		vAnswers = aAnswers;

		vWaitingForResolving.Set( aWaitingForResolving, aWaitingForResolvingNum );

		return INF_ENGINE_SUCCESS;
	}

	/** Установка InfPerson. */
	void SetInfPerson( unsigned int aInfPerson ) { vInfPerson = aInfPerson; }


public:
	unsigned int GetWaitingForResolvingNumber() { return vWaitingForResolving.GetItemsNumber(); }

	InfPatternItems::Base * GetWaitingForResolving( unsigned int aNum ) { return vWaitingForResolving[aNum]; }

public:
	/** Получение значения флага необходимости разрешения тэгов ExtendAnswer в шаблон-ответах. */
	bool NeedResolving() const { return vWaitingForResolving.GetItemsNumber(); }

	/** Получение идентификатора шаблона. */
	const char * GetPatternId( unsigned int * aPatternIdLength = nullptr ) const
	{
		if( aPatternIdLength )
			*aPatternIdLength = vPatternIdLength;
		return vPatternId;
	}

	/** Получение метки --Lable. */
	const char * GetInstrLabel( unsigned int * aInstrLabelLength = nullptr ) const
	{
		if( aInstrLabelLength )
			*aInstrLabelLength = vInstrLabelLength;
		return vInstrLabel;
	}

	/**
	 *  Получение i-того условного оператора уровня шаблона.
	 * @param aNum - номер шаблон-вопроса.
	 */
	InfPatternItems::TLCS_WR & GetTLCS( unsigned int aNum ) const { return *( vTLCS[aNum] ); }

	/** Получение количества условногых операторов уровня шаблона. */
	unsigned int GetTLCSNumber() const { return vTLCSNum; }

	unsigned int GetConditionsNum() const { return vConditionsNum; }

	const InfPatternItems::ConditionRW & GetCondition( unsigned int aNum ) const { return *( vConditions[aNum] ); }

	/** Получение количества шаблон-вопросов. */
	unsigned int GetQuestionNumber() const { return vQuestionsNum; }

	/**
	 *  Получение aNum-того шаблон-вопроса.
	 * @param aNum - номер шаблон-вопроса.
	 */
	InfPatternItems::QuestionWR & GetQuestion( unsigned int aNum ) const { return *( vQuestions[aNum] ); }

	/** Получение количества шаблон-ответов. */
	unsigned int GetAnswerNumber() const { return vAnswersNum; }

	/**
	 *  Получение aNum-того шаблон-ответа.
	 * @param aNum - номер шаблон-ответа.
	 */
	const InfPatternItems::AnswerRW & GetAnswer( unsigned int aNum ) const { return vAnswers[aNum]; }

	/** Получение шаблона-that. */
	InfPatternItems::QuestionWR * GetThat() const { return vThat; }

	/** Получение InfPerson. */
	unsigned int GetInfPerson() const { return vInfPerson; }

	void SetLabel( const char * aLabel, unsigned int aLabelLength ) { vLabel = aLabel; vLabelLength = aLabelLength; }

	const char * GetLabel( unsigned int & aLabelLength ) const { aLabelLength = vLabelLength; return vLabel; }

	/**
	 *  Привязка шаблона к событию.
	 * @param aEvent - событие.
	 */
	void BindEvent( Events aEvent ) { vEvents[(int)aEvent] = true; }

	/** Проверка, привязан ли шаблон к какому-либо событию. */
	bool IsBoundToEvent() const
	{
		for( unsigned int i = 0; i < EVENTS_NUMBER; ++i )
			if( vEvents[i] )
				return true;
		return false;
	}

	/**
	 *  Проверка связи с событием.
	 * @param aEvent - событие.
	 */
	bool CheckEvent( Events aEvent ) const { return vEvents[(int)aEvent]; }

	/** Проверка наличия привязанных шаблонов. */
	bool CheckEvents() const
	{
		for( auto Event : vEvents )
			if( Event )
				return true;
		return false;
	}

public:
	/**
	 *  Замена в тексте шаблона синонимы на канонический вид.
     * @param aSymbolymsBase - база заменяемых символов.
	 * @param aSynonymsBase - база синонимов.
	 * @param aAllocator - аллокатор памяти.
	 */
    InfEngineErrors ApplySynonyms( SymbolymBaseRO & aSymbolymsBase, SynonymBaseRO & aSynonymsBase, nMemoryAllocator & aAllocator );

private:
	/** Идентификатор шаблона. */
	const char * vPatternId { nullptr };

	/** Длина идентификатора шаблона. */
	unsigned int vPatternIdLength { 0 };

	/** Метка (аргумент инструкции --Label). */
	const char * vInstrLabel { nullptr };

	/** Длина метки. */
	unsigned int vInstrLabelLength { 0 };

	/** Список условных операторов уровня шаблона. */
	InfPatternItems::TLCS_WR ** vTLCS { nullptr };

	/** Количество условных операторов уровня шаблона. */
	unsigned int vTLCSNum { 0 };

	/** Список условий. */
	InfPatternItems::ConditionRW ** vConditions { nullptr };

	/** Количество условий. */
	unsigned int vConditionsNum { 0 };

	/** Список шаблон-вопросов. */
	InfPatternItems::QuestionWR ** vQuestions { nullptr };

	/** Количество шаблон-вопросов. */
	unsigned int vQuestionsNum { 0 };

	/** That-строка. */
	InfPatternItems::QuestionWR * vThat { nullptr };

	/** Список шаблон-ответов. */
	const InfPatternItems::AnswerRW * vAnswers { nullptr };

	/** Указатели на элементы, которые могут быть скомпилированы только после парсинга всех шаблонов, например, тэги ExtendAnswer. */
	InfPatternItems::Array vWaitingForResolving;

	/** Количество шаблон-ответов. */
	unsigned int vAnswersNum { 0 };

	/** Идентификатор шаблона InfPerson. */
	unsigned int vInfPerson { 0 };

	const char * vLabel { nullptr };
	unsigned int vLabelLength { 0 };

	/** Маска событий, связанных с шаблоном и её размер. */
	static const unsigned int EVENTS_NUMBER { 3 };
	bool vEvents[EVENTS_NUMBER] = { false, false, false };
};

/**
 *  Манипулятор сохраненным шаблоном.
 */
class InfPatternRO
{
	/** Манипулятор сохраненным шаблон заполняется только через класс DLDataRO. */
	friend class DLDataRO;

public:
	/** Очистка шаблона. */
	void Clear()
	{
		vPatternId = "";
		vInfPerson = 0;
		vConditions.clear();
		vQuestions.clear();
		vThatString.Set( -1, nullptr, nullptr );
		vAnswers.clear();
	}

public:
	/** Получение идентификатора шаблона. */
	const char * GetId() const { return vPatternId; }

	/** Получение идентификатора шаблона. */
	const char * GetInstrLabel() const { return vInstrLabel; }

	/** Получение InfPerson. */
	unsigned int GetInfPerson() const { return vInfPerson; }

	/** Получение количества условий в шаблоне. */
	unsigned int GetConditionsNum() const { return vConditions.size(); }

	/**
	 *  Получение указателя на aNum-ое шаблон-условие.
	 * @param aNum - номер условия.
	 */
	InfPatternItems::ConditionRO GetCondition( unsigned int aNum ) const
	{
		return aNum < vConditions.size() ? vConditions[aNum] : InfPatternItems::ConditionRO();
	}

	/** Получение количества шаблон-вопросов в шаблоне. */
	unsigned int GetQuestionsNum() const { return vQuestions.size(); }

	/**
	 *  Получение указателя на aNum-ый шаблон-вопрос.
	 * @param aNum - номер шаблон-вопроса.
	 */
	InfPatternItems::QuestionRO GetQuestion( unsigned int aNum ) const
	{
		return aNum < vQuestions.size() ? vQuestions[aNum] : InfPatternItems::QuestionRO();
	}

	/** Проверка наличия that-шаблона. */
	bool HasThatString() const { return vThatString.GetItemsNum() > 0; }

	/** Получение that шаблона. */
	InfPatternItems::ThatRO GetThatString() const { return vThatString; }

	/** Получение количества шаблон-ответов в шаблоне. */
	unsigned int GetAnswersNum() const { return vAnswers.size(); }

	/**
	 *  Получение aNum-ого шаблон-ответа.
	 * @param aNum - номер шаблон-ответа.
	 */
	InfPatternItems::AnswerRO GetAnswer( unsigned int aNum ) const
	{
		return aNum < vAnswers.size() ? vAnswers[aNum] : InfPatternItems::AnswerRO();
	}

	/** Получение количества условных операторов уровня шаблона. */
	unsigned int GetTLCSNum() const { return vTLCS.size(); }

	/**
	 *  Получение aNum-ого условного операторова уровня шаблона.
	 * @param aNum - номер условного операторова уровня шаблона.
	 */
	InfPatternItems::TLCS_RO GetTLCS( unsigned int aNum ) const { return aNum < vTLCS.size() ? vTLCS[aNum] : InfPatternItems::TLCS_RO(); }

private:
	const char * vPatternId { "" };
	const char * vInstrLabel { "" };
	unsigned int vInfPerson { 0 };
	std::vector<InfPatternItems::ConditionRO> vConditions;
	std::vector<InfPatternItems::QuestionRO> vQuestions;
	InfPatternItems::ThatRO vThatString;
	std::vector<InfPatternItems::AnswerRO> vAnswers;
	std::vector<InfPatternItems::TLCS_RO> vTLCS;
};

#endif /** INF_PATTERN_HPP */
