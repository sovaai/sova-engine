#include "FltTermsBase.h"
#include <fcntl.h>
//----------------------------------------------------------------------------//
#ifdef _MSC_VER
#pragma warning(disable:4514) // unreferenced inline/local function has been removed
#endif
//============================================================================//
// Terms base header
//----------------------------------------------------------------------------//
void FltTermsBaseHeader::Init()
{
  Clear();

  data.magic = TERMS_BASE_MAGIC;

  data.work_format_version = TERMS_BASE_WORK_FORMAT_VERSION;
  data.work_format_variant = TERMS_BASE_WORK_FORMAT_VARIANT;
  data.transport_format_version = TERMS_BASE_TRANSPORT_FORMAT_VERSION;
  data.transport_format_variant = TERMS_BASE_TRANSPORT_FORMAT_VARIANT;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseHeader::Deserialize(fstorage* fs, fstorage_section_id sid)
{
  // find section
  fstorage_section* sec = fstorage_find_section(fs, sid);
  if(!sec)
    return TERMS_ERROR_FS_FAILED;

  // load section
  if(fstorage_section_load(sec) != FSTORAGE_OK)
    return TERMS_ERROR_FS_FAILED;

  // checking section size
  if(fstorage_section_get_size(sec) < TERMS_BASE_HEADER_SIZE)
    return TERMS_ERROR_FS_FAILED;

  // getting pointer
  uint32_t* pt = (uint32_t *)fstorage_section_get_all_data(sec);
  if(!pt)
    return TERMS_ERROR_FS_FAILED;

  // getting data
  Clear();

  data.magic = ntohl(*pt++);
  data.work_format_version = ntohl(*pt++);
  data.work_format_variant = ntohl(*pt++);
  data.transport_format_version = ntohl(*pt++);
  data.transport_format_variant = ntohl(*pt++);
  data.is_transport = (ntohl(*pt++) != 0);
  data.export_denied = (ntohl(*pt++) != 0);
  data.alignement = ntohl(*pt++);

  for(int i = 0; i < TERMS_BASE_ROOTS_NUM; i++)
    data.roots_ofs[i] = ntohl(*pt++);

  data.indexes_used = ntohl(*pt++);
  data.sets_used = ntohl(*pt++);
  data.terms_used = ntohl(*pt++);
  data.terms_real_count = ntohl(*pt++);
  
  data.base_flags = TERMS_BASE_FLAGS_DEFAULT;
  
  if (data.work_format_variant > 1)
  {
      data.terms_extra_count = ntohl(*pt++);
      data.terms_hier_count = ntohl(*pt++);
      data.terms_indexed_count = ntohl(*pt++);

      if (data.work_format_variant > 2)
          data.base_flags = ntohl(*pt++);
  }
  else
  {
      data.terms_extra_count = 0;
      data.terms_hier_count = 0;
      data.terms_indexed_count = 0;
  }

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseHeader::Serialize(fstorage* fs, fstorage_section_id sid) const
{
  // find or create section
  fstorage_section* sec = fstorage_find_section(fs, sid);
  if(!sec)
    sec = fstorage_section_create(fs, sid);
  if(!sec)
    return TERMS_ERROR_FS_FAILED;

  // allocating memory
  if(fstorage_section_get_size(sec) != TERMS_BASE_HEADER_SIZE)
  {
    if(fstorage_section_realloc(sec, TERMS_BASE_HEADER_SIZE) != FSTORAGE_OK)
      return TERMS_ERROR_FS_FAILED;
  }

  // getting pointer
  uint32_t* pt = (uint32_t *)fstorage_section_get_all_data(sec);
  if(!pt)
    return TERMS_ERROR_FS_FAILED;

  // storing data
  memset(pt, 0, TERMS_BASE_HEADER_SIZE);

  *pt++ = htonl(data.magic);
  *pt++ = htonl(data.work_format_version);
  *pt++ = htonl(data.work_format_variant);
  *pt++ = htonl(data.transport_format_version);
  *pt++ = htonl(data.transport_format_variant);
  *pt++ = htonl(data.is_transport);
  *pt++ = htonl(data.export_denied);
  *pt++ = htonl((uint32_t)data.alignement);

  for(int i = 0; i < TERMS_BASE_ROOTS_NUM; i++)
    *pt++ = htonl(data.roots_ofs[i]);

  *pt++ = htonl((uint32_t)data.indexes_used);
  *pt++ = htonl((uint32_t)data.sets_used);
  *pt++ = htonl((uint32_t)data.terms_used);
  *pt++ = htonl((uint32_t)data.terms_real_count);
  *pt++ = htonl((uint32_t)data.terms_extra_count);
  *pt++ = htonl((uint32_t)data.terms_hier_count);
  *pt++ = htonl((uint32_t)data.terms_indexed_count);
  *pt++ = htonl((uint32_t)data.base_flags);

  return TERMS_OK;
}
//============================================================================//
// Terms base - common
//----------------------------------------------------------------------------//
FltTermsBase::~FltTermsBase()
{
  Reset();
}
//----------------------------------------------------------------------------//
void FltTermsBase::Reset()
{
  if(fs_own && m_fs)
    fstorage_destroy(m_fs);

  m_fs = NULL;
  fs_own = false;

  auto_destroy = false;
  fs_callback_close = false;

  wr_base.Reset();
  ro_base.Reset();
  m_matcher.Free();

  m_status = STATUS_NOT_READY;
}
//============================================================================//
// Open Storage
//----------------------------------------------------------------------------//
static enum FltTermsErrors open_fs(fstorage** new_fs, const char* path,
                                                    enum FltTermsBaseOpenModes mode)
{
  *new_fs = NULL;

  // check params
  if(!path || !*path)
    return TERMS_ERROR_EINVAL;

  // create fs
  fstorage *fs = fstorage_create();
  if(!fs)
    return TERMS_ERROR_ENOMEM;

  // connect fs
  struct fstorage_connect_config fscc;
  memset(&fscc, 0, sizeof(fscc));
  fscc.pid = FSTORAGE_PID_CFLIB;

  enum FltTermsErrors ret = TERMS_OK;
  switch(mode)
  {
  case TERM_BASE_OPEN_RO:
    if(fstorage_connect(fs, path, O_RDONLY, 0644, FSTORAGE_OPEN_READ_ONLY, &fscc))
      ret = TERMS_ERROR_FS_FAILED;
    break;
  case TERM_BASE_OPEN_RO_SHARED:
    if(fstorage_connect_shared(fs, path, &fscc))
      ret = TERMS_ERROR_FS_FAILED; 
    break;
  case TERM_BASE_OPEN_CREATE:
  case TERM_BASE_OPEN_RW:
    if(fstorage_connect(fs, path, O_RDWR | O_CREAT , 0644, FSTORAGE_OPEN_READ_WRITE, &fscc))
      ret = TERMS_ERROR_FS_FAILED;
    break;
  default:
    ret = TERMS_ERROR_ENOTIMPL;
    break;
  };

  // destroy fs on errors
  if(ret != TERMS_OK)
    fstorage_destroy(fs);
  else
    *new_fs = fs;

  return ret;
}

//============================================================================//
// Create + Save
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Create(LingProc* lp, uint32_t flags)
{
    enum FltTermsErrors result = Create(lp);
    if ( result != TERMS_OK )
        return result;

    /* Save the flags */
    wr_base.base_flags = flags;
    
    return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Create(LingProc* lp, fstorage* defaults /* = NULL */)
{
  if(!wr_base.save_callback_count)
    Reset();
  else
    return TERMS_ERROR_INVALID_MODE;

  enum FltTermsErrors ret;
  if((ret = wr_base.Init(this, lp)) != TERMS_OK)
    m_status = STATUS_ERROR;
  else
    m_status = STATUS_WR;

  if(defaults)
  {
    return TERMS_ERROR_ENOTIMPL;
  }

  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Create(LingProc* lp, const char* defaults_path)
{
  enum FltTermsErrors ret;
  if(defaults_path && *defaults_path)
  {
    fstorage* fs;
    if((ret = open_fs(&fs, defaults_path, TERM_BASE_OPEN_RO)) != TERMS_OK)
      return ret;

    ret = Create(lp, fs);

    if(fstorage_close(fs))
      ret = ret ? ret : TERMS_ERROR_FS_FAILED;
    if(fstorage_destroy(fs))
      ret = ret ? ret : TERMS_ERROR_FS_FAILED;
    
    return ret;
  }
  else
    return Create(lp);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Save(struct cf_terms_save_params* params, fstorage* fs)
{
  if(m_status != STATUS_WR || m_fs)
    return TERMS_ERROR_INVALID_MODE;

  m_fs = fs;
  fs_own = false;
  return Save(params);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Save(struct cf_terms_save_params* params, const char* path)
{
  if(m_status != STATUS_WR || m_fs)
    return TERMS_ERROR_INVALID_MODE;

  enum FltTermsErrors ret;

  fstorage* fs;
  if((ret = open_fs(&fs, path, TERM_BASE_OPEN_CREATE)) != TERMS_OK)
    return ret;

  m_fs = fs;
  fs_own = true;
  return Save(params);
}
//============================================================================//
// Open + Save
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Open(LingProc* lp, fstorage* fs, enum FltTermsBaseOpenModes mode)
{
  if(!wr_base.save_callback_count)
    Reset();
  else
    return TERMS_ERROR_INVALID_MODE;

  enum FltTermsErrors ret;
  if(mode == TERM_BASE_OPEN_RO || mode == TERM_BASE_OPEN_RO_SHARED)
  {
    if((ret = ro_base.Init(lp, fs, base_sid)) != TERMS_OK)
      m_status = STATUS_ERROR;
    else
      m_status = STATUS_RO;

  }
  else if(mode == TERM_BASE_OPEN_CREATE)
    ret = Create(lp);
  else
    return TERMS_ERROR_ENOTIMPL;

  m_fs = fs;

  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Open(LingProc* lp, const char* path, enum FltTermsBaseOpenModes mode)
{
  enum FltTermsErrors ret = TERMS_OK;
  
  // open storage
  fstorage* fs;
  if((ret = open_fs(&fs, path, mode)) != TERMS_OK)
    return ret;
  
  // open terms base
  ret = Open(lp, fs, mode);

  // set fs_own
  if(ret == TERMS_OK)
    fs_own = true;
  else
    { fstorage_destroy(fs); m_fs = NULL; }
  
  return ret;
}
//----------------------------------------------------------------------------//
int on_destroy(struct fstorage_stream_section_config *sec_config)
{
  FltTermsBase* base = (FltTermsBase*)(sec_config->context);
    
  // destroy
  delete base;

  return FSTORAGE_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Save(struct cf_terms_save_params* params)
{
  if(m_status != STATUS_WR || !m_fs)
    return TERMS_ERROR_INVALID_MODE;

  enum FltTermsErrors ret;

  if((ret = wr_base.PrepareSave(m_fs, base_sid, params)) != TERMS_OK)
    m_status = STATUS_ERROR;

  if(fs_own)
  {
    if(fstorage_close(m_fs))
      ret = ret ? ret : TERMS_ERROR_FS_FAILED;
    if(fstorage_destroy(m_fs))
      ret = ret ? ret : TERMS_ERROR_FS_FAILED;

    m_fs = NULL; // to avoid fstorage_destroy while Close()
    fs_own = false;
  }

  return ret;
}
//============================================================================//
// Close
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Close()
{
  if(!wr_base.save_callback_count)
  {
    Reset();
    return TERMS_OK;
  }
  else
  {
    fs_callback_close = true;
    return TERMS_WARN_CALLBACK_SET;
  }
}
//============================================================================//
// Auto Destroy
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::SetAutoDestroy()
{
  if(!fs_callback_close || fs_own || !m_fs)
    return TERMS_ERROR_INVALID_MODE;

  fstorage_section* sec = fstorage_find_section(m_fs, base_sid + TERMS_SECTION_OFS_HEADER);
  if(!sec)
    return TERMS_ERROR_FS_FAILED;
  else
  {
    struct fstorage_stream_section_config stream_config;
    memset(&stream_config, 0, sizeof(stream_config));
    stream_config.context = (void*)this;
    stream_config.on_destroy = on_destroy;
    fstorage_section_stream(sec, &stream_config);
  }

  return TERMS_OK;
}  
//============================================================================//
// WR Funcs
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Add(const cf_term_attrs* attrs, const char* text)
{
  if(m_status != STATUS_WR)
    return TERMS_ERROR_INVALID_MODE;

  enum FltTermsErrors ret;
  if((ret = wr_base.Add(attrs, text)) < 0) // may return TERMS_WARN_...
  {
    if ( (ret == TERMS_ERROR_ERANGE) && (wr_base.base_flags & TERMS_BASE_FLAGS_MAP_ERANGE_WARNING) )
      ret = TERMS_WARN_ERANGE;
    else if ( !(wr_base.base_flags & TERMS_BASE_FLAGS_ALLOW_COMPILE_ERRORS) )
      m_status = STATUS_ERROR;
  }

  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::SetStopWords( const char **words, size_t wordsSize )
{
  if(m_status != STATUS_WR)
    return TERMS_ERROR_INVALID_MODE;

  return mapStopStatus( wr_base.m_table_stop.addWords( words, wordsSize, *wr_base.m_lp, StopDictEx::FILL_MODE_ADD ) );
}
enum FltTermsErrors FltTermsBase::UnsetStopWords( const char **words, size_t wordsSize )
{
  if(m_status != STATUS_WR)
    return TERMS_ERROR_INVALID_MODE;

  return mapStopStatus( wr_base.m_table_stop.addWords( words, wordsSize, *wr_base.m_lp, StopDictEx::FILL_MODE_CLR ) );
}
enum FltTermsErrors FltTermsBase::ClearStopWords()
{
  if(m_status != STATUS_WR)
    return TERMS_ERROR_INVALID_MODE;

  return mapStopStatus( wr_base.m_table_stop.clear() );
}
//============================================================================//
// RO Funcs
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Enum(cf_terms_enum_callback callback, void* data, bool for_export)
{
  if(m_status != STATUS_RO)
    return TERMS_ERROR_INVALID_MODE;

  enum FltTermsErrors ret;
  if((ret = ro_base.Enum(callback, data, for_export, false)) != TERMS_OK)
    m_status = STATUS_ERROR;

  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Check(const DocImage* di, const CDocMessage::CoordMapping* mapping /*= NULL*/)
{
  return ro_base.Check(&m_matcher, di, mapping);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBase::Check(FltTermsMatcher* matcher, const DocImage* di, const CDocMessage::CoordMapping* mapping /*= NULL*/)
{
  return ro_base.Check(matcher, di, mapping);
}
//----------------------------------------------------------------------------//
bool FltTermsBase::CheckIfAllStopWords(const DocImage* di)
{
  return ro_base.CheckIfAllStopWords(di);
}
//============================================================================//
static inline int lex_cmp(LEXID cur, LEXID key)
{
  return cur == key ? 0 : (cur < key ? -1 : +1);
}
//----------------------------------------------------------------------------//
bool tidx_lookup(const TERMS_INDEX* index, LEXID key,
		      size_t &found, const TERMS_INDEX_ELEMENT* &found_element)
{
  const TERMS_INDEX_ELEMENT* array = tidx_array(index);

  size_t low = 0;
  size_t up  = tidx_length(index);

  while (low < up)
  {
    size_t cur = (low + up) / 2;
    int r = lex_cmp(tidx_lex(tidx_element(array, cur)), key);

    if(r == 0)
      { found = cur;  found_element = tidx_element(array, found); return true; }
    else if(r < 0)
      { low = cur + 1; }
    else if(r > 0)
      { up = cur; }
  }

  found = low;
  found_element = NULL;
  return false;
}
//============================================================================//
void term_header_hton(TERM_HDR* thdr)
{
  uint32_t* t_idn = (uint32_t *)thdr;
  *t_idn = htonl(*t_idn);

  cf_term_attrs* t_attrs = term_attrs(thdr);
  terms_attr_hton(t_attrs, t_attrs);
}
//----------------------------------------------------------------------------//
void term_header_ntoh(TERM_HDR* thdr)
{
  uint32_t* t_idn = (uint32_t *)thdr;
  *t_idn = ntohl(*t_idn);

  cf_term_attrs* t_attrs = term_attrs(thdr);
  terms_attr_ntoh(t_attrs, t_attrs);
}
//============================================================================//
