#ifndef __SynonymBase_hpp__
#define __SynonymBase_hpp__

#include <LingProc4/LingProcPresets.h>
#include "SynonymIndex.hpp"
#include <NanoLib/NameIndex.hpp>
#include <NanoLib/LogSystem.hpp>

/** Класс для компиляции синонимов и последуещей работы с ними. */
class SynonymBaseWR
{
  public:
	SynonymBaseWR( InfLingProc *& aLp, fstorage_section_id aSectionId );

    /** Создание базы синонимов в режиме WR. */
    InfEngineErrors Create( );

    /** Получение необходимого размера памяти для сохранения объекта. */
    unsigned int GetNeedMemorySize( ) const;

    /**
     *  Сохранение базы синонимов в fstorage.
     * @param aFStorage - открытый fstorage.
     * @param aTmp - признак того, что нужно выполнить временное сохранение.
     */
    InfEngineErrors Save( fstorage * aFStorage, bool aTmp = false );

    /**
     *  Очистка объекта.
     */
    void Reset( );

    /** Закрытие открытой ранее базы синонимов. */
    void Close( );

    /**
     *  Компилирует и индексирует группу синонимов.
     * @param aSynGroup - группа синонимов (один главный синоним и несколько неглавных).
     * @param aSynGroupN - номер группы синонимов в словаре синонимов.
     * @param aSynDictName - название словаря синонимов.
     * @param aSynDictNameLen - длина названия словаря синонимов, не считая '\0'.
     */
    InfEngineErrors AddSynonymGroup( const SynonymParser::SynonymGroup & aSynGroup, unsigned int aSynGroupN,
                                     const char * aSynDictName, unsigned int aSynDictNameLen );

    /**
     *  Возвращает сообщение о последней ошибке.
     */
    const char * GetErrorMsg( ) const;


  private:
    /** Служебная переменная. Сообщение об ошибке. */
    aTextString vErrorMsg;


  protected:

    /** Индекс главных синонимов. */
    NanoLib::NameIndex vMainIndex;

    /** Индекс названий словарей синонимов. */
    NanoLib::NameIndex vDictIndex;

    /** База для неглавных синонимов. */
    SynonymIndexWR vSynonymIndexWR;

    /** База для неглавных синонимов для временного сохранения. */
    SynonymIndexWR vSynonymIndexWR_tmp;

    /** Идентификатор секции индексов главных синонимов и названий словарей синонимов. */
    fstorage_section_id vExtSectionID;

    /** Менеджер памяти. */
    nMemoryAllocator vMemoryAllocator;
};

/** Класс для работы со скомпилированными синонимами. */
class SynonymBaseRO
{
  public:
    SynonymBaseRO( InfLingProc *& aLp, fstorage_section_id aSectionId );

    /**
     *  Открытие базы синонимов.
     * @param aFStorage - fstorage, содержащий базу синонимов.
     */
    InfEngineErrors Open( fstorage* aFStorage );

    /** Закрытие базы синонимов. */
    void Close( );
    
    /** Освобождение памяти выделенной под замены и карты соответствия. */
    inline void Reset( ) const { vMemoryAllocator.Reset(); }

    /**
     *  Заменяет в заданном тексте все слова на главные синонимы.
     * @param aText - текст.
     * @param aTextLen - длина текста.
     * @param aNormalizedText - текст, полученный из заданного путём замены синонимов на главные.
     * @param aNormalizedTextLen - длина полученного текста.
     */
    InfEngineErrors ApplySynonyms( const char * aText, unsigned int aTextLen,
                                   const char *& aNormalizedText, unsigned int & aNormalizedTextLen );

    /**
     *  Заменяет в заданном тексте все слова на главные синонимы и строит карту соответстсвия между исходным и
     *  сгенерированным текстом.
     * @param aText - текст.
     * @param aTextLen - длина текста.
     * @param aNormalizedText - текст, полученный из заданного путём замены синонимов на главные.
     * @param aNormalizedTextLen - длина полученного текста.
     * @param aMap - индексы символов в исходной строке, соответствующие символам в полученной сроке.
     *               Имеет длину aNormalizedTextLen.
     */
    InfEngineErrors ApplySynonyms( const char * aText, unsigned int aTextLen,
                                   const char *& aNormalizedText, unsigned int & aNormalizedTextLen, 
								   const unsigned int *& aMap );

  protected:
    /**
     *  Заменяет в заданном тексте все слова на главные синонимы и строит карту соответстсвия между исходным и
     *  сгенерированным текстом.
     * @param aText - текст.
     * @param aTextLen - длина текста.
     * @param aNormalizedText - текст, полученный из заданного путём замены синонимов на главные.
     * @param aNormalizedTextLen - длина полученного текста.
     * @param aMap - индексы символов в исходной строке, соответствующие символам в полученной сроке.
     *               Имеет длину aNormalizedTextLen.
     * @param aConstructMap - указывает, нужно ли строить карту соответствия.
     */
    InfEngineErrors	ApplySynonymsAndConstructMap( const char * aText, unsigned int aTextLen,
                                                  const char *& aNormalizedText, unsigned int & aNormalizedTextLen, 
												  const unsigned int *& aMap, bool aConstructMap );

  private:
    // Служебная переменная.
    CDocMessage vDocMessage;
    // Служебная переменная.
    DocImage vDocImage;
    // Служебная переменная.
    aTextString vNormalizedText;
    /** Служебный объект для храрнения результатов поиска. */
    SynonymMatcher vMatcher;
    /** Служебный объект для храрнения карты соответствия. */
    avector<unsigned int> vMap;

  protected:
    /** Лингвистический процессор необходим для работы с терминами. */
    InfLingProc *& vLp;

    /** Индекс главных синонимов. */
    NanoLib::NameIndex vMainIndex;

    /** Индекс названий словарей синонимов. */
    NanoLib::NameIndex vDictIndex;

    /** База для неглавных синонимов. */
    SynonymIndexRO vSynonymIndex;

    /** Идентификатор секции индексов главных синонимов и названий словарей синонимов. */
    fstorage_section_id vExtSectionID;

    /** Менеджер памяти. */
    mutable nMemoryAllocator vMemoryAllocator;
};

#endif // __SynonymBase_hpp__


