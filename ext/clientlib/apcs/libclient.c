/* -*- C -*-
 * File: libclient.c
 *
 * Created: Sun Jul  6 12:04:30 2008
 */

#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

#include <sys/types.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>

#include "csdata.h"

#ifdef NO_INET_ATON

int
inet_aton( cp, addr )
register const char *cp;
struct in_addr *addr;
{
    u_long parts[4];
    in_addr_t val;
    char *c;
    char *endptr;
    int gotend, n;

    c = (char *)cp;
    n = 0;
    /*
     * Run through the string, grabbing numbers until
     * the end of the string, or some error
     */
    gotend = 0;
    while( !gotend )
    {
        errno = 0;
        val = strtoul( c, &endptr, 0 );

        if( errno == ERANGE ) /* Fail completely if it overflowed. */
            return (0 );

        /*
         * If the whole string is invalid, endptr will equal
         * c.. this way we can make sure someone hasn't
         * gone '.12' or something which would get past
         * the next check.
         */
        if( endptr == c )
            return (0 );
        parts[n] = val;
        c = endptr;

        /* Check the next character past the previous number's end */
        switch( *c )
        {
            case '.':
                /* Make sure we only do 3 dots .. */
                if( n == 3 ) /* Whoops. Quit. */
                    return (0 );
                n++;
                c++;
                break;

            case '\0':
                gotend = 1;
                break;

            default:
                if( isspace( (unsigned char)*c ) )
                {
                    gotend = 1;
                    break;
                }
                else
                    return (0 ); /* Invalid character, so fail */
        }

    }

    /*
     * Concoct the address according to
     * the number of parts specified.
     */

    switch( n )
    {
        case 0: /* a -- 32 bits */
            /*
             * Nothing is necessary here.  Overflow checking was
             * already done in strtoul().
             */
            break;
        case 1: /* a.b -- 8.24 bits */
            if( val > 0xffffff || parts[0] > 0xff )
                return (0 );
            val |= parts[0] << 24;
            break;

        case 2: /* a.b.c -- 8.8.16 bits */
            if( val > 0xffff || parts[0] > 0xff || parts[1] > 0xff )
                return (0 );
            val |= ( parts[0] << 24 ) | ( parts[1] << 16 );
            break;

        case 3: /* a.b.c.d -- 8.8.8.8 bits */
            if( val > 0xff || parts[0] > 0xff || parts[1] > 0xff ||
                parts[2] > 0xff )
                return (0 );
            val |= ( parts[0] << 24 ) | ( parts[1] << 16 ) | ( parts[2] << 8 );
            break;
    }

    if( addr != NULL )
        addr->s_addr = htonl( val );
    return (1 );
}
#endif

/* translating given name to sockaddr_in structure */

/* on errors sin_port will be -1 */
static int normalize_inet_name( const char *xname, struct sockaddr_in *addr )
{

    struct hostent *h;
    struct servent *serv;
    char *host, *port, *first, *second, *name = NULL;
    int ret = 0;

    if( !( name = strdup( xname ) ) )
        return ENOMEM;

    if( !( first = strchr( name, ':' ) ) || !( second = strrchr( name, ':' ) ) || !( first == second ) )
    {
        ret = EINVAL;
        goto exit;
    }

    *first = '\0';
    host = (char *)name;
    port = first + 1;

    addr->sin_family = AF_INET;

    /* check for numeric addresses by normal way. now possible bugs */
    if( *host >= '0' && *host <= '9' )
        addr->sin_addr.s_addr = inet_addr( host );
    else
    {
        h = gethostbyname( host );
        if( !h )
        {
            ret = ENOENT;
            goto exit;
        }
        addr->sin_addr = *( (struct in_addr *)h->h_addr );
    }

    if( *port >= '0' && *port <= '9' )
        addr->sin_port = htons( atoi( port ) );
    else
    {
        serv = getservbyname( port, (char*)0 );
        if( serv )
            addr->sin_port = serv->s_port;
        else
        {
            ret = ENOENT;
            goto exit;
        }
    }
exit:
    free( name );
    return ret;
}

int wait_for_fd( int fd, long timeout_ms )
{
    fd_set rfd;
    struct timeval tv;
    int state;

    FD_ZERO( &rfd );
    FD_SET( fd, &rfd );
    bzero( &tv, sizeof (tv ) );
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = ( timeout_ms % 1000 )*1000;

    state = select( fd + 1, &rfd, NULL, NULL, &tv );
    if( state > 0 && FD_ISSET( fd, &rfd ) )
    {
        return fd;
    }
    else if( state == 0 )
    {
        return ETIMEDOUT;
    }
    else
    {
        return ECONNREFUSED;
    }
}

/* connecting to TCP socket */
static int inet_connect( const struct sockaddr_in *addr, long timeout_ms )
{
    int retval;
    int flags;
    int state;
    int t = 1;
    struct linger l;
    int fd;

    if( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        //	    s->logp(s, LOG_ERR, "Cannot allocate socket: %s",strerror(errno));
        return ECONNREFUSED;
    }

    if( -1 == ( flags = fcntl( fd, F_GETFL, 0 ) ) )
    {
        close( fd );
        //	    s->logp(s, LOG_ERR, "Cannot get flags for socket %d: %s",
        //		    s->filter_fd,strerror(errno));
        return ECONNREFUSED;
    }
    #if 0
    if( -1 == fcntl( fd, F_SETFL, flags | O_NONBLOCK ) )
    {
        close( fd );
        //	    s->logp(s, LOG_ERR, "Cannot set NONBLOCK on socket %d: %s",
        //		    s->filter_fd,strerror(errno));
        return ECONNREFUSED;
    }
    #endif

    l.l_onoff = 1;
    l.l_linger = 0;
    if( -1 == setsockopt( fd, SOL_SOCKET, SO_LINGER, &l, sizeof (l ) ) )
    {
        close( fd );
        //	    s->logp(s, LOG_ERR, "Cannot set lingering close on socket %d: %s",
        //		    s->filter_fd,strerror(errno));
        return ECONNREFUSED;
    }
    if( -1 == setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, &t, sizeof (t ) ) )
    {
        //	    s->logp(s,LOG_ERR,"Could not set TCP_NODELAY on socket: %s",strerror(errno));
        close( fd );
        return ECONNREFUSED;
    }

    if( ( state = connect( fd, (struct sockaddr *)addr, sizeof (*addr ) ) ) < 0 )
    {
        if( errno != EINPROGRESS )
        {
            //		    s->logp(s, LOG_ERR, "Failed to connect: %s",strerror(errno));
            close( fd );
            return state;
        }
        else
        {
            retval = wait_for_fd( fd, timeout_ms );
            if( retval < 0 && fd > 0 )
            {
                close( fd );
            }
            return retval;
        }
    }
    else
    {
        //	    s->logp(s, LOG_DEBUG+1, "Connected immediately, fd=%d",s->filter_fd);
        return fd;
    }
}

/* connecting to unix socket */
static int unix_connect( const char *sockname, long timeout_ms )
{

    int flags;
    int state;
    struct sockaddr_un port;
    int fd = 0;

    port.sun_family = AF_LOCAL;
    strncpy( port.sun_path, sockname, sizeof (port.sun_path ) - 1 );
    if( ( fd = socket( AF_LOCAL, SOCK_STREAM, 0 ) ) < 0 )
    {
        return ECONNREFUSED;
    }

    if( -1 == ( flags = fcntl( fd, F_GETFL, 0 ) ) )
    {
        close( fd );
        return ECONNREFUSED;
    }
    if( ( state = connect( fd, (struct sockaddr *)&port, sizeof port ) ) < 0 )
    {
        if( errno != EINPROGRESS )
        {
            return ECONNREFUSED;
        }
        else
        {
            return wait_for_fd( fd, timeout_ms );
        }
    }
    else
    {
        return fd;
    }
}

int cs_connect( const char *addr, long timeout_ms )
{

    if( !strncmp( addr, "unix:", 5 ) )
        return unix_connect( addr + 5, timeout_ms );
    else if( !strncmp( addr, "tcp:", 4 ) )
    {
        int status;
        struct sockaddr_in ia;
        if( ( status = normalize_inet_name( addr + 4, &ia ) ) < 0 )
        {
            return status;
        }
        return inet_connect( &ia, timeout_ms );
    }
    else
    {
        errno = EINVAL;
        return -1;
    }
}

csdata_data_t * csdata_rpc_call( const char *addr, long timeout_ms, csdata_data_t *input, int* status )
{
    int fd = cs_connect( addr, timeout_ms );
    int rc;
    csdata_data_t *ret = NULL;
    if( fd < 0 )
    {
        if( status )
            *status = errno;
        return NULL;
    }
    if( ( rc = csdata_send_data( fd, input, timeout_ms * 1000 ) ) < 0 )
    {
        close( fd );
        if( status )
            *status = errno;
        return NULL;
    }
    ret = csdata_recv_data( fd, status, timeout_ms * 1000 );
    close( fd );
    return ret;
}

