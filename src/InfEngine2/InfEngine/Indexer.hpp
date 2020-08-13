#ifndef INDEXER_HPP
#define INDEXER_HPP

#include "InfDict/InfDict.hpp"
#include "InfPattern.hpp"
#include "IndexBase.hpp"
#include "PatternsStorage.hpp"

/**
 *  Базовый индексатор шаблонов. Является основой для построения индексаторов шаблонов разных типов.
 */
class BaseIndexer
{
protected:
	/**
	 *  Конструктор.
	 * @param aSectionId - идентификатор начала пакета секций для сохранения в fstorage.
	 * @param aAllocator - менеджер памяти.
	 */
	BaseIndexer( fstorage_section_id aSectionId, nMemoryAllocator & aAllocator );
	~BaseIndexer() { BaseIndexer::Destroy(); }


protected:
	/** Создание и инициализация индексатора. */
	InfEngineErrors Create();

	/** Уничтожение индексатора. */
	virtual void Destroy();


public:
	/** Сохраняет базу синонимов во временный fstorage и загружает из него на чтение. */
	InfEngineErrors FinalizeSynonyms();
	
	/** Сохраняет базу заменяемых символов во временный fstorage и загружает из него на чтение. */
	InfEngineErrors FinalizeSymbolyms();


public:
	/**
	 *  Сохранение индексатора в сторадж.
	 * @param aFStorage - ссылка на открытый fstorage для сохранения.
	 */
	virtual InfEngineErrors Save( fstorage * aFStorage ) = 0;


protected:
	/**
	 *  Сохранение базовой части индексатора. Нужно использовать до вызова сохранения дополнительных частей индексатора в наследуемых классах.
	 * @param aFStorage - ссылка на открытый fstorage для сохранения.
	 */
	InfEngineErrors BaseSave( fstorage * aFStorage );


public:
	/**
	 *  Индексирование словаря.
	 * @param aDict - указатель на разобранный словаря.
	 * @param aDictPatternsStorage - хранилище шаблонов для словарей.
	 */
	InfEngineErrors AddDict( const InfDict * aDict, const PatternsStorage & aDictPatternsStorage );


public:
	/**
	 *  Индексирование шаблона.
	 * @param aPattern - шаблон.
	 * @param aPatternId - идентификатор шаблона.
	 */
	virtual InfEngineErrors AddPattern( const InfPattern & aPattern, unsigned int aPatternId ) = 0;

	/**
	 *  Подготовка шаблона для индексации и сохранения: обновления весов и флагов.
	 * @param aPattern - шаблон.
	 */
	virtual InfEngineErrors PreparePattern( InfPattern & aPattern ) const = 0;

protected:
	/**
	 *  Подготовка шаблона для индексации и сохранения: обновления весов и флагов.
	 * @param aIndexBase - индексная база.
	 * @param aPattern - шаблон.
	 */
	InfEngineErrors PreparePattern( const IndexBaseWR & aIndexBase, InfPattern & aPattern ) const;


public:
	/** Получение доступа к базе синонимов. */
	SynonymBaseRO & GetSynonymBaseRO() { return vSynonymBaseRO; }
	
	/** Получение доступа к базе заменяемых символов. */
    SymbolymBaseRO & GetSymolymsBaseRO() { return vSymbolymBaseRO; }


protected:
	/** Индикатор состояния индексатора. */
	enum class State
	{
		None  = 1,  /** Индексатор не инициализирован. */
		Ready = 2   /** Индексатор готов к работе. */
	} vState { State::None };

	/** Лингвистический процессор. */
	InfLingProc * vLp { nullptr };

	/** База терминов для словарей. */
	FltTermsBase vBaseDict;

	/** База терминов для словарей. */
	TermsStorageWR vDictTermsStorage;

	/** Для совместимости. */
	unsigned int vSectionId { 0 };

	/** База синонимов для записи. */
	SynonymBaseWR vSynonymBaseWR;	

	/** База синонимов для чтения. */
	SynonymBaseRO vSynonymBaseRO;
	
	/** База заменяемых символов для записи. */
	SymbolymBaseWR vSymbolymBaseWR;
	
	/** База заменяемых символов для чтения. */
    SymbolymBaseRO vSymbolymBaseRO;

private:
	/** Временный fstorage для сохранения базы синонимов перед заменой. */
	fstorage * vFscmn_tmp { nullptr };
};

/**
 *  Индексатор для шаблонов на языке DL.
 */
class InfIndexer : public BaseIndexer
{
public:
	/**
	 *  Конструктор индексатора.
	 * @param aMemoryAllocator - менеджер памяти.
	 */
	InfIndexer( nMemoryAllocator & aMemoryAllocator );


public:
	/** Создание и инициализация индексатора. */
	InfEngineErrors Create();

	/** Уничтожение индексатора. */
	void Destroy();


public:
	/**
	 *  Сохранение индексатора в fstorage.
	 * @param aFStorage - ссылка на открытый fstorage для сохранения.
	 */
	InfEngineErrors Save( fstorage * aFStorage );


public:
	/**
	 *  Индексирование шаблона.
	 * @param aPattern - шаблон.
	 * @param aPatternId - идентификатор шаблона.
	 */
	InfEngineErrors AddPattern( const InfPattern & aPattern, unsigned int aPatternId );

	/**
	 *  Компилирует и индексирует группу синонимов.
	 * @param aSynGroup - группа синонимов (один главный синоним и несколько неглавных).
	 * @param aSynGroupN - номер группы синонимов в словаре синонимов.
	 * @param aSynDictName - название словаря синонимов.
	 * @param aSynDictNameLen - длина названия словаря синонимов, не считая '\0'.
	 * @param aErrorMsg - сообщение об ошибке.
	 */
	InfEngineErrors AddSynonymGroup( const SynonymParser::SynonymGroup & aSynGroup, unsigned int aSynGroupN,
									 const char * aSynDictName, unsigned int aSynDictNameLen, const char* & aErrorMsg );
	
	/**
	 *  Компилирует и индексирует группу заменяемых символов.
     * @param aSymGroup - группа заменяемых символов (один главный символ и несколько заменяемых).
     * @param aSymGroupN - номер группы в словаре.
     * @param aErrorMsg - сообщение об ошибке.
	 */
    InfEngineErrors AddSymbolymGroup(const SynonymParser::SynonymGroup & aSymGroup, unsigned int aSymGroupN, const char* & aErrorMsg );

	/**
	 *  Подготовка шаблона для индексации и сохранения: обновления весов и флагов.
	 * @param aPattern - шаблон.
	 */
	InfEngineErrors PreparePattern( InfPattern & aPattern ) const { return BaseIndexer::PreparePattern( vQBase, aPattern ); }

protected:
	/** Индексная база шаблон-вопросов. */
	IndexBaseWR vQBase;
	/** Индексная база для that-шаблонов. */
	IndexBaseWR vTBase;

	/** Временный fstorage для сохранения базы синонимов перед заменой. */
	fstorage * vFscmn_tmp { nullptr };
};

/**
 *  Индексатор для эллипсисов.
 */
class EllipsisIndexer : public BaseIndexer
{
public:
	/**
	 *  Конструктор индексатора эллипсисов.
	 * @param aMemoryAllocator - менеджер памяти.
	 */
	EllipsisIndexer( nMemoryAllocator & aMemoryAllocator );


public:
	/** Создание индексатора. */
	InfEngineErrors Create();
	/** Уничтожение индексатора. */
	void Destroy();

	/**
	 *  Сохранение индексатора в storage.
	 * @param aFStorage - ссылка на открытый fstorage для сохранения.
	 */
	InfEngineErrors Save( fstorage * aFStorage );


public:
	/** Типы шаблонов эллипсисов. */
	typedef enum
	{
		/** Шаблон эллипсисов первого типа. */
		etFirst   = 0,
		/** Шаблон эллипсисов второго типа. */
		etSecond  = 1,
		/** Шаблон эллипсисов третьего типа. */
		etThird   = 2
	} EllipsisType;

	/**
	 *  Установка флага, определючщего типа шаблона эллипсисов, с которым будут производиться дальнейшие действия.
	 * @param aMode - типа шаблона эллипсисов.
	 */
	void SetMode( EllipsisType aMode ) { vMode = aMode; }

	/**
	 *  Индексирования шаблона.
	 * @param aPattern - шаблон.
	 * @param aPatternId - идентификатор шаблона.
	 */
	InfEngineErrors AddPattern( const InfPattern & aPattern, unsigned int aPatternId );

	/**
	 *  Подготовка шаблона для индексации и сохранения: обновления весов и флагов.
	 * @param aPattern - шаблон.
	 */
	virtual InfEngineErrors PreparePattern( InfPattern & aPattern ) const { return BaseIndexer::PreparePattern( vFBase, aPattern ); }


protected:
	/**
	 *  Флаг, определяющий какого типа шаблон эллипсисов индексируется.
	 */
	EllipsisType vMode;

	/**
	 * Для шаблонов эллипсисов первого типа используется индексная база vQBase.
	 * Для шаблонов эллипсисов второго типа используется индексная база vTBase.
	 */

	/** Индексная база для шаблонов эллипсисов первого типа. */
	IndexBaseWR vFBase;
	/** Индексная база для шаблонов эллипсисов второго типа. */
	IndexBaseWR vSBase;
	/** Индексная база для шаблонов эллипсисов третьего типа. */
	IndexBaseWR vTBase;

};

#endif // INDEXER_HPP
