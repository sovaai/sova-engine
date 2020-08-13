#ifndef __alk_pts_imp_h__
#define __alk_pts_imp_h__

#include "pat_tree.h"

struct __patricia_tree_storage {

    struct {
        pat_tree    *pt;
        pat_id       last_id;
    } common;

    struct {
        pat_tree *pt;
    } private;

};


/*
 * XXX This is not used anymore.
 */

/*
 * Patricia Tree Storage header
 *
 * 4 bytes --- signature
 * 1 byte  --- save format
 * 1 byte  --- format version
 * 2 bytes --- padding 
 * 4 bytes --- length of the header rest
 * -------------------- (fixed header finished )
 * 4 bytes --- pat_tree data offset (from header) 
 * 4 bytes --- length of pat_tree data
 * 10*4 bytes --- for future use (zero-filled now)
 */

#define PTS_HEADER_FIXED_SIZE (4 + 1 + 1 + 2 + 4)
#define PTS_HEADER_SIZE       (PTS_HEADER_FIXED_SIZE + 4 + 4 + 10*4)
#define PTS_SIGNATURE "PTSM"
#define PTS_FORMAT_VERSION 1

struct pts_header {
    unsigned char format, format_version;
    size_t        header_rest;
    size_t        pat_tree_offset;
    size_t        pat_tree_length;
};

int pts_header_parse_fixed(struct pts_header *ptsh, void *data);

int pts_header_parse_rest(struct pts_header *ptsh, void *data);



#endif
