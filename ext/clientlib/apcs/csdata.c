/* -*- C -*-
 * File: csdata.c
 *
 * Created: Sun Jul  6 20:12:28 2008
 */

#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "csdata.h"

#include "readwrite.h"

csdata_data_t* csdata_alloc_data( size_t attr_count )
{
    csdata_data_t* ret = calloc( sizeof (csdata_data_t ), 1 );
    if( !ret )
        return NULL;
    if( attr_count > 0 )
    {
        if( !( ret->attr_list = calloc( sizeof (csdata_attr_t ), attr_count ) ) )
        {
            free( ret );
            return NULL;
        }
        ret->attr_count = attr_count;
    }
    return ret;
}

void csdata_clear_data( csdata_data_t *data )
{
    int i;
    if( !data ) return;
    if( data->body_ptr )
        free( data->body_ptr );
    if( data->attr_list && data->attr_count )
    {
        for( i = 0; i < data->attr_count; i++ )
        {
            // clear attribute name
            if( data->attr_list[i].attr_name
                && !( data->attr_list[i].attr_type & CSDATA_KEEP_ANAME ) )
                free( data->attr_list[i].attr_name );

            // and (string) value
            if( ( data->attr_list[i].attr_type & CSDATA_TYPEMASK ) == CSDATA_STRING
                && data->attr_list[i].attr_value.attr_string
                && !( data->attr_list[i].attr_type & CSDATA_KEEP_AVALUE )
                )
                free( data->attr_list[i].attr_value.attr_string );
        }
    }
    if( data->attr_list )
        free( data->attr_list );
    free( data );
}

static int send_int( int fd, uint32_t type, uint32_t value, unsigned int aTimeOut )
{
    uint32_t n_type = htonl( type );
    uint32_t len = sizeof (value );
    uint32_t n_len = htonl( len );
    uint32_t n_value = htonl( value );

    struct iovec iov[3];
    iov[0].iov_base = &n_type;
    iov[0].iov_len = sizeof (uint32_t );
    iov[1].iov_base = &n_len;
    iov[1].iov_len = sizeof (uint32_t );
    iov[2].iov_base = &n_value;
    iov[2].iov_len = sizeof (uint32_t );

    return nbwritev( fd, iov, 3, aTimeOut );
}

static int send_string( int fd, uint32_t type, char *string, uint32_t string_len, unsigned int aTimeOut )
{
    uint32_t n_type = htonl( type );
    uint32_t len = string_len ? string_len : strlen( string );
    uint32_t n_len = htonl( len );

    struct iovec iov[3];
    iov[0].iov_base = &n_type;
    iov[0].iov_len = sizeof ( n_type );
    iov[1].iov_base = &n_len;
    iov[1].iov_len = sizeof ( n_len );
    iov[2].iov_base = string;
    iov[2].iov_len = len;

    return nbwritev( fd, iov, 3, aTimeOut );
}

int csdata_send_data( int fd, csdata_data_t *data, unsigned int aTimeOut )
{
    uint32_t attrcount = 0, i;
    int ret;
    if( !data )
        return EINVAL;

    /*
    // ���� ������ � ������� �������
    /// type(4 �����), ����� (4 �����), ������ (���������� �����)
    // �������� ���������� � ���� �������
    // CSDATA_ATTRNAME/�����/��� �������� + CSDATA_STRING|INT/�����/�������� ��������
    // ������ ���������� ��� ������������ ����, ���� ���� �� ������ � ������ �����

    // �������� magic
     */
    if( ( ret = send_int( fd, CSDATA_MARKER_TYPE, CSDATA_MAGIC, aTimeOut ) ) < 0 )
        return ret;

    // ������� ���������� ��������� � ��������� ������, �������� ���������� ���������
    for( i = 0; i < data->attr_count; i++ )
    {
        if( data->attr_list[i].attr_name && data->attr_list[i].attr_type )
        {
            attrcount++;
        }
    }
    if( ( ret = send_int( fd, CSDATA_ATTRCOUNT, attrcount, aTimeOut ) ) < 0 )
        return ret;
    // �������� ��������
    if( attrcount > 0 )
    {
        for( i = 0; i < data->attr_count; i++ )
        {
            if( data->attr_list[i].attr_name && data->attr_list[i].attr_type )
            {
                #ifdef CSDATA_EXTENDED
                if( ( ret = send_string( fd, CSDATA_ATTRNAME, data->attr_list[i].attr_name, data->attr_list[i].attr_name_size, aTimeOut ) ) < 0 )
                    return ret;
                #else
                if( ( ret = send_string( fd, CSDATA_ATTRNAME, data->attr_list[i].attr_name, 0, aTimeOut ) ) < 0 )
                    return ret;
                #endif //CSDATA_EXTENDED

                if( ( data->attr_list[i].attr_type & CSDATA_TYPEMASK ) == CSDATA_STRING )
                {
                    #ifdef CSDATA_EXTENDED
                    if( ( ret = send_string( fd, CSDATA_STRING, data->attr_list[i].attr_value.attr_string, data->attr_list[i].attr_value_size, aTimeOut ) ) < 0 )
                        return ret;
                    #else
                    if( ( ret = send_string( fd, CSDATA_STRING, data->attr_list[i].attr_value.attr_string, 0, aTimeOut ) ) < 0 )
                        return ret;
                    #endif //CSDATA_EXTENDED
                }
                else
                {
                    if( ( ret = send_int( fd, data->attr_list[i].attr_type & CSDATA_TYPEMASK, data->attr_list[i].attr_value.attr_int, aTimeOut ) ) < 0 )
                        return ret;
                }
            }
        }
    }

    // �������� body
    if( data->body_ptr )
    {
        if( ( ret = send_string( fd, CSDATA_BODY, data->body_ptr, data->body_size, aTimeOut ) ) < 0 )
            return ret;
    }
    else
    {
        // �������� ������ ������� body
        if( ( ret = send_int( fd, CSDATA_EMPTYBODY, 0, aTimeOut ) ) < 0 )
            return ret;
    }
    // �������� EOF
    if( ( ret = send_int( fd, CSDATA_MARKER_TYPE, CSDATA_EOF, aTimeOut ) ) < 0 )
        return ret;
    return 0;
}

static int recv_int( int fd, uint32_t *type, uint32_t *value, unsigned int aTimeOut )
{
    uint32_t n_type, n_len, n_value;

    // Получение типа и размера пакета.
    struct iovec iov[2];
    iov[0].iov_base = &n_type;
    iov[0].iov_len = sizeof ( uint32_t );
    iov[1].iov_base = &n_len;
    iov[1].iov_len = sizeof ( uint32_t );

    int RetCode = nbreadv( fd, iov, 2, aTimeOut );
    if( RetCode < 0 )
        return RetCode;
    else if( RetCode != 2 * sizeof ( uint32_t ) )
    {
        errno = EINVAL;
        return -1;
    }

    // @info Нужна ли проверка типа?
    *type = ntohl( n_type );
    // Проверка размера пакета.
    if( ntohl( n_len ) != sizeof ( uint32_t ) )
    {
        errno = EINVAL;
        return -1;
    }

    // Получение значения.
    RetCode = nbread( fd, &n_value, sizeof ( uint32_t ), aTimeOut );
    if( RetCode < 0 )
        return RetCode;
    else if( RetCode != sizeof ( uint32_t ) )
    {
        errno = EINVAL;
        return -1;
    }

    *value = ntohl( n_value );

    return 0;
}

static int csdata_recv_attr( int fd, csdata_attr_t *attr, unsigned int aTimeOut )
{
    uint32_t n_type, n_len, type, len, n_val;

    // Получение типа и размера пакета.
    struct iovec iov[2];
    iov[0].iov_base = &n_type;
    iov[0].iov_len = sizeof ( uint32_t );
    iov[1].iov_base = &n_len;
    iov[1].iov_len = sizeof ( uint32_t );

    int RetCode = nbreadv( fd, iov, 2, aTimeOut );
    if( RetCode < 0 )
        return RetCode;
    else if( RetCode != 2 * sizeof ( uint32_t ) )
    {
        errno = EINVAL;
        return -1;
    }

    // Проверка типа пакета.
    type = ntohl( n_type ) & CSDATA_TYPEMASK;
    if( type != CSDATA_ATTRNAME )
    {
        errno = EINVAL;
        return -1;
    }

    // Выделение памяти для получении имени атрибута.
    len = ntohl( n_len );
    if( !( attr->attr_name = malloc( len + 1 ) ) )
    {
        errno = EINVAL;
        return -1;
    }

    // Получение имени атрибута.
    RetCode = nbread( fd, attr->attr_name, len, aTimeOut );
    if( RetCode < 0 )
        return RetCode;
    else if( RetCode != len )
    {
        errno = EINVAL;
        return -1;
    }

    attr->attr_name[len] = '\0';
    #ifdef CSDATA_EXTENDED
    attr->attr_name_size = len;
    #endif


    // Получение типа и размера пакета.
    iov[0].iov_base = &n_type;
    iov[0].iov_len = sizeof ( uint32_t );
    iov[1].iov_base = &n_len;
    iov[1].iov_len = sizeof ( uint32_t );

    RetCode = nbreadv( fd, iov, 2, aTimeOut );
    if( RetCode < 0 )
        return RetCode;
    else if( RetCode != 2 * sizeof ( uint32_t ) )
    {
        errno = EINVAL;
        return -1;
    }

    // Получение типа пакета.
    type = ntohl( n_type ) & CSDATA_TYPEMASK;
    // Получение размера значения пакета.
    len = ntohl( n_len );

    if( type == CSDATA_INT )
    {
        // Значение аргумента - число.
        if( len != sizeof ( uint32_t ) )
        {
            errno = EINVAL;
            return -1;
        }

        // Получение значения атрибута.
        RetCode = nbread( fd, &n_val, sizeof ( uint32_t ), aTimeOut );
        if( RetCode < 0 )
            return RetCode;
        else if( RetCode != sizeof ( uint32_t ) )
        {
            errno = EINVAL;
            return -1;
        }

        attr->attr_value.attr_int = ntohl( n_val );
        attr->attr_type = type;
    }
    else if( type == CSDATA_STRING )
    {
        // Значение атрибута - строка.

        // Выделение памяти под значение.
        if( !( attr->attr_value.attr_string = malloc( len + 1 ) ) )
        {
            errno = ENOMEM;
            return -1;
        }

        // Получение значения атрибута.
        RetCode = nbread( fd, attr->attr_value.attr_string, len, aTimeOut );
        if( RetCode < 0 )
            return RetCode;
        else if( RetCode != len )
        {
            errno = EINVAL;
            return -1;
        }
        attr->attr_value.attr_string[len] = '\0';
        #ifdef CSDATA_EXTENDED
        attr->attr_value_size = len;
        #endif
        attr->attr_type = type;
    }
    else
    {
        // Неизвестный тип.
        errno = EINVAL;
        return -1;
    }

    // Атрибут успешно считан.
    return 0;
}

static int csdata_recv_body( int fd, csdata_data_t *data, unsigned int aTimeOut )
{
    uint32_t n_type, n_len, type, len;

    struct iovec iov[2];

    // Получение типа и размера пакета.
    iov[0].iov_base = &n_type;
    iov[0].iov_len = sizeof ( uint32_t );
    iov[1].iov_base = &n_len;
    iov[1].iov_len = sizeof ( uint32_t );

    int RetCode = nbreadv( fd, iov, 2, aTimeOut );
    if( RetCode < 0 )
        return RetCode;
    else if( RetCode != 2 * sizeof ( uint32_t ) )
    {
        errno = EINVAL;
        return -1;
    }

    // Получение типа пакета.
    type = ntohl( n_type ) & CSDATA_TYPEMASK;
    // Получение размера пакета.
    len = ntohl( n_len );

    if( type == CSDATA_BODY )
    {
        // Пакет с непустым телом.
        if( !( data->body_ptr = malloc( len + 1 ) ) )
        {
            errno = ENOMEM;
            return -5;
        }

        // Получение тела.
        RetCode = nbread( fd, data->body_ptr, len, aTimeOut );
        if( RetCode < 0 )
            return RetCode;
        else if( RetCode != len )
        {
            errno = EINVAL;
            return -1;
        }

        data->body_ptr[len] = '\0';
        data->body_size = len;
    }
    else if( type == CSDATA_EMPTYBODY )
    {
        // Пакет с пустым телом.
        if( len != sizeof ( uint32_t ) )
        {
            errno = EINVAL;
            return -1;
        }

        uint32_t ph;

        data->body_ptr = NULL;
        data->body_size = 0;

        // Получение пакета.
        RetCode = nbread( fd, &ph, sizeof ( uint32_t ), aTimeOut );
        if( RetCode < 0 )
            return RetCode;
        else if( RetCode != sizeof ( uint32_t ) )
        {
            errno = EINVAL;
            return -1;
        }
    }
    else
    {
        errno = EINVAL;
        return -1;
    }

    // Тело успешно считано.
    return 0;
}

csdata_data_t * csdata_recv_data( int fd, int *status, unsigned int aTimeOut )
{
    uint32_t ival, itype, attrcount, i;
    csdata_data_t *ret;

    // Получение magic
    int RetCode = recv_int( fd, &itype, &ival, aTimeOut );
    if( RetCode < 0 )
    {
        if( status )
            *status = errno;

        return NULL;
    }

    if( ival != CSDATA_MAGIC )
    {
        if( status )
            *status = errno;

        return NULL;
    }

    RetCode = recv_int( fd, &itype, &attrcount, aTimeOut );
    if( RetCode != 0 )
    {
        if( status )
            *status = errno;

        return NULL;
    }

    if( itype != CSDATA_ATTRCOUNT )
    {
        if( status )
            *status = errno;
        return NULL;
    }

    if( !( ret = csdata_alloc_data( attrcount ) ) )
    {
        if( status )
            *status = ENOMEM;
        
        return NULL;
    }
    for( i = 0; i < attrcount; i++ )
    {
        if( ( RetCode = csdata_recv_attr( fd, &ret->attr_list[i], aTimeOut ) ) < 0 )
        {
            csdata_clear_data( ret );
            
            if( status )
                *status = errno;
            
            return NULL;
        }
    }
    if( ( RetCode = csdata_recv_body( fd, ret, aTimeOut ) ) < 0 )
    {
        csdata_clear_data( ret );
        
        if( status )
            *status = errno;
        
        return NULL;
    }
    if( ( RetCode = recv_int( fd, &itype, &ival, aTimeOut ) ) < 0 )
    {
        if( status )
            *status = errno;
        
        return NULL;
    }
    if( ival != CSDATA_EOF )
    {
        if( status )
            *status = EINVAL;
        
        return NULL;
    }
    
    if( status )
        *status = 0;
    
    return ret;
}
