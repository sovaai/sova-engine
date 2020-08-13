#ifndef __FltTermsArray_h__
#define __FltTermsArray_h__
//============================================================================//
#ifdef _MSC_VER
#pragma warning(error:4706) // assignment within conditional expression
#endif
//----------------------------------------------------------------------------//
#include "FltTerm.h"
#include <stdio.h>
#include "lib/fstorage/fstorage.h"
#include "lib/aptl/avector.h"
//============================================================================//
class FltTermsArray
{
public:
  FltTermsArray(size_t element_size, size_t delta, size_t limit);
  ~FltTermsArray() { Reset(); }
  void operator=(const class FltTermsArray&) {}
  
  void  Reset();
  void  Clear() { m_used = 0; if(m_limit && m_alloc > m_limit) Reset(); }
  void  Truncate(size_t n) { if(n < m_used) m_used = n; }
  void* Append(bool use_memset = true); // returns new element or NULL if no memory

  void* At(size_t n) { return m_data + n * m_element_size; }
  const void* CAt(size_t n) const { return m_data + n * m_element_size; }

  size_t Size() const { return m_used; }

protected:
  char* m_data;
  size_t m_alloc;
  size_t m_used;

  const size_t m_element_size;
  const size_t m_delta;
  const size_t m_limit;
};
//============================================================================//
class FltTermsRefArray
{
public:
  struct reference
  {
    char* pt;
    size_t alloc;
    long ofs;
  };
  
public:
  ~FltTermsRefArray() { Reset(); }
  void operator=(const class FltTermsRefArray&) {}

  void SetDelta(size_t delta) { m_delta = delta; }

  void Reset();

  enum FltTermsErrors Serialize(int (* write_func)(struct fstorage_section_access_config *fsac, const void *buf, size_t nbytes),
      struct fstorage_section_access_config* fsac, size_t alignement, char* buf, size_t buf_size,
      size_t len_format) const;
  long CalculateOffsets(long base_ofs, size_t alignement, size_t len_format, bool terms_transport);

  inline size_t Size() const { return m_objs.size(); };
  inline size_t Last() { return m_objs.size() != 0 ? m_objs.size()-1 : 0; }

  inline const struct reference* RefAt(size_t n) const
    { return (n < m_objs.size() ? &m_objs[n] : NULL); }

protected:
  FltTermsRefArray(size_t obj_header_size, size_t obj_element_size, size_t obj_delta = 10);

  enum FltTermsErrors AddObject(const void* elements, size_t length, const void* hdr);
  enum FltTermsErrors InsertElement(size_t n, const void* element, size_t pos);
  enum FltTermsErrors AppendElement(size_t n, const void* element)
    { return InsertElement(n, element, ObjLength(n)); }

  inline const void* At(size_t n) const { return n < m_objs.size() ? m_objs[n].pt : NULL; }
  inline const void* AtLast() const { return m_objs.size() != 0 ? m_objs[m_objs.size()-1].pt : NULL; }

  inline const void* HeaderAt(size_t n) const { return n < m_objs.size() ? m_objs[n].pt + sizeof(uint32_t) : NULL; }
  inline const void* BodyAt(size_t n) const { return n < m_objs.size() ? m_objs[n].pt + sizeof(uint32_t) + m_obj_header_size : NULL; }

private:
  size_t ObjLength(size_t n) const
    { return *(uint32_t*)(m_objs[n].pt); }

  void SetObjLength(size_t n, size_t length)
    { *(uint32_t*)(m_objs[n].pt) = (uint32_t)length; }

  size_t ObjSize(size_t n) const
    { return sizeof(uint32_t)+m_obj_header_size+ObjLength(n)*m_obj_element_size; }

private:
  avector<reference> m_objs;

  size_t m_delta;

  const size_t m_obj_header_size;
  const size_t m_obj_element_size;
  const size_t m_obj_delta;
};
//----------------------------------------------------------------------------//
class FltTermsRefArrayIndexes : public FltTermsRefArray
{
public:
  FltTermsRefArrayIndexes(size_t obj_delta = 10)
    : FltTermsRefArray(TERMS_INDEX_HEADER_SIZE, TERMS_INDEX_ELEMENT_SIZE, obj_delta)
    {}

  TERMS_INDEX* At(size_t n) const
    { return (TERMS_INDEX*)FltTermsRefArray::At(n); }

  enum FltTermsErrors AddIndex(const TERMS_INDEX_ELEMENT* elements, size_t length,
      const TERMS_INDEX_HEADER* hdr)
    { return AddObject(elements, length, hdr); }

  enum FltTermsErrors InsertElement(size_t n, const TERMS_INDEX_ELEMENT* element, size_t pos)
  { return FltTermsRefArray::InsertElement(n, element, pos); }

};
//----------------------------------------------------------------------------//
class FltTermsRefArrayTermSets : public FltTermsRefArray
{
public:
  FltTermsRefArrayTermSets(size_t obj_delta = 10)
    : FltTermsRefArray(0, sizeof(TERMS_SET_REF), obj_delta)
  {}

  TERMS_SET* At(size_t n) const
    { return (TERMS_SET*)FltTermsRefArray::At(n); }

  enum FltTermsErrors AddTermSet(const TERMS_SET_REF* elements, size_t length)
    { return AddObject(elements, length, NULL); }

  enum FltTermsErrors AppendElement(size_t n, TERMS_SET_REF element)
    { return FltTermsRefArray::AppendElement(n, &element); }
};
//----------------------------------------------------------------------------//
class FltTermsRefArrayTerms : public FltTermsRefArray
{
public:
  FltTermsRefArrayTerms()
    : FltTermsRefArray(TERM_HDR_SIZE, sizeof(LEXID), 1)
  {}

  TERM* At(size_t n) const
    { return (TERM*)FltTermsRefArray::At(n); }

  enum FltTermsErrors AddTerm(const LEXID* elements, size_t length,
      const TERM_HDR* hdr)
    { return AddObject(elements, length, hdr); }
};
//============================================================================//
#endif
