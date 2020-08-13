#include "FltTermsBase.h"
//----------------------------------------------------------------------//
#ifdef _MSC_VER
#pragma warning(disable:4514) // unreferenced inline/local function has been removed
#endif
//============================================================================//
// C-style API
//============================================================================//
static inline FltTermsBase* tb(HTERMSBASE* phtb)
{
  return *((FltTermsBase**)(phtb));
}
//----------------------------------------------------------------------------//
static inline FltTermsBase* tb(HTERMSBASE htb)
{
  return (FltTermsBase*)htb;
}
//----------------------------------------------------------------------------//
static enum FltTermsErrors create_tb(HTERMSBASE* phtb)
{
  if(!phtb)
    return TERMS_ERROR_EINVAL;
  if(*phtb)
    return TERMS_ERROR_INVALID_MODE;

  FltTermsBase* ptb = new FltTermsBase;
  if(!ptb)
    return TERMS_ERROR_ENOMEM;

  *phtb = (HTERMSBASE)ptb;

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
static void destroy_tb(HTERMSBASE* phtb)
{
  if(phtb && *phtb)
  {
    FltTermsBase* ptb = tb(phtb);
    delete ptb;
    *phtb = NULL;
  }
}
//============================================================================//
enum FltTermsErrors cf_terms_base_create(HTERMSBASE* phtb, LingProc* lp)
{
  enum FltTermsErrors ret;

  if(!lp)
    return TERMS_ERROR_EINVAL;
  if((ret = create_tb(phtb)) != TERMS_OK)
    return ret;
  if((ret = tb(phtb)->Create(lp))!= TERMS_OK)
    destroy_tb(phtb);
  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_base_create_f(HTERMSBASE* phtb, LingProc* lp, uint32_t flags)
{
  enum FltTermsErrors ret;
  
  uint32_t baseFlags = TERMS_BASE_FLAGS_DEFAULT;
  
  if ( flags & TERM_BASE_FLAGS_WITH_TEXT )
          baseFlags |= TERMS_BASE_FLAGS_WITH_TEXT;
  
  if(!lp)
    return TERMS_ERROR_EINVAL;
  if((ret = create_tb(phtb)) != TERMS_OK)
    return ret;
  if((ret = tb(phtb)->Create(lp, baseFlags))!= TERMS_OK)
    destroy_tb(phtb);
  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_base_create_dfn(HTERMSBASE* phtb, LingProc* lp,
                                                      const char* defaults_path)
{
  enum FltTermsErrors ret;

  if(!lp || !defaults_path)
    return TERMS_ERROR_EINVAL;
  if((ret = create_tb(phtb)) != TERMS_OK)
    return ret;
  if((ret = tb(phtb)->Create(lp, defaults_path)) != TERMS_OK)
    destroy_tb(phtb);
  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_base_create_dfs(HTERMSBASE* phtb, LingProc* lp,
                                                      fstorage* defaults)
{
  enum FltTermsErrors ret;

  if(!lp || !defaults)
    return TERMS_ERROR_EINVAL;
  if((ret = create_tb(phtb)) != TERMS_OK)
    return ret;
  if((ret = tb(phtb)->Create(lp, defaults)) != TERMS_OK)
    destroy_tb(phtb);
  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_base_open(HTERMSBASE* phtb, LingProc* lp,
                                 const char* path, enum FltTermsBaseOpenModes mode)
{
  enum FltTermsErrors ret;

  if(!lp || !path || !mode)
    return TERMS_ERROR_EINVAL;
  if((ret = create_tb(phtb)) != TERMS_OK)
    return ret;
  if((ret = tb(phtb)->Open(lp, path, mode)) != TERMS_OK)
    destroy_tb(phtb);
  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_base_open_fs(HTERMSBASE* phtb, LingProc* lp,
                                     fstorage* fs, enum FltTermsBaseOpenModes mode)
{
  enum FltTermsErrors ret;

  if(!lp || !fs || !mode)
    return TERMS_ERROR_EINVAL;
  if((ret = create_tb(phtb)) != TERMS_OK)
    return ret;
  if((ret = tb(phtb)->Open(lp, fs, mode)) != TERMS_OK)
    destroy_tb(phtb);
  return ret;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_base_save_fn(HTERMSBASE* phtb,
                             struct cf_terms_save_params* params, const char* path)
{
  if(!params || !path)
    return TERMS_ERROR_EINVAL;
  if(!phtb || !*phtb)
    return TERMS_ERROR_EINVAL;

  return tb(phtb)->Save(params, path);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_base_save_fs(HTERMSBASE* phtb,
                                 struct cf_terms_save_params* params, fstorage* fs)
{
  if(!params || !fs)
    return TERMS_ERROR_EINVAL;
  if(!phtb || !*phtb)
    return TERMS_ERROR_EINVAL;

  return tb(phtb)->Save(params, fs);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_base_save(HTERMSBASE* phtb,
                                               struct cf_terms_save_params* params)
{
  if(!params)
    return TERMS_ERROR_EINVAL;
  if(!phtb || !*phtb)
    return TERMS_ERROR_EINVAL;

  return tb(phtb)->Save(params);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_base_close(HTERMSBASE* phtb)
{
  if(!phtb || !*phtb)
    return TERMS_ERROR_EINVAL;

  enum FltTermsErrors ret = tb(phtb)->Close();

  if(ret == TERMS_WARN_CALLBACK_SET)
    ret = tb(phtb)->SetAutoDestroy();
  else
    destroy_tb(phtb);

  *phtb = NULL;
  return ret;
}
//============================================================================//
// terms enumeration
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_enum(HTERMSBASE htb,
                      cf_terms_enum_callback callback, void *data, bool for_export)
{
  if(!htb || !callback)
    return TERMS_ERROR_EINVAL;

  return tb(htb)->Enum(callback, data, for_export);
}

//============================================================================//
// add new term
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_add(HTERMSBASE htb, const cf_term_attrs* attrs, const char* text)
{
  if(!htb || !text)
    return TERMS_ERROR_EINVAL;

  return tb(htb)->Add(attrs, text);
}
//============================================================================//
// add stop words
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_add_stop_words(HTERMSBASE htb, const char **stop_words, size_t stop_words_size)
{
  if(!htb || !stop_words)
    return TERMS_ERROR_EINVAL;

  return tb(htb)->SetStopWords(stop_words, stop_words_size);
}
//============================================================================//
// remove stop words
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_clr_stop_words(HTERMSBASE htb, const char **stop_words, size_t stop_words_size)
{
  if(!htb || !stop_words)
    return TERMS_ERROR_EINVAL;

  return tb(htb)->UnsetStopWords(stop_words, stop_words_size);
}
//============================================================================//
// remove all stop words
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_clr_all_words(HTERMSBASE htb)
{
  if(!htb)
    return TERMS_ERROR_EINVAL;

  return tb(htb)->ClearStopWords();
}
//============================================================================//
// terms check
//----------------------------------------------------------------------------//
// returns non-negative amount of terms_check_result's
// or negative value (enum TermsErrors) on errors
// check for enough space for results; res or res_size can be zero for space checking
int cf_terms_check_res(HTERMSBASE htb, struct cf_terms_check_result* res, size_t res_size,
                       const DocImage* di, const CDocMessage::CoordMapping* mapping)
{
  if(!htb || !di)
    return TERMS_ERROR_EINVAL;

  enum FltTermsErrors ret = tb(htb)->Check(di, mapping);
  if(ret)
    return ret;

  return (int)tb(htb)->GetLastResults(res, res_size);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_check(HTERMSBASE htb, const DocImage* di, const CDocMessage::CoordMapping* mapping)
{
  if(!htb || !di)
    return TERMS_ERROR_EINVAL;

  return tb(htb)->Check(di, mapping);
}
//----------------------------------------------------------------------------//
size_t cf_terms_last_res_size(HTERMSBASE htb)
{
  if(!htb)
    return 0;

  return tb(htb)->GetLastResultsSize();
}
//----------------------------------------------------------------------------//
int cf_terms_last_res(HTERMSBASE htb,
                                struct cf_terms_check_result* res, size_t res_size)
{
  if(!htb)
    return TERMS_ERROR_EINVAL;

  return (int)tb(htb)->GetLastResults(res, res_size);
}
//----------------------------------------------------------------------------//
size_t cf_terms_last_found_terms_size(HTERMSBASE htb)
{
  if(!htb)
    return 0;

  return tb(htb)->GetLastFoundTermsSize();
}
//----------------------------------------------------------------------------//
int cf_terms_last_found_terms(HTERMSBASE htb, struct cf_terms_check_info* info,
                 struct cf_terms_check_found_term* terms, size_t max_size)
{
  return cf_terms_last_found_terms_cp(htb, info, terms, max_size);
}
//----------------------------------------------------------------------------//
int cf_terms_last_found_terms_cp(HTERMSBASE htb, struct cf_terms_check_info* info,
                        struct cf_terms_check_found_term* terms, size_t max_size)
{
  if(!htb)
    return TERMS_ERROR_EINVAL;

  return tb(htb)->GetLastFoundTerms(info, terms, max_size);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors cf_terms_check_ex(HTERMSBASE htb,
                                  FltTermsMatcher* matcher, const DocImage* di, const CDocMessage::CoordMapping* mapping)
{
  if(!htb || !di)
    return TERMS_ERROR_EINVAL;

  return tb(htb)->Check(matcher, di, mapping);
}
//============================================================================//
