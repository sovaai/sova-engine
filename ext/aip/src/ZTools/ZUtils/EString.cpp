#include "EString.h"
#include "StrUtils.h"
#include "Alpha.h"

//======================================================================//
EString::EString()
{
  CodePage = 0;
}
//----------------------------------------------------------------------//
EString::EString(const char* str)
{
  CodePage = 0;
  assign(str);
}
//----------------------------------------------------------------------//
EString::~EString()
{
}
//======================================================================//
bool EString::replace_sub(const char *from, const char *to)
{
  if(!from || !to)
    return false;
  
  size_type n = find(from);
  if(n != (size_type)npos)
  {
    replace(n, strlen(from), to);
    return true;
  }
  else
    return false;
}
//----------------------------------------------------------------------//
bool EString::replace_sub_all(const char *from, const char *to)
{
  if(!from || !to)
    return false;

  bool res = false;
  
  size_type n = find(from);
  while(n != (size_type)npos)
  {
    res = true;
    replace(n, strlen(from), to);
    n = find(from, n+strlen(to));
  }
  return res;
}
//----------------------------------------------------------------------//
size_t EString::find_limited(const char *str, size_t start, size_t end)
{
  size_type n = find(str, start);
  if(n != (size_type)npos && n+strlen(str)-1 < end)
    return n;
  else
    return end;
}
//----------------------------------------------------------------------//
int EString::replace_limited(const char *from, const char *to,
                                            size_t* start, size_t* end)
{
  size_type n = find(from, *start);
  if(n != (size_type)npos && n+strlen(from)-1 < *end)
  {
    replace(n, strlen(from), to);
    (*end) += (strlen(to) - strlen(from));
    return n + strlen(to);
  }
  else
    return -1;
}
//----------------------------------------------------------------------//
bool EString::replace_limited_all(const char *from, const char *to,
                                            size_t* start, size_t* end)
{
  int rc = 0;
  size_t pos = *start;

  while(rc != -1)
  {
    rc = replace_limited(from, to, &pos, end);
    pos = rc;
  }
  return true;
}
//======================================================================//
bool EString::truncate(const char *delim)
{
  if(!delim || *delim == '\0')
    return false;

  size_type n = find(delim);
  if(n != npos)
  {
    resize(n);
    return true;
  }
  else
    return false;
}
//----------------------------------------------------------------------//
void alltrim(_STL::string& str)
{
  _STL::vector<char> tmp_buff;
  size_t sz = str.size();

  if (tmp_buff.capacity() < sz + 1) {
    tmp_buff.reserve(sz + 1);
  }

#if !defined(_STLP_DEBUG)
    strncpy(&tmp_buff[0], str.c_str(), sz);
    tmp_buff[sz] = 0x0;
#else
    tmp_buff.clear();
    for (size_t i = 0; i < str.size(); i++) {
        tmp_buff.push_back(str[i]);
    }
    tmp_buff.push_back(0);
#endif

  alltrim(&tmp_buff[0]);
  str = &tmp_buff[0];
}
//----------------------------------------------------------------------//
void EString::trim()
{
  alltrim(*this);
}
//======================================================================//
bool EString::Load(FILE *file, char delim, int cp)
{
  clear();
  CodePage = cp;

  if(ferror(file) || feof(file))
    return false;

  while(!ferror(file) && !feof(file))
  {
    int c = fgetc(file);
    if(c != delim && c != EOF)
      append(1, c);
    else
      break;
  }

  if(feof(file) && length() == 0)
    return false;
  else
    return true;
}
//----------------------------------------------------------------------//
bool EString::Load(FILE *file)
{
  if(!file)
    return false;

  if(file == stdin)
    return Load(file, 0);

  const long max_size = 1024*1024*5;

  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  if(fsize > max_size)
    fsize = max_size;

  fseek(file, 0, SEEK_SET);

#if !defined(_STLP_DEBUG)
  if (tmp_buff.capacity() < (size_t)fsize + 1) {
    tmp_buff.reserve(fsize + 1);
  }
  fsize = fread(&tmp_buff[0], 1, fsize, file);
  tmp_buff[fsize] = 0x0;
  assign(&tmp_buff[0], fsize);
#else
  char *ptr = new char[fsize + 1];
  memset(ptr, 0, fsize + 1);
  fsize = fread(ptr, 1, fsize, file);
  assign(ptr, fsize);
  delete[] ptr;
#endif

  return true;
}
//======================================================================//
bool EString::Encode(int SrcCodePage, int DestCodePage)
{
  if(SrcCodePage != DestCodePage)
  {
    Copy2TmpBuffer(c_str(), size());
    ConvertData_Ex((unsigned char*)&tmp_buff[0], size(), -1, SrcCodePage,
                   DestCodePage, 0);
    assign(&tmp_buff[0], strlen(&tmp_buff[0])); // при конвертировании из unicode'а
    CodePage = DestCodePage;
  }
  return true;
}
//----------------------------------------------------------------------//
void EString::LCase()
{
  Copy2TmpBuffer(c_str(), size());
  setlower(&tmp_buff[0]);
  assign(&tmp_buff[0]);
}
//----------------------------------------------------------------------//
void EString::UCase()
{
  Copy2TmpBuffer(c_str(), size());
  setupper(&tmp_buff[0]);
  assign(&tmp_buff[0]);
}
//----------------------------------------------------------------------//
void EString::Copy2TmpBuffer(const char *s, size_t s_len)
{
    if (tmp_buff.capacity() < s_len + 1) {
        tmp_buff.reserve(s_len + 1);
    }

#if !defined(_STLP_DEBUG)
    strncpy(&tmp_buff[0], s, s_len);
    tmp_buff[s_len] = 0x0;
#else
    tmp_buff.clear();
    for (size_t i = 0; i < s_len; i++) {
        tmp_buff.push_back(s[i]);
    }
    tmp_buff.push_back(0);
#endif

}
//======================================================================//
bool BinBuffer::Load(FILE *file)
{
  if(!file)
    return false;

  const long max_size = 1024*1024*5;

  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  if(fsize > max_size)
    fsize = max_size;

  resize(fsize);
  fseek(file, 0, SEEK_SET);
  fsize = fread(&(operator[](0)), 1, fsize, file);
  resize(fsize);

  return true;
}
//======================================================================//
