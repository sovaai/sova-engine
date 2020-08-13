/**
 * @file   FltTermsStopEx.h
 * @author swizard <me@swizard.info>
 * @date   Sat Oct 10 15:16:16 2009
 * 
 * @brief  FltTerms stop words class with extended functionality.
 * 
 * 
 */
#ifndef _FLTTERMSSTOPEX_H_
#define _FLTTERMSSTOPEX_H_

#include <Filtration3/stopdict_ex/StopDictEx.h>

#include "terms_types.h"

class TermsStopDict : public StopDictEx
{
public:
    TermsStopDict() : StopDictEx() { }

public:
    FltTermsErrors init( LingProc &lp );
};

inline FltTermsErrors mapStopStatus( StopDictEx::Status status )
{
    switch ( status )
    {
    case StopDictEx::OK:                 return TERMS_OK;
    case StopDictEx::ENUM_STOP:          return TERMS_ERROR_BREAK;
    case StopDictEx::ERROR_FS_FAILED:    return TERMS_ERROR_FS_FAILED;
    case StopDictEx::ERROR_INVALID_MODE: return TERMS_ERROR_INVALID_MODE;
    case StopDictEx::ERROR_LP_FAILED:    return TERMS_ERROR_LP_FAILED;

    default: return TERMS_ERROR_EFAULT;
    };
}

#endif /* _FLTTERMSSTOPEX_H_ */

