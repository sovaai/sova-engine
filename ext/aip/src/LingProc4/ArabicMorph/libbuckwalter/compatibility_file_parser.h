#ifndef _LIBBUCKWALTER_COMPATIBILITY_FILE_PARSER_IFACE_H_
#define _LIBBUCKWALTER_COMPATIBILITY_FILE_PARSER_IFACE_H_
#include <cstring>

class ICompatibilityConsumer;
extern const size_t KNoErrorInCompatibilityFile;
size_t ParseCompatibilityFile(const char* filePath, ICompatibilityConsumer& consumer);
#endif /* _LIBBUCKWALTER_COMPATIBILITY_FILE_PARSER_IFACE_H_ */
