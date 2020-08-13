#ifndef __EString_h__
#define __EString_h__
//======================================================================//
#ifdef _MSC_VER
#pragma warning(error:4706) // assignment within conditional expression
#pragma warning(disable:4786) // identifier was truncated to '255' characters in the debug information
#pragma warning(disable:4514) // unreferenced inline/local function has been removed
#pragma warning(disable:4710) // function not inlined
#endif
//======================================================================//
#include <stdio.h>
#include <string>
#include <vector>

#define UNICODE_SUPPORT
#include "_include/CodePages.h"

#ifndef _STLPORT_VERSION
#define _STL std
#endif

//----------------------------------------------------------------------//
void alltrim(_STL::string& str);
//----------------------------------------------------------------------//
class EString : public _STL::string
{

public:
  EString();
  EString(const char*);
  virtual ~EString();
  
  operator const char*() { return c_str(); }

  EString& operator = (const char* str)
    { assign(str); return *this; }
  EString& operator = (char* str)
    { assign(str); return *this; }
  EString& operator = (_STL::string str)
    { assign(str); return *this; }

  bool replace_sub(const char* from, const char* to);
  bool replace_sub_all(const char* from, const char* to);

  size_t find_limited(const char* str, size_t start, size_t end);
  int  replace_limited(const char* from, const char* to, size_t* start, size_t* end);
  bool replace_limited_all(const char* from, const char* to, size_t* start, size_t* end);
  
  bool truncate(const char *delim); 
  void trim();

  bool Load(FILE* file, char delim, int CodePage = 0);
  bool Load(FILE* file);

  bool Encode(int SrcCodePage, int DestCodePage);
  void LCase();
  void UCase();

private:
  int CodePage;
  _STL::vector<char> tmp_buff;

  void Copy2TmpBuffer(const char *s, size_t len);
};
//----------------------------------------------------------------------//
class BinBuffer : public _STL::vector<unsigned char>
{

public:
  BinBuffer() {}
  virtual ~BinBuffer() {}

  bool Load(FILE* file);
};
//======================================================================//
#endif
