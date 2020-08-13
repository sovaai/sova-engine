/**
 * @file   PolicyStandard.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr 21 17:07:31 2008
 * 
 * @brief  Standard policy for PatriciaTree.
 * 
 * 
 */
#ifndef _POLICYSTANDARD_H_
#define _POLICYSTANDARD_H_

#include <_include/_string.h>
#include <_include/_inttype.h>
#include <_include/cc_compat.h>

/* Standard data format (size in bits):
 * +-------+-------+------+------+-----+-----+------+------+--------+
 * | RLink | LLink | LTag | RTag | UF2 | UF1 | Skip | Data | Key    |
 * +-------+-------+------+------+-----+-----+------+------+--------+
 * | 32    | 32    | 1    | 1    | 1   | 1   | 28   | 32   | ASCIIZ |
 * +-------+-------+------+------+-----+-----+------+------+--------+
 *
 * RLink: right node offset
 * LLink: left node offset
 * UF1: userflag1
 * UF2: userflag2
 * LTag: left way tag
 * RTag: right way tag
 * Skip: skip bits count
 * Data: the data stored: uint32_t value
 * Key: asciiz string
 */

const size_t policyStandardAuxSize = (sizeof(uint32_t) * 3);

enum PolicyStandardTagFlag
{
	POLICY_STANDARD_USERFLAG1 = 0x10000000U,
	POLICY_STANDARD_USERFLAG2 = 0x20000000U,
 	POLICY_STANDARD_RIGHT_TAG = 0x40000000U,
	POLICY_STANDARD_LEFT_TAG  = 0x80000000U
};

const uint32_t policyStandardFlagsMask =
	POLICY_STANDARD_USERFLAG1 |
	POLICY_STANDARD_USERFLAG2 |
	POLICY_STANDARD_RIGHT_TAG |
	POLICY_STANDARD_LEFT_TAG;

/* Local helpers */

inline uint8_t *policyStandardGetKey(uint8_t *node)
{
	return (node + policyStandardAuxSize + sizeof(uint32_t));
}

/* Address conversions */

struct PolicyStandardDataToNodeAddress
{
	const uint8_t *operator()(const uint8_t *dataAddress) const
	{
		return dataAddress - policyStandardAuxSize;
	}
};

struct PolicyStandardOffsetToLink
{
	uint32_t operator()(size_t offset) const { return static_cast<uint32_t>(offset); }
};

struct PolicyStandardLinkToOffset
{
	size_t operator()(uint32_t link) const { return static_cast<size_t>(link); }
};

/* Fields accessors */

struct PolicyStandardGetRightLink
{
	uint32_t operator()(uint8_t *node) const { return *( reinterpret_cast<uint32_t *>(node) ); }
};

struct PolicyStandardGetLeftLink
{
	uint32_t operator()(uint8_t *node) const { return *( reinterpret_cast<uint32_t *>(node + sizeof(uint32_t)) ); }
};

#define DEFINE_FLAG_ACCESSORS(name, flag)				\
	struct PolicyStandardCheck ## name				\
	{								\
		uint32_t operator()(uint8_t *node) const		\
		{							\
			return *( reinterpret_cast<uint32_t *>		\
				  (node + (sizeof(uint32_t) << 1)) ) &	\
				POLICY_STANDARD_ ## flag;		\
		}							\
	};								\
	struct PolicyStandardSet ## name				\
	{								\
		void operator()(uint8_t *node)				\
		{							\
			*( reinterpret_cast<uint32_t *>			\
			   (node + (sizeof(uint32_t) << 1)) ) |=	\
				POLICY_STANDARD_ ## flag;		\
		}							\
	};								\
	struct PolicyStandardClear ## name				\
	{								\
		void operator()(uint8_t *node)				\
		{							\
			*( reinterpret_cast<uint32_t *>			\
			   (node + (sizeof(uint32_t) << 1)) ) &=	\
				~POLICY_STANDARD_ ## flag;		\
		}							\
	}

DEFINE_FLAG_ACCESSORS(UserFlag1, USERFLAG1);
DEFINE_FLAG_ACCESSORS(UserFlag2, USERFLAG2);
DEFINE_FLAG_ACCESSORS(LeftTag,   LEFT_TAG);
DEFINE_FLAG_ACCESSORS(RightTag,  RIGHT_TAG);

#undef DEFINE_FLAG_ACCESSORS

struct PolicyStandardGetSkip
{
	uint32_t operator()(uint8_t *node) const
	{
		return *( reinterpret_cast<uint32_t *>(node + (sizeof(uint32_t) << 1)) ) &
			(~policyStandardFlagsMask);
	}
};

struct PolicyStandardGetData
{
	const uint8_t *operator()(uint8_t *node) const
	{
		return node + policyStandardAuxSize;
	}
};

struct PolicyStandardSetData
{
	void operator()(uint8_t *node, const uint8_t *data, size_t dataSize)
	{
		if ( likely(dataSize == sizeof(uint32_t)) )
			*( reinterpret_cast<uint32_t *>(node + policyStandardAuxSize) ) =
				( data == 0 ? 0 : *( reinterpret_cast<const uint32_t *>(data) ) );
	}
};

struct PolicyStandardSetNullData
{
	void operator()(uint8_t *node)
	{
		*( reinterpret_cast<uint32_t *>(node + policyStandardAuxSize) ) = static_cast<uint32_t>(0);
	}
};

struct PolicyStandardSetRightLink
{
	void operator()(uint8_t *node, uint32_t link)
	{
		*( reinterpret_cast<uint32_t *>(node) ) = link;
	}
};

struct PolicyStandardSetLeftLink
{
	void operator()(uint8_t *node, uint32_t link)
	{
		*( reinterpret_cast<uint32_t *>(node + sizeof(uint32_t)) ) = link;
	}
};

struct PolicyStandardSetSkip
{
	void operator()(uint8_t *node, uint32_t value)
	{
		uint32_t *address  = reinterpret_cast<uint32_t *>(node + (sizeof(uint32_t) << 1));
		*address &= policyStandardFlagsMask;
		*address |= value & (~policyStandardFlagsMask);
	}
};

struct PolicyStandardGetKey
{
	uint8_t *operator()(uint8_t *node) const { return policyStandardGetKey(node); }
};

struct PolicyStandardSetKey
{
	void operator()(uint8_t *node, const uint8_t *key, size_t keySize)
	{
		uint8_t *address = node + policyStandardAuxSize + sizeof(uint32_t);
		memcpy( reinterpret_cast<char *>(address),
			reinterpret_cast<const char *>(key),
			keySize);
		address[keySize] = static_cast<uint8_t>('\0');
	}
};

struct PolicyStandardClearNode
{
	void operator()(uint8_t *node, size_t nodeSize) { bzero(node, nodeSize); }
};

struct PolicyStandardGetNodeAuxSize
{
	size_t operator()() const { return policyStandardAuxSize; }
};

struct PolicyStandardGetMinKeySize
{
	/* The minimal key size: terminating '\0' */
	size_t operator()() const { return static_cast<size_t>(1); }
};

struct PolicyStandardGetKeySize
{
	/* The key size: its octets plus terminating '\0' */
	size_t operator()(size_t keyOctetsCount) const { return keyOctetsCount + 1; }
	size_t operator()(uint8_t *node) const
	{
		return strlen( reinterpret_cast<const char *>(PolicyStandardGetKey()(node)) );
	}
};

struct PolicyStandardInitKey
{
        void operator()(const uint8_t *key, size_t keySize, uint8_t *target) const
        {
                memcpy(target, key, keySize);
                target[keySize] = '\0';
        }
        bool operator()() const { return false; }
};

struct PolicyStandardGetNullDataSize
{
	size_t operator()() const { return sizeof(uint32_t); }
};

struct PolicyStandardGetMaxTreeSize
{
	size_t operator()() const { return static_cast<size_t>(-1); }
};

struct PolicyStandardGetMaxDataSize
{
	size_t operator()() const { return sizeof(uint32_t); }
};

struct PolicyStandardGetDataSize
{
	size_t operator()(const uint8_t */* node */) const { return sizeof(uint32_t); }
	size_t operator()(size_t dataSize) const { return dataSize; }
};

struct PolicyStandardCommonPrefixLength
{
	size_t operator()(const uint8_t *key, uint8_t *node) const
	{
                uint8_t *nodeKey = policyStandardGetKey(node);
                
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

struct PolicyStandard
{
	typedef uint32_t                                NodeAddress;

	typedef struct PolicyStandardDataToNodeAddress  DataToNodeAddress;
	typedef struct PolicyStandardOffsetToLink       OffsetToLink;
	typedef struct PolicyStandardLinkToOffset       LinkToOffset;
	
	typedef struct PolicyStandardGetRightLink       GetRightLink;
	typedef struct PolicyStandardSetRightLink       SetRightLink;
	typedef struct PolicyStandardGetLeftLink        GetLeftLink;
	typedef struct PolicyStandardSetLeftLink        SetLeftLink;
	typedef struct PolicyStandardCheckRightTag      CheckRightTag;
	typedef struct PolicyStandardSetRightTag        SetRightTag;
	typedef struct PolicyStandardClearRightTag      ClearRightTag;
	typedef struct PolicyStandardCheckLeftTag       CheckLeftTag;
	typedef struct PolicyStandardSetLeftTag         SetLeftTag;
	typedef struct PolicyStandardClearLeftTag       ClearLeftTag;
	typedef struct PolicyStandardCheckUserFlag1     CheckUserFlag1;
	typedef struct PolicyStandardSetUserFlag1       SetUserFlag1;
	typedef struct PolicyStandardClearUserFlag1     ClearUserFlag1;
	typedef struct PolicyStandardGetSkip            GetSkip;
	typedef struct PolicyStandardSetSkip            SetSkip;
	typedef struct PolicyStandardGetData            GetData;
	typedef struct PolicyStandardSetData            SetData;
	typedef struct PolicyStandardSetNullData        SetNullData;
	typedef struct PolicyStandardGetKey             GetKey;
	typedef struct PolicyStandardSetKey             SetKey;
        typedef struct PolicyStandardInitKey            InitKey;
	typedef struct PolicyStandardClearNode          ClearNode;
	typedef struct PolicyStandardGetNodeAuxSize     GetNodeAuxSize;
	typedef struct PolicyStandardGetMinKeySize      GetMinKeySize;
	typedef struct PolicyStandardGetKeySize         GetKeySize;
	typedef struct PolicyStandardGetNullDataSize    GetNullDataSize;
	typedef struct PolicyStandardGetDataSize        GetDataSize;
	typedef struct PolicyStandardGetMaxTreeSize     GetMaxTreeSize;
	typedef struct PolicyStandardGetMaxDataSize     GetMaxDataSize;
	typedef struct PolicyStandardCommonPrefixLength CommonPrefixLength;
};

#endif /* _POLICYSTANDARD_H_ */

