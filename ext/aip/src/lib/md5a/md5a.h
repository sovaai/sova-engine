#ifndef MD5A_H_SENTRY
#define MD5A_H_SENTRY

/*
 *  MD5.h
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct __MD5_hash_value {
	unsigned char v[16];
} MD5_hash_value;

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;

/* MD5 context. */
typedef struct {
	UINT4 state[4];                                   /* state (ABCD) */
	UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

extern void MD5Init( MD5_CTX * context );
extern void MD5Update( MD5_CTX * context, const unsigned char * input, unsigned int inputLen );
extern void MD5Final( unsigned char digest[16], MD5_CTX * context );


#ifdef __cplusplus
}
#endif


#endif
