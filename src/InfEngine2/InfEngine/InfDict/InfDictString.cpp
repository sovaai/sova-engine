#include "InfDictString.hpp"

unsigned int InfDictString::GetNeedMemorySize() const
{
	unsigned int memory = 0;

	memory += sizeof( uint32_t );   // Общий объем занимаемой шаблоном памяти.
	memory += sizeof( uint32_t );   // Идентификатор, выданный хранилищем шаблонов.
	memory += sizeof( uint16_t );   // Длина оригинальной строки.
	memory += vOriginalLength + 1;  // Оригинальная строка.

	return memory;
}

InfEngineErrors InfDictString::Save( char * aBuffer, uint32_t aBufferSize, uint32_t & aResultMemory ) const
{
	// Проверка аргументов.
	if( !aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка размера буффера.
	uint32_t memory = (uint32_t)GetNeedMemorySize();
	if( aBufferSize < memory )
		ReturnWithError( INF_ENGINE_ERROR_LIMIT, INF_ENGINE_STRING_ERROR_LOW_SAVE_BUFFER_SIZE );

	// Сохранение.
	char * ptr = aBuffer;

	// Сохранение общего размера сохраненного шаблона.
	*( (uint32_t*)ptr ) = memory;
	ptr += sizeof( uint32_t );

	// Сохранение идентификатора, выданного хранилищем шаблонов.
	*( (uint32_t*)ptr ) = vId;
	ptr += sizeof( uint32_t );

	// Сохранение длины оригинальной строки.
	*( (uint16_t*)ptr ) = vOriginalLength + 1;
	ptr += sizeof( uint16_t );

	// Сохранение оригинальной строки.
	memcpy( ptr, vOriginal, vOriginalLength + 1 );
	ptr += vOriginalLength + 1;

	aResultMemory = (uint32_t)( ptr - aBuffer );

	// Проверка.
	if( memory != aResultMemory )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_SAVING_RESULT );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfDictString::ApplySynonyms( SymbolymBaseRO & aSymbolymsBase, SynonymBaseRO & aSynonymsBase, nMemoryAllocator & aAllocator )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	for( unsigned int i = 0; i < vItemsCount; ++i )
	{
		InfPatternItems::ItemType type = vItems[i]->GetType();
		if( InfPatternItems::itText == type )
		{
            iee = static_cast<InfPatternItems::Text*>( vItems[i] )->ApplySynonyms( aSymbolymsBase, aSynonymsBase );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
		}
		else if( InfPatternItems::itDictInline == type )
		{
            iee = static_cast<InfPatternItems::TagDictInline*>( vItems[i] )->ApplySynonyms( aSymbolymsBase, aSynonymsBase, aAllocator );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
		}
	}

	return INF_ENGINE_SUCCESS;
}
