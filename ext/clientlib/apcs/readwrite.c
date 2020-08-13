#include "readwrite.h"

/**
 * @todo Нужно поддержать полноценный таймаут на общее время выполнения всех функций.
 * @todo Нужно, чтобы параметр aTimeOut уменьшался на время выполнения функции.
 */

ssize_t nbread( int aFileDescriptor, void *aBuffer, size_t aTargetSize, unsigned int aTimeOut )
{
    fd_set rfds;

    // Устанавливаем таймаут.
    struct timeval timeout;
    timeout.tv_sec = floor( (double)aTimeOut / 1000000 );
    timeout.tv_usec = aTimeOut % 1000000;

    // Считываем данные.
    size_t size = 0;
    while( size < aTargetSize )
    {
        FD_ZERO( &rfds );
        FD_SET( aFileDescriptor, &rfds );

        // Вызываем селект чтобы проверить статус сокета.
        int sel_status = select( aFileDescriptor + 1, &rfds, NULL, NULL, &timeout );
        if( sel_status < 0 )
        {
            if( errno != EINTR )
                // Ошибка.
                return -1;
            else
                continue;
        }
        else if( sel_status == 0 )
        {
            // Сработал таймаут.
            errno = 0;
            return -1;
        }
        else if( !FD_ISSET( aFileDescriptor, &rfds ) )
            continue;

        // Читаем данные из сокета.
        ssize_t res = read( aFileDescriptor, (char*)aBuffer + size, aTargetSize - size );
        if( res < 0 )
        {
            if( errno != EINTR && errno != EAGAIN )
                // Ошибка.
                return -1;
            else
                continue;
        }
        else if( res == 0 )
            return size;
        else
            size += res;
    }

    return size;
}

ssize_t nbreadv( int aFileDescriptor, const struct iovec *iov, int iovcnt, unsigned int aTimeOut )
{
    ssize_t size = 0;

    // Считываем данные.
    int i;
    for( i = 0; i < iovcnt; i++ )
    {
        // Считываем i-ый буффер.
        ssize_t res = nbread( aFileDescriptor, iov[i].iov_base, iov[i].iov_len, aTimeOut );
        if( res < 0 )
            return res;
        else
            size += res;
    }

    return size;
};

ssize_t nbwrite( int aFileDescriptor, const void *aBuffer, size_t aTargetSize, unsigned int aTimeOut )
{
    fd_set rfds;

    // Устанавливем таймаут.
    struct timeval TimeOut;

    TimeOut.tv_sec = floor( (double)aTimeOut / 1000000 );
    TimeOut.tv_usec = aTimeOut % 1000000;

    // Записываем данные.
    size_t size = 0;
    while( size < aTargetSize )
    {
        FD_ZERO( &rfds );
        FD_SET( aFileDescriptor, &rfds );

        struct timeval LocalTimeOut = TimeOut;

        // Вызываем селект чтобы проверить статус сокета.
        int sel_status = select( aFileDescriptor + 1, NULL, &rfds, NULL, &LocalTimeOut );
        if( sel_status < 0 )
        {
            // Ошибка.
            if( errno != EINTR )
                return -1;
            else
                continue;
        }
        else if( sel_status == 0 )
        {
            // Сработал таймаут.
            errno = 0;
            return -1;
        }
        else if( !FD_ISSET( aFileDescriptor, &rfds ) )
            continue;

        // Пишем данные в сокет.
        ssize_t res = write( aFileDescriptor, (const char *)aBuffer + size, aTargetSize - size );
        if( res < 0 )
        {
            if( errno != EINTR && errno != EAGAIN )
                // Ошибка.
                return -1;
            else
                continue;
        }
        else if( res == 0 )
            return size;
        else
            size += res;
    }

    return size;
}

ssize_t nbwritev( int aFileDescriptor, const struct iovec *iov, int iovcnt, unsigned int aTimeOut )
{
    ssize_t size = 0;

    // Запись данных.
    int i;
    for( i = 0; i < iovcnt; i++ )
    {
        ssize_t res = nbwrite( aFileDescriptor, iov[i].iov_base, iov[i].iov_len, aTimeOut );
        if( res < 0 )
            return res;
        else
            size += res;
    }

    return size;
};
