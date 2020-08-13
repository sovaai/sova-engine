#include "InternalFunction.hpp"
#include "CalcTime.hpp"
#include "InfEngine2/_Include/Errors.h"
#include <NanoLib/aTextString.hpp>

using namespace InternalFunctions;

const DLFucntionArgInfo CalcTime::ARGS[] = {
    { DLFAT_EXTENDED, "" },
    { DLFAT_EXTENDED, "" },
    { DLFAT_EXTENDED, "" },
    { DLFAT_EXTENDED, "" }
};

const DLFunctionInfo CalcTime::DLF_INFO = {
    "CalcTime",
    "",
    "Returns time obtained by applying given offset to given local time.\nExample: [@CalcTime( \"\", \"+00.01.0000\", \"\", \"08:15\") ].\nIt means \"In one month at 8:15 a.m.\"",
    DLFRT_TEXT,
    4,
    ARGS,
    { 1, 0, 0 },
    { 1, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,
    InfDataProtocolVersion,
    nullptr
};

ExtICoreRO * CalcTime::vBase = nullptr;

InfEngineErrors ParseDate( const char * aStr, unsigned int aLength, unsigned int & aPos, 
        int & aDay, int & aMonth, int & aYear )
{      
    aDay = 0;
    aMonth = 0;
    aYear = 0;
    unsigned int pos = aPos;
    bool date_is_short = false;
    unsigned int day = 0;    
    unsigned int month = 0;    
    unsigned int year = 0;
    if( !TextFuncs::ParseDate( aStr, aLength, pos, day, month, year, date_is_short, false, true ) || date_is_short )
        return INF_ENGINE_ERROR_CANT_PARSE;
    
    aDay = day;
    aMonth = month;
    aYear = year;
    aPos = pos;
    
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors ParseTime( const char * aStr, unsigned int aLength, unsigned int & aPos, 
        int & aHour, int & aMinute, int &  aSecond )
{
    aHour = 0;
    aMinute = 0;
    aSecond = 0;
    
    unsigned int pos = aPos;
    bool time_is_short = false;
    
    unsigned int hour = 0;    
    unsigned int minute = 0;    
    unsigned int second = 0;
    if( !TextFuncs::ParseTime( aStr, aLength, pos, hour, minute, second, time_is_short, false, true ) )
        return INF_ENGINE_ERROR_CANT_PARSE;
    
    aHour = hour;
    aMinute = minute;
    aSecond = second;
    aPos = pos;
    
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors ParseDateAndOrTime( const char * aStr, unsigned int aLength, 
        int & aYear, int & aMonth, int & aDay, int & aHour, int & aMinute, int & aSecond,
        bool & aDateParsed, bool & aTimeParsed  )
{
    aYear = 0;
    aMonth = 0;
    aDay = 0;
    aHour = 0;
    aMinute = 0;
    aSecond = 0;
    
    // Пропускаем пробелы.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, 0, aLength );
    if( pos >= aLength )
        return INF_ENGINE_ERROR_CANT_PARSE;
            
    // Пропускаем пробелы.
    pos = TextFuncs::SkipSpaces( aStr, pos, aLength );
    if( pos >= aLength )
        return INF_ENGINE_ERROR_CANT_PARSE;
    
    // Считываем дату.
    InfEngineErrors iee = ParseDate( aStr, aLength, pos, aDay, aMonth, aYear );
    if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
        return iee;
    aDateParsed = INF_ENGINE_SUCCESS == iee;
    
    // Пропускаем пробелы.
    pos = TextFuncs::SkipSpaces( aStr, pos, aLength );
        
    // Считываем время.
    iee = ParseTime( aStr, aLength, pos, aHour, aMinute, aSecond );
    if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
        return iee;
    aTimeParsed = INF_ENGINE_SUCCESS == iee;
    
    // Пропускаем пробелы.
    pos = TextFuncs::SkipSpaces( aStr, pos, aLength );
    
    // Не удалось разобрать строку.
    if( !aTimeParsed && !aDateParsed )
        return INF_ENGINE_ERROR_CANT_PARSE;
    
    // Возможно, время стоит перед датой.
    if( aTimeParsed && !aDateParsed )
    {
        // Считываем дату.
        iee = ParseDate( aStr, aLength, pos, aDay, aMonth, aYear );
        if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
            return iee;
        aDateParsed = INF_ENGINE_SUCCESS == iee ;
    }
    
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors ParseTimeOffsetFull( const char * aStr, unsigned int aLength, 
        int & aYear, int & aMonth, int & aDay, int & aHour, int & aMinute, int & aSecond )
{
    aYear = 0;
    aMonth = 0;
    aDay = 0;
    aHour = 0;
    aMinute = 0;
    aSecond = 0;
    
    // Пропускаем пробелы.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, 0, aLength );
    if( pos >= aLength )
        return INF_ENGINE_ERROR_CANT_PARSE;
        
    // Определяем знак сдвига - положительный или отрицательный.    
    if( aStr[pos] != '-' &&  aStr[pos] != '+' ) 
        return INF_ENGINE_ERROR_CANT_PARSE;
    bool is_negated = aStr[pos] == '-';
    ++pos;     
    
    bool date_parsed = false;
    bool time_parsed = false;
    InfEngineErrors iee = ParseDateAndOrTime( aStr + pos, aLength - pos, aYear, aMonth, aDay, aHour, aMinute, aSecond, date_parsed, time_parsed );
    if( iee != INF_ENGINE_SUCCESS )
        return iee;
        
    if( is_negated )
    {
        aYear *= -1;
        aMonth *= -1;
        aDay *= -1;
        aHour *= -1;
        aMinute *= -1;
        aSecond *= -1;
    }
    
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors ParseTimeOffsetShort( const char * aStr, unsigned int aLength,
        int & aYear, int & aMonth, int & aDay, int & aHour, int & aMinute, int & aSecond )
{
    aYear = 0;
    aMonth = 0;
    aDay = 0;
    aHour = 0;
    aMinute = 0;
    aSecond = 0;
    
    bool years_parsed = false;
    bool months_parsed = false;
    bool days_parsed = false;
    bool hours_parsed = false;
    bool minutes_parsed = false;
    bool seconds_parsed = false;
    
    unsigned int pos = 0;
    while( pos < aLength )
    {
        // Пропускаем пробелы.
        pos = TextFuncs::SkipSpaces( aStr, pos, aLength );
        if( pos < aLength )
        {
            // Определяем знак сдвига - положительный или отрицательный.
            if( aStr[pos] != '-' && aStr[pos] != '+' )
                return INF_ENGINE_ERROR_CANT_PARSE;
            bool is_negated = aStr[pos] == '-';            
            ++pos;
            
            // Пропускаем пробелы.
            pos = TextFuncs::SkipSpaces( aStr, pos, aLength );
            
            // Считываем численное значение сдвига.
            if( !TextFuncs::IsDigit( aStr[pos] ) )
                return INF_ENGINE_ERROR_CANT_PARSE;
            unsigned int end = pos;
            long value = TextFuncs::GetUnsigned( aStr, aLength, end );
            if( pos == end )
                return INF_ENGINE_ERROR_CANT_PARSE;
            pos = end;
            
            // Пропускаем пробелы.
            pos = TextFuncs::SkipSpaces( aStr, pos, aLength );
            
            // Считываем тип смещениея - дни, часы или минуты.
            switch( aStr[pos] )
            {
                // Года.
                case 'y':
                    if( years_parsed )
                        return INF_ENGINE_ERROR_CANT_PARSE;
                    years_parsed = true;
                    aYear = value * ( is_negated ? -1 : 1 );
                    break;
                
                // Месяцы.
                case 'M':
                    if( months_parsed )
                        return INF_ENGINE_ERROR_CANT_PARSE;
                    months_parsed = true;
                    aMonth = value * ( is_negated ? -1 : 1 );
                    break;
                
                // Дни.
                case 'd':
                    if( days_parsed )
                        return INF_ENGINE_ERROR_CANT_PARSE;
                    days_parsed = true;
                    aDay = value * ( is_negated ? -1 : 1 );
                    break;
                
                // Часы.
                case 'h':
                    if( hours_parsed )
                        return INF_ENGINE_ERROR_CANT_PARSE;
                    hours_parsed = true;
                    aHour = value * ( is_negated ? -1 : 1 );
                    break;
                    
                // Минуты.    
                case 'm':
                    if( minutes_parsed )
                        return INF_ENGINE_ERROR_CANT_PARSE;
                    minutes_parsed = true;
                    aMinute = value * ( is_negated ? -1 : 1 );
                    break;
                                        
                default:
                    return INF_ENGINE_ERROR_CANT_PARSE;
            }
            ++pos;
        }
    }
    
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors ParseTimeOffset( const char * aStr, unsigned int aLength, 
        int & aYear, int & aMonth, int & aDay, int & aHour, int & aMinute, int & aSecond )
{
    unsigned int pos = TextFuncs::SkipSpaces( aStr, 0 ,aLength );
    
    // Пустая строка.
    if( pos >= aLength )
    {
        aYear = 0;
        aMonth = 0;
        aDay = 0;
        aHour = 0;
        aMinute = 0;
        aSecond = 0;
        
        return INF_ENGINE_SUCCESS;
    }
    
    // Пробуем разобрать полный формат.
    InfEngineErrors iee = ParseTimeOffsetFull( aStr, aLength, aYear, aMonth, aDay, aHour, aMinute, aSecond );
    if( iee == INF_ENGINE_SUCCESS )
        return iee;
    if( iee != INF_ENGINE_ERROR_CANT_PARSE )
        return iee;
    
    // Разбираем краткий формат.
    return ParseTimeOffsetShort( aStr, aLength, aYear, aMonth, aDay, aHour, aMinute, aSecond );   
}

InfEngineErrors ParseCurrentTime( const char * aStr, unsigned int aLength, 
        int & aYear, int & aMonth, int & aDay, int & aHour, int & aMinute, int & aSecond, const Session * aSession )
{
    bool date_parsed = false;
    bool time_parsed = false;
    // Пропускаем ведущие пробелы.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, 0, aLength );
    if( pos < aLength )
    {
        InfEngineErrors iee = ParseDateAndOrTime( aStr, aLength, aYear, aMonth, aDay, aHour, aMinute, aSecond, date_parsed, time_parsed );
        if( iee != INF_ENGINE_SUCCESS )
            return iee;        
    }
    
    if( !date_parsed || !time_parsed )
    {
        // Получение текущего времени.
        time_t Timer;
        time( &Timer );

        // Изменение времени с учётом текущего часового пояса.
        if( aSession )
        {
            // Получаем часовой пояс из переменной в текущей сессии.
            const Vars::Vector::VarValue var_val = aSession->GetValue( InfEngineVarUserTimeZone );
            if( var_val )
            {
                const Vars::Vector::VarText var_value = var_val.AsText();
                if( var_value )
                {
                    unsigned int var_len = 0;
                    const char * value = var_value.GetValue( var_len );            

                    // Разбираем значение часового пояса, записанное в переменной user_timezone.
                    if( value )
                    {                
                        unsigned int pos = 0;
                        int time_zone_in_seconds = TextFuncs::GetTimeZone( value, var_len, pos );                
                        if( pos )
                            Timer += time_zone_in_seconds;
                    }
                }
            }
        } 

        // Преобразование времени из формата в виде секунд с 1970 года в формат в виде даты.
        struct tm* TimeStruct = localtime( &Timer );
        if( !TimeStruct )
            return INF_ENGINE_WARN_UNSUCCESS;

        // Если локальное время сервера не UTC+4:00 (Москва), то делаем поправку на это.
        if( TimeStruct->tm_gmtoff != 14400 )
            Timer += 14400 - TimeStruct->tm_gmtoff;
        
        // Преобразование времени из формата в виде секунд с 1970 года в формат в виде даты.
        struct tm * t = localtime( &Timer );
        if( !t )
            return INF_ENGINE_WARN_UNSUCCESS;
        
        if( !date_parsed )
        {
            aYear = t->tm_year + 1900;
            aMonth = t->tm_mon + 1;
            aDay = t->tm_mday;
        }
        if( !time_parsed )
        {
            aHour = t->tm_hour;
            aMinute = t->tm_min;
            aSecond = t->tm_sec;
        }               
    }
    
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors ParseDataOutputFormat( const char * aStr, unsigned int aLength, const char *& aFormat )
{    
    unsigned int pos = TextFuncs::SkipSpaces( aStr, 0, aLength );
    
    if( pos >= aLength )
    {
        static const char EMPTY[] = "";
        aFormat = EMPTY;
        return INF_ENGINE_SUCCESS;
    }
 
    // Проверяем формат даты на предопределённое значение.
    if( strncasecmp( aStr + pos, "HIDE", 4 ) )
    {    
        // Считываем дату.
        int day = 0;
        int month = 0;
        int year = 0;
        InfEngineErrors iee = ParseDate( aStr, aLength, pos, day, month, year );
        if( iee != INF_ENGINE_SUCCESS )
            return iee;
        
        // Нормализуем формат даты.
        struct tm t;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_mday = day;
        t.tm_mon = month - 1;
        t.tm_year = year < 100 ? year : year - 1900;
        t.tm_isdst = 0;                
        static char DateBuffer[16];
        if( !strftime( DateBuffer, 15, "%d.%m.%Y", &t ) )
            return INF_ENGINE_ERROR_INTERNAL;
        aFormat = DateBuffer;
    }
    else
        aFormat = nullptr;
    
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors ParseTimeOutputFormat( const char * aStr, unsigned int aLength, const char *& aFormat )
{
    unsigned int pos = TextFuncs::SkipSpaces( aStr, 0, aLength );    
    if( pos >= aLength )
    {
        static const char EMPTY[] = "";
        aFormat = EMPTY;
        return INF_ENGINE_SUCCESS;
    }
 
    // Проверяем формат времени на предопределённое значение.
    if( strncasecmp( aStr + pos, "HIDE", 4 ) )
    {    
        // Считываем время.
        int hour = 0;
        int minute = 0;
        int second = 0;
        InfEngineErrors iee = ParseTime( aStr, aLength, pos, hour, minute, second );
        if( iee != INF_ENGINE_SUCCESS )
            return iee;
        
        // Нормализуем формат времени.
        struct tm t;
        t.tm_sec = second;
        t.tm_min = minute;
        t.tm_hour = hour;
        t.tm_mday = 1;
        t.tm_mon = 1;
        t.tm_year = 1;
        t.tm_isdst = 0;                
        static char DateBuffer[16];
        if( !strftime( DateBuffer, 15, "%H:%M", &t ) )
            return INF_ENGINE_ERROR_INTERNAL;
        aFormat = DateBuffer;
    }
    else
        aFormat = nullptr;
    
    return INF_ENGINE_SUCCESS;
}

InfEngineErrors CalcTime::Apply(const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * aMemoryAllocator, 
        FunctionContext * aFunctionContext )
{
    // Проверка аргументов.    
    if( aArgs[0] && aArgs[1] && aArgs[2] && aArgs[3] && aArgs[4] )
        return INF_ENGINE_ERROR_ARGC;
    
    const char * Current = aArgs[0];
    const char * Offset = Current ? aArgs[1] : nullptr;
    const char * DateFormat = Offset ? aArgs[2] : nullptr;
    const char * TimeFormat = DateFormat ? aArgs[3] : nullptr;
    
    if( !Current )
        Current = ARGS[0].default_value;
    if( !Offset )
        Offset = ARGS[1].default_value;
    if( !DateFormat )
        DateFormat = ARGS[2].default_value;
    if( !TimeFormat )
        TimeFormat = ARGS[3].default_value;
      
    // Разбор первого аргумента - времени, относительно которого вычисляется смещение.
    int cur_year = 0;
    int cur_month = 0;
    int cur_day = 0;
    int cur_hour = 0;
    int cur_minute = 0;
    int cur_second = 0;    
    InfEngineErrors iee = ParseCurrentTime( Current, strlen(Current), cur_year, cur_month, cur_day, cur_hour, cur_minute, cur_second, aFunctionContext->vSession );
    if( iee != INF_ENGINE_SUCCESS )
    {
        if( iee == INF_ENGINE_ERROR_CANT_PARSE )
            return INF_ENGINE_WARN_UNSUCCESS;
        return  iee;
    }
    
    // Разбор второго аргумента - размера смещения.
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;    
    if( INF_ENGINE_SUCCESS != ( iee = ParseTimeOffset( Offset, strlen(Offset), year, month, day, hour, minute, second ) ) )
    {
        if( iee == INF_ENGINE_ERROR_CANT_PARSE )
            return INF_ENGINE_WARN_UNSUCCESS;
        return  iee;
    }
    
    // Разбор тертьего аргумента - формата даты.
    const char * date_format = nullptr;
    if( INF_ENGINE_SUCCESS != ( iee = ParseDataOutputFormat( DateFormat, strlen(DateFormat), date_format ) ) )
    {
        if( iee == INF_ENGINE_ERROR_CANT_PARSE )
            return INF_ENGINE_WARN_UNSUCCESS;
        return  iee;
    }
    
    // Разбор четвёртого аргумента - формата времени.
    const char * time_format;
    if( INF_ENGINE_SUCCESS != ( iee = ParseTimeOutputFormat( TimeFormat, strlen(TimeFormat), time_format ) ) )
    {
        if( iee == INF_ENGINE_ERROR_CANT_PARSE )
            return INF_ENGINE_WARN_UNSUCCESS;
        return  iee;
    }
    
    time_t tmp;
    time( &tmp );
    struct tm * t = localtime( &tmp );
    t->tm_sec = cur_second;
    t->tm_min = cur_minute;
    t->tm_hour = cur_hour;
    t->tm_mday = cur_day;
    t->tm_mon = cur_month - 1;
    t->tm_year = cur_year < 100 ? cur_year : cur_year - 1900;
    t->tm_isdst = 0;
    
    // Коректируем текущее время, согласно заданному смещению.
    t->tm_year += year;
    t->tm_mon += month;
    t->tm_year += t->tm_mon/ 12;
    t->tm_mon = t->tm_mon % 12;
    
    time_t Timer = mktime( t );
    Timer += second + minute*60 + hour*3600 + day*3600*24;
    
    struct tm * TimeStruct = localtime( &Timer );
    if( !TimeStruct )
        return INF_ENGINE_WARN_UNSUCCESS;
    
    // Поправка на зимнее время.
    if( TimeStruct->tm_isdst )
    {
        Timer -= 3600 * TimeStruct->tm_isdst;
        TimeStruct = localtime( &Timer );
        if( !TimeStruct )
            return INF_ENGINE_WARN_UNSUCCESS;
    }
    
    // Создание требуемого формата для возвращаемого функцией значения.
    aTextString format;
    // Формат даты.
    if( date_format )
    {
        if( format.append( date_format[0] == '\0' ? "%d.%m.%Y" : date_format ) != nlrcSuccess )
            return INF_ENGINE_ERROR_NOFREE_MEMORY;
        if( format.append( " ", 1 ) != nlrcSuccess )
            return INF_ENGINE_ERROR_NOFREE_MEMORY;    
    }
    // Формат времени.
    if( time_format )
    {
        if( format.append( time_format[0] == '\0' ? "%H:%M" : time_format ) != nlrcSuccess )
            return INF_ENGINE_ERROR_NOFREE_MEMORY;
    }
    
    char DateBuffer[1024];
    unsigned int DateLength = strftime( DateBuffer, 1024, format.ToConstChar(), TimeStruct );
    
    // Выделяем память под результат.
    char* Buffer = static_cast<char*>( aMemoryAllocator->Allocate( DateLength + 1 ) );
    if( !Buffer )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;

    // Копируем результат.
    memcpy( Buffer, DateBuffer, DateLength );
    Buffer[DateLength] = '\0';
    aResult = Buffer;
    aLength = DateLength;
    
    return INF_ENGINE_SUCCESS;
}
