/**
 * @file   BufferDictStorage.h
 * @author swizard <me@swizard.info>
 * @date   Tue Sep 16 17:27:45 2008
 * 
 * @brief  A data source for the open addressing dictionary: a memory buffer.
 * 
 * 
 */
#ifndef _BUFFERDICTSTORAGE_H_
#define _BUFFERDICTSTORAGE_H_

#include <stdlib.h>
#include <sys/types.h>

#include <_include/cc_compat.h>
#include <_include/_inttype.h>
#include <_include/_string.h>

#include "Common.h"

template<typename KeyType, typename DataType>
class BufferDictStorage : public virtual OpenAddressingCommon
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
                offsetData            = elementHeaderSize + keySize
        };
        
public:
        BufferDictStorage() :
                OpenAddressingCommon(),
                lastStatus( RESULT_STATUS_SUCCESS ),
                buffer( 0 ),
                userMemoryMode( false )
        {
        }
        
        ~BufferDictStorage()
        {
                if ( likely(buffer != 0 && !userMemoryMode) )
                        free(buffer);
        }
        
public:
        /** 
         * Return the raw memory chunk with the dictionary data.
         * 
         * 
         * @return The raw memory chunk with data
         */
        const uint8_t *getBuffer() const { return buffer; }
        
        /** 
         * Sets the new memory chunk with the dictionary data.
         * 
         * @param newBuffer The new memory chunk
         */
        void setBuffer( uint8_t *newBuffer )
        {
                if ( unlikely(buffer != 0 && !userMemoryMode) )
                        free(buffer);
                buffer         = newBuffer;
                userMemoryMode = true;
        }
        
        /** 
         * Returns the raw memory buffer size in bytes.
         * 
         * 
         * @return The raw memory size
         */
        size_t getBufferSize() const { return static_cast<size_t>(headerSize + (getTableSize() * elementSize)); }

protected:
        /* Internal creation */
        ResultStatus create( size_t size )
        {
                if ( unlikely(userMemoryMode) )
                        return RESULT_STATUS_ERROR_USER_MEMORY_RESIZE;
                
                /* Calculate the size needed */
                size_t   totalSize = headerSize + (elementSize * size);
                
                if ( likely(buffer != 0) )
                        free(buffer);
                
                /* Allocate the new buffer */
                buffer = reinterpret_cast<uint8_t *>( malloc(totalSize) );
                if ( unlikely(buffer == 0) )
                        return RESULT_STATUS_ERROR_ALLOCATION;
                memset( buffer, 0, totalSize );
                
                /* Initialize the header */
                // *( getSizePtr()           ) = 0;
                *( getTableSizePtr()      ) = static_cast<uint32_t>(size);
                // *( getRemovesCounterPtr() ) = 0;
                
                /* Cycle the new head links */
                // setPrev( static_cast<NodeLink>(0), static_cast<NodeLink>(0) );
                // setNext( static_cast<NodeLink>(0), static_cast<NodeLink>(0) );

                /* Mark all cells of the new table empty */
                // for ( size_t i = 0; i < size; i++ )
                // {
                //         NodeLink nodeLink = getElementLink(i);
                //         setFlags(nodeLink, static_cast<NodeFlags>(0));
                // }

                return RESULT_STATUS_SUCCESS;
        }
        
        /* Assignation */
        ResultStatus preAssign( BufferDictStorage<KeyType, DataType> &/* target */ )
        {
                return RESULT_STATUS_SUCCESS;
        }
        
        ResultStatus assign( BufferDictStorage<KeyType, DataType> &target )
        {
                if ( likely(buffer != 0) )
                        free(buffer);
                
                buffer                = target.buffer;
                target.userMemoryMode = true;

                return RESULT_STATUS_SUCCESS;
        }
        
public:
        /* Initialization checker */
        bool isInitialized()   const { return buffer != 0;     }
        
protected:
        bool isResizeAllowed() const { return !userMemoryMode; }
        void setCurrentModified()    {  }
        
        ResultStatus getLastStorageStatus() const { return lastStatus; }
        
        /* currentSize accessors */
        uint32_t *getSizePtr() const { return reinterpret_cast<uint32_t *>(buffer + offsetCurrentSize); }
        size_t    getSize() const
        {
                if ( unlikely(buffer == 0) )
                        return 0;
                return static_cast<size_t>( *(getSizePtr()) );
        }
        void      setSize(size_t size)
        {
                if ( likely(buffer != 0) )
                        *( getSizePtr() ) = static_cast<uint32_t>(size);
        }
        
        /* tableSize accessors */
        uint32_t *getTableSizePtr() const { return reinterpret_cast<uint32_t *>(buffer + offsetTableSize); }
        size_t    getTableSize() const
        {
                if ( unlikely(buffer == 0) )
                        return 0;
                return static_cast<size_t>( *( getTableSizePtr() ) );
        }
        void      setTableSize(size_t size)
        {
                if ( likely(buffer != 0) )
                        *( getTableSizePtr() ) = static_cast<uint32_t>(size);
        }

        /* removes counter accessors */
        uint32_t *getRemovesCounterPtr() const { return reinterpret_cast<uint32_t *>(buffer + offsetRemovesCounter); }
        size_t    getRemovesCounter() const
        {
                if ( unlikely(buffer == 0) )
                        return 0;
                return static_cast<size_t>( *(getRemovesCounterPtr()) );
        }
        void    setRemovesCounter(size_t count)
        {
                if ( likely(buffer != 0) )
                        *( getRemovesCounterPtr() ) = static_cast<uint32_t>(count);
        }
        
        /* prev accessors */
        NodeLink *getPrevPtr(NodeLink link) const { return reinterpret_cast<NodeLink *>(buffer + link + offsetPrevNodeLink); }
        NodeLink  getPrev(NodeLink link) const { return *( getPrevPtr(link) ); }
        void      setPrev(NodeLink link, NodeLink prev) { *( getPrevPtr(link) ) = prev; }
        
        /* next accessors */
        NodeLink *getNextPtr(NodeLink link) const { return reinterpret_cast<NodeLink *>(buffer + link + offsetNextNodeLink); }
        NodeLink  getNext(NodeLink link) const { return *( getNextPtr(link) ); }
        void      setNext(NodeLink link, NodeLink next) { *( getNextPtr(link) ) = next; }
        
        /* flags accessors */
        uint32_t  *getFlagsPtr(NodeLink link) const { return reinterpret_cast<uint32_t *>(buffer + link + offsetFlags); }
        NodeFlags  getFlags(NodeLink link) const { return static_cast<NodeFlags>(*( getFlagsPtr(link) )); }
        void       setFlags(NodeLink link, NodeFlags flags) { *( getFlagsPtr(link) ) = static_cast<uint32_t>(flags); }

        /* key accessors */
        KeyType *getKeyPtr(NodeLink link) const { return reinterpret_cast<KeyType *>(buffer + link + offsetKey); }

        /* data accessors */
        DataType *getDataPtr(NodeLink link) const { return reinterpret_cast<DataType *>(buffer + link + offsetData); }
        
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
        uint8_t      *buffer;
        bool          userMemoryMode;
};

#endif /* _BUFFERDICTSTORAGE_H_ */

