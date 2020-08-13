/**
 * @file   LexID.h
 * @author swizard <me@swizard.info>
 * @date   Tue Feb 26 20:31:27 2008
 * 
 * @brief  LingProc meta-header, a frontend for LexID32.h or LexID64.h depending on compilation mode.
 * 
 * 
 */
#ifndef _LEXID_H_
#define _LEXID_H_

#ifdef FORCE_LEXID64
# include "LexID64.h"
#else
# include "LexID32.h"
#endif

#endif /* _LEXID_H_ */

