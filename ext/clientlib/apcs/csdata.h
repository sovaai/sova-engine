/* -*- C -*-
 * File: csdata.h
 *
 * Created: Sun Jul  6 12:04:42 2008
 */

#include <sys/types.h>
#include <stdint.h>

#ifndef __CSDATA_H
#define __CSDATA_H

typedef enum
{
    CSDATA_MARKER_TYPE = 0x00000001U,
    CSDATA_MAGIC = 0x00173414U,
    CSDATA_EOF = 0x22221111U,

    CSDATA_ATTRCOUNT = 0x00000002U,

    CSDATA_ATTRNAME = 0x00000004U,

    CSDATA_STRING = 0x00000008U,
    CSDATA_INT = 0x00000010U,
    CSDATA_BODY = 0x00001000U,
    CSDATA_EMPTYBODY = 0x00001001U,
    CSDATA_TYPEMASK = 0x0000ffffU,
    CSDATA_KEEP_ANAME = 0x10000000U,
    CSDATA_KEEP_AVALUE = 0x20000000U
} CSDATA_DATA_TYPES;

typedef struct
{
    CSDATA_DATA_TYPES attr_type;

    char* attr_name;
    #ifdef CSDATA_EXTENDED
    unsigned int attr_name_size;
    #endif //CSDATA_EXTENDED

    union
    {
        int attr_int;
        char* attr_string;
    } attr_value;
    #ifdef CSDATA_EXTENDED
    unsigned int attr_value_size;
    #endif //CSDATA_EXTENDED
} csdata_attr_t;

typedef struct
{
    uint32_t attr_count;
    csdata_attr_t* attr_list;
    uint32_t body_size;
    char* body_ptr;
} csdata_data_t;

csdata_data_t* csdata_alloc_data( size_t attr_count );
void csdata_clear_data( csdata_data_t *data );
int csdata_send_data( int fd, csdata_data_t *data, unsigned int aTimeOut );
csdata_data_t* csdata_recv_data( int fd, int* status, unsigned int aTimeOut );

typedef csdata_data_t *(*cs_server_callback_t)(csdata_data_t *);
typedef void (*cs_server_logger_t)(int loglevel, const char * fmt, ...);

// Server library functions
int cs_server_main( cs_server_callback_t, int max_requests, int alarm );
void cs_set_server_logger( cs_server_logger_t );
void cs_set_server_loglevel( int loglevel );

// Client functions

/**
 *  Выполнение запроса к серверу.
 * @param aConnectionString - строка, определяющая соединение с сервером.
 * @param aTimeOut - таймаут в милисекундах.
 * @param aRequest - запрос к серверу.
 * @param aStatus - статус запроса.
 */
csdata_data_t * csdata_rpc_call( const char * aConnectionString, long aTimeOut, csdata_data_t* aRequest, int* aStatus );

/**
 *  Фунция для принудительного выхода из серверного процесса с предварительной посылкой ответа клиенту.
 * @param aReturnData - возвращаемые данные.
 * @param aReturnCode - код выхода для серверного процесса.
 * @param aTimeOut - таймаут на операцию посылки данных клиенту.
 */
void csAbnormalExit( csdata_data_t* aReturnData, int aReturnCode, unsigned int aTimeOut );

#endif
