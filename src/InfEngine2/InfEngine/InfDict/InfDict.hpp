#ifndef __InfDict_hpp__
#define __InfDict_hpp__

#include <NanoLib/LogSystem.hpp>

#include <lib/aptl/avector.h>

#include <InfEngine2/_Include/Errors.h>

#include "InfDictSource.hpp"
#include "InfDictString.hpp"
#include "InfDictInstruction.hpp"

/**
 *  Класс для разбора словаря и сохранения и представления его в виде манипулятора.
 */
class InfDict
{
    public:                
        /** Получение необходимого размера памяти для сохранения объекта. **/
        unsigned int GetNeedMemorySize() const;

        /** Сохранение данных в буффер. **/
        InfEngineErrors Save( char* aBuffer, unsigned int aBufferSize, unsigned int& aResultSize ) const;

        /**
         *  Установить разобранные строки словаря.
         * @param aStrings - разобранные строки.
         * @param aStringsCount - количество строке.
         */
        void SetDictStrings( const InfDictString ** aStrings, unsigned int aStringsCount );
        
        /**
         *  Установить разобранные интсрукции словаря.
         * @param aInstructions - разобранные интсрукции.
         * @param aInstructionsCount - количество интсрукций.
         */
        void SetDictInstructions( const InfDictInstruction ** aInstructions, unsigned int aInstructionsCount );
        
        /**
         *  Установить список включённых словарей.
         * @param aSubdicts - список идентификаторов подсловарей.
         * @param aSubdictsCount - количество подсловарей.
         */
        void SetSubdicts( const unsigned int * aSubdicts, unsigned int aSubdictsCount );


        /**
         *  Установить название словаря.
         * @param aName - название словаря.
         * @param aNameLength - длина названия.
         */
        void SetName( const char * aName, unsigned int aNameLength );
        
        /** Получение имени словаря. */
        inline const char * GetName() const
                { return vName; }
        
        /** Получение длины имени словаря. */
        inline unsigned int GetNameLength() const
                { return vNameLength; }

        /**
         *  Установить индекс словаря.
         * @param aId - индекс словаря.
         */
        inline void SetID( unsigned int aId )
                { vDictID = aId; }
        
        /** Получение идентификатора словаря. */
        inline unsigned int GetID() const
                { return vDictID; }
        
        /** Получение количества строк в словаре без учёта подсловарей. */
        inline unsigned int GetStringsNum() const
                { return vStringsCount; }
        
        /** 
         *  Получение заданной строки словаре. 
         * @param aNum - номер строки.
         */
        inline unsigned int GetStringID( unsigned int aNum ) const
                { return vStrings[aNum]->GetId(); }
        
        /** Получение количества интсрукций в словаре. */
        inline unsigned int GetInstructionsNum() const
                { return vInstructionsCount; }
        
        /** 
         *  Получение заданной интсрукции. 
         * @param aNum - номер интсрукции.
         */
        inline const InfDictInstruction * GetInstruction( unsigned int aNum ) const
                { return vInstructions[aNum]; }
        
        /** Установка количества строк с учётом подсловарей. */
        inline void SetDisclosedStringsCount( unsigned int aCount )
                { vDisclosedStringsCount = aCount; }


    private:
        /** Название словаря. **/
        const char * vName;

        /** Длина названия словаря. **/
        unsigned int vNameLength;

        /** Идентификатор словаря. **/
        unsigned int vDictID;

        /** Разобранные строки словаря. **/
		const InfDictString ** vStrings = nullptr;
        
        /** Количество разобранных строк, не считая подсловари. **/
		unsigned int vStringsCount = 0;
        
        /** Количество разобранных строк, включая подсловари. **/
        unsigned int vDisclosedStringsCount;
        
        /** Разобранные инструкции словаря. **/
		const InfDictInstruction ** vInstructions = nullptr;
        
        /** Количество разобранных инструкций. **/
		unsigned int vInstructionsCount = 0;
        
        // Список идентификаторов включаемых подсловарей.
		const unsigned int * vSubdicts = nullptr;
        
        // Количество включаемых подсловарей.
		unsigned int vSubdictsCount = 0;
};

#endif /** __InfDict_hpp__ */
