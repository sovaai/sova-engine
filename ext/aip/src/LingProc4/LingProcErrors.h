#ifndef _LINGPROCERRORS_H_
#define _LINGPROCERRORS_H_

// LingProc error codes
enum LingProcErrors
{
    LP_OK                      = 0,    /* no error */   
    LP_ERROR_ENOTIMPL          = -1,   /* not implemented */
    LP_ERROR_ENOMEM            = -12,  /* not enough memory */
    LP_ERROR_EFAULT            = -14,  /* unknown fault */
    LP_ERROR_EINVAL            = -22,  /* invalid argument */
    LP_ERROR_ERANGE            = -34,  /* data are out of range */
    LP_ERROR_INVALID_MODE      = -100, /* invalid operation for current mode */
    LP_ERROR_INVALID_FORMAT    = -101, /* invalid data format */
    LP_ERROR_FS_FAILED         = -104, /* error while using fstorage */
    LP_ERROR_STRICT_DATA       = -105, /* trying to create strict morphology, but no strict morph data provided */
    LP_ERROR_STRICT            = -106, /* strict morphology error */

    LP_ENUM_STOP               = -107, /* force a enumeration to stop */
    
    LP_ERROR_STRICT_DATA_FS    = -200, /* a problem with strict morph fstorage */
    LP_ERROR_TOO_MANY_LANGS    = -201, /* too many languages for current lingproc */
    LP_ERROR_UNSUPPORTED_LANG  = -202, /* current language is unsupported by current lingproc */
    LP_ERROR_INVALID_LEXID     = -203, /* current lexid provided is invalid */
    LP_ERROR_UNSUPPORTED_MORPH = -204, /* current morphology number provided is invalid for current action */
    LP_ERROR_MORPH_FLAGS       = -205, /* invalid morph flags provided */
    LP_ERROR_NOT_ENOUGH_DATA   = -206, /* data amount provided is too small */
    LP_ERROR_MORPHS_COLLISION  = -207, /* two or more morphologies share one internal number */
    LP_ERROR_DETECTOR          = -208  /* something wrong with language detector */
};

const char *LingProcErrorsText( LingProcErrors err );

#endif /* _LINGPROCERRORS_H_ */

