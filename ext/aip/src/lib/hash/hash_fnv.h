#ifndef _HASH_FNV_H_
#define _HASH_FNV_H_


#include "_include/_inttype.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


#define FNV0_64_INIT ((uint64_t)0)

#ifdef _MSC_VER
 #define FNV1_64_INIT ((uint64_t)0xcbf29ce484222325Ui64)
 #define FNV_64_PRIME ((uint64_t)0x100000001b3Ui64)
#else
 #define FNV1_64_INIT ((uint64_t)0xcbf29ce484222325ULL)
 #define FNV_64_PRIME ((uint64_t)0x100000001b3ULL)
#endif

#define FNV1_32_INIT ((uint32_t)0x811c9dc5)
#define FNV_32_PRIME ((uint32_t)0x01000193)

/*
uint64_t mask = ((uint64_t)1<<bits)-1;
return (hash>>bits) ^ (hash & mask);
*/
#define FNV_XOR_FOLD(hash, bits)    ( (hash)>>(bits) ^ ( (hash) & ( ((uint64_t)1<<(bits))-1 ) ) )

static __inline uint64_t fnv_hash_64_seeded(const void *buf, size_t len, uint64_t seed)
{
    unsigned char *bp = (unsigned char *)buf;	/* start of buffer */
    unsigned char *be = bp + len;		/* beyond end of buffer */
    uint64_t hval = seed;

    /*
     * FNV-1 hash each octet of the buffer
     */
    while (bp < be) {

#if defined(USE_FNV1A)
/* xor first, multiply second */

        /* xor the bottom with the current octet */
	hval ^= (uint64_t)*bp++;
#endif

	/* multiply by the 64 bit FNV magic prime mod 2^64 */
	hval *= FNV_64_PRIME;

#if !defined(USE_FNV1A)
/* multiply first, xor second */

	/* xor the bottom with the current octet */
	hval ^= (uint64_t)*bp++;
#endif
    }

    /* return our new hash value */
    return hval;
}

static __inline uint64_t fnv_hash_64(const void *buf, size_t len)
{
    return fnv_hash_64_seeded(buf, len, FNV1_64_INIT);
}

static __inline uint32_t fnv_hash_32(const void *buf, size_t len)
{
    unsigned char *bp = (unsigned char *)buf;	/* start of buffer */
    unsigned char *be = bp + len;		/* beyond end of buffer */
    uint32_t hval = FNV1_32_INIT;

    /*
     * FNV-1 hash each octet in the buffer
     */
    while (bp < be) {

#if defined(USE_FNV1A)
/* xor first, multiply second */

        /* xor the bottom with the current octet */
	hval ^= (uint32_t)*bp++;
#endif

	/* multiply by the 32 bit FNV magic prime mod 2^32 */

	hval *= FNV_32_PRIME;

#if !defined(USE_FNV1A)
/* multiply first, xor second */

	/* xor the bottom with the current octet */
	hval ^= (uint32_t)*bp++;
#endif
    }

    /* return our new hash value */
    return hval;
}

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
