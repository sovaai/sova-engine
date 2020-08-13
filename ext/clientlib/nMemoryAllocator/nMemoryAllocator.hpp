/**
 *  Аллокатор памяти предназначен для работы с многочисленными выделениями памяти, освобождение которой
 * происходит единомоментно. Аллокатор выделяет память большими кусками, после чего раздает ее на запросы
 * выделения памяти. В случае, когда вся память, выделенная аллокатором выбрана, аллокатор выделяет
 * дополнительный кусок памяти.
 */
#ifndef __nMemoryAllocator_hpp__
#define __nMemoryAllocator_hpp__

#include <stdio.h>

/**
 *  Класс для работы с многочисленными мелкими выделениями памяти при единомоментной ее очистке.
 */
class nMemoryAllocator
{
    public:
        /**
         *  Конструктор. Заметим, что при указании размеров секторов нужно учитывать, что кроме собственно памяти
         * выделяемой по запросу Allocate  в секторе будет хранится некоторое количество технической информации.
         * @param aFirstSectorSize - размер первого сектора выделения памяти. Если значение равно 0, то используется
         *                          значение aSectorSize.
         * @param aSectorSize - размер всех секторов, кроме первого.
         */
        nMemoryAllocator( size_t aFirstSectorSize = 0, size_t aSectorSize = 1024*1024 );
        /** Деструктор. */
        ~nMemoryAllocator()
                { Free(); }


    public:
        /**
         *  Смена размера секторов.
         * @param aSectorSize - новый размер сектора.
         */
        void SetMemoryStep( size_t aSectorSize );

        /**
         *  Выделение памяти.
         * @param aSize - нужный размер памяти.
         */
        void* Allocate( size_t aSize );

        /** Получение информации о общем объеме выделенной памяти. */
        size_t GetAllocatedMemorySize() const 
                { return vFullAllocatedMemorySize; }


    public:
        /** Очистка памяти. */
        void Reset();

        /** Освобождение памяти. */
        void Free();


    private:
        /** Ссылка на первый сектор памяти. */
        void* vFirstSector;

        /** Ссылка на текущий сектор памяти. */
        void* vCurrentSector;

        /** Ссылка на Свободную память. */
        void* vFreeMemory;

        /** Количество имеющейся свободной памяти в текущем секторе. */
        size_t vAllocatedMemory;

        /** Шаг выделения памяти. */
        size_t vFirstSectorSize;
        /** Шаг выделения памяти. */
        size_t vSectorSize;

        /** Объем выделенной памяти. */
        size_t vFullAllocatedMemorySize;
};

/**
 *  Вспомогательная структура и определение операторов new и delete для выделения с помощью nMemoryAllocator'а
 * памяти под объекты требующие инициализации конструкторами.
 */
typedef struct
{
} nMemoryAllocatorElementPtr;

inline void* operator new( size_t, nMemoryAllocatorElementPtr* pt ) { return pt; }

inline void operator delete( void*, nMemoryAllocatorElementPtr* ) { }


#define nAllocateObject( aAllocator, aClass ) static_cast<aClass*>( (aAllocator).Allocate( sizeof( aClass ) ))


#define nAllocateNewObject( aAllocator, aClass, aName ) \
                                          aClass* aName = static_cast<aClass*>( (aAllocator).Allocate( sizeof( aClass ) ))


#define nAllocateObjects( aAllocator, aClass, aNumber ) \
                                                  static_cast<aClass*>( (aAllocator).Allocate( sizeof( aClass )*aNumber ))

#define nAllocateNewObjects( aAllocator, aClass, aNumber, aName ) \
                                  aClass* aName = static_cast<aClass*>( (aAllocator).Allocate( sizeof( aClass )*aNumber ))

#endif /** __nMemoryAllocator_hpp__ */
