#if !defined( __STDIO_h__ )
#define  __STDIO_h__

#include <stdio.h>

#ifdef _MSC_VER
    #define snprintf _snprintf
#endif

#ifdef OS_LINUX
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <_include/_io.h>
static FILE * ap_fopen(const char * path, const char * mode)
{
  int flags = O_LARGEFILE;
  if (strchr(mode,'b')) flags |= O_BINARY;
  if (strchr(mode,'r')) {
    if (strchr(mode,'+')) flags |= O_RDWR;
    else flags |= O_RDONLY;
  }
  else if (strchr(mode,'w')) {
    flags |= O_CREAT | O_TRUNC;
    if (strchr(mode,'+')) flags |= O_RDWR;
    else flags |= O_WRONLY;
  }
  else if (strchr(mode,'a')) {
    flags |= O_CREAT | O_APPEND;
    if (strchr(mode,'+')) flags |= O_RDWR;
    else flags |= O_WRONLY;
  }
  int fd = open(path, flags, 0666);
  if (-1 == fd) return NULL;
  return fdopen(fd, mode);
}
#else
#  define ap_fopen fopen
#endif

#endif  // __STDIO_h__
