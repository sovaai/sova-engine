#include "SynonymCompiler.hpp"
#include "SynonymFormat.hpp"

unsigned int SynonymCompiler::GetNeedMemorySize(const char * aName, unsigned int aLength,
        const avector<SynonymParser::SynonymGroup> & aSynonyms) const
{
    // Размер заголовка скомпилированного словаря синонимов.
    unsigned int need_memory = SynonymDictHeader::HeaderSize;

    // Размер имени словаря.
    need_memory += aLength + 1;

    // Размер таблицы сдвигов к скомпилированным группам синонимов.
    need_memory += aSynonyms.size() * sizeof ( uint32_t);

    // Размеры групп синонимов.
    for (unsigned int group_n = 0; group_n < aSynonyms.size(); ++group_n)
        need_memory += GetSynonymGroupNeedMemorySize(aSynonyms[group_n]);

    return need_memory;
}

unsigned int SynonymCompiler::GetSynonymGroupNeedMemorySize(const SynonymParser::SynonymGroup & aSynonymGroup) const
{
    // Размер заголовка скомпилированной группы синонимов.
    unsigned int need_memory = SynonymGroupHeader::HeaderSize;

    // Размер гоавного синонима.
    need_memory += GetSynonymNeedMemorySize(aSynonymGroup.Major);

    // Размер табилцы сдвигов к неглавным синонимам.
    need_memory += aSynonymGroup.Minors.size() * sizeof (unsigned int);

    // размеры еглавных синонимов.
    for (unsigned int minor_n = 0; minor_n < aSynonymGroup.Minors.size(); ++minor_n)
        need_memory += GetSynonymNeedMemorySize(aSynonymGroup.Minors[minor_n]);

    return need_memory;
}

unsigned int SynonymCompiler::GetSynonymNeedMemorySize(const SynonymParser::Synonym & aSynonym) const
{
    // Размер заголовка скомпилированного синонима.
    unsigned int need_memory = SynonymHeader::HeaderSize;

    // Размер текста синонимя + '\0'.
    need_memory += aSynonym.Length + 1;

    return need_memory;
}

InfEngineErrors SynonymCompiler::Compile(const char * aName, unsigned int aLength,
        const avector<SynonymParser::SynonymGroup> & aSynonyms, char * aBuffer,
        unsigned int aBufferSize, unsigned int & aUsedSize)
{
    InfEngineErrors iee = INF_ENGINE_SUCCESS;
    aUsedSize = 0;

    // Вычисляем необходимое кол-во памяти.
    unsigned int need_memory = GetNeedMemorySize(aName, aLength, aSynonyms);

    // Проверяем достаточность размера буфера.
    if (aBufferSize < need_memory)
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA);

    // Заголовок скомпилированного словаря синонимов.
    SynonymDictHeader header(aBuffer);
    // Перескакиваем заголовок.
    aUsedSize += SynonymDictHeader::HeaderSize;

    // Сохраняем размер словаря в байтах.
    *header.Size = need_memory;
    // Сохраняем имя словаря
    *header.NameLength = aLength;
    *reinterpret_cast<uint32_t*> (*header.Name) = aUsedSize;
    memcpy(aBuffer + aUsedSize, aName, aLength + 1);
    aUsedSize += aLength + 1;
    // Сохраняем кол-во групп синонимов в словаре.
    *header.Count = aSynonyms.size();

    *reinterpret_cast<uint32_t*> (header.Group) = aUsedSize;
    uint32_t * shift = reinterpret_cast<uint32_t*> (aBuffer + aUsedSize);
    // Перескакиваем через таблицу смещений к группам синонимов.
    aUsedSize += aSynonyms.size() * sizeof (uint32_t);
    for (unsigned int group_n = 0; group_n < aSynonyms.size(); ++group_n)
    {
        // Сохраняем в таблицу смещений сдвиг к очередной группе синонимов.
        *shift = aUsedSize;
        ++shift;

        // Сохраняем очередную группу синонимов.
        if (INF_ENGINE_SUCCESS != (iee = CompileSynonymGroup(aSynonyms[group_n], aBuffer, aBufferSize, aUsedSize)))
			ReturnWithTrace( iee);
    }

    if (aUsedSize != need_memory)
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Size check failed: %ui instead %ui", aUsedSize, need_memory);

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors SynonymCompiler::CompileSynonymGroup(const SynonymParser::SynonymGroup & aSynonymsGroup, char * aBuffer,
        unsigned int aBufferSize, unsigned int & aUsedSize)
{
    InfEngineErrors iee = INF_ENGINE_SUCCESS;
    unsigned int base = aUsedSize;

    // Вычисляем необходимое кол-во памяти.
    unsigned int need_memory = GetSynonymGroupNeedMemorySize(aSynonymsGroup);
    // Проверяем достаточность размера буфера.
    if (aBufferSize - aUsedSize < need_memory)
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA);

    // Заголовок скомпилированной группы синонимов.
    SynonymGroupHeader header(aBuffer + base);
    // Перескакиваем заголовок.
    aUsedSize += SynonymGroupHeader::HeaderSize;
    // Сохраняем размер скомпилированной группы синонимов в байтах.
    *header.Size = need_memory;
    // Количество неглавных синонимов в группе.
    *header.Count = aSynonymsGroup.Minors.size();

    // Сохраняем главный синоним.
    *reinterpret_cast<uint32_t*> (header.Major) = aUsedSize - base;
    if (INF_ENGINE_SUCCESS != (iee = CompileSynonym(aSynonymsGroup.Major, aBuffer, aBufferSize, aUsedSize)))
		ReturnWithTrace( iee);

    *reinterpret_cast<uint32_t*> (header.Minor) = aUsedSize - base;
    uint32_t * shift = reinterpret_cast<uint32_t*> (aBuffer + aUsedSize);
    // Перепрыгиваем таблицу синонимов.
    aUsedSize += aSynonymsGroup.Minors.size() * sizeof (uint32_t);
    for (unsigned int minor_n = 0; minor_n < aSynonymsGroup.Minors.size(); ++minor_n)
    {
        // Сохраняем сдвиг к очередному неглавному синониму.
        *shift = aUsedSize;
        ++shift;

        // Сохраняем очередной неглавный синоним.
        if (INF_ENGINE_SUCCESS != (iee = CompileSynonym(aSynonymsGroup.Minors[minor_n], aBuffer, aBufferSize, aUsedSize)))
			ReturnWithTrace( iee);
    }

    if (aUsedSize - base != need_memory)
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Size check failed: %ui instead %ui", aUsedSize, need_memory);

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors SynonymCompiler::CompileSynonym(const SynonymParser::Synonym & aSynonym, char * aBuffer,
        unsigned int aBufferSize, unsigned int & aUsedSize)
{
    unsigned int base = aUsedSize;

    // Вычисляем необходимое кол-во памяти.
    unsigned int need_memory = GetSynonymNeedMemorySize(aSynonym);
    // Проверяем достаточность размера буфера.
    if (aBufferSize - aUsedSize < need_memory)
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA);

    // Заголовок скомпилированного синонима.
    SynonymHeader header(aBuffer + base);
    // Перескакиваем заголовок.
    aUsedSize += SynonymHeader::HeaderSize;
    // Созхраняем размер скомпилированного синонима в байтах.
    *header.Size = need_memory;
    // Сохраняем длину текста синонима.
    *header.Length = aSynonym.Length;
    // Сохраняем текст синонима.
    *reinterpret_cast<uint32_t*> (*header.Text) = aUsedSize - base;
    memcpy(aBuffer + aUsedSize, aSynonym.Text, aSynonym.Length + 1);
    aUsedSize += aSynonym.Length + 1;

    if (aUsedSize - base != need_memory)
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Size check failed: %ui instead %ui", aUsedSize, need_memory);

    return INF_ENGINE_SUCCESS;
}
