#include <string.h>
#include <sys/types.h>
# if defined( WIN32 )
#   include <io.h>
# endif  // WIN32

#include <stdlib.h>

#include "_include/_inttype.h"
#include "_include/ntoh.h"

#include "pat_tree.h"
#include "pat_imp.h"

#ifdef _MSC_VER
#pragma warning(disable:4127) // conditional expression is constant
#pragma warning(disable:4702) // unreachable code
#endif

int pth_save(pat_tree *pt, unsigned int flags, void *buf)
{
    int result = -1;

#define SHIFT(x) do { result += (x); buf = ((char*)buf) + (x); } while(0)

    if(flags == PTS_SAVE_DATA_FORMAT)
        {
            /* data storage, all integers stored in host byte order */

            result = 0;

            memcpy(buf, PAT_TREE_HEADER_SIGNATURE, 4);
            SHIFT(4);
            
            *((unsigned char *)buf) = PTS_SAVE_DATA_FORMAT;
            SHIFT(1);

            *((unsigned char *)buf) = NODES_ALIGNMENT;
            SHIFT(1);
            
            *((unsigned char *)buf) = PAT_TREE_FORMAT_VERSION;
            SHIFT(1);

            *((unsigned char *)buf) = 0;
            SHIFT(1);

            *((uint32_t *)buf) = 8 * sizeof(uint32_t);
            SHIFT(sizeof(uint32_t));

            *((uint32_t *)buf) = pt->reverse_index.used;
            SHIFT(sizeof(uint32_t));

            *((uint32_t *)buf) = pt->reverse_index.used * sizeof(uint32_t);
	    SHIFT(sizeof(uint32_t));
 
            *((uint32_t *)buf) = pt->nodes.used;
            SHIFT(sizeof(uint32_t));
            
            *((uint32_t *)buf) = 0;
            SHIFT(sizeof(uint32_t));
 
            memset(buf, 0, 4 * sizeof(uint32_t));
            SHIFT(4 * sizeof(uint32_t));
        }
    else if(flags == PTS_SAVE_TRANSPORT_FORMAT)
        {
            /* transport storage, all intergers stored in network byte order */

            result = 0;

            memcpy(buf, PAT_TREE_HEADER_SIGNATURE, 4);
            SHIFT(4);
            
            *((unsigned char *)buf) = PTS_SAVE_TRANSPORT_FORMAT;
            SHIFT(1);

            *((unsigned char *)buf) = NODES_ALIGNMENT;
            SHIFT(1);
            
            *((unsigned char *)buf) = PAT_TREE_FORMAT_VERSION;
            SHIFT(1);

            *((unsigned char *)buf) = 0;
            SHIFT(1);

            *((uint32_t *)buf) = htobe32(8 * sizeof(uint32_t));
            SHIFT(sizeof(uint32_t));

            *((uint32_t *)buf) = htobe32(pt->reverse_index.used);
            SHIFT(sizeof(uint32_t));

            *((uint32_t *)buf) = htobe32(0); /* offset of data --- tree doesn't stored in this format */
            SHIFT(sizeof(uint32_t));

            *((uint32_t *)buf) = htobe32(0); /* lenght of data --- tree doesn't stored in this format */
            SHIFT(sizeof(uint32_t));
            
            *((uint32_t *)buf) = htobe32(0); /* not used in this format */
            SHIFT(sizeof(uint32_t));

            memset(buf, 0, 4 * sizeof(uint32_t));
            SHIFT(4 * sizeof(uint32_t));
        }

#undef SHIFT

    return result;
}

int pth_fill_fixed_header(struct patricia_tree_header *pth, const void *buf)
{
    int result = -1;
    uint32_t tmp;

#define SHIFT(x) do { result += (x); buf = ((char*)buf) + (x); } while(0)

    if(memcmp(buf, PAT_TREE_HEADER_SIGNATURE, 4) == 0)
        {
            result = 0;

            memset(pth, 0, sizeof(struct patricia_tree_header));

            SHIFT(4);

            pth->format = *((unsigned char *)buf);
            SHIFT(1);

            pth->nodes_alignment = *((unsigned char *)buf);
            SHIFT(1);

            pth->format_version = *((unsigned char *)buf);
            SHIFT(1);
            
            SHIFT(1);

            tmp = *((uint32_t *)buf);
            if(pth->format == PTS_SAVE_TRANSPORT_FORMAT)
                tmp = htobe32(tmp);

            pth->header_rest = tmp;
            SHIFT(sizeof(uint32_t));
        }

#undef SHIFT

    return result;
}

int pth_fill_header_rest(struct patricia_tree_header *pth, const void *buf)
{
    int result = -1;

#define SHIFT(x) do { buf = ((char*)buf) + (x); } while(0)

    if(pth->format == PTS_SAVE_DATA_FORMAT)
        {
            pth->number_of_data_entries = *((uint32_t *)buf);
            SHIFT(sizeof(uint32_t));

            pth->nodes_offset = *((uint32_t *)buf);
            SHIFT(sizeof(uint32_t));
            
            pth->nodes_length = *((uint32_t *)buf);
            SHIFT(sizeof(uint32_t));

            pth->reverse_index_offset = *((uint32_t *)buf);
            SHIFT(sizeof(uint32_t));

            result = pth->header_rest;
        }
    else if(pth->format == PTS_SAVE_TRANSPORT_FORMAT)
        {
            pth->number_of_data_entries = be32toh(*((uint32_t *)buf));
            SHIFT(sizeof(uint32_t));

            pth->nodes_offset = be32toh(*((uint32_t *)buf));
            SHIFT(sizeof(uint32_t));

            pth->nodes_length = be32toh(*((uint32_t *)buf));
            SHIFT(sizeof(uint32_t));
            
            pth->reverse_index_offset = be32toh(*((uint32_t *)buf));
            SHIFT(sizeof(uint32_t));

            result = pth->header_rest;
        }

#undef SHIFT

    return result;
}

#if 0

int pth_load(struct patricia_tree_header *pth, int fd)
{
    unsigned char fixed_header[PAT_TREE_FIXED_HEADER_SIZE];
    unsigned char header_rest[512];
    int read_result = 0;
    int result = -1;

#define RETURN do { goto finish; } while(0)

    memset(pth, 0, sizeof(struct patricia_tree_header));

    read_result = read(fd, fixed_header, PAT_TREE_FIXED_HEADER_SIZE);

    if(read_result < PAT_TREE_FIXED_HEADER_SIZE)
        RETURN;

    if(pth_fill_fixed_header(pth, fixed_header) < 0)
        RETURN;

    if(sizeof(header_rest) < pth->header_rest)
        RETURN;

    read_result = read(fd, header_rest, pth->header_rest);

    if(read_result < (int)pth->header_rest)
        RETURN;

    if(pth_fill_header_rest(pth, header_rest) < 0)
        RETURN;

    result = PAT_TREE_FIXED_HEADER_SIZE + pth->header_rest;

#undef RETURN

  finish:

    return result;
}

#endif

int pth_get(struct patricia_tree_header *pth, const void *buf)
{
    int result = -1;

#define RETURN do { goto finish; } while(0)

    memset(pth, 0, sizeof(struct patricia_tree_header));
    
    if(pth_fill_fixed_header(pth, buf) < 0)
        RETURN;

    buf = ((char*)buf) + PAT_TREE_FIXED_HEADER_SIZE;

    if(pth_fill_header_rest(pth, buf) < 0)
        RETURN;

    result = PAT_TREE_FIXED_HEADER_SIZE + pth->header_rest;

#undef RETURN

  finish:

    return result;  
}

int pth_nget(struct patricia_tree_header *pth, const void *buf, size_t buflen)
{
    int result = -1;

#define RETURN do { goto finish; } while(0)

    memset(pth, 0, sizeof(struct patricia_tree_header));
    
    if(buflen<PAT_TREE_FIXED_HEADER_SIZE)
        RETURN;
 
    if(pth_fill_fixed_header(pth, buf) < 0)
        RETURN;

    buf = ((char*)buf) + PAT_TREE_FIXED_HEADER_SIZE;
    buflen -= PAT_TREE_FIXED_HEADER_SIZE;

    if(buflen<pth->header_rest)
        RETURN;

    if(pth_fill_header_rest(pth, buf) < 0)
        RETURN;

    result = PAT_TREE_FIXED_HEADER_SIZE + pth->header_rest;

#undef RETURN

  finish:

    return result;  

}

static int pat_tree_limited_build_reverse_index(pat_tree *pt)
{
    int result = -1;
    unsigned char *cur_ptr, *last, *first, *prev;

#define RETURN do { goto finish; } while(0)

    if(!pt)
        RETURN;

    cur_ptr = pt->nodes.data;
    last    = ((unsigned char *)pt->nodes.data) + pt->nodes.used-1;

    prev  = cur_ptr;
    first = cur_ptr;

    for( ; ; )
        {
            if(!*cur_ptr)
                {
                    if(pt->reverse_index.used == pt->reverse_index.alloc)
                        {
                            pt->reverse_index.alloc += 1024;
                            
                            pt->reverse_index.offsets = (unsigned int *)realloc(pt->reverse_index.offsets, 
                                                                                pt->reverse_index.alloc*sizeof(unsigned int));
                        }

                    pt->reverse_index.offsets[pt->reverse_index.used] = prev - first;
                    pt->reverse_index.used++;
                    prev = cur_ptr+1;
                }

            if(cur_ptr == last)
                break;
            cur_ptr++;
        }

    result = 0;

#undef RETURN
  finish:

    return result;
}

int pat_tree_limited_set_strings(pat_tree *pt, void *strings, size_t size)
{
    int result = -1;

#define RETURN do { goto finish; } while(0)

    if(!strings || size == 0)
        RETURN;

    if(pt->nodes.data)
        RETURN;

    pt->nodes.data = strings;
    pt->nodes.used = size;

    if(pat_tree_limited_build_reverse_index(pt))
        RETURN;

    result = 0;

#undef RETURN
  finish:

    return result;
}

int pat_tree_limited_copy_in_strings(pat_tree *pt, void *strings, size_t size)
{
    int result = -1;

#define RETURN do { goto finish; } while(0)

    if(!strings || size == 0)
        RETURN;

    if(pt->nodes.data)
        RETURN;

    pt->nodes.data = malloc(size);
    if(!pt->nodes.data)
        RETURN;

    pt->nodes.alloc = size;
    pt->nodes.used  = size;

    memcpy(pt->nodes.data, strings, size);

    if(pat_tree_limited_build_reverse_index(pt))
        RETURN;

    result = 0;

#undef RETURN
  finish:

    return result;
}


