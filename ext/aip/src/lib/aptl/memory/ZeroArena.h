#ifndef _ZEROARENA_H_
#define _ZEROARENA_H_

#include <pthread.h>
#include <sys/time.h>

#include "BaseArena.h"

namespace Arena
{
    enum ZeroStatus
    {
        ZA_OK,
        ZA_ERROR_MUTEX,
        ZA_ERROR_COND,
        ZA_ERROR_PTHREAD,
        ZA_ERROR_ALLOC,
        ZA_ERROR_INVALID_ARG,
        ZA_ERROR_SMALL_BLOCK,
        ZA_ERROR_NOT_CREATED,
        ZA_ERROR_ALREADY_CREATED
    };

    struct DefaultZeroPolicy : public DefaultPolicy
    {
        typedef ZeroStatus StatusType;

        static const ZeroStatus statusSuccess             = ZA_OK;
        static const ZeroStatus statusErrorMutex          = ZA_ERROR_MUTEX;
        static const ZeroStatus statusErrorCond           = ZA_ERROR_COND;
        static const ZeroStatus statusErrorPthread        = ZA_ERROR_PTHREAD;
        static const ZeroStatus statusErrorAlloc          = ZA_ERROR_ALLOC;
        static const ZeroStatus statusErrorInvalidArg     = ZA_ERROR_INVALID_ARG;
        static const ZeroStatus statusErrorSmallBlock     = ZA_ERROR_SMALL_BLOCK;
        static const ZeroStatus statusErrorNotCreated     = ZA_ERROR_NOT_CREATED;
        static const ZeroStatus statusErrorAlreadyCreated = ZA_ERROR_ALREADY_CREATED;

        static const size_t allocatorWakeupIntervalMs = 250000;
    };


    template< typename Policy >
    class ZeroArenaSynchronizer
    {
        typedef typename Policy::StatusType StatusType;
        
    public:
        ZeroArenaSynchronizer() :
            syncLock(),
            syncCondProd(),
            syncCondCons(),
            workerRunning( false )
        {
        }

        ~ZeroArenaSynchronizer()
        {
            pthread_mutex_destroy( &syncLock );
            pthread_cond_destroy( &syncCondProd );
            pthread_cond_destroy( &syncCondCons );
        }

        StatusType create()
        {
            if ( unlikely(pthread_mutex_init( &syncLock, 0 ) != 0) )
                return Policy::statusErrorMutex;
            if ( unlikely(pthread_cond_init( &syncCondProd, 0 ) != 0) )
                return Policy::statusErrorCond;
            if ( unlikely(pthread_cond_init( &syncCondCons, 0 ) != 0) )
                return Policy::statusErrorCond;

            workerRunning = false;

            return Policy::statusSuccess;
        }

        void lock() { pthread_mutex_lock( &syncLock ); }
        void unlock() { pthread_mutex_unlock( &syncLock ); }
        void waitProd()
        {
            timeval tv;
            gettimeofday( &tv, 0 );

            timeval tvi = { 0, Policy::allocatorWakeupIntervalMs };

            timeval tvr;
            timeradd( &tv, &tvi, &tvr );
            
            timespec tms = { tvr.tv_sec, tvr.tv_usec * 1000 };
            
            pthread_cond_timedwait( &syncCondProd, &syncLock, &tms );

            // pthread_cond_wait( &syncCondProd, &syncLock );
        }
        
        void notifyProd() { pthread_cond_signal( &syncCondProd ); }
        void waitCons() { pthread_cond_wait( &syncCondCons, &syncLock ); }
        void notifyCons() { pthread_cond_signal( &syncCondCons ); }
        void setRunning( bool value ) { workerRunning = value; }
        bool isRunning() const { return workerRunning; }
        
    protected:
        pthread_mutex_t syncLock;
        pthread_cond_t  syncCondProd;
        pthread_cond_t  syncCondCons;
        bool            workerRunning;
    };
    
    template< typename Policy, typename BlockType, typename BlocksSetType >
    class ZeroAreaEngine : public BaseAreaEngineWithReuse< Policy, BlockType, BlocksSetType >
    {
        typedef BaseAreaEngineWithReuse< Policy, BlockType, BlocksSetType > Base;
        typedef typename Policy::StatusType StatusType;
        
    public:
        ZeroAreaEngine() : Base(), inProgressBlock( 0 ), synch( 0 ) { }

        ~ZeroAreaEngine()
        {
            if ( inProgressBlock != 0 )
            {                
                Base::destroyBlock( inProgressBlock );
                inProgressBlock = 0;
            }
        }
        
        void attach( ZeroArenaSynchronizer< Policy > &synch ) { this->synch = &synch; }

        StatusType allocBlock( BlockType *&block )
        {
            synch->lock();
            
            for ( ;; )
            {
                if ( likely(inProgressBlock != 0) )
                    break;

                synch->waitCons();
                
                if ( unlikely(!synch->isRunning()) )
                {
                    synch->unlock();
                    return Policy::statusErrorAlloc;
                }
            }
            
            block = inProgressBlock;
            inProgressBlock = 0;

            synch->notifyProd();
            synch->unlock();
            
            return Policy::statusSuccess;
        }

        StatusType freeBlock( BlockType *block, BlocksSetType &set )
        {
            synch->lock();
            StatusType status = Base::freeBlock( block, set );
            synch->unlock();

            return status;
        }
        
        StatusType allocInProgressBlock()
        {
            if ( likely(inProgressBlock != 0) )
                return Policy::statusSuccess;

            StatusType status = Base::allocBlock( inProgressBlock );
            if ( unlikely(status != Policy::statusSuccess) )
                return status;

            memset( reinterpret_cast<uint8_t *>( inProgressBlock ) + sizeof( BlockType ), 0, Base::blockSize );

            return Policy::statusSuccess;
        }

        StatusType rewind( size_t &/* offset */ ) { return Policy::statusSuccess; }
        
    protected:
        BlockType *inProgressBlock;
        ZeroArenaSynchronizer< Policy > *synch;
    };
    
    template< typename Policy, typename BlockType, typename BlocksSetType, typename AreaEngine >
    class ZeroArea : public BaseArea< Policy, BlockType, BlocksSetType, AreaEngine >
    {
        typedef BaseArea< Policy, BlockType, BlocksSetType, AreaEngine > Base;
        typedef typename Policy::StatusType StatusType;
        
    public:
        ZeroArea() : Base(), synch( 0 ) { }

        void attach( ZeroArenaSynchronizer< Policy > &synch ) { this->synch = &synch; }
        
        StatusType create( size_t blockSize )
        {
            AreaEngine::attach( *synch );

            return Base::create( blockSize );
        }
        
    protected:
        BlockType *inProgressBlock;
        ZeroArenaSynchronizer< Policy > *synch;
    };

    template< typename Policy, typename AreaType, typename AreasSetType >
    class ZeroArenaEngine : public BaseArenaEngine< Policy, AreaType, AreasSetType >
    {
        typedef BaseArenaEngine< Policy, AreaType, AreasSetType > Base;
        typedef typename AreaType::AreaBlockType BlockType;
        typedef typename Policy::StatusType StatusType;
        
    public:
        ZeroArenaEngine() : Base(), synch() { }
        
        StatusType create()
        {
            StatusType status = Base::create();
            if ( unlikely(status != Policy::statusSuccess) )
                return status;
            
            return synch.create();
        }
    
        StatusType allocArea( AreaType *&area )
        {
            StatusType status = Base::allocArea( area );
            if ( unlikely(status != Policy::statusSuccess) )
                return status;

            area->attach( synch );

            return Policy::statusSuccess;
        }

    protected:
        BlockType *inProgressBlock;
        ZeroArenaSynchronizer< Policy > synch;
    };

    template< typename Policy, typename AreaType, typename AreasSetType, typename ArenaEngine >
    class ZeroArenaBase : public BaseArena< Policy, AreaType, AreasSetType, ArenaEngine >
    {
        typedef BaseArena< Policy, AreaType, AreasSetType, ArenaEngine > Base;
        typedef typename Policy::StatusType StatusType;
        
    public:
        ZeroArenaBase() : Base(), allocThread() { }

        ~ZeroArenaBase()
        {
            ArenaEngine::synch.lock();
            if ( ArenaEngine::synch.isRunning() )
            {
                ArenaEngine::synch.setRunning( false );
                ArenaEngine::synch.notifyProd();
            }

            ArenaEngine::synch.unlock();                

            pthread_join( allocThread, 0 );
        }

        StatusType create()
        {
            StatusType status = Base::create();
            if ( unlikely(status != Policy::statusSuccess) )
                return status;

            if ( unlikely(pthread_create( &allocThread, 0, allocatorProc, this ) != 0) )
                return Policy::statusErrorPthread;

            ArenaEngine::synch.lock();
            while ( !ArenaEngine::synch.isRunning() )
                ArenaEngine::synch.waitCons();
            ArenaEngine::synch.unlock();

            return Policy::statusSuccess;
        }

        StatusType makeArea( AreaType *&area, size_t blockSize = Policy::defaultBlockSize )
        {
            ArenaEngine::synch.lock();
            
            StatusType status = Base::makeArea( area, blockSize );

            ArenaEngine::synch.notifyProd();
            ArenaEngine::synch.unlock();

            return status;
        }
        
    protected:
        static void *allocatorProc( void *runParams )
        {
            ZeroArenaBase *worker = reinterpret_cast<ZeroArenaBase *>( runParams );
            worker->allocatorLoop();
            
            return 0;
        }

        void allocatorLoop()
        {
            ArenaEngine::synch.lock();
            ArenaEngine::synch.setRunning( true );
            
            while ( ArenaEngine::synch.isRunning() )
            {
                for ( AreaType *area = Base::areas.getFirst();
                      area != 0;
                      area = Base::areas.getNext( area ) )
                {
                    StatusType status = area->allocInProgressBlock();
                    if ( unlikely(status != Policy::statusSuccess) )
                    {
                        ArenaEngine::synch.setRunning( false );
                        break;
                    }
                }

                ArenaEngine::synch.notifyCons();
                
                if ( ArenaEngine::synch.isRunning() )
                    ArenaEngine::synch.waitProd();
            }

            ArenaEngine::synch.unlock();
        }
        
    protected:
        pthread_t allocThread;
    };


    template< typename Policy >
    struct Zero
    {
        typedef BaseSetMember< Block > LinkedBlock;
        typedef BaseSet< Policy, LinkedBlock > BlocksSet;
        typedef BaseSetMember<
            ZeroArea<
                Policy,
                LinkedBlock,
                BlocksSet,
                ZeroAreaEngine< Policy, LinkedBlock, BlocksSet > > > LinkedBlockLinkedArea;
        typedef BaseSet< Policy, LinkedBlockLinkedArea > AreasSet;
        typedef BaseArena<
            Policy,
            LinkedBlockLinkedArea,
            AreasSet,
            ZeroArenaEngine< Policy, LinkedBlockLinkedArea, AreasSet > > ZeroArenaType;
    };
}

#endif /* _ZEROARENA_H_ */

