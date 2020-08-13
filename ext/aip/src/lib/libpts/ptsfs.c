#include <string.h>
#include <sys/types.h>
# if defined( WIN32 )
#   include "_include/ntoh.h"   /* for ntohl */
# else
#   include <netinet/in.h> /* for ntohl */
# endif
#include "pat_tree.h"
#include "pat_imp.h"
#include "pts.h"
#include "pts_imp.h"
#include "lib/fstorage/fstorage.h"

#ifdef _MSC_VER
#pragma warning(disable:4127) /* conditional expression is constant */
#pragma warning(disable:4702) /* unreachable code */
#endif

#include <stdio.h>

/*******************************************************************/


struct pat_tree_fs_context {

    pat_tree     *pt;
    unsigned int  flags;
    unsigned char buf[30*1024];
    int limited;
};

static int pat_tree_on_write(struct fstorage_stream_section_config *fscc, 
                             struct fstorage_section_access_config *fsac)
{
    int result = -1;
    int write_result = 0;
    size_t buf_used = 0;

    pat_tree *pt = NULL;
    struct pat_tree_fs_context *context = NULL;

#define RETURN do { goto finish; } while(0)

    if(fscc == NULL || fsac == NULL)
        RETURN;

    context = (struct pat_tree_fs_context *)fscc->context;
    if(!context)
        RETURN;

    pt = context->pt;
    if(!pt)
        RETURN;

    if(pth_save(pt, context->flags, context->buf) < 0)
        RETURN;

    write_result = fsac->write(fsac, context->buf, PAT_TREE_HEADER_SIZE);


    if(write_result < PAT_TREE_HEADER_SIZE)
        RETURN;

    if(context->flags == PTS_SAVE_DATA_FORMAT)
        {
            write_result = fsac->write(fsac, pt->reverse_index.offsets, pt->reverse_index.used*sizeof(unsigned int));
            if(write_result < (int)(pt->reverse_index.used*sizeof(unsigned int)))
                RETURN;

            write_result = fsac->write(fsac, pt->nodes.data, pt->nodes.used);
            if(write_result < (int)pt->nodes.used)
                RETURN;
        }
    else if(context->flags == PTS_SAVE_TRANSPORT_FORMAT)
        {
            unsigned int i;
            size_t length;

            for(i = 0; i < pt->reverse_index.used; i++)
                {
                    length = strlen( (char*)pt->nodes.data + pt->reverse_index.offsets[i] ) + 1;

                    if(buf_used && buf_used + length > sizeof(context->buf))
                        {
                            write_result = fsac->write(fsac, context->buf, buf_used);
                            if(write_result < (int)buf_used)
                                RETURN;

                            buf_used = 0;
                        }

                    if(length >= sizeof(context->buf))
                        {
                            write_result = fsac->write(fsac, (char*)pt->nodes.data + pt->reverse_index.offsets[i], length);
                            if(write_result < (int)length)
                                RETURN;
                        }
                    else
                        {
                            memcpy(context->buf + buf_used, (char*)pt->nodes.data + pt->reverse_index.offsets[i], length);
                            buf_used += length;
                        }
                }

            if(buf_used)
                {
                    write_result = fsac->write(fsac, context->buf, buf_used);
                    if(write_result < (int)buf_used)
                        RETURN;
                    
                    buf_used = 0;
                }
        }
    else
        RETURN;

    result = 0;

#undef RETURN

  finish:

    return result;
}

static int pth_load_fs(struct patricia_tree_header *pth, struct fstorage_section_access_config *fsac)
{
    unsigned char fixed_header[PAT_TREE_FIXED_HEADER_SIZE];
    unsigned char header_rest[512];
    int read_result = 0;
    int result = -1;

#define RETURN do { goto finish; } while(0)

    memset(pth, 0, sizeof(struct patricia_tree_header));

    read_result = fsac->read(fsac, fixed_header, PAT_TREE_FIXED_HEADER_SIZE);

    if(read_result < PAT_TREE_FIXED_HEADER_SIZE)
        RETURN;

    if(pth_fill_fixed_header(pth, fixed_header) < 0)
        RETURN;

    if(sizeof(header_rest) < pth->header_rest)
        RETURN;

    read_result = fsac->read(fsac, header_rest, pth->header_rest);

    if(read_result < (int)pth->header_rest)
        RETURN;

    if(pth_fill_header_rest(pth, header_rest) < 0)
        RETURN;

    result = PAT_TREE_FIXED_HEADER_SIZE + pth->header_rest;

#undef RETURN

  finish:

    return result;
}

static int pat_tree_on_read(struct fstorage_stream_section_config *fscc, 
                            struct fstorage_section_access_config *fsac)
{
    int result = -1;
    size_t buf_used = 0;

    struct pat_tree_fs_context *context = NULL;

    struct patricia_tree_header pth;

    unsigned int *offsets = NULL;
    void *data = NULL;

    int read_result = 0;

    pat_char *buf = NULL;
    size_t buf_length = 0;
    size_t cur_finish;
    unsigned int i;

    size_t length;
    int dirty = 0;

#define RETURN do { goto finish; } while(0)

    if(fscc == NULL || fsac == NULL)
        RETURN;

    context = (struct pat_tree_fs_context *)fscc->context;
    if(!context)
        RETURN;

    length = fstorage_section_get_size(fsac->fss);

    if(length < PAT_TREE_HEADER_SIZE)
        RETURN;
    
    if(pth_load_fs(&pth, fsac) < 0)
        RETURN;

    if(length < PAT_TREE_FIXED_HEADER_SIZE + pth.header_rest)
        RETURN;

    length -= PAT_TREE_FIXED_HEADER_SIZE + pth.header_rest;

    /* XXX needs length and header sizes consistency check */

    if(pth.format == PTS_SAVE_DATA_FORMAT)
        {
            /* XXX here we must pay attention to reverse_index_offset and nodes_offset */

            offsets = (unsigned int *)malloc(pth.number_of_data_entries*sizeof(unsigned int));
            if ( offsets == NULL )
                RETURN;
            read_result = fsac->read(fsac, offsets, pth.number_of_data_entries*sizeof(unsigned int));
            if(read_result < 0)
                RETURN;

            data = malloc(pth.nodes_length);
            if ( data == NULL )
                RETURN;
            read_result = fsac->read(fsac, data, pth.nodes_length);
            if(read_result < 0)
                RETURN;
            
            context->pt = (pat_tree *)calloc(1, sizeof(pat_tree));
            
            context->pt->flags |= PAT_TREE_ALLOCATED;

            if(context->limited)
                {
                    /* Really, for data format limited access is nonsense,
                     * but... may be... user really wants it.
                     */

                    context->pt->flags |= PAT_TREE_LIMITED;
                }
            
            context->pt->reverse_index.offsets = offsets;
            context->pt->reverse_index.used    = pth.number_of_data_entries;
            context->pt->reverse_index.alloc   = pth.number_of_data_entries;
            
            context->pt->nodes.data            = data;
            context->pt->nodes.used            = pth.nodes_length;
            context->pt->nodes.alloc           = pth.nodes_length;

            offsets = NULL;
            data    = NULL;
        }
    else if(pth.format == PTS_SAVE_TRANSPORT_FORMAT)
        {
            unsigned int to_read;

            if(!context->limited)
                {
                    context->pt = pat_tree_create();
            
                    dirty = 1;

                    if(!buf)
                        {
                            buf_length = 2048;
                            buf = (pat_char *)malloc(buf_length);
                            if ( buf == NULL )
                                RETURN;
                            buf_used = 0;
                        }

                    cur_finish = 0;
                    
                    for(i = 0; i < pth.number_of_data_entries; i++)
                        {
                            cur_finish = 0;
                            
                            for( ; ; )
                                {
                                    for( ; cur_finish < buf_used; )
                                        {
                                            if(buf[cur_finish] == PAT_CHAR_FINISH)
                                                break;
                                            
                                            cur_finish++;
                                        }
                                    
                                    if(cur_finish > 0 && cur_finish < buf_used)
                                        {
                                            break;
                                        }
                                    else
                                        {
                                            if(buf_used == buf_length)
                                                {
                                                    if(length == 0)
                                                        RETURN;
                                                    
                                                    buf_length += 2048 > length ? length : 2048;
                                                }
                                            
                                            if(buf_length - buf_used > length)
                                                to_read = length;
                                            else
                                                to_read = buf_length - buf_used;
                                            
                                            read_result = fsac->read(fsac, buf+buf_used, to_read);
                                            if(read_result < (int)to_read)
                                                RETURN;
                                            
                                            length -= to_read;
                                            buf_used += to_read;
                                        }
                                }
                            
                            if(pat_tree_append(context->pt, buf) == PAT_ID_NONE)
                                RETURN;
                            
                            memmove(buf, buf+cur_finish+1, buf_used-cur_finish-1);
                            buf_used -= cur_finish+1;
                        }

                    dirty = 0;
                }
            else
                {
                    /* Creating limited edition */

                    buf = (pat_char *)malloc(length);
                    if(!buf)
                        RETURN;

                    read_result = fsac->read(fsac, buf, length);
                    if(read_result < (int)length)
                        RETURN;
                    
                    context->pt = (pat_tree *)calloc(1, sizeof(pat_tree));
                    if(!context->pt)
                        RETURN;

                    context->pt->flags |= PAT_TREE_LIMITED | PAT_TREE_READ_ONLY;

                    if(pat_tree_limited_set_strings(context->pt, buf, length))
                        {
                            pat_tree_destroy(context->pt);
                            RETURN;
                        }

                    context->pt->nodes.alloc = length; /* XXX My knowlege about internal structure of pat_tree */
                    buf = NULL;

                }
        }
    else
        RETURN;

    result = 0;

#undef RETURN

  finish:
    
    if(offsets)
        free(offsets);

    if(data)
        free(data);
    
    if(dirty)
        {
            pat_tree_destroy(context->pt);
            context->pt = NULL;
        }

    if(buf)
        free(buf);

    return result;
}

static int pat_tree_on_destroy(struct fstorage_stream_section_config *fscc)
{
    if(fscc && fscc->context)
        free(fscc->context);

    return 0;
}


int pat_tree_fs_save(pat_tree *pt, fstorage *fs, fstorage_section_id sec_id, unsigned int flags)
{
    int result = -1;
    fstorage_section *sect;
    struct pat_tree_fs_context *context = NULL;
    struct fstorage_stream_section_config fscc;

#define RETURN do { goto finish; } while(0)

    sect = fstorage_get_section(fs, sec_id);
    if(!sect)
        RETURN;

    context = (struct pat_tree_fs_context *)calloc(1, sizeof(struct pat_tree_fs_context));
    if(!context)
        RETURN;

    context->pt = pt;
    context->flags = flags;

    memset(&fscc, 0, sizeof(fscc));

    fscc.context    = context;
    fscc.on_write   = pat_tree_on_write;
    fscc.on_destroy = pat_tree_on_destroy;

    if(fstorage_section_stream(sect, &fscc))
        {
            free(context);
            RETURN;
        }

    result = 0;

#undef RETURN

  finish:
    return result;
}


static pat_tree *my_pat_tree_fs_open(fstorage *fs, fstorage_section_id sec_id, int limited)
{
    pat_tree *result = NULL;
    fstorage_section *sect;
    struct pat_tree_fs_context *context = NULL;
    struct fstorage_stream_section_config fscc;

#define RETURN do { goto finish; } while(0)

    sect = fstorage_get_section(fs, sec_id);
    if(!sect)
        RETURN;

    context = (struct pat_tree_fs_context *)calloc(1, sizeof(struct pat_tree_fs_context));
    if(!context)
        RETURN;

    memset(&fscc, 0, sizeof(fscc));

    context->limited = limited;

    fscc.context    = context;
    fscc.on_read    = pat_tree_on_read;
    fscc.on_destroy = pat_tree_on_destroy;

    if(fstorage_section_stream(sect, &fscc))
        {
            free(context);
            RETURN;
        }

    fstorage_section_load(sect);

    if(context->pt)
        result = context->pt;

    fstorage_section_stream(sect, NULL);

    free(context);

#undef RETURN

  finish:
    return result;
}

pat_tree *pat_tree_fs_open_rw(fstorage *fs, fstorage_section_id sec_id)
{
    return my_pat_tree_fs_open(fs, sec_id, 0);
}

pat_tree *pat_tree_fs_open_limited(fstorage *fs, fstorage_section_id sec_id)
{
    return my_pat_tree_fs_open(fs, sec_id, 1);
}

pat_tree *pat_tree_fs_open_ro(fstorage *fs, fstorage_section_id sec_id)
{
    pat_tree *result = NULL;
    fstorage_section *sect;
    
    void *ptr = NULL;
    size_t length = 0;

#define RETURN do { goto finish; } while(0)

    sect = fstorage_find_section(fs, sec_id);
    if(!sect)
        RETURN;
    
    ptr = fstorage_section_get_all_data(sect);
    length = fstorage_section_get_size(sect);

    result = pat_tree_create_ro(ptr, length);
    if(!result)
        RETURN;

#undef RETURN
  finish:
    
    return result;
}

/*******************************************************************************
 * PTS functions.
 */

patricia_tree_storage *pts_fs_open_rw(fstorage *fs, fstorage_section_id fssec_id)
{
    patricia_tree_storage *result = NULL;
    pat_tree *pt = NULL;
    int dirty = 1;

#define RETURN do { goto finish; } while(0)

    pt = pat_tree_fs_open_rw(fs, fssec_id);
    if(!pt)
        RETURN;
    
    result = (patricia_tree_storage *)calloc(1, sizeof(patricia_tree_storage));
    if(!result)
        RETURN;

    result->private.pt = pt;
    pt = NULL;
    
    dirty = 0;

#undef RETURN
  finish:

    if(dirty)
        {
            if(pt)
                pat_tree_destroy(pt);

            if(result)
                pts_destroy(result);

            result = NULL;
        }
    
    return result;
}

patricia_tree_storage *pts_fs_open_ro(fstorage *fs, fstorage_section_id fssec_id)
{
    patricia_tree_storage *result = NULL;
    pat_tree *pt = NULL;
    int dirty = 1;

#define RETURN do { goto finish; } while(0)

    pt = pat_tree_fs_open_ro(fs, fssec_id);
    if(!pt)
        RETURN;
    
    result = pts_create();
    result->common.pt = pt;
    result->common.last_id = pat_tree_last_id(result->common.pt);
    pt = NULL;
    
    dirty = 0;

#undef RETURN
  finish:

    if(dirty)
        {
            if(pt)
                pat_tree_destroy(pt);

            if(result)
                pts_destroy(result);

            result = NULL;
        }
    
    return result;
}

patricia_tree_storage *pts_fs_open_limited(fstorage *fs, fstorage_section_id fssec_id)
{
    patricia_tree_storage *result = NULL;
    pat_tree *pt = NULL;
    int dirty = 1;

#define RETURN do { goto finish; } while(0)

    pt = pat_tree_fs_open_limited(fs, fssec_id);
    if(!pt)
        RETURN;
    
    result = (patricia_tree_storage *)calloc(1, sizeof(patricia_tree_storage));
    if(!result)
        RETURN;

    result->private.pt = pt;
    pt = NULL;
    
    dirty = 0;

#undef RETURN
  finish:

    if(dirty)
        {
            if(pt)
                pat_tree_destroy(pt);

            if(result)
                pts_destroy(result);

            result = NULL;
        }
    
    return result;
}
    
int pts_fs_save(patricia_tree_storage *pts, unsigned int flags, fstorage *fs, fstorage_section_id fssec_id)
{
    int result = -1;

#define RETURN do { goto finish; } while(0)

    if(pts->common.pt)
        RETURN;

    if(flags != PTS_SAVE_DATA_FORMAT && flags != PTS_SAVE_TRANSPORT_FORMAT)
        RETURN;

    if(pat_tree_fs_save(pts->private.pt, fs, fssec_id, flags))
        RETURN;

    result = 0;

#undef RETURN
  finish:

    return result;
}

int pts_destroy(patricia_tree_storage *pts)
{
    if(pts)
        {
            if(pts->common.pt)
                pat_tree_destroy(pts->common.pt);

            if(pts->private.pt)
                pat_tree_destroy(pts->private.pt);

            free(pts);
        }

    return 0;
}

