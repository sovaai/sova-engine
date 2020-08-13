#ifndef _WORKERSPOOL_H_
#define _WORKERSPOOL_H_

#include <pthread.h>

#include <_include/cc_compat.h>

#include <lib/aptl/avector.h>

template< typename Worker, typename APWorkerPolicy >
class APWorker : public Worker
{
protected:
    typedef typename APWorkerPolicy::PolicyStatusType StatusType;
    typedef typename APWorkerPolicy::PolicyCreateArgs CreateArgs;
    typedef typename APWorkerPolicy::PolicyRunArgs    RunArgs;
    
public:
    APWorker() :
        thread(),
        workersSyncLock( 0 ),
        workersSyncCond( 0 ),
        shutdown( 0 )
    {
    }

    StatusType create( CreateArgs      &createArgs,
                       const bool      *shutdown,
                       pthread_mutex_t *workersSyncLock,
                       pthread_cond_t  *workersSyncCond )
    {
        this->workersSyncLock = workersSyncLock;
        this->workersSyncCond = workersSyncCond;
        this->shutdown        = shutdown;

        if ( workersSyncLock != 0 || workersSyncCond != 0 || shutdown != 0 )
        {
            if ( unlikely(workersSyncLock == 0 || workersSyncCond == 0 || shutdown == 0) )
                return APWorkerPolicy::statusFail;
        
            int pStatus = pthread_create( &thread, 0, APWorker::workerProc, this );
            if ( unlikely(pStatus != 0) )
                return APWorkerPolicy::statusFail;
        }
    
        return Worker::create( createArgs );
    }

    StatusType scheduleTask( RunArgs &args )
    {
        if ( unlikely(Worker::runArgs != 0) )
            return APWorkerPolicy::statusFail;

        Worker::runArgs = &args;
    
        return Worker::scheduleTask( args );
    }

    void finishTask() { Worker::runArgs = 0; }
    
    static void *workerProc( void *runParams )
    {
        APWorker *worker = reinterpret_cast<APWorker *>( runParams );
        return worker->threadLoop();
    }

    void *threadLoop()
    {
        for ( ;; )
        {
            pthread_mutex_lock( workersSyncLock );

            for ( ;; )
            {
                if ( *shutdown )
                {
                    pthread_mutex_unlock( workersSyncLock );
                    return 0;
                }

                if ( !hasFinished() )
                    break;
            
                pthread_cond_wait( workersSyncCond, workersSyncLock );
            }
        
            pthread_mutex_unlock( workersSyncLock );

            Worker::run();
        
            pthread_mutex_lock( workersSyncLock );
            finishTask();
            pthread_cond_broadcast( workersSyncCond );
            pthread_mutex_unlock( workersSyncLock );
        }
        
        return 0;
    }

    bool hasFinished() const { return Worker::runArgs == 0; }

    pthread_t *getThread() { return &thread; }
    
protected:
    pthread_t        thread;
    pthread_mutex_t *workersSyncLock;
    pthread_cond_t  *workersSyncCond;
    const bool      *shutdown;
};

template< typename Worker, typename APWorkerPolicy >
class APWorkersPool
{
protected:
    typedef typename APWorkerPolicy::PolicyStatusType StatusType;
    typedef typename APWorkerPolicy::PolicyCreateArgs CreateArgs;
    typedef typename APWorkerPolicy::PolicyRunArgs    RunArgs;
    
public:
    APWorkersPool() :
        workers(),
        workersIterLock(),
        workersSyncLock(),
        workersSyncCond(),
        shutdown( false )
    {
    }

    ~APWorkersPool()
    {
        if ( workers.size() > 1 )
        {
            shutdown = true;

            pthread_mutex_lock( &workersSyncLock );
            pthread_cond_broadcast( &workersSyncCond );
            pthread_mutex_unlock( &workersSyncLock );

            for ( size_t i = 0; i < workers.size(); i++ )
                pthread_join( *( workers[ i ].getThread() ), 0 );

            pthread_mutex_destroy( &workersIterLock );
            pthread_mutex_destroy( &workersSyncLock );
            pthread_cond_destroy( &workersSyncCond );
        }    
    }
    
    StatusType create( CreateArgs &args, size_t workersCount )
    {
        if ( unlikely(workers.size() != 0) )
            return APWorkerPolicy::statusFail;
        if ( unlikely(workersCount == 0) )
            return APWorkerPolicy::statusFail;

        if ( workersCount > 1 )
        {
            if ( unlikely(pthread_mutex_init( &workersIterLock, 0 ) != 0) )
                return APWorkerPolicy::statusFail;
            if ( unlikely(pthread_mutex_init( &workersSyncLock, 0 ) != 0) )
                return APWorkerPolicy::statusFail;
            if ( unlikely(pthread_cond_init( &workersSyncCond, 0 ) != 0) )
                return APWorkerPolicy::statusFail;
        }

        workers.resize( workersCount );
        if ( unlikely(workers.no_memory()) )
            return APWorkerPolicy::statusFail;
        
        for ( size_t i = 0; i < workersCount; i++ )
        {
            StatusType status = workers[ i ].create( args,
                                                     ( workersCount > 1 ? &shutdown : 0),
                                                     ( workersCount > 1 ? &workersSyncLock : 0),
                                                     ( workersCount > 1 ? &workersSyncCond : 0) );
            if ( unlikely(status != APWorkerPolicy::statusSuccess) )
                return status;
        }

        return APWorkerPolicy::statusSuccess;
    }
    
    StatusType run( RunArgs &args )
    {
        if ( unlikely(workers.size() == 0) )
            return APWorkerPolicy::statusFail;

        if ( workers.size() == 1 )
        {
            StatusType status = workers[ 0 ].scheduleTask( args );
            if ( unlikely(status != APWorkerPolicy::statusSuccess) )
                return status;
            workers[ 0 ].run();
            workers[ 0 ].finishTask();
            return workers[ 0 ].getTaskLastStatus();
        }
        
        pthread_mutex_lock( &workersSyncLock );
        
        for ( size_t i = 0; i < workers.size(); i++ )
        {
            StatusType status = workers[ i ].scheduleTask( args );
            if ( unlikely(status != APWorkerPolicy::statusSuccess) )
                return status;
        }

        pthread_cond_broadcast( &workersSyncCond );

        for ( ;; )
        {
            pthread_cond_wait( &workersSyncCond, &workersSyncLock );
        
            bool everyoneFinished = true;

            for ( size_t i = 0; i < workers.size(); i++ )
                if ( !workers[ i ].hasFinished() )
                {
                    everyoneFinished = false;
                    break;
                }

            if ( everyoneFinished )
                break;
        }

        pthread_mutex_unlock( &workersSyncLock );

        for ( size_t i = 0; i < workers.size(); i++ )
        {
            StatusType status = workers[ i ].getTaskLastStatus();
            if ( unlikely(status != APWorkerPolicy::statusSuccess) )
                return status;
        }
    
        return APWorkerPolicy::statusSuccess;
    }

    void iterationLockGet()     { if ( workers.size() > 1 ) pthread_mutex_lock( &workersIterLock ); }
    void iterationLockRelease() { if ( workers.size() > 1 ) pthread_mutex_unlock( &workersIterLock ); }
    
protected:
    avector<Worker>   workers;
    pthread_mutex_t   workersIterLock;
    pthread_mutex_t   workersSyncLock;
    pthread_cond_t    workersSyncCond;
    bool              shutdown;
};


#endif /* _WORKERSPOOL_H_ */

