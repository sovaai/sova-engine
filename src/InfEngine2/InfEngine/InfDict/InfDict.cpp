#include "InfDict.hpp"

unsigned int InfDict::GetNeedMemorySize() const
{
    /**
     *  uint32_t - общий размер памяти.
     *  uint32_t - идентификатор словаря.
     *  uint32_t - количество строк в словаре, не считая подсловари.
     *  uint32_t - ссылка на имя словаря.
     *  uint32_t - количество вариантов раскрытия словаря.
     *  uint32_t - ссылка на список количеств варинтов раскрытия строк словаря.
     * 
     *  uint32_t - количество инструкций в словаре.
     *  uint32_t - ссылка на таблицу сдвигов к инструкциям словаря.
     *  uint32_t - количество подсловарей.
     *  uint32_t - ссылка на список идентификаторов подсловарей.
     *  uint32_t - количество строк в словаре, включая подсловари.
     * 
     *  ...      - ссылки на элементы словаря.
     *  ...      - таблица замен для подчинённых строк.
     *  uint16_t - длина имени словаря.
     *  ...      - имя словаря.     
     *  ...      - элементы словаря с выравниванием.
     *  ...      - список количеств варинтов раскрытия строк словаря.
     *  ...      - таблица смещений к интсрукциям словаря.
     *  ...      - инструкции словаря.
     *  ...      - список идентификаторов подсловарей.
     */

     unsigned int vNeedMemorySize = 0;

     vNeedMemorySize +=  11*sizeof( uint32_t ) +
                         vStringsCount*sizeof( uint32_t ) +
                         vStringsCount*sizeof( uint32_t ) + // Таблица замен для подчинённых строк.
                         vStringsCount*sizeof( uint32_t ) + // Список количеств варинтов раскрытия строк словаря.                         
                         vSubdictsCount*sizeof( uint32_t ) + // Список идентификаторов подсловарей.
                         sizeof( uint16_t ) +
                         vNameLength + 1;


    for( size_t i = 0; i < vStringsCount; i++ )
    {
        // Смещение.
        vNeedMemorySize += vNeedMemorySize%4 ? 4-vNeedMemorySize%4 : 0;

        // Размер строки.
        vNeedMemorySize += vStrings[i]->GetNeedMemorySize();
    }
     
    // Интсрукции.
    unsigned int instr_to_save_cnt = 0;
    for( unsigned int instr_n = 0; instr_n < vInstructionsCount; ++instr_n )
    {        
        if( InfDictInstruction::IT_INCLUDE == vInstructions[instr_n]->GetType() )
        {
            // Интсрукции типа "--include" сохраняются в виде списка идентификаторов подсловарей
        }
        else
        {
            // Размер инструкции.
            vNeedMemorySize += vInstructions[instr_n]->GetNeedMemorySize();            
            ++instr_to_save_cnt;
        }
    }
     
    vNeedMemorySize += instr_to_save_cnt*sizeof( uint32_t ); // Таблица смещений к инструкциям словаря.
    return vNeedMemorySize;
}

InfEngineErrors InfDict::Save( char * aBuffer, unsigned int aBufferSize, unsigned int& aResultSize ) const
{
    InfEngineErrors iee;
    
    // Инициализация буфера. Нужно заполнить пустые блоки нулями, т.к. потом они будут использованы при подсчёте md5-чексуммы.
    memset( aBuffer, 0, aBufferSize ); 
    
    // Проверка аргументов.
    if( !aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

    // Проверка памяти.
    unsigned int memory = GetNeedMemorySize();
    if( memory > aBufferSize )
		ReturnWithError( INF_ENGINE_ERROR_LIMIT, INF_ENGINE_STRING_ERROR_LOW_SAVE_BUFFER_SIZE );

    char* ptr = aBuffer;

    // Память необходимая для сохранения.
    *((uint32_t*)ptr) = memory;
    ptr += sizeof( uint32_t );

    // Идентификатор.
    *((uint32_t*)ptr) = vDictID;
    ptr += sizeof( uint32_t );

    // Количество строк в словаре, не считая подсловари.
    *((uint32_t*)ptr) = vStringsCount;
    ptr += sizeof( uint32_t );

    uint32_t* pname = (uint32_t*)ptr;
    ptr += sizeof( uint32_t );
    
    uint32_t* pvariants_sum = (uint32_t*)ptr;
    ptr += sizeof( uint32_t );           
            
    uint32_t* pvariants = (uint32_t*)ptr;
    ptr += sizeof( uint32_t );
    
    // Количество инструкций в словаре.
    *((uint32_t*)ptr) = vInstructionsCount;
    ptr += sizeof( uint32_t );
    
    // Ссылка на таблицу сдвигов к инструкциям словаря.
    uint32_t* pinstr_shift_table = (uint32_t*)ptr;
    ptr += sizeof( uint32_t );           
    
    // Количество подсловарей.
    *((uint32_t*)ptr) = vSubdictsCount;
    ptr += sizeof( uint32_t );
    
    // Ссылка на список идентификаторов подсловарей.
    uint32_t* psubdicts_shift = (uint32_t*)ptr;
    ptr += sizeof( uint32_t );           
    
    // Количество строк в словаре, включая подсловари.
    *((uint32_t*)ptr) = vDisclosedStringsCount;
    ptr += sizeof( uint32_t );           
    
    uint32_t* shifts = (uint32_t*)ptr;
    ptr += vStringsCount*sizeof( uint32_t );
    
    // Таблица замен подчинённая строка -> главная строка.
    uint32_t* ref_to_main = (uint32_t*)ptr;
    ptr += vStringsCount*sizeof( uint32_t );

    *pname = (uint32_t)( ptr - aBuffer );

    *((uint16_t*)ptr) = vNameLength;
    ptr += sizeof( uint16_t );
    if( vNameLength )
    {
        memcpy( ptr, vName, vNameLength );
        ptr += vNameLength;
    }
    *ptr = '\0';
    ptr++;
    
    unsigned int mem;
    unsigned int last_main_string_n = 0;
    for( size_t i = 0; i < vStringsCount; i++ )
    {
        // Смещение.
        ptr += ( ptr - aBuffer )%4 ? 4-( ptr - aBuffer )%4 : 0;

        *shifts = (uint32_t)( ptr - aBuffer );
        shifts++;
        
        iee = vStrings[i]->Save( ptr, memory - ( ptr - aBuffer ), mem );
        if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
        ptr += mem;

        if( vStrings[i]->IsMain() )
            last_main_string_n = i;
        
        *ref_to_main = last_main_string_n;
        ++ref_to_main;        
    }
    
    // Количества вариантов раскрытия строк словаря.
    *pvariants = (uint32_t)( ptr - aBuffer );
    unsigned int variants_sum = 0;
    for( size_t i = 0; i < vStringsCount; i++ )
    {
        // Подсчёт количества вариантов раскрытия строки словаря.
        unsigned int variatns_cnt = 1;
        for( unsigned int item_n = 0; item_n < vStrings[i]->GetItemsCount(); ++item_n )
            if( InfPatternItems::itDictInline == vStrings[i]->GetItemType( item_n ) )
            {
                variatns_cnt *= dynamic_cast<const InfPatternItems::TagDictInline*>( vStrings[i]->GetItem( item_n ) )->GetStringsNumber() +
                        (dynamic_cast<const InfPatternItems::TagDictInline*>( vStrings[i]->GetItem( item_n ) )->HasEmptyVariant() ? 1 : 0 );
            }
        
        *((uint32_t*)ptr) = variatns_cnt;
        ptr += sizeof(uint32_t);
        variants_sum += variatns_cnt;
    }
    *pvariants_sum = variants_sum;
    
    // Ссылка на таблицу смещений к инструкциям словаря.
    *pinstr_shift_table = (uint32_t)( ptr - aBuffer );
    shifts = (uint32_t*)ptr;
    
    // Вычисление количества интрукций, подлежащих сохранению.
    unsigned int instr_to_save_cnt = 0;
    for( unsigned int instr_n = 0; instr_n < vInstructionsCount; ++instr_n )
    {
        if( InfDictInstruction::IT_INCLUDE == vInstructions[instr_n]->GetType() )
        {
            // Интсрукции типа "--include" сохраняются в виде списка идентификаторов подсловарей
        }
        else
        {
           ++instr_to_save_cnt;
        }
    }    
    ptr += instr_to_save_cnt * sizeof( uint16_t );
    
    // Инструкции словаря.
    for( unsigned int instr_n = 0; instr_n < vInstructionsCount; ++instr_n )
    {
        if( InfDictInstruction::IT_INCLUDE == vInstructions[instr_n]->GetType() )
        {
            // Интсрукции типа "--include" сохраняются в виде списка идентификаторов подсловарей
        }
        else
        {
            // Смещение к инструкции.
            *shifts = ptr - aBuffer;
            ++shifts;

            // Сохранение инструкции.
            if( INF_ENGINE_SUCCESS != ( iee = vInstructions[instr_n]->Save( ptr, memory - ( ptr - aBuffer ), mem ) ) )
            {
                if( INF_ENGINE_ERROR_INV_DATA == iee )
					ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
                else if ( INF_ENGINE_ERROR_INV_ARGS == iee )
					ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
                else
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Failed to save dictionary \"%s\" instructuon #%i", GetName(), instr_n );
            }
            ptr += mem;
        }
    }
    
    // Ссылка на список идентификаторов подсловарей.
    *psubdicts_shift = (uint32_t)( ptr - aBuffer );
    memcpy( ptr, vSubdicts, vSubdictsCount * sizeof(uint32_t) );
    ptr += vSubdictsCount * sizeof(uint32_t);
    
    aResultSize = ptr - aBuffer;
    if( memory != aResultSize )
		ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, "Wrong checksum in dict saving: %i instead %i", aResultSize, memory );
    
    return INF_ENGINE_SUCCESS;
}

void InfDict::SetDictStrings( const InfDictString ** aStrings, unsigned int aStringsCount )
{
    vStrings = aStrings;
    vStringsCount = aStringsCount;
}

void InfDict::SetDictInstructions( const InfDictInstruction ** aInstructions, unsigned int aInstructionsCount )
{
    vInstructions = aInstructions;
    vInstructionsCount = aInstructionsCount;
}

void InfDict::SetSubdicts( const unsigned int * aSubdicts, unsigned int aSubdictsCount )
{
    vSubdicts = aSubdicts;
    vSubdictsCount = aSubdictsCount;
}

void InfDict::SetName( const char * aName, unsigned int aNameLength )
{
    vName = aName;
    vNameLength = aNameLength;
}

