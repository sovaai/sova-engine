#ifndef _FILE_RAII_H_
#define _FILE_RAII_H_

#include <errno.h>
#include <stdio.h>

#include <_include/_string.h>
#include <lib/aptl/avector.h>

struct FILE_RAII
{
    FILE *f;

    FILE_RAII() : f( 0 ) { }
    FILE_RAII( FILE *_f ) : f( _f ) { }
    FILE_RAII( const char *filename ) : f( fopen( filename, "rb" ) ) { }
    ~FILE_RAII() { if ( f != 0 ) { fclose( f ); f = 0; } }

    int loadContents( avector<char> &contents, const char *filename )
    {
        if ( f == 0 )
        {
            f = fopen( filename, "rb" );
            if ( f == 0 )
            {
                fprintf( stderr, "Filed to open '%s': %s!\n", filename, strerror(errno) );
                return 1;
            }
        }
        
        if ( fseek( f, 0, SEEK_END ) != 0 )
        {
            fprintf( stderr, "Cannot determine file size of '%s': %s!\n", filename, strerror(errno) );
            return 1;
        }

        contents.resize( ftell( f ) );

        if ( contents.no_memory() )
        {
            fprintf( stderr, "No memory for reading file contents: %s!\n", filename );
            return 1;
        }

        rewind( f );

        size_t bread = fread( contents.get_buffer(), 1, contents.size(), f );
        if ( bread != contents.size() )
        {
            fprintf( stderr, "Cannot read file contents of '%s': %s!\n", filename, strerror(errno) );
            return 1;
        }

        return 0;
    }
};

#endif /* _FILE_RAII_H_ */

