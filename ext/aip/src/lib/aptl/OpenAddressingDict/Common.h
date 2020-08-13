/**
 * @file   Common.h
 * @author swizard <me@swizard.info>
 * @date   Tue Sep 16 15:53:04 2008
 * 
 * @brief  Common template parameters for OpenAddressingDict data structure.
 * 
 * 
 */
#ifndef _OPENADDRESSINGDICT_COMMON_H_
#define _OPENADDRESSINGDICT_COMMON_H_

#include <sys/types.h>

#include <_include/_string.h>
#include <_include/_inttype.h>

struct OpenAddressingCommon
{
public:
        enum ResultStatus
        {
                RESULT_STATUS_SUCCESS,
                RESULT_STATUS_NOT_FOUND,
                RESULT_STATUS_ERROR_INVALID_DATA_PTR,
                RESULT_STATUS_ERROR_ALLOCATION,
                RESULT_STATUS_ERROR_USER_MEMORY_RESIZE,
                RESULT_STATUS_ERROR_NOT_INITIALIZED,
                RESULT_STATUS_ERROR_FILE,
                RESULT_STATUS_ERROR_MODE
        };
        
        enum MaskConstants
        {
                maskIsEmpty           = 0x00,
                maskNotEmpty          = 0x01,
                maskIsRemoved         = 0x02
        };
        
protected:
        typedef uint8_t  NodeFlags;
        typedef uint32_t NodeLink;
        
public:
        typedef NodeLink Iterator;

public:
        static size_t nextPrime( size_t wantSize )
        {
                if ( wantSize == 0 )
                        return 3;

                if ( wantSize % 2 == 0 )
                        wantSize++;

                for ( ; !isPrime( wantSize ); wantSize += 2 );

                return wantSize;
        }

        static bool isPrime( size_t n )
        {
                if ( n == 2 || n == 3 )
                        return true;

                if ( n == 1 || n % 2 == 0 )
                        return false;

                for ( size_t i = 3; i * i <= n; i += 2 )
                        if ( n % i == 0 )
                                return false;

                return true;
        }
};

/* A builtin hasher for generic key types */
template<typename KeyType>
struct OpenAddressingHasher
{
	uint32_t getHash(KeyType key) const
	{
		/* A Bob Jenkins hash function for generic key */
		uint8_t  *buffer = reinterpret_cast<uint8_t *>( &key );
		uint32_t  hash   = 0;
		
		for (size_t i = 0; i < sizeof(key); i++)
		{
			hash += buffer[i];
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		
		return hash;
	}
};

/* A custom hasher for ASCIIZ string types */
template<>
struct OpenAddressingHasher<const char *>
{
	uint32_t getHash(const char *key) const
	{
		/* A standard djb2 hash function for ASCIIZ strings */
		uint32_t hash = 5381;
		while ( int c = *key++ )
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		return hash;
	}
};

/* A default comparator */
template<typename KeyType>
struct KeyEqualComparator
{
	bool areKeysEqual(KeyType firstKey, KeyType secondKey) const
	{
		return firstKey == secondKey;
	}
};

template<>
struct KeyEqualComparator<const char *>
{
	bool areKeysEqual(const char *firstKey, const char *secondKey) const
	{
		return strcmp(firstKey, secondKey) == 0;
	}
};

/* Elements placement 'new' and 'delete' overloads */
struct  __OpenAddressingDictPtr { };

inline void *operator new   (size_t, __OpenAddressingDictPtr *ptr) { return ptr; }
inline void  operator delete(void *, __OpenAddressingDictPtr *)    { }

#ifdef PAD4
# undef PAD4
#endif
#define PAD4(value) (((value + 3) >> 2) << 2)

const size_t maxSizeTBits = (sizeof(size_t) << 3) - 1;

/* Probing strategy: linear */
struct LinearProber
{
        size_t nextProbe(size_t probeNumber) const { return probeNumber; }
};

/* Probing strategy: quadratic */
struct QuadraticProber
{
        size_t nextProbe(size_t probeNumber) const { return ((probeNumber + 1) * probeNumber) >> 1; }
};

#endif /* _OPENADDRESSINGDICT_COMMON_H_ */

