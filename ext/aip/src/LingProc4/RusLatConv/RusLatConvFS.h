#ifndef _RUSLATCONVFS_H_
#define _RUSLATCONVFS_H_

#include <assert.h>
#include <stdlib.h>

#include <lib/fstorage/fstorage.h>

#include "RusLatConv.h"
#include "../LingProcErrors.h"

class RusLatConverterFS : public RusLatConverter
{
public:
    RusLatConverterFS() : RusLatConverter() { }
    ~RusLatConverterFS() { }

    LingProcErrors Load( fstorage* fs, fstorage_section_id sec_id );
    LingProcErrors Connect( fstorage* fs, fstorage_section_id sec_id );
    LingProcErrors Save( fstorage* fs, fstorage_section_id sec_id );

private:
    // copy prevention
    RusLatConverterFS( const RusLatConverterFS &src ) : RusLatConverter( src ) {}
    RusLatConverterFS &operator=(const RusLatConverterFS & ) { assert(false); abort(); return *this; }
};

#endif /* _RUSLATCONVFS_H_ */

