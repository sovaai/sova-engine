#ifndef __StrUtils_h__
#define __StrUtils_h__

#include <string.h>
#include <stdio.h>
//----------------------------------------------------------------------//
void strapp(char* dest, const char* src, int maxtotalsize);
#define strapps(d, s) strapp(d, s, sizeof(d))

void  strappn(char* dest, const char* src, int num, int maxtotalsize);
const char* string(const int i);
const char* string(const unsigned i);
//----------------------------------------------------------------------//
#ifdef _MSC_VER
    #pragma warning(disable:4514) // unreferenced inline function has been removed
#endif
inline void  strclr(char* str)  { str[0]='\0'; }
inline void  strcopy(char* dest, const char* src, int maxtotalsize)
  { strclr(dest); strapp(dest, src, maxtotalsize); }
//----------------------------------------------------------------------//
void alltrim(char *s, bool replace_cr = true);
//----------------------------------------------------------------------//
int strcmpcase(const char* s1, const char* s2, bool caseflag);
//----------------------------------------------------------------------//
size_t GetWordLen(const char* str);
const char* GetNextWord(const char* str);
//----------------------------------------------------------------------//
char* fgets_n(char* buf, int size, FILE* f);
//----------------------------------------------------------------------//
#endif
