#include "compatibility_storage.h"
#include "fstorage_utils.h"

template <class T, size_t size>
class TAlmostStaticBuffer {
public:

    TAlmostStaticBuffer()
    : CurrentPtr(StaticBuffer)
    , CurrentSize(size)
    {}

    size_t  Size()const
    {
        return CurrentSize;
    }

    T*  GetBuffer()
    {
        return CurrentPtr;
    }

    void Resize(size_t required)
    {
        int diff    =   CurrentSize - required;
        CurrentSize =   required;

        if (diff >= 0) {
            return;
        }

        if (size >= required) {
            CurrentPtr  =   StaticBuffer;
        }
        else {
            DynamicBuffer.resize(required);
            CurrentPtr  =   DynamicBuffer.get_buffer();
        };
    }

private:
    T           StaticBuffer[size];
    avector<T>  DynamicBuffer;
    T*          CurrentPtr;
    size_t      CurrentSize;
};

typedef TAlmostStaticBuffer<uint8_t, 30>    TGlueBuffer;

static void glue(TCharBuffer first, TCharBuffer second, TGlueBuffer& result)
{
    static const uint8_t    kConnectionSymbol   =   '+';
    result.Resize(first.Size() + 1 + second.Size());
    memcpy(result.GetBuffer(), first.Begin, first.Size());
    result.GetBuffer()[first.Size()]    =   kConnectionSymbol;
    memcpy(result.GetBuffer() + first.Size() + 1, second.Begin, second.Size());
}

bool TCompatibilityStorage::Init(EMorphologyMode mode)
{
    PatriciaTreeCore::ResultStatus  res =   PatriciaTreeCore::RESULT_STATUS_SUCCESS;
    if (EBuildMorphology == mode) {
        res =   Storage.create(kTreeFlags);
    }
    return res == PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

static const uint8_t    kValue  =   0;
bool TCompatibilityStorage::AddComaptibleRule(TCharBuffer first, TCharBuffer second)
{
    TGlueBuffer key;
    glue(first, second, key);
    const uint8_t*      valuePtr    =   &kValue;
    PatriciaTreeCore::ResultStatus  res =   Storage.append( key.GetBuffer()
                                                          , key.Size()
                                                          , &valuePtr
                                                          , sizeof(kValue) );
    return res == PatriciaTreeCore::RESULT_STATUS_SUCCESS;
}

bool TCompatibilityStorage::IsCompatible(TCharBuffer first, TCharBuffer second) const
{
    TGlueBuffer key;
    glue(first, second, key);
    const uint8_t*                  value   =   NULL;
    size_t                          size    =   0;
    PatriciaTreeCore::ResultStatus  res     =   Storage.lookupExact( key.GetBuffer()
                                                                   , key.Size()
                                                                   , &value
                                                                   , &size );
    return ( (NULL != value)
          && (kValue == *value)
          && (0 != size)
          && (PatriciaTreeCore::RESULT_STATUS_SUCCESS == res));
}


fstorage_section_id TCompatibilityStorage::Store(fstorage* storage, fstorage_section_id startSection)
{
    return StorePatriciaTree(Storage, storage, startSection);
}

fstorage_section_id TCompatibilityStorage::Restore(fstorage* storage, fstorage_section_id startSection)
{
    return RestorePatriciaTree(Storage, kTreeFlags, storage, startSection);
}
