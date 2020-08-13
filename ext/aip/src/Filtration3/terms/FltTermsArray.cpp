#include "FltTermsArray.h"
#include "terms.h" // error codes
#include "_include/ntoh.h"

#include <stdlib.h>
#include <string.h>
//----------------------------------------------------------------------//
#ifdef _MSC_VER
#pragma warning(disable:4514) // unreferenced inline/local function has been removed
#endif
//======================================================================//
FltTermsArray::FltTermsArray(size_t element_size, size_t delta, size_t limit)
  : m_element_size(element_size), m_delta(delta), m_limit(limit)
{
  m_data = NULL;
  m_alloc = 0;
  m_used = 0;
}
//----------------------------------------------------------------------//
void FltTermsArray::Reset()
{
  if(m_data)
  {
    free(m_data);
    m_data = NULL;
  }

  m_alloc = m_used = 0;
}
//----------------------------------------------------------------------//
void* FltTermsArray::Append(bool use_memset)
{
  if(m_used == m_alloc)
  {
    void* tmp = realloc(m_data, (m_alloc + m_delta) * m_element_size);
    if(!tmp)
      return NULL;
    m_data = (char*)tmp;
    m_alloc += m_delta;

    memset(m_data + (m_used  * m_element_size), 0, m_delta * m_element_size);
    use_memset = false;
  }

  void* pt = m_data + (m_used++ * m_element_size);

  if(use_memset)
    memset(pt, 0, m_element_size);

  return pt;
}
//======================================================================//
FltTermsRefArray::FltTermsRefArray(size_t obj_header_size,
             size_t obj_element_size, size_t obj_delta /* = 10 */)
	: m_objs( 16384, 2 ), m_delta(100),
	  m_obj_header_size(obj_header_size),
	  m_obj_element_size(obj_element_size),
	  m_obj_delta(obj_delta)
{
}
//----------------------------------------------------------------------//
void FltTermsRefArray::Reset()
{
  for(size_t i = 0; i < m_objs.size(); i++)
  {
    if(m_objs[i].pt)
      free(m_objs[i].pt);
  }
  m_objs.clear();
}
//----------------------------------------------------------------------//
enum FltTermsErrors FltTermsRefArray::AddObject(const void* elements, size_t length, const void* hdr)
{
  char* tmp;
  size_t to_alloc;

  // allocating object
  to_alloc = length > m_obj_delta ? length : m_obj_delta;
  tmp = (char*)malloc(sizeof(uint32_t) + m_obj_header_size + to_alloc * m_obj_element_size);
  if(!tmp)
    return TERMS_ERROR_ENOMEM;

  // adding object
  struct reference *n = m_objs.grow();
  if ( n == 0 )
    return TERMS_ERROR_ENOMEM;
  n->alloc = to_alloc;
  n->pt = tmp;
  
  // filling object
  *((uint32_t*)tmp) = length;
  tmp += sizeof(uint32_t);

  if(m_obj_header_size)
  {
    memcpy(tmp, hdr, m_obj_header_size);
    tmp += m_obj_header_size;
  }

  if(length)
  {
    memcpy(tmp, elements, length * m_obj_element_size);
  }

  return TERMS_OK;
}
//----------------------------------------------------------------------//
enum FltTermsErrors FltTermsRefArray::InsertElement(size_t n, const void* element, size_t pos)
{
  if(n >= m_objs.size())
    return TERMS_ERROR_EINVAL;

  struct reference &object = m_objs[n];
  size_t length = ObjLength(n);

  // reallocating object
  if(length == object.alloc)
  {
    size_t alloc_size = sizeof(uint32_t) + m_obj_header_size;
    alloc_size += (object.alloc + m_obj_delta) * m_obj_element_size;
    char* tmp = (char*)realloc(object.pt, alloc_size);
    if(!tmp)
      return TERMS_ERROR_ENOMEM;
    object.pt = tmp;
    object.alloc += m_obj_delta;
  }

  // inserting element
  if(pos > length)
    pos = length;

  char* array = object.pt + sizeof(uint32_t) + m_obj_header_size;
  char* insert_pt = array + pos * m_obj_element_size;
  
  if(pos != length)
    memmove(insert_pt + m_obj_element_size, insert_pt, (length-pos)*m_obj_element_size);
  memcpy(insert_pt, element, m_obj_element_size);

  SetObjLength(n, length+1);

  return TERMS_OK;
}
//----------------------------------------------------------------------//
struct write_context
{
  int (* write_func)(struct fstorage_section_access_config *fsac, const void *buf, size_t nbytes);
  struct fstorage_section_access_config* fsac;
  char* buf;
  size_t buf_size;
  size_t buf_used;
};

static enum FltTermsErrors write_buf(struct write_context* wc, const void* pt, size_t size)
{
  if((wc->buf && wc->buf_size) && wc->buf_used + size > wc->buf_size)
  {
    int wrote = wc->write_func(wc->fsac, wc->buf, wc->buf_used);
    if(wrote != (int)(wc->buf_used))
      return TERMS_ERROR_FS_FAILED;
    wc->buf_used = 0;
  }

  if(!wc->buf || !wc->buf_size || size > wc->buf_size)
  {
    int wrote = wc->write_func(wc->fsac, pt, size);
    if(wrote != (int)size)
      return TERMS_ERROR_FS_FAILED;
  }
  else
  {
    memcpy(wc->buf + wc->buf_used, pt, size);
    wc->buf_used += size;
  }

  return TERMS_OK;
}
//----------------------------------------------------------------------//
enum FltTermsErrors FltTermsRefArray::Serialize(int (* write_func)(struct fstorage_section_access_config *fsac, const void *buf, size_t nbytes),
              struct fstorage_section_access_config* fsac, size_t alignement, char* buf, size_t buf_size,
	      size_t len_format) const
{
  char align_buf[32];
  memset(align_buf, 0, sizeof(align_buf));

  struct write_context wc;
  memset(&wc, 0, sizeof(wc));

  wc.write_func = write_func;
  wc.fsac = fsac;
  wc.buf = buf;
  wc.buf_size = buf_size;
  wc.buf_used = 0;

  enum FltTermsErrors res;

  if(alignement > sizeof(align_buf))
    return TERMS_ERROR_EINVAL;

  long ofs = 0;
  for(size_t i = 0; i < m_objs.size(); i++)
  {
    if(RefAt(i)->ofs == -1)
      continue;

    if(ofs != RefAt(i)->ofs)
      return TERMS_ERROR_EFAULT;

    size_t obj_size = 0;
    if(len_format == 1)
    {
      uint8_t obj_length = (uint8_t)ObjLength(i);
      if((res = write_buf(&wc, &obj_length, sizeof(obj_length))) != TERMS_OK)
	return res;
      obj_size += len_format; 
    }
    else if(len_format == 2)
    {
      uint16_t obj_length = htons(uint16_t(ObjLength(i)));
      if((res = write_buf(&wc, &obj_length, sizeof(obj_length))) != TERMS_OK)
	return res;
      obj_size += len_format; 
    }
    else if(len_format == 4)
    {
      uint32_t obj_length = htonl(ObjLength(i));
      if((res = write_buf(&wc, &obj_length, sizeof(obj_length))) != TERMS_OK)
	return res;
      obj_size += len_format; 
    }
    else
    {
      uint32_t obj_length = ObjLength(i);
      if((res = write_buf(&wc, &obj_length, sizeof(uint32_t))) != TERMS_OK)
	return res;
      obj_size += sizeof(uint32_t); 
    }
    
    if((res = write_buf(&wc, HeaderAt(i), m_obj_header_size)) != TERMS_OK)
      return res;
    obj_size += m_obj_header_size; 

    if((res = write_buf(&wc, BodyAt(i), ObjLength(i)*m_obj_element_size)) != TERMS_OK)
      return res;
    obj_size += ObjLength(i)*m_obj_element_size; 

    size_t align_size = 0;
    if(alignement && (ofs+obj_size)%alignement)
      align_size = alignement - (ofs+obj_size)%alignement;
    ofs += (obj_size + align_size);

    if(align_size)
    {
      if((res = write_buf(&wc, align_buf, align_size)) != TERMS_OK)
	return res;
    }
  }

  if(wc.buf && wc.buf_used)
  {
    int wrote = write_func(fsac, wc.buf, wc.buf_used);
    if(wrote != (int)(wc.buf_used))
      return TERMS_ERROR_FS_FAILED;
  }

  return TERMS_OK;
}
//----------------------------------------------------------------------//
long FltTermsRefArray::CalculateOffsets(long base_ofs, size_t alignement,
				size_t len_format, bool terms_transport)
{
  long ofs = base_ofs;
  if(!len_format)
    len_format = sizeof(TERM_LEN);

  for(size_t i = 0; i < m_objs.size(); i++)
  {
    if(terms_transport)
    {
      const TERM* term = (const TERM*)At(i);
      if(term_is_extra(term_header(term)))
      {
        m_objs[i].ofs = -1;
	continue;
      }
    }

    if(m_objs[i].ofs && m_objs[i].ofs != ofs)
      return TERMS_ERROR_EFAULT;

    m_objs[i].ofs = ofs;

    size_t obj_size = len_format+m_obj_header_size+ObjLength(i)*m_obj_element_size;
    ofs += obj_size;
    if(alignement && ofs%alignement)
      ofs += alignement - ofs%alignement;
  }

  if(ofs%32)
    ofs += 32 - ofs%32;

  return ofs;
}
//======================================================================//
