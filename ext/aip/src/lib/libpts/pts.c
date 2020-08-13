/*******************************************************************************
 * Proj: Filtration Library                                                    *
 * --------------------------------------------------------------------------- *
 * File: pts.c                                                                 *
 * Created: Sat Dec 13 18:53:34 2003                                           *
 * Desc: Patricia Tree Storage (implementation)                                *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  pts.c
 * \brief Patricia Tree Storage (implementation)
 */

#include <sys/types.h>
#include <stdlib.h>
# if defined( WIN32 )
#   include "_include/ntoh.h"   /* for ntohl */
#   include <io.h>
# else
#   include <unistd.h>
#   include <netinet/in.h> /* for ntohl */
# endif  /* WIN32 */
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "pts.h"
#include "pat_tree.h"

#include "pts_imp.h"

#ifdef _MSC_VER
#pragma warning(disable:4127) /* conditional expression is constant */
#pragma warning(disable:4702) /* unreachable code */
#endif

patricia_tree_storage *pts_create()
{
    patricia_tree_storage *result = NULL;

    result = (patricia_tree_storage *)malloc(sizeof(patricia_tree_storage));
    if ( result == NULL )
            return NULL;
    memset(result, 0, sizeof(patricia_tree_storage));

    result->private.pt = pat_tree_create();

    return result;
}

#if 0

patricia_tree_storage *pts_load(const char *fname)
{
    patricia_tree_storage *result = NULL;
    int fd = -1;
    struct pts_header ptsh;
    unsigned char buf[512];
    int read_result;
    pat_tree *pt = NULL;

#define RETURN do { goto finish; } while(0)

    fd = open(fname, O_RDONLY);
    if(fd < 0)
        RETURN;
    
    read_result = read(fd, buf, PTS_HEADER_FIXED_SIZE);
    if(read_result < PTS_HEADER_FIXED_SIZE)
        RETURN;

    if(pts_header_parse_fixed(&ptsh, buf) < 0)
        RETURN;

    if(sizeof(buf) < ptsh.header_rest)
        RETURN;

    read_result = read(fd, buf, ptsh.header_rest);
    if(read_result < (int)ptsh.header_rest)
        RETURN;

    if(pts_header_parse_rest(&ptsh, buf) < 0)
        RETURN;

    /* XXX pat_tree_offset is ignored! */

    pt = pat_tree_load(fd, ptsh.pat_tree_length);
    if(pt == NULL)
        RETURN;
    
    result = (patricia_tree_storage *)malloc(sizeof(patricia_tree_storage));
    if ( result == NULL )
            return result;
    memset(result, 0, sizeof(patricia_tree_storage));

    result->private.pt = pt;

    pt = NULL;

#undef RETURN

  finish:

    if(fd >= 0)
        close(fd);

    if(pt)
        pat_tree_destroy(pt);

    return result;
}


int pts_save(patricia_tree_storage *pts, unsigned int flags, int mode, const char *fname)
{
    int result = -1;
    int fd = -1;
    unsigned char header_buf[PTS_HEADER_SIZE];
    void *ptr;
    int write_result = 0;

#define RETURN do { goto finish; } while(0)

    if(pts->common.pt)
        RETURN;

    if(flags != PTS_SAVE_DATA_FORMAT && flags != PTS_SAVE_TRANSPORT_FORMAT)
        RETURN;

    fd = open(fname, O_WRONLY | O_TRUNC | O_CREAT, mode);
    if(fd < 0)
        RETURN;
    
    memset(header_buf, 0, sizeof(header_buf));

    ptr = header_buf;
    memcpy(ptr, PTS_SIGNATURE, 4);
      (char*)ptr += 4;

    *((unsigned char *)ptr) = flags;
      (char*)ptr += 1;

    *((unsigned char *)ptr) = PTS_FORMAT_VERSION;
      (char*)ptr += 1;
    
    (char*)ptr += 2;

    if(flags == PTS_SAVE_DATA_FORMAT)
        *((u_long *)ptr) = 12*sizeof(u_long);
    else
        *((u_long *)ptr) = htonl(12*sizeof(u_long));

    (char*)ptr += sizeof(u_long);

    if(flags == PTS_SAVE_DATA_FORMAT)
        *((u_long *)ptr) = 0;
    else
        *((u_long *)ptr) = htonl(0);
    (char*)ptr += sizeof(u_long);


    if(flags == PTS_SAVE_DATA_FORMAT)
        *((u_long *)ptr) = pat_tree_data_size(pts->private.pt, flags);
    else
        *((u_long *)ptr) = htonl(pat_tree_data_size(pts->private.pt, flags));
    (char*)ptr += sizeof(u_long);

    write_result = write(fd, header_buf, sizeof(header_buf));
    if(write_result < sizeof(header_buf))
        RETURN;

    if(pat_tree_save(pts->private.pt, fd, flags) < 0)
        RETURN;
    
    result = 0;

#undef RETURN

  finish:

    if(fd >= 0)
        close(fd);

    return result;
}
#endif


int pts_cleanup(patricia_tree_storage *pts)
{
    if(!pts || !pts->private.pt)
        return -1;
    else
        return pat_tree_cleanup(pts->private.pt);
}
   
pat_id pts_lookup(patricia_tree_storage *pts, const pat_char *sequence)
{
    pat_id result = PAT_ID_NONE;
    pat_id offset = 0;

    if(pts->common.pt)
        {
            result = pat_tree_lookup(pts->common.pt, sequence, PAT_LOOKUP_EXACT_MATCH);
            if(result != PAT_ID_NONE)
                return result;

            offset = pts->common.last_id;
        }

    if(pts->private.pt) {
        result = pat_tree_lookup(pts->private.pt, sequence, PAT_LOOKUP_EXACT_MATCH);
        if(result != PAT_ID_NONE) 
             result += offset;
    }

    return result;
}

pat_id pts_append(patricia_tree_storage *pts, const pat_char *sequence)
{
    pat_id result = PAT_ID_NONE;
    pat_id offset = 0;

    if(pts->common.pt)
        {
            result = pat_tree_lookup(pts->common.pt, sequence, PAT_LOOKUP_EXACT_MATCH);
            if(result != PAT_ID_NONE)
                return result;

            offset = pts->common.last_id;
        }

    if(pts->private.pt) 
        result = pat_tree_append(pts->private.pt, sequence) + offset;
    
    return result;
}

const pat_char *pts_find_id(patricia_tree_storage *pts, pat_id pid)
{
    if(pid == PAT_ID_NONE)
        return NULL;

    if(pts->common.pt && pid <= pts->common.last_id)
        return pat_tree_search_id(pts->common.pt, pid);
    else
        return pat_tree_search_id(pts->private.pt, pid - pts->common.last_id);
}

size_t pts_used(patricia_tree_storage *pts)
{
    return sizeof(patricia_tree_storage) + 
           pat_tree_used(pts->private.pt) + 
           (pts->common.pt ? pat_tree_used(pts->common.pt) : 0);
}

int pts_items_count(patricia_tree_storage *pts)
{
    return pts->common.last_id + pat_tree_last_id(pts->private.pt);
}

size_t pts_alloc(patricia_tree_storage *pts)
{
    return sizeof(patricia_tree_storage) + pat_tree_alloc(pts->private.pt) + (pts->common.pt ? pat_tree_alloc(pts->common.pt) : 0);
}

int pts_statistics(patricia_tree_storage *pts, void *context, component_info_callback cic)
{
    if(cic)
        {
            cic(context, "Statistics for patricia_tree_storage at %p:\n", pts);

            if(pts->common.pt)
                {
                    cic(context, "Common part:\n");
                    pat_tree_statistics(pts->common.pt, context, cic);
                }
            else
                cic(context, "Common part is absent.\n");

            if(pts->private.pt)
                {
                    cic(context, "Private part:\n");
                    pat_tree_statistics(pts->private.pt, context, cic);
                }
            else
                cic(context, "Private part is absent.\n");

        }

    return 0;
}

int  pts_build_info(void *context, component_info_callback cic)
{
    if(cic)
        {
            cic(context, 
                "patricia_tree_storage: default build\n"
                );

            pat_tree_build_info(context, cic);
            /* shared_file_build_info(context, cic); */
        }

    return 0;
}



/*
 * <eof pts.c>
 */
