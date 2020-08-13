/**
 * @file   OpenAddressingDict.h
 * @author swizard <me@swizard.info>
 * @date   Sun Aug 10 17:49:16 2008
 * 
 * @brief  A dictionary data structure based on hash with open addressing collisions resolving.
 * 
 * 
 */
#ifndef _OPENADDRESSINGDICT_H_
#define _OPENADDRESSINGDICT_H_

/* A dictionary based on a hash table with "open addressing" collisions resolving.
 *
 * Algorythm full description: http://en.wikipedia.org/wiki/Open_addressing
 *
 * When to use:
 *
 *  - Need minumum memory usage (maximum compactness).
 *  - Need no clustering (all data should be within one memory chunk)/
 *  - Need easy serialization / deserialization (i.e. into lib/fstorage).
 *  - Need quite good lookup / append operations perfomance.
 *
 * When NOT to use:
 *
 *  - Need maximum lookup / append perfomance.
 *  - Need a lot of remove operations.
 *
 * Template parameters description:
 *
 * Obligatory parameters:
 *  - KeyType: a key type to use. 
 *  - DataType: a data type to use.
 *
 * The hash table element size will be set to (sizeof(KeyType) + sizeof(DataType)) + ~12 bytes overhead.
 *
 * Optional parameters:
 *  - initialSize: initial size of hash table in elements.
 *  - growingIncrement: a linear table growing coefficient: the amount to add to allocation
 *    size each time it should be increased.
 *  - growingPower: a non-linear table growing coefficient: see GrowingPool description for details.
 *  - EqualComparator: a class with defined 'bool areKeysEqual(KeyType firstKey, KeyType secondKey) const' keys comparator.
 *  - Hasher: a class with defined 'uint32_t getHash(KeyType key) const' hashing function.
 *  - rebuildThreshold: a value given in percents indicating a moment for table size increasing.
 *    Should be around 75% capacity for optimal [perfomance / compactness] ratio.
 *    The lower rebuildThreshold, the faster appends/lookups.
 *    The greater rebuildThreshold, the smaller memory usage.
 */

#include <_include/cc_compat.h>

#include "OpenAddressingDict/Common.h"
#include "OpenAddressingDict/BufferDictStorage.h"

template < typename KeyType,
	   typename DataType,
           size_t   initialSize      = 1024,
           size_t   growingIncrement = 1024,
           size_t   growingPower     = maxSizeTBits,
           typename EqualComparator  = KeyEqualComparator<KeyType>,
           typename Hasher           = OpenAddressingHasher<KeyType>,
           typename Prober           = QuadraticProber,
           typename StorageSource    = BufferDictStorage<KeyType, DataType>,
           int      rebuildThreshold = 75 >
class OpenAddressingDict : public virtual OpenAddressingCommon,
                           public Hasher,
                           public EqualComparator,
                           public Prober,
                           public StorageSource
{
public:
        OpenAddressingDict() :
                OpenAddressingCommon(),
                Hasher(),
                EqualComparator(),
                Prober(),
                StorageSource()
        {
        }
        ~OpenAddressingDict()
        {
        }

public:
        /** 
         * Accesses the element within dictionary with the given key and sets its data to the given value.
         * Creates the new element if no elements with the given key exists.
         * 
         * @param key A key for the element
         * @param data A data to set
         * 
         * @return Access status as ResultStatus
         */
        ResultStatus access(KeyType key, DataType data)
        {
                DataType     *dataPtr = 0;
                ResultStatus  status  = access(key, &dataPtr);
                if ( likely(status == RESULT_STATUS_SUCCESS) )
                        *dataPtr = data;
                return status;
        }
        
        /** 
         * Accesses the element within dictionary with the given key and returns a pointer to it.
         * Creates the new element if no elements with the given key exists.
         * 
         * @param key A key for the element
         * @param data A pointer to the data returned (can be null if want no return data)
         * 
         * @return Access status as ResultStatus
         */
        ResultStatus access(KeyType key, DataType **data)
        {
		/* Create the dictionary if not yet */
		if ( unlikely(!StorageSource::isInitialized()) )
		{
			ResultStatus status = resizeTable(initialSize);
			if ( unlikely(status != RESULT_STATUS_SUCCESS) )
				return status;
		}

                size_t currentSize = StorageSource::getSize();
                size_t tableSize   = StorageSource::getTableSize();
                
                /* Check if lazy deletions flush is required */
                if ( unlikely( StorageSource::getRemovesCounter() >= (tableSize - currentSize) ) )
                {
			ResultStatus status  = resizeTable(tableSize);
			if ( unlikely(status != RESULT_STATUS_SUCCESS) )
				return status;
                        currentSize = StorageSource::getSize();
                }

		/* Check if resizing is needed */
		if ( unlikely( (currentSize + 1) * 100 / tableSize >= rebuildThreshold ) )
		{
			size_t       newSize = tableSize + growingIncrement + ( tableSize >> growingPower );
			ResultStatus status  = resizeTable(newSize);
			if ( unlikely(status != RESULT_STATUS_SUCCESS) )
				return status;
		}
                
		/* Obtain the position offset for the hash table */
		NodeLink  nodeLink = findPosition(key);
                
                if ( unlikely(StorageSource::getLastStorageStatus() != RESULT_STATUS_SUCCESS) )
                        return StorageSource::getLastStorageStatus();

                /* Set the data as the return value */
                DataType *dataPtr = StorageSource::getDataPtr(nodeLink);
                
                /* Check if the node is not created yet */
                NodeFlags flags = StorageSource::getFlags(nodeLink);
                if ( !(flags & maskNotEmpty) || (flags & maskIsRemoved) )
                {
                        /* Create the new node */
                        StorageSource::setFlags( nodeLink, (NodeFlags)((flags | maskNotEmpty) & ~maskIsRemoved) );
                        
                        /* Create the key */
                        if ( flags & maskIsRemoved && StorageSource::getRemovesCounter() > 0 )
                                StorageSource::setRemovesCounter( StorageSource::getRemovesCounter() - 1 );
                        else
                        {
                                KeyType *keyPtr =
                                        new( reinterpret_cast<__OpenAddressingDictPtr *>
                                             (StorageSource::getKeyPtr(nodeLink)) ) KeyType();

                                /* Set the key for the node */
                                *( keyPtr ) = key;
                        }
                        
                        /* Append to the list */
                        NodeLink  prevLink = StorageSource::getPrev( static_cast<NodeLink>(0) );
                        StorageSource::setNext( prevLink, nodeLink );
                        StorageSource::setNext( nodeLink, static_cast<NodeLink>(0) );
                        StorageSource::setPrev( nodeLink, prevLink );
                        StorageSource::setPrev( static_cast<NodeLink>(0), nodeLink );
                        
                        /* Increment the elements counter */
                        this->setSize( StorageSource::getSize() + 1 );

                        /* Create and set the data as the return value */
                        dataPtr = new( reinterpret_cast<__OpenAddressingDictPtr *>(StorageSource::getDataPtr(nodeLink)) ) DataType();
                }
                
                if ( data != 0 )
                        *data = dataPtr;
                
                StorageSource::setCurrentModified();
                
                return RESULT_STATUS_SUCCESS;
        }

        /** 
         * Checks if the element with the given key exists in the dictionary and if yes returns a pointer
         * to its data.
         * 
         * @param key A key for the element
         * @param data A pointer to the data returned. Can be null for probe only
         * 
         * @return Get status as ResultStatus
         */
        ResultStatus get(KeyType key, DataType **data) const
        {
                if ( unlikely(StorageSource::getSize() == 0) )
                        return RESULT_STATUS_NOT_FOUND;
                
		/* Obtain the position offset for the hash table */
		NodeLink  nodeLink = findPosition(key);

                if ( unlikely(StorageSource::getLastStorageStatus() != RESULT_STATUS_SUCCESS) )
                        return StorageSource::getLastStorageStatus();
                
                /* Check if the node is not created yet or was removed */
                NodeFlags flags = StorageSource::getFlags(nodeLink);
                if ( !(flags & maskNotEmpty) || (flags & maskIsRemoved) )
                        return RESULT_STATUS_NOT_FOUND;

                /* Set the data as the return value */
                if ( data != 0 )
                        *data = StorageSource::getDataPtr(nodeLink);
                
                return RESULT_STATUS_SUCCESS;
        }
        
        /** 
         * Removes the element with the given key from the dictionary.
         * 
         * @param key A key for the element
         *
         * @return Remove status as ResultStatus
         */
        ResultStatus remove(KeyType key)
        {
                if ( unlikely(!StorageSource::isInitialized()) )
                        return RESULT_STATUS_NOT_FOUND;
                
                return removeAt( static_cast<Iterator>( findPosition(key) ) );
        }

        /** 
         * Removes all elements from the dictionary without freeing its memory.
         * 
         */
        void clear()
        {
                if ( unlikely(!StorageSource::isInitialized()) )
		{
			ResultStatus status = resizeTable(initialSize);
			if ( unlikely(status != RESULT_STATUS_SUCCESS) )
				return;
		}

                /* Check if nothing to do */
                if ( StorageSource::getSize() == 0 )
                        return;
                
                /* Mark all elements as empty */
                size_t tableSize = StorageSource::getTableSize();
                for ( size_t index = 0; index < tableSize; index++)
                {
                        NodeLink   nodeLink = StorageSource::getElementLink(index);
                        NodeFlags  flags    = StorageSource::getFlags(nodeLink);

                        if ( flags & maskNotEmpty && !(flags & maskIsRemoved) )
                        {
                                /* Invoke the destructors */
                                StorageSource::getKeyPtr(nodeLink)->~KeyType();
                                StorageSource::getDataPtr(nodeLink)->~DataType();
                        }
                        
                        StorageSource::setFlags(nodeLink, static_cast<NodeFlags>(maskIsEmpty));
                        StorageSource::setCurrentModified();
                }
                
                /* Clear header */
                StorageSource::setSize( 0 );
                StorageSource::setRemovesCounter( 0 );
                
                /* Cycle the new head links */
                StorageSource::setPrev( static_cast<NodeLink>(0), static_cast<NodeLink>(0) );
                StorageSource::setNext( static_cast<NodeLink>(0), static_cast<NodeLink>(0) );
        }
        
        /** 
         * Returns the number of elements stored in the dictionary.
         * 
         * 
         * @return The elements number
         */
        size_t getCount() const
        {
                size_t count = 0;
                for ( Iterator it = first(); !isEnd(it); it = next(it), count++ );
                return count;
        }

public: /* Iterators */
        
        /** 
         * Returns the iterator position at the beginning of the elements list.
         * 
         * 
         * @return The first element iterator.
         */
        Iterator first() const
        {
                if ( unlikely(!StorageSource::isInitialized()) )
                        return static_cast<NodeLink>(0);
                return StorageSource::getNext( static_cast<NodeLink>(0) );
        }

        /** 
         * Returns the iterator position at the end of the elements list.
         * 
         * 
         * @return The last element iterator.
         */
        Iterator last() const
        {
                if ( unlikely(!StorageSource::isInitialized()) )
                        return static_cast<NodeLink>(0);
                return StorageSource::getPrev( static_cast<NodeLink>(0) );
        }

        /** 
         * Returns the iterator for the next element.
         * 
         * @param it An iterator for which to step forward
         * 
         * @return The next iterator
         */
        Iterator next( Iterator it ) const
        {
                if ( unlikely(!StorageSource::isInitialized()) )
                        return static_cast<NodeLink>(0);
                return StorageSource::getNext( static_cast<NodeLink>(it) );
        }

        /** 
         * Checks if the iterator is at the end of the elements list.
         * 
         * @param it An iterator to check
         * 
         * @return true if no more elements left, false otherwise
         */
        bool isEnd( Iterator it ) const
        {
                if ( unlikely(!StorageSource::isInitialized()) )
                        return true;
                return static_cast<NodeLink>(it) == 0;
        }
        
        /** 
         * Returns the key for the element at the current iterator position.
         * 
         * @param it An iterator to obtain a key for
         * 
         * @return The key const pointer
         */
        const KeyType *getKey( Iterator it ) const
        {
                if ( unlikely(!StorageSource::isInitialized()) )
                        return 0;
                return StorageSource::getKeyPtr( static_cast<NodeLink>(it) );
        }
        
        /** 
         * Returns the data for the element at the current iterator position.
         * 
         * @param it An iterator to obtain a data for
         * 
         * @return The data pointer
         */
        DataType *getData( Iterator it ) const
        {
                if ( unlikely(!StorageSource::isInitialized()) )
                        return 0;
                return StorageSource::getDataPtr( static_cast<NodeLink>(it) );
        }
        
        /** 
         * Removes the element at the given position from the dictionary.
         * 
         * @param it An iterator at which position to delete an element
         *
         * @return Remove status as ResultStatus
         */
        ResultStatus removeAt( Iterator it )
        {
                if ( unlikely(!StorageSource::isInitialized()) )
                        return RESULT_STATUS_NOT_FOUND;
                
                if ( unlikely(StorageSource::getLastStorageStatus() != RESULT_STATUS_SUCCESS) )
                        return StorageSource::getLastStorageStatus();
                
		/* Obtain the position offset for the hash table */
		NodeLink  nodeLink = static_cast<NodeLink>(it);
                
                /* Check if the node is not created yet or was removed */
                NodeFlags flags = StorageSource::getFlags(nodeLink);
                if ( !(flags & maskNotEmpty) || (flags & maskIsRemoved) )
                        return RESULT_STATUS_NOT_FOUND;

                /* Mark the node as removed */
                StorageSource::setFlags(nodeLink, flags | maskIsRemoved);

                /* Remove it from the elements list */
                NodeLink prevLink = StorageSource::getPrev(nodeLink);
                NodeLink nextLink = StorageSource::getNext(nodeLink);
                StorageSource::setNext( prevLink, nextLink );
                StorageSource::setPrev( nextLink, prevLink );
                
                /* Invoke the destructors */
                StorageSource::getKeyPtr(nodeLink)->~KeyType();
                StorageSource::getDataPtr(nodeLink)->~DataType();

                /* Increment removes counter */
                StorageSource::setRemovesCounter( StorageSource::getRemovesCounter() + 1 );
                
                StorageSource::setCurrentModified();
                
                return RESULT_STATUS_SUCCESS;
        }

protected:
        NodeLink findPosition( KeyType key ) const
        {
                size_t tableSize = StorageSource::getTableSize();
                
		/* Perform hashing */
		size_t baseIndex = static_cast<size_t>( this->getHash(key) % tableSize );
		size_t index     = baseIndex;

		/* Search through the table for the key or open space */
		for ( size_t i = 1; ; i++ )
		{
                        NodeLink   nodeLink = StorageSource::getElementLink(index);
			KeyType   *foundKey = StorageSource::getKeyPtr(nodeLink);
                        NodeFlags  flags    = StorageSource::getFlags(nodeLink);
                        
                        if ( unlikely(StorageSource::getLastStorageStatus() != RESULT_STATUS_SUCCESS) )
                                break;
                        
                        /* Stop searching if an empty cell is found */
			if ( !(flags & maskNotEmpty) )
				break;

                        /* Stop searching if keys are equal and non-removed element is found */
                        if ( this->areKeysEqual( *foundKey, key ) )
                                break;

                        index = ( baseIndex + Prober::nextProbe(i) ) % tableSize;
		}

		return StorageSource::getElementLink(index);
        }

        ResultStatus resizeTable( size_t newSize )
        {
                /* Force to have prime number sized hashtable */
                newSize = OpenAddressingCommon::nextPrime( newSize ); 
                
                if ( !StorageSource::isInitialized() )
                        return StorageSource::create( newSize );
                if ( !StorageSource::isResizeAllowed() ) 
                        return RESULT_STATUS_ERROR_USER_MEMORY_RESIZE;

                /* Move the elements to the new bigger memory chunk */
                OpenAddressingDict< KeyType,
                                    DataType,
                                    initialSize,
                                    growingIncrement,
                                    growingPower,
                                    EqualComparator,
                                    Hasher,
                                    Prober,
                                    StorageSource,
                                    rebuildThreshold > target;
                
                ResultStatus status = StorageSource::preAssign( target );
                if ( unlikely(status != RESULT_STATUS_SUCCESS) )
                        return status;
                
                status = target.create( newSize );
                if ( unlikely(status != RESULT_STATUS_SUCCESS) )
                        return status;
                
                for ( Iterator it = first(); !isEnd(it); it = next(it) )
                {
                        DataType     *dataPtr = 0;
                        ResultStatus  status  = target.access( *( getKey(it) ), &dataPtr );
                        if ( unlikely(status != RESULT_STATUS_SUCCESS) )
                                return status;
                        *dataPtr = *( getData(it) );
                }

                /* Replace the current table with the new one */
                status = StorageSource::assign( target );
                if ( unlikely(status != RESULT_STATUS_SUCCESS) )
                        return status;
                
                return RESULT_STATUS_SUCCESS;
        }
     
};

#endif /* _OPENADDRESSINGDICT_H_ */

