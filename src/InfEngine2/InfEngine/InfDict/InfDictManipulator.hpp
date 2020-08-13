#ifndef __InfDictManipulator_hpp__
#define __InfDictManipulator_hpp__

#include <InfEngine2/_Include/Errors.h>

#include "InfDictStringManipulator.hpp"

class DLDataRO;

/**
 *  Класс для манипуляции образом словаря в памяти.
 */
class InfDictManipulator
{
    public:
        InfDictManipulator( const char * aBuffer = nullptr )
                : vBuffer( aBuffer )
        {}
 

    public:
        InfEngineErrors Attach( const char * aBuffer )
                { vBuffer = aBuffer; return vBuffer ? INF_ENGINE_SUCCESS : INF_ENGINE_ERROR_INV_ARGS; }
        
    public:
        /** Получение имени словаря. **/
        const char * GetName() const
                { return vBuffer ? vBuffer+((uint32_t*)vBuffer)[3]+sizeof( uint16_t ) : nullptr; }

        /** Утсновка идентификатора словаря.**/
        void SetID( unsigned int aId)
                { if( vBuffer ) ((uint32_t*)vBuffer)[1] = aId; }

        /** Получение идентификатора словаря.**/
        unsigned int GetID() const
                { return vBuffer ? ((uint32_t*)vBuffer)[1] : 0; }

        /** Получение количества строк в словаре, включая подсловари. **/
        unsigned int GetNum() const
                { return vBuffer ? ((uint32_t*)vBuffer)[10] : 0;}
        
        /** Получение количества строк в словаре, не считая подсловари. **/
        unsigned int GetSelfLinesNum() const 
                { return vBuffer ? ((uint32_t*)vBuffer)[2] : 0; }

        /** Получение указателя на aNum-ую строку. **/
        InfEngineErrors GetString( unsigned int aNum, InfDictStringManipulator& aDictString, const DLDataRO * aDLData, bool aGetMainString = false );
        
        /** Получение количества вариантов раскрытия словаря. **/
        unsigned int GetVariantsNumber() const
                { return vBuffer ? ((uint32_t*)vBuffer)[4] : 0; }
        
        
        /** Получение количества инструкций. **/
        unsigned int GetInstructionsNumber() const
                { return vBuffer ? ((uint32_t*)vBuffer)[6] : 0; }                
        
        /** Получение количетсва подсловарей. **/
        unsigned int GetSubdictsCount() const
                { return vBuffer ? ((uint32_t*)vBuffer)[8] : 0; }                
        
        /** Получение количества вариантов раскрытия заданной строки. **/
        unsigned int GetVariantsNumber( unsigned int aNum ) const
                { return vBuffer ? ((uint32_t*)(vBuffer + ((uint32_t*)vBuffer)[5]))[aNum] : 0; }
        
        /** Получения идентификаторв заданного подсловаря. */
        unsigned int GetSubDictID( unsigned int aNum )
                { return vBuffer ? ((uint32_t*)(vBuffer + GetSubdictsShift()))[aNum] : 0; }

    public:
        /** Получение памяти, необходимой для сохранения словаря. **/
        unsigned int GetSize() const
                { return vBuffer ? *((uint32_t*)vBuffer) : 0; }

        /** Сохранение словаря в буффер. **/
        InfEngineErrors Save( char* aBuffer, unsigned int aBufferSize, unsigned int& aResultSize ) const;
        
   private:
       
       /** Получение смещения к списку идентификаторов подсловарей. **/
        unsigned int GetSubdictsShift() const
                { return vBuffer ? ((uint32_t*)vBuffer)[9] : 0; }
        
        /** Получение смещения к таблице смещений инструкций. **/
        unsigned int GetInstructionsTableShift() const
                { return vBuffer ? ((uint32_t*)vBuffer)[7] : 0; }
    


    private:
        const char * vBuffer;
};

#endif /** __InfDictManipulator_hpp__ */
