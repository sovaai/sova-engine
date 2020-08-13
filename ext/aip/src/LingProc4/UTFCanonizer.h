#ifndef _UTFCANONIZER_H_
#define _UTFCANONIZER_H_

#include <_include/_inttype.h>
#include <lib/aptl/avector.h>

#include "UCharSet/UCharSet.h"

#include <unicode/unorm2.h>

struct UTFCanonizerCustomMap
{
    const char *source;
    const char *target;
};

const UTFCanonizerCustomMap canonizer_custom_mappings[] =
{
#ifdef _MSC_VER
    { "\0", "\0" /* dummy */ }  // for MSVC since it is not accept zero-length arrays
#endif
//    { "No", "\xE2\x84\x96" /* NUMERO_SIGN */ }
};

const uint32_t canonizer_custom_exclude_chars[] =
{
    0x00002116 /* NUMERO_SIGN */
};

class UTFCanonizer
{
public:
    enum Status
    {
        STATUS_OK,
        STATUS_ERROR_ALLOC,
        STATUS_ERROR_INVALID_ARG,
        STATUS_ERROR_ICU
    };
    
public:
    UTFCanonizer();
    virtual ~UTFCanonizer();
    
    Status create()
    {
        return create( canonizer_custom_mappings,
                       sizeof( canonizer_custom_mappings ) /
                       sizeof( struct UTFCanonizerCustomMap ),
                       canonizer_custom_exclude_chars,
                       sizeof( canonizer_custom_exclude_chars ) / sizeof( uint32_t ) );
    }
    Status create( const UTFCanonizerCustomMap *customMappings,
                   size_t                       customMappingsCount,
                   const uint32_t              *excludeChars,
                   size_t                       excludeCharsCount );

    Status canonize( const char *text, size_t textSize = static_cast<size_t>(-1) );

    enum CharType{
        space, word, symbol, combine, error
    };
    struct CoordMappingPoint{
        int32_t orig_point;
        int32_t canonized_point;
        bool is_equal; //if true - the source interval from previous point to current is canonized
        CoordMappingPoint(int32_t orig, int32_t canonized, bool equal):
            orig_point(orig), canonized_point(canonized), is_equal(equal){}
    };
    class CoordMapping : public avector<CoordMappingPoint>{
    public:
        void CreateMappingPoint(int32_t orig, int32_t canonized, bool equal){
            push_back(CoordMappingPoint(orig, canonized, equal));
        }
        void CreateMappingInterval(int32_t orig_offset, int32_t canonized_offset, bool equal){
            const CoordMappingPoint& l=LastMapping();
            push_back(CoordMappingPoint(l.orig_point+orig_offset, l.canonized_point+canonized_offset, equal));
        }
        const CoordMappingPoint& LastMapping() const{
            return back();
        }
    };
    Status CanonizeWithMapping(const UCharSet *ucs, const char *text, size_t textSize, CoordMapping& mapping); //mapping is appended to existing mapping

    uint8_t GetCombiningClass(UChar32 c){
        return unorm2_getCombiningClass(unrm, c);
    }

    const char *getResultText() const { return resultBuffer.get_buffer(); }
    size_t      getResultTextSize() const { return resultBuffer.size(); }
    bool        isCreated() const { return created; }
    
protected:
    Status NormalizeChunk(int32_t actualSrcLength, CoordMapping& mapping, int32_t orig_idx, int32_t orig_offset, int32_t canonized_offset); //must be called only from CanonizeWithMapping
    bool                         created;
    const UTFCanonizerCustomMap *customMappings;
    size_t                       customMappingsCount;
    size_t                      *customMappingsOffsets;
    size_t                      *customMappingsStarts;
    avector<char>                resultBuffer;
    avector<UChar>               normalizeSrcBuffer;
    avector<UChar>               normalizeDstBuffer;
    const UNormalizer2          *unrm;
    UChar32                     *excludePattern;
    USet                        *excludeSet;
};

#endif /* _UTFCANONIZER_H_ */

