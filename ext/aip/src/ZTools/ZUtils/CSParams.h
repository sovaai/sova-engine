#ifndef __CSPARAMS_H__
#define __CSPARAMS_H__
//======================================================================//
#ifdef _MSC_VER
#pragma warning(disable:4786) // identifier was truncated to '255' characters in the debug information
#pragma warning(disable:4514) // unreferenced inline/local function has been removed
#pragma warning(disable:4710) // function not inlined
#endif
//----------------------------------------------------------------------//
#include <string>
#include <map>
#include <set>

#ifndef _STLPORT_VERSION
#define _STL std
#endif

//======================================================================//
/*
ÿ¿¡ÀŒÕ€ œ¿–¿Ã≈“–Œ¬:
  -param[:$| $][=ident]
  /param[:$| $][=ident]
  %number[=ident]
*/
//======================================================================//
struct CSPrs_Template
{
  enum SignEnum {SLASH, HYPH, NONE} Sign;
  enum FormatEnum {SPACE, NO, COLON} ValueFormat;
  
  _STL::string Param;
  _STL::string Ident;
};
//----------------------------------------------------------------------//
typedef _STL::map<_STL::string, CSPrs_Template> CSPrs_TemplMap;
typedef _STL::map<_STL::string, _STL::string> CSPrs_ValuesMap;
//----------------------------------------------------------------------//
class CSParams
{
public:
  CSParams();
  CSParams( int argc, char** argv );
  CSParams( int argc, char** argv, const char** temps, int tempsnum );
  ~CSParams();
  void Clear();

  int Parse( int argc, char** argv, const char** temps = NULL, int tempsnum = 0);
  void SetValue(const char* param, const char* value);

  const char* operator[]( const char* name );
  const char* operator[]( int n );
  int MainNum()  { return (int)NumberOfUnformatted - 1; }
  int AllNum() { return (int)NumberOfParams - 1; }

  int Error() { return errorcode; }

protected:
  int ParseTemplate( const char** temps, int tempsnum );
  int ParseArguments( int argc, char** argv );
                     
protected:
  CSPrs_TemplMap Templates;
  CSPrs_ValuesMap Values;

  size_t NumberOfParams;
  size_t NumberOfUnformatted;
  int errorcode;
};
//======================================================================//
#endif
