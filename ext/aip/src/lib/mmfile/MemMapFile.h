#ifndef __MemMapFile_h__
#define __MemMapFile_h__

#undef MMFILE_USE_OFS_64

# ifdef WIN32
  # include <windows.h>
# else
  # include <unistd.h>
  # include <sys/types.h>
  # include <sys/mman.h>
  # include <sys/stat.h>
#endif

#include <fcntl.h>
#include <errno.h>

typedef long mm_off;

class MemMapFile
{
private:
  #ifdef WIN32
    HANDLE handle;
    HANDLE mapped;
  #else
    int handle;
    size_t maplen;
  #endif
    void* lpdata;

public:
          MemMapFile();
         ~MemMapFile();

  MemMapFile& operator = (MemMapFile& source);

  #ifndef WIN32
    int assign(int fd);
  #endif

  int open(const char* lpname, int flags, int mode = 0644);
  int close();
  const void* map();
  void unmap();
  int read(void* buf, size_t nbytes);
  int write(const void* buf, size_t nbytes);
  int pread(mm_off offset, void* buf, size_t nbytes);
  int pwrite(mm_off offset, const void* buf, size_t nbytes);
  mm_off lseek(mm_off offset, int origin);
  int sync();
  int truncate(mm_off offset);
  bool isopen() const;
};

#ifdef WIN32
  #include "MemMapFileWin.h"
#else
  #include "MemMapFileUnix.h"
#endif

#endif // __MemMapFile_h__
