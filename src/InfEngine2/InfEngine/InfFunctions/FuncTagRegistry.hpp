#ifndef __FuncTagRegistry_hpp__
#define __FuncTagRegistry_hpp__

#include <InfEngine2/InfEngine/InfPattern/Items/TagFunction.hpp>
#include <NanoLib/LogSystem.hpp>
#include <InfEngine2/_Include/FStorage.hpp>
#include <NanoLib/RawHashTable.hpp>
#include <NanoLib/nMemoryAllocator.hpp>
#include <InfEngine2/InfEngine/InfFunctions/FunctionsRegistry.hpp>

class FuncTagRegistry
{
private:
    struct Node
    {
        Node( const char * aData = nullptr, unsigned int aSize = 0)
            :vData( aData )
            ,vSize( aSize ) {}

        const char * vData;
        unsigned int vSize;
    };

    struct Cache
    {
        const char * vValue;
        unsigned int vLength;
        unsigned int vGeneration;
    };

public:
	FuncTagRegistry( bool aROMode = false );

    ~FuncTagRegistry();

    /** Создание рееста в ReadWrite режиме. */
    InfEngineErrors Create();

    /**
     *  Сохранение реестра в fstorage.
     * @param aFStorage - указатель на открытый fstorage.
     */
    InfEngineErrors Save( fstorage* aFStorage ) const;

    /**
     *  Открытие реестра из fstorage в ReadOnly режиме.
     * @param aFStorage - указатель на открытый fstorage.
     * @param aRootDir - корневой каталог для функций.
     * @param aConfigPath - путь к конфигурационному файлу функций.
     */
    InfEngineErrors Open( fstorage* aFStorage, const char * aRootDir, const char * aConfigPath );

    /** Закрытие реестра. */
    void Close();

    /**
     *  Регистрация тэга-функции в индексе.
     * @param aTagFunction - тип условия.
     */
    InfEngineErrors Registrate( const InfPatternItems::TagFunction * aTagFunction, unsigned int & aFuncTagId );

    /**
     *  Возвращает указатель на сохранённый тэг-функцию;
     * @param aFuncTagId - идентификатор тэга-функции.
     */
    const char * GetTagData( unsigned int aFuncTagId ) const;    

    /** Возвращает количество тэгов-функций в индексе в режиме чтения. */
    unsigned int CountRO() const;

    /** Возвращает ссылку на реестр функций. */
    const FunctionsRegistry & GetFunctionsRegistry() const;

    /**
     *  Регистрация в идексе внешней функции.
     * @param aFuncPath - путь к динамической библиотеки.
     * @param aFuncID -  идентификатор функции.
     */
    InfEngineErrors RegistryNewFunction( const char * aFuncPath, unsigned int & aFuncId );
    
    /**
     *  Регистрация в идексе внутренней функции.
     * @param aFuncInfo - информация о функции.
     * @param aFuncID -  идентификатор функции.
     */
    InfEngineErrors RegistryNewFunction( const DLFunctionInfo * aFuncInfo, unsigned int & aFuncId );

    // Функции, возвращающие статистическую информацию о индексе тэгов-функций.
public:

    /** Возвращает суммарный размер в байтах добавленных в индекс тэгов-функций до индексирования. */
    unsigned int GetRealSize() const;

    /** Возвращает размер в байтах тэгов-функций после индексирования. */
    unsigned int GetIndexedSize() const;

    /** Возвращает количество добавленных в индекс тэгов-функций. */
    unsigned int GetTagFunctionsCount() const;

    /** Возвращает количество уникальных добавленных в индекс тэгов-функций. */
    unsigned int GetUniqueTagFunctionsCount() const;

    /** Возвращает количество тэгов-функций, значение которых не может быть закэшировано. */
    unsigned int GetMultipleTagFunctionsCount() const;

    /** Возвращет максимальную длину цепочек в хэш-таблице. */
    unsigned int GetHashTableMaxListLen() const;

    /** Возвращет среднюю длину цепочек в хэш-таблице. */
    double GetHashTableAvgListLen() const;

private:
    /** Индекс в режими записи (хэш-таблица). */
    RawHashTable vTags;

    /** Индекс в режиме чтения. */
    avector<Node> vIndexRO;    

    /** Количество тэгов-функций.*/
    unsigned int vFunctionsNumber;

    /** Суммарный размер тэгов-функций в байтах.*/
    unsigned int vRealSize;

    /** Менеджер памяти. */
    nMemoryAllocator vMemoryAllocator;

    /** Кол-во тэгов-функций, которые могут принимать различные значения при одинаковых условиях. */
    unsigned int vMultipleCount;

    /** Реестр функций для записи. */
    FunctionsRegistryWR vFunctionsRegistryWR;

    /** Реестр функций для чтения. */
    FunctionsRegistryRO vFunctionsRegistryRO;

    /** Флаг режима только для чтения */
    bool vROMode;

};

#endif
