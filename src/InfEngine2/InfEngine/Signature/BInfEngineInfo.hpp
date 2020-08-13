#ifndef __BInfEngineInfo_hpp__
#define __BInfEngineInfo_hpp__

#include "Signature.hpp"
#include "NanoLib/aTextString.hpp"


/** Блок с информацией о InfEngine. */
class BInfEngineInfo: public Signature::Block
{
public:
    /**
     * Конструктор.     
     * @param aInfDataProtoVer - версия формата базы шаблонов.
     * @param aMinInfDataProtoVer - минимальная совместимая версия формата базы шаблонов.
     * @param aLingProcVer - версия LingProc.
     * @param aTag - название тэга.
     */    
	BInfEngineInfo( unsigned int aInfDataProtoVer, unsigned int aMinInfDataProtoVer, unsigned int aLingProcVer, const char * aTag ):
		vInfDataProtoVer( aInfDataProtoVer ),
		vMinInfDataProtoVer( aMinInfDataProtoVer ),
		vLingProcVer( aLingProcVer )
    {
        vTag.assign( aTag );
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
    
    /** Версия формата базы шаблонов. */
    unsigned int vInfDataProtoVer;
    /** Минимальная совместимая версия формата базы шаблонов. */
    unsigned int vMinInfDataProtoVer;
    /** Версия LingProc. */
    unsigned int vLingProcVer;
    /** Название тэга. */
    aTextString vTag;
};


#endif // __BInfEngineInfo_hpp__
