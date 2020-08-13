#ifndef __ConfigNew_hpp__
#define __ConfigNew_hpp__

#include <lib/aptl/avector.h>

namespace NanoLib
{
    /**
     *  Класс для разбора конфигурационных файлов.
     */
    class ConfigFile
    {
        public:
            /** Коды возврата функций-членов класса. */
            enum ReturnCode
            {
                /** Успешное выполнение функции. */
                rcSuccess = 1,

                /** Некорректные аргументы функции. */
                rcErrorInvArg = -1,

                /** Ошибка доступа к файлу. */
                rcErrorFileAccess = -2,

                /** Ошибка выделения памяти. */
                rcErrorNoFreeMemory = -3,

                /** Ошибка чтения файла. */
                rcErrorFileRead = -4,

                /** Некорректный формат файла. */
                rcErrorInvFileFormat = -5
            };

            /**
             *  Получение текстового описания кода возврата.
             * @param aReturnCode - код возврата.
             */
            const char * ReturnCodeToString( ReturnCode aReturnCode ) const;

        public:
            /**
             *  Разбор конфигурационного файла.
             * @param aFilePath - путь к конфигурационному файлу.
             */
            ReturnCode ParseFile( const char * aFilePath );


        public:
            /** Получение числа секций в конфигурационном файле. */
            unsigned int GetSectionsNumber() const
                    { return vSections.size(); }

            /**
             *  Получение имени секции.
             * @param aSectionNumber - номер требуемой секции.
             * @param aNameLength - длина имени секции.
             */
            const char * GetSectionName( unsigned int aSectionNumber ) const;
            const char * GetSectionName( unsigned int aSectionNumber, unsigned int& aNameLength ) const;

            

            /**
             *  Получение количества записей в секции.
             * @param aSectionNumber - номер требуемой секции.
             */
            unsigned int GetRecordsNumber( unsigned int aSectionNumber ) const;

            /** Описание записи. */
            typedef struct
            {
                /** Имя записи. */
                const char * vName;
                /** Длина имени записи. */
                unsigned int vNameLength;
                /** Значение записи. */
                const char * vValue;
                /** Длина значения записи. */
                unsigned int vValueLength;
            } Record;

            /** Секция конфигурационного файла. */
            class Section
            {
                public:
                    Section( const ConfigFile& aConfigFile, unsigned int aSectionId ): vConfigFile( aConfigFile ), vSectionId(aSectionId) {}


                public:
                    unsigned int GetRecordsNumber() const
                            { return vConfigFile.GetRecordsNumber( vSectionId ); }

                    const Record* GetRecord( unsigned int aRecordId ) const
                            { return vConfigFile.GetRecord( vSectionId, aRecordId ); }


                private:
                    const ConfigFile& vConfigFile;
                    const unsigned int vSectionId;
            };
            const Section GetSection( unsigned int aSectionId ) const
                    { Section NewSection( *this, aSectionId ); return NewSection; }

            /**
             *  Получение записи.
             * @param aSectionNumber - номер требуемой секции.
             * @param aRecordNumber - номер требуемой записи в секции.
             */
            const Record* GetRecord( unsigned int aSectionNumber, unsigned int aRecordNumber ) const;


        private:
            /** Очистка внутренних данных. */
            void Reset()
                { vFile.clear(); vSections.clear(); vRecords.clear(); }

        private:
            /** Данные конфигурационного файла. */
            avector<char> vFile;

            /** Внутреннее представление секции. */
            typedef struct
            {
                /** Имя секции. */
                char* vName;
                /** Длина имени секции. */
                unsigned int vNameLength;
                /** Номер первой записи, относящейся к секции. */
                unsigned int vFirstRecord;
                /** Номер последней записи, относящейся к секции. */
                unsigned int vLastRecord;
            } IntSection;
            /** Массив описаний секций. */
            avector<IntSection> vSections;
            /** Массив описаний записей. */
            avector<Record> vRecords;
    };
};

#endif  /** __ConfigNew_hpp__ */
