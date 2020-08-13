#include "Function.hpp"
#include "SignatureFormat.hpp"
#include <cstring>

using namespace SignatureFormat;

unsigned int Function::GetNeedMemorySize() const
{
    // Размер заголовка описания функции.
    unsigned int memory = FunctionHeader::HeaderSize;
    
    // Размер имени функции.
    memory += strlen( vFucntion->name ) + 1;
    
    return memory;
}

InfEngineErrors Function::Save( char * aBuffer, unsigned int aSize, unsigned int & aUsed ) const
{
    // Проверка размера буфера.
    unsigned int memory = GetNeedMemorySize();
    if( memory > aSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_LOW_SAVE_BUFFER_SIZE );
    
    // Создание заголовка.
    FunctionHeader header( aBuffer );
    // Сдвиг к концу заголовка.
    aUsed = FunctionHeader::HeaderSize;
    
    // Сохранение размера блока.
    *header.Size = memory;
    
    // Сохранение версии функции.
    header.Version[0] = vFucntion->version.a;
    header.Version[1] = vFucntion->version.b;
    header.Version[2] = vFucntion->version.c;
    
    // Сохранение минимальной совместимой версии функции.
    header.MinVersion[0] = vFucntion->min_version.a;
    header.MinVersion[1] = vFucntion->min_version.b;
    header.MinVersion[2] = vFucntion->min_version.c;
    
    // Сохранение длины имени функции.
    *header.NameLength = strlen( vFucntion->name );
    
    // Сохранение смещения в байтах относительно начала структуры к имени функции.
    *header.Name = aUsed;
    
    // Сохранение имени функции.
    memcpy( aBuffer + aUsed, vFucntion->name, *header.NameLength );
    // Сдвиг к кнцу имени функции.
    aUsed += *header.NameLength;    
    // Сохранение завершающего нуля.
    aBuffer[aUsed++] = '\0';
    
    // Проверка контрольной суммы.
    if( aUsed != memory )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't save Signature (%u bytes instead %u)", aUsed, memory );
    
    return INF_ENGINE_SUCCESS;
}
