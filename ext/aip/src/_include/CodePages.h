#ifndef _CodePages_h_
#define _CodePages_h_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <_include/language.h>

int ConvertUTF(unsigned char * pData, int nLen/*=-1*/, int nDestCodePage);

int Convert2UTF_len(const unsigned char * pSrc, int ccSrc, int nSrcCodePage, unsigned char * pDst, int ccDst, size_t * pWrt);

int Convert2UTF_lengths(const unsigned char * pSrc, int ccSrc, int nSrcCodePage, unsigned char * pDst, int ccDst, size_t * pWrt, unsigned char * pLenPairs);

int ConvertUTF_Ex(unsigned char * pData, int nSize/*=-1*/, int nLen/*=-1*/, int nDestCodePage, int skipInvalid/*=0*/);

int ConvertUTF_Full(unsigned char * pData, int *nSizePtr, int nLen, int nDestCodePage, int skipInvalid);
        
int ConvertUTF_Ex_2buf(const unsigned char * pSrc, int ccSrc, unsigned char * pDst, int ccDst,  int nDstCodePage);

int ConvertCodepage(const unsigned char * pSrc, int ccSrc, int nSrcCodePage, unsigned char * pDst, int ccDst, int nDstCodePage);

int ConvertCodepage_inplace(unsigned char * pBuf, int ccBuf, int NoSrcChars, int nSrcCodePage, int nDstCodePage);

int Convert2UTF(const unsigned char * pSrc, int ccSrc, int nSrcCodePage, unsigned char * pDst, int ccDst);

int Convert2UTF_Ex(unsigned char * pSrc, int nSize, int nLen, int nSrcCodePage);

int ConvertData(unsigned char* pData, int nLen/*=-1*/, 
                int nSourceCodePage,  int nDestCodePage );
        
int ConvertData_Ex(unsigned char* pData, int nSize/*=-1*/, int nLen/*=-1*/, 
                   int nSourceCodePage,  int nDestCodePage, int skipInvalid/*=0*/ );

int ConvertString( char *pData, int nLen/*=-1*/, 
                   int nSourceCodePage, int nDestCodePage );
/* All ConvertXXX() functions return destination length */

int CPstr2i(const char* pszCP);  /* short name to cpID */
const char* CPi2str(int n);      /* cpID to short name */
const char* CPi2strRFC(int n);   /* cpID to RFC name */
int CPname2i(const char* pszCP); /* any name to cpID */

const unsigned char* CP2CP_Rus(int nSourceCodePage, int nDestCodePage);
                                 /* Russian 8 bit codepages conversion */

unsigned char Unicode2cp(unsigned wc, int nDestCodePage);
int Unicode2cpEx(unsigned wc, int nDestCodePage, unsigned char *pDst, int nDst);
int Cp2cpViaUnicode(unsigned char wc, int nCodePage, unsigned char *pDst, int nDst);
        
unsigned CP2Unicode( wchar_t tab[256], unsigned char c);
#define CP2Unicode(tab,c) ((tab)[(unsigned char)(c)])

/*
   Packs Unicode code point into UTF-8 sequence.
   It is meant to be used with single symbols (such as HTML entities),
   not for string conversion.
   Returns number of bytes in the sequence.
*/
int UCS2UTF8(unsigned wchar, unsigned char * pDst, int ccDst);

/*
   Converts the string from singlebyte encoding into UCS2 (2 byte UTF16 subset)
   returns number of converted characters
*/
int Convert2UCS2(const unsigned char * pSrc, int ccSrc, int nSrcCodePage, unsigned char * pDst, int ccDst);

#ifdef __cplusplus
}
#endif

#endif
