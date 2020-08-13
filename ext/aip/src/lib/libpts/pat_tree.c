/*******************************************************************************
 * Proj: Filtration library                                                    *
 * --------------------------------------------------------------------------- *
 * File: pat_tree.c                                                            *
 * Created: Tue Dec  2 16:41:37 2003                                           *
 * Desc: Patricia Tree Stroage                                                 *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  pat_tree.c
 * \brief Patricia Tree Stroage
 */

#include <sys/types.h>
#include <stdlib.h>

# if defined( WIN32 )
#   include <io.h>
# else
#   include <unistd.h>
# endif

#include <string.h>
#include <stdio.h>

#include "pat_tree.h"
#include "pat_imp.h"

#ifdef _MSC_VER
#pragma warning(disable:4127) // conditional expression is constant
#pragma warning(disable:4702) // unreachable code
#endif

/*
 * Creates new patricia structure for filling.
 */
pat_tree *pat_tree_create()
{
    pat_tree *result = NULL;
    pat_char char_finish = PAT_CHAR_FINISH;
    
    result = (pat_tree *)malloc(sizeof(pat_tree));
    if (result == NULL)
        return NULL;
    memset(result, 0, sizeof(pat_tree));

    result->flags |= PAT_TREE_ALLOCATED;

    result->reverse_index.offsets = (unsigned int *)malloc((result->reverse_index.alloc = 1024)*sizeof(unsigned int));
    result->nodes.data            = malloc(result->nodes.alloc = 256*1024);
    
    if ( result->reverse_index.offsets == NULL || result->nodes.data == NULL )
    {
        if ( result->reverse_index.offsets != NULL )
            free(result->reverse_index.offsets);
        if ( result->nodes.data != NULL )
            free(result->nodes.data);
        free(result);
        return NULL;
    }
    
    /* insert head */

    PAT_CLEAR(result, HEAD_NODE);
    PAT_LTAG_SET(result, HEAD_NODE);
    PAT_LLINK_SET(result, HEAD_NODE, HEAD_NODE);
    PAT_ID_SET(result, HEAD_NODE, PAT_ID_NONE);
    
    memcpy(PAT_KEY(result, HEAD_NODE), &char_finish, 1);

    result->nodes.used = PAT_DATA_SIZE + 1;

    for( ; result->nodes.used % NODES_ALIGNMENT; )
        {
            ((unsigned char *)(result->nodes.data))[result->nodes.used] = 0;
            result->nodes.used++;
        }

    return result;
}

int pat_tree_cleanup(pat_tree *pt)
{
    int result = -1;
    pat_char char_finish = PAT_CHAR_FINISH;

#define RETURN do { goto finish; } while(0)

    if(pt->flags & (PAT_TREE_READ_ONLY | PAT_TREE_LIMITED))
        RETURN;

    pt->reverse_index.used = 0;
    pt->nodes.used = 0;

    PAT_CLEAR(pt, HEAD_NODE);
    PAT_LTAG_SET(pt, HEAD_NODE);
    PAT_LLINK_SET(pt, HEAD_NODE, HEAD_NODE);
    PAT_ID_SET(pt, HEAD_NODE, PAT_ID_NONE);
    
    memcpy(PAT_KEY(pt, HEAD_NODE), &char_finish, 1);

    pt->nodes.used = PAT_DATA_SIZE + 1;

    for( ; pt->nodes.used % NODES_ALIGNMENT; )
        {
            ((unsigned char *)(pt->nodes.data))[pt->nodes.used] = 0;
            pt->nodes.used++;
        }

    result = 0;

#undef RETURN
  finish:

    return result;
}


/*
 * Destroys patricia tree
 */

int pat_tree_destroy(pat_tree *pt)
{
    if(pt)
        {
            if(pt->reverse_index.offsets && pt->reverse_index.alloc)
                free(pt->reverse_index.offsets);

            if(pt->nodes.data && pt->nodes.alloc)
                free(pt->nodes.data);
                
#if 0
            if(pt->flags & PAT_TREE_ALLOCATED)
                {
                    if(pt->reverse_index.offsets)
                        free(pt->reverse_index.offsets);

                    if(pt->nodes.data)
                        free(pt->nodes.data);
                }
#endif

            free(pt);
        }

    return 0;
}

static pat_char bit_mask[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };


/*
 * Returns offset of the node, corresponding to key.
 */

struct lookup_state {

    struct {
        unsigned int previous;
        unsigned int current;
    } nodes;

    struct {
        int number;
        int content;
    } bit;
};

static unsigned int pat_tree_lookup_without_validation(pat_tree *pt, const pat_char *key, size_t bits_count, 
                                                       struct lookup_state *ls)
{
    unsigned int node = HEAD_NODE;
    unsigned int next_node;
    int cur_bit = 0;

    /* current bit number --- numbered from 1 */
    unsigned int cur_bit_num = 0;
    
    unsigned int cur_char_index = 0;
    pat_char cur_char = key[cur_char_index];
    unsigned int bit_in_char = 0;

#if 0
    unsigned int current_chars[1000];
    unsigned int skipped = 0;
    unsigned int ps = 0;
    unsigned int pos = 0;
    
    memcpy(current_chars, key, (bits_count / 32) + (bits_count % 32) ? 1 : 0);
#endif

#define RETURN do { goto finish; } while(0)

    for( ; ; )
        {
            if(cur_bit == 0)
                {
                    next_node = PAT_LLINK(pt, node);
                    if(PAT_LTAG(pt, node))
                        {
                            if(ls)
                                {
                                    ls->nodes.previous = node;
                                    ls->nodes.current  = next_node;
                                    ls->bit.number = cur_bit_num;
                                }

                            node = next_node;
                            RETURN;
                        }
                }
            else
                {  /* cur_bit == 1 */
                    
                    next_node = PAT_RLINK(pt, node);
                    if(PAT_RTAG(pt, node))
                        {
                            if(ls)
                                {
                                    ls->nodes.previous = node;
                                    ls->nodes.current  = next_node;
                                    ls->bit.number = cur_bit_num;
                                }

                            node = next_node;
                            RETURN;
                        }
                }
#if 0
            ps = PAT_SKIP(pt, next_node);
            cur_bit_num += ps;
            skipped += ps;
#else
            cur_bit_num += PAT_SKIP(pt, next_node);
#endif

            if(cur_bit_num > bits_count)
                {
                    if(ls)
                        {
                            ls->nodes.previous = node;
                            ls->nodes.current  = next_node;
                            ls->bit.number = cur_bit_num;
                        }

                    node = next_node;
                    RETURN;
                }

#if 0
            if(skipped >= 32)
                {
                    pos += skipped / 32;
                    skipped %= 32;
                    current_chars[pos] <<= skipped;
                }
            else
                {
                    current_chars[pos] <<= ps;
                }
            
            cur_bit = current_chars[pos] & 0x80000000U;

#else
            /* if(PAT_SKIP(pt, node) / 8) */
            /* cur_char = key[(cur_bit_num-1) / 8]; */
            cur_char = key[(cur_bit_num-1) >> 3];

            /* bit_in_char += PAT_SKIP(pt, node) % 8;
               bit_in_char %= 8; */

            bit_in_char = (cur_bit_num - 1) & 0x7;

            cur_bit = cur_char & bit_mask[bit_in_char];
#endif

            node = next_node;
        }

#undef RETURN
  finish:
    return node;
}

static unsigned int common_prefix_length(const pat_char *key1, const pat_char *key2)
{
    int i, j;
    unsigned int common_bits = 0;

    for(i = 0; key1[i] && key2[i]; i++)
        {
            if(key1[i] == key2[i])
                common_bits += 8;
            else 
                break;
        }

    if(key1[i] == key2[i])
        common_bits += 8;
    else 
        {
            for(j = 0; j < 8; j++)
                {
                    if( (key1[i] & bit_mask[j]) == (key2[i] & bit_mask[j]) )
                        common_bits++;
                    else
                        break;
                }
        }

    return common_bits;
}

/*
 * Appends new key to the patricia tree.
 */
pat_id pat_tree_append(pat_tree *pt, const pat_char *key)
{
    pat_id result  = PAT_ID_NONE;
    
    unsigned int cur_node = HEAD_NODE;
    unsigned int i;
    unsigned int key_bits, common_bits;
    const pat_char *cur_key;
    unsigned int key_length = 0;

    unsigned int node_length = 0;
    unsigned int new_node = 0;

    struct lookup_state ls;
    int tag;
    int key_bit;

#define RETURN do { goto finish; } while(0)
    
    if(pt->flags & (PAT_TREE_READ_ONLY | PAT_TREE_LIMITED))
        return PAT_ID_NONE;

    key_bits = 8;
    for(i = 0; key[i]; i++)
        key_bits += 8; 
    key_length = i+1;

    /* finding length of prefix, which already stored in tree */

    cur_node = pat_tree_lookup_without_validation(pt, key, key_bits, NULL);

    cur_key = PAT_KEY(pt, cur_node);
                    
    common_bits = common_prefix_length(key, cur_key);

    if(common_bits == key_bits)
        {
            /* this key is already stored in patricia tree */
            
            result = PAT_ID(pt, cur_node);
            RETURN;
        }

    /* adding node and key to the tree */

    node_length = PAT_DATA_SIZE + key_length;

    if(node_length % NODES_ALIGNMENT)
        node_length = ((node_length / NODES_ALIGNMENT) + 1)*NODES_ALIGNMENT;

    if(pt->nodes.used + node_length >= pt->nodes.alloc)
        {
            pt->nodes.alloc += ((node_length / (256*1024)) + 1)*256*1024;

            pt->nodes.data = realloc(pt->nodes.data, pt->nodes.alloc);
        }

    new_node = pt->nodes.used;
    pt->nodes.used += node_length;

    memset( (char*)pt->nodes.data + new_node, 0, node_length);

    PAT_CLEAR(pt, new_node);
    memcpy(PAT_KEY(pt, new_node), key, key_length);

    if(pt->reverse_index.used == pt->reverse_index.alloc)
        {
            pt->reverse_index.alloc += 1024;

            pt->reverse_index.offsets = (unsigned int *)realloc(pt->reverse_index.offsets, 
                                                                pt->reverse_index.alloc*sizeof(unsigned int));
        }

    PAT_ID_SET(pt, new_node, pt->reverse_index.used+1);
    pt->reverse_index.offsets[pt->reverse_index.used] = (char*)(PAT_KEY(pt, new_node)) - (char*)pt->nodes.data;

    pt->reverse_index.used++;

    /* finding node, which corresponds to given prefix */ 

    memset(&ls, 0, sizeof(ls));
    cur_node = pat_tree_lookup_without_validation(pt, key, common_bits, &ls);

    /* changing the tree */

    tag = 0;

    key_bit = key[(common_bits) / 8] & bit_mask[(common_bits) % 8];

    if(PAT_LLINK(pt, ls.nodes.previous) == ls.nodes.current)
        {
            /* came from left link */

            if(PAT_LTAG(pt, ls.nodes.previous))
                {
                    tag = 1;
                    PAT_LTAG_CLEAR(pt, ls.nodes.previous);
                }

            PAT_LLINK_SET(pt, ls.nodes.previous, new_node);
        }
    else
        {
            /* came from right link */

            if(PAT_RTAG(pt, ls.nodes.previous))
                {
                    tag = 1;
                    PAT_RTAG_CLEAR(pt, ls.nodes.previous);
                }

            PAT_RLINK_SET(pt, ls.nodes.previous, new_node);
        }

    if(key_bit)
        {
            PAT_LLINK_SET(pt, new_node, ls.nodes.current);
            if(tag)
                PAT_LTAG_SET(pt, new_node);
            
            PAT_RLINK_SET(pt, new_node, new_node);
            PAT_RTAG_SET(pt, new_node);
        }
    else
        {
            PAT_RLINK_SET(pt, new_node, ls.nodes.current);
            if(tag)
                PAT_RTAG_SET(pt, new_node);
            
            PAT_LLINK_SET(pt, new_node, new_node);
            PAT_LTAG_SET(pt, new_node);
        }

    if(tag)
        PAT_SKIP_SET(pt, new_node, (1 + common_bits - ls.bit.number));
    else
        {
            PAT_SKIP_SET(pt, new_node, (1 + common_bits - ls.bit.number + PAT_SKIP(pt, ls.nodes.current)));
            PAT_SKIP_SET(pt, ls.nodes.current, (ls.bit.number - common_bits - 1));
        }

    result = PAT_ID(pt, new_node);
    
#undef RETURN
  finish:

    return result;
}

/*
 * Searches for the key in the patricia tree.
 */
pat_id pat_tree_lookup(pat_tree *pt, const pat_char *key, unsigned int mode)
{
    pat_id result = PAT_ID_NONE;
    unsigned int node;
    unsigned int i, key_bits, common_bits;

    if(pt->flags & (PAT_TREE_LIMITED))
        return PAT_ID_NONE;
    
    key_bits = 8;
    for(i = 0; key[i]; i++)
        key_bits += 8; 

    node = pat_tree_lookup_without_validation(pt, key, key_bits, NULL);

    common_bits = common_prefix_length(key, PAT_KEY(pt, node));

    if(mode == PAT_LOOKUP_EXACT_MATCH)
        {
            /* printf("%i %i\n", common_bits, key_bits); */

            if(common_bits != key_bits)
                result = PAT_ID_NONE;
            else
                result = PAT_ID(pt, node);
        }
    else  /* mode == PAT_LOOKUP_BEST_MATCH */
        { 
            node = pat_tree_lookup_without_validation(pt, key, common_bits, NULL);
            result = PAT_ID(pt, node);
        }

    return result;
}

/*
 * Searches for the id in reverse index.
 */
const pat_char *pat_tree_search_id(pat_tree *pt, pat_id pid)
{
    if(pid != PAT_ID_NONE  && pid <= pt->reverse_index.used)
        return (pat_char *)((char*)pt->nodes.data + pt->reverse_index.offsets[pid-1]);
    else
        return NULL;
}


size_t pat_tree_used(pat_tree *pt)
{
    return sizeof(pat_tree) + pt->reverse_index.used*sizeof(unsigned int) + pt->nodes.used;
}

size_t pat_tree_alloc(pat_tree *pt)
{
    return sizeof(pat_tree) + pt->reverse_index.alloc*sizeof(unsigned int) + pt->nodes.alloc;
}

int pat_tree_statistics(pat_tree *pt, void *context, component_info_callback cic)
{
    if(pt && cic)
        {
            cic(context, 
                "pat_tree %p statistics:\n"
                "  type: %s\n"
                "  Reverse index size: %i/%i (%i/%i elements)\n"
                "  Nodes size: %i/%i\n", 
                pt, (pt->flags & PAT_TREE_READ_ONLY) ? "read-only access" : "read-write access",
                pt->reverse_index.used*sizeof(unsigned int), 
                pt->reverse_index.alloc*sizeof(unsigned int),
                pt->reverse_index.used, pt->reverse_index.alloc,
                pt->nodes.used, pt->nodes.alloc);
        }
    
    return 0;
}

int pat_tree_build_info(void *context, component_info_callback cic)
{
    if(cic)
        cic(context, 
            "pat_tree: default build\n"
            "          NODES_ALIGNMENT=%i\n"
            "          PAT_TREE_FORMAT_VERSION=%i\n", 
            NODES_ALIGNMENT, PAT_TREE_FORMAT_VERSION);

    return 0;
}

pat_id pat_tree_last_id(pat_tree *pt)
{
    return pt->reverse_index.used;
}


size_t pat_tree_data_size(pat_tree *pt, unsigned int flags)
{
    if(flags == PTS_SAVE_DATA_FORMAT)
        {
            return PAT_TREE_HEADER_SIZE + pt->nodes.used + pt->reverse_index.used*sizeof(unsigned int);
        }
    else if(flags == PTS_SAVE_TRANSPORT_FORMAT)
        {
            size_t transport_size = PAT_TREE_HEADER_SIZE;
            unsigned int i;
            
            for(i = 0; i < pt->reverse_index.used; i++)
                transport_size += strlen( (char*)pt->nodes.data + pt->reverse_index.offsets[i] ) + 1;
            
            return transport_size;
        }
    else
        return 0;
}

pat_tree *pat_tree_create_ro(void *data, size_t length)
{
    pat_tree *result = NULL;
    struct patricia_tree_header pth;
    int was_read;
    
#define RETURN do { goto finish; } while(0)

    if(length < PAT_TREE_HEADER_SIZE)
        RETURN;

    was_read = pth_nget(&pth, data, length);

    if(was_read < 0)
        RETURN;

    if(pth.format != PTS_SAVE_DATA_FORMAT)
        RETURN;

    data = ((char*)data) + was_read;
    length -= was_read;

    result = (pat_tree *)malloc(sizeof(pat_tree));
    if ( result == NULL )
        return NULL;
    memset(result, 0, sizeof(pat_tree));

    result->flags |= PAT_TREE_READ_ONLY;

    result->reverse_index.offsets = (unsigned*)((char*)data + pth.reverse_index_offset);
    result->reverse_index.used    = pth.number_of_data_entries;
    result->reverse_index.alloc   = 0;

    result->nodes.data            = (char*)data + pth.nodes_offset;
    result->nodes.used            = pth.nodes_length;
    result->nodes.alloc           = 0;

#undef RETURN

  finish:
    return result;
}

pat_tree *pat_tree_create_limited(void *data, size_t length)
{
    pat_tree *result = NULL;
    struct patricia_tree_header pth;
    int dirty = 1;
    int was_read;

#define RETURN do { goto finish; } while(0)

    if(length < PAT_TREE_HEADER_SIZE)
        RETURN;
    
    was_read = pth_nget(&pth, data, length);

    if(was_read < 0)
        RETURN;

    if(pth.format != PTS_SAVE_TRANSPORT_FORMAT)
        RETURN;

    length -= was_read;
    data = ((char *)data) + was_read;

    result = (pat_tree *)malloc(sizeof(pat_tree));
    if ( result == NULL )
        return NULL;
    memset(result, 0, sizeof(pat_tree));

    result->flags |= PAT_TREE_READ_ONLY | PAT_TREE_LIMITED;

    if(pat_tree_limited_set_strings(result, (char*)data, length))
        RETURN;

    dirty = 0;

#undef RETURN

  finish:
    
    if(dirty)
        {
            pat_tree_destroy(result);
            result = NULL;
        }

    return result;
}

/*
 * <eof pat_tree.c>
 */
