/*******************************************************************************
 * Proj: mm_file                                                               *
 * --------------------------------------------------------------------------- *
 * File: mm_file.h                                                             *
 * Created: Tue Feb  1 11:08:12 2005                                           *
 * Desc: C Wrapper over MemMapFile.h                                           *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  mm_file.h
 * \brief C Wrapper over MemMapFile.h
 */

#ifndef __mm_file_h__
#define __mm_file_h__

typedef long mm_off;

#ifndef WIN32
  #include <unistd.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct tag_mm_file mm_file;

    mm_file*     mm_file_create_handle();
    void         mm_file_destroy(mm_file *mf);

    int          mm_file_open(mm_file *mf, const char* lpname, int flags, int mode);
#ifndef WIN32
    int          mm_file_assign(mm_file *mf, int fd);
#endif
    int          mm_file_close(mm_file *mf);
    const void*  mm_file_map(mm_file *mf);
    void         mm_file_unmap(mm_file *mf);
    int          mm_file_read(mm_file *mf, void *buf, size_t length);
    int          mm_file_write(mm_file *mf, const void *buf, size_t length);
    int          mm_file_pread(mm_file *mf, mm_off offset, void *buf, size_t length);
    int          mm_file_pwrite(mm_file *mf, mm_off offset, const void *buf, size_t length);
    mm_off       mm_file_lseek(mm_file *mf, mm_off offset, int origin);
    int          mm_file_sync(mm_file *mf);
    int          mm_file_truncate(mm_file *mf, mm_off offset);
    int          mm_file_isopen(const mm_file *mf);

#ifdef __cplusplus
}
#endif

#endif /* __mm_file_h__ */

/*
 * <eof mm_file.h>
 */
