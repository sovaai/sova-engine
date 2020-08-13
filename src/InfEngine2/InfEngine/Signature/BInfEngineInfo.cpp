#include "BInfEngineInfo.hpp"
#include "SignatureFormat.hpp"

using namespace SignatureFormat;

unsigned int BInfEngineInfo::GetNeedMemorySize() const
{
    // Размер заголовка блока.
    unsigned int memory = BInfEngineInfoHeader::HeaderSize;
    
    // Размер названиея тэга.
    memory += vTag.size() + 1;
    
    return memory;
}

InfEngineErrors BInfEngineInfo::Save( char * aBuffer, unsigned int aSize, unsigned int & aUsed ) const
{
    // Проверка размера буфера.
    unsigned int memory = GetNeedMemorySize();
    if( memory > aSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_LOW_SAVE_BUFFER_SIZE );
    
    // Создание заголовка.
    BInfEngineInfoHeader header( aBuffer );
    // Сдвиг к концу заголовка.
    aUsed = BInfEngineInfoHeader::HeaderSize;
    
    // Сохранение размера блока.
    *header.Size = memory;
    
    // Сохранение версии формата базы шаблонов
    *header.InfDataProtoVer = vInfDataProtoVer;
    
    // Сохранение минимальной совместимой версии формата базы шаблонов.
    *header.MinInfDataProtoVer = vMinInfDataProtoVer;
    
    // Сохранение версии LingProc.
    *header.LingProcVer = vLingProcVer;
    
    // Сохранение длины в байтах строки с названием соответствующего тэга в git.
    *header.TagLength = vTag.size();
    
    // Сохранение смещения в байтах относительно начала блока к строке с названием соответствующего тэга в git.
    *header.Tag = aUsed;
    
    // Сохранение строки с названием соответствующего тэга в git.
    memcpy( aBuffer + aUsed, vTag.ToConstChar(), vTag.size() );
    
    // Сдвиг к концу названия тэга. 
    aUsed += vTag.size();
    
    // Сохранение завершающего нуля.
    aBuffer[aUsed++] = '\0';
    
    // Проверка контрольной суммы.
    if( aUsed != memory )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't save Signature (%u bytes instead %u)", aUsed, memory );
    
    return INF_ENGINE_SUCCESS;
}
