//===========================================================================
#ifndef __Translit_Cp2Uni_H__
#define __Translit_Cp2Uni_H__
//===========================================================================
#ifdef __cplusplus
extern "C" {
#endif
//-------------------------------------------------------------------------
#include <_include/_inttype.h>
#include <stdlib.h>

extern const uint16_t tabLat12Usc[256];
extern const uint16_t tabWin2Usc[256];
extern const uint16_t tabKoi2Usc[256];
extern const uint16_t tabIso2Usc[256];
extern const uint16_t tabDos2Usc[256];
extern const uint16_t tabMac2Usc[256];
extern const uint16_t tabViet2Ucs[256];
extern const uint16_t tabArab2Ucs[256];

extern const char* const tabUsc2Win[256];
extern const char* const tabUsc2Koi[256];
extern const char* const tabUsc2Iso[256];
extern const char* const tabUsc2Dos[256];
extern const char* const tabUsc2Mac[256];
extern const char* const tabUcs2Viet[256];
extern const char* const tabUsc2Lat1[256];
extern const char* const tabUsc2Lat1_cyr[256];

typedef struct
{
    size_t length;
    const uint16_t *vec;
} ucs_vec_t;

extern const ucs_vec_t * const * const tabUcsVecViet[256];
uint16_t cp2UcCollapsingSeqViet(const unsigned char *chars, int charsCount, int *charsProcessed);

uint32_t EucJpSeq2Unicode(const unsigned char *chars, int charsCount, int *charsProcessed);

//-------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//===========================================================================
#endif // __Translit_Cp2Uni_H__
//===========================================================================
