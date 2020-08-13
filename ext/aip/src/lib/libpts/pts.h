/*******************************************************************************
 * Proj: Filtration Library                                                    *
 * --------------------------------------------------------------------------- *
 * File: pts.h                                                                 *
 * Created: Thu Nov 27 11:07:02 2003                                           *
 * Desc: Patricia Tree Storage                                                 *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  pts.h
 * \brief Patricia Tree Storage (Practical Algorithm to Retrieval Information 
 *        Coded in Alphanumeric)
 */

#ifndef __alk_pts_h__
#define __alk_pts_h__

#include <sys/types.h>
#include <stdlib.h>
#include "lib/component_info/component_info.h"
#include "pat_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct __patricia_tree_storage patricia_tree_storage;

    /* database create and append exclusive access */

    patricia_tree_storage *pts_create();
    patricia_tree_storage *pts_load(const char *fname);

    /*
     * flags --- one of PTS_SAVE_* constans;
     * mode --- unix file mode.
     */
    int                    pts_save(patricia_tree_storage *pts, unsigned int flags, 
                                    int mode, const char *fname);
    
    int                    pts_destroy(patricia_tree_storage *pts);

    /* destroys dinamic part of the read-only storage */
    int                    pts_cleanup(patricia_tree_storage *pts);

    /* operations with patricia tree */

    /* find sequence in the storage; returns PTS_ID_NONE if sequence
     * absent in the tree.
     */
    pat_id            pts_lookup(patricia_tree_storage *pts, const pat_char *sequence);
    
    /* appends sequence to the storage; if sequence already exist,
     * returns pts_id of stored sequence.
     */
    pat_id            pts_append(patricia_tree_storage *pts, const pat_char *sequence);

    const pat_char   *pts_find_id(patricia_tree_storage *pts, pat_id pid);

    size_t            pts_used(patricia_tree_storage *pts);
    size_t            pts_alloc(patricia_tree_storage *pts);
    int               pts_items_count(patricia_tree_storage *pts);
    int               pts_statistics(patricia_tree_storage *pts, void *context, component_info_callback cic);
    int               pts_build_info(void *context, component_info_callback cic);

#ifdef __cplusplus
}
#endif

#endif /* __alk_pts_h__ */

/*
 * <eof pts.h>
 */
