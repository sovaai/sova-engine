#include "SymbolymBase.hpp"
#include <Filtration3/terms/FltTermsBase.h>

bool SymbolymAttrs::Init( nMemoryAllocator & aMemoryAllocator, const SynonymParser::Synonym & aMajor, const SynonymParser::Synonym & aMinor ) {
	// Подсчитываем память для хранения элемента.
	unsigned int memory = sizeof( std::uint16_t ) + aMajor.Length + 1 + sizeof( std::uint16_t ) + aMinor.Length + 1 + sizeof( std::uint16_t );
	
	// Выделяем память для элемента.
	char * buffer = reinterpret_cast<char*>( aMemoryAllocator.Allocate( memory ) );
	if( !buffer )
		return false;
	vBuffer = buffer;

	// Сохраняем элемент.
	
	// Размер всей структуры.
	reinterpret_cast<std::uint16_t*>(buffer)[0] = memory;
	buffer += sizeof( std::uint16_t );
	
	// Длина главного символа.
	reinterpret_cast<std::uint16_t*>(buffer)[0] = aMajor.Length;
	buffer += sizeof( std::uint16_t );
	
	// Длина заменяемого символа.
	reinterpret_cast<std::uint16_t*>(buffer)[0] = aMinor.Length;
	buffer += sizeof( std::uint16_t );
	
	// Главный символ.
	memcpy( buffer, aMajor.Text, aMajor.Length );
	buffer[aMajor.Length] = 0;
	buffer += aMajor.Length + 1;
	
	// Заменяемый символ.
	memcpy( buffer, aMinor.Text, aMinor.Length );
	buffer[aMinor.Length] = 0;
	buffer += aMinor.Length + 1;

	return true;
}

SymbolymBaseWR::SymbolymBaseWR( fstorage_section_id aSectionId ) :
	vExtSectionID( aSectionId + TERMS_SECTION_MAX_NUMBER ) {

}

InfEngineErrors SymbolymBaseWR::Create() {
	// Проверка состояния индексов.
	if( vSymbolsIndex.GetState() != NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "SymbolymBase has been already created or opened." );

	// Создание индекса символов.
	vSymbolsIndex.Create();

	return INF_ENGINE_SUCCESS;
}

void SymbolymBaseWR::Reset() {
	vSymbolsIndex.Reset();
	vSymbols.clear();
	vMemoryAllocator.Reset();
}

unsigned int SymbolymBaseWR::GetNeedMemorySize() const {
	unsigned int memory =	sizeof( std::uint32_t ) +					// Размер объекта
			sizeof( std::uint32_t ) +					// Количество заменяемых символов.
			vSymbols.size() * sizeof( std::uint32_t );	// Таблица смещений к символам.
	
	// Символы.
	for( unsigned int symbol_n = 0; symbol_n < vSymbols.size(); ++symbol_n ) {
		memory += vSymbols[symbol_n].GetSize();
		memory += memory%4 ? 4 - memory%4 : 0;
	}
	
	return memory;
}

InfEngineErrors SymbolymBaseWR::Save( fstorage * aFStorage ) {
	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
	
	// Получение указателя на секцию.
	fstorage_section * SectData = fstorage_get_section( aFStorage, vExtSectionID );
	if( !SectData )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Выделение памяти.
	unsigned int MemorySize = GetNeedMemorySize();
	int ret = fstorage_section_realloc( SectData, MemorySize );
	if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Получение указателя на память.
	char * Buffer = static_cast<char*>( fstorage_section_get_all_data( SectData ) );
	if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );
	
	unsigned int used = 0;
	ReturnWithTraceExt( Save( Buffer, MemorySize, used ), INF_ENGINE_SUCCESS );
}

InfEngineErrors SymbolymBaseWR::Save( char * aBuffer, unsigned int aBufferSize, unsigned int & aUsed ) {
	unsigned int memory = GetNeedMemorySize();
	if( memory < aBufferSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_LOW_SAVE_BUFFER_SIZE );
	
	char * ptr = aBuffer;
	
	// Размер объека в байтах.
	reinterpret_cast<uint32_t*>( ptr )[0] = memory;
	ptr += sizeof( uint32_t );
	
	// Количество заменяемых символов.
	reinterpret_cast<uint32_t*>( ptr )[0] = vSymbols.size();
	ptr += sizeof( uint32_t );
	
	// Таблица смещений.
	uint32_t * shift = reinterpret_cast<uint32_t*>( ptr );
	ptr += vSymbols.size() * sizeof( uint32_t );
	
	// Символы.
	for( unsigned int symbol_n = 0; symbol_n < vSymbols.size(); ++symbol_n ) {
		// Смещение к символу.
		*shift = ptr - aBuffer;
		++shift;
		
		// Символ.
		memcpy( ptr, vSymbols[symbol_n].GetBuffer(), vSymbols[symbol_n].GetSize() );
		ptr += vSymbols[symbol_n].GetSize();
		ptr += (ptr - aBuffer)%4 ? 4 - (ptr - aBuffer)%4 : 0;
	}
	
	aUsed = ptr - aBuffer;
	
	if( aUsed != memory )
		ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_SAVING_RESULT );
	
	return INF_ENGINE_SUCCESS;
}


InfEngineErrors SymbolymBaseWR::AddSymbolymGroup( const SynonymParser::SynonymGroup & aSymGroup, unsigned int aSymGroupN ) {
	bool there_are_duplicates = false;
	
	// Очишаем сообщение об ошибке.
	vErrorMsg.clear();
	
	// Добавляем главный символ в словарь символов.
	unsigned int id = static_cast<unsigned int>( -1 );
	NanoLib::NameIndex::ReturnCode rt = NanoLib::NameIndex::rcSuccess;
	if( NanoLib::NameIndex::rcElementExists == ( rt = vSymbolsIndex.AddName( aSymGroup.Major.Text, id ) ) ) {
		if( nlrcSuccess != vErrorMsg.append( "Symbol \"" ) ||
				nlrcSuccess != vErrorMsg.append( aSymGroup.Major.Text ) ||
				nlrcSuccess != vErrorMsg.append( "\" in group #" ) ||
				nlrcSuccess != vErrorMsg.append( aSymGroupN + 1 ) ||
				nlrcSuccess != vErrorMsg.append( " is duplicated\n\t" ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		there_are_duplicates = true;
	}
	else if( rt != NanoLib::NameIndex::rcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to registrate main symbol \"%s\" : %u", aSymGroup.Major.Text, rt );
	
	// Сохраняем заменяемые символы синонимы.
	for( unsigned int minor_n = 0; minor_n < aSymGroup.Minors.size(); ++minor_n ) {
		if( NanoLib::NameIndex::rcElementExists == ( rt = vSymbolsIndex.AddName( aSymGroup.Minors[minor_n].Text, id ) ) ) {
			// TODO: make another warning message.
			LogWarn( "Symbol \"%s\" is duplicated", aSymGroup.Minors[minor_n].Text );

			if( nlrcSuccess != vErrorMsg.append( "Symbol \"" ) ||
					nlrcSuccess != vErrorMsg.append( aSymGroup.Minors[minor_n].Text ) ||
					nlrcSuccess != vErrorMsg.append( "\" in group #" ) ||
					nlrcSuccess != vErrorMsg.append( aSymGroupN + 1 ) ||
					nlrcSuccess != vErrorMsg.append( " is duplicated\n\t" ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			there_are_duplicates = true;
		}
		if( rt != NanoLib::NameIndex::rcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to registrate symbol \"%s\" : %u", aSymGroup.Minors[minor_n].Text, rt );
		
		SymbolymAttrs * symbol = vSymbols.grow();
		if( vSymbols.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		if( !symbol->Init( vMemoryAllocator, aSymGroup.Major, aSymGroup.Minors[minor_n] ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	return there_are_duplicates ? INF_ENGINE_WARN_SYNONYM_DUPLICATE : INF_ENGINE_SUCCESS;
}

const char * SymbolymBaseWR::GetErrorMsg() const {
	return vErrorMsg.ToConstChar();
}

SymbolymBaseRO::SymbolymBaseRO( fstorage_section_id aSectionId ) :
	vExtSectionID( aSectionId + TERMS_SECTION_MAX_NUMBER ) {
}

InfEngineErrors SymbolymBaseRO::Open( fstorage* aFStorage ) {
	vBuffer = nullptr;
	
	// Проверка агрументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
	
	// Загрузка остальных индексов.
	fstorage_section * sectdata = fstorage_get_section( aFStorage, vExtSectionID );
	if( !sectdata )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Получение указателя на память.
	const char * buffer = static_cast<const char *>( fstorage_section_get_all_data( sectdata ) );
	if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get fstorage pointer." );
	
	vBuffer = buffer;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors SymbolymBaseRO::ApplySymbolyms( const char * aText, unsigned int aTextLen,
												const char *& aNormalizedText, unsigned int & aNormalizedTextLen ) {
	const unsigned int * map = nullptr;
	InfEngineErrors iee = ApplySymbolymsAndConstructMap( aText, aTextLen, aNormalizedText, aNormalizedTextLen, map, false);
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );
	return INF_ENGINE_SUCCESS;
}

InfEngineErrors SymbolymBaseRO::ApplySymbolyms( const char * aText, unsigned int aTextLen,
												const char *& aNormalizedText, unsigned int & aNormalizedTextLen,
												const unsigned int *& aMap ) {
	InfEngineErrors iee = ApplySymbolymsAndConstructMap( aText, aTextLen, aNormalizedText, aNormalizedTextLen, aMap, true );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );
	return INF_ENGINE_SUCCESS;
}

InfEngineErrors SymbolymBaseRO::ApplySymbolymsAndConstructMap(const char * aText, unsigned int aTextLen,
															  const char *& aNormalizedText, unsigned int & aNormalizedTextLen ,
															  const unsigned int *& aMap, bool aConstructMap )
{
	char* normalized_text = nullptr;
	unsigned int* map = nullptr;
	
	// Делаем замены.
	vMap.clear();
	vNormalizedText.clear();
	unsigned int pos = 0;
	while( pos < aTextLen ) {
		bool subst_applied = false;
		for( unsigned int symbol_n = 0; symbol_n < GetSymbolsCount(); ++symbol_n ) {
			const SymbolymAttrs symbol = GetSymbol( symbol_n );
			// Пробуем сопоставить символ, начиная с текущей позиции.
			if( aTextLen - pos >= symbol.GetMinorLength() && !memcmp( aText + pos, symbol.GetMinor(), symbol.GetMinorLength() ) ) {
				// Заменяем символ.
				if( nlrcSuccess != vNormalizedText.append( symbol.GetMajor(), symbol.GetMajorLength() ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				
				vMap.reserve( vMap.size() + symbol.GetMajorLength() );
				vMap.push_back( pos );
				for( unsigned int chr_n = 1; chr_n < symbol.GetMajorLength(); ++chr_n )
					vMap.push_back( pos + symbol.GetMinorLength() );
				
				pos += symbol.GetMinorLength();
				subst_applied = true;
				break;
			}
		}
		if( !subst_applied ) {
			vMap.push_back( pos );
			if( nlrcSuccess != vNormalizedText.append( aText + pos, 1 ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			++pos;
		}
	}
	
	// Сохранение построенной нормализованной строки.
	aNormalizedTextLen = vNormalizedText.size();
	normalized_text = nAllocateObjects( vMemoryAllocator, char, aNormalizedTextLen + 1 );
	if( !normalized_text )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( normalized_text, vNormalizedText.ToConstChar(), aNormalizedTextLen );
	normalized_text[aNormalizedTextLen] = '\0';
	aNormalizedText = normalized_text;
	
	// Сохранение индексов соответствия.
	if( aConstructMap ) {
		if( aNormalizedTextLen != vMap.size() )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Normalized string and map array have different lengths: %u vs %u", aNormalizedTextLen, vMap.size() );

		vMap.push_back( aTextLen );
		if( vMap.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		map = nAllocateObjects( vMemoryAllocator, unsigned int, vMap.size() );
		if( !map )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( map, vMap.get_buffer(), vMap.size() * sizeof (unsigned int) );
		aMap = map;
	}

	return INF_ENGINE_SUCCESS;
}
