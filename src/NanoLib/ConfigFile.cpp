#include "ConfigFile.hpp"
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/stat.h>


using namespace NanoLib;

const char * ConfigFile::ReturnCodeToString( ReturnCode aReturnCode ) const
{
    switch( aReturnCode )
    {
        case rcSuccess:
            return "Success";
        case rcErrorInvArg:
            return "Invalid arguments";
        case rcErrorFileAccess:
            return "Can't access to target file";
        case rcErrorNoFreeMemory:
            return "Can't allocate memory";
        case rcErrorFileRead:
            return "Can't read target file";
        case rcErrorInvFileFormat:
            return "Invalid file format";
        default:
            return nullptr;
    }
}

char* ParseName( char* aBuffer, unsigned int& aNameLength )
{
    // Выделение имени.
    aNameLength = 0;

    // Пропуск пробельных символов.
    for( ; *aBuffer == ' ' || *aBuffer == '\t'; aBuffer++ );

    // Выделение имени.
    for( ; ( 'a' <= *aBuffer && *aBuffer <= 'z' ) || ( 'A' <= *aBuffer && *aBuffer <= 'Z' ) ||
                                                     ( '0' <= *aBuffer && *aBuffer <= '9' ) || *aBuffer == ':'; aBuffer++, aNameLength++ );

    return aBuffer-aNameLength;
}

ConfigFile::ReturnCode ConfigFile::ParseFile( const char * aFilePath )
{
    // Проверка аргументов.
    if( !aFilePath )
        return rcErrorInvArg;

    // Сброс старых данных.
    Reset();

    // Открытие файла.
    int fd = open( aFilePath, 0 );
    if( fd == -1 )
        return rcErrorFileAccess;

    // Блокировка файла.
    if( flock( fd, LOCK_SH ) != 0 )
    {
        close( fd );
        return rcErrorFileAccess;
    }

    struct stat fs;
    if( fstat( fd, &fs ) != 0 )
    {
        close(fd);
        return rcErrorFileAccess;
    }

    // Выделение памяти.
    vFile.resize( fs.st_size+1 );
    if( vFile.no_memory() )
    {
        close(fd);
        return rcErrorNoFreeMemory;
    }

    // Считывание файла.
    if( read( fd, vFile.get_buffer(), fs.st_size ) != fs.st_size )
    {
        close(fd);
        return rcErrorFileRead;
    }
    vFile.get_buffer()[fs.st_size] = '\0';

    // Снятие блокировки.
    if( flock( fd, LOCK_UN ) != 0 )
    {
        close(fd);
        return rcErrorFileAccess;
    }

    // Закрытие файла.
    if( close(fd) != 0 )
        return rcErrorFileAccess;

    // Разбор файла.
    IntSection sect = { nullptr, 0, 0, 0 };

    char* ptr = vFile.get_buffer();
    while( *ptr != '\0' )
    {
        // Пропуск всех пробельных символов.
        for( ; *ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n'; ptr++ );

        // Проверка на конец файла.
        if( *ptr == '\0' )
            break;

        if( *ptr == '[' )
        {
            // Добавление предыдущей секции в список разобранных.
            if( sect.vName && sect.vFirstRecord < vRecords.size() )
            {
                vSections.push_back( sect );
                if( vSections.no_memory() )
                {
                    Reset();
                    return rcErrorNoFreeMemory;
                }
            }
            sect.vName = nullptr;

            // Новая секция.
            ptr++;

            // Выделение имени секции.
            sect.vName = ParseName( ptr, sect.vNameLength );
            if( !sect.vName )
            {
                Reset();

                return rcErrorInvFileFormat;
            }
            ptr += sect.vNameLength;

            // Пропуск пробельных символов.
            for( ; *ptr == ' ' || *ptr == '\t'; ptr++ );

            // Проверка наличия закрывающейся квадратной скобки.
            if( *ptr != ']' )
            {
                Reset();

                return rcErrorInvFileFormat;
            }
            ptr++;

            // Простановка символа конца имени.
            sect.vName[sect.vNameLength] = '\0';

            // Проверка отсутствия значимых символов после закрывающейся квадратной скобки.
            for( ; *ptr != '\n' && *ptr != '\r' && *ptr != '\0'; ptr++ )
            {
                if( *ptr != ' ' && *ptr != '\t' )
                {
                    Reset();

                    return rcErrorInvFileFormat;
                }
            }

            sect.vFirstRecord = vRecords.size();
            sect.vLastRecord = vRecords.size();
        }
        else if( *ptr == '/' )
        {
            ptr++;
            if( *ptr == '/' )
            {
                // Комментарий. Пропускаем строку.
                for( ; *ptr != '\n' && *ptr != '\r' && *ptr != '\0'; ptr++ );
            }
            else
            {
                Reset();

                return rcErrorInvFileFormat;
            }
        }
        else
        {
            // Новая запись.

            if( !sect.vName )
            {
                Reset();

                return rcErrorInvFileFormat;
            }

            Record rcd;
            rcd.vName = ParseName( ptr, rcd.vNameLength );
            if( !rcd.vName )
            {
                Reset();

                return rcErrorInvFileFormat;
            }
            ptr += rcd.vNameLength;

            // Пропуск пробельных символов.
            for( ; *ptr == ' ' || *ptr == '\t'; ptr++ );

            // Проверка наличия знака равенства.
            if( *ptr != '=' )
            {
                Reset();

                return rcErrorInvFileFormat;
            }
            ptr++;
            const_cast<char*>(rcd.vName)[rcd.vNameLength] = '\0';

            // Пропуск пробельных символов.
            for( ; *ptr == ' ' || *ptr == '\t'; ptr++ );

            rcd.vValue = ptr;

            // Поиск конца строки.
            for( ;; )
            {
                if( *ptr == '\r' || *ptr == '\n' )
                {
                    char* end = ptr-1;
                    for( ; *end == ' ' || *end == '\t'; end-- );
                    end++;
                    *end = '\0';
                    rcd.vValueLength = end-rcd.vValue;
                    ptr++;
                    break;
                }
                else if( *ptr == '\0' )
                {
                    char* end = ptr-1;
                    for( ; *end == ' ' || *end == '\t'; end-- );
                    end++;
                    *end = '\0';
                    rcd.vValueLength = end-rcd.vValue;
                    break;
                }
                else
                    ptr++;
            }

            // Добавление записи в секцию.
            vRecords.push_back( rcd );
            if( vRecords.no_memory() )
            {
                Reset();
                return rcErrorNoFreeMemory;
            }

            sect.vLastRecord = vRecords.size()-1;
        }
    }

    // Добавление предыдущей секции в список разобранных.
    if( sect.vName && sect.vFirstRecord < vRecords.size() )
    {
        vSections.push_back( sect );
        if( vSections.no_memory() )
        {
            Reset();
            return rcErrorNoFreeMemory;
        }
    }

    return rcSuccess;
}

const char * ConfigFile::GetSectionName( unsigned int aSectionNumber ) const
{
    if( aSectionNumber < vSections.size() )
        return vSections[aSectionNumber].vName;
    else
        return nullptr;
}

const char * ConfigFile::GetSectionName( unsigned int aSectionNumber, unsigned int& aNameLength ) const
{
    if( aSectionNumber < vSections.size() )
    {
        aNameLength = vSections[aSectionNumber].vNameLength;
        return vSections[aSectionNumber].vName;
    }
    else
    {
        aNameLength = 0;
        return nullptr;
    }
}

unsigned int ConfigFile::GetRecordsNumber( unsigned int aSectionNumber ) const
{
    if( aSectionNumber < vSections.size() )
    {
        return vSections[aSectionNumber].vLastRecord-vSections[aSectionNumber].vFirstRecord+1;
    }
    else
    {
        return 0;
    }
}

const ConfigFile::Record* ConfigFile::GetRecord( unsigned int aSectionNumber, unsigned int aRecordNumber ) const
{
    if( aSectionNumber < vSections.size() &&
        vSections[aSectionNumber].vFirstRecord+aRecordNumber <= vSections[aSectionNumber].vLastRecord )
    {
        return vRecords.get_buffer() + vSections[aSectionNumber].vFirstRecord+aRecordNumber;
    }
    else
    {
        return nullptr;
    }
}
