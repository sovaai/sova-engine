/**
 *  Build configuration.
 */


/** Configuration */

/** InfEngine release information. */
#define InfEngineReleaseVersion "1.8.5s"

#ifdef __FreeBSD__

#define BuildSystem "FreeBSD"

#else

#define BuildSystem "GNU/Linux"

#endif

#ifdef __LP64__

#define ServerType "x64"

#else

#define ServerType "x32"

#endif

#define ServerName "localhost"
