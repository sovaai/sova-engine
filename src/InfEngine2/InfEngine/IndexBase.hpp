#ifndef __IndexBase_hpp__
#define __IndexBase_hpp__

#include "Synonyms/SynonymBase.hpp"
#include "Symbolyms/SymbolymBase.hpp"
#include "TermsStorage.hpp"
#include "InfPattern/Items.hpp"

/**
 *  Индексная база для индексирования строк шаблонов языка DL.
 */
class IndexBaseWR
{
public:
	/**
	 *  Конструктор индексной базы.
	 * @param aLp - лингвистический процессор.
	 * @param aSectionId - идентификатор начала пакета секций fstorage, соответствующих базе. Всего используется TERMS_SECTION_MAX_NUMBER+1 секций.
	 * @param aMemoryAllocator - менеджер памяти.
	 */
	IndexBaseWR( InfLingProc* & aLp, fstorage_section_id aSectionId, nMemoryAllocator & aMemoryAllocator );

	/** Деструктор индексной базы. */
	~IndexBaseWR() { Destroy(); }

public:
	/** Создание и инициализация индексной базы. */
	InfEngineErrors Create();

	/** Уничтожение индексной базы. */
	void Destroy();

public:
	/**
	 *  Сохранение индексной базы в fstorage.
	 * @param aFStorage - ссылка на открытый fstorage.
	 */
	InfEngineErrors Save( fstorage * aFStorage );


public:
	/**
	 *  Подготовка шаблон-строки для индексации и сохранения: обновления весов и флагов.
	 * @param aPatternString - шаблон-строка.
	 */
	InfEngineErrors PreparePatternString( InfPatternItems::QuestionWR & aPatternString ) const;

	/**
	 *  Индексация строки шаблона.
	 * @param aQst - строка шаблона.
	 * @param aId - идентификатор шаблона.
	 * @param aSubId - идентификатор строки шаблона.
	 * @param aInfPersonId - идентификатор InfPerson шаблона.
	 */
	InfEngineErrors AddPatternString( const InfPatternItems::QuestionWR & aQst, unsigned int aId, unsigned int aSubId, unsigned int aInfPersonId );

public:
	// Идентификаторы безтермового шаблона.
	struct IDWT
	{
		IDWT( unsigned int aId = 0, unsigned int aSubId = 0, unsigned int aInfPersonId = 0 ) : vId( aId ), vSubId( aSubId ), vInfPersonId( aInfPersonId ) {}

		// Идентификатор шаблона.
		unsigned int vId;
		// Идентификатор шаблон-вопроса.
		unsigned int vSubId;
		// InfPersonId.
		unsigned int vInfPersonId;
	};

private:
	/** Вспомогательная переменная. */
	avector<TermsStorage::TermAttrs> vAttrs;
	/** Вспомогательная переменная. */
	avector<unsigned int> vTermPos;

protected:
	/** Лингвистический процессор. */
	InfLingProc* & vLp;

	/** База терминов. */
	FltTermsBase vTBase;

	/** База атрибутов терминов. */
	TermsStorageWR vTermsStorageWR;

	/** Массив для хранения идентификаторов безтермовых шаблонов. */
	avector<IDWT> vIDWT;

	/** Идентификатор секции безтермовых шаблонов. */
	const fstorage_section_id vExtSectionID;

	/** Таблица стоп-символов.  */
	TermsStopDict vStopDict;

	/** Служебные объекты. */
	mutable DocImage vTmpDocImage;
	mutable CDocMessage vTmpDocMessage;
};

/**
 *  Индексная база для работы с проиндексированными строками шаблонов языка DL.
 */
class IndexBaseRO
{
public:
	/**
	 *  Конструктор индексной базы.
	 * @param aLp - лингвистический процессор.
	 * @param aSectionId - идентификатор начала пакета секций fstorage, соответствующих базе. Всего используется TERMS_SECTION_MAX_NUMBER+1 секций.
	 */
	IndexBaseRO( InfLingProc* & aLp, fstorage_section_id aSectionId );

	/** Деструктор индексной базы. */
	~IndexBaseRO() { Close(); }


public:
	/**
	 *  Открытие индексной базы.
	 * @param aFStorage - fstorage, содержащий индексную базу.
	 */
	InfEngineErrors Open( fstorage * aFStorage );

	/** Закрытие индексной базы. */
	void Close();


public:
	bool IsReady() { return vState == stReady; }


public:
	InfEngineErrors Check( const DocImage & aRequest, FltTermsMatcher & aMatcher );

	InfEngineErrors LemmatizeDoc( DocText & aDocText, DocImage & aDocImage );


public:
	unsigned int GetNumberOfPWT() const { return ( (const uint32_t*)vIDWT )[1]; }

	unsigned int GetIdOfPWT( unsigned int aNum ) const { return reinterpret_cast<const uint32_t*>( vIDWT + 2 * sizeof( uint32_t ) + aNum * 3 * sizeof( uint32_t ) )[0]; }

	unsigned int GetSubIdOfPWT( unsigned int aNum ) const { return reinterpret_cast<const uint32_t*>( vIDWT + 2 * sizeof( uint32_t ) + aNum * 3 * sizeof( uint32_t ) )[1]; }

	unsigned int GetInfPersonIdOfPWT( unsigned int aNum ) const { return reinterpret_cast<const uint32_t*>( vIDWT + 2 * sizeof( uint32_t ) + aNum * 3 * sizeof( uint32_t ) )[2]; }

	const TermsStorageRO & GetTermsStorage() const { return vTermsStorageRO; }

	const StopDictEx & GetStopDict() const { return vStopDict; }


protected:
	/** Состояние индексной базы. */
	enum
	{
		/** База не готова. */
		stNone  = 0,
		/** База готова к поиску. */
		stReady = 1
	} vState = stNone;

	/** Лингвистический процессор. */
	InfLingProc* & vLp;

	/** База терминов. */
	FltTermsBase vTBase;

	/** База атрибутов терминов. */
	TermsStorageRO vTermsStorageRO;

	/** Массив для хранения идентификаторов безтермовых шаблонов. */
	const char * vIDWT = nullptr;

	/** Идентификатор секции. */
	const fstorage_section_id vSectionId;

	/** Идентификатор секции безтермовых шаблонов. */
	const fstorage_section_id vExtSectionId;

	/** Таблица стоп-символов.*/
	StopDictEx vStopDict;
};

/**
 *  Индексная база для основного движка.
 */
class InfIndexBaseRO
{
public:
	/** Конструктор индексной базы. */
	InfIndexBaseRO();


public:
	/**
	 *  Открытие индексной базы.
	 * @param aFStorage - ссылка на открытый fstorage.
	 */
	InfEngineErrors Open( fstorage * aFStorage );

	/** Закрытие индексной базы. */
	void Close();


public:
	/** Получение ссылки на индексную базу шаблон-словарей. */
	IndexBaseRO & GetIndexDBase() { return vIndexDBase; }

	/** Получение ссылки на индексную базу шаблон-вопросов. */
	IndexBaseRO & GetIndexQBase() { return vIndexQBase; }

	/** Получение ссылки на индексную базу шаблон-that'ов. */
	IndexBaseRO & GetIndexTBase() { return vIndexTBase; }

	/** Получение ссылки на базу синонимов. */
	SynonymBaseRO & GetSynonymBase() { return vSynonymBase; }

	const SynonymBaseRO & GetSynonymBase() const { return vSynonymBase; }

    /** Получение ссылки на базу заменяемых символов. */
    SymbolymBaseRO & GetSymbolymBase() { return vSymbolymBase; }

    const SymbolymBaseRO & GetSymbolymBase() const { return vSymbolymBase; }


public:
	/**
	 *  Лемматизация текста.
	 */
	InfEngineErrors LemmatizeDoc( DocText & aDocText, DocImage & aDocImage ) const;


private:
	/** Лингвистический процессор необходим для работы с терминами. */
	mutable InfLingProc * vLp;

	/** Индексная база словарей. */
	IndexBaseRO vIndexDBase;
	/** Индексная база шаблон-вопросов. */
	IndexBaseRO vIndexQBase;
	/** Индексная база для шаблон-that'ов. */
	IndexBaseRO vIndexTBase;
    /** База заменяемых символов. */
    SymbolymBaseRO vSymbolymBase;
	/** База синонимов. */
	SynonymBaseRO vSynonymBase;
};

/**
 *  Индексная база для эллипсисов.
 */
class EllipsisIndexBaseRO
{
public:
	/** Конструктор индексной базы. */
	EllipsisIndexBaseRO();


public:
	/**
	 *  Открытие индексной базы.
	 * @param aFStorage - ссылка на открытый fstorage.
	 */
	InfEngineErrors Open( fstorage * aFStorage );

	/** Закрытие индексной базы. */
	void Close();


public:

	/** Получение ссылки на индексную базу шаблонов первого типа. */
	IndexBaseRO & GetIndexFBase() { return vIndexFBase; }

	/** Получение ссылки на индексную базу шаблонов второго типа. */
	IndexBaseRO & GetIndexSBase() { return vIndexSBase; }

	/** Получение ссылки на индексную базу шаблонов третьего типа. */
	IndexBaseRO & GetIndexTBase() { return vIndexTBase; }

	/** Получение ссылки на индексную базу шаблон-словарей. */
	IndexBaseRO & GetIndexDBase() { return vIndexDBase; }

	/** Получение ссылки на базу синонимов. */
	SynonymBaseRO & GetSynonymBase() { return vSynonymBase; }


private:
	/** Лингвистический процессор необходим для работы с терминами. */
	InfLingProc * vLp;

	/** Индексная база шаблонов первого типа. */
	IndexBaseRO vIndexFBase;
	/** Индексная база шаблонов второго типа. */
	IndexBaseRO vIndexSBase;
	/** Индексная база шаблонов третьего типа. */
	IndexBaseRO vIndexTBase;
	/** Индексная база словарей. */
	IndexBaseRO vIndexDBase;
	/** База синонимов. */
	SynonymBaseRO vSynonymBase;
};

#endif  /* __IndexBase_hpp__ */
