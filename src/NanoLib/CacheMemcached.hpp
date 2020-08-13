#ifndef __CacheMemcached_hpp__
#define __CacheMemcached_hpp__

#include <libmemcached/memcached.h>

#include "Cache.hpp"

namespace NanoLib
{
    /** Максимальная длина ключа. */
#    define NANOLIB_CACHE_MEMCACHED_KEY_MAX_LENGTH 210

    /**
     *  Хэш на основе memcached.
     */
    class CacheMemcached : public Cache
    {
      public:
        /** Деструктор. */
        ~CacheMemcached( );


      public:
        /** Инициализация объекта кэша. */
        ReturnCode Init( );

        /**
         *  Добавление сервера.
         * @param aServer - сервер.
         * @param aPort - порт.
         */
        ReturnCode AddServer( const char * aServer, unsigned int aPort );


      public:
        /**
         *  Добавление записи в кэш.
         * @param aKey - ключ, по которому нужно произвести добавление записи в кэш.
         * @param aKeySize - длина ключа.
         * @param aData - буфер с данными, который нужно закэшировать.
         * @param aDataSize - размер буфера с данными.
         */
        ReturnCode InsertData( const char * aKey, unsigned int aKeySize, const char * aData, unsigned int aDataSize );

        /**
         *  Получение записи из кэша. Выделенную под результат память должна удалять вызывающая сторона.
         * @param aKey - ключ, по которомунужно произвести поиск в кэше.
         * @param aKeySize - длина ключа.
         * @param aResult - ссылка на результат.
         * @param aResultSize - размер буфера результата.
         */
        ReturnCode GetData( const char * aKey, unsigned int aKeySize, char*& aResult, unsigned int &aResultSize );
        /**
         *  Удаление данных из кэша.
         * @param aKey - ключ, по которому нужно произвести удаление записи из кэша.
         * @param aKeySize - длина ключа.
         */
        ReturnCode DeleteData( const char * aKey, unsigned int aKeySize );


      public:

        /**
         *  Установка значения TTL.
         * @param aTTL - максимальное время жизни запись в кэше.
         */
        void SetTTL( time_t aTTL )
        {
            vTTL = aTTL;
        }

        /** Проверка инициализации. */
        bool IsInitialized( )
        {
            return vState == StateReady ? true : false;
        }


      protected:
        /**
         *  Создание управляющего ключа. Результат помещается в переменную vKey.
         * @param aKey - ключ.
         * @param aKeySize - длина ключа.
         * @param aCtlKeySize - длина управляющего ключа.
         */
        const char * MakeCtlKey( const char * aKey, unsigned int aKeySize, unsigned int& aCtlKeySize );


      protected:
        /** Максимальное время жизни добавляемых через класс данных в кэше. */
		time_t vTTL = 60 * 60;

        /** Объект кэша. */
        memcached_st vMemCached;
    };
};

#endif /** __CacheMemcached_hpp__ */
