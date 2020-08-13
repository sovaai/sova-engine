#include "utils.h"
#include <algorithm>

TConstString Strip(TConstString str, char c)
{
    if (str.IsEmpty()) {
        return str;
    }

    const char* begin   =   str.Begin;
    while ((begin != str.End) && (c == *begin)) {
        ++begin;
    }
    const char* end     =   str.End;
    while ((begin != end) && (c == *(end - 1))) {
        --end;
    }

    return TConstString(begin, end);
}

void Split(TConstString str, char c, TSplits& splits)
{
    const char* begin   =   str.Begin;
    const char* end     =   NULL;
    while ((begin != str.End) && (end != str.End)) {
        end     =   std::find(begin, str.End, c);
        splits.push_back(TConstString(begin, end));
        begin   =   end + 1;
    }
    if (begin == str.End) {
        end     =   begin;
        splits.push_back(TConstString(begin, end));
    }
}

const uint16_t   kTreeFlags  =   PatriciaTree::TREE_GENERIC;
