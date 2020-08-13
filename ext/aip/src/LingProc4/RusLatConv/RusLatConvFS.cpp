
#include <string.h>

#include "../IndirectTableFS.h"

#include "RusLatConvFS.h"

// fstorage section offsets
#define RUSLATCONV_FS_SEC_UNIQUE_CHARS  1
#define RUSLATCONV_FS_SEC_RUS_TO_LAT    2
#define RUSLATCONV_FS_SEC_LAT_TO_RUS    3

LingProcErrors RusLatConverterFS::Load( fstorage* fs, fstorage_section_id sec_id )
{
    if ( fs == 0 )
        return LP_ERROR_EINVAL;

    LingProcErrors status;

    // loading tables

    status = loadFromFSection< CopyLoader >( fs, sec_id + RUSLATCONV_FS_SEC_UNIQUE_CHARS, uniqueChars );
    if ( status != LP_OK )
        return status;

    status = loadFromFSection< CopyLoader >( fs, sec_id + RUSLATCONV_FS_SEC_RUS_TO_LAT, rusToLat );
    if ( status != LP_OK )
        return status;

    status = loadFromFSection< CopyLoader >( fs, sec_id + RUSLATCONV_FS_SEC_LAT_TO_RUS, latToRus );
    if ( status != LP_OK )
        return status;

    return LP_OK;
}

LingProcErrors RusLatConverterFS::Connect(fstorage* fs, fstorage_section_id sec_id)
{
    if ( fs == 0 )
        return LP_ERROR_EINVAL;

    LingProcErrors status;

    // loading tables

    status = loadFromFSection< FixedLoader >( fs, sec_id + RUSLATCONV_FS_SEC_UNIQUE_CHARS, uniqueChars );
    if ( status != LP_OK )
        return status;

    status = loadFromFSection< FixedLoader >( fs, sec_id + RUSLATCONV_FS_SEC_RUS_TO_LAT, rusToLat );
    if ( status != LP_OK )
        return status;

    status = loadFromFSection< FixedLoader >( fs, sec_id + RUSLATCONV_FS_SEC_LAT_TO_RUS, latToRus );
    if ( status != LP_OK )
        return status;

    return LP_OK;
}

LingProcErrors RusLatConverterFS::Save( fstorage *fs, fstorage_section_id sec_id )
{
    if ( fs == 0 )
        return LP_ERROR_EINVAL;

    if ( fstorage_is_shared(fs) )
        return LP_ERROR_INVALID_MODE;

    LingProcErrors status;
    
    // saving byte tables
    
    status = saveToFSection( fs, sec_id + RUSLATCONV_FS_SEC_UNIQUE_CHARS, uniqueChars );
    if ( status != LP_OK )
        return status;

    status = saveToFSection( fs, sec_id + RUSLATCONV_FS_SEC_RUS_TO_LAT, rusToLat );
    if ( status != LP_OK )
        return status;

    status = saveToFSection( fs, sec_id + RUSLATCONV_FS_SEC_LAT_TO_RUS, latToRus );
    if ( status != LP_OK )
        return status;
    
    return LP_OK;
}

