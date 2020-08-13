#include "TagSwitch.hpp"

using namespace InfPatternItems;

unsigned int TagSwitch::GetNeedMemorySize() const
{
	unsigned int NeedMemory = sizeof( uint32_t ) +    // размер тэга в байтах.
                              sizeof( uint32_t ) +    // количество блоков case.
                              sizeof( uint32_t ) +    // смещение к аргументу switch.
                              sizeof( uint32_t ) +    // смещение телу блока default.
                              2 * vCaseNumber * sizeof( uint32_t );    // пары вида (смещение к аргументу case, смещение к телу блока case).

    NeedMemory += vSwitchArg->GetNeedMemorySize();      // аргумент switch.
    NeedMemory += NeedMemory%4 ? 4-NeedMemory%4 : 0;    // выравнивание адреса.

    NeedMemory += vDefaultBody->GetNeedMemorySize();    // тело блока default.
    NeedMemory += NeedMemory%4 ? 4-NeedMemory%4 : 0;    // выравнивание адреса.

    for( unsigned int case_n = 0; case_n < vCaseNumber; ++case_n )
    {
        NeedMemory += vCaseArg[case_n]->GetNeedMemorySize();     // аргумент case.
        NeedMemory += NeedMemory%4 ? 4-NeedMemory%4 : 0;    // выравнивание адреса.
        NeedMemory += vCaseBody[case_n]->GetNeedMemorySize();    // тело блока case.
        NeedMemory += NeedMemory%4 ? 4-NeedMemory%4 : 0;    // выравнивание адреса.
    }

	return NeedMemory;
}

unsigned int TagSwitch::Save( void * aBuffer ) const
{
	// Таблица сдвигов.
    uint32_t * shift = reinterpret_cast<uint32_t*>( aBuffer ) + 2;

    char * ptr = static_cast<char*>( aBuffer ) +
            sizeof( uint32_t ) +    // размер тэга в байтах.
            sizeof( uint32_t ) +    // количество блоков case.

            sizeof( uint32_t ) +    // смещение к аргументу switch.
            sizeof( uint32_t ) +    // смещение телу блока default.
            2 * vCaseNumber * sizeof( uint32_t );    // пары вида (смещение к аргументу case, смещение к телу блока case).

    // Сохраняем количество блоков case.
    reinterpret_cast<uint32_t*>(aBuffer)[1] = vCaseNumber;

    // Сохраняем смещение к аргументу switch.
	*shift = ptr - static_cast<char*>( aBuffer );
	++shift;
    // Сохраняем аргумент switch.
    ptr += vSwitchArg->Save( ptr );
	// Выравнивание адреса.
	ptr += ( ptr - static_cast<char*>( aBuffer ))%4 ? 4 - ( ptr - static_cast<char*>( aBuffer ) )%4 : 0;

    // Сохраняем смещение к телу блока default.
    *shift = ptr - static_cast<char*>( aBuffer );
    ++shift;
    // Сохраняем блок default.
    ptr += vDefaultBody->Save( ptr );
    // Выравнивание адреса.
    ptr += ( ptr - static_cast<char*>( aBuffer ))%4 ? 4 - ( ptr - static_cast<char*>( aBuffer ) )%4 : 0;

    for( unsigned int case_n = 0; case_n < vCaseNumber; ++case_n )
    {
        // Сохраняем смещение к аргументу case.
        *shift = ptr - static_cast<char*>( aBuffer );
        ++shift;
        // Сохраняем аргумент case.
        ptr += vCaseArg[case_n]->Save( ptr );
        // Выравнивание адреса.
        ptr += ( ptr - static_cast<char*>( aBuffer ))%4 ? 4 - ( ptr - static_cast<char*>( aBuffer ) )%4 : 0;

        // Сохраняем смещение к телу блока case.
        *shift = ptr - static_cast<char*>( aBuffer );
        ++shift;
        // Сохраняем блок case.
        ptr += vCaseBody[case_n]->Save( ptr );
        // Выравнивание адреса.
        ptr += ( ptr - static_cast<char*>( aBuffer ))%4 ? 4 - ( ptr - static_cast<char*>( aBuffer ) )%4 : 0;
    }

	// Сохраняем размер тэга в байтах.
    reinterpret_cast<uint32_t*>(aBuffer)[0] = ptr - static_cast<char*>( aBuffer );

	return ptr - static_cast<char*>( aBuffer );
}
