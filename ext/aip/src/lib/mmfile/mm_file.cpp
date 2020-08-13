/*******************************************************************************
 * Proj: APTL                                                                  *
 * --------------------------------------------------------------------------- *
 * File: mm_file.cpp                                                            *
 * Created: Tue Feb  1 11:24:12 2005                                           *
 * Desc: C Wrapper over aptl_io.h                                              *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  mm_file.cpp
 * \brief C Wrapper over aptl_io.h
 */

#include <stdlib.h>
#include "mm_file.h"
#include "MemMapFile.h"

mm_file *mm_file_create_handle()
{
    return (mm_file*)(new MemMapFile);
}

void mm_file_destroy(mm_file *mf)
{
    delete (MemMapFile*)mf;
}

int mm_file_open(mm_file *mf, const char* lpname, int flags, int mode)
{
    return ((MemMapFile*)mf)->open(lpname, flags, mode);
}

#ifndef WIN32
int mm_file_assign(mm_file *mf, int fd)
{
    return ((MemMapFile*)mf)->assign(fd);
}
#endif

int mm_file_close(mm_file *mf)
{
    return ((MemMapFile*)mf)->close();
}

const void* mm_file_map(mm_file *mf)
{
    return ((MemMapFile*)mf)->map();
}

void mm_file_unmap(mm_file *mf)
{
    ((MemMapFile*)mf)->unmap();
}

int mm_file_read(mm_file *mf, void *buf, size_t length)
{
    return ((MemMapFile*)mf)->read(buf, length);
}

int mm_file_write(mm_file *mf, const void *buf, size_t length)
{
    return ((MemMapFile*)mf)->write(buf, length);
}

int mm_file_pread(mm_file *mf, mm_off offset, void *buf, size_t length)
{
    return ((MemMapFile*)mf)->pread(offset, buf, length);
}

int mm_file_pwrite(mm_file *mf, mm_off offset, const void *buf, size_t length)
{
    return ((MemMapFile*)mf)->pwrite(offset, buf, length);
}

mm_off mm_file_lseek(mm_file *mf, mm_off offset, int origin)
{
    return ((MemMapFile*)mf)->lseek(offset, origin);
}

int mm_file_sync(mm_file *mf)
{
    return ((MemMapFile*)mf)->sync();
}

int mm_file_truncate(mm_file *mf, mm_off offset)
{
    return ((MemMapFile*)mf)->truncate(offset);
}

int mm_file_isopen(const mm_file *mf)
{
    return ((MemMapFile*)mf)->isopen();
}

/*
 * <eof mm_file.cpp>
 */
