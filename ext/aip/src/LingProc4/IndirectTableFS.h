#ifndef _INDIRECTTABLEFS_H_
#define _INDIRECTTABLEFS_H_

#include <lib/fstorage/fstorage.h>

#include "LingProcErrors.h"
#include "IndirectTable.h"

struct CopyLoader
{
    void operator()( IndirectTable &table, const char *buffer, size_t size ) const
    {
        table.deserializeCopy( buffer, size );
    }
};

struct FixedLoader
{
    void operator()( IndirectTable &table, const char *buffer, size_t size ) const
    {
        table.deserializeFixed( buffer, size );
    }
};

template< class Loader >
inline LingProcErrors loadFromFSection( fstorage *fs, fstorage_section_id sec_id, IndirectTable &table )
{
    // find section
    fstorage_section *sec = fstorage_find_section( fs, sec_id );
    if ( sec == 0 )
        return LP_ERROR_FS_FAILED;

    // load section
    if ( fstorage_section_load( sec ) != FSTORAGE_OK )
        return LP_ERROR_FS_FAILED;

    // getting pointer
    void *pt = fstorage_section_get_all_data( sec );
    if ( pt == 0 )
        return LP_ERROR_FS_FAILED;

    Loader loader;
    loader( table, reinterpret_cast<const char *>(pt), fstorage_section_get_size( sec ) );
        
    return LP_OK;
}

inline LingProcErrors saveToFSection( fstorage *fs, fstorage_section_id sec_id, IndirectTable &table )
{
    fstorage_section *sec = fstorage_get_section( fs, sec_id );
    if ( sec == 0 )
        return LP_ERROR_FS_FAILED;

    const char *data;
    size_t      dataSize;
    table.serialize( data, dataSize );
    
    if ( fstorage_section_set_data( sec, reinterpret_cast<void *>( const_cast<char *>( data ) ), dataSize ) != 0 )
        return LP_ERROR_FS_FAILED;

    return LP_OK;
}

#endif /* _INDIRECTTABLEFS_H_ */

