#ifndef __ConsoleOutput_hpp__
#define __ConsoleOutput_hpp__

#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <climits>
#include <cstdlib>


#include <InfEngine2/_Include/Errors.h>

/**
 *  Check target dir path and convert it to absolute value.
 * @param aDirPath - target dir path. This pointer will be moved on absolute value buffer.
 * @param aAbsoluteDirPath - buffer for absolute path value.
 */
inline InfEngineErrors PrepareDirPath( const char *& aDirPath, char* aAbsoluteDirPath )
{
    // If aDirPath is nullptr, then return success.
    if( !aDirPath )
    {
        if( aAbsoluteDirPath )
            *aAbsoluteDirPath = '\0';

        return INF_ENGINE_SUCCESS;
    }

    // Get directory stat.
    struct stat fStat;
    if( stat( aDirPath, &fStat ) != 0 )
        return INF_ENGINE_ERROR_FAULT;

    // Check directory path.
    if( !( fStat.st_mode & S_IFDIR ) )
    {
        errno = ENOTDIR;

        return INF_ENGINE_ERROR_FAULT;
    }

    // Create absolute path.
    if( aAbsoluteDirPath && ( aDirPath = realpath( aDirPath, aAbsoluteDirPath ) ) == nullptr )
        return INF_ENGINE_ERROR_FAULT;

    return INF_ENGINE_SUCCESS;
}

/**
 *  Check target file path and convert it to absolute value.
 * @param aFilePath - target file path. This pointer will be moved on absolute value buffer.
 * @param aAbsoluteFilePath - buffer for absolute path value.
 */
inline InfEngineErrors PrepareFilePath( const char *& aFilePath, char* aAbsoluteFilePath )
{
    // If aFilePath is nullptr, then return success.
    if( !aFilePath )
    {
        if( aAbsoluteFilePath )
            *aAbsoluteFilePath = '\0';

        return INF_ENGINE_SUCCESS;
    }

    // Get file stat.
    struct stat fStat;
    if( stat( aFilePath, &fStat ) != 0 )
        return INF_ENGINE_ERROR_FAULT;

    // Check file path.
    if( fStat.st_mode & S_IFDIR || !( fStat.st_mode & S_IFREG ) )
    {
        errno = EISDIR;

        return INF_ENGINE_ERROR_FAULT;
    }

    // Create absolute path.
    if( aAbsoluteFilePath && ( aFilePath = realpath( aFilePath, aAbsoluteFilePath ) ) == nullptr )
        return INF_ENGINE_ERROR_FAULT;

    return INF_ENGINE_SUCCESS;
}

#define PrintDebugMessage( aFormat, ... ) \
    do \
    { \
        printf( "----[DEBUG] " ); \
        printf( aFormat, ##__VA_ARGS__ ); \
        printf( "\n" ); \
    } \
    while( 0 )

#define PrintHeader( aFormat, ... ) \
    do \
    { \
        printf( " * " ); \
        printf( aFormat, ##__VA_ARGS__ ); \
        printf( "\n" ); \
    } \
    while( 0 )

#define PrintWarn( aFormat, ... ) \
    do \
    { \
        printf( "    [WARN] " ); \
        printf( aFormat,##__VA_ARGS__ ); \
        printf( "\n" ); \
    } \
    while( 0 )

#define PrintError( aFormat, ... ) \
    do \
    { \
        printf( "    [FAILED] " ); \
        printf( aFormat,##__VA_ARGS__ ); \
        printf( "\n" ); \
    } \
    while( 0 )

#define PrintSuccess( aFormat, ... ) \
    do \
    { \
        printf( "    [SUCCESS] " ); \
        printf( aFormat,##__VA_ARGS__ ); \
        printf( "\n" ); \
    } \
    while( 0 )

#endif /** __ConsoleOutput_hpp__ */
