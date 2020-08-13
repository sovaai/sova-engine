#ifndef __BFunctions_hpp__
#define __BFunctions_hpp__

#include "Signature.hpp"
#include "Function.hpp"

/** Блок с информацией о системе функций InfEngine. */
class BFunctions: public Signature::Block
{
public:
    /**
     * Конструктор.     
     * @param aDLFuncInterfaceVer - версия интерфейса DL-функций.
     * @param aMinDLFuncInterfaceVer - минимальная совместимая версия интерфейса DL-функций.
     * @param aFunctionsNumber - количество внутренних функций.
     * @param aFucntions - список функций.
     */    
	BFunctions( unsigned int aDLFuncInterfaceVer, unsigned int aMinDLFuncInterfaceVer, unsigned int aFunctionsNumber, const Function ** aFunctions ):
		vDLFuncInterfaceVer( aDLFuncInterfaceVer ),
		vMinDLFuncInterfaceVer( aMinDLFuncInterfaceVer ),
		vFunctionsNumber( aFunctionsNumber ),
		vFunctions( aFunctions )
    {
    }
    
    
    /**
     * Возвращает размер непрерывного участка памяти в байтах, необходимый для сохранения объекта.
     */
    unsigned int GetNeedMemorySize() const;

    /**
     * Сохраняет объект в заданном непрерывном участке памяти.
     * @param aBuffer - указатель участок памяти.
     * @param aSize - размер участка памяти в байтах.
     * @param aUsed - размер фактически использованного участка памяти в байтах.
     */
    InfEngineErrors Save( char * aBuffer, unsigned int aSize, unsigned int & aUsed ) const;
            
protected:
    
    /** Версия интерфейса DL-функций. */
    unsigned int vDLFuncInterfaceVer;    
    
    /** Минимальная совместимая версия интерфейса DL-функций. */
    unsigned int vMinDLFuncInterfaceVer;
    
    /** Количество внутренних функций. */
    unsigned int vFunctionsNumber;
    
    /** Список функций. */
    const Function ** vFunctions;
};


#endif // __BFunctions_hpp__
