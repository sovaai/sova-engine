/**
 * @file   LexIDExtra.h
 * @author swizard <me@swizard.info>
 * @date   Fri Mar 21 15:52:30 2008
 * 
 * @brief  Additional routines for LEXIDs
 * 
 * 
 */
#ifndef _LEXIDEXTRA_H_
#define _LEXIDEXTRA_H_

#ifndef _LEXID32_H_
# ifndef _LEXID64_H_
#  error You should include LexID32.h or LexID64.h first
# endif
#endif

#include <_include/_inttype.h>
#include <_include/ntoh.h>

#if defined(FORCE_LEXID32)

#define HTONLEXID(lexid) (htobe32((uint32_t)lexid))
#define NTOHLEXID(lexid) (be32toh((uint32_t)lexid))

#elif defined(FORCE_LEXID64)

#define HTONLEXID(lexid) (htobe64((uint64_t)lexid))
#define NTOHLEXID(lexid) (be64toh((uint64_t)lexid))

#else
# error No FORCE_LEXID32 nor FORCE_LEXID64 defined
#endif

#endif /* _LEXIDEXTRA_H_ */

