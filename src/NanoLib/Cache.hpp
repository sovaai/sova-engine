#ifndef __Cache_hpp__
#define __Cache_hpp__

#include "LogSystem.hpp"
#include "nMemoryAllocator.hpp"

namespace NanoLib
{

    /**
     *  Базовый класс для работы с кэшем.
     */
    class Cache
    {
      public:

        /** Коды возврата операций с кэшем. */
        typedef enum
        {
            /** Неуспешное завершение операции, но не ошибка. */
            rcUnsuccess = 1,
            /** Успешное завершение операции. */
            rcSuccess = 0,
            /** Некорректные аргументы. */
            rcErrorInvArgs = -1,
            /** Некорректное состояние объекта. */
            rcErrorInvState = -2,
            /** Неклассифицированная ошибка работы кэша. */
            rcErrorFault = -3,
            /** Ошибка выделения памяти. */
            rcErrorNoFreeMemory = -4,
            /** Некорректная длина ключа. */
            rcErrorKey = -6,
        } ReturnCode;


      public:
        /**
         *  Добавление записи в кэш.
         * @param aKey - ключ, по которому нужно произвести добавление записи в кэш.
         * @param aKeySize - длина ключа.
         * @param aData - буфер с данными, который нужно закэшировать.
         * @param aDataSize - размер буфера с данными.
         */
        virtual ReturnCode InsertData( const char * aKey, unsigned int aKeySize,
                                       const char * aData, unsigned int aDataSize ) = 0;

        /**
         *  Получение записи из кэша. Выделенную под результат память должна удалять вызывающая сторона.
         * @param aKey - ключ, по которомунужно произвести поиск в кэше.
         * @param aKeySize - длина ключа.
         * @param aResult - ссылка на результат.
         * @param aResultSize - размер буфера результата.
         */
        virtual ReturnCode GetData( const char * aKey, unsigned int aKeySize, char*& aResult, unsigned int &aResultSize ) = 0;

        /**
         *  Удаление данных из кэша.
         * @param aKey - ключ, по которому нужно произвести удаление записи из кэша.
         * @param aKeySize - длина ключа.
         */
        virtual ReturnCode DeleteData( const char * aKey, unsigned int aKeySize ) = 0;


      protected:

        /** Состояние объекта. */
        enum
        {
            /** Объект не готов для использования. */
            StateNotReady,
            /** Объект готов для использования. */
            StateReady
		} vState = StateNotReady;

        /**
         *  Внутренний аллокатор памяти, используется для выделения внутренних данных и может быть
         * очищен при каждом внешнем вызове.
         */
        nMemoryAllocator vRuntimeAllocator;
    };
};

#endif /** __Cache_hpp__ */
