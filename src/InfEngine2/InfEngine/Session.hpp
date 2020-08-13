#ifndef __Session_hpp__
#define __Session_hpp__

#include <NanoLib/NanoRandomBox.hpp>
#include <NanoLib/FlagsStorage.hpp>
#include <NanoLib/Cache.hpp>

#include "InfPattern/Items/TagDict.hpp"

#include "Vars/Vector.hpp"

#include "RequestStat.hpp"
#include "iAnswer.hpp"

/**
 *  Класс, описывающий контейнер, содержащий состояния словарей udict и sudict.
 * @todo Требуется прокомментировать весь класс. И вынести в отдельный файл, чтобы не путать с сессией.
 */
class DictsStates
{
protected:
	using DictState = RandomBox;

public:
	DictsStates() {}

	InfEngineErrors GetRandomDictLineId( unsigned int aDictId, InfPatternItems::DictType aDictType, unsigned int aDictSize, unsigned int & aLineId );

	InfEngineErrors RemoveLineIdFromDict( unsigned int aDictId, InfPatternItems::DictType aDictType, unsigned int aDictSize, unsigned int aLineId );

	InfEngineErrors ResetDictState( unsigned int aDictId );

	InfEngineErrors IsUsed( unsigned int aDictId, InfPatternItems::DictType aDictType, unsigned int aLineId, bool & aAlreadyUsed );

	InfEngineErrors RollbackLastChanges( );

	unsigned int GetRemainingAmount( unsigned int aDictId, unsigned int aDictSize );

	InfEngineErrors Drop();

public:
	/** Возвращает размер непрерывного блока памяти в байтах, необходимого для сохранения объекта. */
	unsigned int GetNeedMemorySize( ) const;

	/**
	 *  Сохраняет объект в памяти.
	 * @param aBuffer - блок памяти для сохранения объекта.
	 */
	unsigned int Save( void * aBuffer ) const;

	/**
	 *  Загружает объект из памяти.
	 * @param aBuffer - блок памяти, содержащий созранённый ранее объект.
	 * @param aBufferSize  - размер блока памяти.
	 */
	InfEngineErrors Load( const void * aBuffer, unsigned int aBufferSize );

protected:
	class Node
	{
	public:
		Node( unsigned int aDictId, unsigned int aDictSize ):
			vDictId( aDictId ), vDictState( aDictSize ) {}

	public:
		unsigned int vDictId;
		DictState vDictState;
	};

	typedef avector<Node*> StatesSet;

	InfEngineErrors GetDictState( unsigned int aDictId, const DictState *& aDictState );

protected:
	InfEngineErrors AddDictState( unsigned int aDictId, unsigned int aDictSize, DictState *& aDictState );

	InfEngineErrors FindDictState( unsigned int aDictId, DictState *& aDictState );

protected:
	// Структура для запоминания изменений в состояниях.
	struct LogNode
	{
		LogNode( unsigned int aDictId = 0, unsigned int aLineId = 0 ):
			vDictId( aDictId ), vLineId( aLineId ) {}

		LogNode( const LogNode & aLogNode ):
			vDictId( aLogNode.vDictId ), vLineId( aLogNode.vLineId ) {}

		unsigned int vDictId = 0; // Идентификатор словаря.
		unsigned int vLineId = 0; // Идентификатор выданной строки словоря.
	};

	StatesSet vStates;

	// Список изменений в состояниях.
	avector<LogNode> vLog;

	nMemoryAllocator vAllocator;
};

/**
 *  Класс, описывающий сессию.
 */
class Session: public Vars::Vector
{
public:
	/**
	 *  Конструктор.
	 * @param aVarsRegistry - реестр переменных.
	 */
	Session( const Vars::Registry & aVarsRegistry ):
		Vars::Vector( aVarsRegistry ) {}

	/**
	 *  Конструктор.
	 * @param aParentSession - родительская сессия.
	 */
	Session( const Session & aParentSession ): Vars::Vector{ *(aParentSession.vVarsRegistry) }
		{ SetDefaultValues( aParentSession ); }

public:
	/** Переключение реестра с проверкой совместимости. */
	using Vector::SwitchRegistry;

public:
	/**
	 *  Установка значений переменных.
	 * @param aInfId - идентификатор инфа.
	 * @param aSourceSession - массив данных для установки значений.
	 * @param aUnknownVariablesCounter - счетчик незарегистрированных переменных.
	 */
	InfEngineErrors Set( unsigned int aInfId, avector<Vars::Raw> & aSourceSession, unsigned int & aUnknownVariablesCounter );

	/**
	 *  Обновление сессии на основании сформированного ответа.
	 * @param aAnswer - данные сформированного ответа.
	 * @param aInstructionsOnly - флаг, показывающий, что нужно обновить нужно только переменные, описанные в инструкциях.
	 */
	InfEngineErrors Update( const iAnswer & aAnswer, bool aInstructionsOnly = false );


public:
	/** Работа с флагами. */

	/**
	 *  Установка флага для сработавшего шаблон-ответа.
	 * @param aPatternId - идентифкатор шаблона.
	 * @param aAnswerId - идентифкатор шаблон-ответа.
	 */
	InfEngineErrors SetAnswerFlag( unsigned int aPatternId, unsigned int aAnswerId );

	/**
	 *  Снятие флага для сработавшего шаблон-ответа.
	 * @param aPatternId - идентифкатор шаблона.
	 * @param aAnswerId - идентифкатор шаблон-ответа.
	 */
	InfEngineErrors UnsetAnswerFlag( unsigned int aPatternId, unsigned int aAnswerId );


public:
	/** Получение идентификатора инфа. */
	unsigned int GetInfId( ) const
		{ return vInfId; }

	/** Получение идентификатора сессии. */
	unsigned int GetSessionId( ) const
		{ return vSessionId; }

	/** Получение информации о состояниях словарей udict и sudioct. */
	DictsStates & GetDictsStates( ) const
		{ return vDictsStates; }


public:
	/**
	 *  Сохранение сессии в кэш.
	 * @param aSessionId - идентификатор сессии.
	 * @param aCache - кэш.
	 */
	InfEngineErrors Save( unsigned int aSessionId, NanoLib::Cache & aCache );

	/**
	 *  Сохранение сессии в буфер.
	 * @param aBuffer - буфер.
	 */
	InfEngineErrors Save( char * aBuffer ) const;

	/**
	 * Загрузка сессии из кэша.
	 * @param aSessionId - идентификатор сессии.
	 * @param aCache - кэш.
	 */
	InfEngineErrors Load( unsigned int aSessionId, NanoLib::Cache & aCache )
	{
		RequestStat request_stat;

		return Load( aSessionId, aCache, request_stat );
	}

	/**
	 * Загрузка сессии из буфера.
	 * @param aBuffer - буфер.
	 * @param aBufferSize - размер буфера.
	 */
	InfEngineErrors Load( const char * aBuffer, unsigned int aBufferSize );

	/**
	 * Загрузка сессии из кэша.
	 * @param aSessionId - идентификатор сессии.
	 * @param aCache - кэш.
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 */
	InfEngineErrors Load( unsigned int aSessionId, NanoLib::Cache & aCache, RequestStat & aRequestStat );

	/**
	 *  Удаление сессии из кэша.
	 * @param aSessionId - идентификатор сессии.
	 * @param aCache - кэш.
	 */
	InfEngineErrors Delete( unsigned int aSessionId, NanoLib::Cache & aCache );

public:
	/**
	 *  Проверка md5-чексуммы пользовательских словарей.
	 * @param aCheckSum - проверяемая md5-чексумма пользовательских словарей.
	 */
	bool CheckUserDictsCheckSum( unsigned char aCheckSum[16] )
		{ return !memcmp( vUserDictsCheckSum, aCheckSum, 16 ); }

	/**
	 *  Установка новой md5-чексуммы пользовательских словарей.
	 * @param aCheckSum - новая md5-чексумма пользовательских словарей.
	 */
	void SetUserDictsCheckSum( unsigned char aCheckSum[16] )
		{ memcpy( vUserDictsCheckSum, aCheckSum, 16 ); }


public:
	bool GetAnswerFlag( unsigned int aPatternId, unsigned int aAnswerId ) const
		{ return vAnswersFlags.get( aPatternId, aAnswerId ); }

public:
	/** Очистка данных. */
	void Reset( )
	{
		Vars::Vector::Reset( );
		vInfId = 0;
		vSessionId = 0;
		vAnswersFlags.reset();
	}

	/** Получение размера памяти необходимого для сохранения сессии. */
	unsigned int GetNeedMemorySize( ) const
	{
		unsigned int NeedMemorySize = 5*sizeof(uint32_t) + 32;
		binary_data_alignment( NeedMemorySize );
		NeedMemorySize += Vars::Vector::GetNeedMemorySize( );
		binary_data_alignment( NeedMemorySize );
		NeedMemorySize += vDictsStates.GetNeedMemorySize( );
		binary_data_alignment( NeedMemorySize );
		NeedMemorySize += vAnswersFlags.size();
		binary_data_alignment( NeedMemorySize );
		return NeedMemorySize;
	}

private:
	/** Идентификатор инфа. */
	unsigned int vInfId = 0;

	/** Идентификатор сессии. Не сохраняется!!! */
	unsigned int vSessionId = 1;

	/** Флаги сработавших шаблон-ответов. */
	NanoLib::FlagsStorage vAnswersFlags;

	/** @todo Состояния словарей не должны быть элементом сессии. Нужно вынести их и сделать функции сохранения и загрузки их из сессии. */
	/** Состояния словарей. */
	mutable DictsStates vDictsStates;

	/** Чексумма состояния словарей. */
	unsigned char vUserDictsCheckSum[16];
};

#endif /** __Session_hpp__ */
