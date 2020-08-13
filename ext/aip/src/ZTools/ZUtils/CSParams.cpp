#include <assert.h>
#include <stdio.h>
#include "CSParams.h"
//======================================================================//

CSParams::CSParams()
{
  Clear();
}
//----------------------------------------------------------------------//

CSParams::CSParams( int argc, char** argv , const char** temps, int tempsnum )
{
  Clear();
  Parse(argc, argv, temps, tempsnum);
}
//----------------------------------------------------------------------//

CSParams::CSParams( int argc, char** argv )
{
  Clear();
  Parse(argc, argv);
}
//----------------------------------------------------------------------//

CSParams::~CSParams()
{
}
//======================================================================//

// Очистка.
void CSParams::Clear()
{
  Templates.clear();
  Values.clear();
  NumberOfParams = 0;
  NumberOfUnformatted = 0;
  errorcode = -1;
}
//======================================================================//

// Разбор параметров по темплейтам.
// Возвращаемые значения:
//    0 - корректное завершение ;
//    -1 - некорректные аргументы ;
//    -2 - некорректно заданы темплейты ;
//    -3 - дублирование имен ;
//    1 - некорректно задан параметр в коммандной строке;
//    2 - незарегистрированный параметр в коммандной строке;
int CSParams::Parse( int argc, char** argv , const char** temps, int tempsnum )
{
  if(!argv || argc <= 0 || tempsnum < 0 )
    return (errorcode = -1);

  Clear();

  errorcode = ParseTemplate(temps, tempsnum);
  if(!errorcode)
    errorcode = ParseArguments(argc, argv);

  assert(errorcode >= 0);
  
  return errorcode;
};
//======================================================================//

// Разбор темплейтов.
// Возвращаемые значения:
//    0 - корректное завершение ;
//    -1 - некорректные аргументы ;
//    -2 - некорректно заданы темплейты ;
//    -3 - дублирование имен ;
int CSParams::ParseTemplate( const char** temps, int tempsnum )
{
  Templates.clear();

  if(tempsnum < 0 )
    return -1;

  _STL::set<_STL::string> AllNames;
  CSPrs_Template temp;

  // Цикл по описаниям темплейтов
  for( int i = 0; i < tempsnum; i++ )
  {
    size_t pos;
    _STL::string str = temps[i];

    // Заполнение поля Ident
    pos = str.find('=');
    if(pos != str.npos)
    {
      if(pos == 0 || pos+1 == str.size()
                          || str[pos-1] == ' ' || str[pos+1] == ' ')
      {
        return -2;
      }
      temp.Ident = str.substr(pos+1);
      str.resize(pos);
    }
    else
      temp.Ident.clear();
    
    // Заполнение поля Sign
    if( str.size() < 2 )
    {
      return -2;
    }

    pos = 0;
    if(str[0] == '-')
    {
      temp.Sign = CSPrs_Template::HYPH;
      pos++;
    }
    else if(str[0] == '/')
    {
      temp.Sign = CSPrs_Template::SLASH;
      pos++;
    }
    else if(str[0] == '%')
    {
      temp.Sign = CSPrs_Template::NONE;
    }
    else
    {
      return -2;
    }

    // Заполнение поля Param
    size_t ret;
    if( (ret = str.find_first_of(": ")) != str.npos )
    {
      temp.Param = str.substr(pos, ret - pos);
      pos = ret;
    }
    else
    {
      temp.Param = str.substr(pos);
      pos = str.npos;
    }
    if(temp.Param.size() == 0)
    {
      return -2;
    }

    // Заполнение поля ValueFormat
    if( pos == str.npos )
    {
      temp.ValueFormat = CSPrs_Template::NO;
    }
    else if( str[pos] == ':' )
    {
      temp.ValueFormat = CSPrs_Template::COLON;
      pos++;
    }
    else
    {
      temp.ValueFormat = CSPrs_Template::SPACE;
      pos++;
    }

    if(temp.ValueFormat != CSPrs_Template::NO)
    {
      if(pos == str.size() || str[pos] != '$' || pos+1 != str.size())
      {
        return -2;
      }
      if(temp.Sign == CSPrs_Template::NONE)
      {
        return -2;
      }
    }

    // Проверка имен на повторение
    if( AllNames.find(temp.Param) != AllNames.end() )
    {
      return -3;
    }
    AllNames.insert(temp.Param);

    if( temp.Ident.size() > 0 )
    {
      if( AllNames.find(temp.Ident) != AllNames.end() )
      {
        return -3;
      }
      AllNames.insert(temp.Ident);
    }

    // Сохранение тепплейта
    Templates.insert(CSPrs_TemplMap::value_type(temp.Param, temp));
  } // Цикл по описаниям темплейтов

  return 0;
}
//======================================================================//

// Разбор параметров.
// Возвращаемые значения:
//    0 - корректное завершение ;
//    -1 - некорректные аргументы ;
//    1 - некорректно задан параметр в коммандной строке;
//    2 - незарегистрированный параметр в коммандной строке;
int CSParams::ParseArguments( int argc, char** argv )
{
  Values.clear();
  NumberOfParams = 0;
  NumberOfUnformatted = 0;

  if( argc < 1 || !argv )
    return -1;

  size_t pos;
  
  _STL::string str;
  _STL::string name;
  _STL::string value;

  CSPrs_Template::SignEnum Sign;
  CSPrs_TemplMap::iterator iterator;
  
  for( int i = 0; i < argc; i++ )
  {
    str = argv[i];
    pos = 0;
  
    // Определение Sign  и формирование имени в name
    if( str[0] == '-' /* || str[0] == '/' */)
    {
      if( str[0] == '-' )
        Sign = CSPrs_Template::HYPH;
      else
        Sign = CSPrs_Template::SLASH;
      
      name = str.substr(1);
      if( (pos = str.find(':')) != str.npos )
        name.resize(pos-1);    
    }
    else
    {
      Sign = CSPrs_Template::NONE;

      char buffer[256];
#ifdef WIN32
      _snprintf(buffer, sizeof(buffer), "%%%lu", NumberOfUnformatted);
#else
      snprintf(buffer, sizeof(buffer), "%%%lu", NumberOfUnformatted);
#endif
      name = buffer;
      
      NumberOfUnformatted++;
    }

    // Поиск шаблона и формирование значения
    iterator = Templates.find(name);
        
    if( iterator == Templates.end() )
    {
      if( Sign != CSPrs_Template::NONE )
        return 2;
      value = str;
    }
    else
    {
      if( (*iterator).second.Sign !=  Sign)
      {
        return 1;
      }

      if( (*iterator).second.ValueFormat == CSPrs_Template::COLON )
      {
        // Проверка на присутствие : в коммандной строке
        if( pos == str.npos )
          return 1;

        if( pos+1 == str.size() )
          return 1;

        value = str.c_str() + pos + 1;
      }
      else if( (*iterator).second.ValueFormat == CSPrs_Template::SPACE )
      {
        // Проверка на присутствие : в коммандной строке
        if( pos != str.npos )
          return 1;

        if( i+1 == argc )
          return 1;

        i++;

        value = argv[i];
      }
      else
      {
        if(Sign == CSPrs_Template::NONE)
          value = str;
        else
        {
          // Проверка на присутствие : в коммандной строке
          if(pos != str.npos)
            return 1;
          value.clear();
        }
      }      
    }

    // Занесение имен и значения в values
    Values.insert(CSPrs_ValuesMap::value_type(name, value));
    NumberOfParams++;
    if( iterator != Templates.end() && 
       (*iterator).second.Ident.size() > 0 )
    {
      name = (*iterator).second.Ident;
      Values.insert(CSPrs_ValuesMap::value_type(name, value));
    }
  }

  return 0;
}
//======================================================================//
void CSParams::SetValue(const char* param, const char* value)
{
  Values.insert(CSPrs_ValuesMap::value_type(param, value));
}
//======================================================================//

// Выдача значения.
// Возвращаемые значения:
//    строка, отличная от "" - если параметр существует и имеет значение;
//    "" - если параметр существует, но не имеет значения;
//    NULL - если параметр не существует;
const char* CSParams::operator[](const char* name)
{
  CSPrs_ValuesMap::iterator iterator;
  if( (iterator = Values.find(name)) != Values.end())
  {
    return (*iterator).second.c_str();
  }
  return NULL;
}
//----------------------------------------------------------------------//
const char* CSParams::operator[](int n)
{
  char buf[16];
  sprintf(buf, "%%%d", n);

  return (*this)[buf];
}
//======================================================================//
