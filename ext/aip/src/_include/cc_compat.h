/* $Id: cc_compat.h 46032 2013-12-08 12:16:13Z swizard $ */
/**
 * @file   cc_compat.h
 * @author swizard <me@swizard.info>
 * @date   Sat Feb  9 01:03:19 2008
 * 
 * @brief  Compilators specific stuff compatibility layer.
 * 
 * 
 */
#ifndef _CC_COMPAT_H_
#define _CC_COMPAT_H_

#include "_inttype.h"

#if defined(__GNUC__) || defined(__clang__)
/* Every GCC and CLANG compiler */

#define likely(expr)   __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#define restrict_addr(expr) __restrict__ expr

#define MSC_ATTRIBUTE(att) /* Nothing */
#define GCC_ATTRIBUTE(att) __attribute__(att)

#elif defined(_MSC_VER)
/* Every MSVC compiler */

#define likely(expr)   (expr)
#define unlikely(expr) (expr)
#define restrict_addr(expr) __restrict expr

#if _MSC_VER > 1200
/* For every MSVS compiler except VS6 with SP6 or lower */
# define MSC_ATTRIBUTE(att) __declspec(att)
# define GCC_ATTRIBUTE(att) /* Nothing */
#else
# pragma message("MS VS 6.0 compatibility workaround: __attrib__")
# define MSC_ATTRIBUTE(att) /* Nothing */
# define GCC_ATTRIBUTE(att) /* Nothing */
#endif /* _MSC_VER > 1200 */

#endif /* defined(__GNUC__) */

/* Old versions of MSVC (6.0 and previous) don't support C99 for loop scoping rules.  This fixes them. */
#if defined(_MSC_VER) && _MSC_VER <= 1200
# pragma message("MS VS 6.0 compatibility workaround: 'for' scope")
/* This trick will generate a warning; disable the warning */
# pragma warning (disable : 4127) // conditional expression is constant
# define for if (false) {} else for
#endif /* _MSC_VER <= 1200 */

#define ALIGNED_FIELD(type, name, align)			\
	type name;						\
	uint8_t __ ## name ## _pad__[ align - sizeof(type) ];	\

#endif /* _CC_COMPAT_H_ */


