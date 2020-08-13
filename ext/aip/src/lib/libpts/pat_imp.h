#ifndef __pat_imp_h__
#define __pat_imp_h__

#include "_include/_inttype.h"

typedef unsigned int pat_link;

#define PAT_RTAG_FLAG 0x80000000U
#define PAT_LTAG_FLAG 0x40000000U

#define NODE_ADDR(PAT, NODE) ((char*)((PAT)->nodes.data) + NODE)

#define PAT_RLINK(PAT, NODE) ( *((pat_link *)NODE_ADDR(PAT, NODE)) )
#define PAT_LLINK(PAT, NODE) ( *((pat_link *)(NODE_ADDR(PAT,NODE) + sizeof(pat_link))) )

#define PAT_LTAG(PAT, NODE)  ( (*(unsigned int *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link))) & PAT_LTAG_FLAG)
#define PAT_RTAG(PAT, NODE)  ( (*(unsigned int *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link))) & PAT_RTAG_FLAG)

#define PAT_SKIP(PAT, NODE)  ( (*(unsigned int *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link))) & ~(PAT_RTAG_FLAG | PAT_LTAG_FLAG))

#define PAT_ID(PAT,NODE)     ( (*(pat_id *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link)+sizeof(unsigned int))) )

#define PAT_RLINK_SET(PAT, NODE, Q) do { PAT_RLINK(PAT, NODE) = (Q); } while(0)
#define PAT_LLINK_SET(PAT, NODE, Q) do { PAT_LLINK(PAT, NODE) = (Q); } while(0)

#define PAT_LTAG_SET(PAT, NODE) do { (*(unsigned int *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link))) |= PAT_LTAG_FLAG; } while(0)
#define PAT_RTAG_SET(PAT, NODE) do { (*(unsigned int *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link))) |= PAT_RTAG_FLAG; } while(0)

#define PAT_LTAG_CLEAR(PAT, NODE) do {  (*(unsigned int *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link))) &= ~PAT_LTAG_FLAG; } while(0)
#define PAT_RTAG_CLEAR(PAT, NODE) do {  (*(unsigned int *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link))) &= ~PAT_RTAG_FLAG; } while(0)

#define PAT_SKIP_SET(PAT, NODE, Q)                                      \
    do {                                                                \
        (*(unsigned int *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link))) &= (PAT_RTAG_FLAG | PAT_LTAG_FLAG); \
        (*(unsigned int *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link))) |= Q & ~(PAT_RTAG_FLAG | PAT_LTAG_FLAG); \
    } while(0)

#define PAT_ID_SET(PAT,NODE,Q)     do { *((pat_id *)(NODE_ADDR(PAT,NODE) + 2*sizeof(pat_link)+sizeof(unsigned int))) = (Q);  } while(0)

#define PAT_DATA_SIZE (2*sizeof(pat_link)+sizeof(unsigned int)+sizeof(pat_id))
#define PAT_CLEAR(PAT,NODE) do { memset(NODE_ADDR(PAT,NODE), 0, PAT_DATA_SIZE); } while(0)
#define PAT_KEY(PAT,NODE) ((pat_char *)(NODE_ADDR(PAT,NODE)+PAT_DATA_SIZE))


#define PAT_TREE_READ_ONLY 0x00000001U
#define PAT_TREE_ALLOCATED 0x00000002U
#define PAT_TREE_LIMITED   0x00000004U

struct __pat_tree {

    unsigned int flags;

    struct {
        unsigned int *offsets;
        size_t used;
        size_t alloc;
    } reverse_index;

    struct {
        void   *data;
        size_t  used;
        size_t  alloc;
    } nodes;

};

#define HEAD_NODE               0
#define NODES_ALIGNMENT         4
#define PAT_TREE_FORMAT_VERSION 1


/*
 * Patricia tree header: 
 *     signature               (4 bytes)
 *     format definition       (1 byte)
 *     nodes alignment         (1 byte)
 *     format version          (1 byte)
 *     zero-padding            (1 bytes)
 *     length of header rest   (4 bytes)
 *     -------------------------------------  (fixed header finished)
 *     number of data entries  (4 bytes)    (reverse index used field)
 *     offset of nodes         (4 bytes)    (from header)
 *     length of nodes data    (4 bytes)
 *     offset of reverse_index (4 bytes)    (from header)
 *     free space              (4*4 bytes)
 */
#define PAT_TREE_FIXED_HEADER_SIZE (4 + 1 + 1 + 1 + 1 + 4)
#define PAT_TREE_HEADER_SIZE (PAT_TREE_FIXED_HEADER_SIZE + 4 + 4 + 4 + 4 + 4*4)

#define PAT_TREE_HEADER_SIGNATURE "PTS1"

struct patricia_tree_header {
    unsigned char format, nodes_alignment, format_version;
    uint32_t      header_rest;
    uint32_t      number_of_data_entries;
    uint32_t      nodes_offset, nodes_length, reverse_index_offset;
};


/* pat_imp.h */

int pth_save(pat_tree *pt, unsigned int flags, void *buf);
int pth_fill_fixed_header(struct patricia_tree_header *pth, const void *buf);
int pth_fill_header_rest(struct patricia_tree_header *pth, const void *buf);
/* int pth_load(struct patricia_tree_header *pth, int fd); */
int pth_get(struct patricia_tree_header *pth, const void *buf);
int pth_nget(struct patricia_tree_header *pth, const void *buf, size_t buflen);

pat_tree *pat_tree_create_limited();
int pat_tree_limited_set_strings(pat_tree *pt, void *strings, size_t size);
int pat_tree_limited_copy_in_strings(pat_tree *pt, void *strings, size_t size);


#endif
