/**
 * @file   PolicyCompact.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr 24 13:11:43 2008
 * 
 * @brief  Compact format policy for PatriciaTree.
 * 
 * 
 */
#ifndef _POLICYCOMPACT_H_
#define _POLICYCOMPACT_H_

#include <_include/_string.h>
#include <_include/_inttype.h>

/* Compact data format (size in bits):
 * +------+-------+------+------+-----+-----+----------+-------+------+--------+
 * | Skip | LLink | LTag | RTag | UF2 | UF1 | DataSize | RLink | Data |  Key   |
 * +------+-------+------+------+-----+-----+----------+-------+------+--------+
 * | 10   | 22    | 1    | 1    | 1   | 1   | 6        | 22    | ?    | ASCIIZ |
 * +------+-------+------+------+-----+-----+----------+-------+------+--------+
 *
 * RLink: right node offset (in 4-bytes chunks)
 * LLink: left node offset (in 4-bytes chunks)
 * UF1: userflag1
 * UF2: userflag2
 * LTag: left way tag
 * RTag: right way tag
 * Skip: skip bits count
 * DataSize: the data size in octets
 * Data: the data stored: octets string
 * Key: asciiz string
 */

const size_t policyCompactAuxSize = (sizeof(uint32_t) * 2);

enum PolicyCompactTagFlag
{
	POLICY_COMPACT_USERFLAG1 = 0x10000000U,
	POLICY_COMPACT_USERFLAG2 = 0x20000000U,
	POLICY_COMPACT_RIGHT_TAG = 0x40000000U,
	POLICY_COMPACT_LEFT_TAG  = 0x80000000U
};

const uint32_t policyCompactSkipMask     = 0xFFC00000U;
const uint32_t policyCompactFlagsMask    =
	POLICY_COMPACT_USERFLAG1 |
	POLICY_COMPACT_USERFLAG2 |
	POLICY_COMPACT_RIGHT_TAG |
	POLICY_COMPACT_LEFT_TAG;
const uint32_t policyCompactDataSizeMask = 0x0FC00000U;

/* Local helpers */

inline size_t policyCompactGetDataSize(const uint8_t *node)
{
	uint32_t value = *( reinterpret_cast<const uint32_t *>(node + sizeof(uint32_t)) );
	return static_cast<size_t>((value &= policyCompactDataSizeMask) >> 22);
}

inline void policyCompactSetDataSize(uint8_t *node, size_t size)
{
	uint32_t *address = reinterpret_cast<uint32_t *>(node + sizeof(uint32_t));
	*address &= policyCompactFlagsMask | ~(policyCompactFlagsMask | policyCompactDataSizeMask);
	*address |= (size << 22) & policyCompactDataSizeMask;
}

inline uint8_t *policyCompactGetKey(uint8_t *node)
{
        return (node + policyCompactAuxSize + policyCompactGetDataSize(node));
}

/* Address conversions */

struct PolicyCompactDataToNodeAddress
{
	const uint8_t *operator()(const uint8_t *dataAddress) const
	{
		return dataAddress - policyCompactAuxSize;
	}
};

struct PolicyCompactOffsetToLink
{
	uint32_t operator()(size_t offset) const { return static_cast<uint32_t>(offset >> 2); }
};

struct PolicyCompactLinkToOffset
{
	size_t operator()(uint32_t link) const { return static_cast<size_t>(link << 2); }
};

/* Fields accessors */

struct PolicyCompactGetRightLink
{
	uint32_t operator()(uint8_t *node) const
	{
		uint32_t link = *( reinterpret_cast<uint32_t *>(node + sizeof(uint32_t)) ) &
			~(policyCompactFlagsMask | policyCompactDataSizeMask);
		return link << 2;
	}
};

struct PolicyCompactGetLeftLink
{
	uint32_t operator()(uint8_t *node) const
	{
		uint32_t link = *( reinterpret_cast<uint32_t *>(node) ) & ~policyCompactSkipMask;
		return link << 2;
	}
};

#define DEFINE_FLAG_ACCESSORS(name, flag)				\
	struct PolicyCompactCheck ## name				\
	{								\
		uint32_t operator()(uint8_t *node) const		\
		{							\
			return *( reinterpret_cast<uint32_t *>		\
				  (node + sizeof(uint32_t)) ) &		\
				POLICY_COMPACT_ ## flag;		\
		}							\
	};								\
	struct PolicyCompactSet ## name					\
	{								\
		void operator()(uint8_t *node)				\
		{							\
			*( reinterpret_cast<uint32_t *>			\
			   (node + sizeof(uint32_t)) ) |=		\
				POLICY_COMPACT_ ## flag;		\
		}							\
	};								\
	struct PolicyCompactClear ## name				\
	{								\
		void operator()(uint8_t *node)				\
		{							\
			*( reinterpret_cast<uint32_t *>			\
			   (node + sizeof(uint32_t)) ) &=		\
				~POLICY_COMPACT_ ## flag;		\
		}							\
	}

DEFINE_FLAG_ACCESSORS(UserFlag1, USERFLAG1);
DEFINE_FLAG_ACCESSORS(UserFlag2, USERFLAG2);
DEFINE_FLAG_ACCESSORS(LeftTag,   LEFT_TAG);
DEFINE_FLAG_ACCESSORS(RightTag,  RIGHT_TAG);

#undef DEFINE_FLAG_ACCESSORS

struct PolicyCompactGetSkip
{
	uint32_t operator()(uint8_t *node) const
	{
		return *( reinterpret_cast<uint32_t *>(node) ) >> 22;
	}
};

struct PolicyCompactGetData
{
	const uint8_t *operator()(uint8_t *node) const
	{
		return node + policyCompactAuxSize;
	}
};

struct PolicyCompactSetData
{
	void operator()(uint8_t *node, const uint8_t *data, size_t dataSize)
	{
		if (data != 0)
			memcpy(node + policyCompactAuxSize, data, dataSize);
		policyCompactSetDataSize(node, dataSize);
	}
};

struct PolicyCompactSetNullData
{
	void operator()(uint8_t *node)
	{
		policyCompactSetDataSize(node, static_cast<size_t>(0));
	}
};

struct PolicyCompactSetRightLink
{
	void operator()(uint8_t *node, uint32_t link)
	{
		uint32_t *address = reinterpret_cast<uint32_t *>(node + sizeof(uint32_t));
		*address &= policyCompactFlagsMask | policyCompactDataSizeMask;
		*address |= (link >> 2) & ~(policyCompactFlagsMask | policyCompactDataSizeMask);
	}
};

struct PolicyCompactSetLeftLink
{
	void operator()(uint8_t *node, uint32_t link)
	{
		uint32_t *address = reinterpret_cast<uint32_t *>(node);
		*address &= policyCompactSkipMask;
		*address |= (link >> 2) & ~policyCompactSkipMask;
	}
};

struct PolicyCompactSetSkip
{
	void operator()(uint8_t *node, uint32_t value)
	{
		uint32_t *address  = reinterpret_cast<uint32_t *>(node);
		*address &= ~policyCompactSkipMask;
		*address |= (value << 22) & policyCompactSkipMask;
	}
};

struct PolicyCompactGetKey
{
	uint8_t *operator()(uint8_t *node) const { return policyCompactGetKey(node); }
};

struct PolicyCompactSetKey
{
	void operator()(uint8_t *node, const uint8_t *key, size_t keySize)
	{
		uint8_t *address = node + policyCompactAuxSize + policyCompactGetDataSize(node);
		memcpy( reinterpret_cast<char *>(address),
			reinterpret_cast<const char *>(key),
			keySize);
		address[keySize] = static_cast<uint8_t>('\0');
	}
};

struct PolicyCompactInitKey
{
        void operator()(const uint8_t *key, size_t keySize, uint8_t *target) const
        {
                memcpy(target, key, keySize);
                target[keySize] = '\0';
        }
        bool operator()() const { return false; }
};

struct PolicyCompactClearNode
{
	void operator()(uint8_t *node, size_t nodeSize) { bzero(node, nodeSize); }
};

struct PolicyCompactGetNodeAuxSize
{
	size_t operator()() const { return policyCompactAuxSize; }
};

struct PolicyCompactGetMinKeySize
{
	/* The minimal key size: key size field -- terminating zero */
	size_t operator()() const { return static_cast<size_t>(1); }
};

struct PolicyCompactGetKeySize
{
	/* The key size: its octets plus terminating '\0' */
	size_t operator()(size_t keyOctetsCount) const { return keyOctetsCount + 1; }
	size_t operator()(uint8_t *node) const
	{
		return strlen( reinterpret_cast<const char *>(PolicyCompactGetKey()(node)) );
	}
};

struct PolicyCompactGetNullDataSize
{
	size_t operator()() const { return static_cast<size_t>(0); }
};

struct PolicyCompactGetMaxTreeSize
{
	size_t operator()() const { return (static_cast<size_t>(1) << 24) - 1; }
};

struct PolicyCompactGetMaxDataSize
{
	size_t operator()() const { return (static_cast<size_t>(1) << 6) - 1; }
};

struct PolicyCompactGetDataSize
{
	size_t operator()(const uint8_t *node) const { return policyCompactGetDataSize(node); }
	size_t operator()(size_t dataSize) const { return dataSize; }
};

struct PolicyCompactCommonPrefixLength
{
	size_t operator()(const uint8_t *key, uint8_t *node) const
	{
                uint8_t *nodeKey = policyCompactGetKey(node);
                
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

struct PolicyCompact
{
	typedef uint32_t                               NodeAddress;

	typedef struct PolicyCompactDataToNodeAddress  DataToNodeAddress;
	typedef struct PolicyCompactOffsetToLink       OffsetToLink;
	typedef struct PolicyCompactLinkToOffset       LinkToOffset;
	
	typedef struct PolicyCompactGetRightLink       GetRightLink;
	typedef struct PolicyCompactSetRightLink       SetRightLink;
	typedef struct PolicyCompactGetLeftLink        GetLeftLink;
	typedef struct PolicyCompactSetLeftLink        SetLeftLink;
	typedef struct PolicyCompactCheckRightTag      CheckRightTag;
	typedef struct PolicyCompactSetRightTag        SetRightTag;
	typedef struct PolicyCompactClearRightTag      ClearRightTag;
	typedef struct PolicyCompactCheckLeftTag       CheckLeftTag;
	typedef struct PolicyCompactSetLeftTag         SetLeftTag;
	typedef struct PolicyCompactClearLeftTag       ClearLeftTag;
	typedef struct PolicyCompactCheckUserFlag1     CheckUserFlag1;
	typedef struct PolicyCompactSetUserFlag1       SetUserFlag1;
	typedef struct PolicyCompactClearUserFlag1     ClearUserFlag1;
	typedef struct PolicyCompactGetSkip            GetSkip;
	typedef struct PolicyCompactSetSkip            SetSkip;
	typedef struct PolicyCompactGetData            GetData;
	typedef struct PolicyCompactSetData            SetData;
	typedef struct PolicyCompactSetNullData        SetNullData;
	typedef struct PolicyCompactGetKey             GetKey;
	typedef struct PolicyCompactSetKey             SetKey;
        typedef struct PolicyCompactInitKey            InitKey;
	typedef struct PolicyCompactClearNode          ClearNode;
	typedef struct PolicyCompactGetNodeAuxSize     GetNodeAuxSize;
	typedef struct PolicyCompactGetMinKeySize      GetMinKeySize;
	typedef struct PolicyCompactGetKeySize         GetKeySize;
	typedef struct PolicyCompactGetNullDataSize    GetNullDataSize;
	typedef struct PolicyCompactGetDataSize        GetDataSize;
	typedef struct PolicyCompactGetMaxTreeSize     GetMaxTreeSize;
	typedef struct PolicyCompactGetMaxDataSize     GetMaxDataSize;
	typedef struct PolicyCompactCommonPrefixLength CommonPrefixLength;
};

#endif /* _POLICYCOMPACT_H_ */

