# if !defined( __unordered_map_h__ )
# define __unordered_map_h__

// 1) std::unordered_map std::unordered_set adaptors
// 2) specialisation for map/set with const char* keys
//     class springpmap
//     class stringpset

// C++11 standard - no tr1 namespace, no tr1 include dir
// gcc 4.2.1 - tr1 namespace, tr1 include dir
#if defined(_MSC_VER) || defined(__clang__)
#else
#  define _HAVE_TR1 1
#  define _HAVE_FNV 1
#  if __GNUC__ > 3 && __GNUC_MINOR__ > 4
#    define HASH_F _Fnv_hash
#  else
#    define HASH_F _Fnv_hash<>
#  endif
#endif

#if _HAVE_TR1
#  include <tr1/unordered_map>
#  include <tr1/unordered_set>

#  define NS_TR1_BEG namespace tr1 {
#  define NS_TR1_END }
#  define NS_TR1     std::tr1

namespace std {
using tr1::unordered_map;
using tr1::unordered_set;
}

#else
#  include <unordered_map>
#  include <unordered_set>

#  define NS_TR1_BEG
#  define NS_TR1_END
#  define NS_TR1     std
#endif




#if _HAVE_FNV
   template<std::size_t = sizeof(std::size_t)> struct Fnv_hash : public NS_TR1::HASH_F {};
#else
#  include <lib/hash/hash_fnv_tmpl.h>
#endif


//****************************
#include <string.h>

class StrL {
  const char* m_p;
  size_t      m_l;

  public:
    StrL(const char* p=NULL, size_t l=static_cast<size_t>(-1)) : m_p(p), m_l(l) {
      if(!p) m_p="";
      if(l == static_cast<size_t>(-1)) m_l = strlen(m_p);
    }

    const char* str()   const { return m_p; }
    size_t      len()   const { return m_l; }
    bool        empty() const { return m_l == 0; }

    bool operator==(const StrL& x) const { return m_l == x.m_l ? m_l ? !memcmp(m_p,x.m_p,m_l) : true : false; }

    void SetStr(const char* p) { m_p = p; }
    void SetLen(size_t l) { m_l = l; }
    void clear() {m_p =""; m_l=0; }
    void Set(const char* p, size_t l=static_cast<size_t>(-1)) {
        if(p) { m_p=p; m_l= (l == static_cast<size_t>(-1)) ? strlen(m_p) : l; }
        else clear();
    }

};

namespace std {
  NS_TR1_BEG
     template<> struct hash<StrL> : public std::unary_function<const StrL&, std::size_t> {
         std::size_t operator()(const StrL& __s) const
         { return Fnv_hash<>::hash(__s.str(), __s.len()); }
       };
  NS_TR1_END
}

//***************************

template<typename V>
class stringpmap : public NS_TR1::unordered_map<StrL, V> {
  typedef NS_TR1::unordered_map<StrL, V> Parent;
  public:
    using Parent::insert;
    void insert(const char* s, const V& v, size_t l=static_cast<size_t>(-1)) {
        Parent::insert(std::pair<StrL, V>(StrL(s,l), v));
    }
};

//***************************

class stringpset : public NS_TR1::unordered_set<StrL> {
  typedef NS_TR1::unordered_set<StrL> Parent;
  public:
    using Parent::insert;
    void insert(const char* s, size_t l=static_cast<size_t>(-1)) {
        Parent::insert(StrL(s,l));
    }
};

#endif
