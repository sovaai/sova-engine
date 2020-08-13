#include "Alpha.h"
//---------------------------------------------------------------------//
bool IsUpperAlpChar(unsigned char c)
{
  if(c >= 'A' && c <= 'Z')
    return true;
  else if(c >= RUSALP_UPPERFIRST && c <= RUSALP_UPPERLAST)
    return true;
  else if(c == RUSALP_UPPERJO)
    return true;
  else
    return false;
}
//---------------------------------------------------------------------//
bool IsRusAlpChar(unsigned char c)
{
  if(c >= RUSALP_LOWERFIRST /* && c <= RUSALP_LOWERLAST */)
    return true;
  else if(c == RUSALP_LOWERJO)
    return true;
  else if(c >= RUSALP_UPPERFIRST && c <= RUSALP_UPPERLAST)
    return true;
  else if(c == RUSALP_UPPERJO)
    return true;
  else
    return false;
}
//---------------------------------------------------------------------//
unsigned char upperchar(unsigned char c)
{
  if(c >= 'a' && c <= 'z')
    return (unsigned char)('A' + (c - 'a'));
  else if(c >= RUSALP_LOWERFIRST /* && c <= RUSALP_LOWERLAST */)
    return (unsigned char)(RUSALP_UPPERFIRST + (c - RUSALP_LOWERFIRST));
  else if(c == RUSALP_LOWERJO)
    return (unsigned char)RUSALP_UPPERJO;
  else
    return c;
}
//---------------------------------------------------------------------//
char* setupper(char* text)
{
  char* p = text;
  while(*p) {
    *p = upperchar(*p); p++;
  }
  return text;
}
//---------------------------------------------------------------------//
char* setlower(char* text)
{
  unsigned char* p = (unsigned char*)text;
  while(*p)
  {
    if(*p >= 'A' && *p <= 'Z')
      *p = (unsigned char)('a' + (*p - 'A'));
    else if(*p >= RUSALP_UPPERFIRST && *p <= RUSALP_UPPERLAST)
      *p = (unsigned char)(RUSALP_LOWERFIRST + (*p - RUSALP_UPPERFIRST));
    else if(*p == RUSALP_UPPERJO)
      *p = (unsigned char)RUSALP_LOWERJO;
    p++;
  }
  return text;
}
//---------------------------------------------------------------------//
void checkjo(char* text)
{
  unsigned char* p = (unsigned char*)text;
  while(*p)
  {
    if(*p == RUSALP_LOWERJO)
      *p = (unsigned char)RUSALP_LOWERJE;
    else if(*p == RUSALP_UPPERJO)
      *p = (unsigned char)RUSALP_UPPERJE;
    p++;
  }
}
//---------------------------------------------------------------------//
