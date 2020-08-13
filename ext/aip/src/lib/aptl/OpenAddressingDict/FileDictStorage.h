/**
 * @file   FileDictStorage.h
 * @author swizard <me@swizard.info>
 * @date   Wed Sep 17 01:57:07 2008
 * 
 * @brief  A data source for the open addressing dictionary: a file on a disk.
 * 
 * 
 */
#ifndef _FILEDICTSTORAGE_H_
#define _FILEDICTSTORAGE_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <_include/cc_compat.h>
#include <_include/_inttype.h>
#include <_include/_string.h>

#include "Common.h"

template<typename KeyType, typename DataType>
class FileDictStorage : public virtual OpenAddressingCommon
{
protected:
        enum Constants
        {
                pureHeaderSize        = (sizeof(NodeLink) * 2) + (sizeof(uint32_t) * 3),
                headerSize            = PAD4(pureHeaderSize),
                pureElementHeaderSize = sizeof(NodeLink) + sizeof(NodeLink) + sizeof(NodeFlags),
                elementHeaderSize     = PAD4(pureElementHeaderSize),
                keySize               = PAD4(sizeof(KeyType)),
                dataSize              = PAD4(sizeof(DataType)),
                elementSize           = elementHeaderSize + keySize + dataSize,
        
                offsetPrevNodeLink    = 0,
                offsetNextNodeLink    = offsetPrevNodeLink + sizeof(NodeLink),
        
                offsetCurrentSize     = offsetNextNodeLink + sizeof(NodeLink),
                offsetTableSize       = offsetCurrentSize + sizeof(uint32_t),
                offsetRemovesCounter  = offsetTableSize + sizeof(uint32_t),
        
                offsetFlags           = offsetCurrentSize,
                offsetKey             = elementHeaderSize,
                offsetData            = elementHeaderSize + keySize,

                filenameLength        = 128
        };
        
public:
        FileDictStorage() :
                OpenAddressingCommon(),
                lastStatus( RESULT_STATUS_SUCCESS ),
                source( 0 ),
                userMemoryMode( false )
        {
                filename[0]          = '\0';
                nodeCache.link       = 0;
                nodeCache.isModified = false;
        }
        
        ~FileDictStorage()
        {
                if ( likely(source != 0 && !userMemoryMode) )
                {
                        flushMeta();
                        fclose( source );
                }
        }
        
public:
        /** 
         * Flushes the metadata (dictionary header) to the file.
         * 
         * 
         * @return The flush status
         */
        ResultStatus flushMeta()
        {
                if ( unlikely(!isInitialized()) )
                        return RESULT_STATUS_ERROR_NOT_INITIALIZED;
                
                /* Rewind at the beginning */
                rewind(source);

                /* Write header */
                size_t written = fwrite( headerCache, 1, headerSize, source );
                if ( unlikely(written != headerSize) )
                        return RESULT_STATUS_ERROR_FILE;
                
                /* Write node cache if need to */
                return changeNode( static_cast<NodeLink>(0) );
        }

        /** 
         * Initializes the storage before the first use (sets the file).
         * 
         * @param filename A file to use
         * @param userMemoryMode A mode to use: true for r/o and false for r/w
         *
         * @return Initialization status
         */
        ResultStatus setFile( const char *filename, bool userMemoryMode = false )
        {
                this->userMemoryMode = userMemoryMode;
                strcpy(this->filename, filename);
                return create( 0 );
        }
        
        /** 
         * Returns the raw memory buffer size in bytes.
         * 
         * 
         * @return The raw memory size
         */
        size_t getBufferSize() const
        {
                if ( unlikely(!isInitialized()) )
                        return static_cast<size_t>(0);

                if ( unlikely(fseek(source, 0L, SEEK_END)) )
                        return static_cast<size_t>(0);

                long current = ftell(source);
                if ( unlikely(current < 0) )
                        return static_cast<size_t>(0);
                
                return static_cast<size_t>(current);
        }
        
protected:
        /* Internal creation */
        ResultStatus create( size_t size )
        {
                bool createFlag = false;
                if ( source == 0 )
                {
                        /* Check if the file exists */
                        struct stat statBuf;
                        int statResult = stat( filename, &statBuf );
                        if (statResult < 0)
                        {
                                if (errno != ENOENT)
                                        return RESULT_STATUS_ERROR_FILE;
                                createFlag = true;
                        }
                
                        if ( unlikely(createFlag && userMemoryMode) )
                                return RESULT_STATUS_ERROR_MODE;
                
                        /* Create or open the file */
                        source = fopen( filename, (createFlag ? "w+b" : "r+b") );
                        if ( unlikely(source == 0) )
                                return RESULT_STATUS_ERROR_FILE;
                }
                else
                        createFlag = true;
                
                /* Calculate the size needed */
                size_t totalSize = headerSize + (elementSize * size);
                
                if ( createFlag )
                {
                        /* A new file: resize it and reset the header */
                        if ( unlikely(ftruncate(fileno(source), static_cast<off_t>(totalSize)) < 0) )
                                return RESULT_STATUS_ERROR_FILE;
                        
                        /* Initialize the header */
                        *( getSizePtr()           ) = 0;
                        *( getTableSizePtr()      ) = static_cast<uint32_t>(size);
                        *( getRemovesCounterPtr() ) = 0;
                
                        /* Cycle the new head links */
                        setPrev( static_cast<NodeLink>(0), static_cast<NodeLink>(0) );
                        setNext( static_cast<NodeLink>(0), static_cast<NodeLink>(0) );

                        /* Mark all cells of the new table empty */
                        for ( size_t i = 0; i < size; i++ )
                        {
                                NodeLink nodeLink = getElementLink(i);
                                setFlags(nodeLink, static_cast<NodeFlags>(0));
                        }
                }
                else
                {
                        /* An existing file: load it */
                        rewind( source );
                        size_t bread = fread( headerCache, 1, headerSize, source );
                        if ( unlikely(bread != headerSize) )
                                return RESULT_STATUS_ERROR_FILE;
                }

                return RESULT_STATUS_SUCCESS;
        }
        
        /* Assignation */
        ResultStatus preAssign( FileDictStorage<KeyType, DataType> &target )
        {
                strcpy( target.filename, filename );
                strcat( target.filename, ".XXXXX" );

                /* Open the temporary file */
                int fd = mkstemp( target.filename );
                if ( unlikely(fd < 0) )
                        return RESULT_STATUS_ERROR_FILE;

                /* Wrap a FILE structure over it */
                target.source = fdopen(fd, "w+b");
                if ( unlikely(target.source == 0) )
                        return RESULT_STATUS_ERROR_FILE;
                
                return RESULT_STATUS_SUCCESS;
        }
        
        ResultStatus assign( FileDictStorage<KeyType, DataType> &target )
        {
                if ( likely(source != 0) )
                        fclose(source);
                
                if ( unlikely(rename(target.filename, filename) < 0) )
                        return RESULT_STATUS_ERROR_FILE;

                /* Copy caches */
                memcpy( headerCache, target.headerCache, headerSize );
                memcpy( &nodeCache,  &target.nodeCache,  sizeof(_NodeCache) );
                
                source                = target.source;
                target.userMemoryMode = true;

                return RESULT_STATUS_SUCCESS;
        }
        
private:
        ResultStatus changeNode( NodeLink link ) const
        {
                if ( link == nodeCache.link )
                        return RESULT_STATUS_SUCCESS;
                
                if ( nodeCache.isModified )
                {
                        /* Flush the modified cache to the file */
                        if ( unlikely(fseek(source, static_cast<long>(nodeCache.link), SEEK_SET) < 0) )
                                return RESULT_STATUS_ERROR_FILE;

                        size_t bwrite = fwrite( nodeCache.data, 1, elementSize, source );
                        if ( unlikely(bwrite != elementSize) )
                                return RESULT_STATUS_ERROR_FILE;
                }
                
                _NodeCache *cache = const_cast<_NodeCache *>( &nodeCache );
                
                /* Load the new data */
                cache->link       = link;
                cache->isModified = false;
                
                if ( likely(link != 0) )
                {
                        if ( unlikely(fseek(source, static_cast<long>(link), SEEK_SET) < 0) )
                                return RESULT_STATUS_ERROR_FILE;

                        size_t bread = fread( cache->data, 1, elementSize, source );
                        if ( unlikely(bread != elementSize) )
                                return RESULT_STATUS_ERROR_FILE;
                }
                
                return RESULT_STATUS_SUCCESS;
        }
        
public:
        /* Initialization checker */
        bool isInitialized()   const { return source != 0 && *(getTableSizePtr()) != 0; }
        
protected:
        bool isResizeAllowed() const { return !userMemoryMode;      }
        void setCurrentModified()    { nodeCache.isModified = true; }

        ResultStatus getLastStorageStatus() const { return lastStatus; }
        
        /* currentSize accessors */
        uint32_t *getSizePtr() const
        {
                return const_cast<uint32_t *>
                        ( reinterpret_cast<const uint32_t *>(headerCache + offsetCurrentSize) );
        }
        size_t  getSize() const
        {
                if ( unlikely(!isInitialized()) )
                        return 0;
                return static_cast<size_t>( *(getSizePtr()) );
        }
        void    setSize(size_t size)
        {
                if ( likely(isInitialized()) )
                        *( getSizePtr() ) = static_cast<uint32_t>(size);
        }
        
        /* tableSize accessors */
        uint32_t *getTableSizePtr() const
        {
                return const_cast<uint32_t *>
                        ( reinterpret_cast<const uint32_t *>(headerCache + offsetTableSize) );
        }
        size_t  getTableSize() const
        {
                if ( unlikely(!isInitialized()) )
                        return 0;
                return static_cast<size_t>( *(getTableSizePtr()) );
        }
        void    setTableSize(size_t size)
        {
                if ( likely(isInitialized()) )
                        *( getTableSizePtr() ) = static_cast<uint32_t>(size);
        }

        /* removes counter accessors */
        uint32_t *getRemovesCounterPtr() const
        {
                return const_cast<uint32_t *>
                        ( reinterpret_cast<const uint32_t *>(headerCache + offsetRemovesCounter) );
        }
        size_t  getRemovesCounter() const
        {
                if ( unlikely(!isInitialized()) )
                        return 0;
                return static_cast<size_t>( *(getRemovesCounterPtr()) );
        }
        void    setRemovesCounter(size_t count)
        {
                if ( likely(isInitialized()) )
                        *( getRemovesCounterPtr() ) = static_cast<uint32_t>(count);
        }
        
        /* prev accessors */
        NodeLink *getPrevPtr(NodeLink link) const
        {
                if ( link == static_cast<NodeLink>(0) )
                        return const_cast<NodeLink *>( reinterpret_cast<const NodeLink *>(headerCache + offsetPrevNodeLink) );
                        
                ResultStatus status = changeNode( link );
                if ( unlikely(status != RESULT_STATUS_SUCCESS && lastStatus != RESULT_STATUS_SUCCESS) )
                        *( const_cast<ResultStatus *>(&lastStatus) ) = status;
                
                return const_cast<NodeLink *>( reinterpret_cast<const NodeLink *>(nodeCache.data + offsetPrevNodeLink) );
        }
        NodeLink  getPrev(NodeLink link) const { return *( getPrevPtr(link) ); }
        void      setPrev(NodeLink link, NodeLink prev)
        {
                if ( link == static_cast<NodeLink>(0) || nodeCache.link == link )
                        *( getPrevPtr(link) ) = prev;
                else
                {
                        /* Write the value directly to the file */
                        if ( unlikely(fseek(source, static_cast<long>(link + offsetPrevNodeLink), SEEK_SET) < 0) )
                        {
                                if ( likely(lastStatus == RESULT_STATUS_SUCCESS) )
                                        lastStatus = RESULT_STATUS_ERROR_FILE;
                                return;
                        }

                        size_t bwrite = fwrite( &prev, 1, sizeof(NodeLink), source );
                        if ( unlikely(bwrite != sizeof(NodeLink)) )
                        {
                                if ( likely(lastStatus == RESULT_STATUS_SUCCESS) )
                                        lastStatus = RESULT_STATUS_ERROR_FILE;
                                return;
                        }
                }
        }
        
        /* next accessors */
        NodeLink *getNextPtr(NodeLink link) const
        {
                if ( link == static_cast<NodeLink>(0) )
                        return const_cast<NodeLink *>( reinterpret_cast<const NodeLink *>(headerCache + offsetNextNodeLink) );
                
                ResultStatus status = changeNode( link );
                if ( unlikely(status != RESULT_STATUS_SUCCESS && lastStatus != RESULT_STATUS_SUCCESS) )
                        *( const_cast<ResultStatus *>(&lastStatus) ) = status;
                
                return const_cast<NodeLink *>( reinterpret_cast<const NodeLink *>(nodeCache.data + offsetNextNodeLink) );
        }
        NodeLink  getNext(NodeLink link) const { return *( getNextPtr(link) ); }
        void      setNext(NodeLink link, NodeLink next)
        {
                if ( link == static_cast<NodeLink>(0) || nodeCache.link == link )
                        *( getNextPtr(link) ) = next;
                else
                {
                        /* Write the value directly to the file */
                        if ( unlikely(fseek(source, static_cast<long>(link + offsetNextNodeLink), SEEK_SET) < 0) )
                        {
                                if ( likely(lastStatus == RESULT_STATUS_SUCCESS) )
                                        lastStatus = RESULT_STATUS_ERROR_FILE;
                                return;
                        }

                        size_t bwrite = fwrite( &next, 1, sizeof(NodeLink), source );
                        if ( unlikely(bwrite != sizeof(NodeLink)) )
                        {
                                if ( likely(lastStatus == RESULT_STATUS_SUCCESS) )
                                        lastStatus = RESULT_STATUS_ERROR_FILE;
                                return;
                        }
                }
        }
        
        /* flags accessors */
        uint32_t  *getFlagsPtr(NodeLink link) const
        {
                ResultStatus status = changeNode( link );
                if ( unlikely(status != RESULT_STATUS_SUCCESS && lastStatus != RESULT_STATUS_SUCCESS) )
                        *( const_cast<ResultStatus *>(&lastStatus) ) = status;
                
                return const_cast<uint32_t *>( reinterpret_cast<const uint32_t *>(nodeCache.data + offsetFlags) );
        }
        NodeFlags  getFlags(NodeLink link) const { return static_cast<NodeFlags>(*( getFlagsPtr(link) )); }
        void       setFlags(NodeLink link, NodeFlags flags) { *( getFlagsPtr(link) ) = static_cast<uint32_t>(flags); }

        /* key accessors */
        KeyType *getKeyPtr(NodeLink link) const
        {
                ResultStatus status = changeNode( link );
                if ( unlikely(status != RESULT_STATUS_SUCCESS && lastStatus != RESULT_STATUS_SUCCESS) )
                        *( const_cast<ResultStatus *>(&lastStatus) ) = status;
                
                return const_cast<KeyType *>( reinterpret_cast<const KeyType *>(nodeCache.data + offsetKey) );
        }

        /* data accessors */
        DataType *getDataPtr(NodeLink link) const
        {
                ResultStatus status = changeNode( link );
                if ( unlikely(status != RESULT_STATUS_SUCCESS && lastStatus != RESULT_STATUS_SUCCESS) )
                        *( const_cast<ResultStatus *>(&lastStatus) ) = status;
                
                return const_cast<DataType *>( reinterpret_cast<const DataType *>(nodeCache.data + offsetData) );
        }
        
        /* element accessors */
        NodeLink  getElementLink(size_t index) const
        {
                return static_cast<NodeLink>( headerSize + (index * elementSize) );
        }

protected:
        /* Memory chunk: header + data
         *  
         * Header:
         *  NodeLink lastElement;
         *  NodeLink firstElement;
         *  uint32_t currentSize;
         *  uint32_t tableSize;
         *
         * Data element:
         *  NodeLink  prev;
         *  NodeLink  next;
         *  NodeFlags flags;
         *  KeyType   key;
         *  DataType  data;
         */
        ResultStatus  lastStatus;
        FILE         *source;
        bool          userMemoryMode;
        char          filename[filenameLength];
        uint8_t       headerCache[headerSize];
        struct _NodeCache
        {
                NodeLink link;
                uint8_t  data[elementSize];
                bool     isModified;
        } nodeCache;
};

#endif /* _FILEDICTSTORAGE_H_ */

