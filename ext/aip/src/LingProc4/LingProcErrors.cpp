
#include "LingProcErrors.h"

const char *LingProcErrorsText( LingProcErrors err )
{
    switch (err)
    {
    case  LP_OK:                     return "no error";
    case  LP_ERROR_ENOTIMPL:         return "not implemented";
    case  LP_ERROR_ENOMEM:           return "not enough memory";
    case  LP_ERROR_EINVAL:           return "invalid argument";
    case  LP_ERROR_ERANGE:           return "data are out of range";
    case  LP_ERROR_INVALID_MODE:     return "invalid operation for current mode";
    case  LP_ERROR_INVALID_FORMAT:   return "invalid data format";
    case  LP_ERROR_FS_FAILED:        return "error while using fstorage";
    case  LP_ERROR_STRICT_DATA:      return "trying to create strict morphology, but no strict morph data provided";
    case  LP_ERROR_STRICT:           return "strict morphology error";

    case LP_ERROR_STRICT_DATA_FS:    return "a problem with strict morph fstorage";
    case LP_ERROR_TOO_MANY_LANGS:    return "too many languages for current lingproc";
    case LP_ERROR_UNSUPPORTED_LANG:  return "current language is unsupported by current lingproc";
    case LP_ERROR_INVALID_LEXID:     return "current lexid provided is invalid";
    case LP_ERROR_UNSUPPORTED_MORPH: return "current morphology number provided is invalid for current action";
    case LP_ERROR_MORPH_FLAGS:       return "invalid morph flags provided";
    case LP_ERROR_NOT_ENOUGH_DATA:   return "data amount provided is too small";
    case LP_ERROR_MORPHS_COLLISION:  return "two or more morphologies share one internal number";
    case LP_ERROR_DETECTOR:          return "something wrong with language detector";
            
    default:
    case  LP_ERROR_EFAULT:
	break;
    }
    return "unknown fault";
}

