#ifndef __PatternParser_hpp__
#define __PatternParser_hpp__

#include <NanoLib/nVector.hpp>

#include "../DLData.hpp"
#include "../InfDict/InfDictInstrInclude.hpp"

#include "Errors.hpp"

/**
 *  Класс для разбора шаблон-строк.
 */
class DLStringParser
{
public:
	/**
	 *  Конструктор.
	 * @param aDLDataWR - база для данных DL.
	 * @param aTablePatternsId - индекс идентификаторов шаблонов.
	 * @param aTableIndexId - индекс идентификаторов шаблонов.
	 * @param aExtendAnswerErrors - сообщения об ошибках обработки тэга ExtendAnswer.
	 */
	DLStringParser( DLDataWR & aDLDataWR, NanoLib::NameIndex & aTablePatternsId,
					avector<unsigned int> & aTableIndexId, ErrorReports & aExtendAnswerErrors );

public:
	/**
	 *  Описание ошибки разбора.
	 */
	class Error
	{
	public:
		/** Конструктор. */
		Error() noexcept {}

		/** Copy - конструктор. */
		Error( const Error & aError ) noexcept { assign( aError ); }

		/** Move - конструктор. */
		Error( Error && aError ) noexcept { assign( std::move( aError ) ); }

		/**
		 *  Конструктор.
		 * @param aErrorId - индификатор ошибки.
		 * @param aPos - позиция ошибки.
		 */
		Error( DLErrorId aId, unsigned int aPos ) noexcept { vId = aId; vPos = aPos; }

	public:
		/** Установка значения. */
		void assign( const Error & aError ) noexcept { vId = aError.vId; vPos = aError.vPos; }
		void assign( Error && aError ) noexcept { std::swap( vId, aError.vId ); std::swap( vPos, aError.vPos ); }

		void assign( DLErrorId aId, unsigned int aPos ) noexcept { vId = aId; vPos = aPos; }

		/** Установка значения. */
		Error& operator=( const Error & aError ) noexcept { assign( aError ); return *this; }
		Error& operator=( Error && aError ) noexcept { assign( aError ); return *this; }

	public:
		/** Получение текстового описания ошибки разбора. */
		const char * description() const noexcept { return ConvertErrorDescriptionToString( vId ); }

	public:
		/** Получение идентификатора ошибки. */
		DLErrorId GetId() const { return vId; }

		/** Получение позиции ошибки. */
		unsigned int GetPos() const { return vPos; }

	private:
		/** Идентификатор ошибки. */
		DLErrorId vId;

		/** Позиция ошибки в строке. */
		unsigned int vPos;
	};

	/** Список ошибок. */
	using Errors = NanoLib::nVector<Error>;

public:
	/** Описание записи условного оператора уровня шаблона при парсинге. */
	class TLCS_Node
	{
	public:
		using Type = InfPatternItems::TLCS_WR::Type;

	public:
		TLCS_Node( unsigned int aId = static_cast<unsigned int>( -1 ), Type aType = Type::Unknown ) : vId( aId ), vType( aType ) {}

	public:
		unsigned int vId;
		Type vType;
	};

	/** Стэк, описывающий текущее состояние разбора условных операторов уровня шаблона. */
	class TLCS_Stack
	{
	public:
		void clear()
		{
			vTLCS.clear();
			vSwitch.clear();
		}

	public:
		/** Описание всех операторов в стеке. */
		avector<TLCS_Node> vTLCS;
		/** Аргументы операторов switch в стеке. */
		avector<InfPatternItems::Base*> vSwitch;
	};

public:
	/** Получение TLCS стэка. */
	const TLCS_Stack & GetTLCSStack() const { return vTLCS_stack; }

public:
	/**
	 *  Установка флага обработки шаблон-строк эллипсисов.
	 * @param aEllipsisMode - значение флага.
	 */
	void SetEllipsisMode( bool aEllipsisMode ) { vEllipsisMode = aEllipsisMode; }

	/**
	 *  Установка флага жесткости компиляции.
	 * @param aStrictMode - значение флага.
	 */
	void SetStrictMode( bool aStrictMode ) { vStrictMode = aStrictMode; }

	/** Сброс счётчиков уровня шаблона. Должен производиться перед разбором нового шаблона. */
	void Reset()
	{
		vLastQstDictsNumber = 0;
		vMinQstDictsNumber = static_cast<unsigned int>( -1 );
		vMaxQstDictsNumber = 0;

		vTLCS_stack.clear();
	}

public:
	using TLCS_WRs = InfPatternItems::TLCS_WRs;

	/**
	 *  Разбор инструкции --Label.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
	 * @param aLabel - метка.
	 */
	InfEngineErrors ParseInstrLabel( const char * aString, unsigned int aStringLength, aTextString & aLabel );

	/**
	 *  Разбор условного оператора уровня шаблона.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
	 * @param aTLCS - контейнер для результата.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors ParseTLCS_If( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator );

	/**
	 *  Разбор условного оператора уровня шаблона.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
	 * @param aTLCS - контейнер для результата.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors ParseTLCS_Elsif( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator );

	/**
	 *  Разбор условного оператора уровня шаблона.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
	 * @param aTLCS - контейнер для результата.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors ParseTLCS_Else( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator );

	/**
	 *  Разбор условного оператора уровня шаблона.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
	 * @param aTLCS - контейнер для результата.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors ParseTLCS_Endif( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator );

	/**
	 *  Разбор условного оператора уровня шаблона.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
	 * @param aTLCS - контейнер для результата.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors ParseTLCS_Switch( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator );

	/**
	 *  Разбор условного оператора уровня шаблона.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
	 * @param aTLCS - контейнер для результата.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors ParseTLCS_Case( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator );

	/**
	 *  Разбор условного оператора уровня шаблона.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
	 * @param aTLCS - контейнер для результата.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors ParseTLCS_Default( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator );


	/**
	 *  Разбор условного оператора уровня шаблона.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
	 * @param aTLCS - контейнер для результата.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors ParseTLCS_Endswitch( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator );

	/**
	 *  Разбор шаблон-условия.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
     * @param aPos - позициая начала разбора.
	 * @param aResult - контейнер для результата.
	 * @param aInfPerson - идентификатор infperson.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
    InfEngineErrors ParseCondTypePatternString( const char * aString, unsigned int aStringLength, unsigned int aPos,
                                                InfPatternItems::ConditionRW* & aResult,
												unsigned int & aInfPerson, nMemoryAllocator & aAllocator );

	/**
	 *  Разбор шаблон-вопроса.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
     * @param aPos - позициая начала разбора.
	 * @param aResult - контейнер для результата.
	 * @param aReferences - контейнер для списка референций.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
    InfEngineErrors ParseQstTypePatternString( const char * aString, unsigned int aStringLength, unsigned int aPos,
                                               avector<InfPatternItems::Base*> & aResult,
											   avector<InfPatternItems::Segment> & aReferences, nMemoryAllocator & aAllocator );


	/**
	 *  Разбор шаблон-that'а.
	 * @param aString - указатель на строку.
	 * @param aStringLength - длина строки.
     * @param aPos - позициая начала разбора.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
    InfEngineErrors ParseThatTypePatternString (
            const char * aString, unsigned int aStringLength,
            unsigned int aPos,
            avector<InfPatternItems::Base*> & aResult,
            nMemoryAllocator & aAllocator
    );

	/**
	 *  Разбор строки как шаблон-ответа. То есть, при разборе строки допустимыми считаются те тэги, что допустимы в шаблон-ответах.
	 * @param aString - строка.
	 * @param aStringLength - длина строки.
     * @param aPos - позициая начала разбора.
	 * @param aResult - контейнер для результата.
	 * @param WaitingForResolving - элементы, требующие разбора всех шаблонов перед своей компиляцией.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 * @param aIsMultiple - флаг, показывающий, что значение тэга может принимать не одно значение.
	 * @param aIsFunctionArg - флаг, показывающий, что тэг разбирается в качестве аргумента функции.
	 * @param aClearResult - флаг, показывающий, что перед формированием ответа, нужно очистить контейнер aResult.
	 * @param aPatternId - идентификатор разбираемого шаблона (для сообщений об ошибках).
	 * @param aFileName - имя разбираемого файла (для сообщений об ошибках).
	 */
    InfEngineErrors ParseAnsTypePatternString( const char * aString, unsigned int aStringLength, unsigned int aPos,
                                               avector<InfPatternItems::Base*> & aResult,
											   avector<InfPatternItems::Base*> & aWaitingForResolving, nMemoryAllocator & aAllocator,
											   bool & aIsMultiple, bool aIsFunctionArg, const std::string & aPatternId = {},
											   const std::string & aFileName = {} );

	/**
	 *  Разбор инструкции словаря.
	 * @param aDict - строка словаря.
	 * @param aDictLength - длина строки словаря.
	 * @param aResult - результат разбора инструкции.
	 * @param aDictCompilationAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового словаря.
	 * @param aErrorDescription - описание ошибки.
	 * @param aErrorString - строка, в которой встречена ошибка.
	 */
	InfEngineErrors ParseDictInstruction( const char * aDict, unsigned int aDictLength, InfDictInstruction* & aResult,
										  nMemoryAllocator & aDictCompilationAllocator, aTextString & aErrorDescription,
										  aTextString & aErrorString );

	/**
	 *  Разбор интсрукции словаря типа include.
	 * @param aDict - строка словаря.
	 * @param aDictLength - длина строки словаря.
	 * @param aResult - результат разбора инструкции.
	 * @param aDictCompilationAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового словаря.
	 * @param aErrorDescription - описание ошибки.
	 * @param aErrorString - строка, в которой встречена ошибка.
	 */
	InfEngineErrors ParseDictInstrInclude( const char * aDict, unsigned int aDictLength, InfDictInstrInclude* & aResult,
										   nMemoryAllocator & aDictCompilationAllocator, aTextString & aErrorDescription,
										   aTextString & aErrorString );

	/**
	 *  Разбор строки словаря.
	 * @param aString - строка словаря.
	 * @param aStringLength - длина строки словаря.
	 * @param aResult - результат разбора строки словаря.
	 * @param aDictCompilationAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового словаря.
	 * @param aErrorDescription - описание ошибки.
	 * @param aErrorString - строка, в которой встречена ошибка.
	 */
	InfEngineErrors ParseDictString( const char * aDict, unsigned int aDictLength, InfDictString* & aResult,
									 nMemoryAllocator & aDictCompilationAllocator,
									 aTextString & aErrorDescription, aTextString & aErrorString );


public:
	/** Получение списка предупреждений разбора. */
	const Errors & GetWarningsDescriptions() const { return vWarningDescriptions; }

	/** Получение списка ошибок разбора. */
	const Errors & GetErrorsDescriptions() const { return vErrorDescriptions; }


private:
	/**
	 *  Выделяет фрагмент текста в строке словаря.
	 * @param aString - разбираемая строка словаря.
	 * @param aStringLength - длина разбираемой строки.
	 * @param aStartPos - начальная позиция.
	 * @param aFinishPos - конечная позиция (устанавливается функцией).
	 * @param aEndDelimiters - символы начала тэгов.
	 * @param aItems - список разобранных элементов.
	 * @param aDictCompilationAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового словаря.
	 * @param aErrorDescription - описание ошибки.
	 * @param aErrorString - строка, в которой встречена ошибка.
	 */
	InfEngineErrors ParseDictText( const char * aString, unsigned int aStringLength, unsigned int aStartPos, unsigned int & aFinishPos,
								   const char * aEndDelimiters, avector<InfPatternItems::Base*> & aItems,
								   nMemoryAllocator & aDictCompilationAllocator, aTextString & aErrorDescription, aTextString & aErrorString );
	/**
	 *  Разбор базового условия.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 * @param aIsMultiple - устанавливается равеным True, если тэг может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseCondition( const char * aString, unsigned int aStringLength,
									unsigned int & aPos, InfPatternItems::TagFunction* & aCondition,
									nMemoryAllocator & aAllocator, bool & aIsMultiple, bool IsFunctionArg );

	/**
	 *  Разбор базового условия.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseBaseCondition( const char * aString, unsigned int aStringLength,
										unsigned int & aPos, InfPatternItems::BaseCondition* & aResult,
										nMemoryAllocator & aAllocator, bool & aIsMultiple, unsigned int & aInfPerson );
	/**
	 *  Разбор тэга в шаблон-строке ans типа.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблонов перед своей компиляцией.
	 * @param aAllocator - аллокатор памяти.
	 * @param aIsMultiple - устанавливается равеным True, если тэг может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseTagsTypeAns( const char * aString, unsigned int aStringLength, unsigned int & aPos,
									  avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
									  nMemoryAllocator & aAllocator, bool & aIsMultiple, bool IsFunctionArg );

	/**
	 *  Разбор тэга обращения к словарю.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagDict( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								  avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
								  bool & aIsMultiple );

	/**
	 *  Разбор тэга замещения шаблон-ответа.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagGoTo( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								  avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
								  bool & aIsMultiple );

	/**
	 *  Разбор inline-словаря.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 * @param aIsMultiple - флаг, показывающий, может ли значение тэга принимать различные значение при одинаковом запросе.
	 * @param aUseDictSyntax - флаг, показывающий, что осуществляется парсинг словаря, а не шаблона.
	 */
	InfEngineErrors ParseTagDictInline( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
										bool & aIsMultiple, bool aUseDictSyntax );

	/**
	 *  Выделение текста.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 * @param aQstType - флаг, показывающий, что нужно выделять текст, считая, что разбирается шаблон-вопрос.
	 */
	InfEngineErrors ParseText( const char * aString, unsigned int aStringLength, unsigned int & aPos,
							   const char * aEndDelimiters, avector<InfPatternItems::Base*> & aResult,
							   nMemoryAllocator & aAllocator, bool aQstType, bool & aIsMultiple );

	/**
	 *  Разбор ссылки на словарь.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseDictRef( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								  avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
								  bool & aIsMutiple );

	/**
	 *  Разбор референции.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseReference( const char * aString, unsigned int aStringLength, unsigned int & aPos,
									avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
									bool & aIsMutiple );

	/**
	 *  Разбор устаревшего тэга обращения к внешним сервисам.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseExternalService( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										  avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
										  bool & aIsMultiple );

	/**
	 *  Разбор тэга переноса строки.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagBR( const char * aString, unsigned int aStringLength, unsigned int & aPos, avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator, bool & aIsMutiple );

	/**
	 *  Разбор тэга nothing.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagNothing( const char * aString, unsigned int aStringLength, unsigned int & aPos, avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator, bool & aIsMutiple );

	/**
	 *  Разбор тэга disableautovars.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagDisableautovars( const char * aString, unsigned int aStringLength, unsigned int & aPos, avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator, bool & aIsMutiple );

	/**
	 *  Разбор тэга отбрасывания ответа.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagBreak( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								   avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
								   bool & aIsMutiple );

	/**
	 *  Разбор значения переменной и инструкции по его изменению.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 * @param aAllowInstruct - флаг, регулирующий, возможность использования инструкций по изменению значения переменной.
	 */
	InfEngineErrors ParseVarAndInstruct( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										 avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
										 bool aAllowInstruct, bool & aIsMutiple );

	/**
	 *  Разбор тэга кликабельной ссылки.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблонов для своей компиляции.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagInf( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								 avector<InfPatternItems::Base*> & aWaitingForResolving,
								 avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator, bool & aIsMutiple, bool aIsFunctionArg );

	/**
	 *  разбор тэга генерации кликабельной ссылки.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблонов для своей компиляции.
	 * @param aAllocator - аллокатор памяти.
	 * @param aIsMultiple - флаг множественности генерации.
	 */
	InfEngineErrors ParseTagHref( const char * aString, unsigned int aStringLength, unsigned int & aPos, avector<InfPatternItems::Base*> & aResult,
								  avector<InfPatternItems::Base*> & aWaitingForResolving, nMemoryAllocator & aAllocator, bool & aIsMutiple );

	/**
	 *  Разбор тэга открытия ссылки в новомокне.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagOpenWindow( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
										bool & aIsMutiple );

	/**
	 *  Разбор оператора выбра.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблнов для своей компиляции.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagSwitch( const char * aString, unsigned int aStringLength, unsigned int & aPos,
									avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
									nMemoryAllocator & aAllocator, bool & aIsMultiple, bool IsFunctionArg );

	/**
	 *  Разбор условного оператора.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблнов для своей компиляции.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagIf( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
								nMemoryAllocator & aAllocator, bool & aIsMultiple, bool IsFunctionArg, bool aElseifBranch = false );

	/**
	 *  Разбор ветки условного оператора.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * aWaitingForResolving - элементы, требующие разбора всех шаблнов для своей компиляции.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagElsif( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								   avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
								   nMemoryAllocator & aAllocator, bool & aIsMultiple, bool IsFunctionArg );

	/**
	 *  Разбор тэга значения звездочки.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagStar( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								  avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
								  bool & aIsMultiple );

	/**
	 *  Разбор аргумента функции.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * aWaitingForResolving - элементы, требующие разбора всех шаблнов для своей компиляции.
	 * @param aAllocator - аллокатор памяти.
	 * @param aTagBegin - символ, начинающий аргумент.
	 * @param aTagEnd - символ, завершающий аргумент.
	 * @param aArgInfo - информация о типе аргумента.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-аргумент может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseTagFunctionArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										 avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
										 nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, DLFunctionArgType aArgType,
										 bool & aIsMultiple, bool aIsFunctionArg, bool aInterpretTextAsBool, DLFunctionArgAttrs aAttrs = DLFAA_NONE );
	/**
	 *  Разбор тэга функции.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблнов для своей компиляции.
	 * @param aAllocator - аллокатор памяти.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-функция может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseTagFunction( const char * aString, unsigned int aStringLength, unsigned int & aPos,
									  avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
									  nMemoryAllocator & aAllocator, bool & aIsMultiple, bool aIsFunctionArg );

	/**
	 *  Разбор проверки значения переменной. Возвращает функцию IsEqual, IsNotEqual, IsEmpty или IsNotEmpty.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблнов для своей компиляции.
	 * @param aAllocator - аллокатор памяти.
	 * @param aTagBegin - символ, начинающий аргумент.
	 * @param aTagEnd - символ, завершающий аргумент.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-функция может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseTagCheckVarValueLikeFunction( const char * aString, unsigned int aStringLength, unsigned int & aPos,
													   avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
													   nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, bool & aIsMultiple, bool aIsFunctionArg );


	/**
	 *  Проверка самостоятельного тэга, представляемого в виде тэга-функции.
	 * @param aTagFunction - разобранный тэг-функция.
	 * @param aPos - позиция в разбираемой строке.
	 */
	InfEngineErrors CheckTagLikeFinction( InfPatternItems::TagFunction * aTagFunction, unsigned int aPos );

	/**
	 *  Проверка тэга DictsMatch, представляемого в виде тэга-функции.
	 * @param aTagFunction - разобранный тэг-функция.
	 * @param aPos - позиция в разбираемой строке.
	 */
	InfEngineErrors CheckTagDictsMatchLF( InfPatternItems::TagFunction * aTagFunction, unsigned int aPos );

	/**
	 *  Проверка тэга DictsNotMatch, представляемого в виде тэга-функции.
	 * @param aTagFunction - разобранный тэг-функция.
	 * @param aPos - позиция в разбираемой строке.
	 */
	InfEngineErrors CheckTagDictsNotMatchLF( InfPatternItems::TagFunction * aTagFunction, unsigned int aPos );

	/**
	 *  Проверка тэга DictsMatchExt, представляемого в виде тэга-функции.
	 * @param aTagFunction - разобранный тэг-функция.
	 * @param aPos - позиция в разбираемой строке.
	 */
	InfEngineErrors CheckTagDictsMatchExtLF( InfPatternItems::TagFunction * aTagFunction, unsigned int aPos );

	/**
	 *  Разбор тэга запроса RSS.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-функция может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseTagRSS( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								 avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
								 bool & aIsMultiple );

	/**
	 *  Разбор тэга запроса RSS.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблонов перед своей компиляцией.
	 * @param aAllocator - аллокатор памяти.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-функция может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseTagExtendAnswer( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										  avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
										  nMemoryAllocator & aAllocator, bool & aIsMultiple );

	/**
	 *  Разбор алиаса.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблонов перед своей компиляцией.
	 * @param aAllocator - аллокатор памяти.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-функция может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseTagAlias( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								   avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
								   nMemoryAllocator & aAllocator, bool & IsMultiple, bool aIsFunctionArg );

	/**
	 *  Разбор аргумента функции типа TEXT.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 * @param aTagBegin - символ, начинающий аргумент.
	 * @param aTagEnd - символ, завершающий аргумент.
	 * @param aArgInfo - информация о типе аргумента.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-аргумент может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseFunctionTextArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										  avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
										  char aTagBegin, char aTagEnd, DLFunctionArgType aArgType, bool & aIsMultiple );

	/**
	 *  Разбор аргумента функции типа DYNAMIC.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 * @param aTagBegin - символ, начинающий аргумент.
	 * @param aTagEnd - символ, завершающий аргумент.
	 * @param aArgInfo - информация о типе аргумента.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-аргумент может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseFunctionDynamicArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											 avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
											 char aTagBegin, char aTagEnd, DLFunctionArgType aArgType, bool & aIsMultiple );

	/**
	 *  Разбор аргумента функции типа EXTENDED.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблонов перед своей компиляцией.
	 * @param aAllocator - аллокатор памяти.
	 * @param aTagBegin - символ, начинающий аргумент.
	 * @param aTagEnd - символ, завершающий аргумент.
	 * @param aArgInfo - информация о типе аргумента.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-аргумент может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseFunctionExtendedArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											  avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
											  nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, DLFunctionArgType aArgType,
											  bool & aIsMultiple, bool aIsFunctionArg );

	/**
	 *  Разбор аргумента функции типа ANY.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблонов перед своей компиляцией.
	 * @param aAllocator - аллокатор памяти.
	 * @param aTagBegin - символ, начинающий аргумент.
	 * @param aTagEnd - символ, завершающий аргумент.
	 * @param aArgInfo - информация о типе аргумента.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-аргумент может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseFunctionAnyArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										 avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
										 nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, DLFunctionArgType aArgType,
										 bool & aIsMultiple, bool aIsFunctionArg );

	/**
	 *  Разбор аргумента функции типа BOOL.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблонов перед своей компиляцией.
	 * @param aAllocator - аллокатор памяти.
	 * @param aTagBegin - символ, начинающий аргумент.
	 * @param aTagEnd - символ, завершающий аргумент.
	 * @param aArgInfo - информация о типе аргумента.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-аргумент может раскрываться в более чем одно значение.
	 * @param aInterpretTextAsBool - флаг, позволяющий использвать в качестве условия тэга IF функции, возвращающие текстовое значение.
	 */
	InfEngineErrors ParseFunctionBoolArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										  avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
										  nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, DLFunctionArgType aArgType,
										  bool & aIsMultiple, bool aIsFunctionArg, bool aInterpretTextAsBool );

	/**
	 *  Разбор аргумента функции типа IF.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aWaitingForResolving - элементы, требующие разбора всех шаблонов перед своей компиляцией.
	 * @param aAllocator - аллокатор памяти.
	 * @param aTagBegin - символ, начинающий аргумент.
	 * @param aTagEnd - символ, завершающий аргумент.
	 * @param aArgInfo - информация о типе аргумента.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-аргумент может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseFunctionIfArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										avector<InfPatternItems::Base*> & aResult, avector<InfPatternItems::Base*> & aWaitingForResolving,
										nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, DLFunctionArgType aArgType,
										bool & aIsMultiple, bool aIsFunctionArg );

	/**
	 *  Разбор аргумента функции типа VARIABLE.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 * @param aTagBegin - символ, начинающий аргумент.
	 * @param aTagEnd - символ, завершающий аргумент.
	 * @param aArgInfo - информация о типе аргумента.
	 * @param aIsMultiple - устанавливается равеным True, если тэг-аргумент может раскрываться в более чем одно значение.
	 */
	InfEngineErrors ParseFunctionVariableArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											  avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
											  char aTagBegin, char aTagEnd, DLFunctionArgType aArgType, bool & aIsMultiple );

	/**
	 *  Разбор тэга значения звездочки.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseTagPre( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								 avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator,
								 bool & aIsMultiple );

private:
	/**
	 *  Разбор начала форматированного списка.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки с шаблоном.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseStartList( const char * aString, unsigned int aStringLength, unsigned int & aPos,
									avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator );
	/**
	 *  Разбор начала элемента форматированного списка.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки с шабоном.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseListItem( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								   avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator );
	/**
	 *  Разбор конца форматированного списка.
	 * @param aString - строка с шаблоном.
	 * @param aStringLength - длина строки с шаблоном.
	 * @param aPos - позиция в строке.
	 * @param aResult - контейнер для результата.
	 * @param aAllocator - аллокатор памяти.
	 */
	InfEngineErrors ParseEndList( const char * aString, unsigned int aStringLength, unsigned int & aPos,
								  avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator );
	/**
	 *  Создание тэга-функции условного типа, всегда возвращающей истинное значение.
	 * @param aCondition - результат.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors CreateTrueCondition( InfPatternItems::TagFunction* & aCondition, nMemoryAllocator & aAllocator );

	/**
	 *  Создание тэга-функции условного IsEqual, сравнивающей два заданых аргумента.
	 * @param aCondition - результат.
	 * @param aArg1 - первый аргумент функции IsEqual.
	 * @param aArg2 - второй аргумент функции IsEqual.
	 * @param aAllocator - аллокатор памяти, очищаемый перед компиляцией каждого нового шаблона.
	 */
	InfEngineErrors CreateIsEqualCondition( InfPatternItems::TagFunction* & aCondition,
											InfPatternItems::Base * aArg1, InfPatternItems::Base * aArg2,
											nMemoryAllocator & aAllocator );

private:
	InfEngineErrors CheckVariable( const char * aName, unsigned int aNameLength, Vars::Id & aVarId );

	InfEngineErrors CheckLists( InfPatternItems::Base ** aFrom, InfPatternItems::Base ** aTo, unsigned int & aListsCount );

private: /** Счетчики. */
	/** Количество словарей в последнем обработанном шаблон-вопросе. */
	unsigned int vLastQstDictsNumber { 0 };

	/** Минимальное количество словарей в шаблон-вопросах текущего шаблона. */
	unsigned int vMinQstDictsNumber { 0 };

	/** Максимальное количество словарей в шаблон-вопросах текущего шаблона. */
	unsigned int vMaxQstDictsNumber { 0 };


private: /** Настройки. */
	/** Флаг жесткой компиляции. По умолчанию компиляция использует жесткий метод. */
	bool vStrictMode { true };

	/** Флаг компиляции шаблона для эллипсисов. По умолчанию используется компиляция обычных шаблонов. */
	bool vEllipsisMode { false };

private: /** Внешние данные */
	/** База для данных DL. */
	DLDataWR & vDLDataWR;

	/** Индекс идентификаторов шаблонов. */
	NanoLib::NameIndex & vTablePatternsId;
	avector<unsigned int> & vTableIndexId;

private: /** Данные для генерации сообщения об ошибках. */
	/** Информация для генерации сообщений об ошибках. */
	std::string vFileName;
	std::string vPatternId;

	/** Список предупреждений последнего разбора. */
	Errors vWarningDescriptions;

	/** Список ошибок последнего разбора. */
	Errors vErrorDescriptions;

	/** Сообщения об ошибках обработки тэга ExtendAnswer. */
	ErrorReports & vExtendAnswerErrors;

private: /** Внутренние данные, время жизни которых определяется из вне. */
	/** Стек TLCS состояний. */
	TLCS_Stack vTLCS_stack;

private: /** Вспомогательные данные. */
	/** Вспомогательные переменные. */
	mutable aTextString vtmp;

	/** Реестр временных переменных. Очищается перед началом разбора шаблон-ответа. */
	mutable std::map<std::string, unsigned int> vTmpVarsRegistry;

	/** Вспомогательный стек для разбора референций. */
	avector<unsigned int> vOpenReferences;
};

#endif  /** __PatternParser_hpp__ */
