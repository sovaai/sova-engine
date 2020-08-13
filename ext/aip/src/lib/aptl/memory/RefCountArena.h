#ifndef _REFCOUNTARENA_H_
#define _REFCOUNTARENA_H_

#include "RawArena.h"
#include "ZeroArena.h"

namespace Arena
{
    struct RefCountBlock : public Block
    {        
        int64_t refsCount;
    };

    template< typename Policy, typename BlockType, typename BlocksSetType, typename AreaEngine >
    class RefCountAreaEngine : public AreaEngine
    {
        typedef typename Policy::StatusType StatusType;

    public:
        static const size_t areaHeaderSize = ( static_cast<size_t>( sizeof( BlockType * ) + 15 ) &
                                               ( ~(static_cast<size_t>( 15 ) ) ) );
        
        
    public:
        RefCountAreaEngine() : AreaEngine() { }

        StatusType allocBlock( BlockType *&block )
        {
            StatusType status = AreaEngine::allocBlock( block );
            if ( unlikely(status != Policy::statusSuccess) )
                return status;

            block->refsCount = 0;
            
            return Policy::statusSuccess;
        }

        bool notEnoughSpace( size_t offset, size_t amount )
        {
            return AreaEngine::notEnoughSpace( offset, amount + areaHeaderSize );
        }

        StatusType addBlockToSet( BlockType &block, BlocksSetType &set )
        {
            if ( !set.isEmpty() )
            {
                BlockType *activeBlock = set.getFirst();
                if ( activeBlock->refsCount == 0 )
                {
                    StatusType status = AreaEngine::freeBlock( activeBlock, set );
                    if ( unlikely(status != Policy::statusSuccess) )
                        return status;
                }
            }
            
            return AreaEngine::addBlockToSet( block, set );
        }
        
        StatusType allocInBlock( BlockType *activeBlock, uint8_t *&area, size_t &offset, size_t amount )
        {
            StatusType status = AreaEngine::allocInBlock( activeBlock, area, offset, amount + areaHeaderSize );
            if ( unlikely(status != Policy::statusSuccess) )
                return status;

            activeBlock->refsCount++;

            *( reinterpret_cast<BlockType **>( area ) ) = activeBlock;
            area += areaHeaderSize;
            
            return Policy::statusSuccess;
        }

        static size_t allocationCostBytes() { return areaHeaderSize; }
    };
    
    template< typename Policy, typename BlockType, typename Area >
    class RefCountArea : public Area
    {
        typedef typename Policy::StatusType StatusType;
        
    public:
        RefCountArea() : Area() { }

        template< typename T > StatusType free( T *ptr ) { return freeGeneric( reinterpret_cast<uint8_t *>( ptr ) ); }

        StatusType freeGeneric( uint8_t *area )
        {
            BlockType *activeBlock = *( reinterpret_cast<BlockType **>( area - Area::allocationCostBytes() ) );
            if ( unlikely(reinterpret_cast<uint8_t *>( activeBlock ) > area) )
                return Policy::statusErrorInvalidArg;

            if ( unlikely(activeBlock->refsCount <= 0) )
                return Policy::statusErrorInvalidArg;

            activeBlock->refsCount--;

            if ( activeBlock->refsCount == 0 )
            {
                if ( activeBlock == Area::blocks.getFirst() )
                    return Area::rewind();
                
                return Area::freeBlock( activeBlock, Area::blocks );
            }
            
            return Policy::statusSuccess;
        }
    };


    template< typename Policy >
    struct RawRefCount
    {
        typedef BaseSetMember< RefCountBlock > LinkedBlock;
        typedef BaseSet< Policy, LinkedBlock > BlocksSet;
    
        typedef RefCountAreaEngine<
            Policy,
            LinkedBlock,
            BlocksSet,
            BaseAreaEngineWithReuse< Policy, LinkedBlock, BlocksSet > > AreaEngine;
        typedef BaseSetMember<
            RefCountArea<
                Policy,
                LinkedBlock,
                BaseArea<
                    Policy,
                    LinkedBlock,
                    BlocksSet,
                    AreaEngine > > > LinkedBlockLinkedArea;
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

    template< typename Policy >
    struct ZeroRefCount
    {
        typedef BaseSetMember< RefCountBlock > LinkedBlock;
        typedef BaseSet< Policy, LinkedBlock > BlocksSet;

        typedef RefCountAreaEngine<
            Policy,
            LinkedBlock,
            BlocksSet,
            ZeroAreaEngine< Policy, LinkedBlock, BlocksSet > > AreaEngine;
        typedef BaseSetMember<
            RefCountArea<
                Policy,
                LinkedBlock,
                ZeroArea<
                    Policy,
                    LinkedBlock,
                    BlocksSet,
                    AreaEngine > > > LinkedBlockLinkedArea;
        typedef BaseSet< Policy, LinkedBlockLinkedArea > AreasSet;
        typedef ZeroArenaBase<
            Policy,
            LinkedBlockLinkedArea,
            AreasSet,
            ZeroArenaEngine<
                Policy,
                LinkedBlockLinkedArea,
                AreasSet > > ArenaType;
    };
}

#endif /* _REFCOUNTARENA_H_ */

