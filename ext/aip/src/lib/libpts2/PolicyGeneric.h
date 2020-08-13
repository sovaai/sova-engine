/**
 * @file   PolicyGeneric.h
 * @author swizard <me@swizard.info>
 * @date   Tue Dec 16 16:20:41 2008
 * 
 * @brief  Standard policy with byte vector key and byte vector data for PatriciaTree.
 * 
 * 
 */
#ifndef _POLICYGENERIC_H_
#define _POLICYGENERIC_H_

#include <_include/_string.h>
#include <_include/_inttype.h>
#include <_include/cc_compat.h>

/* Generic data format (size in bits):
 * +-------+-------+------+------+-----+-----+------+----------+------+----------------+
 * | RLink | LLink | LTag | RTag | UF2 | UF1 | Skip | DataSize | Data | KeySize  + Key |
 * +-------+-------+------+------+-----+-----+------+----------+------+----------------+
 * | 32    | 32    | 1    | 1    | 1   | 1   | 28   | 32       | ...  | 32       + ... |
 * +-------+-------+------+------+-----+-----+------+----0-----+------+----------------+
 *
 * RLink: right node offset
 * LLink: left node offset
 * UF1: userflag1
 * UF2: userflag2
 * LTag: left way tag
 * RTag: right way tag
 * Skip: skip bits count
 * KeySize: key size in octets
 * Key: key byte vector
 * DataSize: data size in octets
 * Data: data byte vector
 */

#ifndef PAD4
# define PAD4(value) (((value + 3) >> 2) << 2)
#endif

const size_t policyGenericAuxSize = (sizeof(uint32_t) * 3);

enum PolicyGenericTagFlag
{
	POLICY_GENERIC_USERFLAG1 = 0x10000000U,
	POLICY_GENERIC_USERFLAG2 = 0x20000000U,
 	POLICY_GENERIC_RIGHT_TAG = 0x40000000U,
	POLICY_GENERIC_LEFT_TAG  = 0x80000000U
};

const uint32_t policyGenericFlagsMask =
	POLICY_GENERIC_USERFLAG1 |
	POLICY_GENERIC_USERFLAG2 |
	POLICY_GENERIC_RIGHT_TAG |
	POLICY_GENERIC_LEFT_TAG;

/* Local helpers */

inline size_t policyGenericGetDataSize(const uint8_t *node)
{
	return *( reinterpret_cast<const uint32_t *>(node + policyGenericAuxSize) );
}

inline void policyGenericSetDataSize(uint8_t *node, size_t size)
{
	*( reinterpret_cast<uint32_t *>(node + policyGenericAuxSize) ) = static_cast<uint32_t>(size);
}

inline uint8_t *policyGenericGetKeySizePtr(uint8_t *node)
{
        uint32_t offset = policyGenericAuxSize + sizeof(uint32_t) + policyGenericGetDataSize(node);
        return node + PAD4(offset);
}

inline size_t policyGenericGetKeySize(uint8_t *node)
{
	return *( reinterpret_cast<uint32_t *>( policyGenericGetKeySizePtr(node) ) );
}

inline void policyGenericSetKeySize(uint8_t *node, size_t size)
{
	*( reinterpret_cast<uint32_t *>(policyGenericGetKeySizePtr(node)) ) = static_cast<uint32_t>(size);
}


/* Address conversions */

struct PolicyGenericDataToNodeAddress
{
	const uint8_t *operator()(const uint8_t *dataAddress) const
	{
		return dataAddress - policyGenericAuxSize - sizeof(uint32_t);
	}
};

struct PolicyGenericOffsetToLink
{
	uint32_t operator()(size_t offset) const { return static_cast<uint32_t>(offset); }
};

struct PolicyGenericLinkToOffset
{
	size_t operator()(uint32_t link) const { return static_cast<size_t>(link); }
};

/* Fields accessors */

struct PolicyGenericGetRightLink
{
	uint32_t operator()(uint8_t *node) const { return *( reinterpret_cast<uint32_t *>(node) ); }
};

struct PolicyGenericGetLeftLink
{
	uint32_t operator()(uint8_t *node) const { return *( reinterpret_cast<uint32_t *>(node + sizeof(uint32_t)) ); }
};

#define DEFINE_FLAG_ACCESSORS(name, flag)				\
	struct PolicyGenericCheck ## name				\
	{								\
		uint32_t operator()(uint8_t *node) const		\
		{							\
			return *( reinterpret_cast<uint32_t *>		\
				  (node + (sizeof(uint32_t) << 1)) ) &	\
				POLICY_GENERIC_ ## flag;		\
		}							\
	};								\
	struct PolicyGenericSet ## name                                 \
	{								\
		void operator()(uint8_t *node)				\
		{							\
			*( reinterpret_cast<uint32_t *>			\
			   (node + (sizeof(uint32_t) << 1)) ) |=	\
				POLICY_GENERIC_ ## flag;		\
		}							\
	};								\
	struct PolicyGenericClear ## name				\
	{								\
		void operator()(uint8_t *node)				\
		{							\
			*( reinterpret_cast<uint32_t *>			\
			   (node + (sizeof(uint32_t) << 1)) ) &=	\
				~POLICY_GENERIC_ ## flag;		\
		}							\
	}

DEFINE_FLAG_ACCESSORS(UserFlag1, USERFLAG1);
DEFINE_FLAG_ACCESSORS(UserFlag2, USERFLAG2);
DEFINE_FLAG_ACCESSORS(LeftTag,   LEFT_TAG);
DEFINE_FLAG_ACCESSORS(RightTag,  RIGHT_TAG);

#undef DEFINE_FLAG_ACCESSORS

struct PolicyGenericGetSkip
{
	uint32_t operator()(uint8_t *node) const
	{
		return *( reinterpret_cast<uint32_t *>(node + (sizeof(uint32_t) << 1)) ) &
			(~policyGenericFlagsMask);
	}
};

struct PolicyGenericGetData
{
	const uint8_t *operator()(uint8_t *node) const
	{
		return node + policyGenericAuxSize + sizeof(uint32_t);
	}
};

struct PolicyGenericSetData
{
	void operator()(uint8_t *node, const uint8_t *data, size_t dataSize)
	{
                policyGenericSetDataSize(node, dataSize);
                memcpy( node + policyGenericAuxSize + sizeof(uint32_t), data, dataSize );
	}
};

struct PolicyGenericSetNullData
{
	void operator()(uint8_t *node)
	{
                policyGenericSetDataSize(node, 0);
	}
};

struct PolicyGenericSetRightLink
{
	void operator()(uint8_t *node, uint32_t link)
	{
		*( reinterpret_cast<uint32_t *>(node) ) = link;
	}
};

struct PolicyGenericSetLeftLink
{
	void operator()(uint8_t *node, uint32_t link)
	{
		*( reinterpret_cast<uint32_t *>(node + sizeof(uint32_t)) ) = link;
	}
};

struct PolicyGenericSetSkip
{
	void operator()(uint8_t *node, uint32_t value)
	{
		uint32_t *address  = reinterpret_cast<uint32_t *>(node + (sizeof(uint32_t) << 1));
		*address &= policyGenericFlagsMask;
		*address |= value & (~policyGenericFlagsMask);
	}
};

struct PolicyGenericGetKey
{
	uint8_t *operator()(uint8_t *node) const { return (policyGenericGetKeySizePtr(node) + sizeof(uint32_t)); }
};

struct PolicyGenericSetKey
{
	void operator()(uint8_t *node, const uint8_t *key, size_t keySize)
	{
		uint8_t *address = policyGenericGetKeySizePtr(node);
		memcpy( address, key, keySize + sizeof(uint32_t) );
        }
};

struct PolicyGenericClearNode
{
	void operator()(uint8_t *node, size_t nodeSize) { bzero(node, nodeSize); }
};

struct PolicyGenericGetNodeAuxSize
{
	size_t operator()() const
        {
                return policyGenericAuxSize;
        }
};

struct PolicyGenericGetMinKeySize
{
	/* The minimal key size: 0 */
	size_t operator()() const { return static_cast<size_t>(0); }
};

struct PolicyGenericGetKeySize
{
	/* The key size: its octets */
	size_t operator()(size_t keyOctetsCount) const { return keyOctetsCount + sizeof(uint32_t); }
	size_t operator()(uint8_t *node) const { return policyGenericGetKeySize(node); }
};

struct PolicyGenericInitKey
{
        void operator()(const uint8_t *key, size_t keySize, uint8_t *target) const
        {
                uint32_t size = static_cast<uint32_t>(keySize);
                memcpy(target, &size, sizeof(uint32_t));
                memcpy(target + sizeof(uint32_t), key, keySize);
        }
        bool operator()() const { return true; }
};

struct PolicyGenericGetNullDataSize
{
	size_t operator()() const { return 0; }
};

struct PolicyGenericGetMaxTreeSize
{
	size_t operator()() const { return static_cast<size_t>(-1); }
};

struct PolicyGenericGetMaxDataSize
{
	size_t operator()() const { return static_cast<uint32_t>(-1); }
};

struct PolicyGenericGetDataSize
{
	size_t operator()(const uint8_t *node) const { return policyGenericGetDataSize(node); }
	size_t operator()(size_t dataSize) const { return sizeof(uint32_t) + PAD4(dataSize); }
};

struct PolicyGenericCommonPrefixLength
{
	size_t operator()(const uint8_t *key, uint8_t *node) const
	{
                uint8_t *nodeKey   = policyGenericGetKeySizePtr(node);
                size_t   totalSize = policyGenericGetKeySize(node) + sizeof(uint32_t);
                
		size_t i;
                ssize_t j;
		size_t commonBits = 0;

		for ( i = 0; i < totalSize; i++ )
		{
			if (key[i] == nodeKey[i])
				commonBits += (sizeof(uint8_t) << 3);
			else 
				break;
		}
                              
                if ( i < totalSize )
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

struct PolicyGeneric
{
	typedef uint32_t                               NodeAddress;

	typedef struct PolicyGenericDataToNodeAddress  DataToNodeAddress;
	typedef struct PolicyGenericOffsetToLink       OffsetToLink;
	typedef struct PolicyGenericLinkToOffset       LinkToOffset;
	
	typedef struct PolicyGenericGetRightLink       GetRightLink;
	typedef struct PolicyGenericSetRightLink       SetRightLink;
	typedef struct PolicyGenericGetLeftLink        GetLeftLink;
	typedef struct PolicyGenericSetLeftLink        SetLeftLink;
	typedef struct PolicyGenericCheckRightTag      CheckRightTag;
	typedef struct PolicyGenericSetRightTag        SetRightTag;
	typedef struct PolicyGenericClearRightTag      ClearRightTag;
	typedef struct PolicyGenericCheckLeftTag       CheckLeftTag;
	typedef struct PolicyGenericSetLeftTag         SetLeftTag;
	typedef struct PolicyGenericClearLeftTag       ClearLeftTag;
	typedef struct PolicyGenericCheckUserFlag1     CheckUserFlag1;
	typedef struct PolicyGenericSetUserFlag1       SetUserFlag1;
	typedef struct PolicyGenericClearUserFlag1     ClearUserFlag1;
	typedef struct PolicyGenericGetSkip            GetSkip;
	typedef struct PolicyGenericSetSkip            SetSkip;
	typedef struct PolicyGenericGetData            GetData;
	typedef struct PolicyGenericSetData            SetData;
	typedef struct PolicyGenericSetNullData        SetNullData;
	typedef struct PolicyGenericGetKey             GetKey;
	typedef struct PolicyGenericSetKey             SetKey;
        typedef struct PolicyGenericInitKey            InitKey;
	typedef struct PolicyGenericClearNode          ClearNode;
	typedef struct PolicyGenericGetNodeAuxSize     GetNodeAuxSize;
	typedef struct PolicyGenericGetMinKeySize      GetMinKeySize;
	typedef struct PolicyGenericGetKeySize         GetKeySize;
	typedef struct PolicyGenericGetNullDataSize    GetNullDataSize;
	typedef struct PolicyGenericGetDataSize        GetDataSize;
	typedef struct PolicyGenericGetMaxTreeSize     GetMaxTreeSize;
	typedef struct PolicyGenericGetMaxDataSize     GetMaxDataSize;
	typedef struct PolicyGenericCommonPrefixLength CommonPrefixLength;
};

#endif /* _POLICYGENERIC_H_ */




