#ifndef __RawHashTable_hpp__
#define __RawHashTable_hpp__

#include <lib/aptl/avector.h>
#include "nMemoryAllocator.hpp"

class RawHashTable
{
private:    

    /** Внутрення структура, представляющая хранящийся в хэш-таблице элемент. */
    struct Node
    {
        Node( const unsigned long long & aKey, const char * aData, unsigned int aSize, bool aDataCopy )
            :vKey(aKey)
            ,vData(aData)
            ,vSize(aSize)
            ,vDataCopy(aDataCopy) {}


        /** Ключ (не уникальный). */
        const unsigned long long vKey;

        /** Данные. */
        const char * vData;

        /** Размер данных в байтах. */
        const unsigned int vSize;

        /** Признак того, что данные были скопированы внутрь хэш-таблицы */
        const bool vDataCopy;
    };

    /** Внутрення структура, для разрешения коллиций методом цепочек. */
    struct NodeIterator
    {
        /** Идентификатор текущего элемента. */
        unsigned int vId;

        /** Адрес следующего в цепочке итератора. */
        NodeIterator * vNext;
    };

public:
    /**
     *  Конструктор класса.
     * @param aInitialSize - начальный размер хэш-таблицы.
     * @param aCompletionRate - коэффициент (0..1] заполнения хэш-таблицы, определяющий момент увеличение её размера.
     */
    RawHashTable( unsigned int aInitialSize, double aCompletionRate );

    /** Деструктор класса. */
    ~RawHashTable();

    /** Возвращает истинное значение, если хэш-таблица инициализирована успешно и может быть использована. */
    bool IsOpen() const;

    /** Очищает хэш-таблицу. */
    void Clear();

    /**
     *  Вставляет в таблицу новый элемент и возвращает указатель на его идентификатор. В случае ошибки возвращает nullptr.
     * @param aSrc - указатель на вставляемый элемент.
     * @param aSize - размер вставляемого элемента в байтах.
     * @param aCopyData - флаг, определяющий, следует ли копировать вставляемый элемент.
     */
    const unsigned int * Insert( const char * aSrc, unsigned int aSize, bool aCopyData );

    /**
     *  Ищет в таблице элемент и возвращает указатель на его идентификатор. В случае отсутствтия элемента возвращает nullptr.
     * @param aSrc - указатель на искомый элемент.
     * @param aSize - размер искомого элемента в байтах.
     */
    const unsigned int * Search( const char * aSrc, unsigned int aSize ) const;

    /**
     *  Возвращает указатель на элемент по его идентификатору. В случае, если элемента с таким идентификатором не существует, возвращает nullptr.
     * @param aId - идентификатор элемента.
     */
    const char * GetItem( unsigned int aId) const;

    /**
     *  Возвращает указатель на размер элемента по его идентификатору. В случае, если элемента с таким идентификатором не существует, возвращает nullptr.
     * @param aId - идентификатор элемента.
     */
    const unsigned int * GetItemSize( unsigned int aId) const;

    /** Возвращает количество сохранённых в хэш-таблице элементов. */
    unsigned int Count() const;


    // Функции, возвращающие статистическую информацию о работе хэш-таблицы.
public:

    /** Возвращает количество случившихся коллизий. */
    unsigned int GetCollistiosCount() const;

    /** Возвращает максимальную длинну списка, разрешающего коллизии. */
    unsigned int GetMaxListLen() const;

    /** Возвращает среднюю длинну списков, разрешающих коллизии. */
    double GetAvgListLen() const;


private:

    void Init();

    void Resize();

    const unsigned int * SearchInSubList( const NodeIterator & aNodeIterator, const unsigned long long & aHashCode, const char * aSrc, unsigned int aSize ) const;

    const unsigned int * InsertInSubList( NodeIterator *& aNodeIterator, const unsigned long long & aHashCode, const char * aSrc, unsigned int aSize, bool vCopyData );

    void MakeHash( const char * aSrc, unsigned int aSize, unsigned long long & aHashCode ) const;

    unsigned int MakeN( const unsigned long long & aHashCode ) const;

private:

    /** Начальный размер хэш-таблицы. */
    unsigned int vInitialSize;

    /** Коэффициент заполнения хэш-таблицы. */
    double vCompletionRate;

    /** Хэш-таблица. */
    avector<NodeIterator*> vTable;

    /** Массив элементов хэш-таблицы. */
    avector<Node*> vNodes;

    /** Признак отсутсвия ошибок с момента последней инициализации. */
    bool vIsOpen;

    /** Переменная для хранения хэш-кода. */
    mutable unsigned long long vTmpHash;

    /** Менеджер памяти. */
    nMemoryAllocator vMemoryAllocator;

    /** Количество коллизий. */
    int vColl;

    /** Количество занятых ячеек в хэш-таблице. */
    int vFilled;

};

#endif /** __RawHashTable_hpp__ */

