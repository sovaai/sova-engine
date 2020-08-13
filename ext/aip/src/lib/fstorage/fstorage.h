/*******************************************************************************
 * Proj: Content Filtration Library                                            *
 * --------------------------------------------------------------------------- *
 * File: fstorage.h                                                            *
 * Created: Mon Jan 19 14:11:34 2004                                           *
 * Desc: Universal data storage.                                               *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  fstorage.h
 * \brief Universal data storage.
 */

#ifndef __fstorage_h__
#define __fstorage_h__

#include <sys/types.h>

#ifdef WIN32
  #include <stddef.h> /* size_t */
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct tag_fstorage fstorage;

    typedef unsigned int fstorage_project_id;
#define FSTORAGE_PROJECT_NONE (0U)

    typedef unsigned int fstorage_section_id;
#define FSTORAGE_SECTION_NONE (0U)

    typedef struct tag_fstorage_section fstorage_section;

    enum fstorage_errors
    {
        FSTORAGE_OK                    =  0,  /* no error */
        FSTORAGE_ERROR_INVALID_PARAMS  = -1,  /* invalid call parameters, 
                                               * i.e hfs == NOSTORAGE */
        FSTORAGE_ERROR_INVALID_MODE    = -2,  /* action is not possible in current open mode 
                                               * or storage is not opened */
        FSTORAGE_ERROR_NOSECTION       = -3,  /* no section with given id */
        FSTORAGE_ERROR_NOMEMORY        = -4,  /* no memory to allocate internal buffer */
        FSTORAGE_ERROR_INVALID_FILE    = -5,  /* file is not fstorage */
        FSTORAGE_ERROR_INVALID_VERS    = -6,  /* invalid fstorage version or project_id */
        FSTORAGE_ERROR_OPEN            = -7,  /* system error while opening file */
        FSTORAGE_ERROR_READ            = -8,  /* system error while reading data from file */
        FSTORAGE_ERROR_WRITE           = -9,  /* system error while writing data to file */
        FSTORAGE_ERROR_INVALID_VERSION = -10, /* Invalid version of fstorage */
        FSTORAGE_ERROR_UNKNOWN         = -11  /* internal program error */
    };

    int fstorage_last_error(fstorage *fs);

    fstorage *fstorage_create();
    fstorage *fstorage_clone(fstorage *fs);
    int       fstorage_destroy(fstorage *fs);

    enum fstorage_open_modes
    {
        FSTORAGE_OPEN_CLOSED = 0, /* see fstorage_open_mode() */
        FSTORAGE_OPEN_READ_ONLY,
        FSTORAGE_OPEN_READ_WRITE
    };

    struct fstorage_connect_config {

        fstorage_project_id pid;

    };


    /* low level function */
    int fstorage_connect(fstorage *fs, const char *path, int open_flags, int open_mode, 
                        enum fstorage_open_modes fsom, struct fstorage_connect_config *fscc);
    
    int fstorage_connect_shared(fstorage *fs, const char *path, struct fstorage_connect_config *fscc);
                        
    int fstorage_close(fstorage *fs);
    int fstorage_purge(fstorage *fs);

    int fstorage_remove_section(fstorage *fs, fstorage_section_id sid);
    fstorage_section *fstorage_section_create(fstorage *fs, fstorage_section_id sid);

    struct fstorage_section_access_config {

        fstorage_section *fss;

        /* read access functions */
        int (* read)   (struct fstorage_section_access_config *fsac, void *buf, size_t nbytes);

        /* write access functions */
        int (* write)  (struct fstorage_section_access_config *fsac, const void *buf, size_t nbytes);
        int (* sync)   (struct fstorage_section_access_config *fsac);

    };

    struct fstorage_stream_section_config {

        void *context;

        int (* on_read)   (struct fstorage_stream_section_config *fscc, 
                           struct fstorage_section_access_config *fsac);

        int (* on_write)  (struct fstorage_stream_section_config *fscc, 
                           struct fstorage_section_access_config *fsac);

        int (* on_destroy)(struct fstorage_stream_section_config *fscc);
    };

    /*
     * Passing NULL in fssc will clear existing information about stream_config. 
     */
    int fstorage_section_stream(fstorage_section *fss, struct fstorage_stream_section_config *fssc);

    const char *fstorage_get_path(fstorage *fs);
    enum fstorage_open_modes fstorage_open_mode(fstorage *fs);
    int fstorage_is_shared(fstorage *fs);

    
    fstorage_section   *fstorage_get_first_section(fstorage *fs);
    fstorage_section   *fstorage_next_section(fstorage_section *fsect);
    fstorage_section   *fstorage_find_section(fstorage *fs, fstorage_section_id sid);

    /* 
     * If section exist, simple returns it. Otherwise, creates it.
     */
    fstorage_section   *fstorage_get_section(fstorage *fs, fstorage_section_id sid);
    fstorage_section   *fstorage_get_sized_section(fstorage *fs, fstorage_section_id sid, size_t size);

    fstorage_section_id  fstorage_section_get_id(fstorage_section *fss);
    size_t      fstorage_section_get_size(fstorage_section *fss);
    size_t      fstorage_section_get_file_ofs(fstorage_section *fss);
    fstorage   *fstorage_section_get_fs(fstorage_section *fss);

    int         fstorage_load_section(fstorage *fs, fstorage_section_id sid);
    int         fstorage_section_load(fstorage_section *fss);

    void  *fstorage_section_get_data(fstorage_section *fss, size_t offset, size_t amount);
    void  *fstorage_section_get_all_data(fstorage_section *fss);
    int    fstorage_section_set_data(fstorage_section *fss, void *data, size_t size);
    int    fstorage_section_copy_data(fstorage_section *fss, void *data, size_t size);
    int    fstorage_section_realloc(fstorage_section *fss, size_t size);

    /* Debug functions */

    size_t fstorage_section_get_offset(fstorage_section *fss);
        
    /* Transactions support */
    
    enum fstorage_trx_mode
    {
        FSTORAGE_TRX_READ_ONLY = 0,
        FSTORAGE_TRX_READ_WRITE
    };
    
    int fstorage_trx_snapshot(fstorage *fs, enum fstorage_trx_mode mode);
    int fstorage_trx_rollback(fstorage *fs);
    int fstorage_trx_provide_section(fstorage *fs, enum fstorage_trx_mode mode, fstorage_section_id sid, void *buf, size_t buf_size);
    
    struct fstorage_trx_committer
    {
        void *context;
        int (*receive_section)(struct fstorage_trx_committer *fscm, fstorage_section_id sid, void *buf, size_t buf_size);
    };

    int fstorage_trx_commit(fstorage *fs, struct fstorage_trx_committer *fscm);
        
#ifdef __cplusplus
}
#endif


#endif /* __fstorage_h__ */

/*
 * <eof fstorage.h>
 */
