#ifndef _SEQUENCESM_H_
#define _SEQUENCESM_H_

#include <_include/cc_compat.h>

#include "../../LingProcErrors.h"

class UCharSet;

template< typename SM1, typename SM2 >
class SequenceSM2 : public SM1, public SM2
{
public:
    SequenceSM2( const UCharSet &_genericCharset ) :
        SM1( _genericCharset ),
        SM2( _genericCharset )
    {
    }

    LingProcErrors nextChar( size_t current, uint32_t ch )
    {
        LingProcErrors status = SM1::nextChar( current, ch );
        if ( likely(status == LP_OK) )
            status = SM2::nextChar( current, ch );
        return status;
    }

    LingProcErrors flush()
    {
        LingProcErrors status = SM1::flush();
        if ( likely(status == LP_OK) )
            status = SM2::flush();
        return status;
    }

    void reset()
    {
        SM1::reset();
        SM2::reset();
    }
};

#endif /* _SEQUENCESM_H_ */

