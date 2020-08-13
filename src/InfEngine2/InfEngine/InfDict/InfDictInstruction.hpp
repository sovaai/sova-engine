#ifndef __InfDictInstruction_hpp__
#define __InfDictInstruction_hpp__

#include <lib/aptl/avector.h>

#include <NanoLib/LogSystem.hpp>
#include <NanoLib/aTextString.hpp>

#include "../InfPattern/Items.hpp"

/**
 *  Класс для создания представления инструкций в словарях.
 */
class InfDictInstruction
{
public:
    
    // Типы инструкций.
    enum Type
    {
        // Инструкция включения другого словаря.
        IT_INCLUDE = 0,
    };
    
public:
    
    // Конструктор класса.
    InfDictInstruction( Type aType )
        :vType( aType )
    {}
        
    inline Type GetType() const
        { return vType; }
    
    /** Получение размера памяти, необходимого для сохранения. **/
    virtual unsigned int GetNeedMemorySize() const = 0;

    /** Сохранение шаблон-словаря в буффер. **/
    virtual InfEngineErrors Save( char* aBuffer, unsigned int aBufferSize, unsigned int& aResultMemory ) const = 0;
        
protected:
    
    // Тип инструкции.
    Type vType;
    
};


// Манипулятор для сохранённой инструкции словаря.
class InfDictInstructionManip
{
public:
    typedef InfDictInstruction::Type Type;
    
    // Создание манипулятора.
    InfDictInstructionManip( const char * aBuffer = nullptr )
        { Set(aBuffer); } 
    
    /**
     *  Инициализация манипулятора.
     * @param aBuffer - указатель на сохранённую инструкцию.
     */
    inline void Set( const char * aBuffer )
        { vBuffer = aBuffer; }
    
    // Возвращает тип инструкции.
    inline Type GetType() const
        { return (Type)*reinterpret_cast<const uint8_t*>(vBuffer); }
private:
    const char * vBuffer;
};

#endif /** __InfDictInstruction_hpp__ */
