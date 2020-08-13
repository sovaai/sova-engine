#ifndef _RAII_H_
#define _RAII_H_

#include <_include/cc_compat.h>

template< typename Type, int (*dispose)( Type *value ) >
struct RAII
{
    Type *value;

    RAII() : value( 0 ) { }

    RAII( Type *_value ) : value( _value ) { }

    ~RAII()
    {
        detach();
    }

    Type* release()
    {
        Type*   ret =   value;
        value   =   0;
        return ret;
    }

    void attach( Type *_value )
    {
        detach();
        value   =   _value;
    }

    void move( RAII< Type, dispose > &target )
    {
        target.detach();
        target.value = value;
        value        = 0;
    }
private:
    void detach() {
        if (likely(value != 0)) {
            dispose(value);
        }
    }
};

template <class T>
int defaultDispose(T* value)
{
    delete value;
    return 0;
}

template <class T>
class RAIIPtr {
public:
    typedef RAII< T, defaultDispose<T> >    PtrGuard;
};

#endif /* _RAII_H_ */

