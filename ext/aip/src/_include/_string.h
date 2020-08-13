#if !defined( __STRING_h__ )
#define  __STRING_h__

#include <string.h>

#ifdef _MSC_VER
    #define bzero(a, b) memset(a, 0, b)

    #ifndef strcasecmp
        #define strncasecmp _strnicmp
        #define strcasecmp  _stricmp
    #endif
    
    #ifdef __cplusplus
    extern "C" {
    #endif
    /*
        simple strcasestr implementation
        uses toupper from ctype.h and may not work correctly for anything but ASCII
    */
    char *strcasestr(const char *haystack, const char *needle);
    #ifdef __cplusplus
    };    
    #endif
    
#endif /* _MSC_VER */

#ifdef __cplusplus
extern "C" {
#endif
/*
    simple strncasestr implementation
    (searches no more than n characters of haystack, or until \0 reached, whichever happens first)
    uses toupper from ctype.h and may not work correctly for anything but ASCII
*/
char *strncasestr(const char *haystack, const char *needle, size_t n);

#if defined(__linux__)
/* strnstr implementation for linuces */
#ifdef __cplusplus
inline
#else
static
#endif
char *strnstr( const char *s, const char *find, size_t slen )
{
	char c, sc;
	size_t len;

	if ((c = *find++) != '\0') {
		len = strlen(find);
		do {
			do {
				if (slen-- < 1 || (sc = *s++) == '\0')
					return (NULL);
			} while (sc != c);
			if (len > slen)
				return (NULL);
		} while (strncmp(s, find, len) != 0);
		s--;
	}

	return ((char *)s);
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STRING_h__ */
