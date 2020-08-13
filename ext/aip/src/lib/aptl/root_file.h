#pragma once
#include <cstring>

#ifdef LOG_SOURCE_ROOT
#  define SOURCE_ROOT_LEN strlen(LOG_SOURCE_ROOT) + 1
#else
#  define SOURCE_ROOT_LEN 0
#endif

#define NORMALIZE_FILE_ADDITION__ (SOURCE_ROOT_LEN < strlen(__FILE__) ? SOURCE_ROOT_LEN : 0)

#define NORMALIZED__FILE__ (__FILE__ + NORMALIZE_FILE_ADDITION__)
