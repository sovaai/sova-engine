#ifndef __Signature_hpp__
#define __Signature_hpp__

#include <InfEngine2/_Include/Errors.h>
#include <NanoLib/LogSystem.hpp>
#include <lib/aptl/avector.h>
#include <lib/fstorage/fstorage.h>
#include <InfEngine2/_Include/FStorage.hpp>

class Signature
{
public:
    class Block
    {
    public:
        /**
         * Возвращает размер непрерывного участка памяти в байтах, необходимый для сохранения объекта.
         */
        virtual unsigned int GetNeedMemorySize() const = 0;
        
        /**
         * Сохраняет объект в заданном непрерывном участке памяти.
         * @param aBuffer - указатель участок памяти.
         * @param aSize - размер участка памяти в байтах.
         * @param aUsed - размер фактически использованного участка памяти в байтах.
         */
        virtual InfEngineErrors Save( char * aBuffer, unsigned int aSize, unsigned int & aUsed ) const = 0;
    };

    
    class BSignatureInfo: public Block
    {
    public:
        /**
         * Конструктор.
         * @param aVersion - версия сигнатуры.
         */        
		BSignatureInfo( unsigned int aVersion )
			{ vVersion =aVersion; }

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
                
    private:

        /** Версия сигнатуры. */
        unsigned int vVersion;
    };
    
public:    
    /**
     * Конструктор.
     * @param aVersion - версия сигнатуры.
     */
	Signature( unsigned int aVersion );
    
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
    
    /**
     * Сохраняет объект в fstorage.
     */
    InfEngineErrors Save( fstorage* aFStorage, fstorage_section_id aSectionID = FSTORAGE_SECTION_LD_SIGNATURE ) const;
    
public:
    
    InfEngineErrors AddBlock( const Block * aBlock);
    
protected:
    
    /** Блок с информацией о самой сигнатуре. */
    BSignatureInfo vSignatureInfo;
    
    /** Блоки с информацией. */
    avector<const Block*> vBlocks;
};

#endif // __Signature_hpp__
