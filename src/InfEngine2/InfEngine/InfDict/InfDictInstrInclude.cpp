#include "InfDictInstrInclude.hpp"

#include <NanoLib/TextFuncs.hpp>

unsigned int InfDictInstrInclude::GetNeedMemorySize() const
{
    // Размер типа инструкции.
    unsigned int memory = sizeof(uint8_t);
    
    // Размер идентификатора словаря.
    memory += sizeof(uint32_t);
    
    return memory;
}

InfEngineErrors InfDictInstrInclude::Save( char* aBuffer, unsigned int aBufferSize, unsigned int& aResultMemory ) const
{
    // Проверка аргументов.
    if( !aBuffer )
        return INF_ENGINE_ERROR_INV_ARGS;
    
    // Проверка достаточности размера буфера.
    unsigned int memory = GetNeedMemorySize();
    if( aBufferSize < memory )
        return INF_ENGINE_ERROR_INV_DATA;
    
    // Сохранение типа инструкции.
    *reinterpret_cast<uint8_t*>( aBuffer ) = vType;
    
    // Сохранение идентификатора включаемого словаря.
    *reinterpret_cast<uint32_t*>( aBuffer + 1 ) = vDictID;
    
    aResultMemory = memory;
    
    return INF_ENGINE_SUCCESS;
}
