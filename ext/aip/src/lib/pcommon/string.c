#include <ctype.h>

#ifdef _MSC_VER
/* straightforward strcasestr implementation */
char *strcasestr(const char *haystack, const char *needle)
{
    const char *p, *startn = 0, *np = 0;
    
    for (p = haystack; *p; p++) {
        if (np) {
            if (toupper(*p) == toupper(*np)) {
                if (!*++np) {
                    return (char *)startn;
                }
             } else {
                 np = 0;
             }
        } else if (toupper(*p) == toupper(*needle)) {
            np = needle + 1;
            startn = p;
        }
    }
    return 0;
}
#endif /* _MSC_VER */

/* straightforward strncasestr implementation */
char *strncasestr(const char *haystack, const char *needle, unsigned long long n)
{
    const char *p, *startn = 0, *np = 0;
    
    for (p = haystack; *p && p < haystack + n; p++) {
        if (np) {
            if (toupper(*p) == toupper(*np)) {
                if (!*++np) {
                    return (char *)startn;
                }
             } else {
                 np = 0;
             }
        } else if (toupper(*p) == toupper(*needle)) {
            np = needle + 1;
            startn = p;
        }
    }
    return 0;
}

