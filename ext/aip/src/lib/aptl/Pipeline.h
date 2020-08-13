#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include <_include/cc_compat.h>

#include <lib/aptl/WorkersPool.h>
#include <gnu/zeromq/include/zmq.h>

namespace Pl
{
    template< typename PipelinePolicy >
    class PipelinePeer
    {
        typedef typename PipelinePolicy::PolicyStatusType StatusType;
        
    public:
        PipelinePeer() : sideLeft(), sideRight() { }

    public:
        struct WorkloadVec
        {
            const void *data;
            size_t dataSize;

            WorkloadVec() : data( 0 ), dataSize( 0 ) { }
            WorkloadVec( const void *_data, size_t _dataSize ) : data( _data ), dataSize( _dataSize ) { }
        };
        
    protected:
        class Side
        {
        public:
            Side() : sock( 0 ) { }

            ~Side()
            {
                if ( sock != 0 )
                    zmq_close( sock );
            }

            StatusType create( void *context, int socketType, int (*setup)( void *sock, const char *addr ), const char *addr )
            {
                if ( sock != 0 )
                    return PipelinePolicy::statusSuccess;
                
                sock = zmq_socket( context, socketType );
                if ( unlikely(sock == 0) )
                    return PipelinePolicy::statusErrorAlloc;

                if ( unlikely(setup( sock, addr ) != 0) )
                    return PipelinePolicy::statusErrorZmqSetup;

                return PipelinePolicy::statusSuccess;
            }

            void *getSock() { return sock; }
            
        protected:
            void *sock;
        };
        
        class Workload
        {
        public:
            Workload() : mask( 0 ), msgs() { }

            ~Workload() { clear(); }

            void clear()
            {
                for ( int i = 0; i < PipelinePolicy::workloadsCount + 1; i++ )
                    if ( mask & ( static_cast<uint64_t>( 1 ) << i ) )
                        releaseMessage( i );
            }
            
            StatusType recv( void *sock )
            {
                for ( int i = 0; i < PipelinePolicy::workloadsCount + 1; i++ )
                {
                    if ( unlikely(( mask & ( static_cast<size_t>( 1 ) << i ) ) != 0) )
                        return PipelinePolicy::statusErrorProtocol;
                    
                    StatusType status = initMessage( i );
                    if ( unlikely(status != PipelinePolicy::statusSuccess) )
                        return status;

                    for ( ;; )
                    {
                        if ( unlikely(zmq_recv( sock, &msgs[ i ], 0 ) != 0) )
                        {
                            if ( zmq_errno() == EINTR )
                                continue;
                            
                            return PipelinePolicy::statusErrorZmqRecv;
                        }

                        break;
                    }
                    
                    int64_t more;
                    size_t  more_size = sizeof( more );
                    if ( unlikely(zmq_getsockopt( sock, ZMQ_RCVMORE, &more, &more_size ) != 0) )
                        return PipelinePolicy::statusErrorZmqGetsockopt;

                    if ( unlikely(( more == 0 ) && ( i < PipelinePolicy::workloadsCount )) )
                        return PipelinePolicy::statusErrorProtocol;
                }

                return PipelinePolicy::statusSuccess;
            }

            StatusType send( void *sock, const char *type = 0 )
            {
                if ( type != 0 )
                {
                    if ( unlikely(( mask & ( static_cast<size_t>( 1 ) << PipelinePolicy::workloadsCount ) ) != 0) )
                        return PipelinePolicy::statusErrorProtocol;
                    
                    StatusType status = initMessage( PipelinePolicy::workloadsCount, type, 1 );
                    if ( unlikely(status != PipelinePolicy::statusSuccess) )
                        return status;
                }

                for ( int i = 0; i < PipelinePolicy::workloadsCount + 1; i++ )
                {
                    if ( ( type != 0 ) && ( *type != 't' ) && ( i < PipelinePolicy::workloadsCount ) )
                    {
                        if ( unlikely(( mask & ( static_cast<size_t>( 1 ) << i ) ) != 0) )
                            return PipelinePolicy::statusErrorProtocol;
                        
                        StatusType status = initMessage( i );
                        if ( unlikely(status != PipelinePolicy::statusSuccess) )
                            return status;
                    }
                    else if ( unlikely(( mask & ( static_cast<size_t>( 1 ) << i ) ) == 0) )
                        return PipelinePolicy::statusErrorProtocol;
                    
                    if ( unlikely(zmq_send( sock, &msgs[ i ], ( i == PipelinePolicy::workloadsCount ? 0 : ZMQ_SNDMORE ) ) != 0) )
                        return PipelinePolicy::statusErrorZmqSend;

                    mask &= ~( static_cast<uint64_t>( 1 ) << i );
                }
            
                return PipelinePolicy::statusSuccess;
            }

            bool isTerminator() { return isOfType( '~' ); }
            bool isAck() { return isOfType( '+' ); }

            StatusType initMessage( int index, const void *data = 0, size_t dataSize = 0 )
            {
                WorkloadVec v( data, dataSize );

                return initMessageV( index, &v, 1 );
            }
            
            StatusType initMessageV( int index, const WorkloadVec *vecs, size_t vecsCount )
            {
                if ( unlikely(( mask & ( static_cast<uint64_t>( 1 ) << index ) ) != 0) )
                    return PipelinePolicy::statusErrorProtocol;

                size_t totalSize = 0;
                for ( size_t i = 0; i < vecsCount; i++ )
                    totalSize += vecs[ i ].dataSize;
                
                if ( totalSize == 0 )
                {
                    if ( unlikely(zmq_msg_init( &msgs[ index ] ) != 0) )
                        return PipelinePolicy::statusErrorZmqMessage;
                }
                else
                {
                    if ( unlikely(zmq_msg_init_size( &msgs[ index ], totalSize ) != 0) )
                        return PipelinePolicy::statusErrorZmqMessage;

                    for ( size_t i = 0, offset = 0; i < vecsCount; offset += vecs[ i ].dataSize, i++ )
                        if ( vecs[ i ].dataSize != 0 )
                            memcpy( reinterpret_cast<uint8_t *>( zmq_msg_data( &msgs[ index ] ) ) + offset,
                                    vecs[ i ].data,
                                    vecs[ i ].dataSize );
                }

                mask |= ( static_cast<uint64_t>( 1 ) << index );

                return PipelinePolicy::statusSuccess;
            }

            StatusType zeroCopyMessage( int index, zmq_msg_t *dataMsg )
            {
                if ( unlikely(( mask & ( static_cast<uint64_t>( 1 ) << index ) ) != 0) )
                    return PipelinePolicy::statusErrorProtocol;

                if ( unlikely(zmq_msg_init( &msgs[ index ] ) != 0) )
                    return PipelinePolicy::statusErrorZmqMessage;
                
                if ( ( zmq_msg_size( dataMsg ) != 0 ) && unlikely(zmq_msg_move( &msgs[ index ], dataMsg ) != 0) )
                    return PipelinePolicy::statusErrorZmqMessage;

                mask |= ( static_cast<uint64_t>( 1 ) << index );

                return PipelinePolicy::statusSuccess;
            }
            
            StatusType getWorkload( int index, const void *&data, size_t &dataSize )
            {
                if ( unlikely(( mask & ( static_cast<uint64_t>( 1 ) << index ) ) == 0) )
                    return PipelinePolicy::statusErrorInvalidArg;

                data = zmq_msg_data( &msgs[ index ] );
                dataSize = zmq_msg_size( &msgs[ index ] );

                return PipelinePolicy::statusSuccess;
            }

            StatusType detachWorkload( int index, zmq_msg_t *dataMsg )
            {
                if ( unlikely(( mask & ( static_cast<uint64_t>( 1 ) << index ) ) == 0) )
                    return PipelinePolicy::statusErrorInvalidArg;

                if ( unlikely(zmq_msg_move( dataMsg, &msgs[ index ] ) != 0) )
                    return PipelinePolicy::statusErrorZmqMessage;

                releaseMessage( index );

                return PipelinePolicy::statusSuccess;
            }
            
        protected:
            bool isOfType( char type )
            {
                return ( ( zmq_msg_size( &msgs[ PipelinePolicy::workloadsCount ] ) == 1 ) &&
                         ( *reinterpret_cast<const char *>( zmq_msg_data( &msgs[ PipelinePolicy::workloadsCount ] ) ) == type ) );
            }
            
            void releaseMessage( int index )
            {
                zmq_msg_close( &msgs[ index ] );
                mask &= ~( static_cast<uint64_t>( 1 ) << index );
            }
            
        protected:
            uint64_t  mask;
            zmq_msg_t msgs[ PipelinePolicy::workloadsCount + 1 ];
        };

    public:
        void clearWorkload() { workloadA.clear(); }

        StatusType setWorkloadV( int index, const WorkloadVec *vecs, size_t vecsCount  )
        {
            if ( unlikely(index >= PipelinePolicy::workloadsCount) )
                return PipelinePolicy::statusErrorInvalidArg;

            return workloadA.initMessageV( index, vecs, vecsCount );
        }
        
        StatusType setWorkload( int index, const void *data, size_t dataSize )
        {
            WorkloadVec v( data, dataSize );

            return setWorkloadV( index, &v, 1 );
        }

        StatusType zeroCopyWorkload( int index, zmq_msg_t *dataMsg )
        {
            if ( unlikely(index >= PipelinePolicy::workloadsCount) )
                return PipelinePolicy::statusErrorInvalidArg;

            return workloadA.zeroCopyMessage( index, dataMsg );
        }
        
        StatusType getWorkload( int index, const void *&data, size_t &dataSize )
        {
            if ( unlikely(index >= PipelinePolicy::workloadsCount) )
                return PipelinePolicy::statusErrorInvalidArg;

            return workloadA.getWorkload( index, data, dataSize );
        }

        StatusType detachWorkload( int index, zmq_msg_t *dataMsg )
        {
            if ( unlikely(index >= PipelinePolicy::workloadsCount) )
                return PipelinePolicy::statusErrorInvalidArg;
            
            return workloadA.detachWorkload( index, dataMsg );
        }

    protected:
        Side sideLeft;
        Side sideRight;
        Workload workloadA;
        Workload workloadB;
    };

    template< typename PipelinePolicy >
    class PipelineHub : public PipelinePeer< PipelinePolicy >
    {
        typedef typename PipelinePolicy::PolicyStatusType StatusType;
        typedef PipelinePeer< PipelinePolicy > Base;
        
    public:
        PipelineHub() : Base(), clientsOnLeft( 0 ), clientsOnRight( 0 ) { }

        StatusType createLeft( void *context, const char *addr )
        {
            clientsOnLeft++;
            
            return Base::sideLeft.create( context, ZMQ_REP, zmq_bind, addr );
        }

        StatusType createRight( void *context, const char *addr )
        {
            clientsOnRight++;
            
            return Base::sideRight.create( context, ZMQ_REP, zmq_bind, addr );
        }

        StatusType send() { return send( "t" ); }
        
        StatusType recv()
        {
            for ( ;; )
            {
                StatusType status = Base::workloadA.recv( Base::sideLeft.getSock() );
                if ( unlikely(status != PipelinePolicy::statusSuccess) )
                    return status;
            
                status = Base::workloadB.send( Base::sideLeft.getSock(), "+" );
                if ( unlikely(status != PipelinePolicy::statusSuccess) )
                    return status;

                if ( likely(!Base::workloadA.isTerminator()) )
                    break;
                
                clientsOnLeft--;
                Base::workloadA.clear();

                if ( clientsOnLeft == 0 )
                    return shutdown();
            }

            return PipelinePolicy::statusSuccess;
        }

        StatusType shutdown()
        {
            for ( ; clientsOnRight > 0; clientsOnRight-- )
            {
                StatusType status = send( "~" );
                if ( unlikely(status != PipelinePolicy::statusSuccess) )
                    return status;
            }
            
            return PipelinePolicy::statusSuccess;
        }
        
        bool terminated() const { return ( clientsOnLeft == 0 ) && ( clientsOnRight == 0 ); }

    protected:
        StatusType send( const char *type )
        {
            StatusType status = Base::workloadB.recv( Base::sideRight.getSock() );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;

            Base::workloadB.clear();

            status = Base::workloadA.send( Base::sideRight.getSock(), type );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;

            return PipelinePolicy::statusSuccess;
        }
        
    protected:
        size_t clientsOnLeft;
        size_t clientsOnRight;
    };

    template< typename PipelinePolicy >
    class PipelineWire : public PipelinePeer< PipelinePolicy >
    {
        typedef typename PipelinePolicy::PolicyStatusType StatusType;
        typedef PipelinePeer< PipelinePolicy > Base;
        
    public:
        PipelineWire() : Base(), terminateReceived( false ) { }

        StatusType createLeft( void *context, const char *addr ) { return Base::sideLeft.create( context, ZMQ_REQ, zmq_connect, addr ); }
        StatusType createRight( void *context, const char *addr ) { return Base::sideRight.create( context, ZMQ_REQ, zmq_connect, addr ); }

        StatusType link( void *context,
                         PipelineHub< PipelinePolicy > &left,
                         const char *leftAddr,
                         PipelineHub< PipelinePolicy > &right,
                         const char *rightAddr )
        {
            StatusType status;

            status = left.createRight( context, leftAddr );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;

            status = right.createLeft( context, rightAddr );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;
            
            status = createLeft( context, leftAddr );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;

            status = createRight( context, rightAddr );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;

            return PipelinePolicy::statusSuccess;
        }
        
        StatusType recv()
        {
            StatusType status = Base::workloadB.send( Base::sideLeft.getSock(), "+" );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;

            status = Base::workloadA.recv( Base::sideLeft.getSock() );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;
            
            if ( unlikely(Base::workloadA.isTerminator()) )
                terminateReceived = true;

            return PipelinePolicy::statusSuccess;
        }

        StatusType send() { return send( "t" ); }
        StatusType transmit() { return send( 0 ); }
        
        bool terminated() const { return terminateReceived; }

        StatusType shutdown() { return send( "~" ); }

    protected:
        StatusType send( const char *type )
        {
            StatusType status = Base::workloadA.send( Base::sideRight.getSock(), type );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;
            
            status = Base::workloadB.recv( Base::sideRight.getSock() );
            if ( unlikely(status != PipelinePolicy::statusSuccess) )
                return status;

            if ( unlikely(!Base::workloadB.isAck()) )
                return PipelinePolicy::statusErrorProtocol;

            Base::workloadB.clear();

            return PipelinePolicy::statusSuccess;
        }
        
    protected:
        bool terminateReceived;
    };

    template< typename PipelinePolicy >
    class PipelineWorker
    {
        typedef typename PipelinePolicy::PolicyStatusType StatusType;

    public:
        virtual ~PipelineWorker() { }
    
        virtual StatusType create() = 0;
        virtual StatusType run() = 0;
    };

    template< typename PipelinePolicy >
    struct PipelineWorkerCreateArgs
    {
        avector< PipelineWorker< PipelinePolicy > * > workers;
        size_t workerIndex;

        PipelineWorkerCreateArgs() : workers(), workerIndex( 0 ) { }
        ~PipelineWorkerCreateArgs()
        {
            for ( size_t i = 0; i < workers.size(); i++ )
                delete workers[ i ];
        }
    };

    struct PipelineWorkerRunArgs { };

    template< typename PipelinePolicy >
    class PipelinePoolWorkerBase
    {
        typedef typename PipelinePolicy::PolicyStatusType StatusType;
        
    public:
        PipelinePoolWorkerBase() : worker( 0 ), runStatus( PipelinePolicy::statusSuccess ) { }

        StatusType create( PipelineWorkerCreateArgs< PipelinePolicy > &args )
        {
            worker = args.workers[ args.workerIndex ];
            args.workerIndex++;
        
            return worker->create();
        }

        void run() { runStatus = worker->run(); }

        StatusType getTaskLastStatus() const { return runStatus; }
        StatusType scheduleTask( PipelineWorkerRunArgs &/* runArgs */ )  { return PipelinePolicy::statusSuccess; }

    protected:
        PipelineWorker< PipelinePolicy > *worker;
        PipelineWorkerRunArgs *runArgs;
        StatusType runStatus;
    };

    template< typename PipelinePolicy >
    struct PipelineWorkerPolicy
    {
        typedef typename PipelinePolicy::PolicyStatusType PolicyStatusType;
        typedef PipelineWorkerCreateArgs< PipelinePolicy > PolicyCreateArgs;
        typedef PipelineWorkerRunArgs PolicyRunArgs;

        static const PolicyStatusType statusSuccess = PipelinePolicy::statusSuccess;
        static const PolicyStatusType statusFail    = PipelinePolicy::statusErrorInternal;
    };

    template< typename PipelinePolicy >
    class PipelineWorkersPool : public APWorkersPool< APWorker< PipelinePoolWorkerBase< PipelinePolicy >,
                                                                PipelineWorkerPolicy< PipelinePolicy > >,
                                                      PipelineWorkerPolicy< PipelinePolicy > >
    {
        typedef typename PipelinePolicy::PolicyStatusType StatusType;
        typedef APWorkersPool< APWorker< PipelinePoolWorkerBase< PipelinePolicy >,
                                         PipelineWorkerPolicy< PipelinePolicy > >,
                               PipelineWorkerPolicy< PipelinePolicy > > Base;
    
    public:
        PipelineWorkersPool() : Base(), createArgs() { }

        StatusType addPipelineNode( PipelineWorker< PipelinePolicy > *worker )
        {
            if ( unlikely(worker == 0) )
                return PipelinePolicy::statusErrorAlloc;
            
            createArgs.workers.push_back( worker );
            if ( unlikely(createArgs.workers.no_memory()) )
                return PipelinePolicy::statusErrorAlloc;

            return PipelinePolicy::statusSuccess;
        }

        StatusType create() { return Base::create( createArgs, createArgs.workers.size() ); }

        StatusType run()
        {
            PipelineWorkerRunArgs runArgs;

            return Base::run( runArgs );
        }
        
    protected:
        PipelineWorkerCreateArgs< PipelinePolicy > createArgs;
    };

    template< typename PipelinePolicy >
    class PipelineWireBypassWorker : public PipelineWorker< PipelinePolicy >,
                                     public PipelineWire< PipelinePolicy >
    {
        typedef typename PipelinePolicy::PolicyStatusType StatusType;
        typedef PipelineWire< PipelinePolicy > WireBase;
        typedef PipelineWorker< PipelinePolicy > WorkerBase;
        
    public:
        PipelineWireBypassWorker() : WireBase(), WorkerBase() { }

        StatusType create() { return PipelinePolicy::statusSuccess; }

        StatusType run()
        {
            StatusType status = PipelinePolicy::statusSuccess;

            while ( !WireBase::terminated() )
            {
                status = WireBase::recv();
                if ( unlikely(status != PipelinePolicy::statusSuccess) )
                    break;

                status = WireBase::send();
                if ( unlikely(status != PipelinePolicy::statusSuccess) )
                    break;
            }
            
            return status;
        }
    };
}

#endif /* _PIPELINE_H_ */

