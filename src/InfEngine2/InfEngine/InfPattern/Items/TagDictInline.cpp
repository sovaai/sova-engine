#include "TagDictInline.hpp"

using namespace InfPatternItems;

InfEngineErrors TagDictInline::Set( char ** aStrings, unsigned int * aStringsLens, unsigned int aStringsNum )
{
	// Проверка аргументов.
	if( !aStrings || !aStringsLens )
		return INF_ENGINE_ERROR_INV_ARGS;

	vStrings = aStrings;
	vStringsLens = aStringsLens;
	vStringsNum = aStringsNum;

	// Проверка строк на пустоту.
	vHasEmpty = false;
	for( unsigned int i = 0; i < aStringsNum; i++ )
	{
		if( aStringsLens[i] <= 0 )
			vHasEmpty = true;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors TagDictInline::ApplySynonyms( SymbolymBaseRO & aSymbolymsBase, SynonymBaseRO & aSynonymsBase, 
											  nMemoryAllocator & aAllocator )
{
	// Объекты для хранения верменных данных.
	static avector<const char*> strings;
	static avector<unsigned int> strings_lens;
	strings.clear();
	strings_lens.clear();

	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	const char * normalized_text = nullptr;
	unsigned int normalized_text_len = 0;

	// Заменяем для каждого элемента инлайн словаря слова на главные синонимы.
	for( unsigned int i = 0; i < GetStringsNumber(); ++i )				
	{
		// Производим замену символов.
        const char * tmp_text = nullptr;
		unsigned int tmp_text_length = 0;
		iee = aSymbolymsBase.ApplySymbolyms( GetString( i ), GetStringLength( i ), tmp_text, tmp_text_length );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		
		// Производим замену на синонимы.
		iee = aSynonymsBase.ApplySynonyms( tmp_text, tmp_text_length, normalized_text, normalized_text_len );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		// Сохраняем полученные в результате замены данные во временных объектах.
		strings.push_back( normalized_text );
		if( strings.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		strings_lens.push_back( normalized_text_len );
		if( strings_lens.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	// Выделяем память для сохранения списка строк.
	char ** strings_buf = nAllocateObjects( aAllocator, char*, strings.size() );
	if( !strings_buf )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Выделяем память для сохранения списка длин строк.
	unsigned int * strings_lens_buf = nAllocateObjects( aAllocator, unsigned int, strings_lens.size() );
	if( !strings_lens_buf )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Сохраянем список строк и списх их длин.
	memcpy( strings_buf, strings.get_buffer(), strings.size() * sizeof( char* ) );
	memcpy( strings_lens_buf, strings_lens.get_buffer(), strings_lens.size() * sizeof( unsigned int ) );

	vNormalizedStrings = strings_buf;

	vNormalizedStringsLens = strings_lens_buf;

	vNormalizedStringsNum = strings.size();

	// Проверка строк на пустоту.
	vHasEmpty = false;
	for( unsigned int i = 0; i < strings.size(); i++ )
	{
		if( vNormalizedStringsLens[i] <= 0 )
			vHasEmpty = true;
	}

	return INF_ENGINE_SUCCESS;
}

unsigned int TagDictInline::GetNeedMemorySize() const
{
	/**
	 * - Общее число строк.
	 * - Ссылки на строки.
	 * - Строки со сдвигами.
	 **/
	unsigned int NeedMemorySize = 0;
	NeedMemorySize += 1; // Флаг наличия пустого варианта.
	NeedMemorySize += sizeof( uint32_t ) * ( vStringsNum + 1 );
	NeedMemorySize += sizeof( uint32_t ) * ( vNormalizedStringsNum + 1 );
	for( size_t i = 0; i < vStringsNum; i++ )
	{
		// Сдвиг.
		NeedMemorySize += NeedMemorySize % 2;

		NeedMemorySize += sizeof( uint16_t ) + vStringsLens[i] + 1;
	}

	for( size_t i = 0; i < vNormalizedStringsNum; i++ )
	{
		// Сдвиг.
		NeedMemorySize += NeedMemorySize % 2;

		NeedMemorySize += sizeof( uint16_t ) + vNormalizedStringsLens[i] + 1;
	}

	return NeedMemorySize;
}

unsigned int TagDictInline::Save( void * aBuffer ) const
{
	// Проверка аргументов.
	if( !aBuffer )
		return 0;

	char * ptr = static_cast<char*>( aBuffer );

	// Флаг наличия пустого варианта.
	*ptr = vHasEmpty ? 1 : 0;
	ptr++;

	// Количество строк.
	*( (uint32_t*)ptr ) = vStringsNum;
	ptr += sizeof( uint32_t );
	// Количество нормализованных строк.
	*( (uint32_t*)ptr ) = vNormalizedStringsNum;
	ptr += sizeof( uint32_t );

	// Ссылки на строки.
	uint32_t * shifts = (uint32_t*)ptr;
	ptr += vStringsNum * sizeof( uint32_t );

	// Ссылки на нормализованные строки.
	uint32_t * norm_shifts = (uint32_t*)ptr;
	ptr += vNormalizedStringsNum * sizeof( uint32_t );

	// Сохранение строк.
	for( size_t i = 0; i < vStringsNum; i++ )
	{
		// Сдвиг.
		ptr += ( ptr - static_cast<char*>( aBuffer ) ) % 2;

		// Установка ссылки.
		*shifts = ptr - static_cast<char*>( aBuffer );
		++shifts;

		unsigned int size = vStringsLens[i];
		*( (uint16_t*)ptr ) = size + 1;
		ptr += sizeof( uint16_t );

		if( size )
		{
			memcpy( ptr, vStrings[i], size );
			ptr += size;
		}
		*ptr = '\0';
		++ptr;
	}

	// Сохранение нормализованных строк.
	for( size_t i = 0; i < vNormalizedStringsNum; i++ )
	{
		// Сдвиг.
		ptr += ( ptr - static_cast<char*>( aBuffer ) ) % 2;

		// Установка ссылки.
		*norm_shifts = ptr - static_cast<char*>( aBuffer );
		++norm_shifts;

		unsigned int size = vNormalizedStringsLens[i];
		*( (uint16_t*)ptr ) = size + 1;
		ptr += sizeof( uint16_t );

		if( size )
		{
			memcpy( ptr, vNormalizedStrings[i], size );
			ptr += size;
		}
		*ptr = '\0';
		++ptr;
	}

	return ptr - static_cast<char*>( aBuffer );
}
