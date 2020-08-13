#ifndef __Signature_Function_hpp__
#define __Signature_Function_hpp__

#include "Signature.hpp"
#include <InfEngine2/Functions/FunctionLib.hpp>

/** Структура с информацией о DL-функции. */
class Function: public Signature::Block
{
public:
    /**
     * Конструктор.     
     * @param aFucntion - описание DL-функции.
     */    
	Function( const DLFunctionInfo * aFucntion ):
		vFucntion( aFucntion ) { }
    
    
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
    /** Описание DL-функции. */
    const DLFunctionInfo * vFucntion;
};

#endif // __Signature_Function_hpp__
