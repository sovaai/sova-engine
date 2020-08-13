#ifndef __SynonymIndex_hpp__
#define __SynonymIndex_hpp__

#include <LingProc4/LingProcPresets.h>
#include <Filtration3/terms/FltTermsBase.h>
#include "SynonymParser.hpp"
#include "SynonymMatcher.hpp"
#include <NanoLib/LogSystem.hpp>
#include <Filtration3/common/DocMessage.h>

/**
 *  Индексная база для индексирования неглавных синонимов.
 */

class SynonymIndexWR {
public:
    /**
     *  Конструктор индексной базы.
     * @param aLp - лингвистический процессор.
	 * @param aSectionId - идентификатор начала пакета секций fstorage, соответствующих базе. Всего используется
     *                    TERMS_SECTION_MAX_NUMBER+1 секций.
     */
	SynonymIndexWR( InfLingProc *& aLp, fstorage_section_id aSectionId );

	/** Деструктор индексной базы. */
	~SynonymIndexWR()
		{ Destroy(); }


public:
    /** Создание и инициализация индексной базы. */
    InfEngineErrors Create();
    /** Уничтожение индексной базы. */
    void Destroy();

    /**
     *  Сохранение индексной базы в fstorage.
     * @param aFStorage - ссылка на открытый fstorage.
     */
    InfEngineErrors Save(fstorage* aFStorage);
    
    /**
     *  Очистка объекта.
     */
    void Reset();

    /**
     *  Добавить синоним в индекс.
     * @param aSynonym - неглавный синоним.     
     * @param aMainID - идентификатор главного синонима.
     * @param aGroupID - идентификатор группы синонимов.
     * @param aDictId - идентификатор словаря синонимов.
     */
    InfEngineErrors AddSynonym(const SynonymParser::Synonym * aSynonym, unsigned int aMainID,
            unsigned short aGroupID, unsigned short aDictId);
    
private:
    // Служебная переменная.
    cf_term_attrs vAttrs;
    // Служебная переменная.
    SynonymAttrsWR vSynonymAttrs;
    // Служебная переменная.
    CDocMessage vDocMessage;
    // Служебная переменная.
    DocImage vDocImage;
    // Служебная переменная.
    aTextString vWord;

protected:
    /** Лингвистический процессор. */
    InfLingProc*& vLp;

    /** Индекс синонимов. */
    FltTermsBase vTBase;
};

/**
 *  Индексная база для работы с проиндексированными неглавными синонимами.
 */
class SynonymIndexRO {
public:
    /**
     *  Конструктор индексной базы.
     * @param aLp - лингвистический процессор.
     * @param aSectionID - идентификатор начала пакета секций fstorage, соответствующих базе. Всего используется
     *                    TERMS_SECTION_MAX_NUMBER+1 секций.
     */
	SynonymIndexRO( InfLingProc *& aLp, fstorage_section_id aSectionId );

    /** Деструктор индексной базы. */
	~SynonymIndexRO()
		{ Close(); }


public:
    /**
     *  Открытие индексной базы.
     * @param aFStorage - fstorage, содержащий индексную базу.
     */
    InfEngineErrors Open(fstorage* aFStorage);
    
    /** Закрытие индексной базы. */
    void Close();
    
public:

    bool IsReady() {
        return vState == stReady;
    }


public:
    
    /**
     *  Поиск синонимов в заданном тексте.
     * @param aRequest - размеченный текст.
     * @param aMatcher - объект для получения результатов поиска.
     */
    InfEngineErrors Check(const DocImage& aRequest, FltTermsMatcher& aMatcher);

    /**
     *  Лемматизация текста.
     * @param aDocText - текст.
     * @param aDocImage - размеченный текст.
     */
    InfEngineErrors LemmatizeDoc(DocText& aDocText, DocImage& aDocImage);

protected:

    /** Состояние индексной базы. */
    enum {
        /** База не готова. */
        stNone = 0,
        /** База готова к поиску. */
        stReady = 1
    } vState;

    /** Лингвистический процессор. */
    InfLingProc*& vLp;

    /** База терминов. */
    FltTermsBase vTBase;
};


#endif // __SynonymIndex_hpp__
