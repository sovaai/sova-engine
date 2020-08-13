/**
 * @file   PriorityQueue.h
 * @author swizard <me@swizard.info>
 * @date   Mon Aug 25 00:23:54 2008
 * 
 * @brief  Priority queue lemmatize intermediate results collector.
 * 
 * 
 */
#ifndef _PRIORITYQUEUE_H_
#define _PRIORITYQUEUE_H_

#include <sys/types.h>

#include <lib/libpts2/PatriciaTree.h>

#include <LingProc4/StrictMorphGram/StrictMorphErrors.h>

class PriorityQueue
{
public:
        enum Constants
        {
                poolSize = 32
        };
        
public:
        PriorityQueue( const PatriciaTree &_tree );
        ~PriorityQueue();
        
private:
        PriorityQueue &operator=( const PriorityQueue & )
        {
                return *this;
        }
        
public:
        StrictMorphErrors push( const uint8_t *data, size_t stemIdx, bool isExactMatch );
        
private:
        struct Node
        {
                /* The data */
                const uint8_t *data;
                size_t         stemIdx;
                size_t         dataSize;
                uint32_t       nodeLink;
                const uint8_t *key;
                size_t         keyLength;
                
                /* Next node pointer (single linked list) */
                Node          *nextNode;
        };
                
public:
        typedef Node * Iterator;
        
public:
        Iterator first()            { return queueHead; }
        Iterator next(Iterator it)  { return it->nextNode; }
        bool     isEnd(Iterator it) { return it == 0; }

        const uint8_t *getData(Iterator it)      { return it->data; }
        size_t         getStemIdx(Iterator it)   { return it->stemIdx; }
        uint32_t       getNodeLink(Iterator it)  { return it->nodeLink; }
        const uint8_t *getKey(Iterator it)       { return it->key; }
        size_t         getKeyLength(Iterator it) { return it->keyLength; }
                
private:
        bool hasStopBit( const StemRecordsArray &accessor, size_t stemIdx )
        {
                /* Check the bit */
                return ( accessor.checkStopBit(stemIdx) == static_cast<FlexDistribId>(0) ? false : true );
        }

        bool hasCompoundBit( const StemRecordsArray &accessor, size_t stemIdx )
        {
                /* Check the bit */
                return ( accessor.checkCompositeBit(stemIdx) == static_cast<FlexDistribId>(0) ? false : true );
        }
                
private:
        const PatriciaTree &tree;
        size_t              currentSize;
        Node               *queueHead;
        
        Node                pool[poolSize];
};

#endif /* _PRIORITYQUEUE_H_ */

