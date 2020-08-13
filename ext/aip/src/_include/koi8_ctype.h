#ifndef _KOI8_CTYPE_H_
#define _KOI8_CTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define	KOI8_U	0x01
#define	KOI8_L	0x02
#define	KOI8_N	0x04
#define	KOI8_S	0x08
#define	KOI8_P	0x10
#define	KOI8_C	0x20
#define	KOI8_X	0x40
#define	KOI8_B	0x80

extern const unsigned char _koi8_ctype_[];
extern const unsigned char _koi8_lchars_[];
extern const unsigned char _koi8_uchars_[];
extern const unsigned char _koi8_ToUP_[];
extern const unsigned char _koi8_ToLO_[];

#define	koi8_isalnum(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & (KOI8_U|KOI8_L|KOI8_N))
#define	koi8_isalpha(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & (KOI8_U|KOI8_L))
#define	koi8_isascii(c)	((unsigned)(c) <= 0177)
#define	koi8_isblank(c)	((c) == '\t' || (c) == ' ')
#define	koi8_iscntrl(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & KOI8_C)
#define	koi8_isdigit(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & KOI8_N)
#define	koi8_isgraph(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & (KOI8_P|KOI8_U|KOI8_L|KOI8_N))
#define	koi8_islower(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & KOI8_L)
#define	koi8_isprint(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & (KOI8_P|KOI8_U|KOI8_L|KOI8_N|KOI8_B))
#define	koi8_ispunct(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & KOI8_P)
#define	koi8_isspace(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & KOI8_S)
#define	koi8_isupper(c)	((_koi8_ctype_ + 1)[(unsigned char)c] & KOI8_U)
#define	koi8_isxdigit(c) ((_koi8_ctype_ + 1)[(unsigned char)c] & KOI8_X)
#define	koi8_toascii(c)	((c) & 0177)
#define koi8_toupper(c) ((int) (_koi8_ToUP_[ (int)(unsigned char)(c) ]))
#define koi8_tolower(c) ((int) (_koi8_ToLO_[ (int)(unsigned char)(c) ]))

/*string manipulation functions*/
#include <stddef.h>
char *koi8_strlwr( char *string );
char *koi8_strupr( char *string );
char *koi8_strcasestr(const char *big, const char *little);

int koi8_strcmp(  const char *string1, const char *string2 );
int koi8_strncmp( const char *string1, const char *string2, size_t count );
/* "ABab" */

int koi8_strcasecmp(  const char *string1, const char *string2 );
int koi8_strncasecmp( const char *string1, const char *string2, size_t count );
/* "aABb" */

int koi8_strtitlecasecmp(  const char *string1, const char *string2 );
int koi8_strntitlecasecmp( const char *string1, const char *string2, size_t count );
/* "AaBb" */

char *koi8_strlwrdiacritics(char *string);
/* �->�, �->� */

#ifndef STD_CTYPE

#undef  isalnum
#define	isalnum(c)  koi8_isalnum(c)
#undef  isalpha
#define	isalpha(c)  koi8_isalpha(c)
#undef  isascii
#define	isascii(c)  koi8_isascii(c)
#undef  isblank
#define	isblank(c)  koi8_isblank(c)
#undef  iscntrl
#define	iscntrl(c)  koi8_iscntrl(c)
#undef  isdigit
#define	isdigit(c)  koi8_isdigit(c)
#undef  isgraph
#define	isgraph(c)  koi8_isgraph(c)
#undef  islower
#define	islower(c)  koi8_islower(c)
#undef  isprint
#define	isprint(c)  koi8_isprint(c)
#undef  ispunct
#define	ispunct(c)  koi8_ispunct(c)
#undef  isspace
#define	isspace(c)  koi8_isspace(c)
#undef  isupper
#define	isupper(c)  koi8_isupper(c)
#undef  isxdigit
#define	isxdigit(c) koi8_isxdigit(c)
#undef  toascii
#define	toascii(c)  koi8_toascii(c)
#undef  toupper
#define toupper(c)  koi8_toupper(c)
#undef  tolower
#define tolower(c)  koi8_tolower(c)

#endif /* STD_CTYPE */

#ifdef __cplusplus
}
#endif

#endif /* !_KOI8_CTYPE_H_ */
