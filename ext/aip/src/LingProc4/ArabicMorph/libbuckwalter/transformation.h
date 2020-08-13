#ifndef _LIBBUCKWALTER_TRANSFORMATION_H_
#define _LIBBUCKWALTER_TRANSFORMATION_H_

#include <lib/aptl/avector.h>
#include <cstring>
#include <_include/_inttype.h>

//both functions return 0 if symbol is not mapped by buckwalter transforamtion
uint32_t buckwalterFromArabicToLatin(uint32_t symbol);
uint32_t buckwalterFromLatinToArabic(uint32_t symbol);

void buckwalterFromArabicToLatin(const char* str, size_t strLen, avector<char>& latin);
void buckwalterFromLatinToArabic(const char* str, size_t strLen, avector<char>& arabic);

#endif /* _LIBBUCKWALTER_TRANSFORMATION_H_ */
