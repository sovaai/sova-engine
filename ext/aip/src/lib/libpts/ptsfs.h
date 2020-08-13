/*******************************************************************************
 * Proj: Filtration Library                                                    *
 * --------------------------------------------------------------------------- *
 * File: ptsfs.h                                                               *
 * Created: Thu Jul 27 11:07:02 2003                                           *
 * Desc: Save/load functins for patricia storage.                              *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

#ifndef __alk_ptsfs_h_sentry_
#define __alk_ptsfs_h_sentry_

#include "pat_tree.h"
#include "pat_types.h"
#include "pts.h"
#include "lib/fstorage/fstorage.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /***************************************************************************
     * Patricia tree functions.
     */

    /*
     * This function will set all necessary callbacks in fstorage for 
     * pat_tree saving/loading. 
     */
    int       pat_tree_fs_save(pat_tree *pt, fstorage *fs, fstorage_section_id sec_id, unsigned int flags);
    pat_tree *pat_tree_fs_open_rw(fstorage *fs, fstorage_section_id sec_id);
    pat_tree *pat_tree_fs_open_ro(fstorage *fs, fstorage_section_id sec_id);
    pat_tree *pat_tree_fs_open_limited(fstorage *fs, fstorage_section_id sec_id);

    /***************************************************************************
     * Patricia tree storage functions/
     */
    
    patricia_tree_storage *pts_fs_open_rw(fstorage *fs, fstorage_section_id fssec_id);
    /* read only shared access (only for data format) */
    patricia_tree_storage *pts_fs_open_ro(fstorage *fs, fstorage_section_id fssec_id);

    patricia_tree_storage *pts_fs_open_limited(fstorage *fs, fstorage_section_id fssec_id);
    
    /* flags --- one of PTS_SAVE_* constans */
    int pts_fs_save(patricia_tree_storage *pts, unsigned int flags, 
                    fstorage *fs, fstorage_section_id fssec_id);
    

#ifdef __cplusplus
}
#endif

#endif /* sentry */

