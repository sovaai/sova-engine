#ifndef _SM_DLL_H_
#define _SM_DLL_H_

#ifdef _WIN32
#ifdef SM_USE_DLL
#ifdef SM_BUILD_DLL
#define SM_DLL __declspec( dllexport )
#else
#define SM_DLL __declspec( dllimport )
#endif
#endif
#endif

#ifndef SM_DLL
#define SM_DLL
#endif

#endif
