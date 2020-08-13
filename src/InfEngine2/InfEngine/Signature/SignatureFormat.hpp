#ifndef __SignatureFormat_hpp__
#define __SignatureFormat_hpp__

#include "Format.hpp"

using namespace Format;

namespace SignatureFormat
{

// "Шапка" сигнатуры. Генерация классов.
class SignatureHeader;
class SignatureManipulator;
HEADER5(
        Signature,
        ITEM1,          Size,           uint32_t*,  sizeof(uint32_t),    // Размер сигнатуры в байтах.
        ITEM2,          IndexSize,      uint32_t*,  sizeof(uint32_t),    // Размер индекса блоков сигнатуры в байта.    
        ITEM3,          BlocksSize,     uint32_t*,  sizeof(uint32_t),    // Размер блоков сигнатуры в байта.
        ITEM4,          BlocksNumber,   uint32_t*,  sizeof(uint32_t),    // Количество блоков в сигнатуре.
        TABLE5,         Blocks,         uint32_t*,  sizeof(uint32_t)     // Список смещений к блокам сигнатуры в байтах относительно начала сигнатуры.
        )
        
        
// Блок "информмация о сигнатуре". Генерация классов.
class BSignatureInfoHeader;
class BSignatureInfoManipulator;
HEADER2(
        BSignatureInfo,
        ITEM1,          Size,           uint32_t*,  sizeof(uint32_t),    // Размер блока в байтах.
        ITEM2,          Version,        uint32_t*,  sizeof(uint32_t)     // Версия сигнатуры.
        )

        
// Блок "информация о сборке InfEngine". Генерация классов.
class BInfEngineInfoHeader;
class BInfEngineInfoManipulator;
HEADER6(
        BInfEngineInfo,
        ITEM1,          Size,                   uint32_t*,      sizeof(uint32_t),    // Размер блока в байтах.
        ITEM2,          InfDataProtoVer,        uint32_t*,      sizeof(uint32_t),    // Версия формата базы шаблонов.
        ITEM3,          MinInfDataProtoVer,     uint32_t*,      sizeof(uint32_t),    // Минимальная совместимая версия формата базы шаблонов.
        ITEM4,          LingProcVer,            uint32_t*,      sizeof(uint32_t),    // Версия LingProc.
        ITEM5,          TagLength,              uint32_t*,      sizeof(uint32_t),    // Длина в байтах строки с названием соответствующего тэга в git.
        SHIFT6,         Tag,                    uint32_t*,      sizeof(uint32_t)     // Смещение в байтах относительно начала блока к строке с названием соответствующего тэга в git.
        )
  
        
// Блок "информация о функциях". Генерация классов.
class BFunctionsHeader;
class BFunctionsManipulator;
HEADER5(
        BFunctions,
        ITEM1,          Size,                   uint32_t*,      sizeof(uint32_t),    // Размер блока в байтах.
        ITEM2,          DLFuncInterfaceVer,     uint32_t*,      sizeof(uint32_t),    // Версия интерфейса DL-функций.
        ITEM3,          MinDLFuncInterfaceVer,  uint32_t*,      sizeof(uint32_t),    // Минимальная совместимая версия интерфейса DL-функций.
        ITEM4,          FunctionsNumber,        uint32_t*,      sizeof(uint32_t),    // Количество внутренних функций.        
        TABLE5,         Functions,              uint32_t*,      sizeof(uint32_t)     // Ссписок смещений к описанию внутренних функций в байтах относительно начала блока.        
        )
        
        
// Структура данных, содержащая описание одной функции. Генерация классов.
class FunctionHeader;
class FunctionManipulator;
HEADER5(
        Function,
        ITEM1,          Size,           uint32_t*,      sizeof(uint32_t),      // Размер структры в байтах.
        ITEM2,          Version,        uint32_t*,      3*sizeof(uint32_t),    // Версия функции.
        ITEM3,          MinVersion,     uint32_t*,      3*sizeof(uint32_t),    // Минимальная совместимая версия функции.
        ITEM4,          NameLength,     uint32_t*,      sizeof(uint32_t),      // Длина имени функции в байтах.
        SHIFT5,         Name,           char*,          sizeof(uint32_t)       // Cмещение к имени функции относительно начала описания функции.
        )
        
        
// EXAMPLE
HEADER5(
        Example,
        ITEM1,          AtomicValue,    uint16_t*,  sizeof(uint16_t),    // Некоторое атомарное значение, объём памяти для хранения которого заведомо известен.
        ITEM2,          NameLength,     uint32_t*,  sizeof(uint32_t),    // Длина строки в байтах, включая завершающий ноль.
        SHIFT3,         Name,           uint32_t*,  sizeof(uint32_t),    // Некотороая строка.
        ITEM4,          BlocksNumber,   uint32_t*,  sizeof(uint32_t),    // Количество элементов в списке.
        TABLE5,         Blocks,         uint32_t*,  sizeof(uint32_t)     // Список смещений к элементам списка.
        )        
}
#endif // __SignatureFormat_hpp__
