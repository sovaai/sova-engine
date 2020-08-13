#include "PatternsStorage.hpp"

#include <NanoLib/LogSystem.hpp>

class PatternRW : public InfPatternItems::Array
{
public:
	/**
	 *  Конструктор копирования.
	 * @param aPatternString - шаблон-строка.
	 */
	PatternRW( const Array & aPatternString ) : Array( aPatternString ) { vOriginal = nullptr; }

	/**
	 *  Конструктор.
	 * @param aItems - массив DL элементов.
	 * @param aItemsNum - число DL элементов в массиве.
	 * @param aOriginal - оригинальная шалон-строка.
	 */
	PatternRW( InfPatternItems::Base ** aItems = nullptr, unsigned int aItemsNum = 0, const char * aOriginal = nullptr )
	{
		Set( aItems, aItemsNum, aOriginal );
	}

public:
	/**
	 *  Установка данных.
	 * @param aItems - массив DL элементов.
	 * @param aItemsNum - количество DL элементов в массиве.
	 */
	void Set( InfPatternItems::Base ** aItems, unsigned int aItemsNum ) { Set( aItems, aItemsNum, nullptr ); }

	/**
	 *  Установка данных.
	 * @param aArray - массив DL элементов.
	 */
	void Set( const Array & aArray ) { Set( aArray.GetItems(), aArray.GetItemsNumber(), nullptr ); }

	/**
	 *  Установка данных.
	 * @param aItems - массив DL элементов.
	 * @param aItemsNum - количество DL элементов в массиве.
	 * @param aOriginal - оригинальная шаблон-строка.
	 */
	void Set( InfPatternItems::Base ** aItems, unsigned int aItemsNum, const char * aOriginal )
	{
		Array::Set( aItems, aItemsNum );
		vOriginal = aOriginal;
	}

public:
	/** Возвращает размер памяти, необходимой для сохранения шаблона. */
	unsigned int GetNeedMemorySize() const { return sizeof( uint32_t ) + Array::GetNeedMemorySize(); }

	unsigned int Save( void * aBuffer ) const
	{
		*static_cast<uint32_t*>( aBuffer ) = GetNeedMemorySize();
		return Array::Save( static_cast<char*>( aBuffer ) + sizeof( uint32_t ) ) + sizeof( uint32_t );
	}

private:
	/** Текст оригинальной шаблон-строки. */
	const char * vOriginal { nullptr };
};

InfEngineErrors PatternsStorage::Save( fstorage & aFStorage ) const
{
	// Выделение памяти в fstorage.
	fstorage_section * sectdata = fstorage_get_section( &aFStorage, FSTORAGE_SECTION_DLDATA_PATTERNS_STORAGE );
	if( !sectdata )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	unsigned int memory = 0;
	if( vBuffer )
		memory = *static_cast<const uint32_t*>( vBuffer );
	else
	{
		// Размер хранилища в байтах.
		memory += sizeof( uint32_t );

		// Количество шаблонов в хранилище.
		memory += sizeof( uint32_t );

		// Таблица сдвигов к шаблонам.
		memory += sizeof( uint32_t ) * vPatterns.size();

		// Шаблоны с выравниванием.
		for( auto pattern : vPatterns )
		{
			binary_data_alignment( memory );
			memory += pattern->GetNeedMemorySize();
		}
	}

	if( fstorage_section_realloc( sectdata, memory ) != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't allocate memory in fstorage." );

	void * buffer = fstorage_section_get_all_data( sectdata );
	if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, "Can't get fstorage pointer." );

	if( vBuffer )
		memcpy( buffer, vBuffer, memory );
	else
	{
		char * ptr = static_cast<char*>( buffer );

		// Размер хранилища в байтах.
		binary_data_save<uint32_t>( ptr, memory );

		// Количество шаблонов в хранилище.
		binary_data_save<uint32_t>( ptr, vPatterns.size() );

		// Таблица смещений к шаблонам.
		auto shift = binary_data_skip<uint32_t>( ptr, vPatterns.size() );

		// Сохранение шаблонов.
		for( auto pattern : vPatterns )
		{
			*shift++ = binary_data_alignment( ptr, (char*)buffer );
			binary_data_save_object( ptr, *pattern );
		}

	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors PatternsStorage::Open( fstorage & aFStorage )
{
	Reset();
	// Загрузка данных.
	fstorage_section * section = fstorage_get_section( &aFStorage, FSTORAGE_SECTION_DLDATA_PATTERNS_STORAGE );
	if( !section )
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't get fstorage section pointer." );

	const void * buffer = static_cast<const void*>( fstorage_section_get_all_data( section ) );
	if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't get fstorage section pointer." );

	vBuffer = buffer;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors PatternsStorage::AddPatternString( const InfPatternItems::Array & aPatternString, unsigned int & aId )
{
	if( vBuffer )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// @todo: в момент добавления шаблона в хранилище может происходить всяческая уникализация и оптимизация.
	InfEngineErrors iee;

	// Сохранение шаблона.
	PatternRW pattern_wr( aPatternString );
	try { vTmpString.resize( pattern_wr.GetNeedMemorySize() ); } catch( ... ) {
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	pattern_wr.Save( vTmpString.data() );
	Pattern pattern( vTmpString.data() );

	iee = SearchPatternString( pattern, aId );
	if( iee == INF_ENGINE_SUCCESS )
		// Шаблон был уже был добавлен в хранилище.
		return INF_ENGINE_SUCCESS;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		// Вовремя поиска произошла ошибка.
		ReturnWithTrace( iee );

	// Выделение места под новый шаблон.
	char * buffer = nAllocateObjects( vMemoryAllocator, char, vTmpString.size() );
	if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( buffer, vTmpString.data(), vTmpString.size() );

	try
	{
		// Добавление нового шаблона.
		aId = vPatterns.size();
		nAllocateNewObject( vMemoryAllocator, Pattern, new_pattern );
		if( !new_pattern )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new( (nMemoryAllocatorElementPtr*)new_pattern )Pattern( buffer );
		vPatterns.push_back( new_pattern );

		// Добавление шаблона в индекс.
		vIndex.insert( PatternsIndex::value_type( new_pattern, aId ) );
	}
	catch( ... )
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors PatternsStorage::SearchPatternString( const Pattern & aPattern, unsigned int & aId ) const
{
	try
	{
		// Поиск шаблона среди ранее добавленных в хранилище.
		auto iter = vIndex.find( &aPattern );

		// Шаблон не найден.
		if( iter == vIndex.end() )
			return INF_ENGINE_WARN_UNSUCCESS;

		// Шаблон найден.
		aId = iter->second;
		return INF_ENGINE_SUCCESS;
	}
	catch( ... )
	{
		return INF_ENGINE_WARN_UNSUCCESS;
	}
}
