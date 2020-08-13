#ifndef INF_ENGINE_DLDATA_HPP
#define INF_ENGINE_DLDATA_HPP

#include <set>
#include <tuple>

#include <NanoLib/Blocks.hpp>

#include "InfPattern.hpp"
#include "InfDict/InfDict.hpp"
#include "InfDict/InfDictWrapManipulator.hpp"
#include "InfFunctions/FuncTagRegistry.hpp"
#include "InfFunctions/AliasRegistry.hpp"
#include "PatternsStorage.hpp"

/**
 *  Класс для манипулирования базой данных DL.
 */
class DLDataRO
{
public:
	/**
	 *  Конструктор.
	 * @param aMainDataRO - основная база данных DL.
	 */
	DLDataRO( const DLDataRO * aMainDataRO = nullptr );

public:
	/**
	 *  Открытие базы данных DL из fstorage в режиме ReadOnly.
	 * @param aFStorage - ссылка на открытый fstorage.
	 * @param aFunctionsRootDir - корневой каталог для функций.
	 * @param aFunctionsConfigPath - путь к конфигурационному файлу функций.
	 * @param aJoiningMethod
	 */
	InfEngineErrors Open( fstorage * aFStorage, const char * aFunctionsRootDir, const char * aFunctionsConfigPath,
						  InfDictWrapManipulator::JoiningMethod aJoiningMethod = InfDictWrapManipulator::jmDefault );

	/** Закрытие базы данных DL. */
	void Close();

private:
	/**
	 *  Открытие базы данных DL из буффера.
	 * @param aBuffer - буффер с данными.
	 * @param aBufferSize - длина буффера с данными.
	 *
	 * @todo Избавиться от этой функции.
	 */
	InfEngineErrors LoadFromBuffer( const char * aBuffer, unsigned int aBufferSize );

private:
	/**
	 *  Получение из текущей базы манипулятора словаря по идентификатору.
	 * @param aDictId - идентификатор словаря.
	 * @param aDictManipulator - манипулятор для словаря.
	 */
	InfEngineErrors GetDictById( unsigned int aDictId, InfDictManipulator & aDictManipulator ) const;

	/**
	 *  Получение из текущей базы манипулятора словаря по имени.
	 * @param aDictName - имя словаря.
	 * @param aLength - длина имени словаря.
	 * @param aDictManipulator - манипулятор для словаря.
	 */
	InfEngineErrors GetDictByName( const char * aDictName, unsigned int aLength, InfDictManipulator & aDictManipulator ) const;

public:
	/** Получение количества словарей в базе. */
	unsigned int GetDictsCount() const
		{ return vDictsCount; }

	/** Получение количества словарей в текущей базе и всех родительских. */
	unsigned int GetDictMaxId() const
		{ return GetDictsCount() + ( vMainDataRO ? vMainDataRO->GetDictMaxId() : 0 ); }

	/**
	 *  Получение обёртки-манипулятора для нескольких одноимённых словарей по идентификатору.
	 * @param aDictId - идентификатор словаря.
	 * @param aDictManipulator - обёртка-манипулятор для словаря.
	 */
	InfEngineErrors GetDictById( unsigned int aDictId, InfDictWrapManipulator & aDictManipulator ) const;

	/**
	 *  Проверяет, содерит ли данная база словарь с заданным идентификатором.
	 * @param aDictId - идентификатор словаря.
	 */
	bool IsDictExisted( unsigned int aDictId ) const;

	/**
	 *  Получение идентификатора словаря по его имени.
	 * @param aDictName - имя словаря.
	 * @param aLength - длина имени словаря.
	 */
	const unsigned int * GetDictIdByName( const char * aDictName, unsigned int aLength ) const;

	/**
	 *  Получение шаблона по номеру строкового идентификатора.
	 * @param aPatternId - номер, поставленный в соответствие строковому идентификатору.
	 * @param aPatternManipulator - манипулятор шаблона.
	 */
	InfEngineErrors GetPatternByPatternId( unsigned int aPatternId, InfPatternRO & aPattern ) const
	{
		if( vTableIndexId && vTableIndexId[aPatternId] != static_cast<unsigned int>(-1) )
			return GetPattern( vTableIndexId[aPatternId], aPattern ), INF_ENGINE_SUCCESS;
		else
			return INF_ENGINE_WARN_UNSUCCESS;
	}

	/**
	 *  Получение шаблона по идентификатору метки.
	 * @param aLabelId - идентификатор метки.
	 * @param aPatternManipulator - манипулятор шаблона.
	 */
	InfEngineErrors GetPatternByLabelId( unsigned int aLabelId, InfPatternRO & aPattern ) const
	{
		if( aLabelId >= vLabelCorrespondenceTable[0] )
			ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

		GetPattern( vLabelCorrespondenceTable[1+aLabelId], aPattern );

		return INF_ENGINE_SUCCESS;
	}


public:
	/** Получение ссылки на список имен переменных. */
	const Vars::RegistryRO & GetVarsRegistry() const
		{ return vVarsRegistry; }

	/** Получение ссылки на реестр функций. */
	const FunctionsRegistry & GetFunctionsRegistry() const
		{ return vMainDataRO ? vMainDataRO->GetFuncTagRegistry().GetFunctionsRegistry() : vFuncTagRegistry.GetFunctionsRegistry(); }

	/** Получение ссылки на реестр тэгов-функций. */
	const FuncTagRegistry & GetFuncTagRegistry() const
		{ return vMainDataRO ? vMainDataRO->GetFuncTagRegistry() : vFuncTagRegistry; }

	/** Получение ссылки на переменные по умолчанию. */
	const Vars::Vector & GetDefaultVars() const
		{ return vDefaultVars; }

	/** Получение ссылки на реестр условий. */
	const InfConditionsRegistry & GetConditionsRegistry() const
		{ return vConditionsRegistry; }

	/** Получение ссылки на реестр InfPerson'ов. */
	const NanoLib::NameIndex & GetInfPersonRegistry() const
		{ return vMainDataRO ? vMainDataRO->GetInfPersonRegistry() : vInfPersonRegistry; }

	/** Получение указателя на шаблон из хранилища шаблонов для словарей. */
	const PatternsStorage::Pattern GetPatternFromStorage( unsigned int aId ) const
		{ return vPatternsStorage.GetPatternString( aId ); }	

	/**
	 *  Проверка подписи.
	 * @param aSignature - подпись.
	 */
	bool CheckSignature( const unsigned char aSignature[16] ) const
		{ return !memcmp( vSignature, aSignature, 16 ); }

	/**
	 *  Получение чексуммы словарей.
	 * @param aCheckSum - буфер для записи чексуммы (16 байт).
	 */
	void GetDictsCheckSum( unsigned char aCheckSum[16] )
		{ memcpy( aCheckSum, vDictsCheckSum, 16 ); }

public:
	/**
	 *  Получение шаблона по идентификатору.
	 * @param aPatternId - идентификатор шаблона.
	 * @param aPatternManipulator - манипулятор шаблона.
	 */
	InfPatternRO & GetPattern( unsigned int aPatternId, InfPatternRO & aPattern ) const;

	InfPatternItems::ConditionRO GetConditionString( unsigned int aPatternId, unsigned int aConditionId ) const
		{ return InfPatternItems::ConditionRO( GetConditionStringPtr( GetPatternPtr( aPatternId ), aConditionId ) ); }

	InfPatternItems::QuestionRO GetQuestionString( unsigned int aPatternId, unsigned int aQuestionId ) const
		{ return GetQuestionString( GetQuestionsBlockPtr( GetPatternPtr( aPatternId ) ), aQuestionId ); }

	InfPatternItems::ThatRO GetThatString( unsigned int aPatternId ) const
		{ return GetThatString( GetPatternPtr( aPatternId ) ); }

	InfPatternItems::AnswerRO GetAnswerString( unsigned int aPatternId, unsigned int aAnswerId ) const
		{ return GetAnswerString( GetAnswersBlockPtr( GetPatternPtr( aPatternId ) ), aAnswerId ); }

	/** Получение минимального идентифкатора шаблона,которого нет в базе. */
	unsigned int GetNextPatternId() const
		{ return GetBasePatternId() + GetPatternsNumber(); }

	unsigned int GetBasePatternId() const
		{ return *( reinterpret_cast<const uint32_t *>( vPatterns ) + 1 ); }

	unsigned int GetPatternsNumber() const
		{ return *reinterpret_cast<const uint32_t *>( vPatterns ); }

public:
	/** Получение числа событий. */
	unsigned int GetEventsNumber() const
		{ return vEventsBinding ? vEventsBinding[0] : 0; }

	/**
	 *  Получение числа шаблон-ответов, связанных с событием.
	 * @param aEvent - событие.
	 */
	unsigned int GetEventPatternsNumber( InfPattern::Events aEvent ) const
		{ return vEventsBinding && (int)aEvent < GetEventsNumber() ? vEventsBinding[ vEventsBinding[ (int)aEvent + 1 ] ] : 0; }

	/**
	 *  Получение aInd-ого шаблон-ответа, связанного с событием aEvent.
	 * @param aEvent - событие.
	 * @param aInd - номер шаблон-ответа.
	 */
	InfPatternItems::AnswerRO GetEventAnswer( InfPattern::Events aEvent, unsigned int aInd, unsigned int * aPatternId = nullptr, unsigned int * aQuestionId = nullptr ) const
	{
		if( !vEventsBinding || aInd >= GetEventPatternsNumber( aEvent ) )
			return {};
		else
		{
			if( aPatternId )
				*aPatternId = vEventsBinding[ vEventsBinding[ (int)aEvent + 1 ] + 1 + 2 * aInd ];
			if( aQuestionId )
				*aQuestionId = vEventsBinding[ vEventsBinding[ (int)aEvent + 1 ] + 2 + 2 * aInd ];
			return GetAnswerString( vEventsBinding[ vEventsBinding[ (int)aEvent + 1 ] + 1 + 2 * aInd ], vEventsBinding[ vEventsBinding[ (int)aEvent + 1 ] + 2 + 2 * aInd ] );
		}
	}

	/**
	 *  Получение иденитфикатора шаблона с событием aEvent.
	 * @param aEvent - событие.
	 * @param aInd - номер шаблона-ответа.
	 */
	unsigned int GetEventPatternId( InfPattern::Events aEvent, unsigned int aInd) const
	{
		if( !vEventsBinding || aInd >= GetEventPatternsNumber( aEvent ) )
			return static_cast<unsigned int>(-1);
		else
			return vEventsBinding[ vEventsBinding[ (int)aEvent + 1 ] + 1 + 2 * aInd ];

		return INF_ENGINE_SUCCESS;
	}

	/**
	 *  Получение шаблона с событием aEvent.
	 * @param aEvent - событие.
	 * @param aInd - номер шаблона-ответа.
	 * @param aPattern - шаблон - резульат.
	 */
	InfEngineErrors GetEventPattern( InfPattern::Events aEvent, unsigned int aInd, InfPatternRO & aPattern ) const
	{
		if( !vEventsBinding )
			return INF_ENGINE_WARN_UNSUCCESS;
		else
			GetPattern( GetEventPatternId( aEvent, aInd ), aPattern );

		return INF_ENGINE_SUCCESS;
	}

private:
	const void * GetPatternPtr( unsigned int aPatternId ) const
		{ return vPatterns + reinterpret_cast<const uint32_t *>( vPatterns )[aPatternId - GetBasePatternId() + 2]; }

private:
	unsigned int GetConditionStringsNum( const void * aPatternPtr ) const
		{ return *reinterpret_cast<const uint32_t *>( static_cast<const char *>( aPatternPtr ) + static_cast<const uint32_t *>( aPatternPtr )[1] ); }

	const void * GetConditionStringPtr( const void * aPatternPtr, unsigned int aConditionId ) const
	{
		if( GetConditionStringsNum( aPatternPtr ) > aConditionId )
			return static_cast<const char *>( aPatternPtr ) + reinterpret_cast<const uint32_t *>( static_cast<const char *>( aPatternPtr ) + static_cast<const uint32_t *>( aPatternPtr )[1] )[1 + aConditionId];
		else
			return nullptr;
	}

	InfPatternItems::ConditionRO GetConditionString( const void * aPatternPtr, unsigned int aConditionId ) const
		{ return InfPatternItems::ConditionRO( GetConditionStringPtr( aPatternPtr, aConditionId ) ); }

private:
	const void * GetQuestionsBlockPtr( const void * aPatternPtr ) const
		{ return static_cast<const char *>( aPatternPtr ) + static_cast<const uint32_t *>( aPatternPtr )[2]; }

	InfPatternItems::QuestionRO GetQuestionString( const void * aQuestionsBlockPtr, unsigned int aQuestionId ) const
	{
		const void * string = GetStringPtr( aQuestionsBlockPtr, aQuestionId );
		if( string )
			return InfPatternItems::QuestionRO( GetStringId( string ), vPatternsStorage[GetStringId( string )], GetStringAttrsPtr( string ) );
		else
			return InfPatternItems::QuestionRO();
	}

private:
	const void * GetThatStringPtr( const void * aPatternPtr ) const
	{
		if( reinterpret_cast<const uint32_t *>( aPatternPtr )[3] )
			return static_cast<const char *>( aPatternPtr ) + static_cast<const uint32_t *>( aPatternPtr )[3];
		else
			return nullptr;
	}

	InfPatternItems::ThatRO GetThatString( const void * aPattern ) const
	{
		const void * string = GetThatStringPtr( aPattern );
		if( string )
			return InfPatternItems::ThatRO( GetStringId( string ), vPatternsStorage[GetStringId( string )], GetStringAttrsPtr( string ) );
		else
			return InfPatternItems::ThatRO();
	}

private:
	const void * GetAnswersBlockPtr( const void * aPatternPtr ) const
		{ return static_cast<const char *>( aPatternPtr ) + static_cast<const uint32_t *>( aPatternPtr )[4]; }

	InfPatternItems::AnswerRO GetAnswerString( const void * aAnswersBlockPtr, unsigned int aAnswerId ) const
	{
		const void * string = GetStringPtr( aAnswersBlockPtr, aAnswerId );
		if( string )
			return InfPatternItems::AnswerRO( vPatternsStorage[GetStringId( string )], GetStringAttrsPtr( string ) );
		else
			return InfPatternItems::AnswerRO();
	}

private:
	const void * GetTLCSBlockPtr( const void * aPatternPtr ) const
		{ return static_cast<const char*>( aPatternPtr ) + static_cast<const uint32_t *>( aPatternPtr )[5]; }

	InfPatternItems::TLCS_RO GetTLCSString( const void * aTLCSBlockPtr, unsigned int aTLCSId ) const
	{
		const void * string = GetStringPtr( aTLCSBlockPtr, aTLCSId );
		if( string )
			return InfPatternItems::TLCS_RO( vPatternsStorage[GetStringId( string )], GetStringAttrsPtr( string ) );
		else
			return InfPatternItems::TLCS_RO();
	}

private:
	unsigned int GetStringsNumber( const void * aBlockPtr ) const
		{ return *static_cast<const uint32_t *>( aBlockPtr ); }

	const void * GetStringPtr( const void * aBlockPtr, unsigned int aStringId ) const
	{
		if( GetStringsNumber( aBlockPtr ) > aStringId )
			return static_cast<const char *>( aBlockPtr ) + static_cast<const uint32_t *>( aBlockPtr )[aStringId + 1];
		else
			return nullptr;
	}

	unsigned int GetStringId( const void * aStringPtr ) const
		{ return *static_cast<const uint32_t *>( aStringPtr ); }

	const void * GetStringAttrsPtr( const void * aStringPtr ) const
		{ return static_cast<const uint32_t *>( aStringPtr ) + 1; }

private:
	/** Основная база данных DL. */
	const DLDataRO * vMainDataRO = nullptr;

	/** Ссылка на данные. */
	const char * vDictionaries = nullptr;

	/** Индекс словарей */
	const uint32_t * vDictsIndex = nullptr;

	/** Количество словарей. */
	unsigned int vDictsCount = 0;

	/** Указатель на шаблоны. */
	const char * vPatterns;

	/** Таблица соответствий меток. */
	const uint32_t * vLabelCorrespondenceTable;

	/** Связи событий и шаблон-ответов. */
	const uint32_t * vEventsBinding = nullptr;

	/** Имена переменных. */
	Vars::RegistryRO vVarsRegistry;

	/** Индекс тэгов-функций. */
	FuncTagRegistry vFuncTagRegistry;

	/** Реестр InfPerson'ов. */
	NanoLib::NameIndex vInfPersonRegistry;

	/** Реестр имен словарей. */
	NanoLib::NameIndex vDictsRegistry;

	/** Таблица внешних идентификаторов словарей. */
	const uint32_t * vUserDictIDs = nullptr;

	/** Размер таблицы внешних идентификаторов словарей. */
	unsigned int vUserDictIDsCount = 0;

	/** Реестр базовых условий языка DL. */
	InfConditionsRegistry vConditionsRegistry;

	/** Реестр алиасов. */
	AliasRegistry vAliasRegistry;

	/** Переменные по умолчанию. */
	Vars::Vector vDefaultVars;

	/** Таблица соответствия идентификаторов шаблонов их индексам в базе. */
	NanoLib::NameIndex vTablePatternsId;
	const uint32_t * vTableIndexId = nullptr;

	/** Хранилище шаблонов словарей. */
	PatternsStorage vPatternsStorage;

	InfDictWrapManipulator::JoiningMethod vDictJoiningMethod = InfDictWrapManipulator::jmDefault;

	/** Подпись. */
	unsigned char vSignature[16];

	/** MD5-чексумма словарей. */
	unsigned char vDictsCheckSum[16];
};

/**
 *  Класс для создания базы данных DL.
 */
class DLDataWR
{
public:
	/**
	 *  Конструктор.
	 * @param aMainDataRO - данные основной базы.
	 * @param aTmpFilePath - путь к файлу для временного словарей во время компиляции.
	 * @param aMemoryLimit - максимальня память, которую можно выделять для размещения компилируемых шаблонов.
	 */
	DLDataWR( const DLDataRO * aMainDataRO, const char * aTmpFilePath, unsigned int aMemoryLimit );


public:
	/** Создание базы данных DL. */
	InfEngineErrors Create();

	/**
	 *  Сохранение данных в fstorage.
	 * @param aFStorage - открытый fstorage.
	 */
	InfEngineErrors Save( fstorage * aFStorage );

	/** Уничтожение базы данных DL. */
	void Destroy();


public:
	/**
	 *  Регистрация условия в индексе.
	 * @param aConditionType - тип условия.
	 * @param aVarId - идентификатор переменной.
	 * @param aVarValue - значение для сравнения.
	 * @param aConditionId - идентификатор условия.
	 */
	InfEngineErrors RegistrateCondition( InfBaseConditionType aConditionType, Vars::Id aVarId, const char * aVarValue, unsigned int & aConditionId )
		{ ReturnWithTraceExt( vConditionsRegistry.Registrate( aConditionType, aVarId, aVarValue, aConditionId ), INF_ENGINE_SUCCESS ); }

	/**
	 *  Регистрация InfPerson.
	 * @param aInfPerson - значение InfPerson.
	 * @param aInfPersonLength - длина значения InfPerson.
	 * @param aInfPersonId - идентификатор InfPerson.
	 */
	InfEngineErrors RegistrateInfPerson( const char * aInfPerson, unsigned int aInfPersonLength, unsigned int & aInfPersonId )
	{
		NanoLib::NameIndex::ReturnCode nlrc = vInfPersonRegistry.AddName( aInfPerson, aInfPersonLength, aInfPersonId );
		if( nlrc != NanoLib::NameIndex::rcSuccess && nlrc != NanoLib::NameIndex::rcElementExists )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		return INF_ENGINE_SUCCESS;
	}

	/**
	 *  Регистрация тэга-функции.
	 * @param aTagFunction - тэг-функция.
	 * @param aTagFuncId - идентификатор тэга-функции.
	 */
	InfEngineErrors RegistrateTagFunction( InfPatternItems::TagFunction * aTagFunction, unsigned int & aTagFuncId, bool aIsMultiple );

	/**
	 *  Добавление внешней функции.
	 * @param aFuncPath - путь к динамической библиотеки.
	 * @param aFuncID -  идентификатор функции.
	 */
	InfEngineErrors RegistrateFunction( const char * aFuncPath, unsigned int & aFuncId );

	/**
	 *  Добавление внутренней функции.
	 * @param aFuncInfo - информация о функции.
	 * @param aFuncID -  идентификатор функции.
	 */
	InfEngineErrors RegistrateFunction( const DLFunctionInfo * aFuncInfo, unsigned int & aFuncId );

	/**
	 *  Добавление шаблона в базу.
	 * @param aPattern - указатель на шаблон.
	 * @param aPatternId - идентификатор присвоенный шаблону.
	 *
	 * @todo Нужно перейти к const InfPattern& aPattern.
	 */
	InfEngineErrors AddPattern( const InfPattern & aPattern, unsigned int & aPatternId );

	/**
	 *  Добавление шаблона в хранилище шаблонов(копирования данных не происходит).
	 * @param aPattern - шаблон.
	 * @param aId - идентификатор присвоенный шаблону.
	 *
	 * @todo Заменить на регистрацию словаря.
	 */
	InfEngineErrors AddPatternString( const InfPatternItems::Array & aPattern, unsigned int & aId )
		{ ReturnWithTraceExt( vPatternsStorage.AddPatternString( aPattern, aId ), INF_ENGINE_SUCCESS ); }

	/**
	 *  Установка индекса соответствия строковых идентификаторов шаблонов их номерам в базе.
	 * @param aIndex - словарь с идентификаторами шаблонов.
	 * @param aTable - таблица с номерами шаблонов в базе.
	 */
	void SetPatternIdIndex( const NanoLib::NameIndex * aIndex, const avector<unsigned int> * aTable )
	{
		vTablePatternsId = aIndex;
		vTableIndexId = aTable;
	}

	/**
	 *  Получение идентификатора зарегистрированного словаря по его имени.
	 * @param aDictName - имя словаря.
	 * @param aDictNameLength - длина имени словаря.
	 */
	const unsigned int * SearchDictName( const char * aDictName, unsigned int aDictNameLength ) const
		{ return vDictsRegistry.Search( aDictName, aDictNameLength ); }

	/**
	 *  Регистрация названия словаря. Если такое название уже было зарегистрировано ранее, возвращает INF_ENGINE_WARN_UNSUCCESS.
	 * @param aDictName - имя словаря.
	 * @param aDictNameLength - длина имени словаря.
	 */
	InfEngineErrors RegistryDictName( const char * aDictName, unsigned int aDictNameLength );

	/**
	 *  Добавление словаря в основную базу.
	 * @param aDict - указатель на словарь.
	 */
	InfEngineErrors AddMainDict( InfDict * aDict );

	/**
	 *  Добавление словаря в пользовательскую базу.
	 * @param aDict - указатель на словарь.
	 */
	InfEngineErrors AddUserDict( InfDict * aDict );

	/**
	 *  Добавление значения для переменной.
	 * @param aVarId - идентификатор переменной.
	 * @param aVarValue - дефолтное знаения переменной.
	 * @param aVarValueLength - длина дефолтного значения переменной.
	 */
	InfEngineErrors AddVariable( Vars::Id aVarId, const char * aVarValue, unsigned int aVarValueLength )
		{ ReturnWithTraceExt( vDefaultVars.SetTextValue( aVarId, aVarValue, aVarValueLength ), INF_ENGINE_SUCCESS ); }

	/**
	 *  Добавить алиас в реестр алиасов.
	 * @param aAlias - укзазатель на разобранный алиас.
	 */
	InfEngineErrors RegisterAlias( Alias * aAlias )
		{ ReturnWithTraceExt( vAliasRegistry.RegisterAlias( aAlias ), INF_ENGINE_SUCCESS ); }

	/**
	 *  Установить попись базы.
	 * @param aSignature - 16-байтная подпись.
	 */
	void SetSignature( const unsigned char aSignature[16] )
		{ memcpy( vSignature, aSignature, 16 ); }

	/**
	 *  Получение идентификатора пользовательского или основного словаря по его имени.
	 * @param aDictName - имя словаря.
	 * @param aDictLength - длина имени словаря.
	 */
	const unsigned int * GetDictId( const char * aDictName, unsigned int aLength ) const;

private:
	unsigned int GetNeedMemorySizeForInfPattern( const InfPattern & aPattern );

	/** Получение необходимого размера памяти для сохранения объекта.  */
	unsigned int GetNeedMemorySize() const;

	/**
	 *  Добавление словаря.
	 * @param aDict - указатель на словарь.
	 */
	InfEngineErrors AddDict( InfDict * aDict );

	/**
	 *  Подготовка словарей перед сохранением.
	 */
	InfEngineErrors FinalizeDicts();

	/**
	 *  Раскрытие инструкций "--include" в словарях.
	 */
	InfEngineErrors DiscloseDictsIncludes();

	/**
	 *  Раскрытие инструкций "--include" в заданном словаре.
	 */
	InfEngineErrors DiscloseDictIncludes( InfDict * aDict, std::set<unsigned int> & aDisclosedDicts );

	/**
	 *  Сохранение разобранного и подготовленного словаря.
	 * @param aDict - словарь.
	 */
	InfEngineErrors SaveDict( const InfDict * aDict );

	/**
	 *  Сохранение привяок событий.
	 * @param aFStorage - сторадж для сохранения данных.
	 */
	InfEngineErrors SaveEventsBindings( fstorage * aFStorage );

public:
	/** Получение реестра переменных. */
	Vars::RegistryRW & GetVarsRegistry()
		{ return vVarsRegistry; }

	/** Получение реестра функций. */
	const FunctionsRegistry & GetFunctionsRegistry() const
		{ return vMainDataRO ? vMainDataRO->GetFuncTagRegistry().GetFunctionsRegistry() : vFuncTagRegistry.GetFunctionsRegistry(); }

	/** Получение реестра алиасов */
	const AliasRegistry & GetAliasRegistry() const
		{ return vAliasRegistry; }

	/**
	 *  Получение статистичекой информации о реестре тэгов-функций.
	 * @param aRealSize - суммарный размер памяти в байтах, занимаемой всеми тэгами функций.
	 * @param aIndexedSize - суммарный размер памяти в байтах, занимаемой всеми тэгами функций после индексации.
	 * @param aTagFuncCount - количество тэгов-функций.
	 * @param aUniqueTagFuncCount - количество уникальных тэгов-функций.
	 * @param aMultipleTagFuncCount - количество тэгов-функций, значение которых не может быть закэшировано.
	 * @param aHashTableMaxListLen - максимальная длина цепочек, использующихся в хэш-таблице для разрешения коллизий.
	 * @param aHashTableAvgListLen - средняя длина цепочек, использующихся в хэш-таблице для разрешения коллизий.
	 */
	void GetTagFuncRegistryStat( unsigned int & aRealSize, unsigned int & aIndexedSize,
								 unsigned int & aTagFuncCount, unsigned int & aUniqueTagFuncCount,
								 unsigned int & aMultipleTagFuncCount,
								 unsigned int & aHashTableMaxListLen, double & aHashTableAvgListLen) const;


	/** Получение хранилища шаблонов для словарей. */
	const PatternsStorage & GetPatternsStorage() const
		{ return vPatternsStorage; }


	/**
	 *  Помечает заданную DL-функцию как использованную при компиляции шаблонов.
	 * Используется для создания сигнатуры лингвистических данных.
	 * @param aFuncId - идентификатор DL-функции, зарегистрированной в реестре функций.
	 */
	InfEngineErrors SetFunctionUsed( unsigned int aFuncId );

	/**
	 *  Возвращает истинное значение если заданная DL-функция использовалась в скомпилированных шаблонах.
	 * @param aFuncId - идентификатор DL-функции, зарегистрированной в реестре функций.
	 */
	bool FunctionIsUsed( unsigned int aFuncId ) const
		{ return vUsedDLFunctionsIds.size() > aFuncId ? vUsedDLFunctionsIds[aFuncId] : false; }

private:
	/** Ссылка на основную базу DL. */
	const DLDataRO * vMainDataRO = nullptr;

	typedef std::pair<const void *, unsigned int> block;
	/** Список скомпилированных шаблонов. */
	std::vector<block> vPatternsData;

	/** Список скомпилированных словарей. */
	Blocks vCompiledDicts;

	/** Список разобранных словарей. */
	avector<InfDict *> vDicts;

	/** Реестр базовых условий языка DL. */
	InfConditionsRegistry vConditionsRegistry;

	/** реестр тэгов-функций. */
	FuncTagRegistry vFuncTagRegistry;

	/** Реестр переменных. */
	Vars::RegistryRW vVarsRegistry;

	/** Реестр алиасов. */
	AliasRegistry vAliasRegistry;

	/** Реестр InfPerson'ов. */
	NanoLib::NameIndex vInfPersonRegistry;

	/** Переменные по умолчанию. */
	Vars::Vector vDefaultVars;

	/** Реестр словарей. */
	NanoLib::NameIndex vDictsRegistry;

	/** Таблица внешних идентификаторов словарей (для пользовательских словарей). */
	avector<unsigned int> vUserDictsIDs;

	/** Индекс имен меток. */
	NanoLib::NameIndex vPatternLabels;

	/** Массив флагов, показывающих, что данная метка является корректной, то есть является меткой шаблона. */
	avector<bool> vLabelValidate;

	/** Массив соответствий идентификаторов меток идентификаторам шаблонов. */
	avector<unsigned int> vLabelCorrespondence;

	/** Массив флагов для обозначения DL-функций, использованых в шаблонах. Используется для создания сигнатуры лингвистических данных. */
	avector<bool> vUsedDLFunctionsIds;

	/** Таблица соответствия идентификаторов шаблонов их индексам в базе. */
	const NanoLib::NameIndex * vTablePatternsId = nullptr;
	const avector<unsigned int> * vTableIndexId = nullptr;

	/** Хранилище шаблонов. */
	PatternsStorage vPatternsStorage;

	/** Попись базы. */
	unsigned char vSignature[16];

	/** Идентификатор последнего добавленного шаблона. */
	unsigned int vPatternId = 0;

	/** Флаг, показывающий, что словари были подготовлены для сохранения. */
	unsigned int vDictsAreFinalized = 0;

	/** Максимальное число временных переменных в одном шаблон-ответе. */
	unsigned int vTmpVarsNumber = 0;

	/** Идентификаторы шаблонов с привязанными событиями. */
	using EventType = std::tuple<unsigned int,unsigned int,int>;
	std::vector<EventType> vEvents[3];

	nMemoryAllocator vAllocator;
};

#endif /** INF_ENGINE_DLDATA_HPP */
