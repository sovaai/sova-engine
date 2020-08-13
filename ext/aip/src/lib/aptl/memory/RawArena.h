#ifndef _RAWARENA_H_
#define _RAWARENA_H_

#include "BaseArena.h"

namespace Arena
{
    enum RawStatus
    {
        RA_OK,
        RA_ERROR_ALLOC,
        RA_ERROR_INVALID_ARG,
        RA_ERROR_SMALL_BLOCK,
        RA_ERROR_NOT_CREATED,
        RA_ERROR_ALREADY_CREATED
    };

    struct DefaultRawPolicy : public DefaultPolicy
    {
        typedef RawStatus StatusType;

        static const RawStatus statusSuccess             = RA_OK;
        static const RawStatus statusErrorAlloc          = RA_ERROR_ALLOC;
        static const RawStatus statusErrorInvalidArg     = RA_ERROR_INVALID_ARG;
        static const RawStatus statusErrorSmallBlock     = RA_ERROR_SMALL_BLOCK;
        static const RawStatus statusErrorNotCreated     = RA_ERROR_NOT_CREATED;
        static const RawStatus statusErrorAlreadyCreated = RA_ERROR_ALREADY_CREATED;
    };

    template< typename Policy >
    struct Raw
    {
        typedef BaseSetMember< Block > LinkedBlock;
        typedef BaseSet< Policy, LinkedBlock > BlocksSet;
        typedef BaseSetMember<
            BaseArea< Policy,
                      LinkedBlock,
                      BlocksSet,
                      BaseAreaEngineWithReuse<
                          Policy,
                          LinkedBlock,
                          BlocksSet > > > LinkedBlockLinkedArea;
        typedef BaseSet< Policy, LinkedBlockLinkedArea > AreasSet;
        typedef BaseArena<
            Policy,
            LinkedBlockLinkedArea,
            AreasSet,
            BaseArenaEngine<
                Policy,
                LinkedBlockLinkedArea,
                AreasSet > > ArenaType;
    };
}

#endif /* _RAWARENA_H_ */

