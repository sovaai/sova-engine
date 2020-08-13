#ifndef __InfDictInstrInclude_hpp__
#define __InfDictInstrInclude_hpp__

#include "InfDictInstruction.hpp"

#include <NanoLib/LogSystem.hpp>

/**
 *  Класс для создания представления инструкции включения словаря в словарь - inlclude.
 */
class InfDictInstrInclude: public InfDictInstruction
{

public:
    /**
     *  Конструктор класса.
     * @param aDictId  - идентификатор включаемого словаря.
     */
    InfDictInstrInclude( unsigned int aDictId )
        :InfDictInstruction( InfDictInstruction::IT_INCLUDE )
        ,vDictID( aDictId )
    {}
    
    
    /** Получение размера памяти, необходимого для сохранения. **/
    unsigned int GetNeedMemorySize() const;

    /** Сохранение шаблон-словаря в буффер. **/
    InfEngineErrors Save( char* aBuffer, unsigned int aBufferSize, unsigned int& aResultMemory ) const;
    
    /** Получение идентификатора включаемого словаря. **/
    inline unsigned int GetDictID() const
        { return vDictID; }
        
private:
    
    // Идентиикатор включаемого словаря.
    unsigned int vDictID;
    
};


// Манипулятор для сохранённой интсрукции включения словаря.
class InfDictInstrIncludeManip
{
public:
    typedef InfDictInstruction::Type Type;
    
    // Создание манипулятора.
    InfDictInstrIncludeManip( const char * aBuffer = nullptr )
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
    
    // Возвращает идентификатор включаемого словаря.
    inline unsigned int GetDictID() const
        { return *reinterpret_cast<const uint32_t*>(vBuffer+1); }
private:
    const char * vBuffer;
};

#endif /** __InfDictInstrInclude_hpp__ */
