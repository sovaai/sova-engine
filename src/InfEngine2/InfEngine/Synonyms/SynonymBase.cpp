#include <sys/syslog.h>
#include <unistd.h>

#include "SynonymBase.hpp"
#include "InfEngine2/_Include/FStorage.hpp"
#include <ClientLib/Protocol.hpp>

SynonymBaseWR::SynonymBaseWR( InfLingProc *& aLp, fstorage_section_id aSectionId ):
	vSynonymIndexWR( aLp, aSectionId ),
	vSynonymIndexWR_tmp( aLp, aSectionId ),
	vExtSectionID( aSectionId + TERMS_SECTION_MAX_NUMBER )
{
}

InfEngineErrors SynonymBaseWR::Create( )
{
    // Проверка состояния индексов.
	if( vDictIndex.GetState( ) != NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "SynonymBase has been already created or opened." );

	if( vMainIndex.GetState( ) != NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "SynonymBase has been already created or opened." );

    // Создание индекса главных синонимов.
    vMainIndex.Create( );
    // Создание индекса названий словарей синонимов.
    vDictIndex.Create( );
    // Создание индекса неглавных синонимов.
    InfEngineErrors iee = vSynonymIndexWR.Create( );
    if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
    iee = vSynonymIndexWR_tmp.Create( );
    if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );


    return INF_ENGINE_SUCCESS;
}

void SynonymBaseWR::Reset( )
{
    vDictIndex.Reset( );
    vMainIndex.Reset( );
    vSynonymIndexWR.Reset( );
    vSynonymIndexWR_tmp.Reset( );
}

unsigned int SynonymBaseWR::GetNeedMemorySize( ) const
{
    unsigned int NeedMemorySize = 3 * sizeof (uint32_t ) + vDictIndex.GetNeedMemorySize( );
    NeedMemorySize += NeedMemorySize % 4 ? 4 - NeedMemorySize % 4 : 0;
    NeedMemorySize += vMainIndex.GetNeedMemorySize( );

    return NeedMemorySize;
}

InfEngineErrors SynonymBaseWR::Save( fstorage * aFStorage, bool aTmp )
{
    // Проверка аргументов.
    if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

    // Проверка статуса.
	if( vDictIndex.GetState( ) == NanoLib::NameIndex::stClosed ||
		vMainIndex.GetState( ) == NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

    // Сохраняем индекс неглавных синонимов.
    InfEngineErrors iee = INF_ENGINE_SUCCESS;
    if( aTmp )
    {
        if( INF_ENGINE_SUCCESS != ( iee = vSynonymIndexWR_tmp.Save( aFStorage ) ) )
			ReturnWithTrace( iee );
    }
    else
    {
        if( INF_ENGINE_SUCCESS != ( iee = vSynonymIndexWR.Save( aFStorage ) ) )
			ReturnWithTrace( iee );
    }

    // Получение указателя на секцию.
    fstorage_section* SectData = fstorage_get_section( aFStorage, vExtSectionID );
    if( !SectData )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

    // Выделение памяти.
    unsigned int MemorySize = GetNeedMemorySize( );
    int ret = fstorage_section_realloc( SectData, MemorySize );
    if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

    // Получение указателя на память.
    char* Buffer = static_cast<char*>( fstorage_section_get_all_data( SectData ) );
    if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

    // Сдвиг к месту расположения индексов.
    uint32_t * Shift = reinterpret_cast<uint32_t*>( Buffer );
    Buffer += 3 * sizeof (uint32_t );
    // Общий размер сохранённых индексов.
    Shift[0] = MemorySize;
    // Сдвиг к индексу названий словарей синонимов.
    Shift[1] = Buffer - reinterpret_cast<char*>( Shift );
    // Сохраняем индекс названий словарей синонимов.
	Buffer += vDictIndex.Save( Buffer );
    Buffer += ( Buffer - reinterpret_cast<char*>( Shift ) ) % 4 ? 4 - ( Buffer - reinterpret_cast<char*>( Shift ) ) % 4 : 0;

    // Сдвиг к индексу главных синонимов.
    Shift[2] = Buffer - reinterpret_cast<char*>( Shift );
    // Сохраняем индекс главных синонимов.
	vMainIndex.Save( Buffer );

    return INF_ENGINE_SUCCESS;
}

void SynonymBaseWR::Close( )
{
    vMainIndex.Reset( );
    vDictIndex.Reset( );
    vSynonymIndexWR.Destroy( );
    vSynonymIndexWR_tmp.Destroy( );
}

InfEngineErrors SynonymBaseWR::AddSynonymGroup( const SynonymParser::SynonymGroup & aSynGroup, unsigned int aSynGroupN,
                                                const char * aSynDictName, unsigned int aSynDictNameLen )
{
    InfEngineErrors iee = INF_ENGINE_SUCCESS;
    unsigned int main_id = static_cast<unsigned int>( -1 );
    unsigned int dict_id = static_cast<unsigned int>( -1 );

    // Очишаем сообщение об ошибке.
    vErrorMsg.clear( );

    // Сохраняем название словаря синонимов.
    NanoLib::NameIndex::ReturnCode rt = vDictIndex.AddName( aSynDictName, dict_id );
    if( rt != NanoLib::NameIndex::rcSuccess && rt != NanoLib::NameIndex::rcElementExists )
		ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to registrate synonym dictionary name: %u", rt );

    // Сохраняем главный синоним.
    if( NanoLib::NameIndex::rcElementExists == ( rt = vMainIndex.AddName( aSynGroup.Major.Text, main_id ) ) )
    {
        //TODO: error message.
        return INF_ENGINE_WARN_UNSUCCESS;
    }
    if( rt != NanoLib::NameIndex::rcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to registrate main synonym \"%s\" : %u",
                         aSynGroup.Major.Text, rt );

    // Сохраняем неглавные синонимы.
    bool there_are_duplicates = false;
    for( unsigned int minor_n = 0; minor_n < aSynGroup.Minors.size( ); ++minor_n )
    {
        if( INF_ENGINE_SUCCESS != ( iee = vSynonymIndexWR.AddSynonym( &aSynGroup.Minors[minor_n], main_id, aSynGroupN,
                                                                      dict_id ) ) )
        {
            if( iee == INF_ENGINE_WARN_SYNONYM_DUPLICATE )
            {
                // TODO: make another warning message.
				LogWarn( "Synonym \"%s\" is duplicated", aSynGroup.Minors[minor_n].Text );

                if( nlrcSuccess != vErrorMsg.append( "Synonoym \"" ) ||
                    nlrcSuccess != vErrorMsg.append( aSynGroup.Minors[minor_n].Text ) ||
                    nlrcSuccess != vErrorMsg.append( "\" in group #" ) ||
                    nlrcSuccess != vErrorMsg.append( aSynGroupN + 1 ) ||
                    nlrcSuccess != vErrorMsg.append( " is duplicated\n\t" ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

                there_are_duplicates = true;
            }
            else
				ReturnWithTrace( iee );
        }

        if( INF_ENGINE_SUCCESS != ( iee = vSynonymIndexWR_tmp.AddSynonym( &aSynGroup.Minors[minor_n], main_id, aSynGroupN,
                                                                          dict_id ) ) && iee != INF_ENGINE_WARN_SYNONYM_DUPLICATE )
			ReturnWithTrace( iee );
    }

    return there_are_duplicates ? INF_ENGINE_WARN_SYNONYM_DUPLICATE : INF_ENGINE_SUCCESS;
}

const char * SynonymBaseWR::GetErrorMsg( ) const
{
    return vErrorMsg.ToConstChar( );
}


SynonymBaseRO::SynonymBaseRO( InfLingProc *& aLp, fstorage_section_id aSectionId ):
	vLp( aLp ),
	vSynonymIndex( vLp, aSectionId ),
	vExtSectionID( aSectionId + TERMS_SECTION_MAX_NUMBER )
{

}

InfEngineErrors SynonymBaseRO::Open( fstorage* aFStorage )
{
    // Проверка агрументов.
    if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

    // Закрытие базы синонимов, если она было открыта.
    Close( );

    // Открытие индекса неглавных синонимов.
    InfEngineErrors iee = vSynonymIndex.Open( aFStorage );
    if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

    // Загрузка остальных индексов.
    fstorage_section* sectdata = fstorage_get_section( aFStorage, vExtSectionID );
    if( !sectdata )
    {
        Close( );
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );
    }

    // Получение указателя на память.
    const char * buffer = static_cast<const char *>( fstorage_section_get_all_data( sectdata ) );
    if( !buffer )
    {
        Close( );
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get fstorage pointer." );
    }

    // Сдвиг к месту расположения индексов.
    const uint32_t * shift = reinterpret_cast<const uint32_t*>( buffer );

    // Загрузка индекса названий словарей.
    NanoLib::NameIndex::ReturnCode rc = vDictIndex.Open( buffer + shift[1], shift[0] - shift[1] );
    if( rc != NanoLib::NameIndex::rcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NANOLIB, INF_ENGINE_STRING_ERROR_INTERNAL );

    // Загрузка индексов главных синонимов.
    if( NanoLib::NameIndex::rcSuccess != ( rc = vMainIndex.Open( buffer + shift[2], shift[0] - shift[2] ) ) )
		ReturnWithError( INF_ENGINE_ERROR_NANOLIB, INF_ENGINE_STRING_ERROR_INTERNAL );

    return INF_ENGINE_SUCCESS;
}

void SynonymBaseRO::Close( )
{
    vSynonymIndex.Close( );
    vMainIndex.Reset( );
    vDictIndex.Reset( );
}

InfEngineErrors SynonymBaseRO::ApplySynonyms( const char * aText, unsigned int aTextLen,
                                              const char *& aNormalizedText, unsigned int & aNormalizedTextLen )
{
    const unsigned int * map = nullptr;
    InfEngineErrors iee = ApplySynonymsAndConstructMap( aText, aTextLen, aNormalizedText, aNormalizedTextLen, map, false );
    if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors SynonymBaseRO::ApplySynonyms( const char * aText, unsigned int aTextLen,
                                              const char *& aNormalizedText, unsigned int & aNormalizedTextLen, 
											  const unsigned int *& aMap )
{
    InfEngineErrors iee = ApplySynonymsAndConstructMap( aText, aTextLen, aNormalizedText, aNormalizedTextLen, aMap, true );
    if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors SynonymBaseRO::ApplySynonymsAndConstructMap( const char * aText, unsigned int aTextLen,
                                                             const char *& aNormalizedText, unsigned int & aNormalizedTextLen, 
															 const unsigned int *& aMap, bool aConstructMap )
{
    // Подготовка для поиска.
    char* normalized_text = nullptr;
    unsigned int* map = nullptr;
    vDocMessage.ResetAll( );
    vDocImage.Reset( );
    int ret = vDocMessage.SetMessage( nullptr, 0, aText, aTextLen, true );
    if( ret != 0 )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );

    // Установка языка.
    vDocMessage.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );

    // Лемматизация текста.
    InfEngineErrors iee = vSynonymIndex.LemmatizeDoc( vDocMessage, vDocImage );
    if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

    // Поиск синонимов для слов из текста.
    vMatcher.Reset( );
    if( INF_ENGINE_SUCCESS != ( iee = vSynonymIndex.Check( vDocImage, vMatcher ) ) )
		ReturnWithTrace( iee );

    // Построение нормализованной строки.
    vNormalizedText.clear( );
    vMap.clear( );
    if( vMatcher.GetSynonymsCount( ) )
    {
        const DocText * doc_text = vDocImage.GetDocText( );
        unsigned int pos_n = 0;
        for( size_t word_n = 0; word_n < doc_text->WordsCount( ); ++word_n )
        {
            // Если синонимов для слова нет, оставляем слово как есть.
            const char * synonym = doc_text->WordText( word_n );
            unsigned int synonym_len = doc_text->WordLength( word_n );

            // Если найден синоним, то заменяем на него исходное слово.
            if( pos_n < vMatcher.GetSynonymsCount( ) && word_n == vMatcher.GetPosition( pos_n ) )
            {
                // Получаем аттрибуты найденного синонима.
                const SynonymAttrsRO & attrs = vMatcher.GetSynonym( pos_n );
                if( nullptr == ( synonym = vMainIndex.GetName( attrs.GetMainID( ), synonym_len ) ) )
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Main synonym with id [%u] is missed",
                                     attrs.GetMainID( ) );

                // Переходим к следующему синониму.
                ++pos_n;
            }

            // Дописываем слово в конец нормализованной строки.
            if( vNormalizedText.size( ) )
            {
                if( nlrcSuccess != vNormalizedText.append( " ", 1 ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
                                     INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
                vMap.push_back( vMap.back( ) + 1 );
            }

            if( nlrcSuccess != vNormalizedText.append( synonym, synonym_len ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
                                 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

            // Дописываем индексы в карту соответствия.
            vMap.reserve( vMap.size( ) + synonym_len );
            if( vMap.no_memory( ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
                                 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
            vMap.push_back( doc_text->WordOffset( word_n ) );
            for( unsigned int chr_n = 1; chr_n < synonym_len; ++chr_n )
                vMap.push_back( doc_text->WordOffset( word_n ) + doc_text->WordLength( word_n ) - 1 );
        }

    }
    else
    {
        // В случае отсутствия синонимов нормализованная строка - это входная строка.
        vNormalizedText.assign( aText, aTextLen );
        vMap.reserve( aTextLen );
        if( vMap.no_memory( ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
                             INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
        for( unsigned int chr_n = 0; chr_n < aTextLen; ++chr_n )
            vMap.push_back( chr_n );
    }

    // Сохранение построенной нормализованной строки.
    aNormalizedTextLen = vNormalizedText.size( );
    normalized_text = nAllocateObjects( vMemoryAllocator, char, aNormalizedTextLen + 1 );
    if( !normalized_text )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
    memcpy( normalized_text, vNormalizedText.ToConstChar( ), aNormalizedTextLen );
    normalized_text[aNormalizedTextLen] = '\0';
    aNormalizedText = normalized_text;

    // Сохранение индексов соответствия.
    if( aConstructMap )
    {
        if( aNormalizedTextLen != vMap.size( ) )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL,
                             "Normalized string and map array have different lengths: %u vs %u",
                             aNormalizedTextLen, vMap.size( ) );

        vMap.push_back( aTextLen );
        if( vMap.no_memory( ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
                             INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

        map = nAllocateObjects( vMemoryAllocator, unsigned int, vMap.size( ) );
        if( !map )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
        memcpy( map, vMap.get_buffer( ), vMap.size( ) * sizeof (unsigned int) );
        aMap = map;
    }

    return INF_ENGINE_SUCCESS;
}
