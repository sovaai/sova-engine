#ifndef _wintypes_h_
#define _wintypes_h_

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_WIN32) && !defined(WIN32)

#if !defined(FAR)
#    define FAR
#endif

#define win32_unix(w,u) (u)

typedef unsigned char       UCHAR;
typedef unsigned short      WORD;
typedef unsigned short      USHORT;
typedef unsigned            DWORD;
typedef unsigned long       ULONG;
typedef unsigned int        UINT;
typedef signed   long       LONG;
typedef char *              PSTR;
typedef char FAR *          LPSTR;
typedef int                 BOOL;
typedef const char FAR *    LPCSTR;
typedef int       *         PINT;
typedef int   FAR *         LPINT;
typedef WORD      *         PWORD;
typedef WORD  FAR *         LPWORD;
typedef long      *         PLONG;
typedef long  FAR *         LPLONG;
typedef DWORD     *         PDWORD;
typedef DWORD FAR *         LPDWORD;
typedef void      *         PVOID;
typedef void  FAR *         LPVOID;
typedef const void  FAR*    LPCVOID;
typedef long long           LLONG;
typedef unsigned long long  QWORD;

#define MINCHAR     0x80
#define MAXCHAR     0x7f
#define MINSHORT    0x8000
#define MAXSHORT    0x7fff
#define MINLONG     0x80000000
#define MAXLONG     0x7fffffff
#define MAXBYTE     0xff
#define MAXUSHORT   0xffff
#define MAXWORD     0xffff
#define MAXULONG    0xffffffff
#define MAXDWORD    0xffffffff

#ifndef NULL
#  define NULL 0
#endif

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

#  define LOBYTE(w)                ((BYTE)(w))
#  define HIBYTE(w)                ((BYTE)((UINT)(w) >> 8))
#  define LOWORD(l)                ((WORD)(l))
#  define HIWORD(l)                ((WORD)((DWORD)(l) >> 16))
#  define MAKELONG(low, high)       ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#  define MAKELPARAM(low, high)    ((LPARAM)MAKELONG(low, high))
#  define MAKELRESULT(low, high)   ((LRESULT)MAKELONG(low, high))
#  define MAKELP(sel, off)         ((void FAR*)MAKELONG((off), (sel)))

#else /*WIN32*/

#if !defined(STRICT)
#define STRICT
#endif
#  include <windows.h>
#if defined(_MSC_VER)
#  pragma warning (disable : 4786) /* Debug info truncated */
#endif

#define win32_unix(w,u) (w)
#define snprintf _snprintf
#ifdef _MSC_VER
typedef __int64 int64_t;
typedef __int64 LLONG;
typedef unsigned __int64 QWORD;
#endif

#endif/*WIN32*/

#ifndef UNREF
#define UNREF(p) ((void)(p))
#endif

#ifndef ARRAY_SIZE
#  define ARRAY_SIZE(Arr)           (sizeof(Arr) / sizeof (Arr[0]))
#endif

#if defined(_MSC_EXTENSIONS)

#if !defined(APIENTRY)
#    define APIENTRY __stdcall
#endif
#if !defined(CDECL)
#    define CDECL  __cdecl
#endif
#if !defined(APIEXPORT)
#    define APIEXPORT(type) __declspec(dllexport) type CDECL
#endif

#else /* !_MSC_EXTENSIONS */

#if !defined(APIENTRY)
#    define APIENTRY
#endif
#if !defined(CDECL)
#    define CDECL
#endif
#if !defined(APIEXPORT)
#    define APIEXPORT(type) type
#endif

#endif /* _MSC_EXTENSIONS */

typedef unsigned char       BYTE;
typedef BYTE       *        PBYTE;
typedef BYTE  FAR  *        LPBYTE;
typedef const BYTE *        PCBYTE;
typedef const BYTE FAR *    LPCBYTE;
typedef const WORD     *    PCWORD;
typedef const WORD FAR *    LPCWORD;
typedef const DWORD    *    PCDWORD;
typedef const DWORD FAR*    LPCDWORD;
typedef const void     *    PCVOID;

typedef unsigned short      wchar_16;
typedef wchar_16       *    pwchar_16;
typedef const wchar_16 *    pcwchar_16;

#ifndef DECLARE_HANDLE
typedef void*   HANDLE;
typedef HANDLE* PHANDLE;
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#endif

#ifdef __cplusplus
}
#endif

#endif /*_wintypes_h_*/
