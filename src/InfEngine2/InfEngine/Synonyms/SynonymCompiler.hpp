#ifndef __SynonymCompiler_hpp__
#define __SynonymCompiler_hpp__

#include <NanoLib/LogSystem.hpp>
#include <NanoLib/nMemoryAllocator.hpp>
#include "InfEngine2/_Include/Errors.h"
#include "SynonymParser.hpp"
#include <NanoLib/aTextString.hpp>

class SynonymCompiler {
public:
    /**
     *  Возвращает количество памяти в байтах необходимое для сохранения скомпилированных групп синонимов.
     * @param aName - имя словаря.
     * @param aLength - длина имени словаря (не считая '\0').
     * @param aSynonyms - группы синонимов.
     */
    unsigned int GetNeedMemorySize(const char * aName, unsigned int aLength,
            const avector<SynonymParser::SynonymGroup> & aSynonyms) const;

    /**
     *  Компилирует и записывает в заданный буфер группы синонимов.
     * @param aName - имя словаря.
     * @param aLength - длина имени словаря (не считая '\0').
     * @param aSynonyms - группы синонимов.
     * @param aBuffer - буфер для записи.
     * @param aBufferSize - размер буфера.
     * @param aUsedSize - фактически использованный размер буфера.
     */
    InfEngineErrors Compile(const char * aName, unsigned int aLength,
            const avector<SynonymParser::SynonymGroup> & aSynonyms, char * aBuffer, unsigned int aBufferSize,
            unsigned int & aUsedSize);

private:
    /**
     *  Возвращает количество памяти в байтах необходимое для сохранения скомпилированной группы синонимов.
     * @param aSynonymGroup - группа синонимов.
     */
    unsigned int GetSynonymGroupNeedMemorySize(const SynonymParser::SynonymGroup & aSynonymGroup) const;

    /**
     *  Возвращает количество памяти в байтах необходимое для сохранения скомпилированного синонима.
     * @param aSynonym - синоним.
     */
    unsigned int GetSynonymNeedMemorySize(const SynonymParser::Synonym & aSynonym) const;

    /**
     *  Компилирует и записывает в заданный буфер группу синонимов.
     * @param aSynonyms - группа синонимов.
     * @param aBuffer - буфер для записи.
     * @param aBufferSize - размер буфера.
     * @param aUsedSize - фактически использованный размер буфера.
     */
    InfEngineErrors CompileSynonymGroup(const SynonymParser::SynonymGroup & aSynonymsGroup, char * aBuffer,
            unsigned int aBufferSize, unsigned int & aUsedSize);

    /**
     *  Компилирует и записывает в заданный буфер синоним.
     * @param aSynonyms - синоним.
     * @param aBuffer - буфер для записи.
     * @param aBufferSize - размер буфера.
     * @param aUsedSize - фактически использованный размер буфера.
     */
    InfEngineErrors CompileSynonym(const SynonymParser::Synonym & aSynonym, char * aBuffer,
            unsigned int aBufferSize, unsigned int & aUsedSize);
};

#endif // __SynonymCompiler_hpp__
