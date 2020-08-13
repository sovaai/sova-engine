#ifndef __RequestStat_hpp
#define __RequestStat_hpp

#include <sys/time.h>

struct RequestStatEx
{
    unsigned int timer_compilation_userbase = 0; //+

    unsigned int counter_request_userbase_patterns_all_index = 0; //+
    unsigned int counter_request_userbase_patterns_all_fuzzy = 0; //+
    unsigned int counter_request_userbase_patterns_intricate_index = 0; //+
    unsigned int counter_request_userbase_patterns_intricate_fuzzy = 0; //+
    unsigned int counter_request_userbase_patterns_accepted_index = 0; //+
    unsigned int counter_request_userbase_patterns_accepted_fuzzy = 0; //+

    unsigned int counter_request_mainbase_patterns_all_index = 0; //+
    unsigned int counter_request_mainbase_patterns_all_fuzzy = 0; //+
    unsigned int counter_request_mainbase_patterns_intricate_index = 0; //+
    unsigned int counter_request_mainbase_patterns_intricate_fuzzy = 0; //+
    unsigned int counter_request_mainbase_patterns_accepted_index = 0; //+
    unsigned int counter_request_mainbase_patterns_accepted_fuzzy = 0; //+

    unsigned int counter_response_allbases_patterns = 0; //+

    unsigned int timer_request_userbase_patterns_index = 0; //+
    unsigned int timer_request_userbase_patterns_fuzzy = 0; //+
    unsigned int timer_request_userbase_that_match = 0; //+
    unsigned int timer_request_userbase_that_dicts = 0; //+
    unsigned int timer_request_userbase_terms_patterns = 0; //+
    unsigned int timer_request_userbase_terms_dicts = 0; //+
    unsigned int timer_request_userbase_terms_that_match = 0; //+
    unsigned int timer_request_userbase_terms_that_dicts = 0; //+
    unsigned int timer_request_userbase_all = 0; //+

    unsigned int timer_request_mainbase_patterns_index = 0; //+
    unsigned int timer_request_mainbase_patterns_fuzzy = 0; //+
    unsigned int timer_request_mainbase_that_match = 0; //+
    unsigned int timer_request_mainbase_that_dicts = 0; //
    unsigned int timer_request_mainbase_terms_patterns = 0; //+
    unsigned int timer_request_mainbase_terms_dicts = 0; //+
    unsigned int timer_request_mainbase_terms_that_match = 0; //+
    unsigned int timer_request_mainbase_terms_that_dicts = 0; //+
    unsigned int timer_request_mainbase_all = 0; //+

    unsigned int timer_request_allbases_terms_patterns = 0; //+
    unsigned int timer_request_allbases_terms_dicts = 0; //+
    unsigned int timer_request_allbases_terms_that_match = 0; //+
    unsigned int timer_request_allbases_terms_that_dicts = 0; //+
    unsigned int timer_request_allbases_dicts = 0;  //+

    unsigned int timer_response_userbase_all = 0; //+
    unsigned int timer_response_mainbase_all = 0; //+
    unsigned int timer_response_allbases_all = 0;

    unsigned int timer_cache_session_load = 0; //+
    unsigned int timer_cache_session_save = 0; //+
    unsigned int timer_cache_infdata_load = 0; //+
    unsigned int timer_cache_infdata_save = 0; //+

    unsigned int timer_global_iteration = 0; //+
};

// Статистика процесса поиска и генерации ответа.
struct RequestStat
{
    RequestStat()
        : vQst( 0 )
        , vQstDeep( 0 )
        , vQstAcc( 0 )
        , vQstTime( 0 )
        , vPWTQst( 0 )
        , vPWTQstDeep( 0 )
        , vPWTQstAcc( 0 )
        , vPWTQstTime( 0 )
        , vDictTm( 0 )
        , vThatBaseTm( 0 )
        , vThatDictTm( 0 )
        , vTermsCheckUserTm( 0 )
        , vTermsCheckBaseTm( 0 )
        , vTermsCheckDictTm( 0 )
        , vTermsCheckThatTm( 0 )
        , vTermsCheckDictThatTm( 0 )
        , vTermsCheckTime( 0 )
        , vUserTime( 0 )
        , vBaseTime( 0 )
        , vTime( 0 )
        , vAnsUserTm( 0 )
        , vAnsBaseTm( 0 )
        , vAnsTm( 0 )
        , vAnsAttempt( 0 )
        , vCacheLoadUserTm( 0 )
        , vCacheLoadSessionTm( 0 )
        , vRequestTm( 0 )
    {}
    
    /** Статистика компиляции пользовательских шаблонов. */
    unsigned int vUserTmpls;    // Количество шаблонов у пользовательского инфа.
    unsigned int vUserCmplTime; // Время компиляции.
    
    /** Статистика по шаблон-вопросам, содержащим реальные термины. */
    unsigned int vQst;          // Количество проверенных шаблон-вопросов.
    unsigned int vQstDeep;      // Количество проверенных шаблон-вопросов, которые не удалось отсеить эвристиками.
    unsigned int vQstAcc;       // Количество подошедших шаблон-вопросов.
    unsigned int vQstTime;      // Время проверки шаблон-вопросов.
    
    /** Статистика по шаблон-вопросам, не содержащим реальные термины. */
    unsigned int vPWTQst;       // Количество проверенных шаблон-вопросов.
    unsigned int vPWTQstDeep;   // Количество проверенных шаблон-вопросов, которые не удалось отсеить эвристиками.
    unsigned int vPWTQstAcc;    // Количество подошедших шаблон-вопросов.
    unsigned int vPWTQstTime;   // Время проверки шаблон-вопросов.
    
    /** Статистика поиска по словарям. */
    unsigned int vDictTm;       // Время поиска фразы пользователя по словарям.
    
    /** Статистика поиска по That-строкам. */
    unsigned int vThatBaseTm;       // Время поиска reply- по базе that-строк (включая поиск по FltTermsBase).
    unsigned int vThatDictTm;       // Время поиска reply- в словарях (включая поиск по FltTermsBase).
            
    /** Время поиска по дереву с терминами FltTermsBase. */
    unsigned int vTermsCheckUserTm;     // Поиск по терминам пользовательских шаблонов.
    unsigned int vTermsCheckBaseTm;     // Поиск по терминам основаной базы шаблонов.
    unsigned int vTermsCheckDictTm;     // Поиск по базе терминов словарей.
    unsigned int vTermsCheckThatTm;     // Поиск reply- по базе терминов that-строк.
    unsigned int vTermsCheckDictThatTm; // Поиск reply- по базе терминов словарей.
    unsigned int vTermsCheckTime;       // Общее время поиска по всем FltTermsBase.
    
    /** Время, потраченное на всю процедуру поиска. */
    unsigned int vUserTime;     // Поиск по шаблонам пользовательской базы.
    unsigned int vBaseTime;     // Поиск по шаблонам основной базы.
    unsigned int vTime;
    
    /** Время, потраченное на процедуру генерации ответа. */
    unsigned int vAnsUserTm;    // Время генерации ответа, полученного из пользовательской базы.
    unsigned int vAnsBaseTm;    // Время генерации ответа, полученного из основной базы.
    unsigned int vAnsTm;        // Суммарное время генерации ответа, полученное из обеих баз.

    /** Количество обработанных шаблон-ответов. */
    unsigned int vAnsAttempt;

    /** Время, потраченное на работу с кэшем. */
    unsigned int vCacheLoadUserTm;     // Время получения из кэша пользовательской базы.
    unsigned int vCacheLoadSessionTm;  // Время получения из кэша сессии.

    /** Время, потраченное на обработку запроса пользователя. */
    unsigned int vRequestTm;

    /** Набор счётчиков для Графита*/
    RequestStatEx ex;
};


// Класс для измерения времени выполнения участков кода.
class  TimeStamp
{    
public:  
    TimeStamp()
    {        
        if( !gettimeofday( &tv, nullptr ) )
            vUSecinds = tv.tv_sec * 1000000 + tv.tv_usec;
        else
            vUSecinds = static_cast<suseconds_t>(-1);
    }
    
    /**
     *  Возвращает количество микросекунд, прошедших с момента создания объекта.
     */
    inline suseconds_t Interval() const
    {
        if( !gettimeofday( &tv, nullptr ) )
            return tv.tv_sec * 1000000 + tv.tv_usec - vUSecinds;
        else return static_cast<suseconds_t>(-1);
    }
    
    /**
     *  Возвращает количество микросекунд, прошедших с момента создания объекта.
     */
    inline operator long int() const
    {
        return Interval();
    }
    
protected:        
    // Время создания объекта в микросекундах.
    suseconds_t vUSecinds;
    
private:
    mutable struct timeval tv;
        
};


#endif // __RequestStat_hpp
