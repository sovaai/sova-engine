/*******************************************************************************
 * Proj: Content Filtration Library                                            *
 * --------------------------------------------------------------------------- *
 * File: fstorage.c                                                            *
 * Created: Wed Jan 21 15:15:34 2004                                           *
 * Desc: Universal storage (simple implementation)                             *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  fstorage.c
 * \brief Universal storage (simple implementation)
 */

#ifdef _MSC_VER
#pragma warning(disable:4127) // conditional expression is constant
#pragma warning(disable:4702) // unreachable code
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>

#include "_include/ntoh.h"
#include "_include/_inttype.h"

#if defined(WIN32)

#include <malloc.h>
#include <io.h>

#else /* WIN32 */

#include <unistd.h>
#include <sys/param.h>
#include <sys/mman.h>

#endif /* WIN32 */

#include "lib/mmfile/mm_file.h"

#include "lib/hash/hash_fnv.h"

#include "fstorage.h"

#define OFF_T_FORMAT "%qi"

#ifdef FSTORAGE_DEBUG
/* XXX gcc only, may be c99 */
#define FS_DPRINTF(...) fprintf(stderr, " ===> FSTORAGE DEBUG: " ## __VA_ARGS__)
#define FS_DEBUG_PRINT(x) do { FS_DPRINTF ## x; } while(0)
#else
#define FS_DEBUG_PRINT(x) 
#endif

#define SECTION_ALIGNMENT 32

/*
 * fsection type definition.
 */

#define FSTORAGE_SECTION_READING      0x00000001U
#define FSTORAGE_SECTION_WRITING      0x00000002U
#define FSTORAGE_SECTION_MEMORY_OWNER 0x00000004U

struct tag_fstorage_section {

    unsigned int flags;
    fstorage_section_id   sid;

    fstorage *fs;

    struct {
        unsigned int offset; /* offset of the section data (without heder) */
        unsigned int size;   /* size of the section data */
    } file;

    struct {
        char   *ptr;
        size_t  size;
    } memory;
    
    unsigned int                 trx_snapshot_idx;
    struct tag_fstorage_section *shadowed;
    
    struct fstorage_stream_section_config fssc;
};

/*
 * fstorage type definition.
 */

#define REAL_FS(FS) ((FS)->parent ? (FS)->parent : (FS))

struct modes_stack
{
    enum fstorage_open_modes  mode;
    struct modes_stack       *next;
};

struct tag_fstorage {

    fstorage     *parent;
    unsigned int  use_counter;
    unsigned int  trx_snapshot_idx;
    
    struct  {
        
        mm_file *handle;
        mm_file *new_handle; /* temporary descriptor; has sense only at _close stage */

        int   flags;
        int   mode;
        char *path;
    } file;

    struct modes_stack *fsom;

    struct fstorage_connect_config fscc;

    struct {

        fstorage_section **array;
        size_t             number;

    } sections;

    struct {

        /* XXX If fstorage opened in shared mode, whole
         * file will be mmap'ed. This situation have to be
         * changed.
         */

        void   *ptr;
        size_t  length;

    } shared;
    
    int last_error;
};

/*
 * Section functions.
 */

/* internal function, used in fstorage_connect */

static fstorage_section *fstorage_section_create_int(fstorage *fs, fstorage_section_id sid)
{
    fstorage_section *result = NULL;

    FS_DEBUG_PRINT(("fstorage_section_create_int: in: fs=%p, sid=%u\n", fs, sid));

#if 0
    /* XXX Nonsense */

    result = fstorage_find_section(fs, sid);
    if(result)
        result = NULL;
    else
#endif
        {
            fstorage *real_fs = REAL_FS(fs);
            
            result = (fstorage_section *)malloc(sizeof(fstorage_section));
            memset(result, 0, sizeof(fstorage_section));
                    
            result->fs               = real_fs;
            result->sid              = sid;
            result->trx_snapshot_idx = 0;
        }
            
    return result;
}


fstorage_section *fstorage_section_create(fstorage *fs, fstorage_section_id sid)
{
    fstorage *real_fs;
    fstorage_section *result = NULL, *shadowed = NULL;
    size_t i, target_idx;

    FS_DEBUG_PRINT(("fstorage_section_create: in: fs=%p, sid=%u\n", fs, sid));

    if(REAL_FS(fs)->fsom->mode != FSTORAGE_OPEN_READ_WRITE && REAL_FS(fs)->trx_snapshot_idx == 0)
    {
        fs->last_error = FSTORAGE_ERROR_INVALID_PARAMS;
        FS_DEBUG_PRINT(("fstorage_section_create: out: NULL (invalid params)\n"));
        result = NULL;
    }
    else
    {
        real_fs = REAL_FS(fs);

        for (i = 0; i < real_fs->sections.number; i++)
        {
            if ( real_fs->sections.array[i] != NULL && real_fs->sections.array[i]->sid == sid )
            {
                result = real_fs->sections.array[i];
                break;
            }
        }
        
        if ( result && (real_fs->trx_snapshot_idx == result->trx_snapshot_idx) )
        {
            FS_DEBUG_PRINT(("fstorage_section_create: out: NULL (section already exist)\n"));
            result = NULL;
        }
        else
        {
            shadowed = result;
            
            result = (fstorage_section *)malloc(sizeof(fstorage_section));
            memset(result, 0, sizeof(fstorage_section));
                    
            result->fs               = real_fs;
            result->sid              = sid;
            result->trx_snapshot_idx = real_fs->trx_snapshot_idx;
            
            if ( shadowed == NULL )
            {
                target_idx = real_fs->sections.number;
                
                if (real_fs->sections.array)
                {
                    for ( i = 0; i < real_fs->sections.number; i++ )
                        if ( real_fs->sections.array[i] == NULL )
                        {
                            target_idx = i;
                            break;
                        }
                    
                    if ( i >= real_fs->sections.number )
                        real_fs->sections.array = (fstorage_section **)realloc(real_fs->sections.array, 
                                                                               (real_fs->sections.number += 1)*sizeof(fstorage_section *));
                }
                else
                {
                    real_fs->sections.array = (fstorage_section **)malloc((real_fs->sections.number = 1)*sizeof(fstorage_section *));
                }
                
                real_fs->sections.array[target_idx] = result;
            }
            else
            {
                memcpy( &result->file,   &shadowed->file,  sizeof(result->file) );
                memcpy( &result->memory, &shadowed->memory, sizeof(result->file) );
                
                for ( i = 0; i < real_fs->sections.number; i++ )
                {
                    if ( real_fs->sections.array[i] == shadowed )
                    {
                        real_fs->sections.array[i] = result;
                        result->shadowed = shadowed;
                        break;
                    }
                }
                
            }
        }
    }
            
    return result;
}

fstorage_section *fstorage_get_first_section(fstorage *fs)
{
    fstorage *real_fs;
    fstorage_section *fss;
    size_t i;
    
    if(!fs)
        return NULL;
    
    real_fs = REAL_FS(fs);
    
    if(real_fs->fsom->mode == FSTORAGE_OPEN_CLOSED && real_fs->trx_snapshot_idx == 0)
        {
            fs->last_error = FSTORAGE_ERROR_INVALID_MODE;
            return NULL;
        }
        
    for ( i = 0; real_fs->sections.array != NULL && i < real_fs->sections.number; i++ )
    {
        fss = real_fs->sections.array[i];
        if ( fss == NULL || fss->trx_snapshot_idx != real_fs->trx_snapshot_idx )
            continue;
        return fss;
    }
    
    return NULL;
}

fstorage_section *fstorage_next_section(fstorage_section *fsect)
{
    fstorage *real_fs = NULL;
    size_t i;
    int found = 0;
    fstorage_section *fss;

    if(!fsect)
        return NULL;


    real_fs = REAL_FS(fsect->fs);

    if(!real_fs)
        return NULL;

    if(real_fs->sections.number == 0)
        return NULL;
    
    
    for ( i = 0; real_fs->sections.array != NULL && i < real_fs->sections.number; i++ )
    {
        fss = real_fs->sections.array[i];

        if ( !found && fss == fsect )
            found = 1;
        else if ( found && fss != NULL && fss->trx_snapshot_idx == real_fs->trx_snapshot_idx )
            return fss;
    }
    
    return NULL;
}

fstorage_section *fstorage_find_section(fstorage *fs, fstorage_section_id sid)
{
    size_t i;
    fstorage *real_fs = NULL;

    if(!fs)
        return NULL;

    real_fs = REAL_FS(fs);

    if(real_fs->fsom->mode == FSTORAGE_OPEN_CLOSED && real_fs->trx_snapshot_idx == 0)
        {
            fs->last_error = FSTORAGE_ERROR_INVALID_MODE;
            return NULL;
        }

    for (i = 0; i < real_fs->sections.number; i++)
    {
        if ( real_fs->sections.array[i] != NULL &&
             real_fs->sections.array[i]->sid == sid && 
             real_fs->sections.array[i]->trx_snapshot_idx == real_fs->trx_snapshot_idx )
            return real_fs->sections.array[i];
    }

    fs->last_error = FSTORAGE_ERROR_NOSECTION;
    return NULL;
}

fstorage_section *fstorage_get_section(fstorage *fs, fstorage_section_id sid)
{
    fstorage_section *result = NULL;
    
    if(!fs)
        return NULL;

    if(sid == FSTORAGE_SECTION_NONE)
        return NULL;

    result = fstorage_find_section(fs, sid);
    if ( !result || result->trx_snapshot_idx != fs->trx_snapshot_idx )
        result = fstorage_section_create(fs, sid);

    return result;
}

fstorage_section *fstorage_get_sized_section(fstorage *fs, fstorage_section_id sid, size_t size)
{
    fstorage_section *result = NULL;

    result = fstorage_get_section(fs, sid);
    if(!result)
        return NULL;

    if(fstorage_section_realloc(result, size))
        return NULL;

    return result;
}


static int fstorage_section_destroy(fstorage_section *fss)
{
    if(fss->memory.ptr && (fss->flags & FSTORAGE_SECTION_MEMORY_OWNER))
        free(fss->memory.ptr);

    if(fss->fssc.on_destroy)
        fss->fssc.on_destroy(&fss->fssc);

    free(fss);

    return FSTORAGE_OK;
}

int fstorage_remove_section(fstorage *fs, fstorage_section_id sid)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    fstorage *real_fs = NULL;
    size_t i; 
    fstorage_section *shadowed = NULL;
    
#define REACT_ON_ERROR(ERR) do { if(fs) fs->last_error = (ERR); error_code = (ERR); goto finish; } while(0)

    real_fs = REAL_FS(fs);

    if( (real_fs->fsom->mode == FSTORAGE_OPEN_CLOSED || real_fs->fsom->mode == FSTORAGE_OPEN_READ_ONLY) && real_fs->trx_snapshot_idx == 0 )
        REACT_ON_ERROR(FSTORAGE_ERROR_INVALID_MODE);


    for (i = 0; i < real_fs->sections.number; i++)
    {
        if ( real_fs->sections.array[i] != NULL &&
             real_fs->sections.array[i]->sid == sid &&
             real_fs->sections.array[i]->trx_snapshot_idx == real_fs->trx_snapshot_idx )
            break;
    }

    if ( i >= real_fs->sections.number )
        REACT_ON_ERROR(FSTORAGE_ERROR_NOSECTION);
    
    shadowed = real_fs->sections.array[i]->shadowed;
    
    fstorage_section_destroy(real_fs->sections.array[i]);
    
    if ( shadowed == NULL )
    {
        i++;
        for( ; i < real_fs->sections.number; i++)
            real_fs->sections.array[i-1] = real_fs->sections.array[i];

        real_fs->sections.number--;
        real_fs->sections.array = (fstorage_section **)realloc(real_fs->sections.array, sizeof(fstorage_section *)*real_fs->sections.number);
    }
    else
    {
        real_fs->sections.array[i] = shadowed;
    }
    
    
#undef REACT_ON_ERROR
    error_code = FSTORAGE_OK;

  finish:

    return error_code;
}


size_t fstorage_section_get_offset(fstorage_section *fss)
{
    return fss->file.offset;
}

fstorage_section_id fstorage_section_get_id(fstorage_section *fss)
{
    if(fss)
        return fss->sid;
    else
        return FSTORAGE_SECTION_NONE;
}

size_t fstorage_section_get_size(fstorage_section *fss)
{
    if(fss)
        {
            if(fss->memory.size)
                return fss->memory.size;
            else
                return fss->file.size;
        }
    else
        return 0;
}

size_t fstorage_section_get_file_ofs(fstorage_section *fss)
{
    if(fss)
        return fss->file.offset;
    else
        return 0;
}

fstorage *fstorage_section_get_fs(fstorage_section *fss)
{
    if(fss)
        return fss->fs;
    else
        return NULL;
}


void *fstorage_section_get_data(fstorage_section *fss, size_t offset, size_t amount)
{
    if(!fss)
        return NULL;

    if ( (fss->fssc.on_read || fss->fssc.on_write) && fss->fs->trx_snapshot_idx == 0 )
        {
            if(fss->fs)
                {
                    fss->fs->last_error = FSTORAGE_ERROR_INVALID_PARAMS;
                    return NULL;
                }
        }

    if ( fss->fssc.on_write == NULL && fss->memory.ptr == NULL )
        {
            if(fstorage_section_load(fss))
                return NULL;
        }

    if ( fss->memory.ptr )
        {
            if(fss->memory.size >= offset + amount)
                return fss->memory.ptr + offset;
            else
                return NULL;
        }
    else
        {
            /* XXX not implemented yet 
             * Here must be code for partial load of section.
             */

            return NULL;
        }
}

void *fstorage_section_get_all_data(fstorage_section *fss)
{
    if(!fss)
        return NULL;

    if ( (fss->fssc.on_read || fss->fssc.on_write) && fss->fs->trx_snapshot_idx == 0 )
        return NULL;

    if ( fss->fssc.on_write == NULL && fss->memory.ptr == NULL )
    {
        if(fstorage_section_load(fss))
            return NULL;
    }

    return fss->memory.ptr;
}

int fstorage_section_set_data(fstorage_section *fss, void *data, size_t size)
{
    if(!fss)
        return FSTORAGE_ERROR_INVALID_PARAMS;

    if(fss->fssc.on_read || fss->fssc.on_write)
        return FSTORAGE_ERROR_INVALID_MODE;

    if(fss->fs->fsom->mode != FSTORAGE_OPEN_READ_WRITE && fss->fs->trx_snapshot_idx == 0)
        return FSTORAGE_ERROR_INVALID_MODE;
    

    if(fss->memory.ptr && fss->flags & FSTORAGE_SECTION_MEMORY_OWNER)
        {
            free(fss->memory.ptr);
            fss->memory.ptr = NULL;
            fss->memory.size = 0;
        }

    /* 
     * This is not a bug (clearing memory_owner bit): data, which was set
     * from outside world, have to be freed by it.
     */

    fss->flags &= ~FSTORAGE_SECTION_MEMORY_OWNER;
    fss->memory.ptr = data;
    fss->memory.size = size;

    return FSTORAGE_OK;
}

int fstorage_section_copy_data(fstorage_section *fss, void *data, size_t size)
{
    int sub_error_code;

    if(!fss)
        return FSTORAGE_ERROR_INVALID_PARAMS;

    if ( (fss->fssc.on_read || fss->fssc.on_write) && fss->fs->trx_snapshot_idx == 0 )
        return FSTORAGE_ERROR_INVALID_MODE;

    if(fss->fs->fsom->mode != FSTORAGE_OPEN_READ_WRITE && fss->fs->trx_snapshot_idx == 0)
        return FSTORAGE_ERROR_INVALID_MODE;

    if(!(fss->flags & FSTORAGE_SECTION_MEMORY_OWNER) && fss->memory.ptr)
        {
            fss->memory.ptr = NULL;
            fss->memory.size = 0;
        }

    sub_error_code = fstorage_section_realloc(fss, size);
    if(sub_error_code)
        return sub_error_code;

    memcpy(fss->memory.ptr, data, size);

    return FSTORAGE_OK;
}

int fstorage_section_realloc(fstorage_section *fss, size_t size)
{
    if(!fss)
        return FSTORAGE_ERROR_INVALID_PARAMS;

    if ( (fss->fssc.on_read || fss->fssc.on_write) && fss->fs->trx_snapshot_idx == 0 )
        return FSTORAGE_ERROR_INVALID_MODE;

    if(fss->fs->fsom->mode != FSTORAGE_OPEN_READ_WRITE && fss->fs->trx_snapshot_idx == 0)
        return FSTORAGE_ERROR_INVALID_MODE;

    if(fss->flags & FSTORAGE_SECTION_MEMORY_OWNER || fss->memory.ptr == NULL)
        {
            if(fss->memory.ptr)
                fss->memory.ptr = realloc(fss->memory.ptr, fss->memory.size = size);
            else
                fss->memory.ptr = malloc(fss->memory.size = size);

            if(fss->memory.ptr == NULL)
                {
                    fss->memory.size = 0;
                    
                    if(fss->fs)
                        fss->fs->last_error = FSTORAGE_ERROR_NOMEMORY;

                    return FSTORAGE_ERROR_NOMEMORY;
                }

            fss->flags |= FSTORAGE_SECTION_MEMORY_OWNER;

            return FSTORAGE_OK;
        }
    else
        return FSTORAGE_ERROR_INVALID_MODE;
}

int fstorage_section_stream(fstorage_section *fss, struct fstorage_stream_section_config *fssc)
{
    if(!fss)
        return FSTORAGE_ERROR_INVALID_PARAMS;

    if(fssc)
        memcpy(&(fss->fssc), fssc, sizeof(struct fstorage_stream_section_config));
    else
        memset(&(fss->fssc), 0, sizeof(struct fstorage_stream_section_config));

    return FSTORAGE_OK;
}

/*
 * Data access functions for callbacks.
 */

static int fstorage_section_read(struct fstorage_section_access_config *fsac, void *buf, size_t nbytes)
{
    fstorage *real_fs = NULL;
    fstorage_section *fss = NULL;
    mm_off current_offset;

    if(nbytes == 0)
        return 0;

    fss = fsac->fss;
    real_fs = REAL_FS(fss->fs);
    current_offset = mm_file_lseek(real_fs->file.handle, 0, SEEK_CUR);

    if ( current_offset > (mm_off)(fss->file.offset + fss->file.size)
      || current_offset < (mm_off)fss->file.offset )
    {
        /* XXX something wrong? */

        return -1;
    }

    if(current_offset + nbytes > fss->file.offset + fss->file.size)
        nbytes = fss->file.size - (current_offset-fss->file.offset);


    if(nbytes == 0)
        return 0;

    return mm_file_read(real_fs->file.handle, buf, nbytes);
}

static int    fstorage_section_write(struct fstorage_section_access_config *fsac, const void *buf, size_t nbytes)
{
    FS_DEBUG_PRINT(("fstorage_section_write: nbytes=%u\n", nbytes));

    if(mm_file_isopen(fsac->fss->fs->file.new_handle))
        return mm_file_write(fsac->fss->fs->file.new_handle, buf, nbytes);
    else
        return -1;
}

static int fstorage_section_sync(struct fstorage_section_access_config *fsac)
{
    return mm_file_sync(fsac->fss->fs->file.new_handle);
}

/*
 * Storage functions.
 */

int fstorage_last_error(fstorage *fs)
{
    if(fs)
        return fs->last_error;
    else 
        return FSTORAGE_ERROR_INVALID_PARAMS;
}

fstorage *fstorage_create()
{
    fstorage *result = NULL;

    result = (fstorage *)malloc(sizeof(fstorage));
    if ( result == NULL )
        return NULL;
    memset(result, 0, sizeof(fstorage));
    
    result->fsom = (struct modes_stack *)malloc(sizeof(struct modes_stack));
    if ( result->fsom == NULL )
    {
        free(result);
        return NULL;
    }
    memset(result->fsom, 0, sizeof(struct modes_stack));
    
    result->last_error = FSTORAGE_OK;
    result->file.handle = NULL;
    result->file.new_handle = NULL;

    return result;
}

fstorage *fstorage_clone(fstorage *fs)
{
    fstorage *result = NULL;

#define REACT_ON_ERROR(ERR) do { if(fs) fs->last_error = (ERR); goto finish; } while(0)

    if(fs == NULL)
        REACT_ON_ERROR(FSTORAGE_ERROR_INVALID_PARAMS);

    if(fs->file.handle == NULL)
        REACT_ON_ERROR(FSTORAGE_ERROR_INVALID_MODE);

    result = fstorage_create();

    if(fs->parent == NULL)
        result->parent = fs;
    else
        result->parent = fs->parent;

    result->parent->use_counter++;
    
#undef ERROR
  finish:

    return result;
}

int fstorage_destroy(fstorage *fs)
{
    fstorage *parent_fs;
    unsigned int i;
    struct modes_stack *fsom, *next;

    if(fs == NULL)
        return FSTORAGE_ERROR_INVALID_PARAMS;

    if(fs->use_counter > 0)
        return FSTORAGE_OK;

    parent_fs = fs->parent;

    fstorage_purge(fs);

    if(fs->file.path)
        free(fs->file.path);

    if(fs->sections.array)
    {
        for(i = 0; i < fs->sections.number; i++)
        {
            while ( fs->sections.array[i] != NULL )
            {
                fstorage_section *shadowed = fs->sections.array[i]->shadowed;
                fstorage_section_destroy(fs->sections.array[i]);
                fs->sections.array[i] = shadowed;
            }
        }

        free(fs->sections.array);
    }
    
    for ( fsom = fs->fsom, next = NULL; fsom != NULL; fsom = next )
    {
        next = fsom->next;
        free( fsom );
    }
    
    free(fs);

    if(parent_fs)
    {
        parent_fs->use_counter--;

        if(parent_fs->use_counter == 0)
            return fstorage_destroy(parent_fs);
    }

    return FSTORAGE_OK;
}

/*
 * Real fstorage header structure:
 *
 * 4 bytes --- fstorage signature.
 * 4 bytes --- header length (without this two fields).
 * 4 bytes --- project id.
 * 4 bytes --- number of sections.
 * 4 bytes --- sections table offset.
 * 2 bytes --- major version
 * 2 bytes --- minor version
 * 2*4 bytes --- zero padding, available for future use.
 */

struct fstorage_header {
    fstorage_project_id pid;
    
    struct {
        unsigned int number;
        unsigned int offset;
    } sections;

    struct {
        unsigned short major;
        unsigned short minor;
    } version;
};

#define FSTORAGE_HEADER_SIGNATURE "FSH1"
#define FSTORAGE_VERSION_MAJOR    1
#define FSTORAGE_VERSION_MINOR    0

static int fstorage_read_header(fstorage *fs, mm_file *mf, struct fstorage_header *fsh)
{
    int            error_code = FSTORAGE_ERROR_UNKNOWN;
    unsigned char  read_buf[10*sizeof(uint32_t)];
    uint32_t       header_length = 0;
    uint16_t      *ushort_ptr = NULL;

#define ERROR(ERR) do { if(fs) fs->last_error = (ERR); error_code = (ERR); goto finish; } while(0)

    memset(fsh, 0, sizeof(struct fstorage_header));

    if(mm_file_lseek(mf, 0L, SEEK_SET) < 0)
        ERROR(FSTORAGE_ERROR_READ);
        
    if(mm_file_read(mf, read_buf, 2*sizeof(uint32_t)) < (int)(2*sizeof(uint32_t)))
        ERROR(FSTORAGE_ERROR_READ);

    if(memcmp(read_buf, FSTORAGE_HEADER_SIGNATURE, 4))
        ERROR(FSTORAGE_ERROR_INVALID_FILE);
    
    header_length = ntohl(((uint32_t *)read_buf)[1]);
    FS_DEBUG_PRINT(("fstorage_read_header: header_length = %li\n", header_length));

    if(header_length >= 3*sizeof(uint32_t) + 2*sizeof(uint16_t))
        {
            if(mm_file_read(mf, read_buf, 3*sizeof(uint32_t) + 2*sizeof(uint16_t))
              < (int)(3*sizeof(uint32_t) + 2*sizeof(uint16_t)))
                ERROR(FSTORAGE_ERROR_READ);

            fsh->pid             = ntohl(((uint32_t *)read_buf)[0]);
            fsh->sections.number = ntohl(((uint32_t *)read_buf)[1]);
            fsh->sections.offset = ntohl(((uint32_t *)read_buf)[2]);

            ushort_ptr = (uint16_t *)(read_buf + 3*sizeof(uint32_t));

            fsh->version.major = ntohs(ushort_ptr[0]);
            fsh->version.minor = ntohs(ushort_ptr[1]);

            if(fsh->version.major > FSTORAGE_VERSION_MAJOR)
                ERROR(FSTORAGE_ERROR_INVALID_VERSION);
            
            FS_DEBUG_PRINT(("fstorage_read_header: pid = %u, sections_number=%u, sectons_offset=%u\n", 
                            fsh->pid, fsh->sections.number, fsh->sections.offset));

            header_length -= 3*sizeof(uint32_t);
        }
    else
        {
            FS_DEBUG_PRINT(("fstorage_read_header: invalid file, header_length = %li\n", header_length));
            ERROR(FSTORAGE_ERROR_INVALID_FILE);
        }

    if(header_length > 0)
        {
            if(mm_file_lseek(mf, header_length, SEEK_CUR) < 0)
                ERROR(FSTORAGE_ERROR_READ);
        }
    
    error_code = FSTORAGE_OK;

#undef ERROR
  finish:

    return error_code;
}

static int fstorage_write_header(fstorage *fs, mm_file *mf, struct fstorage_header *fsh)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    unsigned char write_buf[10*sizeof(uint32_t)];
    uint32_t  header_length = 0;
    uint16_t *ushort_ptr = NULL;

#define ERROR(ERR) do { if(fs) fs->last_error = (ERR); error_code = (ERR); goto finish; } while(0)

    memset(write_buf, 0, sizeof(write_buf));
    
    header_length = 6*sizeof(uint32_t) + 2*sizeof(uint16_t);

    memcpy(write_buf, FSTORAGE_HEADER_SIGNATURE, 4);

    FS_DEBUG_PRINT(("fstorage_write_header: header_length=%li, pid = %u, sections_number=%u, sectons_offset=%u\n", 
                    header_length, fsh->pid, fsh->sections.number, fsh->sections.offset));
    
    ((uint32_t *)(write_buf + 4))[0] = htonl(header_length);
    ((uint32_t *)(write_buf + 4))[1] = htonl(fsh->pid);
    ((uint32_t *)(write_buf + 4))[2] = htonl(fsh->sections.number);
    ((uint32_t *)(write_buf + 4))[3] = htonl(fsh->sections.offset);

    ushort_ptr = (uint16_t *)(write_buf + 4 + 4*sizeof(uint32_t));

    ushort_ptr[0] = htons(FSTORAGE_VERSION_MAJOR);
    ushort_ptr[1] = htons(FSTORAGE_VERSION_MINOR);

    if(mm_file_write(mf, write_buf, 4+7*sizeof(uint32_t)) < (int)(4+7*sizeof(uint32_t)))
        ERROR(FSTORAGE_ERROR_WRITE);
    
    error_code = FSTORAGE_OK;

#undef ERROR
  finish:

    return error_code;
}   

/*
 * Section header structure.
 * 
 * 4 bytes --- data offset from beginning of the section header
 * 4 bytes --- total size of data (from offset)
 * 4 bytes --- section_id
 * 5*4 bytes --- zero padding (available for future use)
 */

struct fstorage_section_header {

    fstorage_section_id   sid;
    unsigned int data_offset;
    unsigned int total_size;
};

static int fstorage_section_read_header(fstorage *fs, mm_file *mf,
                                        struct fstorage_section_header *fssh)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    unsigned char read_buf[10*sizeof(uint32_t)];

#define ERROR(ERR) do { if(fs) fs->last_error = (ERR); error_code = (ERR); goto finish; } while(0)

    memset(fssh, 0, sizeof(struct fstorage_section_header));

    if(mm_file_read(mf, read_buf, 3*sizeof(uint32_t)) < (int)(3*sizeof(uint32_t)))
        ERROR(FSTORAGE_ERROR_READ);

    fssh->data_offset     = ntohl(((uint32_t *)read_buf)[0]);
    fssh->total_size      = ntohl(((uint32_t *)read_buf)[1]);
    fssh->sid             = ntohl(((uint32_t *)read_buf)[2]);

    if(fssh->data_offset >= 3*sizeof(uint32_t))
        {
            if(mm_file_lseek(mf, fssh->data_offset - 3*sizeof(uint32_t), SEEK_CUR) < 0)
                ERROR(FSTORAGE_ERROR_READ);
        }
    
    error_code = FSTORAGE_OK;

#undef ERROR
  finish:

    return error_code;
}

/*
 * This function fills data_offset by itself.
 */
static int fstorage_section_write_header(fstorage *fs, mm_file *mf,
                                         struct fstorage_section_header *fssh)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    unsigned char write_buf[10*sizeof(uint32_t)];

#define ERROR(ERR) do { if(fs) fs->last_error = (ERR); error_code = (ERR); goto finish; } while(0)

    fssh->data_offset = 3*sizeof(uint32_t);

    memset(write_buf, 0, sizeof(write_buf));

    ((uint32_t *)write_buf)[0] = htonl(8*sizeof(uint32_t));
    ((uint32_t *)write_buf)[1] = htonl(fssh->total_size);
    ((uint32_t *)write_buf)[2] = htonl(fssh->sid);

    if(mm_file_write(mf, write_buf, 8*sizeof(uint32_t)) < (int)(8*sizeof(uint32_t)))
        ERROR(FSTORAGE_ERROR_WRITE);
    
    error_code = FSTORAGE_OK;

#undef ERROR
  finish:

    return error_code;
}

/*
 * Storage file functions.
 */

int fstorage_connect(fstorage *fs, const char *path, int open_flags, int open_mode, 
                     enum fstorage_open_modes fsom, struct fstorage_connect_config *fscc)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    int sub_error_code;
    mm_file *mf = NULL;
    struct stat sb;
    size_t i;
    int check_header = 0;
    struct fstorage_header fsh;

#define ERROR(ERR) do { if(fs) fs->last_error = (ERR); error_code = (ERR); goto finish; } while(0)

    memset(&fsh, 0, sizeof(fsh));

    if(fsom == FSTORAGE_OPEN_CLOSED)
        ERROR(FSTORAGE_ERROR_INVALID_MODE);
    
    /* modified by Croco so that empty file is considered equal 
         to absent file.
       This is useful because the storage is often created but 
       the program then fails and the storage is never closed. 
       Then, the subsequent run fails because there's that empty 
       storage file.
     */
    if(stat(path, &sb) == 0 && sb.st_size>0 && (open_flags & O_TRUNC) == 0)
        {
            /* file exist */
            check_header = 1;
        }

    mf = mm_file_create_handle();

#ifdef WIN32
    open_flags |= O_BINARY;
#endif
    if(mm_file_open(mf, path, open_flags, open_mode))
        ERROR(FSTORAGE_ERROR_OPEN);
    
    if(!mm_file_isopen(mf))
        ERROR(FSTORAGE_ERROR_OPEN);
    
    if(check_header)
        {
            sub_error_code = fstorage_read_header(fs, mf, &fsh);
            if(sub_error_code)
                ERROR(sub_error_code);

            if(fscc && fscc->pid != FSTORAGE_PROJECT_NONE && fscc->pid != fsh.pid)
                ERROR(FSTORAGE_ERROR_INVALID_VERS);
        }

    if(fscc)
        memcpy(&(fs->fscc), fscc, sizeof(struct fstorage_connect_config));
    else if(check_header)
        fs->fscc.pid = fsh.pid;
    else
        fs->fscc.pid = FSTORAGE_PROJECT_NONE;
        
    if(fsh.sections.number)
        {
            char *sections_buf = (char*)alloca(2*sizeof(uint32_t)*fsh.sections.number);
            memset(sections_buf, 0, 2*sizeof(uint32_t)*fsh.sections.number);

            fs->sections.array = (fstorage_section **)malloc(sizeof(fstorage_section *)*fsh.sections.number);
            fs->sections.number = fsh.sections.number;

            memset(fs->sections.array, 0, sizeof(fstorage_section *)*fsh.sections.number);

            if(mm_file_lseek(mf, fsh.sections.offset, SEEK_SET) < 0)
                ERROR(FSTORAGE_ERROR_READ);

            if(mm_file_read(mf, sections_buf, 2*sizeof(uint32_t)*fsh.sections.number) < (int)(2*sizeof(uint32_t)*fsh.sections.number))
                ERROR(FSTORAGE_ERROR_READ);
            
            FS_DEBUG_PRINT(("fstorage_connect: trying to load information about %u sections\n", fs->sections.number));

            for(i = 0; i < fs->sections.number; i++)
                {
                    struct fstorage_section_header fssh;
                    uint32_t offset, size;

                    FS_DEBUG_PRINT(("fstorage_connect: loading section number %u\n", i));

                    memset(&fssh, 0, sizeof(fssh));
                    
                    offset = ntohl(*((uint32_t*)(sections_buf + i*2*sizeof(uint32_t))));
                    size   = ntohl(*((uint32_t*)(sections_buf + (i*2+1)*sizeof(uint32_t))));

                    if(mm_file_lseek(mf, offset, SEEK_SET) < 0)
                        ERROR(FSTORAGE_ERROR_READ);

                    sub_error_code = fstorage_section_read_header(fs, mf, &fssh);
                    if(sub_error_code != FSTORAGE_OK)
                        ERROR(sub_error_code);

                    fs->sections.array[i] = fstorage_section_create_int(fs, fssh.sid);
                    
                    if(fs->sections.array[i] == NULL)
                        ERROR(fs->last_error);

                    fs->sections.array[i]->file.offset = offset + fssh.data_offset;
                    fs->sections.array[i]->file.size   = fssh.total_size;
                }
        }

#undef ERROR
    error_code = FSTORAGE_OK;

    fs->file.handle = mf;
    fs->file.flags = open_flags;
    fs->file.mode = open_mode;
    fs->file.path = strdup(path);
    
    fs->fsom->mode = fsom;

    mf = NULL;

  finish:
    
    if(mf)
        {
            if(mm_file_isopen(mf))
                mm_file_close(mf);
            mm_file_destroy(mf);
        }

    if(error_code != FSTORAGE_OK)
        {
            if(fs->sections.array)
                {
                    for(i = 0; i < fs->sections.number; i++)
                        if(fs->sections.array[i])
                            {
                                fstorage_section_destroy(fs->sections.array[i]);
                                fs->sections.array[i] = NULL;
                            }

                    free(fs->sections.array);

                    fs->sections.array = NULL;
                    fs->sections.number = 0;
                }
        }

    return error_code;
}

int fstorage_connect_shared(fstorage *fs, const char *path, struct fstorage_connect_config *fscc)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    int sub_error_code;
    struct stat sb;

#define ERROR(ERR) do { if(fs) fs->last_error = (ERR); error_code = (ERR); goto finish; } while(0)

    if(stat(path, &sb) < 0)
        ERROR(FSTORAGE_ERROR_READ);
    
    sub_error_code = fstorage_connect(fs, path, O_RDONLY, 0644, FSTORAGE_OPEN_READ_ONLY, fscc);
    if(sub_error_code != FSTORAGE_OK)
        ERROR(sub_error_code);

    fs->shared.ptr    = (void *)mm_file_map(fs->file.handle);
    fs->shared.length = sb.st_size;

    if(fs->shared.ptr == NULL)
        {
            fs->shared.ptr = NULL;
            ERROR(FSTORAGE_ERROR_READ);
        }

#undef ERROR
    error_code = FSTORAGE_OK;

  finish:
    
    if(error_code != FSTORAGE_OK)
        {
            if(fstorage_open_mode(fs) != FSTORAGE_OPEN_CLOSED)
                fstorage_close(fs);
        }

    return error_code;
}

static int fstorage_save(fstorage *fs, mm_file *mf)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    int sub_error_code;
    size_t i;
    struct fstorage_header fsh;
    struct fstorage_section_header fssh;
    fstorage_section *cur_section;                      
    int    read_result, write_result;
    size_t to_read, read_now;
    
    unsigned char rw_buf[40*1024];

    char *section_table = NULL;

#define ERROR(ERR)                                                      \
    do                                                                  \
        {                                                               \
            FS_DEBUG_PRINT(("fstorage_save: error %i at line %i.\n", (ERR), __LINE__)); \
            if(fs) fs->last_error = (ERR);                              \
            error_code = (ERR);                                         \
            goto finish;                                                \
        }                                                               \
    while(0)

    if(fs->parent || fs->fsom->mode != FSTORAGE_OPEN_READ_WRITE)
        ERROR(FSTORAGE_ERROR_INVALID_MODE);

    FS_DEBUG_PRINT(("fstorage_save: in: fs=%p, mf=%p\n", fs, mf));
    
    memset(&fsh, 0, sizeof(fsh));

    fsh.pid = fs->fscc.pid;
    /* other fields will be filled later and rewritten */

    if(mm_file_lseek(mf, 0, SEEK_SET) < 0)
        ERROR(FSTORAGE_ERROR_WRITE);

    if(mm_file_truncate(mf, 0) < 0)
        ERROR(FSTORAGE_ERROR_WRITE);

    section_table = (char*)alloca(sizeof(uint32_t)*2*fs->sections.number);
    memset(section_table, 0, sizeof(uint32_t)*2*fs->sections.number);

    sub_error_code = fstorage_write_header(fs, mf, &fsh);
    if(sub_error_code != FSTORAGE_OK)
        ERROR(sub_error_code);

    FS_DEBUG_PRINT(("fstorage_save: fs->sections.number = %i\n", fs->sections.number));
    for(i = 0; i < fs->sections.number; i++)
        {
            mm_off offset1, offset2, cur_offset;
            size_t section_size;

            cur_section = fs->sections.array[i];

            offset1 = mm_file_lseek(mf, 0L, SEEK_CUR);
            if(offset1 < 0)
                ERROR(FSTORAGE_ERROR_WRITE);

            FS_DEBUG_PRINT(("fstorage_save: saving section number %i at offset " OFF_T_FORMAT "\n", i, offset1));

            if(cur_section->fssc.on_write)
                {
                    struct fstorage_section_access_config fsac;

                    FS_DEBUG_PRINT(("fstorage_save:  section %i (id %u): stream section\n", i, cur_section->sid));
                    
                    memset(&fsac, 0, sizeof(fsac));
                    fsac.fss = cur_section;
                    fsac.write = fstorage_section_write;
                    fsac.sync  = fstorage_section_sync;

                    /* call callback */

                    memset(&fssh, 0, sizeof(fssh));
                    fssh.sid = cur_section->sid;

                    /* will be rewrited */

                    sub_error_code = fstorage_section_write_header(fs, mf, &fssh);
                    if(sub_error_code != FSTORAGE_OK)
                        ERROR(sub_error_code);
                    
                    cur_offset = mm_file_lseek(mf, 0, SEEK_CUR);
                    if(cur_offset < 0)
                        ERROR(FSTORAGE_ERROR_WRITE);

                    /* on_write */

                    sub_error_code = cur_section->fssc.on_write(&(cur_section->fssc), &fsac);
                    if(sub_error_code != FSTORAGE_OK)
                        ERROR(sub_error_code);

                    offset2 = mm_file_lseek(mf, 0, SEEK_CUR);
                    if(offset2 < 0)
                        ERROR(FSTORAGE_ERROR_WRITE);

                    if(mm_file_lseek(mf, offset1, SEEK_SET) < 0)
                        ERROR(FSTORAGE_ERROR_WRITE);

                    /* write real header */

                    memset(&fssh, 0, sizeof(fssh));
                    fssh.sid = cur_section->sid;
                    fssh.total_size = offset2 - cur_offset;

                    sub_error_code = fstorage_section_write_header(fs, mf, &fssh);
                    if(sub_error_code != FSTORAGE_OK)
                        ERROR(sub_error_code);
                    
                    if(mm_file_lseek(mf, offset2, SEEK_SET) < 0)
                        ERROR(FSTORAGE_ERROR_WRITE);
                }
            else if(cur_section->memory.ptr)
                {
                    /* save memory */

                    FS_DEBUG_PRINT(("fstorage_save:  section %i (id %u): memory section, ptr=%p, size=%u\n", i,
                                    cur_section->sid, cur_section->memory.ptr, cur_section->memory.size));

                    memset(&fssh, 0, sizeof(fssh));
                    fssh.sid = cur_section->sid;
                    fssh.total_size = cur_section->memory.size;

                    sub_error_code = fstorage_section_write_header(fs, mf, &fssh);
                    if(sub_error_code != FSTORAGE_OK)
                        ERROR(sub_error_code);
                    
                    if(mm_file_write(mf, cur_section->memory.ptr, cur_section->memory.size) < (int)cur_section->memory.size)
                        ERROR(FSTORAGE_ERROR_WRITE);
                }
            else
                {
                    /* copy data from source storage */

                    FS_DEBUG_PRINT(("fstorage_save:  section %i (id %u): coping data from the source\n", i, cur_section->sid));

                    memset(&fssh, 0, sizeof(fssh));
                    fssh.sid = cur_section->sid;
                    fssh.total_size = cur_section->file.size;

                    sub_error_code = fstorage_section_write_header(fs, mf, &fssh);
                    if(sub_error_code != FSTORAGE_OK)
                        ERROR(sub_error_code);

                    if(mm_file_lseek(fs->file.handle, cur_section->file.offset, SEEK_SET) < 0)
                        ERROR(FSTORAGE_ERROR_READ);

                    to_read = cur_section->file.size;

                    for( ; ; )
                        {
                            read_now = (to_read > sizeof(rw_buf)) ? sizeof(rw_buf) : to_read;

                            read_result = mm_file_read(fs->file.handle, rw_buf, read_now);
                            FS_DEBUG_PRINT(("fstorage_save: section %i (id %u): read %i bytes, fs->file.handle=%i\n", 
                                            i, cur_section->sid, read_result, fs->file.handle));

                            if(read_result == 0)
                                break;
                            if(read_result < (int)read_now)
                                ERROR(FSTORAGE_ERROR_READ);

                            write_result = mm_file_write(mf, rw_buf, read_result);
                            FS_DEBUG_PRINT(("fstorage_save: section %i (id %u): written %i bytes\n", i, cur_section->sid, write_result));

                            if(write_result < read_result)
                                ERROR(FSTORAGE_ERROR_WRITE);

                            to_read -= read_now;
                            if(to_read == 0)
                                break;
                        }
                }

            offset2 = mm_file_lseek(mf, 0, SEEK_CUR);
            if(offset2 < 0)
                ERROR(FSTORAGE_ERROR_WRITE);

            FS_DEBUG_PRINT(("fstorage_save: saving section number %i finished at offset " OFF_T_FORMAT "\n", i, offset2));

            *((uint32_t *)(section_table + i*2*sizeof(uint32_t))) = htonl(offset1);
            *((uint32_t *)(section_table + i*2*sizeof(uint32_t) + sizeof(uint32_t))) = htonl(offset2 - offset1);

            section_size = offset2 - offset1;
            if(section_size % SECTION_ALIGNMENT)
                {
                    int to_write = SECTION_ALIGNMENT - (section_size % SECTION_ALIGNMENT);

                    memset(rw_buf, 0, to_write);

                    write_result = mm_file_write(mf, rw_buf, to_write);
                    FS_DEBUG_PRINT(("fstorage_save: section %i (id %u): written %i bytes\n", i, cur_section->sid, write_result));
                    
                    if(write_result < to_write)
                        ERROR(FSTORAGE_ERROR_WRITE);
                    
                }
        }

    if(fs->sections.number)
        {
            mm_off offset;

            offset = mm_file_lseek(mf, 0, SEEK_CUR);
            if(offset < 0)
                ERROR(FSTORAGE_ERROR_WRITE);

            fsh.sections.number = fs->sections.number;
            fsh.sections.offset = offset;

            if(mm_file_write(mf, section_table, fs->sections.number*2*sizeof(uint32_t)) < (int)(fs->sections.number*2*sizeof(uint32_t)))
                ERROR(FSTORAGE_ERROR_WRITE);

            FS_DEBUG_PRINT(("fstorage_save: section table was written at " OFF_T_FORMAT " offset\n", offset));

            if(mm_file_lseek(mf, 0, SEEK_SET) < 0)
                ERROR(FSTORAGE_ERROR_WRITE);

            sub_error_code = fstorage_write_header(fs, mf, &fsh);
            if(sub_error_code != FSTORAGE_OK)
                ERROR(sub_error_code);

        }
            
#undef ERROR
    error_code = FSTORAGE_OK;

  finish:

    return error_code;
}

static int fstorage_trx_section_read( struct fstorage_section_access_config *fsac, void *buf, size_t nbytes )
{
    fstorage_section *fss;
    unsigned int      current_offset;
    
    if (nbytes == 0)
        return 0;
    
    fss            = fsac->fss;
    current_offset = fss->file.offset;
    
    if ( fss->memory.ptr == NULL || current_offset >= fss->memory.size )
    {
        /* XXX something wrong? */

        return -1;
    }

    if ( current_offset + nbytes > fss->memory.size)
        nbytes = fss->memory.size - current_offset;

    if (nbytes == 0)
        return 0;
    
    memcpy( buf, fss->memory.ptr + current_offset, nbytes );
    
    fss->file.offset += nbytes;
    
    return nbytes;
}


int fstorage_load_section(fstorage *fs, fstorage_section_id sid)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    int sub_error_code = 0;
    fstorage *real_fs = NULL;
    fstorage_section *fss = NULL;
    size_t old_offset;
    
#define ERROR(ERR)                                                      \
    do                                                                  \
    {                                                                   \
        FS_DEBUG_PRINT(("fstorage_load_section: error %i at line %i.\n", (ERR), __LINE__)); \
        if(fs) fs->last_error = (ERR);                                  \
        error_code = (ERR);                                             \
        goto finish;                                                    \
    }                                                                   \
    while(0)

    FS_DEBUG_PRINT(("fstorage_load_section: in: fs=%p, sid=%i\n", fs, sid));

    if(!fs || sid == FSTORAGE_SECTION_NONE)
        ERROR(FSTORAGE_ERROR_INVALID_PARAMS);

    if(fs->fsom->mode == FSTORAGE_OPEN_CLOSED && fs->trx_snapshot_idx == 0)
        ERROR(FSTORAGE_ERROR_INVALID_MODE);

    real_fs = REAL_FS(fs);

    fss = fstorage_find_section(real_fs, sid);
    if (fss == NULL)
        ERROR(FSTORAGE_ERROR_NOSECTION);

    if ( fss->fssc.on_read )
    {
        /* streamed sections will be loaded again */

        struct fstorage_section_access_config fsac;
            
        FS_DEBUG_PRINT(("fstorage_load: section id %u: stream section\n", fss->sid));
                    
        memset(&fsac, 0, sizeof(fsac));
        fsac.fss  = fss;
        old_offset = fss->file.offset;
        
        if (fss->fs->trx_snapshot_idx > 0)
        {
            fsac.read        = fstorage_trx_section_read;
            fss->file.offset = 0;
        }
        else
        {
            fsac.read        = fstorage_section_read;
            if( mm_file_lseek(real_fs->file.handle, fss->file.offset, SEEK_SET) < 0 )
                ERROR(FSTORAGE_ERROR_READ);
        }

        /* on_read */
            
        sub_error_code   = fss->fssc.on_read(&(fss->fssc), &fsac);
        fss->file.offset = old_offset;
        if (sub_error_code != FSTORAGE_OK)
            ERROR(sub_error_code);
    }
    else if (fss->memory.ptr)
    {
        /* section is already loaded */
    }
    else if (real_fs->shared.ptr)
    {
        fss->memory.ptr = (char*)real_fs->shared.ptr + fss->file.offset;
        fss->memory.size = fss->file.size;
    }
    else 
    {
        fss->memory.ptr = malloc(fss->file.size);
        if(!fss->memory.ptr)
            ERROR(FSTORAGE_ERROR_NOMEMORY);
        fss->memory.size = fss->file.size;
        memset(fss->memory.ptr, 0, fss->memory.size);

        fss->flags |= FSTORAGE_SECTION_MEMORY_OWNER;
            
        if(mm_file_lseek(real_fs->file.handle, fss->file.offset, SEEK_SET) < 0)
            ERROR(FSTORAGE_ERROR_READ);
                    
        if(fss->memory.size != 0 && mm_file_read(real_fs->file.handle, fss->memory.ptr, fss->memory.size) < (int)fss->memory.size)
            ERROR(FSTORAGE_ERROR_READ);
    }

    error_code = FSTORAGE_OK;
    
#undef ERROR
    error_code = FSTORAGE_OK;

finish:

    return error_code;
}

int fstorage_section_load(fstorage_section *fss)
{
    return fstorage_load_section(fss->fs, fss->sid);
}


int fstorage_close(fstorage *fs)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    int sub_error_code;
    fstorage *real_fs = NULL;

# if !defined( WIN32 )
    int   tmphandle;
# endif
    /* !WIN32 */

#define ERROR(ERR)                                                      \
    do                                                                  \
        {                                                               \
            if(fs) fs->last_error = (ERR);                              \
            error_code = (ERR);                                         \
            FS_DEBUG_PRINT(("fstorage_close: error %i at line %i.\n", (ERR), __LINE__)); \
            goto finish;                                                \
        }                                                               \
    while(0)

    if(fs == NULL)
        ERROR(FSTORAGE_ERROR_INVALID_PARAMS);

    real_fs = REAL_FS(fs);

    if(real_fs->fsom->mode == FSTORAGE_OPEN_READ_WRITE)
        {
            /* XXX have to track changes and save data only 
             * if there was some changes
             */

            /* Win32 port: if to unlink() the file and to truncate it,
             * the data is lost before it may be copied to the new storage
             * being created
             * To prevent this problem, the new, temporary, file is created,
             * the data is written to it, and then the original file is removed,
             * and the temporary one is renamed to the original file name.
             * This looks more safe in the implemetation
             */
            char* psztpl = alloca( strlen( real_fs->file.path ) + 7 );
            
            /* allocate space for the temporary file folder               */
            strcat( strcpy( psztpl, real_fs->file.path ), "XXXXXX" );
            
            if(!real_fs->file.new_handle)
                real_fs->file.new_handle = mm_file_create_handle();

            if(!real_fs->file.new_handle)
                ERROR(FSTORAGE_ERROR_WRITE);

            
#ifdef WIN32
            /* create temporary name */
            if(mktemp(psztpl) == NULL)
                ERROR(FSTORAGE_ERROR_WRITE);
            
            /* open the temporary file to save storage */
            if(mm_file_open(real_fs->file.new_handle, psztpl, O_TRUNC | O_RDWR | O_CREAT | O_BINARY,
                           real_fs->file.mode))
                ERROR(FSTORAGE_ERROR_WRITE);
#else
            /* to avoid warnings at linker-time better to use mkstemp */
            /* Keva: !better, but worse! mkstemp creates the new file */
            /* handle with the attributes 0x600, but we need the same */
            /* attributes as the storage has; the special fix call is */
            /* performed to cover this feature                        */
            if ( (tmphandle = mkstemp(psztpl)) == -1 )
                ERROR(FSTORAGE_ERROR_WRITE);
            if ( fchmod( tmphandle, real_fs->file.mode ) != 0 )
                ERROR(FSTORAGE_ERROR_WRITE);
            if(mm_file_assign(real_fs->file.new_handle, tmphandle))
                ERROR(FSTORAGE_ERROR_WRITE);
#endif
            
            if(!mm_file_isopen(real_fs->file.new_handle))
                ERROR(FSTORAGE_ERROR_WRITE);
            
            /* save the storage */
            sub_error_code = fstorage_save(real_fs, real_fs->file.new_handle);

            mm_file_close(real_fs->file.handle);

            mm_file_close(real_fs->file.new_handle);
            mm_file_destroy(real_fs->file.new_handle);
            real_fs->file.new_handle = NULL;

            /* rename the file and free temporary name */
            remove(real_fs->file.path);
            if(rename(psztpl, real_fs->file.path))
                ERROR(FSTORAGE_ERROR_WRITE);

#ifdef WIN32
            mm_file_open(real_fs->file.handle, real_fs->file.path, 
                        O_RDWR | O_BINARY, real_fs->file.mode);
#else
            mm_file_open(real_fs->file.handle, real_fs->file.path, O_RDWR,
                        real_fs->file.mode);
#endif
            if(!mm_file_isopen(real_fs->file.handle))
                ERROR(FSTORAGE_ERROR_WRITE);

            if(sub_error_code)
                ERROR(sub_error_code);
        }

    if(fs->shared.ptr != NULL)
        mm_file_unmap(real_fs->file.handle);

    fs->shared.ptr    = NULL;
    fs->shared.length = 0;

    if(real_fs->use_counter == 0 && real_fs->file.handle)
        {
            if(mm_file_isopen(real_fs->file.handle))
               mm_file_close(real_fs->file.handle);
            mm_file_destroy(real_fs->file.handle);
            real_fs->file.handle = NULL;
        }
    

#undef ERROR
    error_code = FSTORAGE_OK;

  finish:

    return error_code;
}

int fstorage_purge(fstorage *fs)
{
    int error_code = FSTORAGE_ERROR_UNKNOWN;
    fstorage *real_fs = NULL;

#define ERROR(ERR) do { if(fs) fs->last_error = (ERR); error_code = (ERR); goto finish; } while(0)

    if(fs == NULL)
        ERROR(FSTORAGE_ERROR_INVALID_PARAMS);

    real_fs = REAL_FS(fs);

    if(real_fs->use_counter == 0 && real_fs->file.handle)
        {
            if(mm_file_isopen(real_fs->file.handle))
                mm_file_close(real_fs->file.handle);

            mm_file_destroy(real_fs->file.handle);
            real_fs->file.handle = NULL;
        }

#undef ERROR
    error_code = FSTORAGE_OK;

  finish:

    return error_code;
}

const char *fstorage_get_path(fstorage *fs)
{
    if(fs == NULL)
        return NULL;
    else
        return fs->file.path;
}

enum fstorage_open_modes fstorage_open_mode(fstorage *fs)
{
    if(fs == NULL)
        return FSTORAGE_OPEN_CLOSED;
    else
        return fs->fsom->mode;
}

int fstorage_is_shared(fstorage *fs)
{
    if(fs == NULL)
        return 0;

    if ( fs->shared.ptr != NULL && fs->trx_snapshot_idx == 0 )
        return 1;
    else 
        return 0;
}

static int fstorage_trx_section_write(struct fstorage_section_access_config *fsac, const void *buf, size_t nbytes)
{
    if ( nbytes > 0 )
    {
        size_t current_size = fsac->fss->memory.size;
        int    result       = fstorage_section_realloc( fsac->fss, current_size + nbytes );
        if ( result != FSTORAGE_OK )
            return -1;

        memcpy( fstorage_section_get_data(fsac->fss, current_size, nbytes), buf, nbytes );
    }
    return nbytes;
}

static int fstorage_trx_section_sync(struct fstorage_section_access_config * fsac)
{
    (void)fsac; /* not used */
    return 0;
}

static int fstorage_trx_sync_to_memory( fstorage_section *fss )
{
    int result;
    struct fstorage_section_access_config fsac;
    
    if ( fss->fssc.on_write )
    {
        memset( &fsac, 0, sizeof(fsac) );
        fsac.fss   = fss;
        fsac.write = fstorage_trx_section_write;
        fsac.sync  = fstorage_trx_section_sync;

        /* clear section */
        
        result = fstorage_section_realloc( fss, 0 );
        if ( result != FSTORAGE_OK )
        {
            if ( fss->fs != NULL )
                fss->fs->last_error = result;
            return result;
        }

        /* call callback */
        
        result = fss->fssc.on_write( &(fss->fssc), &fsac );
        if (result != FSTORAGE_OK)
        {
            if ( fss->fs != NULL )
                fss->fs->last_error = result;
            return result;
        }
        
        /* clear callback */
        
        fss->fssc.on_write = NULL;
    }
    else if (fss->memory.ptr)
    {
        /* nothing to do, everything seems to be ok */
    }
    else
    {
        /* load data from source storage */
            
        result = fstorage_section_load( fss );
        if (result != FSTORAGE_OK)
        {
            if ( fss->fs != NULL )
                fss->fs->last_error = result;
            return result;
        }
    }
    
    fss->fssc.on_read = NULL;
    
    return FSTORAGE_OK;
}

int fstorage_trx_snapshot( fstorage *fs, enum fstorage_trx_mode mode )
{
    fstorage *real_fs;
    struct modes_stack *new_mode;
    size_t i;
    fstorage_section *fss;
    int result;
    void *ptr;
    
    if (fs == NULL)
        return FSTORAGE_ERROR_INVALID_PARAMS;
    
    real_fs = REAL_FS(fs);

    new_mode = (struct modes_stack *)malloc(sizeof(struct modes_stack));
    if ( new_mode == NULL )
        return FSTORAGE_ERROR_NOMEMORY;
    switch ( mode )
    {
    case FSTORAGE_TRX_READ_ONLY:  new_mode->mode = FSTORAGE_OPEN_READ_ONLY;  break;
    case FSTORAGE_TRX_READ_WRITE: new_mode->mode = FSTORAGE_OPEN_READ_WRITE; break;
    default: new_mode->mode = FSTORAGE_OPEN_CLOSED; break;
    };
    new_mode->next = real_fs->fsom;
    real_fs->fsom  = new_mode;
    
    real_fs->trx_snapshot_idx++;
    
    for ( i = 0; i < real_fs->sections.number; i++ )
    {
        if ( real_fs->sections.array[i] != NULL )
        {
            if ( real_fs->sections.array[i]->trx_snapshot_idx == real_fs->trx_snapshot_idx )
                continue;
            
            fss = fstorage_section_create( fs, real_fs->sections.array[i]->sid );
            if ( fss == NULL )
                return FSTORAGE_ERROR_NOMEMORY;
            
            result = fstorage_trx_sync_to_memory( fss->shadowed );
            if ( result != FSTORAGE_OK )
                return result;
            
            result = FSTORAGE_ERROR_INVALID_PARAMS;
            if ( mode == FSTORAGE_TRX_READ_ONLY )
            {
                if ( real_fs->shared.ptr != NULL )
                    result = FSTORAGE_OK;
                else
                    result = fstorage_section_set_data( fss,
                                                        fstorage_section_get_all_data(fss->shadowed),
                                                        fstorage_section_get_size(fss->shadowed) );
            }
            else if ( mode == FSTORAGE_TRX_READ_WRITE )
            {
                if ( real_fs->shared.ptr != NULL )
                {
                    ptr       = fss->memory.ptr;
                    fss->memory.ptr = NULL;
                    result = fstorage_section_realloc( fss, fss->memory.size );
                    if ( result == FSTORAGE_OK )
                        memcpy( fss->memory.ptr, ptr, fss->memory.size );
                }
                else
                    result = fstorage_section_copy_data( fss,
                                                         fstorage_section_get_all_data(fss->shadowed),
                                                         fstorage_section_get_size(fss->shadowed) );
            }
            if ( result != FSTORAGE_OK )
                return result;
        }
    }
       
    return FSTORAGE_OK;
}

int fstorage_trx_rollback(fstorage *fs)
{
    fstorage *real_fs;
    size_t i;
    fstorage_section *fss;
    struct modes_stack *cur;
    
    if (fs == NULL)
        return FSTORAGE_ERROR_INVALID_PARAMS;
    
    real_fs = REAL_FS(fs);
    
    if ( real_fs->trx_snapshot_idx == 0 )
        return FSTORAGE_OK;
    
    for ( i = 0; i < real_fs->sections.number; i++ )
    {
        fss = real_fs->sections.array[i];
        
        if ( fss != NULL && fss->trx_snapshot_idx == real_fs->trx_snapshot_idx )
        {
            if ( fss->shadowed != NULL )
                real_fs->sections.array[i] = fss->shadowed;
            else
                real_fs->sections.array[i] = NULL;
        
            fstorage_section_destroy(fss);
        }
    }
    
    real_fs->trx_snapshot_idx--;

    cur = real_fs->fsom;
    real_fs->fsom = real_fs->fsom->next;
    free(cur);
    
    return FSTORAGE_OK;
}

int fstorage_trx_provide_section(fstorage *fs, enum fstorage_trx_mode mode, fstorage_section_id sid, void *buf, size_t buf_size)
{
    fstorage_section *sec;
    
    if (fs == NULL)
        return FSTORAGE_ERROR_INVALID_PARAMS;
    
    sec = fstorage_get_section( fs, sid );
    if (sec == NULL)
        return FSTORAGE_ERROR_NOSECTION;
    
    if ( mode == FSTORAGE_TRX_READ_ONLY )
        return fstorage_section_set_data( sec, buf, buf_size );
    else if ( mode == FSTORAGE_TRX_READ_WRITE )
        return fstorage_section_copy_data( sec, buf, buf_size );
    return FSTORAGE_ERROR_INVALID_PARAMS;
}

int fstorage_trx_commit(fstorage *fs, struct fstorage_trx_committer *fscm)
{
    size_t i;
    fstorage_section *cur_section;
    int result;
    size_t size;
    
    if (fs == NULL)
        return FSTORAGE_ERROR_INVALID_PARAMS;

    if (fs->trx_snapshot_idx == 0)
        return FSTORAGE_OK;
    
    /* Collect data and commit the difference */
    
    for ( i = 0; i < fs->sections.number; i++ )
    {
        cur_section = fs->sections.array[i];

        if ( cur_section->trx_snapshot_idx != fs->trx_snapshot_idx )
            continue;
        
        result = fstorage_trx_sync_to_memory( cur_section );
        if ( result != FSTORAGE_OK )
        {
            if ( fs != NULL )
                fs->last_error = result;
            return result;
        }
        
        size = fstorage_section_get_size(cur_section);
        
        if ( cur_section->shadowed != NULL &&
             size == fstorage_section_get_size(cur_section->shadowed) &&
             (fnv_hash_64(fstorage_section_get_all_data(cur_section), size) ==
              fnv_hash_64(fstorage_section_get_all_data(cur_section->shadowed), size)) )
            continue;

        /* Commit */
        result = fscm->receive_section( fscm,
                                        fstorage_section_get_id( cur_section ),
                                        fstorage_section_get_all_data( cur_section ),
                                        size );
        if ( result != FSTORAGE_OK )
        {
            if ( fs != NULL )
                fs->last_error = result;
            return result;
        }
    }
    
    return FSTORAGE_OK;
}

/*
 * <eof fstorage.c>
 */
