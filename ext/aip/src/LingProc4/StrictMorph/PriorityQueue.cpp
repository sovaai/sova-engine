/**
 * @file   PriorityQueue.cpp
 * @author swizard <me@swizard.info>
 * @date   Mon Aug 25 00:43:35 2008
 * 
 * @brief  Priority queue lemmatize intermediate results collector (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include "Stem.h"
#include "PriorityQueue.h"

PriorityQueue::PriorityQueue( const PatriciaTree &_tree ) :
        tree( _tree ),
        currentSize( 0 ),
        queueHead( 0 )
{
}

PriorityQueue::~PriorityQueue()
{
}

StrictMorphErrors PriorityQueue::push( const uint8_t *data, size_t stemIdx, bool /* isExactMatch */ )
{
        /* Check the pool limit */
        if ( unlikely(currentSize >= poolSize) )
                return STRICT_ERROR_COMPOUND_WORD_TOO_LONG;

        /* Retrieve the new node */
        Node &newNode = pool[currentSize];

        /* Fill the data */
        newNode.data      = data;
        newNode.stemIdx   = stemIdx;
        newNode.dataSize  = tree.getDataSize(data);
        newNode.nodeLink  = tree.dataAddressToLink(data);
        newNode.key       = tree.linkToKeyAddress(newNode.nodeLink);
        newNode.keyLength = 0;
        for ( ; newNode.key[ newNode.keyLength ] != '\0'; newNode.keyLength++ );
        
        StemRecordsArray newNodeArray( data, newNode.dataSize );
        
        /* Position the new node: insert it after all stop-bits and excludes nodes */
        Node **prevPtr, *current;
        for ( prevPtr = &queueHead, current = queueHead;
              current != 0;
              prevPtr = &current->nextNode, current = current->nextNode )
        {
                StemRecordsArray currentArray( current->data, current->dataSize );

                if ( newNodeArray.checkExcludeBit() && !currentArray.checkExcludeBit() )
                        break;
                else if ( !currentArray.checkExcludeBit() )
                {                    
                        if ( !hasStopBit(currentArray, current->stemIdx) && (newNode.keyLength >= current->keyLength) )
                                break;
                        else if ( hasStopBit(newNodeArray, stemIdx)              &&
                                  hasCompoundBit(currentArray, current->stemIdx) &&
                                  !hasCompoundBit(newNodeArray, stemIdx) )
                                break;
                }
        }
                
        newNode.nextNode = current;
        *prevPtr         = &newNode;
        
        currentSize++;
        
        return STRICT_OK;
}

