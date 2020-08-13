#ifndef __MemMapFileUnix_h__
#define __MemMapFileUnix_h__
#ifndef WIN32

inline MemMapFile::MemMapFile() : handle(-1), maplen(0), lpdata(NULL)
{
}

inline MemMapFile::~MemMapFile()
{
  close();
}

inline MemMapFile& MemMapFile::operator = (MemMapFile& source)
{
  close();

  handle = source.handle;
  source.handle = -1;
  
  maplen = source.maplen;
  source.maplen = 0;

  lpdata = source.lpdata;
  source.lpdata = NULL;

  return *this;
}

inline int MemMapFile::assign(int fd)
{
  if(fd < 0)
    return -1;

  handle = fd;

  return 0;
}

inline int MemMapFile::open(const char* lpname, int flags, int mode)
{
  close();
  if((handle = ::open(lpname, flags, mode
#ifdef OS_LINUX
  | O_LARGEFILE
#endif  
  )) == -1)
    return errno;
  return 0;
}

inline int MemMapFile::close()
{
  unmap();

  if(handle != -1)
  {
    ::close(handle);
    handle = -1;
  }

  return 0;
}

inline const void* MemMapFile::map()
{
  struct stat fiinfo;

  // get file info
  if(fstat(handle, &fiinfo) < 0)
    return NULL;

  #ifdef OS_LINUX
    maplen = (fiinfo.st_size / getpagesize() + 1) * getpagesize();
  #else
    maplen = fiinfo.st_size;
  #endif

  if((lpdata = mmap(NULL, maplen, PROT_READ, MAP_SHARED, handle, 0)) == MAP_FAILED)
  {
    maplen = 0;
    lpdata = NULL;
    return NULL;
  }

  return lpdata;
}


inline void MemMapFile::unmap()
{
  if(lpdata != NULL && lpdata != MAP_FAILED)
  {
    munmap(lpdata, maplen);
    lpdata = NULL;
    maplen = 0;
  }
}

inline int MemMapFile::read(void* buf, size_t nbytes)
{
  return ::read(handle, buf, nbytes);
}

inline int MemMapFile::write(const void* buf, size_t nbytes)
{
  return ::write(handle, buf, nbytes);
}

inline int MemMapFile::pread(mm_off offset, void* buf, size_t nbytes)
{
  return ::pread(handle, buf, nbytes, offset);
}

inline int MemMapFile::pwrite(mm_off offset, const void* buf, size_t nbytes)
{
  return ::pwrite(handle, buf, nbytes, offset);
}

inline mm_off MemMapFile::lseek(mm_off offset, int origin)
{
  return ::lseek(handle, offset, origin);
}

inline int MemMapFile::sync()
{
  return fsync(handle);
}

inline int MemMapFile::truncate(mm_off offset)
{
  return ftruncate(handle, offset);
}

inline bool MemMapFile::isopen() const
{
  return handle != -1;
}

#endif // WIN32
#endif
