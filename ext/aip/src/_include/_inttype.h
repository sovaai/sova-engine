# if !defined( __compat_inttype_h__ )
# define __compat_inttype_h__

# if !defined( _MSC_VER )
#   include <inttypes.h>
# else
#   if !defined( _SYS_INTTYPES_H_ )
#     define  _SYS_INTTYPES_H_

#     if !defined(_SSIZE_T_DEFINED) && !defined(_SSIZE_T_DECLARED)
        typedef long int ssize_t;
#       define _SSIZE_T_DEFINED
#       define _SSIZE_T_DECALARED
#     endif
      
#     if !defined( _INT8_T_DECLARED )
        typedef  signed char  int8_t;
#       define	_INT8_T_DECLARED
#     endif   // !_INT8_T_DECLARED
      
#     if !defined( _INT16_T_DECLARED )
        typedef	signed short  int16_t;
#       define	_INT16_T_DECLARED
#     endif   // !_INT16_T_DECLARED
      
#     if !defined( _INT32_T_DECLARED )
        typedef	int           int32_t;
#       define  _INT32_T_DECLARED
#     endif   // !_INT32_T_DECLARED
      
#     if !defined( _INT64_T_DECLARED )
        typedef	__int64		    int64_t;
#       define	_INT64_T_DECLARED
#     endif   // _INT64_T_DECLARED
      
#     if !defined( _UINT8_T_DECLARED )
        typedef	unsigned char     uint8_t;
        typedef	uint8_t           u_int8_t;
#       define	_UINT8_T_DECLARED
#     endif   // _UINT8_T_DECLARED
      
#     if !defined( _UINT16_T_DECLARED )
        typedef	unsigned short    uint16_t;
        typedef	uint16_t          u_int16_t;
#       define	_UINT16_T_DECLARED
#     endif   // _UINT16_T_DECLARED
      
#     if !defined( _UINT32_T_DECLARED )
        typedef	unsigned int      uint32_t;
        typedef	uint32_t          u_int32_t;
#       define	_UINT32_T_DECLARED
#     endif   // _UINT32_T_DECLARED
      
#     if !defined( _UINT64_T_DECLARED )
        typedef unsigned __int64  uint64_t;
        typedef	uint64_t          u_int64_t;
#       define	_UINT64_T_DECLARED
#     endif   // _UINT64_T_DECLARED
/*
typedef	__int_least8_t		int_least8_t;
typedef	__int_least16_t		int_least16_t;
typedef	__int_least32_t		int_least32_t;
typedef	__int_least64_t		int_least64_t;

typedef	__uint_least8_t		uint_least8_t;
typedef	__uint_least16_t	uint_least16_t;
typedef	__uint_least32_t	uint_least32_t;
typedef	__uint_least64_t	uint_least64_t;

typedef	__int_fast8_t		int_fast8_t;
typedef	__int_fast16_t		int_fast16_t;
typedef	__int_fast32_t		int_fast32_t;
typedef	__int_fast64_t		int_fast64_t;

typedef	__uint_fast8_t		uint_fast8_t;
typedef	__uint_fast16_t		uint_fast16_t;
typedef	__uint_fast32_t		uint_fast32_t;
typedef	__uint_fast64_t		uint_fast64_t;

typedef	__intmax_t		intmax_t;
typedef	__uintmax_t		uintmax_t;
*/

#     if !defined( _INTPTR_T_DEFINED )
        typedef	int*        intptr_t;
#       define	_INTPTR_T_DEFINED
#     endif   // _INTPTR_T_DEFINED

#     if !defined( _UINTPTR_T_DEFINED )
        typedef	unsigned*   uintptr_t;
#       define	_UINTPTR_T_DEFINED
#     endif


#   endif  // !_SYS_INTTYPES_H_
# endif

/* compatibility section */

# if !defined( put_word_32 )
#   define  put_word_32( buffer, uvalue )           \
    ((char*)(buffer))[0] = (char)(uvalue);          \
    ((char*)(buffer))[1] = (char)((uvalue) >> 8);   \
    ((char*)(buffer))[2] = (char)((uvalue) >> 16);  \
    ((char*)(buffer))[3] = (char)((uvalue) >> 24);
# endif  // put_word_32

# if !defined( get_word_32 )
#   define  get_word_32( buffer )                   \
      ((buffer)[0] | (((buffer)[1]) << 8)           \
    | (((buffer)[2]) << 16) | (((buffer)[3]) << 24))
# endif  // get_word_32

#ifndef _MSC_VER
#define CONSTANTUI64(const_val) (const_val##ULL)
#define CONSTANTI64(const_val) (const_val##LL)
#else
#define CONSTANTUI64(const_val) (const_val##ui64)
#define CONSTANTI64(const_val) (const_val##i64)
#endif

# endif  // __compat_inttype_h__
