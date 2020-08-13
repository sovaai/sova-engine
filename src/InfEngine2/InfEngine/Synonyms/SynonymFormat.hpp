#ifndef __SynonymFormat_hpp__
#define __SynonymFormat_hpp__


#include <InfEngine2/InfEngine/Format.hpp>

// Синоним.
HEADER3(
        Synonym,
        ITEM1,  Size,   uint32_t*, sizeof(uint32_t),   // Размер скомпилированного синонима в байтах.
        ITEM2,  Length, uint32_t*, sizeof(uint32_t),   // Длина текста синонима в байтах (без '\0').
        SHIFT3, Text,   char*,     sizeof(uint32_t)    // Текст синонима (заканчинвается на '\0').
        )

// Группа синонимов.
HEADER4(
        SynonymGroup,
        ITEM1,  Size,  uint32_t*, sizeof(uint32_t),   // Размер скомпилированной группы синонимов в байтах.
        ITEM2,  Count, uint32_t*, sizeof(uint32_t),   // Количество неглавных синонимов в группе.
        SHIFT3, Major, char*,     sizeof(uint32_t),   // Главный синоним.
        TABLE4, Minor, char*,     sizeof(uint32_t)    // Список неглавных синонимов.
        )

// Словарь синонимов.
HEADER5(
        SynonymDict,
        ITEM1,  Size,       uint32_t*, sizeof(uint32_t),   // Размер скомпилированного словаря синонимов в байтах.
        ITEM2,  NameLength, uint32_t*, sizeof(uint32_t),   // Длина имени словаря в байтах, не считая '\0'.
        SHIFT3, Name,       char*,     sizeof(uint32_t),   // Имя словаря (заканчинвается на '\0').
        ITEM4,  Count,      uint32_t*, sizeof(uint32_t),   // Количество групп синонимов в словаре.
        TABLE5, Group,      char*,     sizeof(uint32_t)    // Список групп синонимов.
        )

#endif // __SynonymFormat_hpp__
