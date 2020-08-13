#include "compatibility_file_parser.h"
#include "compatibility_consumer_iface.h"
#include "utils.h"
#include <fstream>
#include <limits>
#include <string>

const size_t KNoErrorInCompatibilityFile    =   std::numeric_limits<size_t>::max();
size_t ParseCompatibilityFile(const char* filePath, ICompatibilityConsumer& consumer)
{
    std::ifstream   file(filePath);
    if (!file.is_open()) {
        return 0;
    }
    std::string     str;
    TConstString    buffer;
    TSplits         splits;
    size_t          lineNumber  =   0;
    bool            isError     =   false;
    while ((!isError) && (std::getline(file, str))) {
        lineNumber  +=  1;
        buffer      =   Strip(TConstString(str.c_str(), str.c_str() + str.size()));

        if (buffer.IsEmpty()) {
            continue;
        }

        if (buffer.Begin[0] == ';') {
            continue;
        }

        splits.clear();

        Split(buffer, ' ', splits);

        if (splits.size() != 2) {
            isError =   true;
            continue;
        }

        if (splits[0].IsEmpty()) {
            isError =   true;
            continue;
        }

        if (splits[1].IsEmpty()) {
            isError =   true;
            continue;
        }

        TCharBuffer first( reinterpret_cast<const uint8_t*>(splits[0].Begin)
                         , reinterpret_cast<const uint8_t*>(splits[0].End) );
        TCharBuffer second( reinterpret_cast<const uint8_t*>(splits[1].Begin)
                          , reinterpret_cast<const uint8_t*>(splits[1].End) );

        isError =   !consumer.Consume(first, second);
    }

    if (!isError) {
        lineNumber  =   KNoErrorInCompatibilityFile;
    }

    return lineNumber;
}

