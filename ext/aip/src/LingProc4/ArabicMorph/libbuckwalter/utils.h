#ifndef _LIBBUCKWALTER_UTILS_H_
#define _LIBBUCKWALTER_UTILS_H_
#include <lib/aptl/avector.h>
#include <_include/_inttype.h>
#include <lib/libpts2/PatriciaTree.h>

//TODO:move buffer class for common space, e.g. lib/aptl
template<class T>
struct TBuffer {
public:
    TBuffer(const TBuffer<T>& another)
    : Begin(another.Begin)
    , End(another.End)
    {}

    TBuffer(const T* begin = static_cast<const T*>(NULL), const T* end = static_cast<const T*>(NULL))
    : Begin(begin)
    , End(end)
    {}

    TBuffer(const T* begin, size_t size)
    : Begin(begin)
    , End(begin + size)
    {}

    bool IsInvalid()
    {
        return ((NULL == Begin) || (NULL == End));
    }

    bool IsEmpty()
    {
        return End == Begin;
    }

    size_t Size() const
    {
        return End - Begin;
    }

    TBuffer<T>& operator = (const TBuffer<T>& another)
    {
        Begin   =   another.Begin;
        End     =   another.End;
        return *this;
    }

    bool Cmpbody(TBuffer<T> another)
    {
        if (Size() != another.Size()) {
            return false;
        }
        return memcmp(Begin, another.Begin, Size()) == 0;
    }

    const T*    Begin;
    const T*    End;
};

typedef TBuffer<uint8_t>    TCharBuffer;
typedef TBuffer<char>       TConstString;

template <class T, class U>
TBuffer<T>  MakeBuffer(const U* begin, const U* end)
{
    return TBuffer<T>(reinterpret_cast<const T*>(begin), reinterpret_cast<const T*>(end));
}

template <class T, class U>
TBuffer<T>  MakeBuffer(const U* begin, size_t size)
{
    return TBuffer<T>(reinterpret_cast<const T*>(begin), size);
}

TConstString Strip(TConstString str, char c = ' ');
typedef avector<TConstString>   TSplits;
void Split(TConstString str, char c, TSplits& splits);

enum EMorphologyMode {
    EBuildMorphology = 0,
    EUseMorphology
};

extern const uint16_t   kTreeFlags;

template <class I, class T>
void Join(I begin, I end, typename T::value_type c, T& result)
{
    I iter =   begin;
    result.clear();
    while (iter != end) {
        if (iter != begin) {
            result  +=  c;
        }
        result  +=  **iter;
        iter    +=  1;
    }
}

template<class T>
class TUniqPtr {
public:

    TUniqPtr(T* ptr = NULL)
    : Ptr(ptr)
    {}

    ~TUniqPtr()
    {
        Clear();
    }

    T* Get()
    {
        return Ptr;
    }

    void Set(T* ptr)
    {
        Clear();
        Ptr =   ptr;
    }

    T* operator -> ()
    {
        return Ptr;
    }

    void Clear()
    {
        if (NULL != Ptr) {
            delete Ptr;
            Ptr =   NULL;
        }
    }

private:
    T*  Ptr;
};

#endif /* _LIBBUCKWALTER_UTILS_H_ */
