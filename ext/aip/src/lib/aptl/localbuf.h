#ifndef __aptl_localbuf_h__
#define __aptl_localbuf_h__

#include <stddef.h>

template<typename T>
class CAutoBuffer {
   T* p;
   size_t l;
   CAutoBuffer(const CAutoBuffer&);
   CAutoBuffer& operator=(const CAutoBuffer&);
 public:
   CAutoBuffer() : p(NULL), l(0) {}
   CAutoBuffer(size_t len) : p(new T[len]), l(len) {}
   ~CAutoBuffer() { if(p) delete[] p; }
   T* get_buffer(size_t len) { if (len > l) { if(p) delete[] p; p=new T[len]; l=len; } return p; }
};

template<typename T, size_t N>
class CLocalBuffer : private CAutoBuffer<T> {
  typedef CAutoBuffer<T> Parent;
  T local[N];
  CLocalBuffer(const CLocalBuffer&);
  CLocalBuffer& operator=(const CLocalBuffer&);
 public:
  CLocalBuffer()  {}
  ~CLocalBuffer() {}
  T* get_buffer(size_t len) { return len > N ? Parent::get_buffer(len) : local; }
};

#endif
