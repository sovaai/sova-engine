#include "DocMessage.h"
//============================================================================//
size_t find_start_idx(const UTFCanonizer::CoordMapping& mapping, uint32_t start_coord, size_t& idx){
    for(; idx<mapping.size()-1; ++idx){
        if(mapping[idx].canonized_point<=start_coord && mapping[idx+1].canonized_point>start_coord) //due to mapping properties, this interval always exists
            break;
    }
    return idx;
}
size_t find_end_idx(const UTFCanonizer::CoordMapping& mapping, uint32_t end_coord, size_t& idx){
    if(idx==mapping.size()-1)
        return idx;
    for(; idx<mapping.size()-1; ++idx){
        if(mapping[idx].canonized_point<end_coord && mapping[idx+1].canonized_point>=end_coord) //due to mapping properties, this interval always exists
            break;
    }
    return idx+1;
}

static LingProcErrors WordBreakFromMapping(DocText& dt, const LingProc *lp, const UTFCanonizer::CoordMapping& mapping, CDocMessage::CoordMapping& out_mapping){ //also mapping is modified to contain words intervals only
    out_mapping.clear();
    LingProcErrors rc = lp->WordBreakDoc(dt);
    if(rc!=LP_OK)
        return rc;
    //update mapping
    //if interval equal (i.e., already canonized), border can be inside it, otherwise
    //borders should not be inside intervals, if so then it will be matched to full interval
    size_t mapping_idx=0;
    for(size_t i=0; i<dt.WordsCount(); ++i){
        size_t start_idx=find_start_idx(mapping, dt.WordOffset(i), mapping_idx);
        size_t end_idx=find_end_idx(mapping, dt.WordOffset(i)+dt.WordLength(i), mapping_idx);

        CDocMessage::CoordMappingInterval new_interval;
        if(mapping[start_idx+1].is_equal){
            int32_t offset=dt.WordOffset(i)-mapping[start_idx].canonized_point;
            new_interval.orig_begin=mapping[start_idx].orig_point+offset;
        }
        else{
            new_interval.orig_begin=mapping[start_idx].orig_point;
        }
        if(mapping[end_idx].is_equal){
            int32_t offset=(dt.WordOffset(i)+dt.WordLength(i))-mapping[end_idx-1].canonized_point;
            new_interval.orig_end=mapping[end_idx-1].orig_point+offset;
        }
        else{
            new_interval.orig_end=mapping[end_idx].orig_point;
        }

        out_mapping.push_back(new_interval);
    }
    return LP_OK;
}

int CDocMessage::SetMessageSaveCoords(const char* subj, size_t s_size, const char* body, size_t b_size, const LingProc *lp){
    const UCharSet *ucs=lp->GenericCharSet();
    int nerror;
    ResetAll();
    mapping.clear();
    // size limits
    if(m_MaxTextSize){
        if(s_size > m_MaxTextSize)
            s_size = m_MaxTextSize;
        if(s_size + b_size > m_MaxTextSize)
            b_size = m_MaxTextSize - s_size;
    }

    UTFCanonizer::CoordMapping draft_mapping;
    // subject
    if(subj != NULL && s_size > 0){
        nerror = AppendStringSaveCoords(subj, s_size, ucs, draft_mapping);
        if(nerror!=0)
            return 1;
        nerror = markup.append( LPMarkupItem(MARKUP_SUBJECT, 0, TextLength()) );
        if(nerror!=0)
            return 1;
        //add separator between subject and body
        if(body != NULL && b_size > 0){
            if((nerror = AppendString("\n", 1, CPG_UTF8, false)) != 0)
                return 1;
            draft_mapping.CreateMappingPoint(s_size+1, TextLength(), false);
        }
    }
    // body
    if(body != NULL && b_size > 0){
        size_t body_start=TextLength();
        nerror = AppendStringSaveCoords(body, b_size, ucs, draft_mapping);
        if(nerror!=0)
            return 1;
        nerror = markup.append( LPMarkupItem(MARKUP_BODY, body_start, TextLength()-body_start) );
        if(nerror!=0)
            return 1;
    }
    assert(TextLength()==draft_mapping.LastMapping().canonized_point);

    WordBreakFromMapping(*this, lp, draft_mapping, mapping);

    return 0;
}
//----------------------------------------------------------------------------//
int CDocMessage::SetMessage(const char* subj, size_t s_size, const char* body, size_t b_size, bool force_utf_canonize)
{
  int nerror;

  ResetAll();
  mapping.clear();

  // size limits
  if(m_MaxTextSize)
  {
      if(s_size > m_MaxTextSize)
        s_size = m_MaxTextSize;
      if(s_size + b_size > m_MaxTextSize)
        b_size = m_MaxTextSize - s_size;
  }

  // subject
  if(subj != NULL && s_size > 0)
  {
    if((nerror = AppendString(subj, s_size, CPG_UTF8, force_utf_canonize)) != 0)
      return 1;
    if((nerror = markup.append( LPMarkupItem(MARKUP_SUBJECT, 0, TextLength()) )) != 0)
      return 1;
    //add separator between subject and body
    if(body != NULL && b_size > 0){
      if((nerror = AppendString("\n", 1, CPG_UTF8, false)) != 0) 
        return 1;
    }
  }

  // body
  if(body != NULL && b_size > 0)
  {
    size_t body_start=TextLength();
    if((nerror = AppendString(body, b_size, CPG_UTF8, force_utf_canonize)) != 0) 
      return 1;
    if((nerror = markup.append( LPMarkupItem(MARKUP_BODY, body_start, TextLength()-body_start) )) != 0)
      return 1;
  }

  return 0;
}
//============================================================================//
