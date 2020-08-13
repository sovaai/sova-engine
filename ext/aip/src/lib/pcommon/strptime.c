/*
 * Powerdog Industries kindly requests feedback from anyone modifying
 * this function:
 *
 * Date: Thu, 05 Jun 1997 23:17:17 -0400  
 * From: Kevin Ruddy <kevin.ruddy@powerdog.com>
 * To: James FitzGibbon <james@nexis.net>
 * Subject: Re: Use of your strptime(3) code (fwd)
 * 
 * The reason for the "no mod" clause was so that modifications would
 * come back and we could integrate them and reissue so that a wider 
 * audience could use it (thereby spreading the wealth).  This has   
 * made it possible to get strptime to work on many operating systems.
 * I'm not sure why that's "plain unacceptable" to the FreeBSD team.
 * 
 * Anyway, you can change it to "with or without modification" as
 * you see fit.  Enjoy.                                          
 * 
 * Kevin Ruddy
 * Powerdog Industries, Inc.
 */
/*
 * Copyright (c) 1994 Powerdog Industries.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *      This product includes software developed by Powerdog Industries.
 * 4. The name of Powerdog Industries may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY POWERDOG INDUSTRIES ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE POWERDOG INDUSTRIES BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef _MSC_VER
#pragma warning(disable:4115) /* 'type' : named type definition in parentheses */
#pragma warning(disable : 4127 4131) /* conditional expression is constant */
#endif                               /* assignment within conditional expression */

# include <_include/wintypes.h>
# include <_include/_string.h>
# include <_include/_time.h>

#if 0
#ifdef LIBC_RCS
static const char rcsid[] =
  "$FreeBSD: src/lib/libc/stdtime/strptime.c,v 1.17.2.3 2002/03/12 17:24:54 phantom Exp $";
#endif

#ifndef lint
#ifndef NOID
static char copyright[] =
"@(#) Copyright (c) 1994 Powerdog Industries.  All rights reserved.";
static char sccsid[] = "@(#)strptime.c    0.1 (Powerdog) 94/03/27";
#endif /* !defined NOID */
#endif /* not lint */
#endif /* 0 */

#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(WIN32) || defined(_WIN32)
#define WIN32FUNC
#define _tzconv(s) conv_win32zone(s)
static const char* conv_win32zone(const char* szSrc);
#else
#define _tzconv(s) (s)
#endif

static char * _int_strptime(const char *, const char *, struct tm *, int *);

#define asizeof(a)    (sizeof (a) / sizeof ((a)[0]))


struct lc_time_T {
    const char    *mon[12];
    const char    *month[12];
    const char    *wday[7];
    const char    *weekday[7];
    const char    *X_fmt;
    const char    *x_fmt;
    const char    *c_fmt;
    const char    *am;
    const char    *pm;
    const char    *date_fmt;
    const char    *alt_month[12];
    const char    *md_order;
    const char    *ampm_fmt;
};


static struct lc_time_T    _C_time_locale = {
    {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    }, {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    }, {
        "Sun", "Mon", "Tue", "Wed",
        "Thu", "Fri", "Sat"
    }, {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    },

    /* X_fmt */
    "%H:%M:%S",

    /*
     * x_fmt
     * Since the C language standard calls for
     * "date, using locale's date format," anything goes.
     * Using just numbers (as here) makes Quakers happier;
     * it's also compatible with SVR4.
     */
    "%m/%d/%y",

    /*
     * c_fmt
     */
    "%a %b %e %H:%M:%S %Y",

    /* am */
    "AM",

    /* pm */
    "PM",

    /* date_fmt */
    "%a %b %e %H:%M:%S %Z %Y",
    
    /* alt_month
     * Standalone months forms for %OB
     */
    {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    },

    /* md_order
     * Month / day order in dates
     */
    "md",

    /* ampm_fmt
     * To determine 12-hour clock format time (empty, if N/A)
     */
    "%I:%M:%S %p"
};


static char *
_int_strptime(const char *buf, const char *fmt, struct tm *ptm, int *got_GMT)
{
    char    c;
    const char *ptr;
    int    i,
        len=0;
    int Ealternative, Oalternative;
    struct lc_time_T *tptr = &_C_time_locale;

    if(!ptm || !fmt ||!buf) return NULL;

    tzset(); /* Always! */
    memset(ptm,0,sizeof(*ptm));
    ptm->tm_isdst = -1; /* We'll override them if timezone recognized */
    ptr = fmt;
    while (*ptr != 0) {
        if (*buf == 0)
            break;

        c = *ptr++;

        if (c != '%') {
            if (isspace((unsigned char)c))
                while (*buf != 0 && isspace((unsigned char)*buf))
                    buf++;
            else if (c != *buf++)
                return 0;
            continue;
        }

        Ealternative = 0;
        Oalternative = 0;
label:
        c = *ptr++;
        switch (c) {
        case 0:
        case '%':
            if (*buf++ != '%')
                return 0;
            break;

        case '+':
            buf = _int_strptime(buf, tptr->date_fmt, ptm,got_GMT);
            if (buf == 0)
                return 0;
            break;

        case 'C':
            if (!isdigit((unsigned char)*buf))
                return 0;

            /* XXX This will break for 3-digit centuries. */
            len = 2;
            for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i < 19)
                return 0;

            ptm->tm_year = i * 100 - 1900;
            break;

        case 'c':
            buf = _int_strptime(buf, tptr->c_fmt, ptm,got_GMT);
            if (buf == 0)
                return 0;
            break;

        case 'D':
            buf = _int_strptime(buf, "%m/%d/%y", ptm,got_GMT);
            if (buf == 0)
                return 0;
            break;

        case 'E':
            if (Ealternative || Oalternative)
                break;
            Ealternative++;
            goto label;

        case 'O':
            if (Ealternative || Oalternative)
                break;
            Oalternative++;
            goto label;

        case 'F':
            buf = _int_strptime(buf, "%Y-%m-%d", ptm,got_GMT);
            if (buf == 0)
                return 0;
            break;

        case 'R':
            buf = _int_strptime(buf, "%H:%M", ptm,got_GMT);
            if (buf == 0)
                return 0;
            break;

        case 'r':
            buf = _int_strptime(buf, tptr->ampm_fmt, ptm,got_GMT);
            if (buf == 0)
                return 0;
            break;

        case 'T':
            buf = _int_strptime(buf, "%H:%M:%S", ptm,got_GMT);
            if (buf == 0)
                return 0;
            break;

        case 'X':
            buf = _int_strptime(buf, tptr->X_fmt, ptm,got_GMT);
            if (buf == 0)
                return 0;
            break;

        case 'x':
            buf = _int_strptime(buf, tptr->x_fmt, ptm,got_GMT);
            if (buf == 0)
                return 0;
            break;

        case 'j':
            if (!isdigit((unsigned char)*buf))
                return 0;

            len = 3;
            for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i < 1 || i > 366)
                return 0;

            ptm->tm_yday = i - 1;
            break;

        case 'M':
        case 'S':
            if (*buf == 0 || isspace((unsigned char)*buf))
                break;

            if (!isdigit((unsigned char)*buf))
                return 0;

            len = 2;
            for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }

            if (c == 'M') {
                if (i > 59)
                    return 0;
                ptm->tm_min = i;
            } else {
                if (i > 60)
                    return 0;
                ptm->tm_sec = i;
            }

            if (*buf != 0 && isspace((unsigned char)*buf))
                while (*ptr != 0 && !isspace((unsigned char)*ptr))
                    ptr++;
            break;

        case 'H':
        case 'I':
        case 'k':
        case 'l':
            /*
             * Of these, %l is the only specifier explicitly
             * documented as not being zero-padded.  However,
             * there is no harm in allowing zero-padding.
             *
             * XXX The %l specifier may gobble one too many
             * digits if used incorrectly.
             */
            if (!isdigit((unsigned char)*buf))
                return 0;

            len = 2;
            for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (c == 'H' || c == 'k') {
                if (i > 23)
                    return 0;
            } else if (i > 12)
                return 0;

            ptm->tm_hour = i;

            if (*buf != 0 && isspace((unsigned char)*buf))
                while (*ptr != 0 && !isspace((unsigned char)*ptr))
                    ptr++;
            break;

        case 'p':
            /*
             * XXX This is bogus if parsed before hour-related
             * specifiers.
             */
            len = (int)strlen(tptr->am);
            if (strncasecmp(buf, tptr->am, len) == 0) {
                if (ptm->tm_hour > 12)
                    return 0;
                if (ptm->tm_hour == 12)
                    ptm->tm_hour = 0;
                buf += len;
                break;
            }

            len = (int)strlen(tptr->pm);
            if (strncasecmp(buf, tptr->pm, len) == 0) {
                if (ptm->tm_hour > 12)
                    return 0;
                if (ptm->tm_hour != 12)
                    ptm->tm_hour += 12;
                buf += len;
                break;
            }

            return 0;

        case 'A':
        case 'a':
            for (i = 0; i < (int)asizeof(tptr->weekday); i++) {
                len = (int)strlen(tptr->weekday[i]);
                if (strncasecmp(buf, tptr->weekday[i],
                        len) == 0)
                    break;
                len = (int)strlen(tptr->wday[i]);
                if (strncasecmp(buf, tptr->wday[i],
                        len) == 0)
                    break;
            }
            if (i == (int)asizeof(tptr->weekday))
                return 0;

            ptm->tm_wday = i;
            buf += len;
            break;

        case 'U':
        case 'W':
            /*
             * XXX This is bogus, as we can not assume any valid
             * information present in the tm structure at this
             * point to calculate a real value, so just check the
             * range for now.
             */
            if (!isdigit((unsigned char)*buf))
                return 0;

            len = 2;
            for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i > 53)
                return 0;

            if (*buf != 0 && isspace((unsigned char)*buf))
                while (*ptr != 0 && !isspace((unsigned char)*ptr))
                    ptr++;
            break;

        case 'w':
            if (!isdigit((unsigned char)*buf))
                return 0;

            i = *buf - '0';
            if (i > 6)
                return 0;

            ptm->tm_wday = i;

            if (*buf != 0 && isspace((unsigned char)*buf))
                while (*ptr != 0 && !isspace((unsigned char)*ptr))
                    ptr++;
            break;

        case 'd':
        case 'e':
            /*
             * The %e specifier is explicitly documented as not
             * being zero-padded but there is no harm in allowing
             * such padding.
             *
             * XXX The %e specifier may gobble one too many
             * digits if used incorrectly.
             */
            if (!isdigit((unsigned char)*buf))
                return 0;

            len = 2;
            for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i > 31)
                return 0;

            ptm->tm_mday = i;

            if (*buf != 0 && isspace((unsigned char)*buf))
                while (*ptr != 0 && !isspace((unsigned char)*ptr))
                    ptr++;
            break;

        case 'B':
        case 'b':
        case 'h':
            for (i = 0; i < (int)asizeof(tptr->month); i++) {
                if (Oalternative) {
                    if (c == 'B') {
                        len = (int)strlen(tptr->alt_month[i]);
                        if (strncasecmp(buf,
                                tptr->alt_month[i],
                                len) == 0)
                            break;
                    }
                } else {
                    len = (int)strlen(tptr->month[i]);
                    if (strncasecmp(buf, tptr->month[i],
                            len) == 0)
                        break;
                    len = (int)strlen(tptr->mon[i]);
                    if (strncasecmp(buf, tptr->mon[i],
                            len) == 0)
                        break;
                }
            }
            if (i == (int)asizeof(tptr->month))
                return 0;

            ptm->tm_mon = i;
            buf += len;
            break;

        case 'm':
            if (!isdigit((unsigned char)*buf))
                return 0;

            len = 2;
            for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i < 1 || i > 12)
                return 0;

            ptm->tm_mon = i - 1;

            if (*buf != 0 && isspace((unsigned char)*buf))
                while (*ptr != 0 && !isspace((unsigned char)*ptr))
                    ptr++;
            break;

        case 's':
            {
            char *cp;
            time_t t;
            t = strtol(buf, &cp, 10);
            if (t == LONG_MAX)
                return 0;
            buf = cp;
#ifdef WIN32FUNC
            {
                struct tm *tm_t = gmtime(&t);
                memmove(ptm,tm_t,sizeof(struct tm));
            }
#else
            gmtime_r(&t, ptm);
#endif
            *got_GMT = 1;
            }
            break;

        case 'Y':
        case 'y':
            if (*buf == 0 || isspace((unsigned char)*buf))
                break;

            if (!isdigit((unsigned char)*buf))
                return 0;

            len = (c == 'Y') ? 4 : 2;
            for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (c == 'Y')
                i -= 1900;
            if (c == 'y' && i < 69)
                i += 100;
            if (i < 0)
                return 0;

            ptm->tm_year = i;

            if (*buf != 0 && isspace((unsigned char)*buf))
                while (*ptr != 0 && !isspace((unsigned char)*ptr))
                    ptr++;
            break;

        case 'Z':
            {
            const char *cp;

            for (cp = buf; *cp && isupper((unsigned char)*cp); ++cp) {/*empty*/}
                if ( (cp > buf) && (cp - buf <= 7) ) { /* prevent too large alloca */
                    char *zonestr = (char*)alloca(cp - buf + 1);
                    strncpy(zonestr, buf, cp - buf);
                    zonestr[cp - buf] = '\0';
                    if (0 == strcmp(zonestr, "GMT")) {
                        *got_GMT = 1;
                    } else if (0 == strcmp(zonestr, _tzconv(tzname[0]))) {
                        ptm->tm_isdst = 0;
                    } else if (0 == strcmp(zonestr, _tzconv(tzname[1]))) {
                        ptm->tm_isdst = 1;
                    } else {
                        return 0;
                    }
                    buf += cp - buf;
                }
            }
            break;
        case 'z':
            if(*buf==0 || (buf[0]!='+' && buf[0] != '-'))
            break;
            
            {
            signed char sign;
            
            sign = (char)((buf++[0] == '-') ? -1 : +1);
            
            len = 4;
            for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) 
            {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            
            ptm->tm_hour-= sign*(i/100);
            ptm->tm_min-= sign*(i%100);
            *got_GMT=1;
            
            }
            break;

        }
    }
    return (char *)buf;
}

/* stolen from Apache 1.33 src/main/util_time.c */
time_t ap_timegm(const struct tm * t)
{
    int year;
    time_t days;
    static const int dayoffset[12] =
    {306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

    year = t->tm_year;

    if (year < 70 || ((sizeof(time_t) <= 4) && (year >= 138)))
        return -1;

    /* shift new year to 1st March in order to make leap year calc easy */

    if (t->tm_mon < 2)
        year--;

    /* Find number of days since 1st March 1900 (in the Gregorian calendar). */

    days = year * 365 + year / 4 - year / 100 + (year / 100 + 3) / 4;
    days += dayoffset[t->tm_mon] + t->tm_mday - 1;
    days -= 25508;        /* 1 jan 1970 is 25508 days since 1 mar 1900 */

    days = ((days * 24 + t->tm_hour) * 60 + t->tm_min) * 60 + t->tm_sec;

    if (days < 0)
        return -1;    /* must have overflowed */
    else
        return days;  /* must be a valid time */
}

char *
ap_strptime(const char *buf, const char *fmt, struct tm *ptm)
{
    char *ret;
    int got_GMT;

    got_GMT = 0;
    ret = _int_strptime(buf, fmt, ptm,&got_GMT);
    if (ret && got_GMT) 
    {
#ifdef WIN32FUNC
        time_t t = ap_timegm(ptm);
        struct tm *tm_p;
        tm_p = localtime(&t);
        memmove(ptm,tm_p,sizeof(struct tm));
#else
        time_t t = timegm(ptm);
        localtime_r(&t, ptm);
#endif
        got_GMT = 0;
    }

    return ret;
}

#ifndef TZ_MAX_TIMES
/*
** The TZ_MAX_TIMES value below is enough to handle a bit more than a
** year's worth of solar time (corrected daily to the nearest second) or
** 138 years of Pacific Presidential Election time
** (where there are three time zone transitions every fourth year).
*/
#define TZ_MAX_TIMES   370
#endif /* !defined TZ_MAX_TIMES */

#ifndef TZ_MAX_TYPES
#ifndef NOSOLAR
#define TZ_MAX_TYPES   256 /* Limited by what (unsigned char)'s can hold */
#endif /* !defined NOSOLAR */
#ifdef NOSOLAR
/*
** Must be at least 14 for Europe/Riga as of Jan 12 1995,
** as noted by Earl Chew <earl@hpato.aus.hp.com>.
*/
#define TZ_MAX_TYPES    20    /* Maximum number of local time types */
#endif /* !defined NOSOLAR */
#endif /* !defined TZ_MAX_TYPES */

#ifndef TZ_MAX_CHARS
#define TZ_MAX_CHARS    50    /* Maximum number of abbreviation characters */
                            /* (limited by what unsigned chars can hold) */
#endif /* !defined TZ_MAX_CHARS */

#ifndef TZ_MAX_LEAPS
#define TZ_MAX_LEAPS    50    /* Maximum number of leap second corrections */
#endif /* !defined TZ_MAX_LEAPS */

#define SECSPERMIN      60
#define MINSPERHOUR     60
#define HOURSPERDAY     24
#define DAYSPERWEEK      7
#define DAYSPERNYEAR   365
#define DAYSPERLYEAR   366
#define SECSPERHOUR    (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY     ((long) SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR     12

#define TM_SUNDAY       0
#define TM_MONDAY       1
#define TM_TUESDAY      2
#define TM_WEDNESDAY    3
#define TM_THURSDAY     4
#define TM_FRIDAY       5
#define TM_SATURDAY     6

#define TM_JANUARY      0
#define TM_FEBRUARY     1
#define TM_MARCH        2
#define TM_APRIL        3
#define TM_MAY          4
#define TM_JUNE         5
#define TM_JULY         6
#define TM_AUGUST       7
#define TM_SEPTEMBER    8
#define TM_OCTOBER      9
#define TM_NOVEMBER    10
#define TM_DECEMBER    11

#define TM_YEAR_BASE  1900

#define EPOCH_YEAR    1970
#define EPOCH_WDAY    TM_THURSDAY

/*
** Accurate only for the past couple of centuries;
** that will probably do.
*/

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

#ifndef USG

/*
** Use of the underscored variants may cause problems if you move your code to
** certain System-V-based systems; for maximum portability, use the
** underscore-free variants.  The underscored variants are provided for
** backward compatibility only; they may disappear from future versions of
** this file.
*/

#define SECS_PER_MIN     SECSPERMIN
#define MINS_PER_HOUR    MINSPERHOUR
#define HOURS_PER_DAY    HOURSPERDAY
#define DAYS_PER_WEEK    DAYSPERWEEK
#define DAYS_PER_NYEAR   DAYSPERNYEAR
#define DAYS_PER_LYEAR   DAYSPERLYEAR
#define SECS_PER_HOUR    SECSPERHOUR
#define SECS_PER_DAY     SECSPERDAY
#define MONS_PER_YEAR    MONSPERYEAR

#endif /* !defined USG */
#ifndef INT_STRLEN_MAXIMUM
/*
** 302 / 1000 is log10(2.0) rounded up.
** Subtract one for the sign bit if the type is signed;
** add one for integer division truncation;
** add one more for a minus sign if the type is signed.
*/
#define INT_STRLEN_MAXIMUM(type) \
    ((TYPE_BIT(type) - TYPE_SIGNED(type)) * 302 / 1000 + 1 + TYPE_SIGNED(type))
#endif /* !defined INT_STRLEN_MAXIMUM */

#ifndef TYPE_BIT
#define TYPE_BIT(type)    (sizeof (type) * CHAR_BIT)
#endif /* !defined TYPE_BIT */

#ifndef TYPE_SIGNED
#define TYPE_SIGNED(type) (((type) -1) < 0)
#endif /* !defined TYPE_SIGNED */




static char *    _add (const char *, char *, const char *);
static char *    _conv (int, const char *, char *, const char *);
static char *    _fmt (const char *, const struct tm *, char *, const char *);

size_t ap_strftime (char *, size_t, const char *, const struct tm *);

#ifndef WIN32FUNC 
extern char *    tzname[]; /* Already defined in <time.h> */
#endif 

size_t
ap_strftime(char *s, size_t maxsize, const char *format, const struct tm *t)
{
    char *p;

    tzset();
    p = _fmt(((format == NULL) ? "%c" : format), t, s, s + maxsize);
    if (p == s + maxsize)
        return 0;
    *p = '\0';
    return p - s;
}

static char *
_fmt(const char *format, const struct tm *t, char *pt, const char *ptlim)
{
    int Ealternative, Oalternative;
    struct lc_time_T *tptr = &_C_time_locale;

    for ( ; *format; ++format) {
        if (*format == '%') {
            Ealternative = 0;
            Oalternative = 0;
label:
            switch (*++format) {
            case '\0':
                --format;
                break;
            case 'A':
                pt = _add((t->tm_wday < 0 || t->tm_wday > 6) ?
                    "?" : tptr->weekday[t->tm_wday],
                    pt, ptlim);
                continue;
            case 'a':
                pt = _add((t->tm_wday < 0 || t->tm_wday > 6) ?
                    "?" : tptr->wday[t->tm_wday],
                    pt, ptlim);
                continue;
            case 'B':
                pt = _add((t->tm_mon < 0 || t->tm_mon > 11) ? 
                    "?" : (Oalternative ? tptr->alt_month :
                    tptr->month)[t->tm_mon],
                    pt, ptlim);
                continue;
            case 'b':
            case 'h':
                pt = _add((t->tm_mon < 0 || t->tm_mon > 11) ?
                    "?" : tptr->mon[t->tm_mon],
                    pt, ptlim);
                continue;
            case 'C':
                /*
                ** %C used to do a...
                **    _fmt("%a %b %e %X %Y", t);
                ** ...whereas now POSIX 1003.2 calls for
                ** something completely different.
                ** (ado, 5/24/93)
                */
                pt = _conv((t->tm_year + TM_YEAR_BASE) / 100,
                    "%02d", pt, ptlim);
                continue;
            case 'c':
                pt = _fmt(tptr->c_fmt, t, pt, ptlim);
                continue;
            case 'D':
                pt = _fmt("%m/%d/%y", t, pt, ptlim);
                continue;
            case 'd':
                pt = _conv(t->tm_mday, "%02d", pt, ptlim);
                continue;
            case 'E':
                if (Ealternative || Oalternative)
                    break;
                Ealternative++;
                goto label;
            case 'O':
                /*
                ** POSIX locale extensions, a la
                ** Arnold Robbins' strftime version 3.0.
                ** The sequences
                **      %Ec %EC %Ex %EX %Ey %EY
                **    %Od %oe %OH %OI %Om %OM
                **    %OS %Ou %OU %OV %Ow %OW %Oy
                ** are supposed to provide alternate
                ** representations.
                ** (ado, 5/24/93)
                **
                ** FreeBSD extensions
                **      %OB %Ef %EF
                */
                if (Ealternative || Oalternative)
                    break;
                Oalternative++;
                goto label;
            case 'e':
                pt = _conv(t->tm_mday, "%2d", pt, ptlim);
                continue;
            case 'F':
                pt = _fmt("%Y-%m-%d", t, pt, ptlim);
                continue;
            case 'H':
                pt = _conv(t->tm_hour, "%02d", pt, ptlim);
                continue;
            case 'I':
                pt = _conv((t->tm_hour % 12) ?
                    (t->tm_hour % 12) : 12,
                    "%02d", pt, ptlim);
                continue;
            case 'j':
                pt = _conv(t->tm_yday + 1, "%03d", pt, ptlim);
                continue;
            case 'k':
                /*
                ** This used to be...
                **    _conv(t->tm_hour % 12 ?
                **        t->tm_hour % 12 : 12, 2, ' ');
                ** ...and has been changed to the below to
                ** match SunOS 4.1.1 and Arnold Robbins'
                ** strftime version 3.0.  That is, "%k" and
                ** "%l" have been swapped.
                ** (ado, 5/24/93)
                */
                pt = _conv(t->tm_hour, "%2d", pt, ptlim);
                continue;
#ifdef KITCHEN_SINK
            case 'K':
                /*
                ** After all this time, still unclaimed!
                */
                pt = _add("kitchen sink", pt, ptlim);
                continue;
#endif /* defined KITCHEN_SINK */
            case 'l':
                /*
                ** This used to be...
                **    _conv(t->tm_hour, 2, ' ');
                ** ...and has been changed to the below to
                ** match SunOS 4.1.1 and Arnold Robbin's
                ** strftime version 3.0.  That is, "%k" and
                ** "%l" have been swapped.
                ** (ado, 5/24/93)
                */
                pt = _conv((t->tm_hour % 12) ?
                    (t->tm_hour % 12) : 12,
                    "%2d", pt, ptlim);
                continue;
            case 'M':
                pt = _conv(t->tm_min, "%02d", pt, ptlim);
                continue;
            case 'm':
                pt = _conv(t->tm_mon + 1, "%02d", pt, ptlim);
                continue;
            case 'n':
                pt = _add("\n", pt, ptlim);
                continue;
            case 'p':
                pt = _add((t->tm_hour >= 12) ?
                    tptr->pm :
                    tptr->am,
                    pt, ptlim);
                continue;
            case 'R':
                pt = _fmt("%H:%M", t, pt, ptlim);
                continue;
            case 'r':
                pt = _fmt(tptr->ampm_fmt, t, pt, ptlim);
                continue;
            case 'S':
                pt = _conv(t->tm_sec, "%02d", pt, ptlim);
                continue;
            case 's':
                {
                    struct tm   ttm;
                    char        buf[INT_STRLEN_MAXIMUM(
                                time_t) + 1];
                    time_t      mkt;

                    ttm = *t;
                    mkt = mktime(&ttm);
                    if (TYPE_SIGNED(time_t))
                        (void) sprintf(buf, "%ld",
                            (long) mkt);
                    else    (void) sprintf(buf, "%lu",
                            (unsigned long) mkt);
                    pt = _add(buf, pt, ptlim);
                }
                continue;
            case 'T':
                pt = _fmt("%H:%M:%S", t, pt, ptlim);
                continue;
            case 't':
                pt = _add("\t", pt, ptlim);
                continue;
            case 'U':
                pt = _conv((t->tm_yday + 7 - t->tm_wday) / 7,
                    "%02d", pt, ptlim);
                continue;
            case 'u':
                /*
                ** From Arnold Robbins' strftime version 3.0:
                ** "ISO 8601: Weekday as a decimal number
                ** [1 (Monday) - 7]"
                ** (ado, 5/24/93)
                */
                pt = _conv((t->tm_wday == 0) ? 7 : t->tm_wday,
                    "%d", pt, ptlim);
                continue;
            case 'V':    /* ISO 8601 week number */
            case 'G':    /* ISO 8601 year (four digits) */
            case 'g':    /* ISO 8601 year (two digits) */
/*
** From Arnold Robbins' strftime version 3.0:  "the week number of the
** year (the first Monday as the first day of week 1) as a decimal number
** (01-53)."
** (ado, 1993-05-24)
**
** From "http://www.ft.uni-erlangen.de/~mskuhn/iso-time.html" by Markus Kuhn:
** "Week 01 of a year is per definition the first week which has the
** Thursday in this year, which is equivalent to the week which contains
** the fourth day of January. In other words, the first week of a new year
** is the week which has the majority of its days in the new year. Week 01
** might also contain days from the previous year and the week before week
** 01 of a year is the last week (52 or 53) of the previous year even if
** it contains days from the new year. A week starts with Monday (day 1)
** and ends with Sunday (day 7).  For example, the first week of the year
** 1997 lasts from 1996-12-30 to 1997-01-05..."
** (ado, 1996-01-02)
*/
                {
                    int    year;
                    int    yday;
                    int    wday;
                    int    w;

                    year = t->tm_year + TM_YEAR_BASE;
                    yday = t->tm_yday;
                    wday = t->tm_wday;
                    for ( ; ; ) {
                        int    len;
                        int    bot;
                        int    top;

                        len = isleap(year) ?
                            DAYSPERLYEAR :
                            DAYSPERNYEAR;
                        /*
                        ** What yday (-3 ... 3) does
                        ** the ISO year begin on?
                        */
                        bot = ((yday + 11 - wday) %
                            DAYSPERWEEK) - 3;
                        /*
                        ** What yday does the NEXT
                        ** ISO year begin on?
                        */
                        top = bot -
                            (len % DAYSPERWEEK);
                        if (top < -3)
                            top += DAYSPERWEEK;
                        top += len;
                        if (yday >= top) {
                            ++year;
                            w = 1;
                            break;
                        }
                        if (yday >= bot) {
                            w = 1 + ((yday - bot) /
                                DAYSPERWEEK);
                            break;
                        }
                        --year;
                        yday += isleap(year) ?
                            DAYSPERLYEAR :
                            DAYSPERNYEAR;
                    }
#ifdef XPG4_1994_04_09
                    if ((w == 52
                         && t->tm_mon == TM_JANUARY)
                        || (w == 1
                        && t->tm_mon == TM_DECEMBER))
                        w = 53;
#endif /* defined XPG4_1994_04_09 */
                    if (*format == 'V')
                        pt = _conv(w, "%02d",
                            pt, ptlim);
                    else if (*format == 'g') {
                        pt = _conv(year % 100, "%02d",
                            pt, ptlim);
                    } else    pt = _conv(year, "%04d",
                            pt, ptlim);
                }
                continue;
            case 'v':
                /*
                ** From Arnold Robbins' strftime version 3.0:
                ** "date as dd-bbb-YYYY"
                ** (ado, 5/24/93)
                */
                pt = _fmt("%e-%b-%Y", t, pt, ptlim);
                continue;
            case 'W':
                pt = _conv((t->tm_yday + 7 -
                    (t->tm_wday ?
                    (t->tm_wday - 1) : 6)) / 7,
                    "%02d", pt, ptlim);
                continue;
            case 'w':
                pt = _conv(t->tm_wday, "%d", pt, ptlim);
                continue;
            case 'X':
                pt = _fmt(tptr->X_fmt, t, pt, ptlim);
                continue;
            case 'x':
                pt = _fmt(tptr->x_fmt, t, pt, ptlim);
                continue;
            case 'y':
                pt = _conv((t->tm_year + TM_YEAR_BASE) % 100,
                    "%02d", pt, ptlim);
                continue;
            case 'Y':
                pt = _conv(t->tm_year + TM_YEAR_BASE, "%04d",
                    pt, ptlim);
                continue;
            case 'Z': /* tzset() already has done! */
#ifndef WIN32FUNC
                if (t->tm_zone != NULL)
                    pt = _add(t->tm_zone, pt, ptlim);
                else
#endif
                if (t->tm_isdst >= 0) {
                    pt = _add(_tzconv(tzname[(t->tm_isdst==0)?0:1]), pt, ptlim);
                } else  
                    pt = _add("?", pt, ptlim);
                continue;
            case 'z': /* tzset() already has done! */
                { 
#ifdef WIN32FUNC
                    long absoff = -timezone; /* Win32: Difference in seconds between UTC and local time */
#else
                    long absoff = t->tm_gmtoff; /* Unix: offset from UTC in seconds */
#endif
                    if ( absoff >= 0) {
                        pt = _add("+", pt, ptlim);
                    } else {
                        absoff = -absoff;
                        pt = _add("-", pt, ptlim);
                    }
                    pt = _conv(absoff / 3600, "%02d",
                        pt, ptlim);
                    pt = _conv((absoff % 3600) / 60, "%02d",
                        pt, ptlim);
                }
                continue;
            case '+':
                pt = _fmt(tptr->date_fmt, t, pt, ptlim);
                continue;
            case '%':
            /*
             * X311J/88-090 (4.12.3.5): if conversion char is
             * undefined, behavior is undefined.  Print out the
             * character itself as printf(3) also does.
             */
            default:
                break;
            }
        }
        if (pt == ptlim)
            break;
        *pt++ = *format;
    }
    return pt;
}

static char *
_conv(int n, const char *format, char *pt, const char *ptlim)
{
    char    buf[INT_STRLEN_MAXIMUM(int) + 1];

    (void) sprintf(buf, format, n);
    return _add(buf, pt, ptlim);
}

static char *
_add(const char *str, char *pt, const char *ptlim)
{
    while (pt < ptlim && (*pt = *str++) != '\0')
        ++pt;
    return pt;
}


/* Parse string to time_t, analogue of ap_strptime */
char*  ap_strptime_t(const char *buf, const char *fmt, time_t* t)
{
    struct tm ttm;
    char* ret = ap_strptime(buf,fmt,&ttm);
    *t = mktime(&ttm);
    return ret;
}

/* Format time_t value, analogue of ap_strftime */
size_t ap_strftime_t(char *buf, size_t maxsize, const char *fmt, time_t t)
{
    struct tm ttm; 

#ifdef WIN32FUNC
    struct tm *tm_p;
    tm_p = localtime(&t);
    if (tm_p)
       memmove(&ttm,tm_p,sizeof(ttm));
    else
       return 0;
#else
    localtime_r(&t, &ttm);
#endif

    return ap_strftime(buf,maxsize,fmt,&ttm);
}

#ifdef WIN32FUNC
static const char* conv_win32zone(const char* szSrc)
{
  struct xlattz_t {
    const char* src; 
    const char* dst;
  };
  static const struct xlattz_t xlattz [] = {
      { "Russian Standard Time", "MSK" }, /* GMT+03:00 */
      { "Russian Daylight Time", "MSD" }, /* GMT+04:00 */
      { "Greenwich Standard Time", "GMT" }, /* GMT+00:00 */
      { "Greenwich Daylight Time", "WEDT" }, /* GMT+01:00 */
      { "Pacific Standard Time", "PST" }, /* GMT-08:00 */
      { "Pacific Daylight Time", "PDT" }, /* GMT-07:00 */
      { "Mountain Standard Time", "MST" }, /* GMT-07:00 */
      { "Mountain Daylight Time", "MDT" }, /* GMT-06:00 */
      { "Central America Standard Time", "CST" }, /* GMT-06:00 */
      { "Central America Daylight Time", "CDT" }, /* GMT-05:00 */
      { "Central Standard Time", "CST" }, /* GMT-06:00 */
      { "Central Daylight Time", "CDT" }, /* GMT-05:00 */
      { "Eastern Standard Time", "EST" }, /* GMT-05:00 */
      { "Eastern Daylight Time", "EDT" }, /* GMT-04:00 */
      { "Central Europe Standard Time", "CET"  }, /* GMT+01:00 */
      { "Central Europe Daylight Time", "CEDT" }, /* GMT+02:00 */
      { "E. Europe Standard Time", "EET" }, /* GMT+02:00 */
      { "E. Europe Daylight Time", "EEDT"}, /* GMT+03:00 */
      { "Ekaterinburg Standard Time", "YEKT" }, /* GMT+05:00 */
      { "Ekaterinburg Daylight Time", "YEKST"}, /* GMT+06:00 */
      { "N. Central Asia Standard Time", "NOVT" }, /* GMT+06:00 */
      { "N. Central Asia Daylight Time", "NOVST"}, /* GMT+07:00 */
      { "North Asia East Standard Time", "IRKT" }, /* GMT+08:00 */
      { "North Asia East Daylight Time", "IRKST"}, /* GMT+09:00 */
      { "Yakutsk Standard Time", "YAKT" }, /* GMT+09:00 */
      { "Yakutsk Daylight Time", "YAKST"}, /* GMT+10:00 */
      { "Tokyo Standard Time", "JST"}, /* GMT+09:00 */
      { "Vladivostok Standard Time", "VLAT" }, /* GMT+10:00 */
      { "Vladivostok Daylight Time", "VLAST"}  /* GMT+11:00 */
  };
  static const size_t dim = asizeof(xlattz);
  size_t i;
  for ( i=0; i<dim; i++ ) {
      if ( 0 == strcmp(xlattz[i].src,szSrc) )
          return xlattz[i].dst;
  }
  return szSrc;
}
#endif
