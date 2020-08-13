/*******************************************************************************
 * Proj: Content Filtration Library                                            *
 * --------------------------------------------------------------------------- *
 * File: pat_tree.h                                                            *
 * Created: Wed Dec 10 14:35:35 2003                                           *
 * Desc: Patricia tree interface                                               *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  pat_tree.h
 * \brief Patricia tree interface.
 */

#ifndef __pat_tree_h__
#define __pat_tree_h__

#include <sys/types.h>

#include "lib/component_info/component_info.h"
#include "pat_types.h"

#ifdef __cplusplus
extern "C" 
{
#endif

    typedef struct __pat_tree pat_tree;

    pat_tree       *pat_tree_create();

    /* shared access for data format */
    pat_tree       *pat_tree_create_ro(void *data, size_t length);
    /* shared access for transport format (limited) */
    pat_tree       *pat_tree_create_limited(void *data, size_t length);
    int             pat_tree_destroy(pat_tree *pt);

    int             pat_tree_cleanup(pat_tree *pt);

    pat_id          pat_tree_append(pat_tree *pt, const pat_char *key);
    pat_id          pat_tree_lookup(pat_tree *pt, const pat_char *key, unsigned int mode);

    const pat_char *pat_tree_search_id(pat_tree *pt, pat_id pid);

    pat_id          pat_tree_last_id(pat_tree *pt);

    size_t          pat_tree_data_size(pat_tree *pt, unsigned int flags);

    size_t          pat_tree_used(pat_tree *pt);
    size_t          pat_tree_alloc(pat_tree *pt);
    int             pat_tree_statistics(pat_tree *pt, void *context, component_info_callback cic);
    int             pat_tree_build_info(void *context, component_info_callback cic);

#ifdef __cplusplus
}
#endif

#endif /* __pat_tree_h__ */


/*
 * <eof pat_tree.h>
 */
