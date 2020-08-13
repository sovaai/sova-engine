#ifndef __ListFileParser_hpp__
#define __ListFileParser_hpp__

#include <NanoLib/LogSystem.hpp>
#include <NanoLib/nMemoryAllocator.hpp>
#include "InfEngine2/_Include/Errors.h"
#include <NanoLib/aTextString.hpp>
#include <aptl/avector.h>

/**
 *  Парсер для списка файлов с именованными ссылками.
 */
class ListFileParser {
public:
	/**
     *  Парсинг списка файлов из текстового файла.
     * @param aFilePath - имя текстового файла со списком.
     */
    InfEngineErrors ParseFromFile(const char * aFilePath);

    /**
     *  Парсинг одной строки.
     * @param aString - строка.
     * @param aLength - длина строки в байтах, не считая терминирующий символ.
     */
    InfEngineErrors ParseString(const char * aString, unsigned int aLength);

    /**
     *  Возвращает количество файлов, о которых есть данные.
     */
    inline unsigned int GetFilesCount() {
        return vPathList.size();
    }

    /**
     *  Возвращает адрес файла.
     * @param aFileN - номер файла.
     */
    inline const char * GetPath(unsigned int aFileN) {
        return vPathList[aFileN];
    }

    /**
     *  Возвращает имя ссылки на файла.
     * @param aFileN - номер файла.
     */
    inline const char * GetName(unsigned int aFileN) {
        return vNameList[aFileN];
    }

    /**
     *  Очистка объекта.
     */
    void Reset();

private:
    /** Список считанных адресов файлов. */
    avector<const char *> vPathList;

    /** Список считанных имён файлов. */
    avector<const char *> vNameList;

    /** Аллокатор памяти. */
    nMemoryAllocator vMemoryAllocator;

};

#endif // __ListFileParser_hpp__
