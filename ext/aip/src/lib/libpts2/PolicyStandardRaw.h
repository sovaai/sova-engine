/**
 * @file   PolicyStandardRaw.h
 * @author swizard <me@swizard.info>
 * @date   Fri Oct 31 01:10:23 2008
 * 
 * @brief  Standard policy with raw (byte vector) data for PatriciaTree.
 * 
 * 
 */
#ifndef _POLICYSTANDARDRAW_H_
#define _POLICYSTANDARDRAW_H_

#include "PolicyStandard.h"

/* Standard (raw) data format (size in bits):
 * +-------+-------+------+------+-----+-----+------+----------+------+--------+
 * | RLink | LLink | LTag | RTag | UF2 | UF1 | Skip | DataSize | Data | Key    |
 * +-------+-------+------+------+-----+-----+------+----------+------+--------+
 * | 32    | 32    | 1    | 1    | 1   | 1   | 28   | 32       | 32   | ASCIIZ |
 * +-------+-------+------+------+-----+-----+------+----------+------+--------+
 *
 * RLink: right node offset
 * LLink: left node offset
 * UF1: userflag1
 * UF2: userflag2
 * LTag: left way tag
 * RTag: right way tag
 * Skip: skip bits count
 * DataSize: the data size in octets
 * Data: the data stored: uint32_t value
 * Key: asciiz string
 */

const size_t policyStandardRawAuxSize = (sizeof(uint32_t) * 4);

/* Local helpers */

inline size_t policyStandardRawGetDataSize(const uint8_t *node)
{
	return *( reinterpret_cast<const uint32_t *>(node + (sizeof(uint32_t) * 3)) );
}

inline void policyStandardRawSetDataSize(uint8_t *node, size_t size)
{
	*( reinterpret_cast<uint32_t *>(node + (sizeof(uint32_t) * 3)) ) = static_cast<uint32_t>(size);
}

inline uint8_t *policyStandardRawGetKey(uint8_t *node)
{
        return (node + policyStandardRawAuxSize + policyStandardRawGetDataSize(node));
}

/* Address conversions */

struct PolicyStandardRawDataToNodeAddress
{
	const uint8_t *operator()(const uint8_t *dataAddress) const
	{
		return dataAddress - policyStandardRawAuxSize;
	}
};

/* Fields accessors */

struct PolicyStandardRawGetData
{
	const uint8_t *operator()(uint8_t *node) const
	{
		return node + policyStandardRawAuxSize;
	}
};

struct PolicyStandardRawSetData
{
	void operator()(uint8_t *node, const uint8_t *data, size_t dataSize)
	{
		if (data != 0)
			memcpy(node + policyStandardRawAuxSize, data, dataSize);
		policyStandardRawSetDataSize(node, dataSize);
	}
};

struct PolicyStandardRawSetNullData
{
	void operator()(uint8_t *node)
	{
		policyStandardRawSetDataSize(node, static_cast<size_t>(0));
	}
};

struct PolicyStandardRawGetKey
{
	uint8_t *operator()(uint8_t *node) const { return policyStandardRawGetKey(node); }
};

struct PolicyStandardRawSetKey
{
	void operator()(uint8_t *node, const uint8_t *key, size_t keySize)
	{
		uint8_t *address = node + policyStandardRawAuxSize + policyStandardRawGetDataSize(node);
		memcpy( reinterpret_cast<char *>(address),
			reinterpret_cast<const char *>(key),
			keySize);
		address[keySize] = static_cast<uint8_t>('\0');
	}
};

struct PolicyStandardRawGetNodeAuxSize
{
	size_t operator()() const { return policyStandardRawAuxSize; }
};

struct PolicyStandardRawGetKeySize
{
	/* The key size: its octets plus terminating '\0' */
	size_t operator()(size_t keyOctetsCount) const { return keyOctetsCount + 1; }
	size_t operator()(uint8_t *node) const
	{
		return strlen( reinterpret_cast<const char *>(PolicyStandardRawGetKey()(node)) );
	}
};

struct PolicyStandardRawGetNullDataSize
{
	size_t operator()() const { return static_cast<size_t>(0); }
};

struct PolicyStandardRawGetMaxDataSize
{
	size_t operator()() const { return static_cast<size_t>( static_cast<uint32_t>(-1) ); }
};

struct PolicyStandardRawGetDataSize
{
	size_t operator()(const uint8_t *node) const { return policyStandardRawGetDataSize(node); }
	size_t operator()(size_t dataSize) const { return sizeof(uint32_t) + dataSize; }
};

struct PolicyStandardRawCommonPrefixLength
{
	size_t operator()(const uint8_t *key, uint8_t *node) const
	{
                uint8_t *nodeKey = policyStandardRawGetKey(node);
                
		int    i, j;
		size_t commonBits = 0;

		for ( i = 0;
		      key[i] != static_cast<uint8_t>('\0') &&
			      nodeKey[i] != static_cast<uint8_t>('\0');
		      i++ )
		{
			if (key[i] == nodeKey[i])
				commonBits += (sizeof(uint8_t) << 3);
			else 
				break;
		}

		if (key[i] == nodeKey[i])
			commonBits += 8;
		else 
		{
			for (j = 7; j >= 0; j--)
			{
				if ( (key[i] & (1 << j)) == (nodeKey[i] & (1 << j)) )
					commonBits++;
				else
					break;
			}
		}

		return commonBits;
	}
};

struct PolicyStandardRaw
{
	typedef uint32_t                                NodeAddress;

	typedef struct PolicyStandardRawDataToNodeAddress  DataToNodeAddress;
	typedef struct PolicyStandardOffsetToLink          OffsetToLink;
	typedef struct PolicyStandardLinkToOffset          LinkToOffset;
	
	typedef struct PolicyStandardGetRightLink          GetRightLink;
	typedef struct PolicyStandardSetRightLink          SetRightLink;
	typedef struct PolicyStandardGetLeftLink           GetLeftLink;
	typedef struct PolicyStandardSetLeftLink           SetLeftLink;
	typedef struct PolicyStandardCheckRightTag         CheckRightTag;
	typedef struct PolicyStandardSetRightTag           SetRightTag;
	typedef struct PolicyStandardClearRightTag         ClearRightTag;
	typedef struct PolicyStandardCheckLeftTag          CheckLeftTag;
	typedef struct PolicyStandardSetLeftTag            SetLeftTag;
	typedef struct PolicyStandardClearLeftTag          ClearLeftTag;
	typedef struct PolicyStandardCheckUserFlag1        CheckUserFlag1;
	typedef struct PolicyStandardSetUserFlag1          SetUserFlag1;
	typedef struct PolicyStandardClearUserFlag1        ClearUserFlag1;
	typedef struct PolicyStandardGetSkip               GetSkip;
	typedef struct PolicyStandardSetSkip               SetSkip;
	typedef struct PolicyStandardRawGetData            GetData;
	typedef struct PolicyStandardRawSetData            SetData;
	typedef struct PolicyStandardRawSetNullData        SetNullData;
	typedef struct PolicyStandardRawGetKey             GetKey;
	typedef struct PolicyStandardRawSetKey             SetKey;
        typedef struct PolicyStandardInitKey               InitKey;
	typedef struct PolicyStandardClearNode             ClearNode;
	typedef struct PolicyStandardRawGetNodeAuxSize     GetNodeAuxSize;
	typedef struct PolicyStandardGetMinKeySize         GetMinKeySize;
	typedef struct PolicyStandardRawGetKeySize         GetKeySize;
	typedef struct PolicyStandardRawGetNullDataSize    GetNullDataSize;
	typedef struct PolicyStandardRawGetDataSize        GetDataSize;
	typedef struct PolicyStandardGetMaxTreeSize        GetMaxTreeSize;
	typedef struct PolicyStandardRawGetMaxDataSize     GetMaxDataSize;
	typedef struct PolicyStandardRawCommonPrefixLength CommonPrefixLength;
};

#endif /* _POLICYSTANDARDRAW_H_ */

