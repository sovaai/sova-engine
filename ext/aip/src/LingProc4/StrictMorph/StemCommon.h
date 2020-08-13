/**
 * @file   StemCommon.h
 * @author swizard <me@swizard.info>
 * @date   Sat May 17 21:57:06 2008
 * 
 * @brief  Common stem operations for StemRecordsArray and Lexeme classes.
 * 
 * 
 */
#ifndef _STEMCOMMON_H_
#define _STEMCOMMON_H_

typedef uint32_t StemId;
typedef uint8_t  StemHomoNumber;
typedef uint32_t StemNodeRef;

const   size_t   stemsExtraFlagsSize = 3;

const StemHomoNumber stemHomoDummy = static_cast<StemHomoNumber>(0x0000000FUL);
const StemNodeRef    stemNodeDummy = static_cast<StemNodeRef>   (0x00FFFFFFUL);

inline StemId stemConstruct(StemNodeRef ref, StemHomoNumber homo, int endListMark = 0)
{
	ref  &= stemNodeDummy;
	homo &= stemHomoDummy;
	return
		static_cast<StemId>(ref)                                              |
		( (static_cast<StemId>(homo) & 0x0F) << ((sizeof(StemId) - 1) << 3) ) |
		( endListMark != 0 ?
		  ( static_cast<StemId>(0x10) << ((sizeof(StemId) - 1) << 3) ) :
		  0 );
}

inline int stemConstructDummy( int endListMark = 0 )
{
	return stemConstruct( stemNodeDummy, stemHomoDummy, endListMark );
}

inline int stemListEndMark(StemId id)
{
	return ( id >> ((sizeof(StemId) - 1) << 3) ) & 0x10;
}

inline StemHomoNumber getStemHomoNumber(StemId id)
{
	return (StemHomoNumber)(( id >> ((sizeof(StemId) - 1) << 3) ) & 0x0F);
}

inline StemNodeRef getStemNodeRef(StemId id)
{
	return id & ((1 << ((sizeof(StemId) - 1) << 3)) - 1);
}

inline int isStemDummy(StemId id)
{
	return (getStemHomoNumber(id) == stemHomoDummy && getStemNodeRef(id) == stemNodeDummy);
}

#endif /* _STEMCOMMON_H_ */

