#include <stdlib.h>
#include <assert.h>

#include <_include/cc_compat.h>
#include <LingProc4/LingProc.h>

#include "FltTermsBase.h"

//----------------------------------------------------------------------//
#ifdef _MSC_VER
#pragma warning(disable:4514) // unreferenced inline/local function has been removed
#endif
//============================================================================//
// Terms base - read only mode
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseRO::Init(const LingProc* lp, fstorage* fs, fstorage_section_id base_sid)
{
    enum FltTermsErrors ret;
    Reset();

    // init lp
    if(!lp)
        return TERMS_ERROR_EINVAL;
    m_lp = lp;

    // check fstorage
    if(fstorage_open_mode(fs) == FSTORAGE_OPEN_CLOSED /* || fstorage_last_error(fs) != FSTORAGE_OK */)
        return TERMS_ERROR_INVALID_MODE;

    // deserialize header
    if((ret = m_hdr.Deserialize(fs, base_sid + TERMS_SECTION_OFS_HEADER)) != TERMS_OK)
        return ret;

    // check versions
    if(m_hdr.data.magic != TERMS_BASE_MAGIC)
        return TERMS_ERROR_INVALID_FORMAT;

    if(m_hdr.data.is_transport)
    {
        if(m_hdr.data.transport_format_version != TERMS_BASE_TRANSPORT_FORMAT_VERSION)
            return TERMS_ERROR_INVALID_FORMAT;
    }
    else
    {
        if(m_hdr.data.work_format_version != TERMS_BASE_WORK_FORMAT_VERSION)
            return TERMS_ERROR_INVALID_FORMAT;
    }

    // load all sections and setting pointers
    /* indexes */
    if(!m_hdr.data.is_transport)
    {
        if(fstorage_load_section(fs, base_sid + TERMS_SECTION_OFS_ROOTS) != FSTORAGE_OK)
            return TERMS_ERROR_FS_FAILED;
        if(fstorage_load_section(fs, base_sid + TERMS_SECTION_OFS_INDEX) != FSTORAGE_OK)
            return TERMS_ERROR_FS_FAILED;
        if(fstorage_load_section(fs, base_sid + TERMS_SECTION_OFS_SETS) != FSTORAGE_OK)
            return TERMS_ERROR_FS_FAILED;

        m_roots_pt = (char*)fstorage_section_get_all_data(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_ROOTS));
        m_index_pt = (char*)fstorage_section_get_all_data(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_INDEX));
        m_sets_pt = (char*)fstorage_section_get_all_data(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_SETS));

        m_roots_size = fstorage_section_get_size(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_ROOTS));
        m_index_size = fstorage_section_get_size(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_INDEX));
        m_sets_size = fstorage_section_get_size(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_SETS));

        if(!m_roots_pt || !m_index_pt || !m_sets_pt)
            return TERMS_ERROR_FS_FAILED;
    }

    /* terms */
    if(fstorage_load_section(fs, base_sid + TERMS_SECTION_OFS_TERMS) != FSTORAGE_OK)
        return TERMS_ERROR_FS_FAILED;

    m_terms_pt = (char*)fstorage_section_get_all_data(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_TERMS));
    m_terms_size = fstorage_section_get_size(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_TERMS));

    if(!m_terms_pt)  
        return TERMS_ERROR_FS_FAILED;
    
    if ( m_hdr.data.base_flags & TERMS_BASE_FLAGS_WITH_TEXT )
    {
        /* terms texts */
        
        m_texts_data =
            (uint8_t*)fstorage_section_get_all_data(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_TXT_DATA));
        m_texts_data_size =
            fstorage_section_get_size(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_TXT_DATA));

        m_texts_index =
            (uint8_t*)fstorage_section_get_all_data(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_TXT_IDX));
        m_texts_index_size =
            fstorage_section_get_size(fstorage_find_section(fs, base_sid + TERMS_SECTION_OFS_TXT_IDX));

        if ( m_texts_data == 0 || m_texts_index == 0 )
            return TERMS_ERROR_FS_FAILED;
    }

    /* stop table */
    if(!m_hdr.data.is_transport)
    {
        FltTermsErrors status = mapStopStatus( m_table_stop.load( fs, base_sid + TERMS_SECTION_OFS_STOP_EX ) );
        if ( status != TERMS_OK )
            return status;
    }

    // setting status
    m_status = m_hdr.data.is_transport ? STATUS_TRANSPORT : STATUS_WORK;
     
    return TERMS_OK;
}
//----------------------------------------------------------------------------//
void FltTermsBaseRO::Reset()
{
    m_hdr.Clear();
    
    if ( m_texts_data_enc != 0 )
        free( m_texts_data_enc );
    m_texts_data_enc = 0;
    
    m_lp = NULL;
    m_roots_pt = m_index_pt = m_sets_pt = m_terms_pt = NULL;
    m_roots_size = m_index_size = m_sets_size = m_terms_size = 0;
    
    m_texts_data       = 0;
    m_texts_index      = 0;
    m_texts_data_size  = 0;
    m_texts_index_size = 0;
    
    m_status = STATUS_NOT_READY;
}
//============================================================================//
// Terms Enumeration
//----------------------------------------------------------------------------//
struct dump_term_data
{
    const LEXID* lex;
    size_t length;
    unsigned long id;
    bool is_extra;
    bool is_hier;
};

enum FltTermsErrors FltTermsBaseRO::Enum(cf_terms_enum_callback callback, void* data,
                                         bool for_export, bool dump) const
{
    // check pointers
    if(!callback)
        return TERMS_ERROR_EINVAL;

    // check mode
    if(m_status != STATUS_WORK && m_status != STATUS_TRANSPORT)
        return TERMS_ERROR_INVALID_MODE;

    // check access
    if(for_export && m_hdr.data.export_denied)
        return TERMS_ERROR_EACCES;

    // text buffer for term text - may be reallocated in GetTermText
    size_t bufsize = 2000;
    char* buf = (char*)malloc(bufsize);
    if(!buf)
        return TERMS_ERROR_ENOMEM;

    // enumerating terms
    enum FltTermsErrors ret = TERMS_OK;
    size_t ofs = 0;
    while(ofs < m_terms_size)
    {
        TERM* term;
        if(m_hdr.data.is_transport)
        {
            /* transport format */
            const char* pt = m_terms_pt + ofs;

            char term_buf[sizeof(TERM_LEN) + TERM_HDR_SIZE + sizeof(LEXID) * TERM_MAX_LENGTH];
            term = (TERM*)&term_buf;

            // length
            *(TERM_LEN*)term = (unsigned char)(*pt++);
            if(term_lex_len(term) > TERM_MAX_LENGTH)
                return TERMS_ERROR_ERANGE;

            // header
            memcpy(term_header(term), pt, TERM_HDR_SIZE);
            term_header_ntoh(term_header(term));
            pt += TERM_HDR_SIZE;

            // lex array
            memcpy(term_ptlex(term), pt, sizeof(LEXID) * term_lex_len(term));
            LEXID* lex = term_ptlex(term);
            pt += sizeof(LEXID) * term_lex_len(term);

            size_t n;
            for(n = 0; n < term_lex_len(term); n++)
                lex[n] = ntohl(lex[n]);

            // ref body
            if(term_is_refbody(term_header(term)))
            {
                // copy real body
                if(term_lex_len(term) != 1)
                    return TERMS_ERROR_EFAULT;

                const char* ref_pt = m_terms_pt + *(term_ptlex(term));
                size_t ref_term_len = *ref_pt++;
                if(ref_term_len > TERM_MAX_LENGTH)
                    return TERMS_ERROR_ERANGE;
                ref_pt += TERM_HDR_SIZE;
                memcpy(term_ptlex(term), ref_pt, sizeof(LEXID) * ref_term_len);

                // adjust term
                *(TERM_LEN*)term = (TERM_LEN)ref_term_len;
                clear_term_refbody(term_header(term));

                for(n = 0; n < term_lex_len(term); n++)
                    lex[n] = ntohl(lex[n]);
            }

            // getting next term
            ofs += (pt - (m_terms_pt + ofs));
        }
        else
        {
            /* work format */
            term = (TERM*)(m_terms_pt + ofs);

            // getting next term
            ofs += term_full_size(term);
            if(m_hdr.data.alignement && ofs%m_hdr.data.alignement)
                ofs += (m_hdr.data.alignement - ofs%m_hdr.data.alignement);
        }

        if(!term_is_extra(term_header(term)))
        {
            // prepare term text
            if((ret = GetTermText(&buf, &bufsize, term)) != TERMS_OK)
                break;

            // callback
            void* call_data = data;
            struct dump_term_data dump_data;
            if(dump)
            {
                dump_data.lex = GetTermLex(term);
                dump_data.length = GetTermLength(term);
                dump_data.id = term_id(term);
                dump_data.is_extra = term_is_extra(term_header(term));
                dump_data.is_hier = term_is_hier(term_header(term));
                call_data = &dump_data;
            }

            if ((*callback)(term_attrs(term_header(term)), buf, call_data))
            { ret = TERMS_ERROR_BREAK;  break; }
        }
    }
  
    if(buf)
        free(buf);

    return ret;
}
//============================================================================//
// Base info and dump
//----------------------------------------------------------------------------//
static int dump_term(const cf_term_attrs* /* attrs */, const char* txt, void* data)
{
    const struct dump_term_data* dump_data = (const struct dump_term_data*)data;

    printf("%lu %s ==", dump_data->id, txt);
    for(size_t i = 0; i < dump_data->length; i++)
    {
        printf(" %08X", dump_data->lex[i]);
    }

    if(dump_data->is_extra)
        printf(" extra");

    if(dump_data->is_hier)
        printf(" hier");

    printf("\n");
    return 0;
}
//----------------------------------------------------------------------------//
void FltTermsBaseRO::PrintInfo(bool dump_indexes, bool dump_terms) const
{
    // check mode
    if(m_status != STATUS_WORK && m_status != STATUS_TRANSPORT)
        return;

    // common info
    printf("** TERMS DATA INFO **\n");
    printf("---------------------\n");
    printf("Version: %u.%u.%u.%u\n",
           static_cast<unsigned>( m_hdr.data.work_format_version ),
           static_cast<unsigned>( m_hdr.data.work_format_variant ),
           static_cast<unsigned>( m_hdr.data.transport_format_version ),
           static_cast<unsigned>( m_hdr.data.transport_format_variant ));
    printf("Transfer mode: %u, Alignement: %u, Export Denied: %u\n",
           static_cast<unsigned>( m_hdr.data.is_transport ),
           static_cast<unsigned>( m_hdr.data.alignement ),
           static_cast<unsigned>( m_hdr.data.export_denied ));
    printf( "Terms texts in base: %s\n",
            ( m_hdr.data.base_flags & TERMS_BASE_FLAGS_WITH_TEXT ? "available" : "unavailable" ) );
    if ( m_hdr.data.base_flags & TERMS_BASE_FLAGS_WITH_TEXT )
    {
        printf( "\tTerms texts section size: %d\n"
                "\tTerms indexes count: %d\n",
                static_cast<int>( m_texts_data_size ), static_cast<int>( m_texts_index_size / sizeof(uint32_t) ) );
    }
    printf("---------------------\n");
    printf("ROOTS Sizes: %u = ", static_cast<unsigned>( m_roots_size ));

    size_t i;
    if(!m_hdr.data.is_transport)
    {
        for(i = 1; i < TERMS_BASE_ROOTS_NUM; i++)
        {
            printf("%i(%lu), ", static_cast<int>( m_hdr.data.roots_ofs[i] - m_hdr.data.roots_ofs[i-1] ),
                   static_cast<unsigned long>( tidx_length((const TERMS_INDEX*)(m_roots_pt + m_hdr.data.roots_ofs[i-1])) ));
            if(i%4 == 0)
                printf("\n               ");
        }
        printf("%i(%lu)\n", static_cast<int>( m_roots_size - m_hdr.data.roots_ofs[i-1] ),
               static_cast<unsigned long>( tidx_length((const TERMS_INDEX*)(m_roots_pt + m_hdr.data.roots_ofs[i-1])) ));
        printf("ROOTS Offsets: ");
        for(i = 0; i < TERMS_BASE_ROOTS_NUM; i++)
        {
            if(i != TERMS_BASE_ROOTS_NUM-1)
            {
                printf("%08X, ", m_hdr.data.roots_ofs[i]);
                if((i+1)%4 == 0)
                    printf("\n               ");
            }
            else
                printf("%08X\n", m_hdr.data.roots_ofs[i]);
        }
    }

    printf("INDEXES number: %u, size: %u\n", static_cast<unsigned>( m_hdr.data.indexes_used ), static_cast<unsigned>( m_index_size ));
    printf("SETS    number: %u, size: %u\n", static_cast<unsigned>( m_hdr.data.sets_used ), static_cast<unsigned>( m_sets_size ));
    printf("TERMS   number: %u (real = %u, extra = %u, hier = %u, indexed = %u), size: %u\n",
           static_cast<unsigned>( m_hdr.data.terms_used ), 
           static_cast<unsigned>( m_hdr.data.terms_real_count ),
           static_cast<unsigned>( m_hdr.data.terms_extra_count ),
           static_cast<unsigned>( m_hdr.data.terms_hier_count ),
           static_cast<unsigned>( m_hdr.data.terms_indexed_count ),
           static_cast<unsigned>( m_terms_size ));
    printf("---------------------\n");

    // terms dump
    if(dump_terms)
    {
        printf("---------------------\n");
        printf("TERMS:\n");
        Enum(dump_term, NULL, false, true);
    }

    // indexes dump
    if(dump_indexes && !m_hdr.data.is_transport)
    {
        // root indexes
        for(i = 0; i < TERMS_BASE_ROOTS_NUM; i++)
        {
            if(tidx_length(GetRootIndex(m_hdr.data.roots_ofs[i])))
            {
                printf("ROOT INDEX %u\n", static_cast<unsigned>( i ));
                DumpIndex(GetRootIndex(m_hdr.data.roots_ofs[i]));
                printf("---------------------\n");
            }
        }

        // other indexes
        unsigned int ofs = 0;
        for(i = m_hdr.data.indexes_used; i > 0; i--)
        {
            printf("INDEX %lX\n", static_cast<unsigned long>( ofs ));
            const TERMS_INDEX* idx = GetIndex(ofs);
            DumpIndex(idx);
            printf("---------------------\n");

            // getting next index offset
            ofs += (unsigned int)index_full_size(idx);
            if(m_hdr.data.alignement && ofs%m_hdr.data.alignement)
                ofs += (unsigned int)(m_hdr.data.alignement - ofs%m_hdr.data.alignement);
        }
    }
}
//----------------------------------------------------------------------------//
void FltTermsBaseRO::DumpIndex(const TERMS_INDEX* index) const
{
    const TERMS_INDEX_HEADER* ihdr = tidx_header(index);

    // index length
    printf("length: %lu", static_cast<unsigned long>( tidx_length(index) ));

    // index header ref
    TERMS_INDEX_REF ref = tidx_ref(ihdr);
    if(tidx_is_index_ref(ref))
         printf(" ref: ind %lX", static_cast<unsigned long>( tidx_ref_value(ref) ));
    else if(tidx_is_term_ref(ref))
         printf(" ref: term %lX", static_cast<unsigned long>( tidx_ref_value(ref) ));
    else if(tidx_is_set_ref(ref))
         printf(" ref: set %lX", static_cast<unsigned long>( tidx_ref_value(ref) ));
    else if(!tidx_is_empty_ref(ref))
         printf(" ref: none %lX", static_cast<unsigned long>( ref ));

    // index header cross
    ref = tidx_cross(ihdr);
    if(tidx_is_index_ref(ref))
        printf(" cross: ind %lX", static_cast<unsigned long>( tidx_ref_value(ref) ));
    else if(tidx_is_term_ref(ref))
        printf(" cross: term %lX", static_cast<unsigned long>( tidx_ref_value(ref) ));
    else if(tidx_is_set_ref(ref))
        printf(" cross: set %lX", static_cast<unsigned long>( tidx_ref_value(ref) ));
    else if(!tidx_is_empty_ref(ref))
        printf(" cross: none %lX", static_cast<unsigned long>( ref ));

    printf("\n");

    // index elements
    const TERMS_INDEX_ELEMENT* array = tidx_array(index);
    size_t n;
    for(n = 0; n < tidx_length(index); n++)
    {
        LEXID lex = tidx_lex(tidx_element(array, n));
        BufferReceiver<4096> textReceiver;
        LingProcErrors status = m_lp->LexText(lex, textReceiver);
        if ( unlikely(status != LP_OK) )
            printf("%08lX (ERROR) --> ", (long unsigned int)lex);
        else
            printf("%08lX (%.*s) --> ", (long unsigned int)lex, (int)textReceiver.size(), textReceiver.getBuffer());

        TERMS_INDEX_REF ref = tidx_ref(tidx_element(array, n));
        if(tidx_is_index_ref(ref))
            printf("ind %lX", static_cast<unsigned long>( tidx_ref_value(ref) ));
        else if(tidx_is_term_ref(ref))
            printf("term %lX", static_cast<unsigned long>( tidx_ref_value(ref) ));
        else if(tidx_is_set_ref(ref))
            printf("set %lX", static_cast<unsigned long>( tidx_ref_value(ref) ));
        else
            printf("none %lX", static_cast<unsigned long>( ref ));
  
        printf("\n");
    }
}
//============================================================================//
// Terms Search (Check)
//----------------------------------------------------------------------------//
/*enum FltTermsErrors FltTermsBaseRO::Check(FltTermsMatcher* matcher,
                                          const DocImage* di, bool bNoMarkup)
{
    return Check(matcher, di, NULL, bNoMarkup);
}*/

enum FltTermsErrors FltTermsBaseRO::Check(FltTermsMatcher* matcher,
                                          const DocImage* di, const CDocMessage::CoordMapping* mapping, bool bNoMarkup)
{
    enum FltTermsErrors ret;

    // check pointers
    if(!di || !matcher)
        return TERMS_ERROR_EINVAL;

    // check mode
    if(m_status != STATUS_WORK)
        return TERMS_ERROR_INVALID_MODE;

    // clear search results
    matcher->Reset();
    matcher->m_ro_base = this;

    // get DocImage info
    struct terms_di_info di_info;
    if((ret = FillDocImageInfo(m_lp, di, mapping, &di_info, bNoMarkup)) != TERMS_OK)
        return ret;
  
    // check message length
    if(di_info.size == 0)
        return TERMS_OK;

    // lookup terms
    if((ret = Lookup(matcher, &di_info, TDATA_WHERE_SUBJ)) != TERMS_OK)
        return ret;
    if((ret = Lookup(matcher, &di_info, TDATA_WHERE_BODY)) != TERMS_OK)
        return ret;

    // calculating weight
    if((ret = matcher->FinalizeMatch(di_info)) != TERMS_OK)
        return ret;

    return TERMS_OK;
}
//----------------------------------------------------------------------------//
bool FltTermsBaseRO::CheckIfAllStopWords(const DocImage* di, bool bNoMarkup)
{
    // check mode
    if(m_status != STATUS_WORK)
        return false;

    // get DocImage info
    struct terms_di_info di_info;
    if ( FillDocImageInfo( m_lp, di, NULL, &di_info, bNoMarkup ) != TERMS_OK ) //TODO:check
        return false;
    
    size_t not_stop_word = 0;

    DocImageIterator start_pos( *di_info.di );
    DocImageIterator finish_pos( *di_info.di );
    start_pos  = di_info.body_from;
    finish_pos = di_info.body_to;
    
    TermsDocImageIterator it( (*di_info.di), &m_table_stop, true, start_pos );
    for ( ; it < finish_pos; ++it, ++not_stop_word );
    
    return not_stop_word == 0;
}
//----------------------------------------------------------------------------//
FltTermsErrors FltTermsBaseRO::insertUnique(LookupPositions &pool, const TERMS_INDEX *index)
{
    bool found = false;
    for ( size_t i = 0; i < pool.getSize(); i++ )
        if ( pool[i] == index )
        {
            found = true;
            break;
        }
                
    if ( !found )
    {
        const TERMS_INDEX **pPoint = pool.grow();
        if ( unlikely(pPoint == 0) )
            return TERMS_ERROR_ENOMEM;
        *pPoint = index;
    }

    return TERMS_OK;
}
    
enum FltTermsErrors FltTermsBaseRO::Lookup(FltTermsMatcher* matcher,
                                           struct terms_di_info* di_info, TDATA_FLAGS_TYPE where)
{
    LEXID hier;
  
    const TERMS_INDEX* hier_index = NULL;
    
    indexesSwitch = 0;
    
    primIndexes[0].reset();
    suppIndexes[0].reset();
    primIndexes[1].reset();
    suppIndexes[1].reset();

    const TERMS_INDEX **pPoint;
    
    pPoint = primIndexes[0].grow();
    if ( unlikely(pPoint == 0) )
        return TERMS_ERROR_ENOMEM;
    *pPoint = 0;
    pPoint = suppIndexes[0].grow();
    if ( unlikely(pPoint == 0) )
        return TERMS_ERROR_ENOMEM;
    *pPoint = 0;
    
    DocImageIterator start_pos(*di_info->di);
    DocImageIterator finish_pos(*di_info->di);
    if(where == TDATA_WHERE_SUBJ)
        { start_pos = di_info->subj_from; finish_pos = di_info->subj_to; }
    else if(where == TDATA_WHERE_BODY)
        { start_pos = di_info->body_from; finish_pos = di_info->body_to; }
    else
        return TERMS_ERROR_EINVAL;
    
    TermsDocImageIterator it( (*di_info->di), &m_table_stop, true, start_pos );
    for ( ; it < finish_pos; ++it )
    {
        const LEXID *primArray, *suppArray;
        size_t       primSize,   suppSize;
        if ( Lookup_GetHomo(di_info, it, primArray, primSize, suppArray, suppSize, hier) != 0 )
            return TERMS_ERROR_ENOMEM;

        const TERMS_INDEX *last_hier = hier_index;
        
        if ( !it.IsMiddleCompoundPos() && hier != LEXNONE )
        {
            // lookup hieroglyphs
            if(!hier_index)
                hier_index = GetRootIndexLex(hier);
            TermsDocImageIterator cur_it = it;
            cur_it.SetCompoundMode(false);
            hier_index = Lookup_Index(matcher, cur_it, hier_index, hier, where);
        }
        
        // lookup all languages
        struct Pass
        {
            Pass( LookupPositions &_indexes,
                  LookupPositions &_nextIndexes,
                  const LEXID     *_lexArray,
                  size_t           _lexSize,
                  bool             _isCompoundModeOn,
                  bool             _allowRun ) :
                indexes( _indexes ),
                nextIndexes( _nextIndexes ),
                lexArray( _lexArray ),
                lexSize( _lexSize ),
                isCompoundModeOn( _isCompoundModeOn ),
                allowRun( _allowRun ) { }
            
            LookupPositions    &indexes;
            LookupPositions    &nextIndexes;
            const LEXID        *lexArray;
            size_t              lexSize;
            bool                isCompoundModeOn;
            bool                allowRun;
        
        private:
            // private assignement operator
            Pass& operator = (const Pass&) { return *this; }
        } passes[] =
              {
                  Pass(primIndexes[indexesSwitch], primIndexes[(indexesSwitch + 1) % 2], primArray, primSize,
                       di_info->prim_compound_support, true),
                  Pass(suppIndexes[indexesSwitch], suppIndexes[(indexesSwitch + 1) % 2], suppArray, suppSize,
                       di_info->supp_compound_support, (di_info->lang_supp ? true : false))
              };

        for ( size_t p = 0; p < (sizeof(passes) / sizeof(Pass)); p++ )
        {
            if ( !passes[p].allowRun )
                continue;
            
            if ( it.IsMiddleCompoundPos() && !passes[p].isCompoundModeOn )
                continue;
            
            passes[p].nextIndexes.reset();

            for ( size_t posIdx = 0; posIdx < passes[p].indexes.getSize(); posIdx++ )
            {
                const TERMS_INDEX *currentPoint = passes[p].indexes[posIdx];
            
                if (passes[p].lexSize == 0 && currentPoint == last_hier)
                {
                    FltTermsErrors status = insertUnique(passes[p].nextIndexes, hier_index);
                    if ( unlikely(status != TERMS_OK) )
                        return status;
                }
                else
                {
                    if (passes[p].lexSize == 0)
                    {
                        passes[p].lexArray = &hier;
                        passes[p].lexSize  = 1;
                    }

                    for (size_t k = 0; k < passes[p].lexSize; k++)
                    {
                        LEXID lex = LexNormalize( passes[p].lexArray[k] );

                        TermsDocImageIterator cur_it = it;
                        cur_it.SetCompoundMode(passes[p].isCompoundModeOn);

                        const TERMS_INDEX *idx = currentPoint;
                        if ( idx == 0 )
                            idx = GetRootIndexLex(lex);
                        
                        LEXID branches[2] = { lex, hier };
                        for ( size_t l = 0; l < sizeof(branches) / sizeof(LEXID); l++ )
                        {
                            FltTermsErrors status = insertUnique( passes[p].nextIndexes,
                                                                  Lookup_Index( matcher,
                                                                                cur_it,
                                                                                idx,
                                                                                branches[l],
                                                                                where,
                                                                                ( branches[l] == hier ? hier_index : NULL ),
                                                                                k != 0 /* Here: processing homonym or not */ ) );
                            if ( unlikely(status != TERMS_OK) )
                                return status;
                        }
                    }
                }
            }
        }

        indexesSwitch = (indexesSwitch + 1) % 2;

        if(!it.IsMiddleCompoundPos())
            di_info->length++;
    }

    return TERMS_OK;
}
//----------------------------------------------------------------------------//
const TERMS_INDEX* FltTermsBaseRO::Lookup_Index(FltTermsMatcher* matcher,
                                                const TermsDocImageIterator &it,
                                                const TERMS_INDEX* index, LEXID key,
                                                TDATA_FLAGS_TYPE where, const TERMS_INDEX *suppress_hier, bool isHomo) const
{
    size_t found_el_no = 0;
    const TERMS_INDEX_ELEMENT* found_el = NULL;

    TERMS_INDEX_REF cross_ref = NULL;
    const TERMS_INDEX* cross_index = NULL;

    if(!tidx_lookup(index, key, found_el_no, found_el))
    {
        // not found in current index
        // jumping by cross ref
        cross_ref = tidx_cross(tidx_header(index));
        assert(!tidx_is_final_ref(cross_ref));

        while(tidx_is_index_ref(cross_ref))
        {
            cross_index = GetIndex(tidx_ref_value(cross_ref));
      
            // loop control - possible only if data is incorrect
            assert(cross_index != index);
            if(cross_index == index)  
                return NULL;

            if(!tidx_lookup(cross_index, key, found_el_no, found_el))
            {
                cross_ref = tidx_cross(tidx_header(cross_index));
                assert(!tidx_is_final_ref(cross_ref));
            }
            else
                // found in cross index
                break;
        }
        if(!tidx_is_index_ref(cross_ref))
        {
            if(!tidx_lookup(GetRootIndexLex(key), key, found_el_no, found_el))
                return NULL;
        }
    }

    assert(found_el);
  
    // found
    if(tidx_is_final_ref(tidx_ref(found_el)))
    {
        AddToMatcher(matcher, it, tidx_ref(found_el), where, isHomo);
        return NULL;
    }
    else if(tidx_is_index_ref(tidx_ref(found_el)))
    {
        const TERMS_INDEX* found_index = GetIndex(tidx_ref_value(tidx_ref(found_el)));

        // check terms in header of found index
        if(tidx_is_final_ref(tidx_ref(tidx_header(found_index))) && (suppress_hier == NULL || suppress_hier != found_index))
            AddToMatcher(matcher, it, tidx_ref(tidx_header(found_index)), where, isHomo);

        // scanning cross refs to get found terms
        cross_ref = tidx_cross(tidx_header(found_index));
        assert(!tidx_is_final_ref(cross_ref));
        while(tidx_is_index_ref(cross_ref))
        {
            cross_index = GetIndex(tidx_ref_value(cross_ref));

            // loop control - possible only if data is incorrect
            assert(cross_index != found_index);
            if(cross_index == found_index)  
                return found_index;

            if(tidx_is_final_ref(tidx_ref(tidx_header(cross_index))))
                AddToMatcher(matcher, it, tidx_ref(tidx_header(cross_index)), where, isHomo);

            cross_ref = tidx_cross(tidx_header(cross_index));
            assert(!tidx_is_final_ref(cross_ref));
            if(tidx_is_final_ref(cross_ref))
                return NULL;
        }
        return found_index;
    }
    else
        // error in data
        return NULL;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseRO::AddToMatcher( FltTermsMatcher             *matcher,
                                                  const TermsDocImageIterator &it,
                                                  TERMS_INDEX_REF              ref,
                                                  TDATA_FLAGS_TYPE             where,
                                                  bool                         isHomo) const
{
    if(!tidx_is_final_ref(ref))
        return TERMS_ERROR_EFAULT;
    
    if(tidx_is_term_ref(ref))
    {
        return matcher->AddFound(GetTerm(tidx_ref_value(ref)), it, where, isHomo);
    }

    if(tidx_is_set_ref(ref))
    {
        const TERMS_SET* set = GetTermSet(tidx_ref_value(ref));
        const TERMS_SET_REF* refs = term_set_refs(set);
        for(size_t i = 0; i < term_set_length(set); i++)
        {
            enum FltTermsErrors ret = matcher->AddFound(GetTerm(refs[i]), it, where, isHomo);
            if(ret)
                return ret;
        }
    }

    return TERMS_OK;
}
//============================================================================//
const LEXID* FltTermsBaseRO::GetTermLex(const TERM* term) const
{
    const LEXID* t_lex = term_ptlex(term);
    size_t t_length = term_lex_len(term);
    if(term_is_refbody(term_header(term)))
    {
        if(t_length != 1)
            return NULL;
        const TERM* ref_term = GetTerm(*t_lex);
        if(!ref_term)
            return NULL;
        t_lex = term_ptlex(ref_term);
    }
    return t_lex;
}
//----------------------------------------------------------------------------//
size_t FltTermsBaseRO::GetTermLength(const TERM* term) const
{
    const LEXID* t_lex = term_ptlex(term);
    size_t t_length = term_lex_len(term);
    if(term_is_refbody(term_header(term)))
    {
        if(t_length != 1)
            return 0;
        const TERM* ref_term = GetTerm(*t_lex);
        if(!ref_term)
            return 0;
        t_length = term_lex_len(ref_term);
    }
    return t_length;
}
//============================================================================//
