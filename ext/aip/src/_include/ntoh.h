#ifndef __NTOH_H__
#define __NTOH_H__

#ifdef _WIN32

/* Check conflicts */
#ifdef _WINSOCK2API_
  #error ntoh.h conflicts with winsock2.h
#endif

#ifdef _WINSOCKAPI_
  #error ntoh.h conflicts with winsock.h
#endif

#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */

#endif /* _WIN32 */

/* sys/endian seems to be available only on *BSD */
#if (defined(__linux__) && defined(__GLIBC__) && ((__GLIBC__ < 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 9))) || defined(__APPLE__) || defined(_WIN32)

#include <_include/_inttype.h>

#if defined(__cplusplus)
# define __INLINE__ inline
#elif defined(__GNUC__)
# define __INLINE__ static __inline
#else
# define __INLINE__ static
# if defined(_WIN32)
#  pragma warning (disable : 4505) /* unreferenced local function has been removed */
# endif
#endif

#define bswap16(x)		 \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

#define bswap32(x)		      \
	((((x) & 0xff000000) >> 24) | \
	 (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | \
	 (((x) & 0x000000ff) << 24))

#define bswap64(x)							\
	( ( (x) >> 56)                           |			\
	  (((x) >> 40) & 0xff00)                 |			\
	  (((x) >> 24) & 0xff0000)               |			\
	  (((x) >> 8)  & 0xff000000)             |			\
	  (((x) << 8)  & ((uint64_t)0xff << 32)) |			\
	  (((x) << 24) & ((uint64_t)0xff << 40)) |			\
	  (((x) << 40) & ((uint64_t)0xff << 48)) |			\
	  (((x) << 56)) )

/* Here the functions are used instead of macros,
 * because of following construction failure:
 *
 * uint32_t value = be32toh(*pt++);
 *
 * If a macro is in use, the construction '*pt++'
 * repeats 4 times, resulting an error.
 */

__INLINE__ uint16_t htobe16(uint16_t x)	{ return ((uint16_t)bswap16((x))); }
__INLINE__ uint32_t htobe32(uint32_t x)	{ return ((uint32_t)bswap32((x))); }
__INLINE__ uint64_t htobe64(uint64_t x)	{ return ((uint64_t)bswap64((x))); }
__INLINE__ uint16_t htole16(uint16_t x)	{ return ((uint16_t)(x)); }
__INLINE__ uint32_t htole32(uint32_t x)	{ return ((uint32_t)(x)); }
__INLINE__ uint64_t htole64(uint64_t x)	{ return ((uint64_t)(x)); }

__INLINE__ uint16_t be16toh(uint16_t x)	{ return ((uint16_t)bswap16((x))); }
__INLINE__ uint32_t be32toh(uint32_t x)	{ return ((uint32_t)bswap32((x))); }
__INLINE__ uint64_t be64toh(uint64_t x)	{ return ((uint64_t)bswap64((x))); }
__INLINE__ uint16_t le16toh(uint16_t x)	{ return ((uint16_t)(x)); }
__INLINE__ uint32_t le32toh(uint32_t x)	{ return ((uint32_t)(x)); }
__INLINE__ uint64_t le64toh(uint64_t x)	{ return ((uint64_t)(x)); }

#endif /* defined(__linux__) || defined(__APPLE__) || defined(_WIN32) */

/* _WIN32 does not have htonl-like routines */
#ifdef _WIN32

#ifdef __cplusplus
inline uint32_t htonl (uint32_t hostlong)   { return htobe32(hostlong); }
inline uint32_t ntohl (uint32_t netlong)    { return be32toh(netlong); }
inline uint16_t htons (uint16_t hostshort)  { return htobe16(hostshort); }
inline uint16_t ntohs (uint16_t netshort)   { return be16toh(netshort); }
#else
# define ntohl(x) htobe32(x)
# define htonl(x) be32toh(x)
# define ntohs(x) htobe16(x)
# define htons(x) be16toh(x)
#endif

#else /* Not _WIN32 */

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* *BSD machines have their own endian routines */
#if !defined(__linux__) && !defined(__APPLE__) && !defined(_WIN32)

#include <sys/endian.h>

#endif /* !defined(__linux__) && !defined(__APPLE__) && !defined(_WIN32) */

#endif /* _WIN32 */

#endif /* __NTOH_H__ */
