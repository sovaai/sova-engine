#ifdef _WIN32
#include <io.h>
#else
#include <fnmatch.h>
#include <glob.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <dirent.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "StrUtils.h"
#include "ScanPath.h"

#ifdef _WIN32
int ScanPath(const char* path, bool subdirs, int (*filefunc)(const char*, const char*, const char*, void*), const char* relpath, void* extra)
{
  char path_buffer[8192];
  
  char drive[_MAX_DRIVE];
  char dir[8192];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];

  char newdir[8192];

  char relpath_buffer[8192];

  struct _finddata_t fd;
  long filespec;

  // Return code
  int ret = 0;
 
  // Splitting path
  _splitpath(path, drive, dir, fname, ext);

  // Scanning files
  if( (filespec = _findfirst(path, &fd)) != -1 )
  {
    do
    {
      if( !(fd.attrib & _A_SUBDIR)  )
      {
        _makepath(path_buffer, drive, dir, fd.name, "");
        if( ( ret = filefunc(path_buffer, fd.name, relpath, extra) ) != 0 )
          break;
      }
    }
    while( !_findnext(filespec, &fd) );
  }
  else if( errno == EINVAL )
  {
    return -1;
  }
  _findclose(filespec);

  // Checking return code
  if( ret )
    return ret;

  if(!subdirs)
    return ret;

  // Scanning subdirectories
  _makepath(path_buffer, drive, dir, "*", "*");
  if( (filespec = _findfirst(path_buffer, &fd)) != -1 )
  {
    do
    {
      if( (fd.attrib & _A_SUBDIR) &&
                    strcmp(fd.name, ".") && strcmp(fd.name, "..") )
      {
        if(strlen(dir)+strlen(fd.name)+2 > sizeof(newdir))
          continue;

        strcpy(newdir, dir);
        strcat(newdir, fd.name);
        strcat(newdir, "/");
        _makepath(path_buffer, drive, newdir, fname, ext);

        if(relpath)
        {
          if(*relpath)
          {
            strcpy(relpath_buffer, relpath);
            strcat(relpath_buffer, "/");
          }
          else
            relpath_buffer[0] = '\0';

          strcat(relpath_buffer, fd.name);
        }

        if( (ret = ScanPath(path_buffer, subdirs, filefunc,
                                    (relpath ? relpath_buffer : NULL), extra)) != 0)
          break;
      }
    }
    while( !_findnext(filespec,&fd) );
  }
  else
  {
    return -1;
  }
  _findclose(filespec);

  return ret;
}
#else
int ScanPath(const char* path, bool subdirs, int (*filefunc)(const char*, const char*, const char*, void*), const char* relpath, void* extra)
{
    glob_t g;
    struct stat sb;
    int ret = 0;
    size_t i;

    if(!glob(path, GLOB_TILDE, NULL, &g)) {
        for(i = 0; g.gl_pathv[i]; i++) {
            if(stat(g.gl_pathv[i], &sb))
                continue;
            if(sb.st_mode & S_IFDIR)
                continue;
            if((ret = filefunc(g.gl_pathv[i], g.gl_pathv[i], relpath, extra)))
                break;
        }
        globfree(&g);
        if(ret)
            return ret;
    }

    if(!subdirs)
        return ret;
       
    char pszParentDir[MAXPATHLEN + 2];
    char pszBasename[MAXPATHLEN];
    {
        char szTmp[MAXPATHLEN];        // linux: dirname modify input!!!
        strcpy(szTmp, path);
        strcpy(pszParentDir, dirname(szTmp));
        strcpy(szTmp, path);
        strcpy(pszBasename, basename(szTmp));
    }
    strcat(pszParentDir, "/*");

    if(glob(pszParentDir, GLOB_TILDE, NULL, &g))
        return 0;
    for(i = 0; g.gl_pathv[i]; i++) {
        if(stat(g.gl_pathv[i], &sb))
            continue;
        if(!(sb.st_mode & S_IFDIR))
            continue;
        char* buf = new char[strlen(g.gl_pathv[i]) + strlen(pszBasename) + 2];
        strcpy(buf, g.gl_pathv[i]);
        strcat(buf, "/");
        strcat(buf, pszBasename);
        ret=ScanPath(buf, true, filefunc, relpath, extra);
        delete[] buf;
        if(ret)
            break;
    }
    globfree(&g);

    return ret;
}
#endif

int ScanFileList(const char* filelist, int (*filefunc)(const char*, const char*, const char*, void*), void* extra)
{
  FILE* list = fopen(filelist, "rt");
  if(!list)
    return 1;

#ifdef _WIN32
  char path_buffer[_MAX_PATH];
  
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];

  _splitpath(filelist, drive, dir, fname, ext);
#else
  char path_buffer[PATH_MAX]; path_buffer[PATH_MAX-2] = 0;
  strncpy(path_buffer,filelist,sizeof(path_buffer)-2);

  char* dir = dirname(path_buffer);
  size_t dir_end = strlen(dir);
  memmove(path_buffer,dir,dir_end);
  path_buffer[dir_end++] = '/';
  path_buffer[dir_end] = 0;
#endif

  // Return code
  int ret = 0;

  char buf[4096];

  while(fgets_n(buf, sizeof(buf), list))
  {
    // comments
    char* p = strstr(buf, "//");
    if(p)
      *p = '\0';
    while(buf[0] != '\0' && (buf[strlen(buf)-1] == ' ' || buf[strlen(buf)-1] == '\t'))
      buf[strlen(buf)-1] = '\0';
    if(buf[0] == '\0')
      continue;

#ifdef _WIN32
    _makepath(path_buffer, drive, dir, buf, "");
#else
    strcpy(path_buffer+dir_end,buf);
#endif

    if( (ret = ScanPath(path_buffer, false, filefunc, NULL, extra)) != 0 )
      break;

  }

  fclose(list);
  return ret;
}


#ifdef _TEST_

int fff(const char* path, const char* name, const char* relpath, void* extra)
{
    printf("file='%s'\n", path);
    return 0;
}

int main(int ac, char** av)
{
    for(int i=1; i<ac; i++) {
        int ret = ScanPath(av[i], true, fff, NULL, NULL);
        printf("    Err=%d\n", ret);
    }
    return 0;
}
#endif
