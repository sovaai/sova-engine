#ifndef __FltTerm_h__
#define __FltTerm_h__
//============================================================================//
#ifdef _MSC_VER
#pragma warning(error:4706) // assignment within conditional expression
#endif
//----------------------------------------------------------------------------//
#include <_include/_inttype.h>
#include "LingProc4/LexID.h"
#include "terms.h"
//============================================================================//
// TERM STRUCTURE
//----------------------------------------------------------------------------//
typedef struct __term TERM;
/* Consisits of:
  TERM_LEN length;
  TERM_HDR hdr;
  LEXID lex[length];
*/

/* Term Length - number of lexems in term */
typedef uint32_t TERM_LEN;

#define TERM_MAX_LENGTH 255 // used to allocate temporary buffers

/* Term Header */
typedef struct __term_hdr TERM_HDR;
#define TERM_HDR_SIZE (sizeof(THDR_IDN) + TDATA_ATTRS_SIZE)

/* Consisits of:
  THDR_IDN idn;
  cf_term_attrs attrs; // see term_data.h
*/

/* term id - uint32_t: type(1 byte) + id(3 bytes) */
#define THDR_IDN uint32_t
  #define THDR_FLAG_HIER    0x10000000
  #define THDR_FLAG_EXTRA   0x20000000
  #define THDR_FLAG_REFBODY 0x40000000
  #define THDR_FLAG_MASK    (THDR_FLAG_HIER | THDR_FLAG_EXTRA)

  #define THDR_IDN_MASK	    0x00FFFFFF

// FUNCTIONS
/* length, size */
inline size_t term_lex_len(const TERM* term) // note: for terms with ref body is 1
  { return *((const TERM_LEN*)term); }

inline size_t term_full_size(const TERM* term)
  { return sizeof(TERM_LEN) + TERM_HDR_SIZE + term_lex_len(term)*sizeof(LEXID); }

/* term's elements */
inline const TERM_HDR* term_header(const TERM* term)
  { return (const TERM_HDR*)(((const char*)term)+sizeof(TERM_LEN)); }
inline TERM_HDR* term_header(TERM* term)
  { return (TERM_HDR*)(((char*)term)+sizeof(TERM_LEN)); }

inline const cf_term_attrs* term_attrs(const TERM* term)
  { return (const cf_term_attrs*)(((const char*)term)+sizeof(TERM_LEN)+sizeof(THDR_IDN)); }

inline const LEXID* term_ptlex(const TERM* term)
  { return (const LEXID*)(((const char*)term)+sizeof(TERM_LEN)+TERM_HDR_SIZE);}
inline LEXID* term_ptlex(TERM* term)
  { return (LEXID*)(((const char*)term)+sizeof(TERM_LEN)+TERM_HDR_SIZE);}

/* term's header - id */
inline uint32_t term_id(const TERM_HDR* thdr)
  { return (*((const THDR_IDN*)thdr)) & THDR_IDN_MASK; }
inline void set_term_id(TERM_HDR* thdr, uint32_t value)
  { *((THDR_IDN*)thdr) = ((*((THDR_IDN*)thdr)) & ~THDR_IDN_MASK) | (value & THDR_IDN_MASK); }

inline uint32_t term_id(const TERM* term)
  { return term_id(term_header(term)); }

/* term's header - term type */
inline bool term_is_hier(const TERM_HDR* thdr)
  { return ((*((const THDR_IDN*)thdr)) & THDR_FLAG_HIER) != 0; }
inline void set_term_hier(TERM_HDR* thdr)
  { (*((THDR_IDN*)thdr)) |= THDR_FLAG_HIER; }

inline bool term_is_extra(const TERM_HDR* thdr)
  { return ((*((const THDR_IDN*)thdr)) & THDR_FLAG_EXTRA) != 0; }
inline void set_term_extra(TERM_HDR* thdr)
  { (*((THDR_IDN*)thdr)) |= THDR_FLAG_EXTRA; }

inline bool term_is_refbody(const TERM_HDR* thdr)
  { return ((*((const THDR_IDN*)thdr)) & THDR_FLAG_REFBODY) != 0; }
inline void set_term_refbody(TERM_HDR* thdr)
  { (*((THDR_IDN*)thdr)) |= THDR_FLAG_REFBODY; }
inline void clear_term_refbody(TERM_HDR* thdr)
  { (*((THDR_IDN*)thdr)) &= ~THDR_FLAG_REFBODY; }

/* term's header - attrs */
inline const cf_term_attrs* term_attrs(const TERM_HDR* thdr)
  { return (const cf_term_attrs*)(((const char*)thdr)+sizeof(THDR_IDN)); }
inline cf_term_attrs* term_attrs(TERM_HDR* thdr)
  { return (cf_term_attrs*)(((char*)thdr)+sizeof(THDR_IDN)); }

/* byte order conversion */
void term_header_hton(TERM_HDR* thdr);
void term_header_ntoh(TERM_HDR* thdr);
//============================================================================//
// TERM SET
//----------------------------------------------------------------------------//
typedef struct __term_set TERMS_SET;
/* Contains:
    TERMS_SET_LEN length;
    TERMS_SET_REF ref[length];
*/

typedef uint32_t TERMS_SET_LEN;
typedef uint32_t TERMS_SET_REF;

// FUNCTIONS
inline TERMS_SET_LEN term_set_length(const TERMS_SET* set)
  { return *((const TERMS_SET_LEN*)set); }
inline const TERMS_SET_REF* term_set_refs(const TERMS_SET* set)
  { return (const TERMS_SET_REF*)(((const char*)set)+sizeof(TERMS_SET_LEN));}
inline TERMS_SET_REF* term_set_refs(TERMS_SET* set)
  { return (TERMS_SET_REF*)(((char*)set)+sizeof(TERMS_SET_LEN));}

//============================================================================//
// INDEX
//----------------------------------------------------------------------------//
/* Index ref */
// index in ref array (write) or ofs (read), first 4 bits = type
typedef uint32_t TERMS_INDEX_REF;

#define TIDX_REF_TYPE_INDEX     0x10000000
#define TIDX_REF_TYPE_TERM      0x20000000
#define TIDX_REF_TYPE_TERMSET   0x40000000
#define TIDX_REF_TYPE_FINAL     (TIDX_REF_TYPE_TERM | TIDX_REF_TYPE_TERMSET)
#define TIDX_REF_TYPE_MASK      0xF0000000
#define TIDX_REF_VALUE_MASK     0x0FFFFFFF
#define TIDX_REF_VALUE_MAX      0x0FFFFFFF

// FUNCTIONS
inline bool tidx_is_ref(const TERMS_INDEX_REF ref)
  { return (ref & TIDX_REF_TYPE_MASK) != 0; }
inline bool tidx_is_empty_ref(const TERMS_INDEX_REF ref)
  { return ref == 0; }

inline bool tidx_is_index_ref(const TERMS_INDEX_REF ref)
  { return (ref & TIDX_REF_TYPE_INDEX) != 0; }
inline bool tidx_is_term_ref(const TERMS_INDEX_REF ref)
  { return (ref & TIDX_REF_TYPE_TERM) != 0; }
inline bool tidx_is_set_ref(const TERMS_INDEX_REF ref)
  { return (ref & TIDX_REF_TYPE_TERMSET) != 0; }
inline bool tidx_is_final_ref(const TERMS_INDEX_REF ref)
  { return (ref & TIDX_REF_TYPE_FINAL) != 0; }

inline uint32_t tidx_ref_value(const TERMS_INDEX_REF ref)
  { return ref & TIDX_REF_VALUE_MASK; }

inline TERMS_INDEX_REF tidx_ref_empty()
  { return 0; }
inline TERMS_INDEX_REF tidx_ref_index(uint32_t value)
  { return value | TIDX_REF_TYPE_INDEX; }
inline TERMS_INDEX_REF tidx_ref_term(uint32_t value)
  { return value | TIDX_REF_TYPE_TERM; }
inline TERMS_INDEX_REF tidx_ref_set(uint32_t value)
  { return value | TIDX_REF_TYPE_TERMSET; }

//----------------------------------------------------------------------------//
/* Index element */
typedef struct __term_index_element TERMS_INDEX_ELEMENT;
#define TERMS_INDEX_ELEMENT_SIZE (sizeof(LEXID)+sizeof(TERMS_INDEX_REF))
/* Contains:
  LEXID lex;
  TERMS_INDEX_REF ref;
*/

inline LEXID tidx_lex(const TERMS_INDEX_ELEMENT* element)
  { return *(const LEXID*)element;}
inline void set_tidx_lex(TERMS_INDEX_ELEMENT* element, LEXID lex)
  { *(LEXID*)element = lex;}

inline TERMS_INDEX_REF tidx_ref(const TERMS_INDEX_ELEMENT* element)
  { return *(const TERMS_INDEX_REF*)((const char*)element + sizeof(LEXID));}
inline void set_tidx_ref(TERMS_INDEX_ELEMENT* element, TERMS_INDEX_REF ref)
  { *(TERMS_INDEX_REF*)((char*)element + sizeof(LEXID)) = ref; }

//----------------------------------------------------------------------------//
/* Index header */

typedef struct __term_index_header TERMS_INDEX_HEADER;
#define TERMS_INDEX_HEADER_SIZE (sizeof(TERMS_INDEX_REF) * 2)
/* Contains:
  TERMS_INDEX_REF ref;    // reference to term
  TERMS_INDEX_REF cross;  // reference to tree node
*/

inline TERMS_INDEX_REF tidx_ref(const TERMS_INDEX_HEADER* ihdr)
  { return *(const TERMS_INDEX_REF*)ihdr;}
inline void set_tidx_ref(TERMS_INDEX_HEADER* ihdr, TERMS_INDEX_REF ref)
  { *(TERMS_INDEX_REF*)ihdr = ref; }

inline TERMS_INDEX_REF tidx_cross(const TERMS_INDEX_HEADER* ihdr)
  { return *(const TERMS_INDEX_REF*)((const char*)ihdr + sizeof(TERMS_INDEX_REF));}
inline void set_tidx_cross(TERMS_INDEX_HEADER* ihdr, TERMS_INDEX_REF ref)
  { *(TERMS_INDEX_REF*)((char*)ihdr + sizeof(TERMS_INDEX_REF)) = ref; }

//----------------------------------------------------------------------------//
/* Index */

typedef struct __terms_index TERMS_INDEX;
/* Contains:
    TERMS_INDEX_LEN length;
    TERMS_INDEX_HEADER  hdr;
    TERMS_INDEX_ELEMENT array[length];
*/

typedef uint32_t TERMS_INDEX_LEN;

// FUNCTIONS
// size and pointers
inline TERMS_INDEX_LEN tidx_length(const TERMS_INDEX* index)
  { return *((TERMS_INDEX_LEN*)index); }

inline size_t index_full_size(const TERMS_INDEX* index)
  { return sizeof(TERMS_INDEX_LEN)+TERMS_INDEX_HEADER_SIZE+TERMS_INDEX_ELEMENT_SIZE*tidx_length(index); }

inline const TERMS_INDEX_HEADER* tidx_header(const TERMS_INDEX* index)
  { return (const TERMS_INDEX_HEADER*)(((const char*)index)+sizeof(TERMS_INDEX_LEN)); }
inline TERMS_INDEX_HEADER* tidx_header(TERMS_INDEX* index)
  { return (TERMS_INDEX_HEADER*)(((char*)index)+sizeof(TERMS_INDEX_LEN)); }

inline const TERMS_INDEX_ELEMENT* tidx_array(const TERMS_INDEX* index)
  { return (const TERMS_INDEX_ELEMENT*)(((const char*)index)+sizeof(TERMS_INDEX_LEN)+TERMS_INDEX_HEADER_SIZE);}
inline TERMS_INDEX_ELEMENT* tidx_array(TERMS_INDEX* index)
  { return (TERMS_INDEX_ELEMENT*)(((char*)index)+sizeof(TERMS_INDEX_LEN)+TERMS_INDEX_HEADER_SIZE);}

inline const TERMS_INDEX_ELEMENT* tidx_element(const TERMS_INDEX_ELEMENT* array, size_t n)
  { return (const TERMS_INDEX_ELEMENT*)(((const char*)array) + TERMS_INDEX_ELEMENT_SIZE*n);}
inline TERMS_INDEX_ELEMENT* tidx_element(TERMS_INDEX_ELEMENT* array, size_t n)
  { return (TERMS_INDEX_ELEMENT*)(((char*)array) + TERMS_INDEX_ELEMENT_SIZE*n);}

// lookup
bool tidx_lookup(const TERMS_INDEX* index, LEXID key,
		      size_t &found, const TERMS_INDEX_ELEMENT* &found_element);
//============================================================================//
#endif
