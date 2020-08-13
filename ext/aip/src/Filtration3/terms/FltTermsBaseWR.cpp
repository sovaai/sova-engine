#include <stdlib.h>

#include "FltTermsBase.h"
//----------------------------------------------------------------------//
#ifdef _MSC_VER
    #pragma warning(disable:4514) // unreferenced inline/local function has been removed
    #pragma warning(disable:4512) // assignment operator could not be generated
    #include <time.h>
    long random()
    { 
        static time_t seed = 0;
        if ( seed == 0 ) {
            time(&seed);
            srand(unsigned((int64_t)seed%UINT_MAX));
        }
        return (rand()<<15)^rand();
    }
#endif
//============================================================================//
// Terms base - write mode
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::Init(class FltTermsBase* tb, LingProc* lp)
{
  if(save_callback_count)
    return TERMS_ERROR_INVALID_MODE;

  Reset();

  FltTermsErrors ret = m_table_stop.init( *lp );
  if (ret != TERMS_OK)
    return ret;
  
  m_tb = tb;

  // init lp
  if(!lp)
    return TERMS_ERROR_EINVAL;
  m_lp = lp;

  // init m_roots
  char new_ihdr_buf[TERMS_INDEX_HEADER_SIZE];
  TERMS_INDEX_HEADER* new_ihdr = (TERMS_INDEX_HEADER*)&new_ihdr_buf;
  memset(new_ihdr, 0, TERMS_INDEX_HEADER_SIZE);

  for(size_t n = 0; n < TERMS_BASE_ROOTS_NUM; n++)
  {
    ret = m_roots.AddIndex(NULL, 0, new_ihdr);
    if(ret)
      return ret;
  }

  m_status = STATUS_READY;

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
void FltTermsBaseWR::Reset()
{
  m_roots.Reset();
  m_index.Reset();
  m_sets.Reset();
  m_terms.Reset();

  m_terms_real_count = 0;
  m_terms_extra_count = 0;
  m_terms_hier_count = 0;
  m_terms_indexed_count = 0;

  m_tb = NULL;
  m_lp = NULL;

  if(m_write_buf)
    free(m_write_buf);
  m_write_buf = NULL;
  m_write_buf_size = 0;
  m_write_alignement = 0;

  save_callback_count = 0;
  m_status = STATUS_NOT_READY;
}
//============================================================================//
// Add new term
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::Add(const cf_term_attrs* attrs, const char* text)
{
    // check params
    if(!attrs || !text)
        return TERMS_ERROR_EINVAL;

    // check mode
    if(m_status != STATUS_READY)
        return TERMS_ERROR_INVALID_MODE;

    // getting free id
    unsigned int new_id = (unsigned int)(m_terms_real_count + 1);
    if(new_id & ~THDR_IDN_MASK)
        return TERMS_ERROR_ERANGE;
        
    // prepare header
    char* thdr_buf[TERM_HDR_SIZE];
    TERM_HDR* thdr = (TERM_HDR*)&thdr_buf;
    memset(thdr, 0, TERM_HDR_SIZE);

    set_term_id(thdr, new_id);
    memcpy(term_attrs(thdr), attrs, term_attrs_size());

    // lemmatize term
    FltTermsErrors status = LemmatizeTerm(thdr, text);
    if(status == TERMS_ERROR_LP_MORPH || status == TERMS_ERROR_LP_UNSUPPORTED_MORPH)
        status = TERMS_WARN_LP_MORPH;

    if ( status < 0 )
        return status;
    
    /* Store the text of the term if need to */
    if ( base_flags & TERMS_BASE_FLAGS_WITH_TEXT && m_terms_real_count == new_id )
    {
        size_t text_len = strlen(text);

        /* Allocate space for utf-8 string */
        texts_data.resize( texts_last_idx + text_len + 1 );
        if ( texts_data.no_memory() )
            return TERMS_ERROR_ENOMEM;

        memcpy( reinterpret_cast<char *>(texts_data.get_buffer() + texts_last_idx),
                text,
                text_len + 1 );
        
        /* Create and add an index record */
        uint32_t idx = static_cast<uint32_t>(texts_last_idx);
        
        texts_index.push_back(idx);
        if ( texts_index.no_memory() )
            return TERMS_ERROR_ENOMEM;

        texts_last_idx += strlen( reinterpret_cast<char *>(texts_data.get_buffer() + texts_last_idx) );
    }

    return status;
}
//----------------------------------------------------------------------------//
static int filler( const FltTermsBaseWR::FltTermsBaseLinkedLexid *cursor, int depth, LEXID *buffer )
{
    if ( depth >= TERM_MAX_LENGTH )
        return -1;
    if ( cursor == 0 )
        return depth;
    int size = filler( cursor->prev, depth + 1, buffer );
    if ( size < 0 )
        return size;
    buffer[size - depth - 1] = cursor->lex;
    return size;
}

enum FltTermsErrors FltTermsBaseWR::Add(const FltTermsBaseLinkedLexid *lexList, 
                                        const TERM_HDR* thdr, enum FltTermsErrors& warn,
                                        bool force_index_only)
{
    // check term_id
    bool new_term;
    if(term_id(thdr) == m_terms_real_count + 1) // new term
        new_term = true;
    else if(term_id(thdr) == m_terms_real_count) // next variant of the same term
        new_term = false;
    else
        return TERMS_ERROR_EFAULT;

    // check force_index_only
    if(new_term && force_index_only) // new term must be added to m_terms in any case
        return TERMS_ERROR_EFAULT;

    // check extra
    if(new_term && term_is_extra(thdr))
        return TERMS_ERROR_EFAULT;

    #ifdef USE_INF_TERMS
    if(term_is_extra(thdr))
        return TERMS_OK;
    #endif

    // check hier
    if(term_is_hier(thdr) && !new_term)
        return TERMS_ERROR_EFAULT;
    if(term_is_hier(thdr) && term_is_extra(thdr))
        return TERMS_ERROR_EFAULT;

    size_t lex_size;
    LEXID  lex[TERM_MAX_LENGTH];

    /* Recursively fill lex buffer */
    int fillResult = filler(lexList, 0, lex);
    if (fillResult < 0)
        return TERMS_ERROR_ERANGE;
    lex_size = static_cast<size_t>(fillResult);
        
    enum FltTermsErrors ret;
    
    LEXID term_body_ref = (LEXID)(-1);
    size_t term_ref = force_index_only ? m_terms.Size() - 1 : m_terms.Size();
    if((ret = AddToIndex(lex, lex_size, thdr, term_ref, term_body_ref, warn)) != TERMS_OK)
        return ret;

    m_terms_indexed_count++;  // term is added to index
    
    if(force_index_only)
        return TERMS_OK;
    
    #ifdef USE_TERMS_REF_BODY  
    if(term_body_ref != (LEXID)(-1))
    {
        char* thdr_new_buf[TERM_HDR_SIZE];
        TERM_HDR* thdr_new = (TERM_HDR*)&thdr_new_buf;
        memcpy(thdr_new, thdr, TERM_HDR_SIZE);

        set_term_refbody(thdr_new);
        
        if((ret = m_terms.AddTerm( &term_body_ref, 1, thdr_new )) != TERMS_OK)
            return ret;
    }
    else
    #endif
        if((ret = m_terms.AddTerm(lex, lex_size, thdr)) != TERMS_OK)
            return ret;
    
    if(new_term)
        m_terms_real_count++; // new term is added to term list
    if(term_is_extra(thdr))
        m_terms_extra_count++;
    if(term_is_hier(thdr))
        m_terms_hier_count++;

    return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::AddToIndex(const LEXID* lex, size_t lex_size,
		const TERM_HDR* thdr, size_t term_ref, LEXID& body_term_ref,
                enum FltTermsErrors& warn)
{
  warn = TERMS_OK;
  body_term_ref = (LEXID)(-1);

  // cannot add empty term
  if(!lex_size)
    return TERMS_ERROR_EMPTY;
  // now we can be sure that lex_size >= 1

  enum FltTermsErrors ret;

  // getting root index
  FltTermsRefArrayIndexes* cur_ind_array = &m_roots;
  size_t cur_index_no = LexNoMrph(lex[0]);
  TERMS_INDEX* cur_index = (TERMS_INDEX*)m_roots.At(cur_index_no);
  if(!cur_index)
    return TERMS_ERROR_EFAULT;
  
  // lookup lex
  size_t found_el_no = 0;
  const TERMS_INDEX_ELEMENT* found_el = NULL;

  bool found = false;
  size_t cur_lex = 0;
  while(cur_lex < lex_size)
  {
    LEXID key = LexNormalize(lex[cur_lex]);
    found = tidx_lookup(cur_index, key, found_el_no, found_el);
    if(!found)
      break;
    else if(tidx_is_final_ref(tidx_ref(found_el)))
      break;
    else if(tidx_is_index_ref(tidx_ref(found_el)))
    {
      cur_index_no = tidx_ref_value(tidx_ref(found_el));
      cur_index = m_index.At(cur_index_no);
      if(!cur_index)
        return TERMS_ERROR_EFAULT;
    }
    else
      return TERMS_ERROR_EFAULT;
    cur_lex++;
    cur_ind_array = &m_index;
  }

  /* inserting */
  if(cur_lex == lex_size)
  {
    /* longer term is already in index */
    // inserting term ref into index header
    TERMS_INDEX_REF hdr_ref = tidx_ref(tidx_header(cur_index));
    if(CheckRepeatedTerm(lex, lex_size, thdr, hdr_ref, warn))
      return warn;
    if((ret = AddNewTermRef(&hdr_ref, (unsigned int)term_ref)) != TERMS_OK)
      return ret;
    set_tidx_ref(tidx_header(cur_index), hdr_ref);
    if(tidx_is_set_ref(hdr_ref))
      body_term_ref = GetTermRefBody(lex, lex_size, thdr, m_sets.At(tidx_ref_value(hdr_ref)));

  }
  else if(!found || (found && tidx_is_final_ref(tidx_ref(found_el)) && cur_lex < lex_size - 1))
  {
    /* cur_lex is not found in index or shorter term is already in index */
    // inserting index chain

    char new_ihdr_buf[TERMS_INDEX_HEADER_SIZE];
    TERMS_INDEX_HEADER* new_ihdr = (TERMS_INDEX_HEADER*)&new_ihdr_buf;
    memset(new_ihdr, 0, TERMS_INDEX_HEADER_SIZE);

    char new_el_buf[TERMS_INDEX_ELEMENT_SIZE];
    TERMS_INDEX_ELEMENT* new_el = (TERMS_INDEX_ELEMENT*)&new_el_buf;
    memset(new_el, 0, TERMS_INDEX_ELEMENT_SIZE);

    bool first = true;
    while(cur_lex < lex_size - 1)
    {
      if(first && found)
        set_tidx_ref(new_ihdr, tidx_ref(found_el));

      if((ret = m_index.AddIndex(NULL, 0, new_ihdr)) != 0)
	return ret;

      if(first && found)
      {
        set_tidx_ref(new_ihdr, tidx_ref_empty());
	set_tidx_ref(const_cast <TERMS_INDEX_ELEMENT*>(found_el), tidx_ref_index((unsigned int)m_index.Last()));
      }
      else
      {
        set_tidx_lex(new_el, LexNormalize(lex[cur_lex]));
	set_tidx_ref(new_el, tidx_ref_index((unsigned int)m_index.Last()));
	if((ret = cur_ind_array->InsertElement(cur_index_no, new_el, found_el_no)) != 0)
	  return ret;
      }
      first = false;
      
      cur_index_no = m_index.Last();
      found_el_no = 0;
      cur_lex++;
      cur_ind_array = &m_index;
    }
    // inserting ref to term
    set_tidx_lex(new_el, LexNormalize(lex[cur_lex]));
    set_tidx_ref(new_el, tidx_ref_term((unsigned int)term_ref));
    if((ret = cur_ind_array->InsertElement(cur_index_no, new_el, found_el_no)) != 0)
      return ret;
  }
  else if(tidx_is_final_ref(tidx_ref(found_el)) && cur_lex == lex_size - 1) 
  {
    /* term of the same length is already in index */
    TERMS_INDEX_REF found_ref = tidx_ref(found_el);
    if(CheckRepeatedTerm(lex, lex_size, thdr, found_ref, warn))
      return warn;
    if((ret = AddNewTermRef(&found_ref, (unsigned int)term_ref)) != TERMS_OK)
      return ret;
    set_tidx_ref(const_cast <TERMS_INDEX_ELEMENT*>(found_el), found_ref);
    if(tidx_is_set_ref(found_ref))
      body_term_ref = GetTermRefBody(lex, lex_size, thdr, m_sets.At(tidx_ref_value(found_ref)));
  }
  else
    return TERMS_ERROR_EFAULT;

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::AddNewTermRef(TERMS_INDEX_REF* ref,
                                                            unsigned int term_ref)
{
  if(tidx_is_ref(term_ref))
    return TERMS_ERROR_EINVAL;

  enum FltTermsErrors ret;

  if(tidx_is_empty_ref(*ref))
    *ref = tidx_ref_term(term_ref);
  else if(tidx_is_term_ref(*ref))
  {
    unsigned int old_term_ref = tidx_ref_value(*ref);
    if((ret = m_sets.AddTermSet(NULL, 0)) != TERMS_OK)
      return ret;
    if((ret = m_sets.AppendElement(m_sets.Last(), old_term_ref)) != TERMS_OK)
      return ret;
    
    if((ret = m_sets.AppendElement(m_sets.Last(), term_ref)) != TERMS_OK)
      return ret;
    *ref = tidx_ref_set((unsigned int)m_sets.Last());
  }
  else if(tidx_is_set_ref(*ref))
  {
    if((ret = m_sets.AppendElement(tidx_ref_value(*ref), term_ref)) != TERMS_OK)
      return ret;
  }
  else
    return TERMS_ERROR_EFAULT;

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
LEXID FltTermsBaseWR::GetTermRefBody(const LEXID* lex, size_t lex_size,
                               const TERM_HDR* thdr, const TERMS_SET* set) const
{
  if(!term_is_morph_any(term_attrs(thdr)))
      return (LEXID)(-1);

  #ifndef OLD_PAT_COMPATIBLE
  if(term_caps(term_attrs(thdr)) == CAPS_ANY)
    return term_set_refs(set)[0];
  #endif

  // comparing capitalization
  for(size_t n = 0; n < term_set_length(set) - 1; n++)
  {
    unsigned int ref = term_set_refs(set)[n];
    const TERM* term = GetTermWR(ref);
    if(!term)
        return (LEXID)(-1);

    if(term_is_extra(term_header(term)))
      continue;

    size_t t_len = GetTermLength(term);
    const LEXID* t_lex = GetTermLex(term);
    if(!t_lex)
        return (LEXID)(-1);

    if(t_len != lex_size) // in fact not possible
      continue;

    #ifdef OLD_PAT_COMPATIBLE
    if(term_where(term_attrs(thdr)) != term_where(term_attrs(term)))
      continue;
    if(term_caps(term_attrs(thdr)) == CAPS_ANY && term_caps(term_attrs(term)) == CAPS_ANY)
      return term_is_refbody(term_header(term)) ? *term_ptlex(term) : ref;
    #endif

    bool equal = true;
    for(size_t i = 0; i < lex_size; i++)
    {
      if(LexNormalForm(lex[i]) != LexNormalForm(t_lex[i]))
        { equal = false; break; }
    }

    if(equal)
      return term_is_refbody(term_header(term)) ? *term_ptlex(term) : ref;
  }

  return (LEXID)(-1);
}
//----------------------------------------------------------------------------//
bool FltTermsBaseWR::CheckRepeatedTerm(const LEXID* new_lex, size_t new_lex_length,
               const TERM_HDR* new_thdr, TERMS_INDEX_REF ref, enum FltTermsErrors& warn) const
{
#ifndef USE_INF_TERMS
  warn = TERMS_OK;
  enum FltTermsErrors false_warn = TERMS_OK;

  const TERM* old_term;
  if(tidx_is_term_ref(ref))
  {
    old_term = m_terms.At(tidx_ref_value(ref));
    return CheckRepeatedTerm(new_lex, new_lex_length, new_thdr, old_term, warn);
  }
  else if(tidx_is_set_ref(ref))
  {
    const TERMS_SET* set = m_sets.At(tidx_ref_value(ref));
    const TERMS_SET_REF* array = term_set_refs(set);
    for(size_t n = 0;  n < term_set_length(set); n++)
    {
      old_term = m_terms.At(array[n]);
      if(CheckRepeatedTerm(new_lex, new_lex_length, new_thdr, old_term, warn))
	return true;
      else if(warn != TERMS_OK && false_warn == TERMS_OK)
        false_warn = warn;
    }
  }

  warn = false_warn;
  return false;
#else
  warn = TERMS_OK;
  return false;
#endif
}
//----------------------------------------------------------------------------//
bool FltTermsBaseWR::CheckRepeatedTerm(const LEXID* new_lex, size_t new_lex_length,
        const TERM_HDR* new_thdr, const TERM* old_term, enum FltTermsErrors& warn) const
{
  /* RETURN VALUES */
  // true - term should be skipped
  //    warn = TERMS_WARN_EXIST - new term is duplicate
  //    warn = TERMS_WARN_EXIST_CONFLICT - new term is duplicate and has attribute conflicts
  // false - term should be added
  //    warn = TERMS_OK - new term is not duplicate
  //    warn = TERMS_WARN_CONFLICT - new term is duplicate and has attribute conflicts
  //                                 but it is "stonger" than old term

  /* COMPARING ATTRIBUTES */
  // up to this momemnt we can be sure that lexems without form and caps are equal
  // ranged search attributes: where, morph, caps
  // ranged weight attributes: strict/suppl, weight
  // special treatment: rubric_id, lang
  // to take into account: extra

  warn = TERMS_OK;

  const TERM_HDR* old_thdr = term_header(old_term);

  const cf_term_attrs* new_attrs = term_attrs(new_thdr);
  const cf_term_attrs* old_attrs = term_attrs(old_thdr);

  /* rubric id */
  if(term_rubric_id(new_attrs) != term_rubric_id(old_attrs))
    return false;

  /* attributes */
  int search_res = CompareSearchAttrs(new_lex, new_lex_length, new_thdr, old_term);
  int weight_res = CompareWeightAttrs(new_lex, new_lex_length, new_thdr, old_term);
  switch(search_res)
  {
  case 1: // search attributes are not equal
    warn = TERMS_OK;
    return false;
  case 0: // search attributes are equal
    if(weight_res == 0)
      // to do: need to exclude from warning extra terms with different languages
      { warn = TERMS_WARN_EXIST; return true; }
    else if(weight_res == -2)
      #ifdef OLD_PAT_COMPATIBLE
      { warn = TERMS_WARN_CONFLICT; return false; }
      #else
      { warn = TERMS_WARN_EXIST_CONFLICT; return true; }
      #endif
    else if(weight_res == +2)
      { warn = TERMS_WARN_CONFLICT; return false; }
    else
      { warn = TERMS_WARN_EXIST_CONFLICT; return true; }
    break;
  case -2:
    if(weight_res == 0)
      { warn = TERMS_WARN_EXIST_CONFLICT; return true; }
    else
      { warn = TERMS_OK; return false; }
    break;
  case +2:
    if(weight_res == 0)
      { warn = TERMS_WARN_CONFLICT; return false; }
    else
      { warn = TERMS_OK; return false; }
    break;
  }

  return false;
}
//----------------------------------------------------------------------------//
int FltTermsBaseWR::CompareSearchAttrs(const LEXID* new_lex, size_t new_lex_length,
                         const TERM_HDR* new_thdr, const TERM* old_term) const
{
  int pos   = ComparePos(new_lex, new_lex_length, new_thdr, old_term);
  int where = CompareWhere(new_lex, new_lex_length, new_thdr, old_term);
  int morph = CompareMorph(new_lex, new_lex_length, new_thdr, old_term);
  int caps = CompareCaps(new_lex, new_lex_length, new_thdr, old_term);

  if(where == morph && morph == caps && caps == pos)
    return where;
  else
    return 1;
}
//----------------------------------------------------------------------------//
int FltTermsBaseWR::ComparePos(const LEXID* /*new_lex*/, size_t /*new_lex_length*/,
                           const TERM_HDR* new_thdr, const TERM* old_term) const
{
  if( (term_is_position_leftmost(term_attrs(new_thdr))  &&
       term_is_position_leftmost(term_attrs(old_term)))  ||
      (term_is_position_rightmost(term_attrs(new_thdr)) &&
       term_is_position_rightmost(term_attrs(old_term))) ||
      (term_is_position_match(term_attrs(new_thdr))     &&
       term_is_position_match(term_attrs(old_term)))     ||
      (term_is_position_any(term_attrs(new_thdr))       &&
       term_is_position_any(term_attrs(old_term))) )
    return 0;
  if(term_is_position_any(term_attrs(new_thdr)))
    return +2;
  if(term_is_position_any(term_attrs(old_term)))
    return -2;

  return 1;
}
//----------------------------------------------------------------------------//
int FltTermsBaseWR::CompareWhere(const LEXID* /*new_lex*/, size_t /*new_lex_length*/,
                           const TERM_HDR* new_thdr, const TERM* old_term) const
{
  if(term_where(term_attrs(new_thdr)) == term_where(term_attrs(old_term)))
    return 0;
  if(term_is_where_all(term_attrs(new_thdr)))
    return +2;
  if(term_is_where_all(term_attrs(old_term)))
    return -2;

  return 1;
}
//----------------------------------------------------------------------------//
int FltTermsBaseWR::CompareMorph(const LEXID* /*new_lex*/, size_t /*new_lex_length*/,
                           const TERM_HDR* new_thdr, const TERM* old_term) const
{
  if(term_morph(term_attrs(new_thdr)) == term_morph(term_attrs(old_term)))
    return 0;
  if(term_is_morph_any(term_attrs(new_thdr)))
    return +2;
  if(term_is_morph_any(term_attrs(old_term)))
    return -2;

  return 1;
}
//----------------------------------------------------------------------------//
int FltTermsBaseWR::CompareCaps(const LEXID* new_lex, size_t new_lex_length,
                         const TERM_HDR* new_thdr, const TERM* old_term) const
{
  enum FltTermCaps new_caps = term_caps(term_attrs(new_thdr));
  enum FltTermCaps old_caps = term_caps(term_attrs(old_term));

  if(new_caps == CAPS_ANY && old_caps == CAPS_ANY)
    return 0;
  if(new_caps == CAPS_ANY)
    return +2;
  if(old_caps == CAPS_ANY)
    return -2;

  if(new_caps != old_caps)
    return 1;

  // comparing lexems' capitalization
  size_t t_length = GetTermLength(old_term);
  const LEXID* t_lex = GetTermLex(old_term);
  if(!t_lex)
    return 1;
  if(new_lex_length != t_length)
    return 1;

  for(size_t n = 0; n < new_lex_length; n++)
  {
    if(LexNormalForm(new_lex[n]) != LexNormalForm(t_lex[n]))
      return 1;
  }

  return 0;
}
//----------------------------------------------------------------------------//
int FltTermsBaseWR::CompareWeightAttrs(const LEXID* /*new_lex*/, size_t /*new_lex_length*/,
                         const TERM_HDR* new_thdr, const TERM* old_term) const
{
  if(term_is_strict(term_attrs(new_thdr)) && !term_is_strict(term_attrs(old_term)))
    return +2;
  if(!term_is_strict(term_attrs(new_thdr)) && term_is_strict(term_attrs(old_term)))
    return -2;

  if(term_is_suppl(term_attrs(new_thdr)) && !term_is_suppl(term_attrs(old_term)))
    return -2;
  if(!term_is_suppl(term_attrs(new_thdr)) && term_is_suppl(term_attrs(old_term)))
    return +2;
  if(term_weight(term_attrs(new_thdr)) != term_weight(term_attrs(old_term)))
    return 1;

  return 0;
}
//----------------------------------------------------------------------------//
const LEXID* FltTermsBaseWR::GetTermLex(const TERM* term) const
{
  const LEXID* t_lex = term_ptlex(term);
  size_t t_length = term_lex_len(term);
  if(term_is_refbody(term_header(term)))
  {
    if(t_length != 1)
      return NULL;
    const TERM* ref_term = GetTermWR(*t_lex);
    if(!ref_term)
      return NULL;
    t_lex = term_ptlex(ref_term);
  }
  return t_lex;
}
//----------------------------------------------------------------------------//
size_t FltTermsBaseWR::GetTermLength(const TERM* term) const
{
  const LEXID* t_lex = term_ptlex(term);
  size_t t_length = term_lex_len(term);
  if(term_is_refbody(term_header(term)))
  {
    if(t_length != 1)
      return 0;
    const TERM* ref_term = GetTermWR(*t_lex);
    if(!ref_term)
      return 0;
    t_length = term_lex_len(ref_term);
  }
  return t_length;
}
//============================================================================//
// Prepare data for saving
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::Prepare(struct cf_terms_save_params* params)
{
  if(!params->transport)
  {
    // calculating cross references
    enum FltTermsErrors ret = PrepareCrossRefs();
    if(ret)
      return ret;

    // calculating offsets
    m_roots.CalculateOffsets(0, 32, 0, false);
    m_index.CalculateOffsets(0, params->alignement, 0, false);
    m_sets.CalculateOffsets(0, params->alignement, 0, false);
  }

  if(params->transport)
    m_terms.CalculateOffsets(0, 0, 1, true);
  else
    m_terms.CalculateOffsets(0, params->alignement, 0, false);

  // setting offsets in terms body refs
  size_t n;
  for(n = 0; n < m_terms.Size(); n++)
  {
    TERM* term = m_terms.At(n);
    if(term_is_refbody(term_header(term)))
    {
      if(term_lex_len(term) != 1)
        return TERMS_ERROR_EFAULT;
      const struct FltTermsRefArray::reference* ref = m_terms.RefAt(*term_ptlex(term));
      if(!ref)
        return TERMS_ERROR_EFAULT;
      *term_ptlex(term) = ref->ofs;
    }
  }

  // setting offsets in all references
  if(!params->transport)
  {
    enum FltTermsErrors ret = PrepareData();
    if(ret)
      return ret;
  }

  // converting terms in transport format
  if(params->transport)
  {
    for(n = 0; n < m_terms.Size(); n++)
    {
      TERM* term = m_terms.At(n);
      
      // header
      term_header_hton(term_header(term));

      // lex array
      LEXID* lex = term_ptlex(term);
      for(size_t l = 0; l < term_lex_len(term); l++)
	lex[l] = htonl(lex[l]);
    }
  }

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::PrepareData()
{
  enum FltTermsErrors ret;
  
  size_t n;
  size_t k;

  // replace all refs in sets with offsets
  for(n = 0; n < m_sets.Size(); n++)
  {
    TERMS_SET* cur_set = m_sets.At(n);
    TERMS_SET_REF* refs = term_set_refs(cur_set);
    for(k = 0; k < term_set_length(cur_set); k++)
      refs[k] = m_terms.RefAt(refs[k])->ofs;
  }

  // replace all refs in indexes with ofsets
  TERMS_INDEX* cur_index;
  for(n = 0; n < m_index.Size(); n++)
  {
    cur_index = m_index.At(n);
    if((ret = PrepareIndex(cur_index)) != TERMS_OK)
      return ret;
  }

  // replace all refs in root indexes with ofsets
  for(n = 0; n < m_roots.Size(); n++)
  {
    cur_index = (TERMS_INDEX*)m_roots.At(n);
    if((ret = PrepareIndex(cur_index)) != TERMS_OK)
      return ret;
  }

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::PrepareIndex(TERMS_INDEX* cur_index)
{
  TERMS_INDEX_HEADER* hdr = tidx_header(cur_index);
  if(tidx_is_term_ref(tidx_ref(hdr)))
    set_tidx_ref(hdr, tidx_ref_term(m_terms.RefAt(tidx_ref_value(tidx_ref(hdr)))->ofs));
  else if(tidx_is_set_ref(tidx_ref(hdr)))
    set_tidx_ref(hdr, tidx_ref_set(m_sets.RefAt(tidx_ref_value(tidx_ref(hdr)))->ofs));
  else if(!tidx_is_empty_ref(tidx_ref(hdr)))
    return TERMS_ERROR_EFAULT;

  if(tidx_is_index_ref(tidx_cross(hdr)))
    set_tidx_cross(hdr, tidx_ref_index(m_index.RefAt(tidx_ref_value(tidx_cross(hdr)))->ofs));
  else if(!tidx_is_empty_ref(tidx_cross(hdr)))
    return TERMS_ERROR_EFAULT;

  TERMS_INDEX_ELEMENT* array = tidx_array(cur_index);
  for(size_t k = 0; k < tidx_length(cur_index); k++)
  {
    TERMS_INDEX_ELEMENT* cur_el = tidx_element(array, k);
    if(tidx_is_index_ref(tidx_ref(cur_el)))
      set_tidx_ref(cur_el, tidx_ref_index(m_index.RefAt(tidx_ref_value(tidx_ref(cur_el)))->ofs));
    else if(tidx_is_term_ref(tidx_ref(cur_el)))
      set_tidx_ref(cur_el, tidx_ref_term(m_terms.RefAt(tidx_ref_value(tidx_ref(cur_el)))->ofs));
    else if(tidx_is_set_ref(tidx_ref(cur_el)))
      set_tidx_ref(cur_el, tidx_ref_set(m_sets.RefAt(tidx_ref_value(tidx_ref(cur_el)))->ofs));
  }

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
// Prepare cross references
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::PrepareCrossRefs()
{
  FltTermsArray crlist(sizeof(size_t), 8192, 16384);

  for(size_t n = 0; n < m_roots.Size(); n++)
  {
    TERMS_INDEX* root = (TERMS_INDEX*)m_roots.At(n);
    TERMS_INDEX_ELEMENT* array = tidx_array(root);
    for(size_t k = 0; k < tidx_length(root); k++)
    {
      TERMS_INDEX_REF cur_ref = tidx_ref(tidx_element(array, k));
      if(tidx_is_index_ref(cur_ref))
      {
        TERMS_INDEX* cur_index = GetIndexWR(tidx_ref_value(cur_ref));
        enum FltTermsErrors ret = PrepareCrossRefs(cur_index, crlist, 0);
        if(ret != TERMS_OK)
          return ret;
      }
    }
  }

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::PrepareCrossRefs(TERMS_INDEX* index, FltTermsArray& crlist,
                                                             size_t crlist_first)
{
  enum FltTermsErrors ret;

  TERMS_INDEX_ELEMENT* array = tidx_array(index);

  size_t n;
  for(n = 0; n < tidx_length(index); n++)
  {
    if((ret = PrepareCrossRefs(tidx_element(array, n), crlist, crlist_first)) != TERMS_OK)
      return ret;
  }
  return TERMS_OK;
}
//----------------------------------------------------------------------------//
static enum FltTermsErrors ensure_is_index_ref(TERMS_INDEX_ELEMENT* element,
                                                FltTermsRefArrayIndexes& m_index)
{
  if(tidx_is_index_ref(tidx_ref(element)))
    return TERMS_OK;
  else if(!tidx_is_final_ref(tidx_ref(element)))
    return TERMS_ERROR_EFAULT;
  else
  {
    // inserting new index
    char new_ihdr_buf[TERMS_INDEX_HEADER_SIZE];
    TERMS_INDEX_HEADER* new_ihdr = (TERMS_INDEX_HEADER*)&new_ihdr_buf;
    memset(new_ihdr, 0, TERMS_INDEX_HEADER_SIZE);

    set_tidx_ref(new_ihdr, tidx_ref(element));

    enum FltTermsErrors ret;
    if((ret = m_index.AddIndex(NULL, 0, new_ihdr)) != TERMS_OK)
      return ret;

    set_tidx_ref(element, tidx_ref_index((unsigned int)m_index.Last()));
    return TERMS_OK;
  }
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::SetCrossRef(TERMS_INDEX_ELEMENT* element,
                                             TERMS_INDEX_ELEMENT* cross_element)
{
  enum FltTermsErrors ret;

  // validating current reference
  if((ret = ensure_is_index_ref(element, m_index)) != TERMS_OK)
    return ret;

  // validating found cross reference
  if((ret = ensure_is_index_ref(cross_element, m_index)) != TERMS_OK)
    return ret;

  // setting cross reference found
  TERMS_INDEX* idx = GetIndexWR(tidx_ref_value(tidx_ref(element)));
  if(!idx)
    return TERMS_ERROR_EFAULT;
  TERMS_INDEX_HEADER* ihdr = tidx_header(idx);
  if(tidx_cross(ihdr))
    return TERMS_ERROR_EFAULT;
  set_tidx_cross(ihdr, tidx_ref(cross_element));

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::PrepareCrossRefs(TERMS_INDEX_ELEMENT* element,
                                      FltTermsArray& crlist, size_t crlist_first)
{
  enum FltTermsErrors ret;
  size_t found = 0;
  const TERMS_INDEX_ELEMENT* found_element = NULL;

  // get cur size of cross indexes list
  size_t crlist_prev_size = crlist.Size();
  
  // getting cur key and cur reference
  LEXID key = tidx_lex(element);

  bool cross_is_set = false;
  size_t n;
  for(n = crlist_first; n < crlist_prev_size; n++)
  {
    // getting cur cross index
    size_t cur_cross_ind_no = *(size_t*)crlist.At(n);
    const TERMS_INDEX* cur_cross_index = GetIndexWR(cur_cross_ind_no);
    if(!cur_cross_index)
      return TERMS_ERROR_EFAULT;

    // search in cross index
    bool search_res = tidx_lookup(cur_cross_index, key, found, found_element);

    if(!search_res)
      continue;

    // seting cross ref
    if(!cross_is_set)
    {
      if((ret = SetCrossRef(element, const_cast <TERMS_INDEX_ELEMENT*>(found_element))) != TERMS_OK)
        return ret;
      cross_is_set = true;
    }
    
    // adding cross index to cross index list
    if(tidx_is_index_ref(tidx_ref(found_element)))
    {
      size_t* new_cr = (size_t*)crlist.Append();
      if(!new_cr)
        return TERMS_ERROR_ENOMEM;
      *new_cr = tidx_ref_value(tidx_ref(found_element));
    }
  }

  // search ref in root index
  const TERMS_INDEX* root_index = (const TERMS_INDEX*)m_roots.At(LexNoMrph(key));
  if(tidx_lookup(root_index, key, found, found_element))
  {
    if(!cross_is_set)
    {
      if((ret = SetCrossRef(element, const_cast <TERMS_INDEX_ELEMENT*>(found_element))) != TERMS_OK)
        return ret;
      cross_is_set = true;
    }
    if(tidx_is_index_ref(tidx_ref(found_element)))
    {
      size_t* new_cr = (size_t*)crlist.Append();
      if(!new_cr)
        return TERMS_ERROR_ENOMEM;
      *new_cr = tidx_ref_value(tidx_ref(found_element));
    }
  }

  if(tidx_is_index_ref(tidx_ref(element)))
  {
    TERMS_INDEX* idx = GetIndexWR(tidx_ref_value(tidx_ref(element)));
    if((ret = PrepareCrossRefs(idx, crlist, crlist_prev_size)) != TERMS_OK)
      return ret;
  }

  crlist.Truncate(crlist_prev_size);

  return TERMS_OK;
}
//============================================================================//
// Saving data
//----------------------------------------------------------------------------//
int on_write_roots(struct fstorage_stream_section_config *sec_config,
                                  struct fstorage_section_access_config *ac)
{
  FltTermsBaseWR* base = (FltTermsBaseWR*)(sec_config->context);
  if(base->save_callback_count)
    base->save_callback_count--;
    
  // check mode
  if(base->m_status != FltTermsBaseWR::STATUS_PREPARED_WORK)
    return FSTORAGE_ERROR_WRITE;

  // save
  if(base->m_roots.Serialize(ac->write, ac, 32, base->m_write_buf, base->m_write_buf_size, 0) != TERMS_OK)
    return FSTORAGE_ERROR_WRITE;

  base->CallbackClose();
  return FSTORAGE_OK;
}
//----------------------------------------------------------------------------//
int on_write_index(struct fstorage_stream_section_config *sec_config,
                                  struct fstorage_section_access_config *ac)
{
  FltTermsBaseWR* base = (FltTermsBaseWR*)(sec_config->context);
  if(base->save_callback_count)
    base->save_callback_count--;
    
  // check mode
  if(base->m_status != FltTermsBaseWR::STATUS_PREPARED_WORK)
    return FSTORAGE_ERROR_WRITE;

  // save
  if(base->m_index.Serialize(ac->write, ac, base->m_write_alignement,
                         base->m_write_buf, base->m_write_buf_size, 0) != TERMS_OK)
    return FSTORAGE_ERROR_WRITE;

  base->CallbackClose();
  return FSTORAGE_OK;
}
//----------------------------------------------------------------------------//
int on_write_sets(struct fstorage_stream_section_config *sec_config,
                                  struct fstorage_section_access_config *ac)
{
  FltTermsBaseWR* base = (FltTermsBaseWR*)(sec_config->context);
  if(base->save_callback_count)
    base->save_callback_count--;
    
  // check mode
  if(base->m_status != FltTermsBaseWR::STATUS_PREPARED_WORK)
    return FSTORAGE_ERROR_WRITE;

  // save
  if(base->m_sets.Serialize(ac->write, ac, base->m_write_alignement,
                          base->m_write_buf, base->m_write_buf_size, 0) != TERMS_OK)
    return FSTORAGE_ERROR_WRITE;

  base->CallbackClose();
  return FSTORAGE_OK;
}
//----------------------------------------------------------------------------//
int on_write_terms(struct fstorage_stream_section_config *sec_config,
                                  struct fstorage_section_access_config *ac)
{
  FltTermsBaseWR* base = (FltTermsBaseWR*)(sec_config->context);
  if(base->save_callback_count)
    base->save_callback_count--;
    
  // check mode

  size_t len_format = 0;
  size_t alignement = base->m_write_alignement;
  bool terms_transport = false;

  if(base->m_status == FltTermsBaseWR::STATUS_PREPARED_TRANSPORT)
    { len_format = 1; alignement = 0; terms_transport = true; }
  else if(base->m_status != FltTermsBaseWR::STATUS_PREPARED_WORK)
    return FSTORAGE_ERROR_WRITE;

  // save
  if(base->m_terms.Serialize(ac->write, ac, alignement,
                           base->m_write_buf, base->m_write_buf_size, len_format) != TERMS_OK)
    return FSTORAGE_ERROR_WRITE;

  base->CallbackClose();
  return FSTORAGE_OK;
}
//----------------------------------------------------------------------------//
int on_write_texts_data(struct fstorage_stream_section_config *sec_config,
                        struct fstorage_section_access_config *ac)
{
    FltTermsBaseWR* base = (FltTermsBaseWR*)(sec_config->context);
    fstorage_section_id base_sid = base->m_tb->GetBaseSid();
    if(base->save_callback_count)
        base->save_callback_count--;
    
    // check mode
    if(base->m_status != FltTermsBaseWR::STATUS_PREPARED_WORK)
        return FSTORAGE_ERROR_WRITE;
    
    if ( base->texts_last_idx != 0 )
    {
        size_t padded_size = base->texts_last_idx;
            
        // save
        int wrote = ac->write(ac, base->texts_data.get_buffer(), padded_size);
        if (wrote != (int)padded_size)
            return FSTORAGE_ERROR_WRITE;
    }

    base->CallbackClose();
    return FSTORAGE_OK;
}
//----------------------------------------------------------------------------//
int on_write_texts_index(struct fstorage_stream_section_config *sec_config,
                         struct fstorage_section_access_config *ac)
{
    FltTermsBaseWR* base = (FltTermsBaseWR*)(sec_config->context);
    if(base->save_callback_count)
        base->save_callback_count--;
    
    // check mode
    if ( base->m_status != FltTermsBaseWR::STATUS_PREPARED_TRANSPORT && 
         base->m_status != FltTermsBaseWR::STATUS_PREPARED_WORK )
        return FSTORAGE_ERROR_WRITE;
    
    // store the last index position for length calculations
    base->texts_index.push_back(base->texts_last_idx);
    if ( base->texts_index.no_memory() )
        return FSTORAGE_ERROR_NOMEMORY;
    
    // save
    if ( base->m_status == FltTermsBaseWR::STATUS_PREPARED_WORK )
    {
        size_t size = base->texts_index.size() * sizeof(uint32_t);
        int wrote = ac->write(ac, base->texts_index.get_buffer(), size);
        if (wrote != (int)size)
            return FSTORAGE_ERROR_WRITE;
    }
    else
    {
        for ( size_t i = 0; i < base->texts_index.size(); i++ )
        {
            uint32_t value = htobe32( base->texts_index[i] );
            
            int wrote = ac->write(ac, &value, sizeof(uint32_t));
            if (wrote != sizeof(uint32_t))
                return FSTORAGE_ERROR_WRITE;
        }
    }

    base->CallbackClose();
    return FSTORAGE_OK;
}
//----------------------------------------------------------------------------//
static int on_read(struct fstorage_stream_section_config*,
                                  struct fstorage_section_access_config*)
{
    return FSTORAGE_ERROR_READ;
}
//----------------------------------------------------------------------------//
void FltTermsBaseWR::CallbackClose()
{
  if(!save_callback_count && m_tb && m_tb->fs_callback_close)
    m_tb->Close();
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::PrepareSave(fstorage* fs, fstorage_section_id base_sid, struct cf_terms_save_params* params)
{
  enum FltTermsErrors ret;

  // check params
  if(!fs || !params)
    return TERMS_ERROR_EINVAL;

  // check mode
  if(m_status != STATUS_READY)
    return TERMS_ERROR_INVALID_MODE;

  // check fstorage
  if(fstorage_open_mode(fs) != FSTORAGE_OPEN_READ_WRITE /* || fstorage_last_error(fs) != FSTORAGE_OK */)
    return TERMS_ERROR_INVALID_MODE;

  // preparing data
  if((ret = Prepare(params)) != TERMS_OK)
  {
    m_status = STATUS_ERROR;
    return ret;
  }
  m_status = params->transport ? STATUS_PREPARED_TRANSPORT : STATUS_PREPARED_WORK;

  // setting callbacks for saving data
  if((ret = PrepareSection(fs, base_sid + TERMS_SECTION_OFS_TERMS, on_write_terms)) != TERMS_OK)
    return ret;
  if((ret = PrepareSection(fs, base_sid + TERMS_SECTION_OFS_TXT_DATA, on_write_texts_data)) != TERMS_OK)
    return ret;
  if((ret = PrepareSection(fs, base_sid + TERMS_SECTION_OFS_TXT_IDX, on_write_texts_index)) != TERMS_OK)
    return ret;
  if(!params->transport)
  {
    if((ret = PrepareSection(fs, base_sid + TERMS_SECTION_OFS_ROOTS, on_write_roots)) != TERMS_OK)
      return ret;
    if((ret = PrepareSection(fs, base_sid + TERMS_SECTION_OFS_INDEX, on_write_index)) != TERMS_OK)
      return ret;
    if((ret = PrepareSection(fs, base_sid + TERMS_SECTION_OFS_SETS, on_write_sets)) != TERMS_OK)
      return ret;
  }

  // preparing write buffer
  m_write_alignement = params->alignement;
  if(params->write_buf_size)
  {
    if(m_write_buf_size != params->write_buf_size)
    {
      if(m_write_buf)
        free(m_write_buf);
      m_write_buf = (char*)malloc(params->write_buf_size);
      // if no memory m_write_buf will remain NULL, it's possible

      if(m_write_buf)
        m_write_buf_size = params->write_buf_size;
      else
        m_write_buf_size = 0;
    }
  }
  else
  {
    if(m_write_buf)
      free(m_write_buf);
    m_write_buf = NULL;
    m_write_buf_size = 0;
  }

  // serialize header
  FltTermsBaseHeader hdr;
  hdr.Init();

  hdr.data.is_transport = (params->transport != 0);
  hdr.data.alignement = params->alignement;
  hdr.data.export_denied = (params->export_denied != 0);
  
  if(m_roots.Size() != TERMS_BASE_ROOTS_NUM)
    return TERMS_ERROR_EFAULT;
  for(size_t n = 0; n < m_roots.Size(); n++)
    hdr.data.roots_ofs[n] = m_roots.RefAt(n)->ofs;

  hdr.data.indexes_used = m_index.Size();
  hdr.data.sets_used = m_sets.Size();
  hdr.data.terms_used = m_terms.Size();

  hdr.data.terms_real_count = m_terms_real_count;
  hdr.data.terms_extra_count = m_terms_extra_count;
  hdr.data.terms_hier_count = m_terms_hier_count;
  hdr.data.terms_indexed_count = m_terms_indexed_count;
  
  hdr.data.base_flags = base_flags;
  
  if((ret = hdr.Serialize(fs, base_sid)) != TERMS_OK)
    return ret;

  // serialize stop table
  if(!params->transport)
  {
    FltTermsErrors status = mapStopStatus( m_table_stop.save( fs, base_sid + TERMS_SECTION_OFS_STOP_EX ) );
    if ( unlikely(status != TERMS_OK) )
      return status;
  }

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::PrepareSection(fstorage* fs, unsigned int id,
    int (*write_func)(struct fstorage_stream_section_config*, struct fstorage_section_access_config *))
{
  fstorage_section* sec = fstorage_find_section(fs, id);
  if(!sec)
    sec = fstorage_section_create(fs, id);
  if(!sec)
    return TERMS_ERROR_FS_FAILED;

  struct fstorage_stream_section_config stream_config;
  memset(&stream_config, 0, sizeof(stream_config));

  stream_config.context = (void*)this;
  stream_config.on_write = write_func;
  stream_config.on_read = on_read;
  fstorage_section_stream(sec, &stream_config);

  save_callback_count++;

  return TERMS_OK;
}
//============================================================================//
