#include "CacheMemcached.hpp"

using namespace NanoLib;

#include <unistd.h>
#include <ctime>
#include <cstring>

#define NANO_CACHE_MEMCACHED_PROTOCOL_EXTENDED 0x1

/** Максимальная длина значения записи. */
#define NANO_CACHE_MEMCACHED_VALUE_MAX_LENGTH 1000000

CacheMemcached::~CacheMemcached( )
{
    // Уничтожение объекта.
    if( vState == StateReady )
        memcached_free( &vMemCached );
}

Cache::ReturnCode CacheMemcached::Init( )
{
    // Проверка состояния.
    if( vState == StateReady )
		ReturnWithError( rcErrorInvState, "Cache has been initialized already." );

    // Создание объекта memcached.
    if( !memcached_create( &vMemCached ) )
		ReturnWithError( rcErrorNoFreeMemory, "Can't create memcached object." );

    // Установка состояния.
    vState = StateReady;

    return rcSuccess;
}

Cache::ReturnCode CacheMemcached::AddServer( const char * aServer, unsigned int aPort )
{
    // Проверка аргументов.
    if( !aServer )
		ReturnWithError(rcErrorInvArgs, "Internal error: Invalid arguments." );

    // Проверка состояния.
    if( vState != StateReady )
		ReturnWithError(rcErrorInvState, "Cache has not been created." );

    // ToDo: нужно обновить версию либы libmemcached на релизе и убрать этот каст.
    memcached_return mr = memcached_server_add( &vMemCached, (char*)aServer, aPort );
    if( mr != MEMCACHED_SUCCESS )
		ReturnWithError(rcErrorFault, "Can't add server( %s:%u ) to memcached."
                         " Memcached error: %s", aServer ? aServer : "NULL", aPort, memcached_strerror( &vMemCached, mr ) );

    return rcSuccess;
}

Cache::ReturnCode CacheMemcached::InsertData( const char * aKey, unsigned int aKeySize,
                                              const char * aData, unsigned int aDataSize )
{
    // Проверка аргументов.
    if( !aKey || !aData )
		ReturnWithError(rcErrorFault, "Invalid arguments." );

    // Проверка состояния.
    if( vState == StateNotReady )
		ReturnWithError(rcErrorInvState, "Cache has not been created." );

    // Проверка ключа.
    if( aKeySize > NANOLIB_CACHE_MEMCACHED_KEY_MAX_LENGTH )
		ReturnWithError(rcErrorKey, "Key is too long: %u", aKeySize );

    // Сброс аллокатора.
    vRuntimeAllocator.Reset( );

    if( aDataSize < NANO_CACHE_MEMCACHED_VALUE_MAX_LENGTH )
    {
        // Расширенный протокол не используется.

        // Создание ключа.
        unsigned int KeyLen;
        const char * Key = MakeCtlKey( aKey, aKeySize, KeyLen );
        if( !Key )
			ReturnWithTrace( rcErrorNoFreeMemory );

        // Добавление значения в кэш.
        memcached_return mr = memcached_set( &vMemCached, Key, KeyLen, aData, aDataSize, vTTL, 0 );
        if( mr != MEMCACHED_SUCCESS )
			ReturnWithError(rcErrorFault, "Can't add data to cache( key: %s[ %u ] )."
                             " Memcached error: %s", Key, KeyLen, memcached_strerror( &vMemCached, mr ) );

        return rcSuccess;
    }

    // Используется расширенный протокол.

    // Проверка размера данных.

    // Подготовка базовой части ключа.
    unsigned int BaseKeyLen = 0;
    char* basekey = static_cast<char*>( vRuntimeAllocator.Allocate( NANOLIB_CACHE_MEMCACHED_KEY_MAX_LENGTH + 41 ) );
    if( !basekey )
		ReturnWithError(rcErrorNoFreeMemory, "Can't allocate memory." );
    memcpy( basekey, aKey, aKeySize );
    BaseKeyLen += aKeySize;
    basekey[BaseKeyLen] = '-';
    BaseKeyLen++;
    BaseKeyLen += sprintf( basekey + BaseKeyLen, "%u", (unsigned int)getpid( ) );
    basekey[BaseKeyLen] = '-';
    BaseKeyLen++;
    BaseKeyLen += sprintf( basekey + BaseKeyLen, "%u", (unsigned int)time( nullptr ) );
    basekey[BaseKeyLen] = '-';
    BaseKeyLen++;

    // Разбиваем буффер на части и записываем их в кэш.
    unsigned int part;
    for( part = 0; part * NANO_CACHE_MEMCACHED_VALUE_MAX_LENGTH < aDataSize; part++ )
    {
        // Создание ключа.
        unsigned int KeyLen = BaseKeyLen;
        KeyLen += sprintf( basekey + BaseKeyLen, "%u", part );
        basekey[KeyLen] = '\0';

        // Вычисление размера текущей части.
        unsigned int size = ( ( part + 1 ) * NANO_CACHE_MEMCACHED_VALUE_MAX_LENGTH < aDataSize ) ?
                NANO_CACHE_MEMCACHED_VALUE_MAX_LENGTH : aDataSize - part*NANO_CACHE_MEMCACHED_VALUE_MAX_LENGTH;

        // Добавление значения в кэш.
        memcached_return mr = memcached_set( &vMemCached, basekey, KeyLen,
                                             aData + part*NANO_CACHE_MEMCACHED_VALUE_MAX_LENGTH, size, vTTL, 0 );
        if( mr != MEMCACHED_SUCCESS )
			ReturnWithError(rcErrorFault, "Can't add data to cache( key: %s[ %u ] )."
                             " Memcached error: %s", basekey, KeyLen, memcached_strerror( &vMemCached, mr ) );
    }

    // Создание управляющей записи.
    unsigned int BufferLen = BaseKeyLen + sizeof (uint32_t )*2;
    char* buffer = (char*)vRuntimeAllocator.Allocate( BufferLen + 1 );
    if( !buffer )
		ReturnWithError(rcErrorNoFreeMemory, "Can't allocate memory." );

    ( (uint32_t*)buffer )[0] = part;
    ( (uint32_t*)buffer )[1] = aDataSize;
    memcpy( buffer + 2 * sizeof (uint32_t ), basekey, BaseKeyLen );
    buffer[BufferLen] = '\0';

    // Создание ключа.
    unsigned int KeyLen;
    const char * Key = MakeCtlKey( aKey, aKeySize, KeyLen );
    if( !Key )
		ReturnWithTrace( rcErrorNoFreeMemory );

    // Добавление управляющей записи в кэш.
    memcached_return mr = memcached_set( &vMemCached, Key, KeyLen, buffer, BufferLen,
                                         vTTL, NANO_CACHE_MEMCACHED_PROTOCOL_EXTENDED );
    if( mr != MEMCACHED_SUCCESS )
		ReturnWithError(rcErrorFault, "Can't add data to cache( key: %s[ %u ] )."
                         " Memcached error: %s", Key, KeyLen, memcached_strerror( &vMemCached, mr ) );

    return rcSuccess;
}

Cache::ReturnCode CacheMemcached::GetData( const char * aKey, unsigned int aKeySize, char*& aResult, unsigned int &aResultSize )
{
    // Проверка аргументов.
    if( !aKey || aKeySize == 0 )
		ReturnWithError(rcErrorInvArgs, "Internal error: Invalid arguments." );

    // Проверка состояния.
    if( vState == StateNotReady )
		ReturnWithError(rcErrorInvState, "Incorrect memcached object state: %d", vState );

    // Проверка ключа.
    if( aKeySize > NANOLIB_CACHE_MEMCACHED_KEY_MAX_LENGTH )
		ReturnWithError(rcErrorKey, "Key is too long: %u", aKeySize );

    // Сброс аллокатора.
    vRuntimeAllocator.Reset( );

    // Создание ключа.
    unsigned int KeyLen;
    const char * Key = MakeCtlKey( aKey, aKeySize, KeyLen );
    if( !Key )
		ReturnWithTrace( rcErrorNoFreeMemory );

    // Получение значения из кэша.
    uint32_t flags;
    size_t Size;
    memcached_return mr;
    char* vTmpPtr = memcached_get( &vMemCached, Key, KeyLen, &Size, &flags, &mr );
    if( mr != MEMCACHED_SUCCESS )
    {
        if( mr != MEMCACHED_NOTFOUND && mr != MEMCACHED_DATA_DOES_NOT_EXIST )
        {
			ReturnWithError(rcErrorFault, "Can't get data from memcached( key: %s[ %u ] )"
                             ". Memcached error: %s", Key, KeyLen, memcached_strerror( &vMemCached, mr ) );
        }
        else
            return rcUnsuccess;
    }
    aResultSize = Size;

    if( !( flags & NANO_CACHE_MEMCACHED_PROTOCOL_EXTENDED ) )
    {
        // Расширенный протокол не используется.

        aResult = vTmpPtr;

        return rcSuccess;
    }

    // Проверка размера записи.
    if( aResultSize < 2 * sizeof (uint32_t ) + 1 )
		ReturnWithError(rcErrorFault, "Internal error: Invalid data." );

    // Используется расширенный протокол.
    char* ptr = vTmpPtr;

    uint32_t parts = *( (uint32_t*)ptr );
    ptr += sizeof ( uint32_t );

    uint32_t fullsize = *( (uint32_t*)ptr );
    ptr += sizeof ( uint32_t );

    // Подготовка базовой части ключа.
    char* basekey = static_cast<char*>( vRuntimeAllocator.Allocate( NANOLIB_CACHE_MEMCACHED_KEY_MAX_LENGTH + 41 ) );
    if( !basekey )
		ReturnWithError(rcErrorNoFreeMemory, "Can't allocate memory." );

    unsigned int BaseKeyLen = aResultSize - ( ptr - vTmpPtr );
    memcpy( basekey, ptr, BaseKeyLen );

    // Освобождение памяти.
    free( vTmpPtr );

    // Выделение памяти.
    char* tmp = static_cast<char*>( malloc( fullsize ) );
    if( !tmp )
		ReturnWithError(rcErrorNoFreeMemory, "Can't allocate memory." );
    ptr = tmp;

    // Выборка всех частей из кэша.
    for( unsigned int i = 0; i < parts; i++ )
    {
        // Создание ключа.
        unsigned int KeyFullLen = BaseKeyLen + sprintf( basekey + BaseKeyLen, "%u", i );
        basekey[KeyFullLen] = '\0';

        // Получение данных.
        size_t Size;
        vTmpPtr = memcached_get( &vMemCached, basekey, KeyFullLen, &Size, &flags, &mr );
        if( mr != MEMCACHED_SUCCESS )
        {
            free( tmp );
            if( mr != MEMCACHED_NOTFOUND && mr != MEMCACHED_DATA_DOES_NOT_EXIST )
				ReturnWithError(rcErrorFault, "Can't get data from memcached( key: %s[ %u ] ). Memcached error: %s", basekey, KeyFullLen, memcached_strerror( &vMemCached, mr ) );
            else
                return rcUnsuccess;
        }
        aResultSize = Size;
        memcpy( ptr, vTmpPtr, aResultSize );

        // Освобождение памяти выделенной через libmemcached.
        free( vTmpPtr );

        ptr += aResultSize;
    }

    // Проверка корректности получения данных.
    if( (unsigned int)( ptr - tmp ) != fullsize )
    {
        free( tmp );

		ReturnWithWarn( rcUnsuccess, "Corrupted data in memcached: %u != %u", ptr - tmp, fullsize );
    }

    aResultSize = fullsize;

    aResult = tmp;

    return rcSuccess;
}

Cache::ReturnCode CacheMemcached::DeleteData( const char * aKey, unsigned int aKeySize )
{
    // Проверка состояния.
    if( vState == StateNotReady )
		ReturnWithError(rcErrorInvState, "Incorrect memcached object state: %d", vState );

    // Создание ключа.
    unsigned int KeyLen;
    const char * Key = MakeCtlKey( aKey, aKeySize, KeyLen );
    if( !Key )
		ReturnWithTrace( rcErrorNoFreeMemory );

    // Удаление значения из кэша.
    memcached_return mr = memcached_delete( &vMemCached, Key, KeyLen, 0 );
    if( mr != MEMCACHED_SUCCESS && mr != MEMCACHED_NOTFOUND )
		ReturnWithError(rcErrorFault, "Can't delete data from cache( key: %s [ %u ] )."
                         " Memcached error: %s", Key, KeyLen, memcached_strerror( &vMemCached, mr ) );

    return rcSuccess;
}

const char * CacheMemcached::MakeCtlKey( const char * aKey, unsigned int aKeySize, unsigned int& aCtlKeySize )
{
    aCtlKeySize = aKeySize + 4;

    // Выделение памяти под ключ.
    char* key = static_cast<char*>( vRuntimeAllocator.Allocate( aCtlKeySize + 1 ) );
    if( !key )
		ReturnWithError(nullptr, "Can't allocate memory." );

    // Создание ключа.
    memcpy( key, aKey, aKeySize );
    memcpy( key + aKeySize, "_ctl", 4 );
    key[aCtlKeySize] = '\0';

    return key;
}
