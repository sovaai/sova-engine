#ifndef _UCHARSETFS_H_
#define _UCHARSETFS_H_

#include <lib/fstorage/fstorage.h>

#include "../LingProcErrors.h"

#include "UCharSet.h"

class UCharSetFS : public UCharSet
{
private:
    // copy prevention
    UCharSetFS( const UCharSetFS &src ) : UCharSet( src ) { }
    UCharSetFS &operator=( const UCharSetFS & ) { abort(); return *this; }

public:
    UCharSetFS() : UCharSet() { }
    UCharSetFS( LangCodes _lang ) : UCharSet( _lang ) { }
    ~UCharSetFS() { }

    LingProcErrors Load( fstorage *fs, fstorage_section_id sec_id );
    LingProcErrors Connect( fstorage *fs, fstorage_section_id sec_id );
    LingProcErrors Save( fstorage *fs, fstorage_section_id sec_id, bool transport );

protected:
    LingProcErrors LoadHeader( fstorage *fs, fstorage_section_id sec_id );
    LingProcErrors SaveHeader( fstorage *fs, fstorage_section_id sec_id );
};

#endif /* _UCHARSETFS_H_ */

