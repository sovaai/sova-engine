#include "nMemoryAllocator.hpp"

#include <cstring>
#include <cstdint>

#include <stdlib.h>


/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      Параметры и функции для работы с технической частью выделяемых секторов
 *
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

/** Определение размера технической области каждого сектора выделяемой памяти. **/
static size_t gReservedSectorPartSize = sizeof( void* ) + sizeof( size_t );

/** Получение сектора, следующего за указанным. **/
inline void* GetNextSector( void* aSector )
{
    return *static_cast<void**>(aSector);
}

/** Запись указателя на следущий сектор для указанного. **/
inline void  SetNextSector( void* aSector, void* aNextSector )
{
    memcpy( aSector, &aNextSector, sizeof( void* ) );
}

/** Получение размера указанного сектора. **/
inline size_t GetSectorSize( void* aSector )
{
    return *reinterpret_cast<size_t*>((static_cast<char*>(aSector)+sizeof(void*)));
}

/** Установка размера указанного сектора. **/
inline void SetSectorSize( void* aSector, size_t aSectorSize )
{
    *reinterpret_cast<size_t*>(static_cast<char*>(aSector)+sizeof(void*)) = aSectorSize;
}

nMemoryAllocator::nMemoryAllocator( size_t aFirstSectorSize, size_t aSectorSize )
{
    if( aSectorSize <= gReservedSectorPartSize || aSectorSize == size_t(-1) )
        vSectorSize = 1000;
    else
        vSectorSize = aSectorSize;

    if( aFirstSectorSize <= gReservedSectorPartSize || aFirstSectorSize == size_t(-1) )
        vFirstSectorSize = vSectorSize;
    else
        vFirstSectorSize = aFirstSectorSize;

    // Инициализация всех переменных.
    vFirstSector = NULL;
    vCurrentSector = NULL;
    vFreeMemory = NULL;

    vAllocatedMemory = 0;
    vFullAllocatedMemorySize = 0;
}

void nMemoryAllocator::SetMemoryStep( size_t aSectorSize )
{
    if( aSectorSize > gReservedSectorPartSize && aSectorSize != size_t(-1) )
        vSectorSize = aSectorSize;
}

void* nMemoryAllocator::Allocate( size_t aSize )
{
    // Проверка аргументов.
    if( aSize == 0 || size_t(-1) - aSize <= sizeof(uint32_t) )
        return NULL;

    // Приведение размера к кратности.
    aSize = aSize + ( aSize%sizeof(uint32_t) ? sizeof(uint32_t)-aSize%sizeof(uint32_t) : 0 ) ;

    if( !vCurrentSector )
    {
        // Проверка на превышение общего лимита выделения памяти.
        if( size_t(-1) - aSize <= gReservedSectorPartSize || 
            size_t(-1) - vFullAllocatedMemorySize <= gReservedSectorPartSize + aSize )
            return NULL;

        // Память еще не выделялась.
        size_t size = 0;
        if( vFirstSectorSize < aSize + gReservedSectorPartSize )
            size = aSize + gReservedSectorPartSize;
        else
            size = vFirstSectorSize;

        // Выделяем память.
        void* nptr = malloc( size );
        if( !nptr )
            return NULL;

        // Зануленье памяти.
        memset( nptr, 0, size );

        vFullAllocatedMemorySize += size;

        // Формируем техническую часть сектора.
        SetSectorSize( nptr, size-gReservedSectorPartSize );
        SetNextSector( nptr, NULL );

        // Установка указателей класса.
        vCurrentSector = nptr;
        vFirstSector = nptr;
        vFreeMemory = static_cast<char*>(nptr)+gReservedSectorPartSize;
        vAllocatedMemory = size-gReservedSectorPartSize;
    }
    else if( aSize > vAllocatedMemory )
    {
        // Памяти в текущем секторе недостаточно.
        void* ptr = NULL;
        void* prev = vCurrentSector;

        if( GetNextSector( vCurrentSector ) != NULL )
        {
            // Пытаемся использовать ранее выделенную память.
            ptr = GetNextSector( vCurrentSector );
            while( ptr )
            {
                if( GetSectorSize( ptr ) >= aSize )
                    break;

                prev = ptr;
                ptr  = GetNextSector( ptr );
            }
        }

        if( ptr )
        {
            // Нужный сектор найден.
            if( prev != vCurrentSector )
            {
                // Переставляем сектора.
                void* next = GetNextSector( vCurrentSector );
                void* tail = GetNextSector( ptr );

                SetNextSector( vCurrentSector, ptr );
                SetNextSector( ptr, next );
                SetNextSector( prev, tail );
            }

            vCurrentSector = ptr;
            vFreeMemory = static_cast<char*>(ptr)+gReservedSectorPartSize;
            vAllocatedMemory = GetSectorSize( ptr );
        }
        else
        {
            // Ранее выделенной памяти нехватает.

            // Проверка на превышение общего лимита выделения памяти.
            if( size_t(-1) - aSize <= gReservedSectorPartSize || 
                size_t(-1) - vFullAllocatedMemorySize <= gReservedSectorPartSize + aSize )
                return NULL;

            size_t size = 0;
            if( vSectorSize < aSize + gReservedSectorPartSize || size_t(-1) - vSectorSize <= vFullAllocatedMemorySize )
                size = aSize + gReservedSectorPartSize;
            else
                size = vSectorSize;

            // Выделяем память.
            void* nptr = malloc( size );
            if( !nptr )
                return NULL;

            // Зануленье памяти.
            memset( nptr, 0, size );

            vFullAllocatedMemorySize += size;

            SetSectorSize( nptr, size-gReservedSectorPartSize );
            SetNextSector( nptr, GetNextSector( vCurrentSector ) );
            SetNextSector( vCurrentSector, nptr );

            // Установка указателей класса.
            vCurrentSector = nptr;
            vFreeMemory = static_cast<char*>(nptr)+gReservedSectorPartSize;
            vAllocatedMemory = size-gReservedSectorPartSize;
        }
    }

    // Памяти достаточно.
    void* ptr = vFreeMemory;

    // Передвигаем указатель на свободную память.
    vFreeMemory = static_cast<char*>(vFreeMemory) + aSize;

    // Уменьшаем счетчик доступной памяти.
    if( aSize <= vAllocatedMemory )
        vAllocatedMemory -= aSize;
    else
        vAllocatedMemory = 0;

    return ptr;
}

void nMemoryAllocator::Reset()
{
    vCurrentSector = vFirstSector;
    if( vCurrentSector )
    {
        vFreeMemory = static_cast<char*>(vCurrentSector)+gReservedSectorPartSize;
        vAllocatedMemory = GetSectorSize( vCurrentSector );
    }
    else
        vAllocatedMemory = 0;
}

void nMemoryAllocator::Free()
{
    void* ptr = vFirstSector;

    unsigned int i = 0;
    while( ptr )
    {
        i++;
        void* cur = ptr;
        ptr = GetNextSector( ptr );
        free( cur );
    }

    vFirstSector = NULL;
    vCurrentSector = NULL;

    vFullAllocatedMemorySize = 0;
    vAllocatedMemory = 0;
}

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      Обертка на C для nMemoryAllocator
 *
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/
#include "nMemoryAllocator.h"

extern "C" ncMemoryAllocator* ncMemoryAllocatorCreate()
{
    // Создание аллокатора.
    return reinterpret_cast<ncMemoryAllocator*>(new nMemoryAllocator);
}

extern "C" void ncMemoryAllocatorDestroy( ncMemoryAllocator* aMemoryAllocator )
{
    // Проверка аргументов.
    if( !aMemoryAllocator )
        return;

    // Удаление аллокатора.
    delete reinterpret_cast<nMemoryAllocator*>(aMemoryAllocator);
}

extern "C" void* AllocatorAllocate( ncMemoryAllocator* aMemoryAllocator, size_t aTargetMemorySize )
{
    // Проверка аргументов.
    if( !aMemoryAllocator || aTargetMemorySize == 0 )
        return NULL;

    // Выделение памяти.
    return reinterpret_cast<nMemoryAllocator*>(aMemoryAllocator)->Allocate( aTargetMemorySize );
}

extern "C" void ncMemoryAllocatorFree( ncMemoryAllocator* aMemoryAllocator )
{
    // Проверка аргументов.
    if( !aMemoryAllocator )
        return;

    // Освобождение памяти.
    reinterpret_cast<nMemoryAllocator*>(aMemoryAllocator)->Free();
}

extern "C" void ncMemoryAllocatorReset( ncMemoryAllocator* aMemoryAllocator )
{
    // Проверка аргументов.
    if( !aMemoryAllocator )
        return;

    // Очистка памяти.
    reinterpret_cast<nMemoryAllocator*>(aMemoryAllocator)->Reset();
}
