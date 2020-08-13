#ifndef __Alias_hpp__
#define __Alias_hpp__

#include <InfEngine2/Functions/FunctionLib.hpp>
#include <NanoLib/aTextString.hpp>
#include <lib/aptl/avector.h>
#include <InfEngine2/_Include/Errors.h>
#include <NanoLib/nMemoryAllocator.hpp>
#include <NanoLib/NameIndex.hpp>


// Класс для представления алиаса.
class Alias
{
private:

    Alias( const Alias & ) {}
    Alias & operator=( const Alias& ) { return *this; }

public:

    enum ResultType { UNKNOWN, TEXT, BOOL };

    /**
     *  Конструктор класса.
     */
    Alias();

    /**
     *  Задать тип возвращаемого результата.
     * @param aResultType - тип возвращаемого результата.
     */
    void SetType( ResultType aResultType );

    /**
     *  Задать имя алиаса.
     * @param aName - имя алиаса.
     * @param aNameLength - длина имени алиаса.
     */
    InfEngineErrors SetName( const char * aName, unsigned int aNameLength  );

    /**
     *  Зарегистрировать аргумент алиаса.
     * @param aArgumentName - имя нового аргумента.
     * @param DLFunctionArgType - тип нового аргумента.
     */
    InfEngineErrors RegisterArgument( const char * aArgumentName, DLFunctionArgType aArgumentType,
                                     const char ** aValidValues = nullptr, unsigned int aValidValuesCount = 0,
                                     const char * aDefaultValue = nullptr, unsigned int aDefaultValueLen = 0);

    /**
     *  Добавить элемент-текст.
     * @param aText - текст.
     */
    InfEngineErrors AddTextItem( const char * aText );

    /**
     *  Добавить элемент-текст.
     * @param aText - текст.
     * @param aTextLength - длина текста.
     */
    InfEngineErrors AddTextItem( const char * aText, unsigned int aTextLength );

    /**
     *  Добавить ссылку на аргумент.
     * @param aArgumentName - имя аргумента.
     */
    InfEngineErrors AddVariableItem( const char * aArgumentName, unsigned int aArgumentNameLength );

    /**
     *  Возвращает тип результата, возвращаемого алиасом значения.
     */
    ResultType GetResultType() const;

    /**
     *  Возвращает имя алиаса.
     */
    const char * GetName() const;

    /**
     *  Возвращает количество аргументов алиаса.
     */
    unsigned int GetArgumentsCount() const;
    
    /**
     *  Возвращает минимальное необходимое количество аргументов алиаса (без значений по умолчанию).
     */
    unsigned int GetMinArgumentsCount() const;

    /**
     *  Возвращает тип аргумента по его номеру.
     * @param aArgN - номер аргумента.
     */
    const DLFunctionArgType * GetArgumentType( unsigned int aArgN ) const;

    /**
     *  Возвращает тип аргумента по его имени.
     * @param aArgName - имя аргумента.
     */
    const DLFunctionArgType * GetArgumentType( const char * aArgName ) const;

    /**
     *   Возвращет номер аргумента по его имени или -1, если аргумента с таким именем не существует.
     * @param aArgName - имя аргумента.
     */
    const unsigned int * GetArgumentN( const char * aArgName ) const;

    /**
     *   Возвращает количество значений, которые может принимать аргумент (только для аргументов типа TEXT).
     * @param aArgN - номер аргумента.
     */
    unsigned int GetArgumentValidValuesCount( unsigned int aArgN )  const;

    /**
     *   Возвращает список значений, которые может принимать аргумент (только для аргументов типа TEXT).
     * @param aArgN - номер аргумента.
     */
    const char ** GetArgumentValidValues( unsigned int aArgN ) const;

    /**
     *  Раскрытие алиаса.
     * @param aArgVal - список аргументы.
     * @param aArgLen - список длин аргументов.
     * @param aArgCount - количество аргументов.
     * @param aMemoryAllocator - менеджер памяти.
     * @param aResult - строка с результатом раскрытия.
     */
    InfEngineErrors Apply( const char ** aArgVal, const unsigned int * aArgLen, unsigned int aArgCount,
                          nMemoryAllocator & aMemoryAllocator, char *& aResult, unsigned int & aResultLength ) const;

private:

    // Тип элементов, описывающих аргументы алиаса.
    struct Argument;

    // Тип элементов, составляющих алиас.
    struct Item;

    // тип возвращаемого результата.
    ResultType vResultType;

    // Имя алиаса.
    aTextString vName;

    // Идекс имён аргументов.
    NanoLib::NameIndex vArgIndex;

    // Список типов аргументов.
    avector<Argument> vArguments;
    
    // Минимальное необходимое количество аргументов (без значений по умолчанию).
    unsigned int vMinArgsCount;

    // Элементы, составляющие алиас.
    avector<Item> vItems;

    // Суммарная длина текстовых элементов.
    unsigned int vTextLength;

};

// Тип элементов, описывающих аргументы алиаса.
struct Alias::Argument
{
    Argument( DLFunctionArgType aType = DLFAT_TEXT, const char ** aValidValues = nullptr, unsigned int aValidValuesCount = 0,
                const char * aDefaultValue = nullptr, unsigned int aDefaultValueLen = 0 )
        :vType( aType )
        ,vValidValues( aValidValues )
        ,vValidValuesCount( aValidValuesCount )
        ,vDefaultValue( aDefaultValue )
        ,vDefaultValueLen( aDefaultValueLen ) {}

    inline bool IsValid( const char * aValue, unsigned int aValueLength ) const
    {
        if( !vValidValues )
            return true;

        for( unsigned int val_n = 0; val_n < vValidValuesCount; ++val_n )
            if( !strncmp( aValue, vValidValues[val_n], aValueLength ) )
                return true;

        return false;
    }

    // Тип аргумента.
    DLFunctionArgType vType;

    // Список значений, которые может принимать аргумент (только для аргументов  типа TEXT).
    const char ** vValidValues;

    // Длина списка значений, которые может принимать аргумент (только для аргументов  типа TEXT).
    unsigned int vValidValuesCount;
    
    // Значение по умолчанию.
    const char * vDefaultValue;
    
    // Длина значения по умолчанию.
    unsigned int vDefaultValueLen;
};

// Тип элементов, составляющих алиас.
struct Alias::Item
{
    // Тип элемента.
    enum Type
    {
        // Текст.
        TEXT,

        // Ссылка на ргумент.
        ARGUMENT
    } vType;

    // Тело элемента.
    union
    {
        // Номер аргумента.
        unsigned int vNum;

        // Тестовый элемент.
        struct
        {
            // Текстовая строка.
            const char * vData;

            // Длина строки.
            unsigned int vLength;
        } vText;
    };
};

#endif // __Alias_hpp__
