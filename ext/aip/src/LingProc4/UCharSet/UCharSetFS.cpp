
#include <_include/_inttype.h>
#include <_include/ntoh.h>

#include "../IndirectTableFS.h"

#include "UCharSetFS.h"

#define CHARSET_FS_HEADER_SIZE (sizeof(uint32_t) * 8)
#define CHARSET_FS_MAGIC 0x0F032A13u
#define CHARSET_FS_WORK_FORMAT_VERSION  2
#define CHARSET_FS_WORK_FORMAT_VARIANT  1
#define CHARSET_FS_TRANSPORT_FORMAT_VERSION  2
#define CHARSET_FS_TRANSPORT_FORMAT_VARIANT  1

#define CHARSET_FS_SEC_CHAR_PROPS     1
#define CHARSET_FS_SEC_CHAR_LOWER     2

LingProcErrors UCharSetFS::SaveHeader( fstorage *fs, fstorage_section_id sec_id )
{
    // requesting section
    fstorage_section *sec = fstorage_get_sized_section( fs, sec_id, CHARSET_FS_HEADER_SIZE );
    if ( sec == 0 )
        return LP_ERROR_FS_FAILED;

    // getting pointer
    uint32_t *pt = reinterpret_cast<uint32_t *>( fstorage_section_get_all_data( sec ) );
    if ( pt == 0 )
        return LP_ERROR_FS_FAILED;

    // saving data

    *pt++ = htobe32( static_cast<uint32_t>( CHARSET_FS_MAGIC ) );
    *pt++ = htobe32( static_cast<uint32_t>( CHARSET_FS_WORK_FORMAT_VERSION ) );
    *pt++ = htobe32( static_cast<uint32_t>( CHARSET_FS_WORK_FORMAT_VARIANT ) );
    *pt++ = htobe32( static_cast<uint32_t>( CHARSET_FS_TRANSPORT_FORMAT_VERSION ) );
    *pt++ = htobe32( static_cast<uint32_t>( CHARSET_FS_TRANSPORT_FORMAT_VARIANT ) );
    *pt++ = htobe32( static_cast<uint32_t>( 0 ) );
    *pt++ = htobe32( static_cast<uint32_t>( lang ) );
    *pt++ = htobe32( static_cast<uint32_t>( tuneMode ) );
    
    return LP_OK;
}

LingProcErrors UCharSetFS::LoadHeader( fstorage *fs, fstorage_section_id sec_id )
{
    // find section
    fstorage_section* sec = fstorage_find_section( fs, sec_id );
    if ( sec == 0 )
        return LP_ERROR_FS_FAILED;

    // load section
    if ( fstorage_section_load( sec ) != FSTORAGE_OK )
        return LP_ERROR_FS_FAILED;

    // checking section size
    if ( fstorage_section_get_size(sec) < CHARSET_FS_HEADER_SIZE )
        return LP_ERROR_FS_FAILED;

    // getting pointer
    uint32_t *pt = reinterpret_cast<uint32_t *>( fstorage_section_get_all_data( sec ) );
    if ( pt == 0 )
        return LP_ERROR_FS_FAILED;

    // getting data

    uint32_t magic = be32toh( *pt++ );
    if ( magic != CHARSET_FS_MAGIC )
        return LP_ERROR_INVALID_FORMAT;

    uint32_t work_format_version      = be32toh( *pt++ );
    /* uint32_t work_format_variant      = */ be32toh( *pt++ );
    /* uint32_t transport_format_version = */ be32toh( *pt++ );
    /* uint32_t transport_format_variant = */ be32toh( *pt++ );
    bool     is_transport             = ( be32toh( *pt++ ) != 0 );
    if ( is_transport )
        return LP_ERROR_ENOTIMPL;
    
    if ( work_format_version != CHARSET_FS_WORK_FORMAT_VERSION )
        return LP_ERROR_INVALID_FORMAT;
    
    lang     = static_cast<LangCodes>( be32toh( *pt++ ) );
    tuneMode = be32toh( *pt++ );
  
    return LP_OK;
    
}

LingProcErrors UCharSetFS::Load( fstorage *fs, fstorage_section_id sec_id )
{
    LingProcErrors status = LoadHeader( fs, sec_id );
    if ( status != LP_OK )
        return status;

    status = loadFromFSection< CopyLoader >( fs, sec_id + CHARSET_FS_SEC_CHAR_PROPS, charProps );
    if ( status != LP_OK )
        return status;

    status = loadFromFSection< CopyLoader >( fs, sec_id + CHARSET_FS_SEC_CHAR_LOWER, charLower );
    if ( status != LP_OK )
        return status;
    
    return LP_OK;
}

LingProcErrors UCharSetFS::Connect( fstorage *fs, fstorage_section_id sec_id )
{
    LingProcErrors status = LoadHeader( fs, sec_id );
    if ( status != LP_OK )
        return status;

    status = loadFromFSection< FixedLoader >( fs, sec_id + CHARSET_FS_SEC_CHAR_PROPS, charProps );
    if ( status != LP_OK )
        return status;

    status = loadFromFSection< FixedLoader >( fs, sec_id + CHARSET_FS_SEC_CHAR_LOWER, charLower );
    if ( status != LP_OK )
        return status;
    
    return LP_OK;
}

LingProcErrors UCharSetFS::Save( fstorage *fs, fstorage_section_id sec_id, bool /* transport */ )
{
    LingProcErrors status = SaveHeader( fs, sec_id );
    if ( status != LP_OK )
        return status;

    status = saveToFSection( fs, sec_id + CHARSET_FS_SEC_CHAR_PROPS, charProps );
    if ( status != LP_OK )
        return status;

    status = saveToFSection( fs, sec_id + CHARSET_FS_SEC_CHAR_LOWER, charLower );
    if ( status != LP_OK )
        return status;
    
    return LP_OK;
}

