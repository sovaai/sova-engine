#include "Signature.hpp"
#include "SignatureFormat.hpp"

using namespace SignatureFormat;

Signature::Signature( unsigned int aVersion )
		: vSignatureInfo( aVersion )
{
    // Добавление в список блока с информацией о самой сигнатуре.
    AddBlock( &vSignatureInfo );
}

unsigned int Signature::GetNeedMemorySize() const
{
    // Размер заголовка.
    unsigned int memory = SignatureHeader::HeaderSize;
    
    // Размер таблицы со сдвигами к блокам сигнатуры.
    memory += SignatureHeader::SizeOfTable_Blocks( vBlocks.size() );
    
    // Размер блоков.
    for( unsigned int block_n = 0; block_n < vBlocks.size(); ++block_n )
        memory += vBlocks[block_n]->GetNeedMemorySize();
    
    return memory;
}

InfEngineErrors Signature::Save( char * aBuffer, unsigned int aSize, unsigned int & aUsed ) const
{
    InfEngineErrors iee = INF_ENGINE_SUCCESS;
    aUsed = 0;
    
    // Проверка размера буфера.
    unsigned int memory = GetNeedMemorySize();
    if( memory > aSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_LOW_SAVE_BUFFER_SIZE );
    
    // Создание заголовка.
    SignatureHeader header( aBuffer );
    // Сдвиг к концу заголовка.
    aUsed = SignatureHeader::HeaderSize;
    
    // Сохранение размера сигнатуры.
    *header.Size = memory;
    
    // Сохранение размера индекса блоков сигнатуры.
    *header.IndexSize = SignatureHeader::HeaderSize + SignatureHeader::SizeOfTable_Blocks( vBlocks.size() );    
    
    // Сохранение размера блоков сигнатуры.
    *header.BlocksSize = memory - *header.IndexSize;
    
    // Инициализация таблицы блоков сигнатуры.
    header.InitTable_Blocks( vBlocks.size(), aUsed );
    // Сдвиг к концу таблицы блоков сигнатуры.
    aUsed += header.SizeOfTable_Blocks();
    
    // Сохранение блоков сигнатуры.
    for( unsigned int block_n = 0; block_n < vBlocks.size(); ++block_n )
    {
        // Сохранение смещения к очередному блоку.
        header.AssignTableRow_Blocks( block_n, aUsed );
        
        // Сохранение очередного блока.
        unsigned int used = 0;
        if( INF_ENGINE_SUCCESS != ( iee = vBlocks[block_n]->Save( aBuffer + aUsed, aSize - aUsed, used ) ) )
			ReturnWithTrace( iee );
        
        // Сдвиг к концу очередного блока.
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

InfEngineErrors Signature::Save( fstorage* aFStorage, fstorage_section_id aSectionID ) const
{
    // Проверка аргументов.
    if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

    // Получение указателя на секцию.
    fstorage_section * sect_data = fstorage_get_section( aFStorage, aSectionID );
    if( !sect_data )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

    // Выделение памяти.
    unsigned int memory = GetNeedMemorySize( );
    int ret = fstorage_section_realloc( sect_data, memory );
    if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

    // Получение указателя на память.
    char * buffer = static_cast<char*>( fstorage_section_get_all_data( sect_data ) );
    if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

    // Сохранение сигнатуры.
    unsigned int used = 0;
    InfEngineErrors iee = Save( buffer, memory, used );
    if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

    return INF_ENGINE_SUCCESS;
}


InfEngineErrors Signature::AddBlock( const Block * aBlock)
{
    // Добавление в сигнатуру очередного блока с ифнормацией.
    vBlocks.push_back( aBlock );
    if( vBlocks.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
        
    return INF_ENGINE_SUCCESS;
}


unsigned int Signature::BSignatureInfo::GetNeedMemorySize() const
{
    return BSignatureInfoHeader::HeaderSize;
}

InfEngineErrors Signature::BSignatureInfo::Save( char * aBuffer, unsigned int aSize, unsigned int & aUsed ) const
{
    // Проверка размера буфера.
    unsigned int memory = GetNeedMemorySize();
    if( memory > aSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_LOW_SAVE_BUFFER_SIZE );
    
    // Создание заголовка.
    BSignatureInfoHeader header( aBuffer );
    
    // Сохранение размера блока.
    *header.Size = memory;
    
    // Сохранение версии сигнатуры.
    *header.Version = vVersion;
    
    aUsed = memory;
    
    return INF_ENGINE_SUCCESS;
}
