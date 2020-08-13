#ifndef __FltTermsBase_h__
#define __FltTermsBase_h__
//----------------------------------------------------------------------------//
#ifdef WITH_OLD_LINGPROC
  #error "WITH_OLD_LINGPROC is obsolete"
#endif
//----------------------------------------------------------------------------//
#include <string.h>

#include <lib/aptl/avector.h>
#include <lib/aptl/GrowingPool.h>

#include "FltTerm.h"
#include "FltTermsArray.h"
#include "FltTermsMatcher.h"
#include "FltTermsStopEx.h"
#include "terms.h"
#include "term_attrs_ntoh.h"

#include "../common/DocMessage.h"
#include "LingProc4/DocImage/DocImage.h"

#include "TermsDocImageIterator.h"

//============================================================================//
// Special Defines
//----------------------------------------------------------------------------//
#define USE_TERMS_REF_BODY
#define OLD_PAT_COMPATIBLE

//============================================================================//
// Terms sections in fstorage
//----------------------------------------------------------------------------//
#include "lib/fstorage/fstorage_id.h"
#define TERMS_SECTION_OFS_HEADER    0
#define TERMS_SECTION_OFS_ROOTS     1
#define TERMS_SECTION_OFS_INDEX     2
#define TERMS_SECTION_OFS_SETS      3
#define TERMS_SECTION_OFS_TERMS     4
#define TERMS_SECTION_OFS_STOP      5
#define TERMS_SECTION_OFS_TXT_DATA  6
#define TERMS_SECTION_OFS_TXT_IDX   7
#define TERMS_SECTION_OFS_STOP_EX   8
#define TERMS_SECTION_OFS_ENC_KEY   9

#define TERMS_SECTION_MAX_NUMBER    16
//============================================================================//
// Terms base header
//----------------------------------------------------------------------------//
#define TERMS_BASE_MAGIC 0x67AD01B5
#define TERMS_BASE_WORK_FORMAT_VERSION  2
#define TERMS_BASE_WORK_FORMAT_VARIANT  4
#define TERMS_BASE_TRANSPORT_FORMAT_VERSION  2
#define TERMS_BASE_TRANSPORT_FORMAT_VARIANT  4

#define TERMS_BASE_FLAGS_DEFAULT              0x00000000UL
#define TERMS_BASE_FLAGS_WITH_TEXT            0x00000001UL
#define TERMS_BASE_FLAGS_ALLOW_COMPILE_ERRORS 0x00000002UL
#define TERMS_BASE_FLAGS_MAP_ERANGE_WARNING   0x00000004UL

#define TERMS_BASE_ENC_KEY_LENGTH  64

class FltTermsBaseHeader
{
public:
    struct _header_data {
        unsigned int magic;

        unsigned int work_format_version;
        unsigned int work_format_variant;
        unsigned int transport_format_version;
        unsigned int transport_format_variant;

        bool is_transport;
        bool export_denied;

        size_t alignement;

#define TERMS_BASE_ROOTS_NUM (1 << (BitsCounter< static_cast<uint64_t>(LEX_MORPH) >::n))

        unsigned int roots_ofs[TERMS_BASE_ROOTS_NUM];

        size_t indexes_used;
        size_t sets_used;
        size_t terms_used;

        size_t terms_real_count;    // number of 'real' terms = last used id
        size_t terms_extra_count;   // number of 'extra' terms
        size_t terms_hier_count;    // number of 'hier' terms
        size_t terms_indexed_count; // number of terms added to index
          
        uint32_t base_flags;        // terms base flags
    } data;

#define TERMS_BASE_HEADER_SIZE ((16 * sizeof(uint32_t)) + (TERMS_BASE_ROOTS_NUM * sizeof(uint32_t))) // 1 uint32_t are reserved

public:
    FltTermsBaseHeader() { Clear(); }
    void Clear() { memset(&data, 0, sizeof(data)); }
  
    void Init();

    enum FltTermsErrors Deserialize(fstorage* fs, fstorage_section_id sid);
    enum FltTermsErrors Serialize(fstorage* fs, fstorage_section_id sid) const;
};
//============================================================================//
// Terms base - write mode
//----------------------------------------------------------------------------//
class FltTermsBaseWR
{
    friend class FltTermsBase;
public:
    FltTermsBaseWR() :
        m_lp(NULL), m_tb(NULL), 
        m_roots(500),
	m_index(10),
	m_sets(4),
        m_terms_real_count(0),
        m_terms_extra_count(0),
        m_terms_hier_count(0),
        m_terms_indexed_count(0),
        m_table_stop(),
        m_write_buf(NULL),
        m_write_buf_size(0),
        m_write_alignement(0),
        save_callback_count(0),
        m_status(STATUS_NOT_READY),
        texts_data(1024, 2),
        texts_index(512, 3),
        texts_last_idx(0),
        base_flags(TERMS_BASE_FLAGS_DEFAULT)
    {
        m_roots.SetDelta(TERMS_BASE_ROOTS_NUM);
        m_index.SetDelta(16384);
        m_sets.SetDelta(4096);
        m_terms.SetDelta(16384);
    }
    ~FltTermsBaseWR() { Reset(); }
    void operator=(const class FltTermsBaseWR&) {}

    enum FltTermsErrors Init(class FltTermsBase* tb, LingProc* lp);
    void Reset();

    enum FltTermsErrors Add(const cf_term_attrs* td, const char* text);
    enum FltTermsErrors PrepareSave(fstorage* fs, fstorage_section_id base_sid, struct cf_terms_save_params* params);
  
private:
    FltTermsBaseWR(FltTermsBaseWR&) {}
  
public:
    /* A linked list of lexids */
    struct FltTermsBaseLinkedLexid
    {
        const FltTermsBaseLinkedLexid *prev;
        LEXID                          lex;
    };
  
private:
    /* A context for lemmatization recursive process */
    struct FltTermsBaseLemmatizeContext 
    {
        TERM_HDR       *termHeader;
        bool            isFuzzy;
        bool            forceIndexOnly;
        bool            mainWasAdded;
        FltTermsErrors  warning;
    };
  
    FltTermsErrors imageRecurseProcess( FltTermsBaseLemmatizeContext  &context,
                                        const FltTermsBaseLinkedLexid *prevLexid,
                                        const TermsDocImageIterator   &imageIterator,
                                        bool                           followHierChain,
                                        bool                           recursionRootFlag );
  
    enum FltTermsErrors LemmatizeTerm( TERM_HDR *thdr, const char *txt );
  
    enum FltTermsErrors Add( const FltTermsBaseLinkedLexid *lexList, 
                             const TERM_HDR* thdr, enum FltTermsErrors& warn, bool force_index_only = false );

    enum FltTermsErrors AddToIndex(const LEXID* lex, size_t lex_size,
                                   const TERM_HDR* thdr, size_t term_ref,
                                   LEXID& body_term_ref, enum FltTermsErrors& warn);
    enum FltTermsErrors AddNewTermRef(TERMS_INDEX_REF* ref, unsigned int term_ref);
    LEXID GetTermRefBody(const LEXID* lex, size_t lex_size, const TERM_HDR* thdr, const TERMS_SET* set) const;
    bool CheckRepeatedTerm(const LEXID* new_lex, size_t new_lex_length,
                           const TERM_HDR* new_thdr, TERMS_INDEX_REF ref, enum FltTermsErrors& warn) const;
    bool CheckRepeatedTerm(const LEXID* new_lex, size_t new_lex_length,
                           const TERM_HDR* new_thdr, const TERM* old_term, enum FltTermsErrors& warn) const;

    int CompareSearchAttrs(const LEXID* new_lex, size_t new_lex_length,
                           const TERM_HDR* new_thdr, const TERM* old_term) const;
    int ComparePos(const LEXID* new_lex, size_t new_lex_length,
                   const TERM_HDR* new_thdr, const TERM* old_term) const;
    int CompareWhere(const LEXID* new_lex, size_t new_lex_length,
                     const TERM_HDR* new_thdr, const TERM* old_term) const;
    int CompareMorph(const LEXID* new_lex, size_t new_lex_length,
                     const TERM_HDR* new_thdr, const TERM* old_term) const;
    int CompareCaps(const LEXID* new_lex, size_t new_lex_length,
                    const TERM_HDR* new_thdr, const TERM* old_term) const;

    int CompareWeightAttrs(const LEXID* new_lex, size_t new_lex_length,
                           const TERM_HDR* new_thdr, const TERM* old_term) const;

    const LEXID* GetTermLex(const TERM* term) const; // returns null on error
    size_t GetTermLength(const TERM* term) const;

    // prepare section for saving
    enum FltTermsErrors PrepareSection(fstorage* fs, unsigned int id,
                                       int (*write_func)(struct fstorage_stream_section_config*,
                                                         struct fstorage_section_access_config *));

    friend int on_write_roots(struct fstorage_stream_section_config *sec_config,
                              struct fstorage_section_access_config *ac);
    friend int on_write_index(struct fstorage_stream_section_config *sec_config,
                              struct fstorage_section_access_config *ac);
    friend int on_write_sets(struct fstorage_stream_section_config *sec_config,
                             struct fstorage_section_access_config *ac);
    friend int on_write_terms(struct fstorage_stream_section_config *sec_config,
                              struct fstorage_section_access_config *ac);
    friend int on_write_texts_data(struct fstorage_stream_section_config *sec_config,
                                   struct fstorage_section_access_config *ac);
    friend int on_write_texts_index(struct fstorage_stream_section_config *sec_config,
                                    struct fstorage_section_access_config *ac);

    void CallbackClose();
  
    // prepare data for saving
    enum FltTermsErrors Prepare(struct cf_terms_save_params* params);
    enum FltTermsErrors PrepareData();
    enum FltTermsErrors PrepareIndex(TERMS_INDEX* cur_index);

    // prepare cross references
    enum FltTermsErrors PrepareCrossRefs();
    enum FltTermsErrors PrepareCrossRefs(TERMS_INDEX* index,
                                         FltTermsArray& crlist, size_t crlist_first);
    enum FltTermsErrors PrepareCrossRefs(TERMS_INDEX_ELEMENT* element,
                                         FltTermsArray& crlist, size_t crlist_first);
    enum FltTermsErrors SetCrossRef(TERMS_INDEX_ELEMENT* element,
                                    TERMS_INDEX_ELEMENT* cross_element);
  
    TERMS_INDEX* GetIndexWR(size_t index_no) const // returns null on error
    { return m_index.At(index_no); }

    TERM* GetTermWR(size_t term_no) const // returns null on error
    { return m_terms.At(term_no); }

private:
    // data
    LingProc* m_lp;
    class FltTermsBase* m_tb;

    FltTermsRefArrayIndexes   m_roots;
    FltTermsRefArrayIndexes   m_index;
    FltTermsRefArrayTermSets  m_sets;
    FltTermsRefArrayTerms     m_terms;
    size_t m_terms_real_count;    // number of 'real' terms = last used id
    size_t m_terms_extra_count;   // number of 'extra' terms
    size_t m_terms_hier_count;    // number of 'hier' terms
    size_t m_terms_indexed_count; // number of terms added to index
  
    TermsStopDict m_table_stop;

    char* m_write_buf;
    size_t m_write_buf_size;
    size_t m_write_alignement;

    unsigned int save_callback_count;

    enum WRStatus
    { STATUS_NOT_READY, STATUS_READY, STATUS_PREPARED_WORK, STATUS_PREPARED_TRANSPORT, STATUS_ERROR } m_status;

    DocText  doc_text;
    DocImage doc_image;

    avector<uint8_t>  texts_data;
    avector<uint32_t> texts_index;
    size_t            texts_last_idx;
    uint32_t          base_flags;
};
//============================================================================//
// Terms base - read only mode
//----------------------------------------------------------------------------//
class FltTermsBaseRO
{
public:
    enum ROStatus
    { STATUS_NOT_READY, STATUS_WORK, STATUS_TRANSPORT, STATUS_ERROR };
    enum GetTextMode
    { GT_AUTO, GT_USE_TEXTS, GT_USE_LP };

public:
    FltTermsBaseRO() : m_table_stop(), m_texts_data_enc(0) { Reset(); }
    ~FltTermsBaseRO() { Reset(); }

    enum FltTermsErrors Init(const LingProc* lp, fstorage* fs, fstorage_section_id base_sid);
    void Reset();

public:
    enum FltTermsErrors Enum(cf_terms_enum_callback callback, void* data, bool for_export, bool dump) const;
    void PrintInfo(bool dump_indexes = false, bool dump_terms = false) const;
    //enum FltTermsErrors Check(FltTermsMatcher* matcher, const DocImage* di, bool bNoMarkup=false);
    enum FltTermsErrors Check(FltTermsMatcher* matcher, const DocImage* di, const CDocMessage::CoordMapping* mapping, bool bNoMarkup=false);
    bool CheckIfAllStopWords(const DocImage* di, bool bNoMarkup=false);

    const LEXID* GetTermLex(const TERM* term) const; // returns null on error
    size_t GetTermLength(const TERM* term) const;

    enum ROStatus GetStatus() const { return m_status; }

    // export
    enum FltTermsErrors GetTermText(char** buf, size_t* bufsize, const TERM* term) const;
    enum FltTermsErrors GetTermText( char             **buf,
                                     size_t            *bufsize,
                                     const TERM        *term,
                                     enum GetTextMode   mode ) const;

private:
    // check
    enum FltTermsErrors FillDocImageInfo(const LingProc* lp,
                                         const DocImage* di, const CDocMessage::CoordMapping* mapping, struct terms_di_info* info, bool bNoMarkup=false) const;
    enum FltTermsErrors Lookup(FltTermsMatcher* matcher,
                               struct terms_di_info* di_info, TDATA_FLAGS_TYPE where);
    const TERMS_INDEX* Lookup_Index(FltTermsMatcher* matcher,
                                    const TermsDocImageIterator &it,
                                    const TERMS_INDEX* index, LEXID key,
                                    TDATA_FLAGS_TYPE where, const TERMS_INDEX *suppress_hier = NULL, bool isHomo = false) const;
    int Lookup_GetHomo(const struct terms_di_info* di_info,
                       const        DocImageIterator  &it,
                       const LEXID *&primArray, size_t &primSize,
                       const LEXID *&suppArray, size_t &suppSize, LEXID& hier) const;
    enum FltTermsErrors AddToMatcher( FltTermsMatcher* matcher,
                                      const TermsDocImageIterator &it,
                                      TERMS_INDEX_REF ref, TDATA_FLAGS_TYPE where, bool isHomo ) const;
  
    // dump
    void DumpIndex(const TERMS_INDEX* index) const;

    // data access
    inline const TERMS_INDEX* GetRootIndexLex(LEXID key) const
    { return GetRootIndex(m_hdr.data.roots_ofs[LexNoMrph(key)]); }

    inline const TERMS_INDEX* GetRootIndex(unsigned int ofs) const
    { return (const TERMS_INDEX*)(m_roots_pt + ofs); }
    inline const TERMS_INDEX* GetIndex(unsigned int ofs) const
    { return (const TERMS_INDEX*)(m_index_pt + ofs); }
    inline const TERMS_SET* GetTermSet(unsigned int ofs) const
    { return (const TERMS_SET*)(m_sets_pt + ofs); }
    inline const TERM* GetTerm(unsigned int ofs) const
    { return (const TERM*)(m_terms_pt + ofs); }

private:
    // data
    const LingProc* m_lp;

    FltTermsBaseHeader m_hdr;
    const char* m_roots_pt;
    const char* m_index_pt;
    const char* m_sets_pt;
    const char* m_terms_pt;

    size_t m_roots_size;
    size_t m_index_size;
    size_t m_sets_size;
    size_t m_terms_size;
  
    TermsStopDict m_table_stop;
  
    enum ROStatus m_status;
  
private:
    // terms texts
    const uint8_t *m_texts_data;
    const uint8_t *m_texts_index;
  
    uint8_t       *m_texts_data_enc;
  
    size_t m_texts_data_size;
    size_t m_texts_index_size;
  
private:
    typedef GrowingPool<const TERMS_INDEX *> LookupPositions;

private:
    FltTermsErrors insertUnique(LookupPositions &pool, const TERMS_INDEX *index);
  
private:
    // caches
    int             indexesSwitch;
    LookupPositions primIndexes[2];
    LookupPositions suppIndexes[2];
};
//----------------------------------------------------------------------------//
// doc image info - internal data used in Check()
struct terms_di_info {
    const DocImage* di;
    size_t size;    // number of words in DocImage
    size_t length;  // number of nonstop words in DocImage, calculated during lookup

    enum LangCodes lang_prim;
    enum LangCodes lang_supp;
    bool prim_by_supp;
    bool supp_by_prim;
    bool prim_compound_support;
    bool supp_compound_support;

    const CDocMessage::CoordMapping *mapping;

    LEXID morph_prim; // shifted morph number
    LEXID morph_supp; // shifted morph number
    LEXID morph_prim_strict; // shifted morph number (dictionary morph)
    LEXID morph_supp_strict; // shifted morph number (dictionary morph)

    size_t subj_from;
    size_t subj_to;
    size_t body_from;
    size_t body_to;
};
//============================================================================//
// Terms base - common
//----------------------------------------------------------------------------//
class FltTermsBase
{
    friend class FltTermsBaseWR;
public:
    FltTermsBase(fstorage_section_id sid = FSTORAGE_SECTION_TERMS) :
        m_fs(NULL), base_sid(sid), fs_own(false),
        auto_destroy(false), fs_callback_close(false),
        m_matcher(m_matcher_std),
        m_status(STATUS_NOT_READY)
    { }
    FltTermsBase(FltTermsMatcherClf &_m_matcher, fstorage_section_id sid = FSTORAGE_SECTION_TERMS) :
        m_fs(NULL), base_sid(sid), fs_own(false),
        auto_destroy(false), fs_callback_close(false),
        m_matcher(_m_matcher),
        m_status(STATUS_NOT_READY)
    { }
    ~FltTermsBase();

public:
    // create + save
    enum FltTermsErrors Create(LingProc* lp, fstorage* defaults = NULL);
    enum FltTermsErrors Create(LingProc* lp, const char* defaults_path);
    enum FltTermsErrors Create(LingProc* lp, uint32_t flags);
    enum FltTermsErrors Save(struct cf_terms_save_params* params, const char* path);
    enum FltTermsErrors Save(struct cf_terms_save_params* params, fstorage* fs);

    // open (mode = TERM_BASE_OPEN_CREATE also allowed) + save
    enum FltTermsErrors Open(LingProc* lp, fstorage* fs, enum FltTermsBaseOpenModes mode);
    enum FltTermsErrors Open(LingProc* lp, const char* path, enum FltTermsBaseOpenModes mode);
    enum FltTermsErrors Save(struct cf_terms_save_params* params);

    enum FltTermsErrors Close(); // discards all changes unless Save() was called earlier

    enum FltTermsErrors Add(const cf_term_attrs* attrs, const char* text);
  
    enum FltTermsErrors SetStopWords( const char **words, size_t wordsSize );
    enum FltTermsErrors UnsetStopWords( const char **words, size_t wordsSize );
    enum FltTermsErrors ClearStopWords();
  
    enum FltTermsErrors Enum(cf_terms_enum_callback callback, void* data, bool for_export);
    void PrintInfo(bool dump_indexes = false, bool dump_terms = false)
    { ro_base.PrintInfo(dump_indexes, dump_terms); }

    enum FltTermsErrors Check(const DocImage* di, const CDocMessage::CoordMapping* mapping = NULL);
    enum FltTermsErrors Check(FltTermsMatcher* matcher, const DocImage* di, const CDocMessage::CoordMapping* mapping = NULL);
    bool CheckIfAllStopWords(const DocImage* di);
  
    const FltTermsMatcher* GetMatcher()
    { return &m_matcher; };

    size_t GetLastResults(struct cf_terms_check_result* results, size_t res_size) const
    { return m_matcher.GetResults(results, res_size); }
    size_t GetLastResultsSize() const { return m_matcher.GetResultsSize(); }

    int GetLastFoundTerms(struct cf_terms_check_info* info,
                          struct cf_terms_check_found_term* terms,
                          size_t max_size)
    { return m_matcher.GetFoundTerms(info, terms, max_size); }
    size_t GetLastFoundTermsSize() const { return m_matcher.GetFoundTermsSize(); }

    void Reset(); // low level; do not use directly
    enum FltTermsErrors SetAutoDestroy(); // low level; do not use directly
        
public:
    size_t GetLastAddedTermId() const
    {
        return ( m_status == STATUS_WR ? wr_base.m_terms_real_count : static_cast<size_t>(-1) );
    }
    fstorage_section_id GetBaseSid() const { return base_sid; }
        
private:
    fstorage* m_fs;
    const fstorage_section_id base_sid;
    bool fs_own;

    bool auto_destroy;
    bool fs_callback_close;

    FltTermsBaseWR wr_base;
    FltTermsBaseRO ro_base;
    FltTermsMatcherClf &m_matcher;
    FltTermsMatcherClf m_matcher_std;

    enum Status
    { STATUS_NOT_READY, STATUS_WR, STATUS_RO, STATUS_ERROR } m_status;
};
//============================================================================//
#endif
