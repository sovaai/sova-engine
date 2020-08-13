#ifndef _MMAPCOMMON_H_
#define _MMAPCOMMON_H_

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <_include/_string.h>
#include <_include/cc_compat.h>

struct MmapCommonPolicyRO
{
    static const int openFlags  = O_RDONLY;
    static const int openMode   = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    static const int protection = PROT_READ;
    static const int mmapFlags  = MAP_PRIVATE;
};

struct MmapCommonPolicyRW
{
    static const int openFlags  = O_RDWR | O_CREAT;
    static const int openMode   = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    static const int protection = PROT_READ | PROT_WRITE;
    static const int mmapFlags  = MAP_SHARED;
};

struct MmapCommonPolicyPrivateRW
{
    static const int openFlags  = O_RDWR | O_CREAT;
    static const int openMode   = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    static const int protection = PROT_READ | PROT_WRITE;
    static const int mmapFlags  = MAP_PRIVATE;
};

template< typename BasePolicy >
struct MmapCommonPolicyIntRet : public BasePolicy
{
    typedef int PolicyStatusType;
    
    static const int statusSuccess         = 0;
    static const int statusErrorOpen       = 1;
    static const int statusErrorStat       = 1;
    static const int statusErrorNotCreated = 1;
    static const int statusErrorTruncate   = 1;
    static const int statusErrorSync       = 1;
    static const int statusErrorMmap       = 1;
};

template< typename StatusPolicy, typename BasePolicy >
struct MmapCommonPolicyBase : public StatusPolicy, public BasePolicy { };

template< typename MmapCommonPolicy >
class MmapCommon
{
protected:
    typedef typename MmapCommonPolicy::PolicyStatusType StatusType;
    
public:
    MmapCommon() : mmapFd( -1 ), mmapArea( MAP_FAILED ), mmapSize( 0 ) { }

    ~MmapCommon() { close(); }

    StatusType close()
    {
        if ( likely(mmapArea != MAP_FAILED) )
        {
            ::munmap( mmapArea, mmapSize );
            mmapArea = MAP_FAILED;
        }
        
        if ( likely(mmapFd >= 0) )
        {
            ::close( mmapFd );
            mmapFd = -1;
        }

        return MmapCommonPolicy::statusSuccess;
    }
    
    StatusType open( const char *filename )
    {
        int fd = ::open( filename, MmapCommonPolicy::openFlags, MmapCommonPolicy::openMode );
        if ( unlikely(fd < 0) )
            return MmapCommonPolicy::statusErrorOpen;

        return fdOpen( fd );
    }

    StatusType fdOpen( int fd )
    {
        mmapFd = fd;
        
        struct stat st;
        memset( &st, 0, sizeof( struct stat ) );
            
        int status = ::fstat( mmapFd, &st );
        if ( unlikely(status < 0 ) )
            return MmapCommonPolicy::statusErrorStat;

        mmapSize = st.st_size;

        return MmapCommonPolicy::statusSuccess;
    }
    
    StatusType setSizeBeforeMmap( size_t size )
    {
        if ( unlikely(mmapFd < 0) )
            return MmapCommonPolicy::statusErrorNotCreated;

        if ( unlikely(::ftruncate( mmapFd, size ) < 0) )
            return MmapCommonPolicy::statusErrorTruncate;

        mmapSize = size;
        
        return MmapCommonPolicy::statusSuccess;
    }

    StatusType sync()
    {
        if ( unlikely(mmapFd < 0) )
            return MmapCommonPolicy::statusErrorNotCreated;

        if ( unlikely(::fsync( mmapFd ) < 0) )
            return MmapCommonPolicy::statusErrorSync;

        return MmapCommonPolicy::statusSuccess;
    }

    StatusType mmap()
    {
        if ( unlikely(mmapFd < 0) )
            return MmapCommonPolicy::statusErrorNotCreated;
        if ( unlikely(mmapSize == 0) )
            return MmapCommonPolicy::statusSuccess;

        mmapArea = ::mmap( 0,
                           mmapSize,
                           MmapCommonPolicy::protection,
                           MmapCommonPolicy::mmapFlags,
                           mmapFd,
                           0 );
        if ( unlikely(mmapArea == MAP_FAILED) )
            return MmapCommonPolicy::statusErrorMmap;

        return MmapCommonPolicy::statusSuccess;
    }

    void *getMmapArea() { return mmapArea; }
    const void *getMmapArea() const { return mmapArea; }
    size_t getMmapSize() const { return mmapSize; }
    int getMmapFd() const { return mmapFd; }
    
protected:
    int        mmapFd;
    void      *mmapArea;
    size_t     mmapSize;
};

#endif /* _MMAPCOMMON_H_ */
