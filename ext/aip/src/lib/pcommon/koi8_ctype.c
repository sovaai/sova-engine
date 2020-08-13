/* Copyright (C) 2001-2003 Alexey P. Ivanov, MSU, Dept. of Phys., Moscow
 *
 * PROJECT: PCOMMON
 * NAME:    KOI8 string and character manipulation support
 *
 * COMPILER: MS VC++ 6.0, GCC     TARGET: Win32,FreeBSD-x86,Linux-x86
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 06.11.01: v 1.00 created by AlexIV
 *==========================================================================
 */
#include "koi8_ctype.h"

const unsigned char _koi8_lchars_[] = "�����ţ��������������������������";
const unsigned char _koi8_uchars_[] = "��������������������������������";

const unsigned
char _koi8_ctype_[]=  {
0x00,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x28,0x28,0x28,0x28,0x28,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x88,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x10,0x10,0x10,0x10,0x10,0x10,
0x10,0x41,0x41,0x41,0x41,0x41,0x41,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x10,0x10,0x10,0x10,0x10,
0x10,0x42,0x42,0x42,0x42,0x42,0x42,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x10,0x10,0x10,0x10,0x20,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01};

const unsigned 
char _koi8_ToUP_[]=  {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xb3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,
0xff};

const unsigned 
char _koi8_ToLO_[]=  {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xa3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xdf};

char *koi8_strupr( char *string )
{
  if (string) {
    char *s = string;
    while(*s)
    {
      *s = _koi8_ToUP_[(unsigned char)*s];
      s++;
    }
  }
  return string;
}

char *koi8_strlwr( char *string )
{
  if (string) {
    char *s = string;
    while(*s)
    {
      *s = _koi8_ToLO_[(unsigned char)*s];
      s++;
    }
  }
  return string;
}

static const unsigned 
char _koi8_sort_[]=  {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xe5,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,
0xaf,0xb0,0xb1,0xc4,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,
0xfe,0xdf,0xe0,0xf6,0xe3,0xe4,0xf4,0xe2,0xf5,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,
0xef,0xff,0xf0,0xf1,0xf2,0xf3,0xe6,0xe1,0xfc,0xfb,0xe7,0xf8,0xfd,0xf9,0xf7,0xfa,
0xdd,0xbe,0xbf,0xd5,0xc2,0xc3,0xd3,0xc1,0xd4,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,
0xce,0xde,0xcf,0xd0,0xd1,0xd2,0xc5,0xc0,0xdb,0xda,0xc6,0xd7,0xdc,0xd8,0xd6,0xd9,
0xd9};

static const unsigned 
char _koi8_isort_[]=  {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x43,0x45,0x47,0x49,0x4b,0x4d,0x4f,0x51,0x53,0x55,0x57,0x59,0x5b,0x5d,
0x5f,0x61,0x63,0x65,0x67,0x69,0x6b,0x6d,0x6f,0x71,0x73,0x75,0x76,0x77,0x78,0x79,
0x7a,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xcb,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,
0xaf,0xb0,0xb1,0xca,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,
0xfd,0xbf,0xc1,0xed,0xc7,0xc9,0xe9,0xc5,0xeb,0xd1,0xd3,0xd5,0xd7,0xd9,0xdb,0xdd,
0xdf,0xff,0xe1,0xe3,0xe5,0xe7,0xcd,0xc3,0xf9,0xf7,0xcf,0xf1,0xfb,0xf3,0xef,0xf5,
0xfc,0xbe,0xc0,0xec,0xc6,0xc8,0xe8,0xc4,0xea,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,
0xde,0xfe,0xe0,0xe2,0xe4,0xe6,0xcc,0xc2,0xf8,0xf6,0xce,0xf0,0xfa,0xf2,0xee,0xf4,
0xf4};

#ifdef _MSC_VER
#pragma warning(disable : 4706) // assignment within conditional expression
#endif

int koi8_strcmp( const char *src, const char *dst )
{
  int ret = 0 ;

  while( !(ret = _koi8_sort_[*(unsigned char *)src] - 
                 _koi8_sort_[*(unsigned char *)dst]) && *dst)
     ++src, ++dst;

  if ( ret < 0 )
     ret = -1 ;
  else if ( ret > 0 )
     ret = 1 ;
  return( ret );
}

int koi8_strcasecmp( const char *src, const char *dst )
{
  int ret = 0 ;

  while( !(ret = _koi8_sort_[_koi8_ToLO_[*(unsigned char *)src]] - 
                 _koi8_sort_[_koi8_ToLO_[*(unsigned char *)dst]]) && *dst)
     ++src, ++dst;

  if ( ret < 0 )
     ret = -1 ;
  else if ( ret > 0 )
     ret = 1 ;
  return( ret );
}

int koi8_strtitlecasecmp( const char *src, const char *dst )
{
  int ret = 0 ;

  while( !(ret = _koi8_isort_[*(unsigned char *)src] - 
                 _koi8_isort_[*(unsigned char *)dst]) && *dst)
     ++src, ++dst;

  if ( ret < 0 )
     ret = -1 ;
  else if ( ret > 0 )
     ret = 1 ;
  return( ret );
}

#include <stddef.h>

int koi8_strncmp( const char *src, const char *dst, size_t count )
{
  int ret = 0 ;

  while( count && 
         !(ret = _koi8_sort_[*(unsigned char *)src] - 
                 _koi8_sort_[*(unsigned char *)dst]) && *dst )
     ++src, ++dst, count--;

  if ( ret < 0 )
     ret = -1 ;
  else if ( ret > 0 )
     ret = 1 ;
  return( ret );
}

int koi8_strncasecmp( const char *src, const char *dst, size_t count )
{
  int ret = 0 ;

  while( count && 
         !(ret = _koi8_sort_[_koi8_ToLO_[*(unsigned char *)src]] - 
                 _koi8_sort_[_koi8_ToLO_[*(unsigned char *)dst]]) && *dst)
     ++src, ++dst, count--;

  if ( ret < 0 )
     ret = -1 ;
  else if ( ret > 0 )
     ret = 1 ;
  return( ret );
}

int koi8_strntitlecasecmp( const char *src, const char *dst, size_t count )
{
  int ret = 0 ;

  while( count && 
         !(ret = _koi8_isort_[*(unsigned char *)src] - 
                 _koi8_isort_[*(unsigned char *)dst]) && *dst)
     ++src, ++dst, count--;

  if ( ret < 0 )
     ret = -1 ;
  else if ( ret > 0 )
     ret = 1 ;
  return( ret );
}

char *koi8_strcasestr(const char *str1, const char *str2)
{
  char *cp = (char *)str1;
  char *s1, *s2;
  
  if ( !*str2 )
      return((char *)str1);
  
  while (*cp)
  {
        s1 = cp;
        s2 = (char *)str2;

        while ( *s1 && *s2 && 
                (_koi8_ToLO_[*(unsigned char *)s1]==
                 _koi8_ToLO_[*(unsigned char *)s2]) )
                s1++, s2++;

        if (!*s2) return(cp);
        cp++;
  }
  return(NULL);
}

char *koi8_strlwrdiacritics(char *string)
{
  char *str = string;
  while (*str)
  {
    if (*str == '\xA3')
        *str='\xC5';
    else if (*str == '\xB3')
        *str='\xE5';
    ++str;
  }
  return string;
}
