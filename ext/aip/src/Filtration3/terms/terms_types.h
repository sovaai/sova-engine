/*******************************************************************************
 * Proj: Content Filtration Library, version 2                                 *
 * --------------------------------------------------------------------------- *
 * File: terms_types.h                                                         *
 * Created: Thu Feb 9 11:17:45 2005                                            *
 * Desc: General types for terms library                                       *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  terms_types.h
 * \brief General types for terms library
 */

#ifndef __terms_types_h__
#define __terms_types_h__

#include <sys/types.h>
#include "term_attrs.h"

/*------------------------------------------------------------------------------*/
/* Terms Check Results                                                          */
/*------------------------------------------------------------------------------*/
struct cf_terms_check_result
{
  unsigned int NCH;  /* number of strict terms; */
  double WMAIN; /* main weight in this category; */
  double WSUPP; /* supplemental weigth in this category; */
  TDATA_RID_TYPE RID;  /* category (rubric) ID; */
};

struct cf_terms_check_found_term_coords
{
  size_t term_w_from;
  size_t term_w_to;

  size_t term_c_from;
  size_t term_c_to;
};

struct cf_terms_check_found_term
{
  char text[512];   //TODO: increase length?
  size_t len;
  cf_term_attrs attrs;

  size_t count;
  struct cf_terms_check_found_term_coords coords;
  const struct cf_terms_check_found_term_coords* more_coords;
};

struct cf_terms_check_info
{
  size_t nMsgWords;   /* number of words in the message */
  enum LangCodes wLangPrim;     /* first detected language */
  enum LangCodes wLangSupp;     /* second detected language */
};

/*------------------------------------------------------------------------------*/
/* Error Codes                                                                  */
/*------------------------------------------------------------------------------*/
enum FltTermsErrors
{
  TERMS_OK                         = 0,      /* no error */
  TERMS_ERROR_ENOTIMPL             = -1,     /* not implemented */
  TERMS_ERROR_BREAK                = -4,     /* break iteration */
  TERMS_ERROR_ENOMEM               = -12,    /* not enough memory */
  TERMS_ERROR_EACCES               = -13,    /* permission denied */
  TERMS_ERROR_EFAULT               = -14,    /* unknown fault */
  TERMS_ERROR_EINVAL               = -22,    /* invalid argument */
  TERMS_ERROR_ERANGE               = -34,    /* data is out of range */
  TERMS_ERROR_EMPTY                = -35,    /* dataset is empty */
  TERMS_ERROR_INVALID_MODE         = -100,   /* invalid operation for current mode */
  TERMS_ERROR_INVALID_FORMAT       = -101,   /* invalid data format */
  TERMS_ERROR_LP_MORPH             = -102,   /* invalid morph results */
  TERMS_ERROR_LP_FAILED            = -103,   /* error while using lingproc */
  TERMS_ERROR_FS_FAILED            = -104,   /* error while using fstorage */

  TERMS_ERROR_LP_ENOTIMPL          = -200, /* not implemented */
  TERMS_ERROR_LP_ENOMEM            = -201, /* not enough memory */
  TERMS_ERROR_LP_EFAULT            = -202, /* unknown fault */
  TERMS_ERROR_LP_EINVAL            = -203, /* invalid argument */
  TERMS_ERROR_LP_ERANGE            = -204, /* data are out of range */
  TERMS_ERROR_LP_INVALID_MODE      = -205, /* invalid operation for current mode */
  TERMS_ERROR_LP_INVALID_FORMAT    = -206, /* invalid data format */
  TERMS_ERROR_LP_FS_FAILED         = -207, /* error while using fstorage */
  TERMS_ERROR_LP_STRICT_DATA       = -208, /* trying to create strict morphology, but no strict morph data provided */
  TERMS_ERROR_LP_STRICT            = -209, /* strict morphology error */

  TERMS_ERROR_LP_STRICT_DATA_FS    = -210, /* a problem with strict morph fstorage */
  TERMS_ERROR_LP_TOO_MANY_LANGS    = -211, /* too many languages for current lingproc */
  TERMS_ERROR_LP_UNSUPPORTED_LANG  = -212, /* current language is unsupported by current lingproc */
  TERMS_ERROR_LP_INVALID_LEXID     = -213, /* current lexid provided is invalid */
  TERMS_ERROR_LP_UNSUPPORTED_MORPH = -214, /* current morphology number provided is invalid for current action */
  TERMS_ERROR_LP_MORPH_FLAGS       = -215, /* invalid morph flags provided */
  TERMS_ERROR_LP_NOT_ENOUGH_DATA   = -216, /* data amount provided is too small */
  TERMS_ERROR_LP_MORPHS_COLLISION  = -217, /* two or more morphologies share one internal number */
  TERMS_ERROR_LP_DETECTOR          = -218, /* something wrong with language detector */
  
  TERMS_WARN_EMPTY            = 1,      /* term is empty */
  TERMS_WARN_LP_MORPH         = 2,      /* invalid morph results */
  TERMS_WARN_EXIST            = 3,      /* term is duplicate, skipped */
  TERMS_WARN_EXIST_CONFLICT   = 4,      /* term is duplicate and has attribute conflicts, skipped */
  TERMS_WARN_CONFLICT         = 5,      /* term is duplicate and has attribute conflicts, added */
  TERMS_WARN_ERANGE           = 6,      /* term length is out of range, skipped */
  TERMS_WARN_QUESTIONS_STR    = 7,      /* term appears to contain a string of question symbols, skipped */

  TERMS_WARN_CALLBACK_SET     = 100     /* callback is set */
};

/*------------------------------------------------------------------------------*/
/* Terms Base Handler */
typedef struct __HTERMSBASE* HTERMSBASE;

/* Terms Base Open Modes */
enum FltTermsBaseOpenModes /* != 0 */
{
  TERM_BASE_OPEN_RO = 1,
  TERM_BASE_OPEN_RO_SHARED,
  TERM_BASE_OPEN_CREATE,
  TERM_BASE_OPEN_RW
};

/* Terms Base Flags */
enum FltTermsBaseFlags
{
  TERM_BASE_FLAGS_DEFAULT       = 0x00000000,
  TERM_BASE_FLAGS_WITH_TEXT     = 0x00000001
};

/* Terms Base Save Parameters */
struct cf_terms_save_params
{
  int    transport;
  size_t alignement;
  int    export_denied;
  size_t write_buf_size;
};
/*------------------------------------------------------------------------------*/

typedef int (* cf_terms_enum_callback)(const cf_term_attrs* attrs, const char* txt, void* data);

#endif /* __terms_types_h__ */

/*
 * <eof terms_types.h>
 */
