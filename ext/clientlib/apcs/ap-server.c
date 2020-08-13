/* -*- C -*-
 * File: ap-server.c
 *
 * Created: Sun Apr 13 16:21:03 2003
 */

#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/signal.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/resource.h>

#include "csdata.h"
#include "readwrite.h"

#if defined(_UNIXDEBUG) && defined(OS_LINUX)
#include <sys/prctl.h>
#endif /* _UNIXDEBUG */

#ifndef MASTER_STAT_TAKEN

#define MASTER_STAT_TAKEN  0
#define MASTER_STAT_AVAIL  1
#define MASTER_FLOW_READ   3
#define MASTER_FLOW_WRITE  4
#define MASTER_STATUS_FD   5
#define MASTER_LISTEN_FD   6
#endif


#define MAX_ACTIONS     1000
#define MAX_ACT_PER_RULE    10
#define MAX_HDR_PER_ACTION  10
#define MAX_TO          10

typedef struct _MASTER_STATUS
{
    int pid;
    int avail;
} MASTER_STATUS;

/** Current Socket Id. For internal use only. */
static int gSocketFD = 0;


int static_log_level = LOG_NOTICE;

void cs_set_server_loglevel( int loglevel )
{

    static_log_level = loglevel;
}

void default_logger( int loglevel, char *fmt, ... )
{
    va_list ap;
    if( loglevel > static_log_level ) return;
    va_start( ap, fmt );
    vsyslog( LOG_INFO, fmt, ap );
    va_end( ap );
}

cs_server_logger_t logger = (cs_server_logger_t)default_logger;

void cs_set_server_logger( cs_server_logger_t n )
{
    logger = n;
}

void process_connection( int fd, cs_server_callback_t cb, unsigned int aTimeOut )
{
    int status = 11;
    csdata_data_t *input = csdata_recv_data( fd, &status, aTimeOut );
    if( !input )
    {
        if( logger )
            ( *logger )( LOG_ERR, "Cannot receive data, Status: %d", status );
    }
    else
    {
        csdata_data_t *out = ( *cb )( input );
        if( out )
        {
            csdata_send_data( fd, out, aTimeOut );
            csdata_clear_data( out );
        }
        csdata_clear_data( input );
    }
    close( fd );
}


int graceful_exit = 0;

void sighup( int sig )
{
    graceful_exit++;
}

int cs_server_main( cs_server_callback_t cb, int max_requests, int alrm )
{
    fd_set rfd;
    struct timeval tv;
    int flags, sel, a_fd;
    struct sockaddr_un sun_accepted;
    socklen_t s_len;
    int req_count = 0;

    signal( SIGHUP, sighup );

    flags = fcntl( MASTER_LISTEN_FD, F_GETFL, 0 );
    if( fcntl( MASTER_LISTEN_FD, F_SETFL, flags | O_NONBLOCK ) == -1 )
    {
        if( logger )
            logger( LOG_ERR, "Cannot set O_NONBLOCK on MASTER_LISTEN_FD" );
    }
    while( 1 )
    {
        FD_ZERO( &rfd );
        bzero( &tv, sizeof (tv ) );
        tv.tv_sec = 1;
        FD_SET( MASTER_LISTEN_FD, &rfd );
        sel = select( MASTER_LISTEN_FD + 1, &rfd, NULL, NULL, &tv );
        if( sel < 0 && errno != EINTR )
        {
            if( logger )
                logger( LOG_ERR, "Select: %s", strerror( errno ) );
            return -1;
        }
        else if( sel > 0 )
        {
            if( FD_ISSET( MASTER_LISTEN_FD, &rfd ) )
            {
                // trying to accept
                s_len = sizeof (sun_accepted );
                a_fd = accept( MASTER_LISTEN_FD, (struct sockaddr*)&sun_accepted, &s_len );
                if( a_fd < 0 )
                {
                    if( errno != EWOULDBLOCK && errno != ECONNABORTED &&
                        errno != EINTR
                        #ifdef EPROTO
                        && errno != EPROTO
                        #endif
                        )
                    {
                        if( logger )
                            logger( LOG_ERR, "Accept: %s", strerror( errno ) );
                    }
                }
                else
                {
                    MASTER_STATUS ms;
                    memset( &ms, 0, sizeof (ms ) );
                    ms.pid = getpid( );
                    ms.avail = MASTER_STAT_TAKEN;
                    if( nbwrite( MASTER_STATUS_FD, &ms, sizeof (ms ), alrm * 1000000 ) != sizeof (ms ) )
                    {
                        if( logger )
                            logger( LOG_ERR, "Can\'t inform master, exiting" );
                        graceful_exit = 1;
                    }
                    // set blocking to socket
                    #if 1
                    flags = fcntl( a_fd, F_GETFL, 0 );
                    flags &= ~O_NONBLOCK;
                    fcntl( a_fd, F_SETFL, flags );
                    #endif
                    if( alrm )
                        alarm( alrm );
                    gSocketFD = a_fd;
                    process_connection( a_fd, cb, alrm * 1000000 );
                    gSocketFD = 0;
                    if( alrm )
                        alarm( 0 );
                    memset( &ms, 0, sizeof (ms ) );
                    ms.pid = getpid( );
                    ms.avail = MASTER_STAT_AVAIL;
                    if( nbwrite( MASTER_STATUS_FD, &ms, sizeof (ms ), alrm * 1000 ) != sizeof (ms ) )
                    {
                        if( logger )
                            logger( LOG_ERR, "Can\'t inform master, exiting" );
                        graceful_exit = 1;
                    }
                }
            }
        }
        if( graceful_exit )
            break;
        req_count++;
        if( max_requests && req_count >= max_requests )
            break;
    }
    return 0;
}

void csAbnormalExit( csdata_data_t* aReturnData, int aReturnCode, unsigned int aTimeOut )
{
    // Посылка данных клиенту.
    if( gSocketFD && aReturnData )
        csdata_send_data( gSocketFD, aReturnData, aTimeOut*1000000 );

    // Очистка клиентских данных.
    // @todo Нужно продумать и сделать флаг, показывающий как была выделена память.
/*
    if( aReturnData )
        csdata_clear_data( aReturnData );
*/

    // Выход.
    _exit( aReturnCode );
}
