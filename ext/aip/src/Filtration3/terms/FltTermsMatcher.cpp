#include "FltTermsMatcher.h"
#include "FltTermsBase.h"

double FuncMsgLen(size_t len);
double FuncTermLen(size_t len);
double FuncDupl(size_t n);
//============================================================================//
// Add Found Term To Matcher
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsMatcher::AddFound(const TERM* term,
                                              const TermsDocImageIterator& last_pos, TDATA_FLAGS_TYPE where, bool isHomo)
{
    // find first term position
    TermsDocImageIterator first_pos = last_pos;
    size_t term_pos = m_ro_base->GetTermLength(term);
    if(term_pos)
       --term_pos;
    while(!first_pos.IsHead() && term_pos)
    {
        --first_pos;
        --term_pos;
    }
        
    FoundTerm* fterm = m_found_terms.uninitialized_grow();
    if(!fterm)
        return TERMS_ERROR_ENOMEM;
    fterm->term = term;
    fterm->where = where;
    fterm->coords.first_pos = first_pos;
    fterm->coords.last_pos = last_pos;
    fterm->deleted = false;

    if (isHomo)
    {
        if ( m_last_main != static_cast<size_t>(-1) )
            if ( (fterm->coords.first_pos == m_found_terms[m_last_main].coords.first_pos) &&
                 (fterm->coords.last_pos  == m_found_terms[m_last_main].coords.last_pos)  &&
                 (term_rubric_id( term_attrs( term_header( fterm->term ) ) ) ==
                  term_rubric_id( term_attrs( term_header( m_found_terms[m_last_main].term ) ) ) ) )
                fterm->deleted = true;
    }
    else
        m_last_main = m_found_terms.size() - 1;
    
    return TERMS_OK;
}
//============================================================================//
// Calculate weights
//----------------------------------------------------------------------------//
static int cmp_found_terms_pos(const void *elem1, const void *elem2)
{
    const TermsDocImageIterator& pos1 = ((FltTermsMatcher::FoundTerm*)elem1)->coords.first_pos;
    const TermsDocImageIterator& pos2 = ((FltTermsMatcher::FoundTerm*)elem2)->coords.first_pos;

    return pos1 == pos2 ? 0 : (pos1 < pos2 ? -1 : +1);
}
//----------------------------------------------------------------------------//
static int cmp_found_terms(const void *elem1, const void *elem2)
{
    const TERM_HDR* t1 = term_header(((FltTermsMatcher::FoundTerm*)elem1)->term);
    const TERM_HDR* t2 = term_header(((FltTermsMatcher::FoundTerm*)elem2)->term);
    if(term_rubric_id(term_attrs(t1)) < term_rubric_id(term_attrs(t2)))
        return -1;
    if(term_rubric_id(term_attrs(t1)) > term_rubric_id(term_attrs(t2)))
        return +1;
    if(term_id(t1) < term_id(t2))
        return -1;
    if(term_id(t1) > term_id(t2))
        return +1;

    const TermsDocImageIterator& pos1 = ((FltTermsMatcher::FoundTerm*)elem1)->coords.first_pos;
    const TermsDocImageIterator& pos2 = ((FltTermsMatcher::FoundTerm*)elem2)->coords.first_pos;

    return pos1 == pos2 ? 0 : (pos1 < pos2 ? -1 : +1);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsMatcherClf::FinalizeMatch(struct terms_di_info& cur_di_info)
{
    m_di = cur_di_info.di;
    m_di_size = cur_di_info.size;
    m_di_length = cur_di_info.length;
    m_di_lang_prim = cur_di_info.lang_prim;
    m_di_lang_supp = cur_di_info.lang_supp;
    assert(mapping==NULL || mapping->size()==m_di_size);
    mapping=cur_di_info.mapping;
    // check if found terms match
    size_t n;
    for(n = 0; n < m_found_terms.size(); n++)
    {
        if(!IsMatched(m_found_terms[n]))
            m_found_terms[n].deleted = true;
    }

    // check terms relations (stronger term supersedes weeker one)
    qsort(m_found_terms.get_buffer(), m_found_terms.size(), sizeof(m_found_terms[0]), cmp_found_terms_pos);

    size_t accepted = 0;
    TermsDocImageIterator last_pos;
    for(n = 0; n < m_found_terms.size(); n++)
    {
        FoundTerm& term1 = m_found_terms[n];
        if(term1.deleted)
            continue;

        if(term1.coords.first_pos >= last_pos)
            accepted = n;

        for(size_t k = accepted; k < n; k++)
        {
            FoundTerm& term2 = m_found_terms[k];
            if(term2.deleted)
                continue;

            int res = Stronger(term1.term, term1.coords, term2.term, term2.coords);
            if(res == 0)
                continue;
            if(res < 0)
                term2.deleted = true;
            if(res > 0)
                term1.deleted = true;
        }

        if(term1.coords.last_pos >= last_pos)
        {
            last_pos = term1.coords.last_pos;
            ++last_pos;
        }
    }

    // calculate terms count
    if(!m_found_terms.size())
        return TERMS_OK;

    qsort(m_found_terms.get_buffer(), m_found_terms.size(), sizeof(m_found_terms[0]),
                                                cmp_found_terms);

    unsigned int cur_term_id = 0;
    MatchedTerm* new_mterm = NULL;
    for(n = 0; n < m_found_terms.size(); n++)
    {
        FoundTerm& fterm = m_found_terms[n];
        if(fterm.deleted)
            continue;

        if(term_id(fterm.term) != cur_term_id)
        {
            new_mterm = m_match_terms.uninitialized_grow();
            if(!new_mterm)
                return TERMS_ERROR_ENOMEM;
            new_mterm->term = fterm.term;
            new_mterm->count = 1;
            new_mterm->coords.first_pos = fterm.coords.first_pos;
            new_mterm->coords.last_pos = fterm.coords.last_pos;
            new_mterm->more_coords.index=0;
            new_mterm->more_coords.size=0;

            cur_term_id = term_id(fterm.term);
        }
        else if(new_mterm)
        {
            new_mterm->count++;

            if(!new_mterm->more_coords.size)
                new_mterm->more_coords.index = m_coords_pool.size();

            struct TermCoords* coords = m_coords_pool.uninitialized_grow();
            if(!coords)
                return TERMS_ERROR_ENOMEM;

            coords->first_pos = fterm.coords.first_pos;
            coords->last_pos = fterm.coords.last_pos;

            new_mterm->more_coords.size++;
        }
        else
        {
            return TERMS_ERROR_EFAULT;
        }
    }

    // calculate categories weights
    TDATA_RID_TYPE cur_rid = 0;
    MatchedCategory* cat = NULL;
    for(n = 0; n < m_match_terms.size(); n++)
    {
        MatchedTerm& mterm = m_match_terms[n];

        if(term_rubric_id(term_attrs(mterm.term)) != cur_rid)
        {
            cat = m_match_cats.grow();
            if(!cat)
                return TERMS_ERROR_ENOMEM;
            cat->RID = cur_rid = term_rubric_id(term_attrs(mterm.term));
        }
        if(cat)
        {
            double wt = FuncTermLen(m_ro_base->GetTermLength(mterm.term)) * FuncDupl(mterm.count);

            if(term_is_suppl(term_attrs(mterm.term)))
                cat->Wsuppl += wt * term_weight(term_attrs(mterm.term));
            else
                cat->Wmain += wt * term_weight(term_attrs(mterm.term));

            if(term_is_strict(term_attrs(mterm.term)))
                cat->Nch++;
        }
    }

    double wMsgLen = FuncMsgLen(m_di_length);
    for(n = 0; n < m_match_cats.size(); n++)
    {
        m_match_cats[n].Wmain /= wMsgLen;
        m_match_cats[n].Wsuppl /= wMsgLen;
    }

    return TERMS_OK;
}
//----------------------------------------------------------------------------//
bool FltTermsMatcherClf::IsMatched(const FoundTerm& fterm) const
{
    const TERM_HDR* thdr = term_header(fterm.term);
    const cf_term_attrs* attrs = term_attrs(thdr);

    // check subject/body
    if(! (term_where(attrs) & fterm.where))
        return false;

    // morphology usage
    if(term_is_morph_none(attrs) && !term_is_hier(thdr)) // in fact it is not possible
        return false;                                    // due to compliation algorithm

    // capitalization
    enum FltTermCaps caps = term_caps(attrs);
    if(caps != CAPS_ANY)
    {
        const LEXID* trm_lex_array = m_ro_base->GetTermLex(fterm.term);
        size_t term_len = m_ro_base->GetTermLength(fterm.term);

        TermsDocImageIterator cur_pos = fterm.coords.first_pos;

        for(size_t n = 0; n < term_len; n++, ++cur_pos)
        {
            if(cur_pos > fterm.coords.last_pos) // internal error
                return false;

            LEXID trm_lex = trm_lex_array[n];
            LEXID txt_lex = cur_pos.GetLex();
            if(txt_lex == LEXINVALID) // internal error
                return false;

            switch(caps)
            {
            case CAPS_NATIVE:
                if(LexIsLower(trm_lex) && !LexIsLower(txt_lex) && !LexIsUpper(txt_lex))
                    return false;
                if(LexIsTitle(trm_lex) && !LexIsTitle(txt_lex) && !LexIsUpper(txt_lex))
                    return false;
                if(LexIsUpper(trm_lex) && !LexIsUpper(txt_lex))
                    return false;
                if(LexIsNoCase(trm_lex) && !LexIsNoCase(txt_lex))
                    return false;
                break;
            case CAPS_NO:
                if(!LexIsLower(txt_lex) && !LexIsNoCase(txt_lex))
                    return false;
                break;
            case CAPS_FIRST:
                if(n == 0 && !LexIsTitle(txt_lex) && !LexIsNoCase(txt_lex))
                    return false;
                if(n != 0 && !LexIsLower(txt_lex) && !LexIsNoCase(txt_lex))
                    return false;
                break;
            case CAPS_WORD:
                if(!LexIsTitle(txt_lex) && !LexIsNoCase(txt_lex))
                    return false;
                break;
            case CAPS_ALL:
                if(!LexIsUpper(txt_lex) && !LexIsNoCase(txt_lex))
                    return false;
                break;
            case CAPS_ANY:
                break;
            }
        }
    }

    // position
    if ( !term_is_position_any(attrs) )
    {
        TermsDocImageIterator pre_first_pos = fterm.coords.first_pos;
        TermsDocImageIterator pre_last_pos  = fterm.coords.last_pos;
        --pre_first_pos;
        ++pre_last_pos;
        
        if ( term_is_position_leftmost(attrs) && !pre_first_pos.IsHead() )
            return false;
        if ( term_is_position_rightmost(attrs) && !pre_last_pos.IsEnd() )
            return false;
    }
    
    return true;
}
//----------------------------------------------------------------------------//
int FltTermsMatcherClf::Stronger(const TERM* term1, const TermCoords& coords1,
                              const TERM* term2, const TermCoords& coords2) const
{
    // check superposition: wider is stronger
    if(coords1.first_pos == coords2.first_pos && coords1.last_pos == coords2.last_pos) // exact superposition
        return StrongerAttribs(term1, term2);
    else if(coords1.first_pos <= coords2.first_pos && coords1.last_pos >= coords2.last_pos) // inclusion
        return -1;
    else if(coords2.first_pos <= coords1.first_pos && coords2.last_pos >= coords1.last_pos) // inclusion
        return +1;
    else
        return 0;
}
//----------------------------------------------------------------------------//
int FltTermsMatcherClf::StrongerAttribs(const TERM* term1, const TERM* term2) const
{
    const TERM_HDR* hdr1 = term_header(term1);
    const TERM_HDR* hdr2 = term_header(term2);
    const cf_term_attrs* attrs1 = term_attrs(hdr1);
    const cf_term_attrs* attrs2 = term_attrs(hdr2);

    // extra is weeker
    if(term_is_extra(hdr1) && !term_is_extra(hdr2) && term_rubric_id(attrs1) == term_rubric_id(attrs2))
        return +1;
    if(term_is_extra(hdr2) && !term_is_extra(hdr1) && term_rubric_id(attrs1) == term_rubric_id(attrs2))
        return -1;

    // same id
    if(term_id(hdr1) == term_id(hdr2))
        return -1;

    // where - all is weeker
    if(term_is_where_all(attrs1) && !term_is_where_all(attrs2))
        return +1;
    if(term_is_where_all(attrs2) && !term_is_where_all(attrs1))
        return -1;

    // morphology
    if(term_is_morph_none(attrs1) && !term_is_morph_none(attrs2))
        return -1;
    if(term_is_morph_none(attrs2) && !term_is_morph_none(attrs1))
        return +1;

    // capitalization
    enum FltTermCaps caps1 = term_caps(attrs1);
    enum FltTermCaps caps2 = term_caps(attrs2);
    if(caps1 != caps2)
    {
        if(caps1 == CAPS_ANY)
            return +1;
        if(caps2 == CAPS_ANY)
            return -1;
        if(caps1 == CAPS_NATIVE)
            return +1;
        if(caps2 == CAPS_NATIVE)
            return -1;
        return -1;
    }

    return 0;
}
//============================================================================//
// Get Results
//----------------------------------------------------------------------------//
size_t FltTermsMatcherClf::GetResults(struct cf_terms_check_result* results,
                                                           size_t res_size) const
{
    // filling results
    if(!results || !res_size)
        return m_match_cats.size();

    size_t n;
    for(n = 0; n < m_match_cats.size() && n < res_size; n++)
    {
        results[n].RID = m_match_cats[n].RID;
        results[n].NCH = m_match_cats[n].Nch;
        results[n].WMAIN = m_match_cats[n].Wmain;
        results[n].WSUPP = m_match_cats[n].Wsuppl;
    }

    return m_match_cats.size();
}
//----------------------------------------------------------------------------//
static void fill_coords_original(struct cf_terms_check_found_term_coords& coords_external,
                        struct FltTermsMatcher::TermCoords& coords, const CDocMessage::CoordMapping* mapping)
{
    coords_external.term_w_from = coords.first_pos.GetWordPos();
    coords_external.term_w_to = coords.last_pos.GetWordPos() + 1;
    coords_external.term_c_from = mapping->operator[](coords_external.term_w_from).orig_begin;
    coords_external.term_c_to = mapping->operator[](coords_external.term_w_to-1).orig_end;
}
//----------------------------------------------------------------------------//
static void fill_coords(struct cf_terms_check_found_term_coords& coords_external,
                        struct FltTermsMatcher::TermCoords& coords)
{
    coords_external.term_w_from = coords.first_pos.GetWordPos();
    coords_external.term_w_to = coords.last_pos.GetWordPos() + 1;

    const DocTextWord *wrd = coords.first_pos.GetDocTextWord();
    if ( likely(wrd != 0) )
        coords_external.term_c_from = wrd->Offset();
    else
        coords_external.term_c_from = 0;

    wrd = coords.last_pos.GetDocTextWord();
    if ( likely(wrd != 0) )
        coords_external.term_c_to = wrd->Offset() + wrd->Length();
    else
        coords_external.term_c_to = 0;
}

int FltTermsMatcherClf::GetFoundTerms(struct cf_terms_check_info* info,
                 struct cf_terms_check_found_term* terms, size_t max_size)
{
    if(!m_ro_base || m_ro_base->GetStatus() != FltTermsBaseRO::STATUS_WORK)
        return TERMS_ERROR_INVALID_MODE;

    if(info)
    {
        info->nMsgWords = m_di_length;
        info->wLangPrim = m_di_lang_prim;
        info->wLangSupp = m_di_lang_supp;
    }

    // filling results
    if(!terms || !max_size)
        return (int)m_match_terms.size();

    size_t bufsize = 1024;
    char* buf = (char*)malloc(bufsize);
    if(!buf)
        return TERMS_ERROR_ENOMEM;

    enum FltTermsErrors ret;

    size_t n;
    for(n = 0; n < m_coords_pool.size(); n++)
    {
        struct cf_terms_check_found_term_coords coords_external;
        if(mapping)
            fill_coords_original(coords_external, m_coords_pool[n], mapping);
        else
            fill_coords(coords_external, m_coords_pool[n]);
        m_coords_pool_external.push_back(coords_external);
        if(m_coords_pool_external.no_memory())
            return TERMS_ERROR_ENOMEM;
    }

    for(n = 0; n < m_match_terms.size() && n < max_size; n++)
    {
        MatchedTerm& mterm = m_match_terms[n];

        if((ret = m_ro_base->GetTermText(&buf, &bufsize, mterm.term)) != TERMS_OK)
            return ret;

        strncpy(terms[n].text, buf, sizeof(terms[n].text)-1);
        terms[n].text[sizeof(terms[n].text)-1] = '\0';

        terms[n].len = m_ro_base->GetTermLength(mterm.term);

        memcpy(&terms[n].attrs, term_attrs(term_header(mterm.term)), term_attrs_size());

        terms[n].count = mterm.count;

        if(mapping)
            fill_coords_original(terms[n].coords, mterm.coords, mapping);
        else
            fill_coords(terms[n].coords, mterm.coords);

        if(mterm.count > 1 && mterm.more_coords.size)
            terms[n].more_coords = &m_coords_pool_external[mterm.more_coords.index];
        else
            terms[n].more_coords = NULL;
    }

    if(buf)
        free(buf);
    
    return (int)m_match_terms.size();
}
//============================================================================//
