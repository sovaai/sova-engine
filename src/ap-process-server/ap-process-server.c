/* -*- C -*- 
 * File: ap-process-server.c
 * 
 * Created: Fri Apr 18 09:10:39 MSD 2003
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <syslog.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <fcntl.h>

#include "apcs/readwrite.h"

#if defined(FREEBSD2) || defined(FREEBSD3) || defined(FREEBSD4) || defined(FREEBSD5) || defined(OS_FREEBSD)
#define HAS_DUPLEX_PIPE
#endif

/* constants */

#define	PATHSIZE		4096
#define CMDLINESIZE		4096
#define CONFIGLINESIZE		4096
#define MAX_CMDLINE_ARGUMENTS	256

/* timed parameters in microseconds */
#define FILTER_ERROR_DELAY	100000

#define SIGHUP_THRESHOLD	10*1000000
#define SIGTERM_THRESHOLD	10*1000000


/* 
 * From master/mail_proto.h 
 */


#define MASTER_STATUS_FD   5
#define MASTER_LISTEN_FD   6

#define NUM_DUMMY_DESCRIPTORS	MASTER_LISTEN_FD
int dummy_descriptors[NUM_DUMMY_DESCRIPTORS];

/* config defaults */

#define DEFAULT_FILTER_CMDLINE		"-V 1"
#define DEFAULT_LISTEN			"unix:/var/tmp/ap-process-server-socket"
#define DEFAULT_PIDFILE			"/var/tmp/ap-process-server.pid"
//#define DEFAULT_CONFIGFILE		"/usr/local/ap-mailfilter/etc/ap-process-server.conf"
//#define DEFAULT_FILTER_PATH		"/usr/local/ap-mailfilter/libexec/ap-mailfilter"
#define DEFAULT_START_FILTERS		0
#define DEFAULT_MAX_FILTERS		50
#define DEFAULT_MIN_SPARE_FILTERS	0
#define DEFAULT_UID			-1
#define DEFAULT_SYSLOG_FACILITY		LOG_LOCAL1
#define DEFAULT_LOG_LEVEL		3

#ifndef DEFAULT_CONFIGFILE
#error define DEFAULT_CONFIGFILE at compile time
#endif
#ifndef DEFAULT_FILTER_PATH
#error define DEFAULT_FILTER_PATH at compile time
#endif

/* crossplatform definitions */

#ifdef UNIX_PATH_MAX
#else
#define UNIX_PATH_MAX	108
#endif

#ifdef MAX_SOCK_ADDR
#define SOCKADDR_SIZE	MAX_SOCK_ADDR
#else
#define SOCKADDR_SIZE	128
#endif

/* types */

typedef struct _config_t
{
    char filter_path[PATHSIZE];
    char filter_cmdline[CMDLINESIZE];
    char listen[PATHSIZE];
    char pidfile[PATHSIZE];
    size_t start_filters;
    size_t max_filters;
    size_t min_spare_filters;
    int syslog_facility;
    int log_level;
} config_t;

typedef struct _filter_t
{
    pid_t pid;
    struct timeval execution_time;
    int avail;
    int usage;

    enum
    {
        run, sighup, sigterm
    } state;
    struct timeval state_timestamp;
} filter_t;

typedef struct _state_t
{
    int daemonized;
    config_t config;
    int listen_fd;
    int status_fd[2];
    filter_t *filters;
    size_t filters_alloc;
    size_t num_filters;
    size_t spare_filters;
    size_t start;
    struct timeval start_time;
} state_t;

typedef struct _MASTER_STATUS
{
    int pid;
    int avail;
} MASTER_STATUS;



/* Global variables */

char config_filename[PATHSIZE] = DEFAULT_CONFIGFILE;

volatile size_t childs_exited = 0;
volatile size_t reread_configuration = 0;
volatile size_t restart_filters = 0;
volatile size_t terminate_process_server = 0;

state_t STATE;

/* Logging */

#define DEFINE_LOG_LEVEL(level,priority) \
    void log_##level(char *format,...) \
    { \
        if( STATE.config.log_level >= level ) \
        { \
            va_list ap; \
            va_start( ap, format ); \
            vsyslog( level, format, ap ); \
            va_end(ap); \
            if( !STATE.daemonized ) \
            { \
                va_start( ap, format ); \
                vfprintf( stderr, format, ap ); \
                va_end(ap); \
                fprintf( stderr, "\n" ); \
            } \
        } \
    }

DEFINE_LOG_LEVEL( 1, LOG_ERR )
DEFINE_LOG_LEVEL( 2, LOG_INFO )
DEFINE_LOG_LEVEL( 3, LOG_INFO )
DEFINE_LOG_LEVEL( 4, LOG_INFO )
DEFINE_LOG_LEVEL( 5, LOG_INFO )
DEFINE_LOG_LEVEL( 6, LOG_DEBUG )



/* Signals handling */

void signal_hup_handler( int num )
{
    reread_configuration = 1;
    restart_filters = 1;
    log_4( "SIGHUP has been received" );
}

void signal_usr1_handler( int num )
{
    reread_configuration = 1;
    log_4( "SIGUSR1 has been received" );
}

void signal_death_handler( int sig )
{
    struct sigaction action;
    pid_t pid = getpid( );

    log_3( "terminating on signal %d", sig );
    terminate_process_server++;

    sigemptyset( &action.sa_mask );
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    if( sigaction( SIGTERM, &action, (struct sigaction *)0 ) < 0 )
        log_1( "sigaction: %m" );
    if( kill( -pid, SIGTERM ) < 0 )
        log_1( "kill process group: %m" );

    /*
     * Deliver the signal to ourselves and clean up. XXX We're running as a
     * signal handler and really should not be doing complicated things...
     */
    sigemptyset( &action.sa_mask );
    action.sa_flags = 0;
    action.sa_handler = SIG_DFL;
    if( sigaction( sig, &action, (struct sigaction *)0 ) < 0 )
        log_1( "sigaction: %m" );
    unlink( STATE.config.pidfile );
    if( kill( pid, sig ) < 0 )
        log_1( "self kill: %m" );
    exit( 1 );
}

void signal_chld_handler( int num )
{
    childs_exited = 1;
    log_5( "SIGCHLD has been received" );
}

/* configuration loading */

void config_set_defaults( config_t *config )
{
    memset( config, 0, sizeof (config_t ) );
    strcpy( config->filter_path, DEFAULT_FILTER_PATH );
    strcpy( config->filter_cmdline, DEFAULT_FILTER_CMDLINE );
    strcpy( config->listen, DEFAULT_LISTEN );
    strncpy( config->pidfile, DEFAULT_PIDFILE, PATHSIZE );
    config->start_filters = DEFAULT_START_FILTERS;
    config->max_filters = DEFAULT_MAX_FILTERS;
    config->min_spare_filters = DEFAULT_MIN_SPARE_FILTERS;
    //config->uid = DEFAULT_UID;
    config->syslog_facility = DEFAULT_SYSLOG_FACILITY;
    config->log_level = DEFAULT_LOG_LEVEL;
}

int config_load_file( config_t *config )
{
    int result = 0;
    FILE *cfg;
    char line[CONFIGLINESIZE];
    int linenumber;
    char *keyword, *arg;
    char *p;
    char *endptr;
    //struct passwd *pw;

    #undef CONFIG_ASSIGN_INT_VALUE
    #define CONFIG_ASSIGN_INT_VALUE(var)				\
{								\
    unsigned int tmp = strtoul(arg,&endptr,10);			\
    if(endptr && *endptr) {					\
        log_1("config (%s:%d): keyword %s: invalid number: %s",	\
    	    config_filename,linenumber,keyword,arg);		\
	result = -1;						\
    } else							\
	config->var = tmp;					\
}

    cfg = fopen( config_filename, "r" );
    if( !cfg )
    {
        if( !strcmp( config_filename, DEFAULT_CONFIGFILE ) )
        {
            log_1( "Cannot open default config file %s, using defaults", config_filename );
            return 0;
        }
        else
        {
            log_1( "Could not open config file: %s: %m", config_filename );
            return -1;
        }
    }

    linenumber = 1;
    while( !feof( cfg ) )
    {
        if( !fgets( line, CONFIGLINESIZE, cfg ) ) break;
        if( *line == '#' || *line == ';' || *line == '\n' ) continue;
        for( p = &line[strlen( line ) - 1]; *p <= 32 && p >= line; p-- )
            *p = '\0';
        keyword = strtok( line, " \t" );

        /* only FilterCmdline has space delimited arguments */
        if( keyword != NULL &&
            ( !strcasecmp( keyword, "FilterCmdline" ) ||
              !strcasecmp( keyword, "FilterParam" ) ) )
        {
            arg = keyword + strlen( keyword ) + 1;
        }
        else
            arg = strtok( NULL, " \t" );

        if( !keyword || !*keyword || !arg || !*arg )
        {
            log_1( "Invalid config line %s:%d: '%s'",
                   config_filename, linenumber, line );
            result = -1;
        }
        if( !strcasecmp( keyword, "FilterPath" ) )
        {
            if( strlen( arg ) >= PATHSIZE )
            {
                log_1( "config (%s:%d): keyword %s: argument is too long",
                       config_filename, linenumber, keyword );
                result = -1;
            }
            else
                strcpy( config->filter_path, arg );
        }
        else if( !strcasecmp( keyword, "FilterCmdline" ) ||
                 !strcasecmp( keyword, "FilterParam" ) )
        {
            if( strlen( arg ) >= CMDLINESIZE )
            {
                log_1( "config (%s:%d): keyword %s: argument is too long",
                       config_filename, linenumber, keyword );
                result = -1;
            }
            else
                strcpy( config->filter_cmdline, arg );
        }
        else if( !strcasecmp( keyword, "Listen" ) ||
                 !strcasecmp( keyword, "Bind" ) )
        {
            if( strlen( arg ) >= PATHSIZE )
            {
                log_1( "config (%s:%d): keyword %s: argument is too long",
                       config_filename, linenumber, keyword );
                result = -1;
            }
            else
                strcpy( config->listen, arg );
        }
        else if( !strcasecmp( keyword, "PidFile" ) )
        {
            if( strlen( arg ) >= PATHSIZE )
            {
                log_1( "config (%s:%d): keyword %s: argument is too long",
                       config_filename, linenumber, keyword );
                result = -1;
            }
            else
                strncpy( config->pidfile, arg, PATHSIZE );
        }
        else if( !strcasecmp( keyword, "Start" ) ||
                 !strcasecmp( keyword, "StartFilters" ) )
        {
            CONFIG_ASSIGN_INT_VALUE( start_filters );
        }
        else if( !strcasecmp( keyword, "MaxFilters" ) )
        {
            CONFIG_ASSIGN_INT_VALUE( max_filters );
        }
        else if( !strcasecmp( keyword, "MinSpareFilters" ) )
        {
            CONFIG_ASSIGN_INT_VALUE( min_spare_filters );
        }
        else if( !strcasecmp( keyword, "SyslogFacility" ) ||
                 !strcasecmp( keyword, "LogFacility" ) )
        {
            if( !strcasecmp( arg, "auth" ) ) config->syslog_facility = LOG_AUTH;
            else if( !strcasecmp( arg, "cron" ) ) config->syslog_facility = LOG_CRON;
            else if( !strcasecmp( arg, "daemon" ) ) config->syslog_facility = LOG_DAEMON;
            else if( !strcasecmp( arg, "kern" ) ) config->syslog_facility = LOG_KERN;
            else if( !strcasecmp( arg, "local0" ) ) config->syslog_facility = LOG_LOCAL0;
            else if( !strcasecmp( arg, "local1" ) ) config->syslog_facility = LOG_LOCAL1;
            else if( !strcasecmp( arg, "local2" ) ) config->syslog_facility = LOG_LOCAL2;
            else if( !strcasecmp( arg, "local3" ) ) config->syslog_facility = LOG_LOCAL3;
            else if( !strcasecmp( arg, "local4" ) ) config->syslog_facility = LOG_LOCAL4;
            else if( !strcasecmp( arg, "local5" ) ) config->syslog_facility = LOG_LOCAL5;
            else if( !strcasecmp( arg, "local6" ) ) config->syslog_facility = LOG_LOCAL6;
            else if( !strcasecmp( arg, "local7" ) ) config->syslog_facility = LOG_LOCAL7;
            else if( !strcasecmp( arg, "lpr" ) ) config->syslog_facility = LOG_LPR;
            else if( !strcasecmp( arg, "mail" ) ) config->syslog_facility = LOG_MAIL;
            else if( !strcasecmp( arg, "news" ) ) config->syslog_facility = LOG_NEWS;
            else if( !strcasecmp( arg, "syslog" ) ) config->syslog_facility = LOG_SYSLOG;
            else if( !strcasecmp( arg, "user" ) ) config->syslog_facility = LOG_USER;
            else if( !strcasecmp( arg, "uucp" ) ) config->syslog_facility = LOG_UUCP;
            else
            {
                log_1( "config (%s:%d): keyword %s: invalid value: %s",
                       config_filename, linenumber, keyword, arg );
                result = -1;
            }
        }
        else if( !strcasecmp( keyword, "LogLevel" ) ||
                 !strcasecmp( keyword, "Debug" ) )
        {
            CONFIG_ASSIGN_INT_VALUE( log_level );
        }
        else if( !strcasecmp( keyword, "User" ) )
        {
            /* for compatibility reason */
        }
        else
        {
            log_1( "config (%s:%d): invalid config keyword: '%s'",
                   config_filename, linenumber, keyword );
            result = -1;
        }
    }
    
    // Get environment variables&
    const char * max_filters = getenv( "APS_MAX_FILTERS" );
	if( max_filters ) {
		unsigned int tmp = strtoul( max_filters, &endptr, 10 );
		if( endptr && *endptr ) {
			log_1( "environment: APS_MAX_FILTERS: invalid number: %s", max_filters );
			result = -1;
		} else
			config->max_filters = tmp;
	}
	
	const char * start_filters = getenv( "APS_START_FILTERS" );
	if( start_filters ) {
		unsigned int tmp = strtoul( start_filters, &endptr, 10 );
		if( endptr && *endptr ) {
			log_1( "environment: APS_START_FILTERS: invalid number: %s", start_filters );
			result = -1;
		} else
			config->start_filters = tmp;
	}
	
	const char * min_filters = getenv( "APS_MIN_FILTERS" );
	if( min_filters ) {
		unsigned int tmp = strtoul( min_filters, &endptr, 10 );
		if( endptr && *endptr ) {
			log_1( "environment: APS_MIN_FILTERS: invalid number: %s", min_filters );
			result = -1;
		} else
			config->min_spare_filters = tmp;
	}
	
	const char * log_level = getenv( "APS_LOG_LEVEL" );
	if( log_level ) {
		unsigned int tmp = strtoul( log_level, &endptr, 10 );
		if( endptr && *endptr ) {
			log_1( "environment: APS_LOG_LEVEL: invalid number: %s", log_level );
			result = -1;
		} else
			config->log_level = tmp;
	}
    
    fclose( cfg );
    return result;
    #undef CONFIG_ASSIGN_INT_VALUE
}

void config_check_values( config_t *config )
{
    if( !( strlen( config->filter_path ) > 0 ) )
    {
        log_1( "config_check: FilterPath is empty: using '"DEFAULT_FILTER_PATH"'" );
        strcpy( config->filter_path, DEFAULT_FILTER_PATH );
    }
    if( !( strlen( config->listen ) > 5 ) )
    {
        log_1( "config_check: looks like Bind/Listen is malformed: using '"DEFAULT_LISTEN"'" );
        strcpy( config->listen, DEFAULT_LISTEN );
    }
    if( !( config->max_filters > 0 ) )
    {
        log_1( "config_check: MaxFilters must be greater than zero: using %d",
               DEFAULT_MAX_FILTERS );
        config->max_filters = DEFAULT_MAX_FILTERS;
    }
    if( !( config->start_filters <= config->max_filters ) )
    {
        log_1( "config_check: StartFilters must be lesser than MaxFilters: using %d",
               config->max_filters );
        config->start_filters = config->max_filters;
    }
    if( !( config->min_spare_filters < config->max_filters ) )
    {
        log_1( "config_check: MinSpareFilters must be lesser than MaxFilters: using %d",
               config->max_filters );
        config->min_spare_filters = config->max_filters;
    }
}

/* checking pidfile */

int init_pidfile( config_t *config )
{
    FILE *pf;
    pf = fopen( config->pidfile, "w" );
    if( pf == NULL )
    {
        log_1( "could not create pidfile: %s: %m", config->pidfile );
        return -1;
    }
    fprintf( pf, "%d\n", getpid( ) );
    fclose( pf );
    return 0;

}

int check_pidfile( config_t *config )
{
    FILE *pf;
    char buf[32];
    pid_t pid;

    pf = fopen( config->pidfile, "r" );
    if( pf )
    {
        *buf = '\0';
        fgets( buf, 32, pf );
        fclose( pf );
        pid = atoi( buf );
        if( pid != 0 && kill( pid, 0 ) == 0 )
        {
            log_1( "pidfile '%s' already exists and process (%d) running",
                   config->pidfile, pid );
            return -1;
        }
    }
    return 0;
}

int config_check_execution( config_t *config )
{
    if( access( config->filter_path, X_OK ) )
    {
        log_1( "could not find executable filter: %s", config->filter_path );
        return -1;
    }
    return 0;
}

int aps_config_load( config_t *config )
{
    if( !config ) return -1;
    config_set_defaults( config );
    if( config_load_file( config ) ) return -1;
    config_check_values( config );
    return config_check_execution( config );
}

char** parse_cmdline( )
{
    char **argv;
    char *arg;
    int p;
    char *slashp;

    argv = (char**)malloc( MAX_CMDLINE_ARGUMENTS * sizeof (char*) );
    if( argv == NULL )
    {
        log_1( "could not allocate memory (%d bytes) for command line arguments array",
               MAX_CMDLINE_ARGUMENTS * sizeof (char*) );
        return NULL;
    }
    memset( argv, 0, MAX_CMDLINE_ARGUMENTS * sizeof (char*) );

    slashp = strrchr( STATE.config.filter_path, '/' );
    if( slashp && *slashp ) slashp++;
    if( !slashp || !*slashp )
        slashp = STATE.config.filter_path;

    argv[0] = strdup( slashp );
    if( argv[0] == NULL )
    {
        log_1( "could not allocate memory (%d bytes) for command line argument",
               strlen( STATE.config.filter_path ) + 1 );
    }

    arg = strtok( STATE.config.filter_cmdline, " \t" );
    for( p = 1; arg != NULL && p < MAX_CMDLINE_ARGUMENTS - 1; p++ )
    {
        argv[p] = strdup( arg );
        if( argv[p] == NULL )
        {
            log_1( "could not allocate memory (%d bytes) for command line argument",
                   strlen( arg ) + 1 );
        }
        arg = strtok( NULL, " \t" );
    }
    argv[p] = NULL;

    return argv;
}

/* listening descriptor initialization */

int parse_listening_description( char *description, struct sockaddr *addr )
{
    int result = 0;
    char tmp[PATHSIZE];
    struct sockaddr_in *inaddr = (struct sockaddr_in *)addr;
    struct sockaddr_un *unaddr = (struct sockaddr_un *)addr;
    char *field, *field2;
    struct hostent *host;
    unsigned int port;
    char *endptr;

    memset( addr, 0, SOCKADDR_SIZE );

    strcpy( tmp, description );
    field = strtok( tmp, ":" );

    if( field && !strcmp( field, "tcp" ) )
    {
        inaddr->sin_family = AF_INET;
        field = strtok( NULL, ":" );
        field2 = strtok( NULL, ":" );
        if( field )
        {
            if( field2 )
            {
                if( ( *field >= '0' && *field <= '9' &&
                      !inet_aton( field, &inaddr->sin_addr ) ) ||
                    *field < '0' || *field > '9' )
                {
                    host = gethostbyname( field );
                    if( !host )
                    {
                        switch( errno )
                        {
                            case HOST_NOT_FOUND:
                                log_1( "host not found: %s", field );
                            case NO_ADDRESS:
                                log_1( "host has not IP address: %s", field );
                            default:
                                log_1( "gethostbyname: %s: %m", field );
                        }
                        result = -1;
                    }
                    else if( host->h_length != 1 )
                    {
                        log_1( "hostname references to multiple IP addresses: %s",
                               field );
                        result = -1;
                    }
                    else
                        inaddr->sin_addr.s_addr = *( host->h_addr );
                }
                field = field2;
            }
            else
                inaddr->sin_addr.s_addr = INADDR_ANY;
            if( !result )
            {
                if( field )
                {
                    port = strtoul( field, &endptr, 10 );
                    if( !endptr || !*endptr )
                    {
                        inaddr->sin_port = htons( port );
                    }
                    else
                        result = -1;
                }
                else
                    result = -1;
            }
        }
    }
    else if( field && !strcmp( field, "unix" ) )
    {
        unaddr->sun_family = AF_UNIX;
        field = strtok( NULL, ":" );
        if( field )
        {
            if( strlen( field ) >= UNIX_PATH_MAX )
            {
                log_1( "unix socket path too long: %s", field );
                result = -1;
            }
            else
                strcpy( unaddr->sun_path, field );
        }
        else
            result = -1;
    }
    else
        result = -1;

    if( result )
        log_1( "malformed Bind/Listen: '%s'", description );
    return result;
}

int check_listening_description( config_t *config )
{
    unsigned char addr_buf[SOCKADDR_SIZE];
    return parse_listening_description( config->listen, (struct sockaddr *)&addr_buf );
}

int init_listening_descriptor( char *description, char *old_description, int old_fd )
{
    unsigned char addr_buf[SOCKADDR_SIZE];
    unsigned char old_addr_buf[SOCKADDR_SIZE];
    struct sockaddr *addr = (struct sockaddr *)&addr_buf;
    struct sockaddr *old_addr = (struct sockaddr *)&old_addr_buf;
    char *path;
    int fd = -1;
    int t = 1;
    socklen_t socklen;

    if( parse_listening_description( description, addr ) ) return old_fd;

    if( old_description != NULL )
    {
        parse_listening_description( old_description, old_addr );
        if( !memcmp( addr, old_addr, SOCKADDR_SIZE ) ) return old_fd;
    }

    switch( addr->sa_family )
    {
        case AF_UNIX:
            path = ( (struct sockaddr_un *)addr )->sun_path;
            if( !access( path, F_OK ) )
            {
                if( unlink( path ) )
                {
                    log_1( "could not unlink: %s: %m", path );
                    return old_fd;
                }
            }
            fd = socket( PF_UNIX, SOCK_STREAM, 0 );
            socklen = sizeof (struct sockaddr_un);
            break;

        case AF_INET:
            fd = socket( PF_INET, SOCK_STREAM, 0 );
            socklen = sizeof (struct sockaddr_in);
            if( -1 == setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, &t, sizeof (t ) ) )
            {
                log_1( "could not setsockopt (TCP_NODELAY): %s", strerror( errno ) );
                close( fd );
                return old_fd;
            }
            break;

        default:
            log_1( "INTERNAL ERROR: invalid socket family: %d", addr->sa_family );
            return old_fd;
    }

    if( fd == -1 )
    {
        log_1( "could not initialize socket: %m" );
        return old_fd;
    }

    if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, addr, socklen ) )
    {
        log_1( "could not setsockopt (SO_REUSEADDR): %m" );
        close( fd );
        return old_fd;
    }

    if( bind( fd, addr, socklen ) )
    {
        log_1( "could not bind socket: %m" );
        close( fd );
        return old_fd;
    }

    if( listen( fd, STATE.config.max_filters * 2 ) )
    {
        log_1( "could not listen socket: %m" );
        close( fd );
        return old_fd;
    }

    if( old_fd > 0 )
    {
        close( old_fd );
        if( old_addr->sa_family == AF_UNIX )
        {
            if( unlink( ( (struct sockaddr_un *)old_addr )->sun_path ) )
                log_1( "could not unlink: %s: %m",
                       ( (struct sockaddr_un *)old_addr )->sun_path );
        }
    }

    return fd;
}

void close_listening_descriptor( )
{
    unsigned char addr_buf[SOCKADDR_SIZE];
    struct sockaddr *addr = (struct sockaddr *)&addr_buf;

    if( STATE.listen_fd < 0 ) return;

    parse_listening_description( STATE.config.listen, addr );

    close( STATE.listen_fd );
    STATE.listen_fd = -1;

    if( addr->sa_family == AF_UNIX )
    {
        if( unlink( ( (struct sockaddr_un *)addr )->sun_path ) )
            log_1( "could not unlink: %s: %m",
                   ( (struct sockaddr_un *)addr )->sun_path );
    }
}

int non_blocking( int fd, int on )
{
    int flags;

    if( ( flags = fcntl( fd, F_GETFL, 0 ) ) < 0 )
    {
        log_1( "fcntl: get flags: %m" );
        return -1;
    }
    if( fcntl( fd, F_SETFL, on ? flags | O_NONBLOCK : flags & ~O_NONBLOCK ) < 0 )
    {
        log_1( "fcntl: set non-blocking flag %s: %m", on ? "on" : "off" );
        return -1;
    }
    return (( flags & O_NONBLOCK ) != 0 );
}

int close_on_exec( int fd, int on )
{
    int flags;

    if( ( flags = fcntl( fd, F_GETFD, 0 ) ) < 0 )
        log_1( "fcntl: get flags: %m" );
    if( fcntl( fd, F_SETFD, on ? flags | FD_CLOEXEC : flags & ~FD_CLOEXEC ) < 0 )
        log_1( "fcntl: set close-on-exec flag %s: %m", on ? "on" : "off" );
    return (( flags & FD_CLOEXEC ) != 0 );
}

/* status pipe */

int sane_socketpair( int domain, int type, int protocol, int *result )
{
    static int socketpair_ok_errors[] = {
                                         EINTR,
                                         0,
    };
    int count;
    int err;
    int ret;

    /*
     * Solaris socketpair() can fail with EINTR.
     */
    while( ( ret = socketpair( domain, type, protocol, result ) ) < 0 )
    {
        for( count = 0; /* void */; count++ )
        {
            if( ( err = socketpair_ok_errors[count] ) == 0 )
                return (ret );
            if( errno == err )
            {
                log_3( "socketpair: %m (trying again)" );
                usleep( 300000 );
                break;
            }
        }
    }
    return (ret );
}

int duplex_pipe( int *fds )
{
    #ifdef HAS_DUPLEX_PIPE
    return (pipe( fds ) );
    #else
    return (sane_socketpair( AF_UNIX, SOCK_STREAM, 0, fds ) );
    #endif
}

int open_status_pipe( )
{
    if( duplex_pipe( STATE.status_fd ) )
    {
        log_1( "could not create status pipe: %m" );
        return -1;
    }
    non_blocking( STATE.status_fd[0], 0 );
    close_on_exec( STATE.status_fd[0], 1 );
    close_on_exec( STATE.status_fd[1], 1 );
    return 0;
}

void close_status_pipe( )
{
    if( STATE.status_fd[0] > 0 )
    {
        close( STATE.status_fd[0] );
        STATE.status_fd[0] = -1;
    }
    if( STATE.status_fd[1] > 0 )
    {
        close( STATE.status_fd[1] );
        STATE.status_fd[1] = -1;
    }
}



/* timeval functions */

#ifndef timercmp
#define timercmp(tvp, uvp, cmp)		\
    ((tvp)->tv_sec cmp (uvp)->tv_sec ||	\
    ((tvp)->tv_sec == (uvp)->tv_sec &&	\
     (tvp)->tv_usec cmp (uvp)->tv_usec))
#endif

#define timerdiffmin(tvp,uvp)		\
    (((tvp)->tv_sec - (uvp)->tv_sec 	\
      - ((tvp)->tv_usec < (uvp)->tv_usec ? 1 : 0))/60)

#define timerdiffsec(tvp,uvp)		\
    (((tvp)->tv_sec - (uvp)->tv_sec	\
      + ((tvp)->tv_usec < (uvp)->tv_usec ? 1 : 0))%60)

#define timerdiffmsec(tvp,uvp)		\
    (((tvp)->tv_usec - (uvp)->tv_usec	\
      + ((tvp)->tv_usec < (uvp)->tv_usec ? 1000000 : 0))/1000)

#define timermin(tvp)	\
    (((tvp)->tv_sec)/60)

#define timersec(tvp)	\
    (((tvp)->tv_sec)%60)

#define timermsec(tvp)	\
    (((tvp)->tv_usec)/1000)

void timerincrement( struct timeval *tvp, long usec )
{
    tvp->tv_usec += ( usec % 1000000 );
    if( tvp->tv_usec > 1000000 )
    {
        tvp->tv_usec -= 1000000;
        tvp->tv_sec++;
    }
    tvp->tv_sec += usec / 1000000;
}

void timerdecrement( struct timeval *tvp, long usec )
{
    tvp->tv_usec -= ( usec % 1000000 );
    if( tvp->tv_usec < 0 )
    {
        tvp->tv_usec += 1000000;
        tvp->tv_sec--;
    }
    tvp->tv_sec -= usec / 1000000;
}

/* filters array handling */

int allocate_filters( )
{
    size_t size = STATE.config.max_filters * sizeof (filter_t );

    STATE.filters_alloc = STATE.config.max_filters;
    STATE.filters = (filter_t*)malloc( size );
    if( STATE.filters )
        memset( STATE.filters, 0, size );
    else
    {
        log_1( "could not allocate memory (%d bytes) for filters array", size );
        return -1;
    }

    return 0;
}

void free_filters( )
{
    if( STATE.filters )
    {
        free( STATE.filters );
        STATE.filters = NULL;
        STATE.filters_alloc = 0;
    }
}

int reallocate_filters( size_t new_amount )
{
    filter_t *filters;

    if( STATE.filters_alloc < new_amount )
    {
        filters = (filter_t*)realloc( STATE.filters, new_amount * sizeof (filter_t ) );
        if( filters == NULL )
        {
            log_1( "could not reallocate memory: %m" );
            return -1;
        }
        memset( &filters[STATE.filters_alloc], 0,
                ( new_amount - STATE.filters_alloc ) * sizeof (filter_t ) );
        STATE.filters = filters;
        STATE.filters_alloc = new_amount;
        return 0;
    }
    return 0;
}

void spawn_filter( struct timeval* timeofday )
{
    int i;
    filter_t *filter = NULL;
    pid_t pid;
    char **argv;

    for( i = 0; i < STATE.filters_alloc; i++ )
        if( STATE.filters[i].pid <= 0 )
        {
            filter = &STATE.filters[i];
            break;
        }
    if( !filter ) return;

    STATE.start_time = *timeofday;

    pid = fork( );

    switch( pid )
    {
        case -1:
            log_1( "could not spawn filter: fork: %m" );
            break;

        case 0:

            for( i = 0; i < NUM_DUMMY_DESCRIPTORS; i++ )
                close( dummy_descriptors[i] );

            close( STATE.status_fd[0] );
            if( dup2( STATE.status_fd[1], MASTER_STATUS_FD ) == -1 )
            {
                log_1( "could not dup2 %d to %d",
                       STATE.status_fd[1], MASTER_STATUS_FD );
                exit( 1 );
            }
            close( STATE.status_fd[1] );
            if( close_on_exec( MASTER_STATUS_FD, 0 ) )
            {
                log_1( "Cannot set close-on-exec-flag on %d: %m", MASTER_STATUS_FD );
                exit( 1 );
            }
            if( dup2( STATE.listen_fd, MASTER_LISTEN_FD ) == -1 )
            {
                log_1( "could not dup2 %d to %d",
                       STATE.listen_fd, MASTER_LISTEN_FD );
                exit( 1 );
            }
            close( STATE.listen_fd );
            if( close_on_exec( MASTER_LISTEN_FD, 0 ) )
            {
                log_1( "Cannot set close-on-exec-flag on %d: %m", MASTER_LISTEN_FD );
                exit( 1 );
            }
            close( STATE.listen_fd );
            argv = parse_cmdline( );
            if( argv == NULL )
            {
                log_1( "Could not guess cmdline,exiting" );
                exit( 1 );
            }
            execv( STATE.config.filter_path, argv );
            log_1( "could not execute %s: %m", STATE.config.filter_path );

            exit( 1 );
    }

    filter->pid = pid;
    filter->execution_time = *timeofday;
    filter->avail = 1;
    filter->state = run;
    filter->usage = 0;
    filter->state_timestamp = *timeofday;

    STATE.start--;
    STATE.num_filters++;
    STATE.spare_filters++;

    log_3( "filter (%d) has been started", pid );

    if( STATE.num_filters == STATE.config.max_filters )
        log_2( "filter process limit has been reached" );
}

void hup_filters( )
{
    struct sigaction sa;
    pid_t pid = getpid( );


    // remove our sighup handler, then kill all group
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;
    sa.sa_handler = SIG_IGN;
    if( sigaction( SIGHUP, &sa, (struct sigaction*)0 ) < 0 )
    {
        log_1( "sigaction: %m" );
        return;
    }
    if( kill( -pid, SIGHUP ) < 0 )
    {
        log_1( "kill process group: %m" );
        return;
    }
    // restore handler
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;
    sa.sa_handler = signal_hup_handler;
    if( sigaction( SIGHUP, &sa, (struct sigaction*)0 ) < 0 )
    {
        log_1( "sigaction: %m" );
        return;
    }
}

void check_filter_statuses( struct timeval* timeofday )
{
    fd_set rfds;
    int maxfd;
    struct timeval timeout;
    MASTER_STATUS status;
    int i, sel_status;

    if( STATE.status_fd[0] < 0 ) return;

    FD_ZERO( &rfds );
    FD_SET( STATE.status_fd[0], &rfds );
    maxfd = STATE.status_fd[0];

    if( STATE.start == 0 && STATE.spare_filters == 0 &&
        STATE.num_filters < STATE.config.max_filters )
    {
        FD_SET( STATE.listen_fd, &rfds );
        if( STATE.listen_fd > maxfd )
            maxfd = STATE.listen_fd;
    }

    if( STATE.start > 0 || STATE.spare_filters == 0 )
    {
        //	    timersub(&STATE.start_time,timeofday,&timeout);
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;
    }
    else
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
    }

    errno = 0;
    sel_status = select( maxfd + 1, &rfds, NULL, NULL, &timeout );
    if( sel_status > 0 )
    {
        if( FD_ISSET( STATE.status_fd[0], &rfds ) )
        {
            if( nbread( STATE.status_fd[0], &status, sizeof (status ), 1000000 ) == sizeof (status ) )
            {
                for( i = 0; i < STATE.filters_alloc; i++ )
                    if( STATE.filters[i].pid == status.pid )
                    {
                        if( !status.avail )
                            STATE.filters[i].usage++;
                        if( !STATE.filters[i].avail && status.avail )
                            STATE.spare_filters++;
                        if( STATE.filters[i].avail && !status.avail )
                        {
                            STATE.spare_filters--;
                        }
                        STATE.filters[i].avail = status.avail;
                        break;
                    }
            }
        }

        if( FD_ISSET( STATE.listen_fd, &rfds ) )
        {
            STATE.start++;
        }

    }
    else if( sel_status < 0 )
    {
        if( errno != EINTR && errno != 0 )
        {
            log_1( "select: %m" );
        }
    }
}

/* childs handling */

void bury_childs( struct timeval* timeofday )
{
    pid_t pid;
    int status;
    struct rusage rusage;
    int i;
    filter_t *filter;

    while( childs_exited )
    {
        do
        {
            pid = wait3( &status, WNOHANG, &rusage );
        }
        while( pid == -1 && errno == EINTR );
        if( errno == ECHILD )
            childs_exited = 0;
        if( pid <= 0 ) break;

        for( i = 0; i < STATE.filters_alloc; i++ )
            if( STATE.filters[i].pid == pid )
            {
                filter = &STATE.filters[i];
                STATE.num_filters--;
                if( filter->avail )
                    STATE.spare_filters--;

                if( ( WIFEXITED( status ) && WEXITSTATUS( status ) != 0 ) ||
                    ( WIFSIGNALED( status ) && (
                                                 ( WTERMSIG( status ) == SIGHUP && filter->state != sighup ) ||
                                                 ( WTERMSIG( status ) == SIGTERM && filter->state != sigterm ) ||
                                                 ( WTERMSIG( status ) != SIGHUP && WTERMSIG( status ) != SIGTERM )
                                                 ) ) )
                {
                    if( WIFEXITED( status ) )
                        log_4( "filter process %d has been terminated with exit status %d",
                               pid, WEXITSTATUS( status ) );
                    if( WIFSIGNALED( status ) )
                        log_4( "filter process %d has been terminated on signal %d",
                               pid, WTERMSIG( status ) );
                    if( ( ( WIFSIGNALED( status ) && WTERMSIG( status ) != SIGTERM ) || WIFEXITED( status ) ) && filter->usage == 0 )
                    {
                        log_1( "pid %d exited with error without processed messages, delaying next start to %d msec", pid, FILTER_ERROR_DELAY / 1000 );
                        timerincrement( &STATE.start_time, FILTER_ERROR_DELAY );
                    }
                }
                log_3( "pid %d end", pid );
                log_5( "resource usage for filter process %d: "
                       "real %dm%d.%03d: "
                       "user %dm%d.%03d: "
                       "sys %dm%d.%03d: "
                       "maxrss %d",
                       pid,
                       timerdiffmin( timeofday, &filter->execution_time ),
                       timerdiffsec( timeofday, &filter->execution_time ),
                       timerdiffmsec( timeofday, &filter->execution_time ),
                       timermin( &rusage.ru_utime ),
                       timersec( &rusage.ru_utime ),
                       timermsec( &rusage.ru_utime ),
                       timermin( &rusage.ru_stime ),
                       timersec( &rusage.ru_stime ),
                       timermsec( &rusage.ru_stime ),
                       rusage.ru_maxrss );
                memset( filter, 0, sizeof (filter_t ) );
                break;
            }
    }

}

/* configuration rereading */

void reread_config( struct timeval* timeofday, int restart_childs )
{
    config_t config;
    int new_listen_fd = -1;

    log_3( "rereading confinguration" );
    if( aps_config_load( &config ) ) return;

    new_listen_fd = init_listening_descriptor( config.listen,
                                               STATE.config.listen,
                                               STATE.listen_fd );
    if( new_listen_fd != STATE.listen_fd )
    {
        restart_childs = 1;
        close_listening_descriptor( );
    }
    STATE.listen_fd = new_listen_fd;
    if( restart_childs )
    {
        close_status_pipe( );
        open_status_pipe( );
    }


    if( config.syslog_facility != STATE.config.syslog_facility )
    {
        closelog( );
        openlog( "ap-process-server", LOG_PID, config.syslog_facility );
    }

    if( config.max_filters > STATE.filters_alloc )
    {
        if( reallocate_filters( config.max_filters ) )
        {
            config.max_filters = STATE.filters_alloc;
            if( config.start_filters > config.max_filters )
                config.start_filters = config.max_filters;
            if( config.min_spare_filters > config.max_filters )
                config.min_spare_filters = config.max_filters;
        }
    }
    /* preserve old PID path */
    strncpy( config.pidfile, STATE.config.pidfile, PATHSIZE );
    memcpy( &STATE.config, &config, sizeof (config_t ) );

}

/* main loop */

void main_loop( )
{
    int terminate_loop = 0;
    struct timeval timeofday;
    int t;

    while( !terminate_loop || STATE.num_filters > 0 )
    {
        gettimeofday( &timeofday, NULL );

        if( terminate_process_server && !terminate_loop )
        {
            close_status_pipe( );

            close_listening_descriptor( );

            terminate_loop = 1;
        }

        if( childs_exited )
            bury_childs( &timeofday );

        if( terminate_loop )
        {
            if( STATE.num_filters > 0 )
                usleep( 500 );
            continue;
        }

        if( STATE.filters == NULL || STATE.status_fd[0] < 0 || STATE.listen_fd < 0 )
        {
            terminate_loop = 1;
            close_status_pipe( );
            continue;
        }


        if( STATE.start > 0 && STATE.num_filters < STATE.config.max_filters &&
            timercmp( &STATE.start_time, &timeofday, < ) )
            spawn_filter( &timeofday );

        check_filter_statuses( &timeofday );

        if( reread_configuration )
        {
            reread_configuration = 0;
            t = restart_filters;
            restart_filters = 0;
            if( t )
                hup_filters( );
            reread_config( &timeofday, t );
        }

        if( ( STATE.start == 0 ) &&
            ( STATE.spare_filters < STATE.config.min_spare_filters ) &&
            ( STATE.num_filters < STATE.filters_alloc )
            )
        {
            STATE.start++;
        }
        #if 0
        if( ( STATE.filters_alloc > STATE.config.max_filters ) &&
            ( STATE.num_filters < STATE.config.max_filters ) )
            reallocate_filters( STATE.config.max_filters );
        #endif
    }

}

/* the Main */

int main( int argc, char *argv[] )
{
    int i;
    int fd;
    struct sigaction action;
    static int sigs[] = {
                         SIGINT, SIGQUIT, SIGILL, SIGBUS, SIGSEGV, SIGTERM,
    };
    static int ign_sigs[] = {
                             SIGPIPE, SIGUSR2,
    };

    if( argc > 1 )
    {
        if( *argv[1] == '-' )
        {
            fprintf( stderr, "usage: %s [/path/to/config/file]\n", argv[0] );
            exit( 1 );
        }
        if( strlen( argv[1] ) >= PATHSIZE )
        {
            fprintf( stderr, "Path to config file is too long\n" );
            exit( 1 );
        }
        strcpy( config_filename, argv[1] );
    }
    for( fd = 3; fd < 500; fd++ )
        close( fd );

    memset( &STATE, 0, sizeof (STATE ) );
    STATE.listen_fd = STATE.status_fd[0] = STATE.status_fd[1] = -1;

    openlog( "ap-process-server", LOG_PID, DEFAULT_SYSLOG_FACILITY );

    if( aps_config_load( &STATE.config ) )
    {
        closelog( );
        exit( 1 );
    }

    closelog( );
    openlog( "ap-process-server", LOG_PID, STATE.config.syslog_facility );

    if( check_pidfile( &STATE.config ) )
        exit( 1 );

    if( check_listening_description( &STATE.config ) )
        exit( 1 );


    STATE.start = STATE.config.start_filters;

    // daemonize:
    switch( fork( ) )
    {
        case -1:
            log_1( "could not daemonize: %m" );
            free_filters( );
            close_status_pipe( );
            close_listening_descriptor( );
            closelog( );
            exit( 1 );
        case 0:
            break;
        default:
            _exit( 0 );
    }
    if( setsid( ) == -1 )
    {
        log_1( "could not set session group: %m" );
        free_filters( );
        close_status_pipe( );
        close_listening_descriptor( );
        closelog( );
        exit( 1 );
    }

    for( fd = 0; fd < 3; fd++ )
    {
        close( fd );
        if( open( "/dev/null", O_RDWR, 0 ) != fd )
        {
            log_1( "Cannot reopen fd %d", fd );
            exit( 1 );
        }
    }

    for( fd = 0; fd < NUM_DUMMY_DESCRIPTORS; fd++ )
    {
        if( close_on_exec( dup( 0 ), 1 ) < 0 )
        {
            log_1( "Cannot dup(0): %m" );
            exit( 1 );
        }
    }

    STATE.daemonized = 1;
    init_pidfile( &STATE.config );

    sigemptyset( &action.sa_mask );
    action.sa_flags = 0;
    /*
     * Prepare to kill our children when we receive any of the above signals.
     */
    action.sa_handler = signal_death_handler;
    for( i = 0; i < sizeof (sigs ) / sizeof (sigs[0] ); i++ )
        if( sigaction( sigs[i], &action, (struct sigaction *)0 ) < 0 )
            log_1( "sigaction(%d): %m", sigs[i] );

    /* ignored signals */
    for( i = 0; i < sizeof (ign_sigs ) / sizeof (ign_sigs[0] ); i++ )
        if( signal( ign_sigs[i], SIG_IGN ) < 0 )
            log_1( "signal(%d): %m", ign_sigs[i] );

    /* specially handled signals */
    #ifdef SA_RESTART
    action.sa_flags |= SA_RESTART;
    #endif
    action.sa_handler = signal_hup_handler;
    if( sigaction( SIGHUP, &action, (struct sigaction *)0 ) < 0 )
        log_1( "sigaction(SIGHUP): %m", ign_sigs[i] );

    action.sa_handler = signal_usr1_handler;
    if( sigaction( SIGUSR1, &action, (struct sigaction *)0 ) < 0 )
        log_1( "sigaction(SIGHUP): %m", ign_sigs[i] );

    action.sa_handler = signal_chld_handler;
    if( sigaction( SIGCHLD, &action, (struct sigaction *)0 ) < 0 )
        log_1( "sigaction(SIGHUP): %m", ign_sigs[i] );

    for( i = 0; i < NUM_DUMMY_DESCRIPTORS; i++ )
        dummy_descriptors[i] = open( "/dev/null", 0 );

    STATE.listen_fd = init_listening_descriptor( STATE.config.listen, NULL, -1 );
    open_status_pipe( );
    allocate_filters( );

    if( STATE.filters == NULL || STATE.status_fd[0] < 0 || STATE.listen_fd < 0 )
    {
        free_filters( );
        close_status_pipe( );
        close_listening_descriptor( );
        closelog( );
        exit( 1 );
    }

    log_3( "started (%d)", getpid( ) );

    main_loop( );

    log_3( "terminated" );

    free_filters( );
    closelog( );

    unlink( STATE.config.pidfile );

    exit( 0 );
}
