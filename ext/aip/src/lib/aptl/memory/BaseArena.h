#ifndef _BASEARENA_H_
#define _BASEARENA_H_

#include <assert.h>
#include <stdlib.h>

#include <_include/cc_compat.h>
#include <_include/_string.h>

namespace Arena
{    
    struct DefaultPolicy
    {
        static const size_t defaultBlockSize = 8UL * 1024 * 1024;
    };

    
    template< typename MemberType >
    struct BaseSetMember : public MemberType
    {
        BaseSetMember() : MemberType() { }
        
	BaseSetMember *next;
	BaseSetMember **pPrev;
    };

    template< typename Policy, typename MemberType >
    class BaseSet
    {
        typedef typename Policy::StatusType StatusType;
        
    public:
        BaseSet() : first( 0 ) { }

        StatusType initialize() { first = 0; return Policy::statusSuccess; }

        StatusType add( MemberType &item )
        {
            item.next = first;
            if ( !isEmpty() )
                first->pPrev = &item.next;
            first = &item;
            item.pPrev = &first;
            
            return Policy::statusSuccess;
        }

        MemberType *getFirst() { return first; }

        MemberType *getNext( MemberType *item ) { return item->next; }
        
        void remove( MemberType &item )
        {
            if ( item.next != 0 )
                item.next->pPrev = item.pPrev;
            *item.pPrev = item.next;
        }
        
        bool isEmpty() const { return first == 0; }
        
    protected:
        MemberType *first;
    };
    
    struct Block
    {
        volatile uint8_t *area;
    };

    template< typename Policy, typename BlockType, typename BlocksSetType >
    class BaseAreaEngine
    {
        typedef typename Policy::StatusType StatusType;
        
    public:
        typedef BlockType AreaBlockType;
        
    public:
        BaseAreaEngine() : blockSize( Policy::defaultBlockSize ) { }
        
        StatusType create( size_t blockSize, BlocksSetType &/* blocks */ )
        {
	    this->blockSize = blockSize;
        
            return Policy::statusSuccess;
        }

        bool notEnoughSpace( size_t offset, size_t amount ) { return offset + amount > blockSize; }
        
        StatusType allocBlock( BlockType *&block )
        {
	    size_t sizeToAlloc = ( blockSize + sizeof( BlockType ) + 0xF ) & ( ~(static_cast<size_t>( 0xF )) );

            block = reinterpret_cast<BlockType *>( ::malloc( sizeToAlloc ) );
            if ( unlikely(block == 0) )
                return Policy::statusErrorAlloc;

	    block->area = reinterpret_cast<uint8_t *>( block ) + sizeToAlloc - blockSize;

            return Policy::statusSuccess;
        }

        StatusType allocInBlock( BlockType *activeBlock, uint8_t *&area, size_t &offset, size_t amount )
        {
            area    = (uint8_t *)( activeBlock->area ) + offset;
            offset += amount;
        
            return Policy::statusSuccess;
        }

        StatusType shrink( BlocksSetType &set, size_t &offset, uint8_t *area, size_t areaSize, size_t shrinkAreaSize )
        {
            if ( areaSize == shrinkAreaSize )
                return Policy::statusSuccess;
            
            if ( unlikely(set.isEmpty() || ( areaSize < shrinkAreaSize )) )
                return Policy::statusErrorInvalidArg;

            BlockType *activeBlock = set.getFirst();
            if ( unlikely(( area + areaSize ) != ( activeBlock->area + offset )) )
                return Policy::statusErrorInvalidArg;

            offset -= ( areaSize - shrinkAreaSize );
            
            return Policy::statusSuccess;
        }
        
        StatusType addBlockToSet( BlockType &block, BlocksSetType &set ) { return set.add( block ); }
        
        StatusType destroyBlock( BlockType *block )
        {        
            ::free( block );
            
            return Policy::statusSuccess;
        }

        StatusType freeBlock( BlockType *block, BlocksSetType &set )
        {            
            set.remove( *block );
            
            return destroyBlock( block, set );
        }

        StatusType rewind( size_t &offset ) 
        { 
	    offset = 0;
	    return Policy::statusSuccess; 
	}

        static size_t allocationCostBytes() { return 0; }
        
    protected:
        size_t blockSize;
    };

    template< typename Policy, typename BlockType, typename BlocksSetType >
    class BaseAreaEngineWithReuse : public BaseAreaEngine< Policy, BlockType, BlocksSetType >
    {
        typedef BaseAreaEngine< Policy, BlockType, BlocksSetType > Base;
        typedef typename Policy::StatusType StatusType;
        
    public:
        BaseAreaEngineWithReuse() : Base(), freeBlocks() { }

        ~BaseAreaEngineWithReuse()
        {
            while ( !freeBlocks.isEmpty() )
            {
                BlockType *block = freeBlocks.getFirst();
                freeBlocks.remove( *block );
                Base::destroyBlock( block );
            }
        }
        
        StatusType create( size_t blockSize, BlocksSetType &blocks )
        {
            StatusType status = Base::create( blockSize, blocks );
            if ( unlikely(status != Policy::statusSuccess) )
                return status;

            return freeBlocks.initialize();
        }
        
        StatusType allocBlock( BlockType *&block )
        {
            if ( freeBlocks.isEmpty() )
                return Base::allocBlock( block );

            block = freeBlocks.getFirst();
            freeBlocks.remove( *block );

            return Policy::statusSuccess;
        }

        StatusType freeBlock( BlockType *block, BlocksSetType &set )
        {
            set.remove( *block );
            
            return freeBlocks.add( *block );
        }
        
    protected:
        BlocksSetType freeBlocks;
    };    
    
    template< typename Policy, typename BlockType, typename BlocksSetType, typename AreaEngine >
    class BaseArea : public AreaEngine
    {
        typedef typename Policy::StatusType StatusType;
        
    public:
        BaseArea() :
            AreaEngine(),
            blocks(),
            offset( 0 )
        {
        }

        ~BaseArea()
        {
            while ( !blocks.isEmpty() )
            {
                BlockType *block = blocks.getFirst();
                blocks.remove( *block );
                AreaEngine::destroyBlock( block );
            }
        }
        
        StatusType create( size_t blockSize )
        {
            StatusType status = AreaEngine::create( blockSize, blocks );
            if ( unlikely(status != Policy::statusSuccess) )
                return status;

            offset = 0;
            
            return blocks.initialize();
        }

        static size_t allocationCostBytes() { return AreaEngine::allocationCostBytes(); }
        
        template< typename T >
        StatusType alloc( T *&ptr, size_t amount = sizeof( T ) )
        {
            uint8_t *tmpArea;
            StatusType status = allocGeneric( tmpArea, amount );
            if ( unlikely(status != Policy::statusSuccess) )
                return status;

            ptr = reinterpret_cast<T *>( tmpArea );

            return Policy::statusSuccess;
        }

        StatusType allocGeneric( uint8_t *&area, size_t amount )
        {
            if ( unlikely(amount > AreaEngine::blockSize) )
                return Policy::statusErrorSmallBlock;

            area = 0;

            if ( blocks.isEmpty() || AreaEngine::notEnoughSpace( offset, amount ) )
            {
                BlockType *block;
                StatusType status = AreaEngine::allocBlock( block );
                if ( unlikely(status != Policy::statusSuccess) )
                    return status;

                status = AreaEngine::addBlockToSet( *block, blocks );
                if ( unlikely(status != Policy::statusSuccess) )
                    return status;

                status = AreaEngine::rewind( offset );
                if ( unlikely(status != Policy::statusSuccess) )
                    return status;
            }

            return AreaEngine::allocInBlock( blocks.getFirst(), area, offset, amount );
        }

        StatusType freeAll()
        {
            while ( !blocks.isEmpty() )
            {
                StatusType status = AreaEngine::freeBlock( blocks.getFirst(), blocks );
                if ( unlikely(status != Policy::statusSuccess) )
                    return status;
            }

            return Policy::statusSuccess;
        }

        StatusType shrink( uint8_t *area, size_t areaSize, size_t shrinkAreaSize )
        {
            return AreaEngine::shrink( blocks, offset, area, areaSize, shrinkAreaSize );
        }

        StatusType rewind() { return AreaEngine::rewind( offset ); }
        
    protected:
        BlocksSetType blocks;
        size_t offset;
    };

    template< typename Policy, typename AreaType, typename AreasSetType >
    class BaseArenaEngine
    {
        typedef typename Policy::StatusType StatusType;

    public:
        StatusType create() { return Policy::statusSuccess; }
    
        StatusType allocArea( AreaType *&area )
        {
            area = new AreaType;
            if ( unlikely(area == 0) )
                return Policy::statusErrorAlloc;

            return Policy::statusSuccess;
        }
        
        StatusType freeArea( AreaType *area )
        {
            delete area;

            return Policy::statusSuccess;
        }
    };
    
    template< typename Policy, typename AreaType, typename AreasSetType, typename ArenaEngine >
    class BaseArena : public ArenaEngine
    {
        typedef typename Policy::StatusType StatusType;
        
    public:
        typedef AreaType Area;

    public:
        BaseArena() :
            ArenaEngine(),
            areas(),
            isCreated( false )
        {
        }

        ~BaseArena()
        {
            while ( !areas.isEmpty() )
            {
                Area *area = areas.getFirst();
                areas.remove( *area );
                ArenaEngine::freeArea( area );
            }
        }

        StatusType create()
        {
            if ( unlikely(isCreated) )
                return Policy::statusErrorAlreadyCreated;

            StatusType status = areas.initialize();
            if ( unlikely(status != Policy::statusSuccess) )
                return status;
            
            status = ArenaEngine::create();
            if ( unlikely(status != Policy::statusSuccess) )
                return status;
 
            isCreated = true;
            return Policy::statusSuccess;
        }

        StatusType makeArea( Area *&area, size_t blockSize = Policy::defaultBlockSize )
        {
            if ( unlikely(!isCreated) )
                return Policy::statusErrorNotCreated;

            StatusType status = ArenaEngine::allocArea( area );
            if ( unlikely(status != Policy::statusSuccess) )
                return status;
            
            status = area->create( blockSize );
            if ( unlikely(status != Policy::statusSuccess) )
            {
                ArenaEngine::freeArea( area );
                return status;
            }

            status = areas.add( *area );
            if ( unlikely(status != Policy::statusSuccess) )
            {
                ArenaEngine::freeArea( area );
                return status;
            }
            
            return Policy::statusSuccess;
        }
        
    protected:
        AreasSetType areas;
        bool         isCreated;
    };
}

#endif /* _BASEARENA_H_ */

