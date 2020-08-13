#include "pascalstrings.h"
#include <cstring>

size_t pascal_size(const std::string& str)
{
    return str.size() + 1;
}

void pascal_write(const std::string& str, uint8_t* buffer)
{
    uint8_t length  =   str.size();
    memcpy(buffer, static_cast<void*>(&length), sizeof(length));
    memcpy(buffer + 1, static_cast<const void*>(str.c_str()), sizeof(char) * length);
}

uint8_t pascal_size(const uint8_t* buffer)
{
    return *buffer;
}

TCharBuffer pascal_get(const uint8_t* buffer)
{
    return TCharBuffer(buffer + 1, pascal_size(buffer) + 1 + buffer);
}
