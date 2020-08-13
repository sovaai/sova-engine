#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "LexID.h"
#include "BinaryData.h"

/* Constants */
static const uint16_t lpMaxMorphsNum     = (1 << BitsCounter< static_cast<uint64_t>(LEX_MORPH) >::n);
static const uint16_t lpMaxLangNum       = ( lpMaxMorphsNum - 3 /* hier, punct, num */ ) / 2; /* fuzzy and strict */
static const uint32_t lpMaxWordLength    = 4095;
static const uint32_t lpAvgWordImageSize = 3;

#endif /* _CONSTANTS_H_ */

