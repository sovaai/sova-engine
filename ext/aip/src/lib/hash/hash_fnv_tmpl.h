#ifndef _HASH_FNV_TMPL_H_
#define _HASH_FNV_TMPL_H_

// C++ template wrapper over hash_fnv

#include <cstddef>
#include "hash_fnv.h"

// Dummy generic implementation (for sizeof(size_t) != 4, 8).
template<std::size_t = sizeof(std::size_t)>
struct Fnv_hash
  {
      static std::size_t
      hash(const char* __first, std::size_t __length)
      {
          std::size_t __result = 0;
          for (; __length > 0; --__length)
            __result = (__result * 131) + *__first++;
          return __result;
      }
  };

template<>
struct Fnv_hash<4>
  {
      static std::size_t
      hash(const char* __first, std::size_t __length)
      { return static_cast<std::size_t>(fnv_hash_32(__first, __length)); }
  };

template<>
struct Fnv_hash<8>
  {
      static std::size_t
      hash(const char* __first, std::size_t __length)
      { return static_cast<std::size_t>(fnv_hash_64(__first, __length)); }
  };

#endif
