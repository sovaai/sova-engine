#ifndef _LIBBUCKWALTER_PASCAL_H_
#define _LIBBUCKWALTER_PASCAL_H_
#include "utils.h"
#include <_include/_inttype.h>
#include <string>
size_t      pascal_size(const std::string& str);
void        pascal_write(const std::string& str, uint8_t* buffer);
uint8_t     pascal_size(const uint8_t* buffer);
TCharBuffer pascal_get(const uint8_t* buffer);
#endif /* _LIBBUCKWALTER_PASCAL_H_ */
