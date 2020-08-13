#ifndef __TIME_H
#define __TIME_H

# include <time.h>
#ifndef _WIN32
# include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* strptime replacement. Supports additional format strings:
  %Z - Timezone character name. Only GMT and two local timezones are
	 supported
  %z - timezone in numeric format (+|-hhmm)

  ap_strptime always return struct tm filled with _local_ time
  settings. Use tzset(); before call
*/

char*  ap_strptime(const char *buf, const char *fmt, struct tm *ptm);

/* strftime replacement */
size_t ap_strftime(char *buf, size_t maxsize, const char *fmt, 
                   const struct tm * ptm);

/* Some helpers */

/* timegm replacement, works for UTC tm */
time_t ap_timegm(const struct tm * t);

/* Parse string to time_t, analogue of ap_strptime */
char*  ap_strptime_t(const char *buf, const char *fmt, time_t* t);

/* Format time_t value, analogue of ap_strftime */
size_t ap_strftime_t(char *buf, size_t maxsize, const char *fmt, time_t t);

#ifdef _WIN32
#if !defined(_WINSOCK2API_) && !defined(_WINSOCKAPI_)
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};
#endif
struct ap_timezone {
        int     tz_minuteswest; /* minutes west of Greenwich */
        int     tz_dsttime;     /* type of dst correction */
};
int gettimeofday(struct timeval * tp, struct ap_timezone * tzp/*not used anywhere*/); 
#endif

#ifdef __cplusplus
};
#endif

#endif /*__TIME_H*/
