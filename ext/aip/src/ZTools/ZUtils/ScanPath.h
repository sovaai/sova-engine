#ifndef __SCANPATH_H__
#define __SCANPATH_H__

int ScanPath(const char* path, bool subdirs, int (*filefunc)(const char*, const char*, const char*, void*), const char* relpath = NULL, void* extra = NULL);
int ScanFileList(const char* filelist, int (*filefunc)(const char*, const char*, const char*, void*), void* extra);

#endif

