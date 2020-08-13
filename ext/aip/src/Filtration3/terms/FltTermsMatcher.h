#ifndef __FltTermsMatcher_h__
#define __FltTermsMatcher_h__

#include "lib/aptl/avector.h"

#include "FltTerm.h"
#include "../common/DocMessage.h"
#include "TermsDocImageIterator.h"

#ifdef _MSC_VER
    #pragma warning(error:4706) // assignment within conditional expression
    #pragma warning(disable:4514) // unreferenced inline/local function has been removed
#endif
//=================================================================================//
class FltTermsBase;
class FltTermsBaseRO;

class FltTermsMatcher
{
    friend class FltTermsBaseRO;

public:
    /* INTERNAL DATA STRUCTURES */
    struct TermCoords
    {
        TermsDocImageIterator first_pos;
        TermsDocImageIterator last_pos;
    };

    struct FoundTerm
    {
        FoundTerm() : term(NULL), where((TDATA_FLAGS_TYPE)0), deleted(false) {}

        const TERM* term;
        TDATA_FLAGS_TYPE where;
        TermCoords coords;
        bool deleted;
    };

public:
    /* PUBLIC ACCESS */
    FltTermsMatcher() { InitData(); }
    virtual ~FltTermsMatcher() { }

    virtual void Reset()
        { InitData(); m_found_terms.clear(); }
    virtual void Free()
        { InitData(); m_found_terms.free_buffer(); }
    
private:
    /* PRIVATE METHODS */
    void InitData()
    {
        m_ro_base = NULL;
        m_di = NULL;
        m_di_size = m_di_length = 0;
        m_di_lang_prim = m_di_lang_supp = LNG_UNKNOWN;
        m_last_main = static_cast<size_t>(-1);
        mapping=NULL;
    }
    virtual enum FltTermsErrors FinalizeMatch(struct terms_di_info& cur_di_info) = 0;
    
protected:
    virtual enum FltTermsErrors AddFound(const TERM* term, const TermsDocImageIterator& last_pos, TDATA_FLAGS_TYPE where, bool isHomo = false);
    
protected:
    /* POOLS CLASSES DEFINITION */
    class FoundTermsPool : public avector<FoundTerm>
    {
    public:
        FoundTermsPool() : avector<FoundTerm>(0, 1, 20000)
            { set_initial_size(1024); }
    };

protected:
    /* PRIVATE DATA */
    const FltTermsBaseRO*   m_ro_base;
    FoundTermsPool          m_found_terms;

    const DocImage*         m_di;
    size_t                  m_di_size;    // number of words in DocImage
    size_t                  m_di_length;  // number of nonstop words in DocImage
    enum LangCodes          m_di_lang_prim;
    enum LangCodes          m_di_lang_supp;

    const CDocMessage::CoordMapping* mapping; //word coordinates in original text

    size_t                  m_last_main; // last term added which is not homonym
};
//=================================================================================//
class FltTermsMatcherClf : public FltTermsMatcher
{
public:
    /* INTERNAL DATA STRUCTURES */
    struct MatchedTerm
    {
        MatchedTerm() : term(NULL), count(0) { }

        const TERM* term;
        size_t count;
        
        TermCoords coords;
        struct MoreCoords
        {
            MoreCoords() : index(0), size(0) {}
            
            size_t index;
            size_t size;
        } more_coords;
    };

    struct MatchedCategory
    {
        MatchedCategory() : Wmain(0), Wsuppl(0), Nch(0), RID((TDATA_RID_TYPE)0) {}

        double Wmain;
        double Wsuppl;
        unsigned int Nch;
        TDATA_RID_TYPE RID;
    };

public:
    /* PUBLIC ACCESS */
    FltTermsMatcherClf() : FltTermsMatcher() { }
    virtual ~FltTermsMatcherClf() { }

    virtual void Reset()
    {
        FltTermsMatcher::Reset();
        m_match_terms.clear(); m_match_cats.clear();
        m_coords_pool.clear(); m_coords_pool_external.clear();
    }

    virtual void Free()
    {
        FltTermsMatcher::Free();
        m_match_terms.free_buffer(); m_match_cats.free_buffer();
        m_coords_pool.free_buffer(); m_coords_pool_external.free_buffer();
    }

    size_t GetResults(struct cf_terms_check_result* results, size_t res_size) const;
    size_t GetResultsSize() const
        { return m_match_cats.size(); }

    int GetFoundTerms(struct cf_terms_check_info* info,
            struct cf_terms_check_found_term* terms, size_t max_size);
    size_t GetFoundTermsSize() const
        { return m_match_terms.size(); }

private:
    /* PRIVATE METHODS */
    virtual enum FltTermsErrors FinalizeMatch(struct terms_di_info& cur_di_info);

protected:
    bool IsMatched(const FoundTerm& fterm) const;
    virtual int  Stronger(const TERM* term1, const TermCoords& coords1,
                          const TERM* term2, const TermCoords& coords2) const;
    int  StrongerAttribs(const TERM* term1, const TERM* term2) const;

private:
    /* POOLS CLASSES DEFINITION */
    class MatchedTermsPool : public avector<MatchedTerm>
    {
    public:
        MatchedTermsPool() : avector<MatchedTerm>(0, 1, 10000)
            { set_initial_size(1024); }
    };

    class MatchedCategoriesPool : public avector<MatchedCategory>
    {
    public:
        MatchedCategoriesPool() : avector<MatchedCategory>(0, 2, 5000)
            { set_initial_size(256); }
    };

    class CoordsPool : public avector<TermCoords>
    {
    public:
        CoordsPool() : avector<TermCoords>(0, 1, 20000)
            { set_initial_size(1024); }
    };

    class CoordsPoolExternal : public avector<struct cf_terms_check_found_term_coords>
    {
    public:
        CoordsPoolExternal() : avector<struct cf_terms_check_found_term_coords>(0, 1, 20000)
            { set_initial_size(1024); }
    };

private:
    /* PRIVATE DATA */
    MatchedTermsPool        m_match_terms;
    MatchedCategoriesPool   m_match_cats;
    CoordsPool              m_coords_pool;
    CoordsPoolExternal      m_coords_pool_external;
};
//=================================================================================//
#endif
