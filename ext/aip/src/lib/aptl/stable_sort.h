#ifndef __stable_sort_h
#define __stable_sort_h

///////////////////////////////////////////////////////////////////////////////
// stable sort (and related)
// copied from STLport
// (iterators replaced with pointers)
///////////////////////////////////////////////////////////////////////////////


// public declarations

template <class _Arg1, class _Arg2, class _Result>
struct aptl_binary_function {
  typedef _Arg1 first_argument_type;
  typedef _Arg2 second_argument_type;
  typedef _Result result_type;
};

template <class T> struct aptl_less : aptl_binary_function <T,T,bool> {
  bool operator() (const T& x, const T& y) const
    { return x < y; }
};

// converts qsort-compatible comparator int cmp(const void *, const void *)
// to STL-compatible bool less(T& r1, T& r2)
template<class T, class F>
struct aptl_compare_converter {
    F f;
    aptl_compare_converter(F func) : f(func) {}
    inline bool operator () (T& r1, T& r2) const
    {
        return f(&r1, &r2) < 0;
    }
};


template <class T, class Compare>
inline T* aptl_lower_bound(T* first,
                           T* last,
                           T& value,
                           Compare comp);

template <class T, class Compare>
inline T* aptl_upper_bound(T* first,
                           T* last,
                           T& value,
                           Compare comp);


template <class T, class Compare>
void aptl_inplace_stable_sort(T* __first,
                              T* __last,
                              Compare __comp);

template <class T, class Compare>
void aptl_inplace_stable_sort(T* __first,
                              size_t __size,
                              Compare __comp);

template <class T>
void aptl_inplace_stable_sort(T* __first, T* __last);

template <class T>
void aptl_inplace_stable_sort(T* __first, size_t __size);

// examples: sorting avector
/*
    // qsort-style compare function; returns <0, 0, >0 if a < b, a == b, a > b respectively
    int some_type_compare(const void *a, const void *b);
    ...
    avector<some_type> a;
    ...
    // compare function converter: some_type_less defines a weak ordering returning true if a < b
    aptl_compare_converter<some_type, int (*)(const void *, const void *)> some_type_less(some_type_compare);
    
    // apply sort template function externally
    aptl_inplace_stable_sort(&a[0], a.size(), some_type_less);
    
    // or begin-end version
    aptl_inplace_stable_sort(&a[0], &a[0] + a.size(), some_type_less);
    
    // or with default comparator, which is aptl_less<T>
    aptl_inplace_stable_sort(&a[0], a.size());
    
    // or begin-end version with default comparator
    aptl_inplace_stable_sort(&a[0], &a[0] + a.size());
    
*/

// private stuff

namespace aptl {

template <class T>
inline void aptl_swap ( T& a, T& b )
{
    T c(a);
    a = b;
    b = c;
}

template <class T>
inline void aptl_iter_swap ( T* a, T* b )
{
      aptl_swap (*a, *b);
}

template <class T>
inline T* aptl_swap_ranges( T* first1, T* last1, T* first2 )
{
    while (first1!=last1) aptl_swap(*first1++, *first2++);
    return first2;
}

template <class T>
inline T* aptl_rotate(T* __first,
               T* __middle,
               T* __last)
{
  if (__first == __middle)
    return __last;
  if (__last  == __middle)
    return __first;

  T* __first2 = __middle;
  do {
    aptl_swap(*__first++, *__first2++);
    if (__first == __middle)
      __middle = __first2;
  } while (__first2 != __last);

  T* __new_middle = __first;

  __first2 = __middle;

  while (__first2 != __last) {
    aptl_swap(*__first++, *__first2++);
    if (__first == __middle)
      __middle = __first2;
    else if (__first2 == __last)
      __first2 = __middle;
  }

  return __new_middle;
}

template <class T, class Compare>
inline void aptl_merge_without_buffer( T* __first,
                                       T* __middle,
                                       T* __last,
                                       size_t __len1,
                                       size_t __len2,
                                       Compare __comp)
{
  if (__len1 == 0 || __len2 == 0)
    return;
  if (__len1 + __len2 == 2) {
    if (__comp(*__middle, *__first)) {
      //_STLP_VERBOSE_ASSERT(!__comp(*__first, *__middle), _StlMsg_INVALID_STRICT_WEAK_PREDICATE)
      aptl_iter_swap(__first, __middle);
    }
    return;
  }
  T* __first_cut = __first;
  T* __second_cut = __middle;
  size_t __len11 = 0;
  size_t __len22 = 0;
  if (__len1 > __len2) {
    __len11 = __len1 / 2;
    __first_cut += __len11; // advance(__first_cut, __len11);
    __second_cut = aptl_lower_bound(__middle, __last, *__first_cut, __comp);
    __len22 += __second_cut - __middle; // distance(__middle, __second_cut);
  }
  else {
    __len22 = __len2 / 2;
    __second_cut += __len22; // advance(__second_cut, __len22);
    __first_cut = aptl_upper_bound(__first, __middle, *__second_cut, __comp);
    __len11 += __first_cut - __first; // distance(__first, __first_cut);
  }
  T* __new_middle
    = aptl_rotate(__first_cut, __middle, __second_cut);
  aptl_merge_without_buffer(__first, __first_cut, __new_middle, __len11, __len22,
                            __comp);
  aptl_merge_without_buffer(__new_middle, __second_cut, __last, __len1 - __len11,
                            __len2 - __len22, __comp);
}

// insert for small datasets (also recursion exit point)
template <class T, class Compare>
inline void aptl_unguarded_linear_insert(T* __last,
                                         T __val,
                                         Compare __comp)
{
  T* __next = __last;
  --__next;
  while (__comp(__val, *__next)) {
    //_STLP_VERBOSE_ASSERT(!__comp(*__next, __val), _StlMsg_INVALID_STRICT_WEAK_PREDICATE)
    *__last = *__next;
    __last = __next;
    --__next;
  }
  *__last = __val;
}

template<class T>
inline T* aptl_copy_backward (T* first,
                              T* last,
                              T* result )
{
    while (last!=first) *(--result) = *(--last);
    return result;
}

template <class T, class Compare>
inline void aptl_linear_insert(T* __first,
                               T* __last,
                               T __val,
                               Compare __comp)
{
  //*TY 12/26/1998 - added __val as a paramter
  //  _Tp __val = *__last;        //*TY 12/26/1998 - __val supplied by caller
  if (__comp(__val, *__first)) {
    //_STLP_VERBOSE_ASSERT(!__comp(*__first, __val), _StlMsg_INVALID_STRICT_WEAK_PREDICATE)
    aptl_copy_backward(__first, __last, __last + 1);
    *__first = __val;
  }
  else {
    aptl_unguarded_linear_insert(__last, __val, __comp);
  }
}

template <class T, class Compare>
inline void aptl_insertion_sort(T* __first,
                                T* __last,
                                // _Tp *,
                                Compare __comp)
{
  if (__first == __last) return;
  for (T* __i = __first + 1; __i != __last; ++__i) {
    aptl_linear_insert<T, Compare>(__first, __i, *__i, __comp);  //*TY 12/26/1998 - supply *__i as __val
  }
}

}; /* namespace aptl */

// public functions definitions

template <class T, class Compare>
inline T* aptl_lower_bound(T* first,
                           T* last,
                           T& value,
                           Compare comp)
{
  T* it;
  size_t count, step;
  count = last - first; // distance(first,last);
  while (count > 0)
  {
    it = first; step = count / 2; it += step; // advance (it,step);
    if (comp(*it, value)) // if (*it<value)                   // or: if (comp(*it,value)), for the comp version
      { first = ++it; count -= step + 1;  }
    else count = step;
  }
  return first;
}

template <class T, class Compare>
inline T* aptl_upper_bound(T* first,
                           T* last,
                           T& value,
                           Compare comp)
{
  T* it;
  size_t count, step;
  count = last - first; // distance(first,last);
  while (count > 0)
  {
    it = first; step=count / 2; it += step; // advance (it,step);
    if (!comp(value,*it)) // if (!(value<*it))                 // or: if (!comp(value,*it)), for the comp version
      { first = ++it; count -= step + 1;  }
    else count = step;
  }
  return first;
}


template <class T, class Compare>
void aptl_inplace_stable_sort(T* __first,
                              T* __last,
                              Compare __comp) {
  if (__last - __first < 15) {
      aptl::aptl_insertion_sort(__first, __last, /*_STLP_VALUE_TYPE(__first,_RandomAccessIter),*/ __comp);
    return;
  }
  T* __middle = __first + (__last - __first) / 2;
  aptl_inplace_stable_sort(__first, __middle, __comp);
  aptl_inplace_stable_sort(__middle, __last, __comp);
  aptl::aptl_merge_without_buffer(__first, __middle, __last,
                                  __middle - __first,
                                  __last - __middle,
                                  __comp);
}

template <class T, class Compare>
void aptl_inplace_stable_sort(T* __first,
                              size_t __size,
                              Compare __comp)
{
    return aptl_inplace_stable_sort(__first, __first + __size, __comp);
}

template <class T>
void aptl_inplace_stable_sort(T* __first, T* __last)
{
    aptl_inplace_stable_sort(__first, __last, aptl_less<T>() );
}

template <class T>
void aptl_inplace_stable_sort(T* __first, size_t __size)
{
    return aptl_inplace_stable_sort(__first, __first + __size);
}


#endif /*__stable_sort_h*/

