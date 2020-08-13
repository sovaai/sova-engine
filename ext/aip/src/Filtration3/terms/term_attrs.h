#ifndef __term_attrs_h__
#define __term_attrs_h__

#include "_include/language.h"
#include "_include/_inttype.h"

/*------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma warning(disable:4127) // conditional expression is constant
#endif
/*------------------------------------------------------------------------------*/
/* TERM ATTRIBUTES DATA STRUCTURE - cf_term_attrs                               */
/*------------------------------------------------------------------------------*/
#ifdef USE_INF_TERMS
  typedef uint16_t cf_term_attrs[9];
#else
  typedef uint16_t cf_term_attrs[4];
#endif

#define TDATA_ATTRS_SIZE (sizeof(cf_term_attrs))

/* Consists of:
 rubric id - uint16_t
 term flags - uint16_t
 term language - uint16_t: 4 bits are reserved

 term compiler attributes - uint8_t
 term weight - uint8_t
*/

#ifdef USE_INF_TERMS
  /* offsets */
  #define TDATA_RID_OFS	  0
  #define TDATA_EID_OFS   2
  #define TDATA_IFL_OFS   4
  #define TDATA_FLG_OFS	  6
  #define TDATA_LNG_OFS	  7
  #define TDATA_BYTES_OFS	8
#else
  /* offsets */
  #define TDATA_RID_OFS	0
  #define TDATA_FLG_OFS	1
  #define TDATA_LNG_OFS	2
  #define TDATA_BYTES_OFS	3
#endif

#define TDATA_BYTES_BASE_OFS (TDATA_BYTES_OFS * sizeof(uint16_t))
#define TDATA_BYTES_CMPL_OFS TDATA_BYTES_BASE_OFS
#define TDATA_BYTES_WGT_OFS  TDATA_BYTES_BASE_OFS + 1

#ifdef USE_INF_TERMS
  /* rubric id - uint32_t */
  #define TDATA_RID_TYPE  uint32_t
  #define TDATA_RID_MAX	0xFFFFFFFF
#else
  /* rubric id - uint16_t */
  #define TDATA_RID_TYPE  uint16_t
  #define TDATA_RID_MAX	0xFFFF
#endif

#ifdef USE_INF_TERMS
  #define TDATA_EID_TYPE  uint32_t
  #define TDATA_EID_MAX 0xFFFFFFFF

  #define TDATA_IFL_TYPE  uint32_t
  #define TDATA_IFL_MAX 0xFFFFFFFF
#endif

/* term flags - uint16_t */
#define TDATA_FLAGS_TYPE uint16_t
/* term type - two bits are reserved */
#define TDATA_TYPE_STRICT  0x1000
#define TDATA_TYPE_SUPPL   0x2000
#define TDATA_TYPE_MASK    (TDATA_TYPE_STRICT | TDATA_TYPE_SUPPL)

/* where - two values are reserved;
   these two bits can't be used for other attributes */
#define TDATA_WHERE_BODY   0x0100
#define TDATA_WHERE_SUBJ   0x0200
#define TDATA_WHERE_ALL    0x0F00
#define TDATA_WHERE_MASK   0x0F00

/* morph - two bits are reserved */
#define TDATA_MORPH_FUZZY  0x0010
#define TDATA_MORPH_DICT   0x0020
#define TDATA_MORPH_ANY    (TDATA_MORPH_FUZZY | TDATA_MORPH_DICT)
#define TDATA_MORPH_MASK   (TDATA_MORPH_FUZZY | TDATA_MORPH_DICT)

/* term position */
#define TDATA_POSITION_LEFTMOST  0x0040
#define TDATA_POSITION_RIGHTMOST 0x0080
#define TDATA_POSITION_MATCH     (TDATA_POSITION_LEFTMOST | TDATA_POSITION_RIGHTMOST)
#define TDATA_POSITION_MASK      (TDATA_POSITION_LEFTMOST | TDATA_POSITION_RIGHTMOST)

/* capitalization - one bit is reserved */
#define TDATA_CAPS_MASK    0x0007 /* see enum FltTermCaps */

/* term language - uint16_t */
#define TDATA_LANG_TYPE	  uint16_t

/* term compiler attributes - uint8_t */
#define TDATA_CMPL_TYPE	uint8_t

/* term weight - uint8_t */
#define TDATA_WGT_TYPE	uint8_t
#define TDATA_WGT_MAX   0xFF

/*------------------------------------------------------------------------------*/
/* capitalization enum */
enum FltTermCaps
{
  CAPS_ANY, CAPS_NATIVE, CAPS_FIRST, CAPS_WORD, CAPS_ALL, CAPS_NO
};

/*------------------------------------------------------------------------------*/
/* TERM ATTRIBUTES DATA STRUCTURE - ACCESS FUNCTIONS to cf_term_attrs           */
/*------------------------------------------------------------------------------*/
/* common defines - DO NOT USE DIRECTLY */
#define __term_attrs_size() (TDATA_ATTRS_SIZE)

//#define __term_rubric_id(attrs) (*(((uint16_t*)attrs) + TDATA_RID_OFS))
#define __term_rubric_id(attrs) (*((TDATA_RID_TYPE*)(((uint16_t*)attrs) + TDATA_RID_OFS)))
#define __set_term_rubric_id(attrs, rid) __term_rubric_id(attrs) = (TDATA_RID_TYPE)rid

#ifdef USE_INF_TERMS
  #define __term_ext_id(attrs) (*((TDATA_EID_TYPE*)(((uint16_t*)attrs) + TDATA_EID_OFS)))
  #define __set_term_ext_id(attrs, eid) __term_ext_id(attrs) = (TDATA_EID_TYPE)eid

  #define __term_ifl(attrs) (*((TDATA_IFL_TYPE*)(((uint16_t*)attrs) + TDATA_IFL_OFS)))
  #define __set_term_ifl(attrs, ifl) __term_ifl(attrs) = (TDATA_IFL_TYPE)ifl
#endif

#define __term_flags(attrs) (*(((uint16_t*)attrs) + TDATA_FLG_OFS))

#define __term_is_strict(attrs)   (__term_flags(attrs) & TDATA_TYPE_STRICT)
#define __term_is_suppl(attrs)    (__term_flags(attrs) & TDATA_TYPE_SUPPL)
#define __term_type(attrs)        ((TDATA_FLAGS_TYPE)(__term_flags(attrs) & TDATA_TYPE_MASK))

#define __set_term_strict(attrs)    (__term_flags(attrs) |= TDATA_TYPE_STRICT)
#define __set_term_suppl(attrs)     (__term_flags(attrs) |= TDATA_TYPE_SUPPL)

#define __clear_term_strict(attrs)  (__term_flags(attrs) &= ~TDATA_TYPE_STRICT)
#define __clear_term_suppl(attrs)   (__term_flags(attrs) &= ~TDATA_TYPE_SUPPL)

#define __term_is_where_body(attrs)   (__term_flags(attrs) & TDATA_WHERE_BODY)
#define __term_is_where_subj(attrs)   (__term_flags(attrs) & TDATA_WHERE_SUBJ)
#define __term_is_where_all(attrs)    ((__term_flags(attrs) & TDATA_WHERE_MASK) == TDATA_WHERE_ALL)
#define __term_where(attrs)           ((TDATA_FLAGS_TYPE)(__term_flags(attrs) & TDATA_WHERE_MASK))

#define __set_term_where_body(attrs)  (__term_flags(attrs) = (TDATA_FLAGS_TYPE)((__term_flags(attrs) & ~TDATA_WHERE_MASK) | TDATA_WHERE_BODY))
#define __set_term_where_subj(attrs)  (__term_flags(attrs) = (TDATA_FLAGS_TYPE)((__term_flags(attrs) & ~TDATA_WHERE_MASK) | TDATA_WHERE_SUBJ))
#define __set_term_where_all(attrs)   (__term_flags(attrs) |= TDATA_WHERE_ALL)

#define __term_is_morph_fuzzy(attrs)  (__term_flags(attrs) & TDATA_MORPH_FUZZY)
#define __term_is_morph_dict(attrs)   (__term_flags(attrs) & TDATA_MORPH_DICT)
#define __term_is_morph_any(attrs)    ((__term_flags(attrs) & TDATA_MORPH_ANY) == TDATA_MORPH_ANY)
#define __term_is_morph_none(attrs)   ((__term_flags(attrs) & TDATA_MORPH_ANY) == 0)
#define __term_morph(attrs)           ((TDATA_FLAGS_TYPE)(__term_flags(attrs) & TDATA_MORPH_MASK))

#define __set_term_morph_fuzzy(attrs) (__term_flags(attrs) = (TDATA_FLAGS_TYPE)((__term_flags(attrs) & ~TDATA_MORPH_MASK) | TDATA_MORPH_FUZZY))
#define __set_term_morph_dict(attrs)  (__term_flags(attrs) = (TDATA_FLAGS_TYPE)((__term_flags(attrs) & ~TDATA_MORPH_MASK) | TDATA_MORPH_DICT))
#define __set_term_morph_any(attrs)   (__term_flags(attrs) |= TDATA_MORPH_ANY)
#define __set_term_morph_none(attrs)  (__term_flags(attrs) &= ~TDATA_MORPH_ANY)

#define __term_is_position_leftmost(attrs)  (__term_flags(attrs) & TDATA_POSITION_LEFTMOST)
#define __term_is_position_rightmost(attrs)  (__term_flags(attrs) & TDATA_POSITION_RIGHTMOST)
#define __term_is_position_match(attrs) ((__term_flags(attrs) & TDATA_POSITION_MATCH) == TDATA_POSITION_MATCH)
#define __term_is_position_any(attrs)   ((__term_flags(attrs) & TDATA_POSITION_MATCH) == 0)

#define __set_term_position_leftmost(attrs)  (__term_flags(attrs) = (TDATA_FLAGS_TYPE)((__term_flags(attrs) & ~TDATA_POSITION_MASK) | TDATA_POSITION_LEFTMOST))
#define __set_term_position_rightmost(attrs) (__term_flags(attrs) = (TDATA_FLAGS_TYPE)((__term_flags(attrs) & ~TDATA_POSITION_MASK) | TDATA_POSITION_RIGHTMOST))
#define __set_term_position_match(attrs)     (__term_flags(attrs) = (TDATA_FLAGS_TYPE)((__term_flags(attrs) & ~TDATA_POSITION_MASK) | TDATA_POSITION_MATCH))
#define __set_term_position_any(attrs)       (__term_flags(attrs) = (TDATA_FLAGS_TYPE)((__term_flags(attrs) & ~TDATA_POSITION_MASK)))

#define __term_caps(attrs)            ((enum FltTermCaps)(__term_flags(attrs) & TDATA_CAPS_MASK))
#define __set_term_caps(attrs, caps) \
        __term_flags(attrs) = (TDATA_FLAGS_TYPE)((__term_flags(attrs) & ~TDATA_CAPS_MASK) | (caps & TDATA_CAPS_MASK))

#define __term_lng(attrs)            (*(((uint16_t*)attrs) + TDATA_LNG_OFS))
#define __term_lang(attrs)           ((enum LangCodes)__term_lng(attrs))
#define __set_term_lang(attrs, lang)  __term_lng(attrs) = (TDATA_LANG_TYPE)lang

#define __term_cmpl_attrs(attrs)      (*(((uint8_t*)attrs) + TDATA_BYTES_CMPL_OFS))
#define __set_term_cmpl_attrs(attrs, cmpl_attribs) \
        __term_cmpl_attrs(attrs) = (TDATA_CMPL_TYPE)cmpl_attribs
#define __clear_term_cmpl_attrs(attrs) \
        __term_cmpl_attrs(attrs) = (TDATA_CMPL_TYPE)0

#define __check_term_cmpl_attr(attrs, cmpl_attrib_mask)   (__term_cmpl_attrs(attrs) & cmpl_attrib_mask)
#define __add_term_cmpl_attr(attrs, cmpl_attrib_mask)                        \
        __term_cmpl_attrs(attrs) |= (TDATA_CMPL_TYPE)cmpl_attrib_mask
#define __remove_term_cmpl_attr(attrs, cmpl_attrib_mask)                        \
        __term_cmpl_attrs(attrs) &= ~(TDATA_CMPL_TYPE)cmpl_attrib_mask

#define __term_weight(attrs)      (*(((uint8_t*)attrs) + TDATA_BYTES_WGT_OFS))
#define __set_term_weight(attrs, weight)  __term_weight(attrs) = (TDATA_WGT_TYPE)weight

/* C-style usage */
#ifndef __cplusplus
  #define term_attrs_size()               __term_attrs_size()

  #define term_rubric_id(attrs)           __term_rubric_id(attrs)
  #define set_term_rubric_id(attrs, rid)  __set_term_rubric_id(attrs, rid)

  #ifdef USE_INF_TERMS
    #define term_ext_id(attrs)           __term_ext_id(attrs)
    #define set_term_ext_id(attrs, rid)  __set_term_ext_id(attrs, rid)

    #define term_ifl(attrs)           __term_ifl(attrs)
    #define set_term_ifl(attrs, rid)  __set_term_ifl(attrs, rid)
  #endif

  #define term_is_strict(attrs)           __term_is_strict(attrs)
  #define term_is_suppl(attrs)            __term_is_suppl(attrs)
  #define term_type(attrs)                __term_type(attrs)

  #define set_term_strict(attrs)          __set_term_strict(attrs)
  #define set_term_suppl(attrs)           __set_term_suppl(attrs)

  #define clear_term_strict(attrs)        __clear_term_strict(attrs)
  #define clear_term_suppl(attrs)         __clear_term_suppl(attrs)

  #define term_is_body(attrs)             __term_is_body(attrs)
  #define term_is_subj(attrs)             __term_is_subj(attrs)
  #define term_is_where_all(attrs)        __term_is_where_all(attrs)
  #define term_where(attrs)               __term_where(attrs)

  #define set_term_where_body(attrs)      __set_term_where_body(attrs)
  #define set_term_where_subj(attrs)      __set_term_where_subj(attrs)
  #define set_term_where_all(attrs)       __set_term_where_all(attrs)

  #define term_is_morph_fuzzy(attrs)      __term_is_morph_fuzzy(attrs)
  #define term_is_morph_dict(attrs)       __term_is_morph_dict(attrs)
  #define term_is_morph_any(attrs)        __term_is_morph_any(attrs)
  #define term_is_morph_none(attrs)       __term_is_morph_none(attrs)
  #define term_morph(attrs)               __term_morph(attrs)

  #define set_term_morph_fuzzy(attrs)     __set_term_morph_fuzzy(attrs)
  #define set_term_morph_dict(attrs)      __set_term_morph_dict(attrs)
  #define set_term_morph_any(attrs)       __set_term_morph_any(attrs)
  #define set_term_morph_none(attrs)      __set_term_morph_none(attrs)

  #define term_is_position_leftmost(attrs)   __term_is_position_leftmost(attrs)
  #define term_is_position_rightmost(attrs)  __term_is_position_rightmost(attrs)
  #define term_is_position_match(attrs)      __term_is_position_match(attrs)
  #define term_is_position_any(attrs)        __term_is_position_any(attrs)

  #define set_term_position_leftmost(attrs)  __set_term_position_leftmost(attrs)
  #define set_term_position_rightmost(attrs) __set_term_position_rightmost(attrs)
  #define set_term_position_match(attrs)     __set_term_position_match(attrs)
  #define set_term_position_any(attrs)       __set_term_position_any(attrs)

  #define term_caps(attrs)                __term_caps(attrs)
  #define set_term_caps(attrs, caps)      __set_term_caps(attrs, caps)

  #define term_lang(attrs)                __term_lang(attrs)
  #define set_term_lang(attrs, lang)      __set_term_lang(attrs, lang)

  #define term_cmpl_attrs(attrs)            __term_cmpl_attrs(attrs)
  #define set_term_cmpl_attrs(attrs, cmpl)  __set_term_cmpl_attrs(attrs, cmpl)
  #define clear_term_cmpl_attrs(attrs)      __clear_term_cmpl_attrs(attrs)

  #define check_term_cmpl_attr(attrs, mask)   __check_term_cmpl_attr(attrs, mask)
  #define add_term_cmpl_attr(attrs, mask)     __add_term_cmpl_attr(attrs, mask)
  #define remove_term_cmpl_attr(attrs, mask)  __remove_term_cmpl_attr(attrs, mask)

  #define term_weight(attrs)              __term_weight(attrs)
  #define set_term_weight(attrs, wgt)     __set_term_weight(attrs, wgt)

  #define term_attrs_ntoh(dest, src)      __term_attrs_ntoh(dest, src)
  #define term_attrs_hton(dest, src)      __term_attrs_hton(dest, src)
#endif

/* CPP-style usage */
#ifdef __cplusplus
  inline size_t term_attrs_size()
    { return __term_attrs_size(); }

  inline TDATA_RID_TYPE term_rubric_id(const cf_term_attrs* attrs)
    { return __term_rubric_id(attrs); }
  inline void set_term_rubric_id(cf_term_attrs* attrs, TDATA_RID_TYPE rid)
    { __set_term_rubric_id(attrs, rid);}

  #ifdef USE_INF_TERMS
    inline TDATA_EID_TYPE term_ext_id(const cf_term_attrs* attrs)
      { return __term_ext_id(attrs); }
    inline void set_term_ext_id(cf_term_attrs* attrs, TDATA_EID_TYPE eid)
      { __set_term_ext_id(attrs, eid);}

    inline TDATA_IFL_TYPE term_ifl(const cf_term_attrs* attrs)
      { return __term_ifl(attrs); }
    inline void set_term_ifl(cf_term_attrs* attrs, TDATA_IFL_TYPE ifl)
      { __set_term_ifl(attrs, ifl);}
  #endif

  inline int term_is_strict(const cf_term_attrs* attrs)
    { return __term_is_strict(attrs);}
  inline int term_is_suppl(const cf_term_attrs* attrs)
    { return __term_is_suppl(attrs); }
  inline TDATA_FLAGS_TYPE term_type(const cf_term_attrs* attrs)
    { return __term_type(attrs); }

  inline void set_term_strict(cf_term_attrs* attrs)
    { __set_term_strict(attrs); }
  inline void set_term_suppl(cf_term_attrs* attrs)
    { __set_term_suppl(attrs); }

  inline void clear_term_strict(cf_term_attrs* attrs)
    { __clear_term_strict(attrs); }
  inline void clear_term_suppl(cf_term_attrs* attrs)
    { __clear_term_suppl(attrs); }

  inline int term_is_where_body(const cf_term_attrs* attrs)
    { return __term_is_where_body(attrs); }
  inline int term_is_where_subj(const cf_term_attrs* attrs)
    { return __term_is_where_subj(attrs); }
  inline int term_is_where_all(const cf_term_attrs* attrs)
    { return __term_is_where_all(attrs); }
  inline TDATA_FLAGS_TYPE term_where(const cf_term_attrs* attrs)
    { return __term_where(attrs); }

  inline void set_term_where_body(cf_term_attrs* attrs)
    { __set_term_where_body(attrs); }
  inline void set_term_where_subj(cf_term_attrs* attrs)
    { __set_term_where_subj(attrs); }
  inline void set_term_where_all(cf_term_attrs* attrs)
    { __set_term_where_all(attrs); }

  inline int term_is_morph_fuzzy(const cf_term_attrs* attrs)
    { return __term_is_morph_fuzzy(attrs); }
  inline int term_is_morph_dict(const cf_term_attrs* attrs)
    { return __term_is_morph_dict(attrs); }
  inline int term_is_morph_any(const cf_term_attrs* attrs)
    { return __term_is_morph_any(attrs); }
  inline int term_is_morph_none(const cf_term_attrs* attrs)
    { return __term_is_morph_none(attrs); }
  inline TDATA_FLAGS_TYPE term_morph(const cf_term_attrs* attrs)
    { return __term_morph(attrs); }

  inline void set_term_morph_fuzzy(cf_term_attrs* attrs)
    { __set_term_morph_fuzzy(attrs); }
  inline void set_term_morph_dict(cf_term_attrs* attrs)
    { __set_term_morph_dict(attrs); }
  inline void set_term_morph_any(cf_term_attrs* attrs)
    { __set_term_morph_any(attrs); }
  inline void set_term_morph_none(cf_term_attrs* attrs)
    { __set_term_morph_none(attrs); }

  inline int term_is_position_leftmost(const cf_term_attrs* attrs)
    { return __term_is_position_leftmost(attrs); }
  inline int term_is_position_rightmost(const cf_term_attrs* attrs)
    { return __term_is_position_rightmost(attrs); }
  inline int term_is_position_match(const cf_term_attrs* attrs)
    { return __term_is_position_match(attrs); }
  inline int term_is_position_any(const cf_term_attrs* attrs)
    { return __term_is_position_any(attrs); }

  inline void set_term_position_leftmost(cf_term_attrs* attrs)
    { __set_term_position_leftmost(attrs); }
  inline void set_term_position_rightmost(cf_term_attrs* attrs)
    { __set_term_position_rightmost(attrs); }
  inline void set_term_position_match(cf_term_attrs* attrs)
    { __set_term_position_match(attrs); }
  inline void set_term_position_any(cf_term_attrs* attrs)
    { __set_term_position_any(attrs); }

  inline enum FltTermCaps term_caps(const cf_term_attrs* attrs)
    { return __term_caps(attrs); }
  inline void set_term_caps(cf_term_attrs* attrs, enum FltTermCaps caps)
    { __set_term_caps(attrs, caps); }

  inline enum LangCodes term_lang(const cf_term_attrs* attrs)
    { return __term_lang(attrs); }
  inline void set_term_lang(cf_term_attrs* attrs, enum LangCodes lang)
    { __set_term_lang(attrs, lang); }

  inline TDATA_CMPL_TYPE term_cmpl_attrs(const cf_term_attrs* attrs)
    { return __term_cmpl_attrs(attrs); }
  inline void set_term_cmpl_attrs(cf_term_attrs* attrs, TDATA_CMPL_TYPE cmpl_attribs)
    { __set_term_cmpl_attrs(attrs, cmpl_attribs); }
  inline void clear_term_cmpl_attrs(cf_term_attrs* attrs)
    { __clear_term_cmpl_attrs(attrs); }

  inline int check_term_cmpl_attr(const cf_term_attrs* attrs, TDATA_CMPL_TYPE cmpl_attrib_mask)
    { return __check_term_cmpl_attr(attrs, cmpl_attrib_mask); }
  inline void add_term_cmpl_attr(cf_term_attrs* attrs, TDATA_CMPL_TYPE cmpl_attrib_mask)
    { __add_term_cmpl_attr(attrs, cmpl_attrib_mask); }
  inline void remove_term_cmpl_attr(cf_term_attrs* attrs, TDATA_CMPL_TYPE cmpl_attrib_mask)
    { __remove_term_cmpl_attr(attrs, cmpl_attrib_mask); }

  inline TDATA_WGT_TYPE term_weight(const cf_term_attrs* attrs)
    { return __term_weight(attrs); }
  inline void set_term_weight(cf_term_attrs* attrs, TDATA_WGT_TYPE weight)
    { __set_term_weight(attrs, weight); }

#endif
/*------------------------------------------------------------------------------*/
#endif
