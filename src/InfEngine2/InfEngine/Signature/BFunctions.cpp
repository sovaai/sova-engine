#include "BFunctions.hpp"
#include "SignatureFormat.hpp"

using namespace SignatureFormat;

unsigned int BFunctions::GetNeedMemorySize() const
{
    // Размер заголовка блока с информацией о системе функций InfEngine
    unsigned int memory = BFunctionsHeader::HeaderSize;
    
    // Размер таблицы со сдвигами к структурам с описаниями функций.
    memory += BFunctionsHeader::SizeOfTable_Functions( vFunctionsNumber );
    
    // Размер структур с описаниями функций.
    for( unsigned int func_n =0; func_n < vFunctionsNumber; ++func_n )
        memory += vFunctions[func_n]->GetNeedMemorySize();
    
    return memory;
}

InfEngineErrors BFunctions::Save( char * aBuffer, unsigned int aSize, unsigned int & aUsed ) const
{
    InfEngineErrors iee = INF_ENGINE_SUCCESS;
    aUsed = 0;
    
    // Проверка размера буфера.
    unsigned int memory = GetNeedMemorySize();
    if( memory > aSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_LOW_SAVE_BUFFER_SIZE );
    
    // Создание заголовка.
    BFunctionsHeader header( aBuffer );
    // Сдвиг к концу заголовка.
    aUsed = BFunctionsHeader::HeaderSize;
    
    // Сохранение размера блока.
    *header.Size = memory;
    
    // Сохранение версии интерфейса DL-функций.
    *header.DLFuncInterfaceVer = vDLFuncInterfaceVer;
    
    // Сохранение минимальной совместимой версии интерфейса DL-функций.
    *header.MinDLFuncInterfaceVer = vMinDLFuncInterfaceVer;
    
    // Инициализация таблицы смещений к структурам с описаниями функций.
    header.InitTable_Functions( vFunctionsNumber, aUsed );
    // Сдвиг к концу таблицы структур с описаниями функций.
    aUsed += header.SizeOfTable_Functions();
    
    // Сохранение структур с описаниями функций.
    for( unsigned int func_n = 0; func_n < vFunctionsNumber; ++func_n )
    {
        // Сохранение смещения к очередной структуре.
        header.AssignTableRow_Functions( func_n, aUsed );
        
        // Сохранение очередной структуры.
        unsigned int used = 0;
        if( INF_ENGINE_SUCCESS != ( iee = vFunctions[func_n]->Save( aBuffer + aUsed, aSize - aUsed, used ) ) )
			ReturnWithTrace( iee );
        
        // Сдвиг к концу очередной структуры.
        aUsed += used;
        
        // Проверка промежуточной суммы.
        if( aUsed > memory )
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't save Signature (%u bytes instead %u)", aUsed, memory );
    }
    
    // Проверка контрольной суммы.
    if( aUsed != memory )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't save Signature (%u bytes instead %u)", aUsed, memory );
        
    return INF_ENGINE_SUCCESS;
}
