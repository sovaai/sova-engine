#ifndef __a_vector_h__
#define __a_vector_h__
//============================================================================//
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
    #pragma warning(disable:4786) // identifier was truncated to '255' characters in the debug information
#endif
//============================================================================//
// avector DECLARATION
//============================================================================//
template <class T>
class avector
{
private:
    /* copy prevention */
    avector<T>(const avector<T>&) { }
    avector<T>& operator = (const avector<T>&) { return *this; }

public:
    /* CONSTRUCTORS AND DESTRUCTORS */
    avector(size_t growing_increment = 0,
            size_t growing_power = 0,
            size_t auto_free_limit = 0,
            size_t alloc_limit = size_t(-1)) :
            m_size(0), m_alloc(0), m_buffer(0), m_no_memory_flag(false)
    {
        m_growing_increment = growing_increment;
        m_growing_power = growing_power;
        m_auto_free_limit = auto_free_limit;
        m_alloc_limit = alloc_limit;
        m_initial_size = 0;
    }

    virtual ~avector() { free_buffer(); }

    /* PUBLIC ACCESS  = vector's methods */

    // clear - Erases the elements of the vector.
    void clear() { resize(0); }

    // empty - Tests if the vector container is empty.
    bool empty() const { return (m_size == 0); }

    // size - Returns the number of elements in the vector.
    size_t size() const { return m_size; }
    
    // size_bytes - Returns total memory occupied by the vector's elements.
    size_t size_bytes() const { return m_size * sizeof(T); }
    
    // resize - Specifies a new size for a vector.
    void resize(size_t new_size);

    // push_back - Add an element to the end of the vector.
    void push_back(const T& item);

    // pop_back - Deletes the element at the end of the vector.
    void pop_back() { if(m_size) resize(m_size - 1); }
 
    // reserve - Reserves a minimum length of storage for a vector object.
    void reserve(size_t new_capacity) { ensure_alloc(new_capacity); }

    // capacity - Returns the number of elements that the vector could contain without allocating more storage.
    size_t capacity() const { return m_alloc; }

    // max_size - Returns the maximum length of the vector.
    size_t max_size() const { return m_alloc_limit; }

    // at - Returns a reference to the element at a specified location in the vector.
    T& at(size_t pos) { return m_buffer[pos]; }
    const T& at(size_t pos) const { return m_buffer[pos]; };
    
    // operator[] - Returns a reference to the vector element at a specified position.
    T& operator[](size_t pos) { return m_buffer[pos]; }
    const T& operator[](size_t pos) const { return m_buffer[pos]; }

    // back - Returns a reference to the last element of the vector.
    T& back() { return m_buffer[m_size - 1]; }
    const T& back() const { return m_buffer[m_size - 1]; }

    // front - Returns a reference to the first element in a vector.
    T& front() { return m_size ? m_buffer[0] : 0; }
    const T& front() const { return m_size ? m_buffer[0] : 0; }

    // assign - Erases a vector and copies the specified elements to the empty vector
    // begin - Returns a random-access iterator to the first element in the container.
    // end - Returns a random-access iterator that points just beyond the end of the vector.
    // erase - Removes an element or a range of elements in a vector from specified positions.
    // get_allocator -  Returns an object to the allocator class used by a vector.
    // insert - Inserts an element or a number of elements into the vector at a specified position.
    // rbegin - Returns an iterator to the first element in a reversed vector.
    // rend - Returns an iterator to the end of a reversed vector.
    // swap - Exchanges the elements of two vectors.
    
    /* PUBLIC ACCESS  = extra methods */

    // free allocated memory
    void free_buffer() { resize(0); if(m_buffer) free(m_buffer); m_buffer = NULL; m_alloc = 0; m_no_memory_flag = false; }

    // check status of last realloc
    bool no_memory() const { return m_no_memory_flag; }

    // add a new element and return pointer to it, NULL on error
    T* grow();

    // add a new ***UNINITIALIZED*** element and return pointer to it, NULL on error
    // WARNING: caller MUST construct (by inplace new or by other way) object T
    T* uninitialized_grow(size_t nGrow=1);

    // returns pointer to buffer, may become invalid after next realloc
    T* get_buffer() const { return m_buffer; }

    void remove(size_t pos);

    void qsort(int (*comp_func)(const void *, const void *));
    
protected:
    void set_initial_size(size_t initial_size) { m_initial_size = initial_size; }
    void set_growing_increment(size_t growing_increment) { m_growing_increment = growing_increment; }
    void set_growing_power(size_t growing_power) { m_growing_power = growing_power; }
    
private:
    /* PRIVATE MEMBERS */
    bool ensure_alloc(size_t new_alloc);

    /* PRIVATE DATA */
    size_t m_size;
    size_t m_alloc;
    T* m_buffer;
    bool m_no_memory_flag;

    size_t m_growing_increment;
    size_t m_growing_power;
    size_t m_initial_size;
    size_t m_auto_free_limit;
    size_t m_alloc_limit;

    enum { DEFAULT_GROWING_INCREMENT = 16,
           DEFAULT_GROWING_POWER = 5,
           DEFAULT_INITIAL_SIZE = 32};
};

//============================================================================//
// astring DECLARATION
/*----------------------------------------------------------------------------*/
class astring : public avector<char>
{
private:
    /* copy prevention */
    astring(const astring&): avector<char>() { }
    astring& operator = (const astring&) { return *this; }

public:
    astring(size_t growing_increment = 0,
            size_t growing_power = 0,
            size_t auto_free_limit = 0,
            size_t alloc_limit = size_t(-1))
            : avector<char>(growing_increment, growing_power, auto_free_limit, alloc_limit)
            {}

    operator char*() { return get_buffer(); }
    operator const char*() const { return get_buffer(); }
};

//============================================================================//
// avector IMPLEMENTATION
//============================================================================//
/* SAFE CONSTRUCTION AND DISTRUCTION */
/*----------------------------------------------------------------------------*/
struct  __the_avector_element_ptr
{
};

inline void* operator new(size_t, __the_avector_element_ptr* pt)
    {  return pt;  }

inline void operator delete(void*, __the_avector_element_ptr*)
    { }
/*----------------------------------------------------------------------------*/
// class
template <class A>
inline void __safe_avector_construct_def(A* p, size_t n)
{
    memset(p, 0, n * sizeof(A));
    while(n-- > 0)
        new ((__the_avector_element_ptr*)(p++))A;
}

template <class A>
inline void __safe_avector_destruct(A* pt, size_t n)
    { for ( ; n-- > 0; pt++ ) pt->~A(); }

template <class A, class B>
inline void __safe_avector_construct_cpy(A* p, B r)
{
    new ((__the_avector_element_ptr*)p)A(r);
}
/*----------------------------------------------------------------------------*/
/* ALLOC AND RESIZE IMPLEMENTATION */
/*----------------------------------------------------------------------------*/
template <class T>
inline bool avector<T>::ensure_alloc(size_t new_alloc)
{
    // check limit
    if(new_alloc > m_alloc_limit)
    {
        m_no_memory_flag = true;
        return false;
    }

    // already allocated
    if(new_alloc <= m_alloc)
    {
        m_no_memory_flag = false;
        return true;
    }
    
    // default growing parameters
    size_t growing_increment = m_growing_increment;
    size_t growing_power = m_growing_power;
    if(!growing_increment && !growing_power)
        growing_power = static_cast<size_t>( DEFAULT_GROWING_POWER );

    // count delta
    size_t delta = 0;
    if(m_alloc == 0)
        delta = m_initial_size ? m_initial_size : static_cast<size_t>( DEFAULT_INITIAL_SIZE );
    else if(growing_power)
    {
        delta = m_alloc >> (growing_power - 1);
        if(delta < growing_increment)
            delta = growing_increment;
    }
    else
        delta = m_growing_increment;
    if(delta == 0)
        delta = static_cast<size_t>( DEFAULT_GROWING_INCREMENT );
    
    // count needed_size
    size_t needed_size = m_alloc + delta;
    if(needed_size < new_alloc)
        needed_size = new_alloc;
    else if(needed_size > m_alloc_limit)
        needed_size = m_alloc_limit;

    // allocating memory
    T* buffer = static_cast<T*>(realloc(m_buffer, (needed_size * sizeof(T))));
    if(!buffer)
    {
        m_no_memory_flag = true;
        return false;
    }
    else
    {
        m_no_memory_flag = false;
        m_buffer = buffer;
        m_alloc = needed_size;
        return true;
    }
}
/*----------------------------------------------------------------------------*/
template <class T>
inline void avector<T>::resize(size_t new_size)
{
    if(new_size == m_size)
        return;

    // add new elements
    if(new_size > m_size)
    {
        if(!ensure_alloc(new_size))
            return; // no memory
        __safe_avector_construct_def(m_buffer + m_size, new_size - m_size);
        m_size = new_size;
        return;
    }

    // delete elements
    if(new_size < m_size)
    {
        __safe_avector_destruct(m_buffer + new_size, m_size - new_size);
        m_size = new_size;
        return;
    }
}
/*----------------------------------------------------------------------------*/
template <class T>
inline void avector<T>::push_back(const T& item)
{
    if(!ensure_alloc(m_size + 1))
        return; // no memory
    __safe_avector_construct_cpy(m_buffer + m_size, item);
    m_size++;
}
/*----------------------------------------------------------------------------*/
template <class T>
inline void avector<T>::remove(size_t pos)
{
    if(pos < m_size)
    {
        __safe_avector_destruct(m_buffer + pos, 1);
        memmove(m_buffer+pos, m_buffer+pos+1, (m_size-pos-1)* sizeof(T));
        m_size--;
    }
}
/*----------------------------------------------------------------------------*/
template <class T>
inline T* avector<T>::uninitialized_grow(size_t nGrow)
{
    if(!ensure_alloc(m_size+nGrow))
            return 0; // no memory
    m_size+=nGrow;
    return m_buffer + (m_size - nGrow);
}

template <class T>
inline T* avector<T>::grow()
{
    resize(m_size + 1);
    if(m_no_memory_flag)
        return NULL;
    else
        return m_buffer + (m_size - 1);
}
/*----------------------------------------------------------------------------*/
template <class T>
inline void avector<T>::qsort(int (*comp_func)(const void *, const void *))
{
    ::qsort(m_buffer, m_size, sizeof(T), comp_func);
}
//============================================================================//
#endif /* __a_vector_h__ */
