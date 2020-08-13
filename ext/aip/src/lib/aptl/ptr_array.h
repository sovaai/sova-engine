#if !defined( __ptr_array_h__ )
#define __ptr_array_h__
#include "array.h"
#include "RAII.h"

template <class T, class R>
int defaultArrayOfPointersDispose(array<T,R>* arrPtr)
{
    if (NULL == arrPtr) {
        return 0;
    }

    array<T,R>& arr(*arrPtr);
    for (int num = 0; num != arr.GetLen(); ++num) {
        delete arr[num];
        arr[num]    =   NULL;
    }

    return 0;
}

template <class T, class R>
class TRAIIArrayOfPointer {
public:
    typedef RAII< array<T,R>, defaultArrayOfPointersDispose<T, R> >   TGuard;
};


#endif //__ptr_array_h__
