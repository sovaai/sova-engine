#include <_include/cc_compat.h>
#include <_include/_string.h>

#include <assert.h>

#include <unicode/utf8.h>
#include <unicode/uset.h>
#include <unicode/ucnv.h>
#include <unicode/ustring.h>

#include "UTFCanonizer.h"

UTFCanonizer::UTFCanonizer() :
    created( false ),
    customMappings( 0 ),
    customMappingsCount( 0 ),
    customMappingsOffsets( 0 ),
    customMappingsStarts( 0 ),
    resultBuffer(),
    normalizeSrcBuffer(),
    normalizeDstBuffer(),
    unrm( 0 ),
    excludePattern( 0 ),
    excludeSet( 0 )
{
}

UTFCanonizer::~UTFCanonizer()
{
    if ( likely(customMappingsOffsets != 0) )
    {
        delete [] customMappingsOffsets;
        customMappingsOffsets = 0;
    }
    if ( likely(customMappingsStarts != 0) )
    {
        delete [] customMappingsStarts;
        customMappingsStarts = 0;
    }
    if ( likely(unrm != 0 && excludeSet != 0) )
    {
        unorm2_close( const_cast<UNormalizer2 *>( unrm ) );
        unrm = 0;
    }
    if ( likely(excludeSet != 0) )
    {
        uset_close( excludeSet );
        excludeSet = 0;
    }
    if ( likely(excludePattern != 0) )
    {
        delete [] excludePattern;
        excludePattern = 0;
    }
}

UTFCanonizer::Status UTFCanonizer::create( const UTFCanonizerCustomMap *customMappings,
                                           size_t                       customMappingsCount,
                                           const uint32_t              *excludeChars,
                                           size_t                       excludeCharsCount )
{
    this->customMappings        = customMappings;
    this->customMappingsCount   = customMappingsCount;

    this->customMappingsOffsets = new size_t[ customMappingsCount ];
    if ( unlikely(this->customMappingsOffsets == 0) )
        return STATUS_ERROR_ALLOC;

    this->customMappingsStarts = new size_t[ customMappingsCount ];
    if ( unlikely(this->customMappingsStarts == 0) )
        return STATUS_ERROR_ALLOC;
    
    UErrorCode status = U_ZERO_ERROR;

    this->unrm = unorm2_getNFKCInstance( &status );
    if ( unlikely(!U_SUCCESS(status)) )
        return STATUS_ERROR_ICU;
    
    if ( excludeCharsCount > 0 )
    {
        excludePattern = new UChar32[ excludeCharsCount + 4 ];
        excludePattern[ 0 ] = static_cast<UChar32>( '[' );
        excludePattern[ 1 ] = static_cast<UChar32>( '^' );
        for ( size_t i = 0; i < excludeCharsCount; i++ )
            excludePattern[ i + 2 ] = excludeChars[ i ];
        excludePattern[ excludeCharsCount + 2 ] = static_cast<UChar32>( ']' );
        excludePattern[ excludeCharsCount + 3 ] = static_cast<UChar32>( '\0' );
        
        normalizeSrcBuffer.resize( excludeCharsCount + 4 );
        if ( unlikely(normalizeSrcBuffer.no_memory()) )
            return STATUS_ERROR_ALLOC;

        int32_t actualPatternSize = 0;
        u_strFromUTF32( normalizeSrcBuffer.get_buffer(),
                        normalizeSrcBuffer.size(),
                        &actualPatternSize,
                        excludePattern,
                        -1,
                        &status );
        if ( unlikely(!U_SUCCESS(status)) )
            return STATUS_ERROR_ICU;
    
        excludeSet = uset_openPattern( normalizeSrcBuffer.get_buffer(),
                                       actualPatternSize,
                                       &status );
        if ( unlikely(!U_SUCCESS(status)) )
            return STATUS_ERROR_ICU;

        uset_freeze( excludeSet );
    
        this->unrm = unorm2_openFiltered( this->unrm, excludeSet, &status );
        if ( unlikely(!U_SUCCESS(status)) )
            return STATUS_ERROR_ICU;
    }

    created = true;
    return STATUS_OK;
}

UTFCanonizer::Status UTFCanonizer::canonize( const char *text, size_t textSize )
{
    if ( unlikely(text == 0) )
        return STATUS_ERROR_INVALID_ARG;

    if ( textSize == static_cast<size_t>(-1) )
        textSize = strlen( text );

    // remove utf-8 signature if any
    UErrorCode status          = U_ZERO_ERROR;
    int32_t    signatureLength = 0;

    ucnv_detectUnicodeSignature( text, textSize, &signatureLength, &status );
    if ( unlikely(!U_SUCCESS(status)) )
        return STATUS_ERROR_ICU;

    text     += signatureLength;
    textSize -= signatureLength;
    
    // prepare source buffer
    // assume the worst case: each utf-8 octet will become two utf-16 elements
    // ODA: which case is it?
    normalizeSrcBuffer.resize( textSize * 2 + 1 ); 
    if ( unlikely(normalizeSrcBuffer.no_memory()) )
        return STATUS_ERROR_ALLOC;
    
    int32_t actualSrcLength = 0;
    
    u_strFromUTF8WithSub( normalizeSrcBuffer.get_buffer(),
                          static_cast<int32_t>( normalizeSrcBuffer.size() ),
                          &actualSrcLength, 
                          text,
                          textSize,
                          static_cast<UChar32>( '?' ),
                          NULL,
                          &status );
    if ( unlikely(!U_SUCCESS(status)) )
        return STATUS_ERROR_ICU;

    // prepare destination buffer
    // assume worst case: output text will become twice larger than source text
    // ODA: this is not the worst case: due to NFKC tables one codepoint can be mapped in upto 18 codepoints (!) see \uFDFA
    normalizeDstBuffer.resize( actualSrcLength * 2 + 1 );
    if ( unlikely(normalizeDstBuffer.no_memory()) )
        return STATUS_ERROR_ALLOC;

    // perform ICU NFKC normalization
    int32_t actualDstLength =
        unorm2_normalize( unrm,
                          normalizeSrcBuffer.get_buffer(),
                          actualSrcLength,
                          normalizeDstBuffer.get_buffer(),
                          normalizeDstBuffer.size(),
                          &status );
    if ( unlikely(!U_SUCCESS(status)) )
        return STATUS_ERROR_ICU;

    // prepare result buffer
    // assume worst case: each codepoint will become four octets in utf-8
    resultBuffer.resize( actualDstLength * 4 + 1 );
    if ( unlikely(resultBuffer.no_memory()) )
        return STATUS_ERROR_ALLOC;
    
    // perform custom normalization
    memset( customMappingsOffsets, 0, customMappingsCount * sizeof( size_t ) );
    memset( customMappingsStarts,  0, customMappingsCount * sizeof( size_t ) );

    char          *result = resultBuffer.get_buffer();
    size_t         resultOffset = 0;
    UCharIterator  it;
    uiter_setString( &it, normalizeDstBuffer.get_buffer(), actualDstLength );
    for ( UChar32 ch = uiter_next32( &it ); ch != U_SENTINEL; ch = uiter_next32( &it ) )
    {
        size_t previousResultOffset = resultOffset;
        U8_APPEND_UNSAFE( result, resultOffset, ch );

        for ( size_t i = 0; i < customMappingsCount; i++ )
        {
            const UTFCanonizerCustomMap &map = customMappings[ i ];

            UChar32 chMap;
            bool    doBreak = false;
            for ( ;; )
            {
                if ( customMappingsOffsets[ i ] == 0 )
                {
                    customMappingsStarts[ i ] = previousResultOffset;
                    doBreak = true;
                }

                U8_NEXT_UNSAFE( map.source, customMappingsOffsets[ i ], chMap );
                if ( chMap != ch )
                    customMappingsOffsets[ i ] = 0;
                else
                    break;

                if ( doBreak )
                    break;
            }

            if ( chMap != static_cast<UChar32>( '\0' ) )
            {
                size_t offset = customMappingsOffsets[ i ];
                U8_NEXT_UNSAFE( map.source, offset, chMap );
                if ( chMap == static_cast<UChar32>( '\0' ) )
                {
                    // full sequence match, perform replacement
                    resultOffset  = customMappingsStarts[ i ];
                    size_t offset = 0;
                    for ( ;; )
                    {
                        U8_NEXT_UNSAFE( map.target, offset, chMap );
                        if ( chMap == static_cast<UChar32>( '\0' ) )
                            break;
                        U8_APPEND_UNSAFE( result, resultOffset, chMap );
                    }

                    // reset matchers
                    memset( customMappingsOffsets, 0, customMappingsCount * sizeof( size_t ) );
                    memset( customMappingsStarts,  0, customMappingsCount * sizeof( size_t ) );
                    break;
                }
            }
        }
    }

    // terminate and fix result buffer
    resultBuffer[ resultOffset ] = '\0';
    resultBuffer.resize( resultOffset );

    return STATUS_OK;
}

//here comes data for canonization with mapping
enum CharType{
    space, word, symbol, combine, error
};

static inline CharType GetCharType(UChar32 cur_char, const UNormalizer2 *unrm, const UCharSet *ucs){
    //defining current character type
    //if(cur_char<0) //current character is wrong, map it to '?', and, also this ends previous interval
    //    return error;
    if(ucs->IsSpace(cur_char))
        return space;
    if(ucs->IsHyphen(cur_char) || ucs->IsPunct(cur_char))
        return symbol;
    if(ucs->IsWord(cur_char))
        return word;
    if(unorm2_getCombiningClass(unrm, cur_char)!=0)
        return combine;
    return symbol;
}

static inline UTFCanonizer::Status BorderSync(const UNormalizer2 *unrm, const UCharSet *ucs, UTFCanonizer::CoordMapping& mapping, const char *orig_data, int32_t& orig_offset, const UChar *data, int32_t data_size, int32_t& processed, avector<UChar>& normalizeDstBuffer, avector<char>& result_buffer, int32_t& result_string_size){
    //finding possible border, iterating over UChar *data
    int32_t i=0, i_prev=0;
    CharType cur_char_type=space, prev_char_type=space;
    int char_count=0, combine_char_count=0;

    while(i<data_size){
        //update variables
        int32_t i_new=i;
        prev_char_type=cur_char_type;

        //go to next char
        UChar32 c;
        U16_NEXT_UNSAFE(data, i_new, c);

        //update char type
        cur_char_type=GetCharType(c, unrm, ucs);
        if(i!=0){
            //in this section error char is impossible, since it is filtered way before
            if(cur_char_type==symbol) // symbol always has the border before
                break;
            if(cur_char_type==combine){// yeah, combine... so, it's not so simple
                //A + dia1 + ... + diaN must be a separate group, because combining may change charater type
                //so if we have ABCD + dia+... border must be set between C and D
                //TODO: optimize this
                if(!combine_char_count && i_prev!=0){
                    i=i_prev;
                    --char_count;
                    //but one more border line must be berfore previous character
                    //so A + dia1 + ... + diaN is a separate group
                    break;
                }
                ++combine_char_count;
            }
            else{
                combine_char_count=0;
                if(cur_char_type!=prev_char_type) //changing char_type is border (letter/space)
                    break;
            }
        }
        i_prev=i;
        i=i_new;
        ++char_count;
    }

    //canonization
    //at this point: i - end of chunk, char_count - number of characters to count
    //normalize src buffer must be filled, it appends to resultBuffer and mapping
    processed=i; //number of UChar processed
    UErrorCode status=U_ZERO_ERROR;
    if(normalizeDstBuffer.size()<(unsigned)i*2U){
        normalizeDstBuffer.resize((unsigned)i*2U);
        if ( unlikely(normalizeDstBuffer.no_memory()) )
            return UTFCanonizer::STATUS_ERROR_ALLOC;
    }
    int32_t actualDstLength = unorm2_normalize(unrm, data, i, normalizeDstBuffer.get_buffer(), normalizeDstBuffer.size(), &status); //canonization
    //check if we have allocated not enough memory to store nomalized result
    if(status==U_BUFFER_OVERFLOW_ERROR){
        status=U_ZERO_ERROR;
        normalizeDstBuffer.resize(actualDstLength);
        if ( unlikely(normalizeDstBuffer.no_memory()) )
            return UTFCanonizer::STATUS_ERROR_ALLOC;
        actualDstLength = unorm2_normalize(unrm, data, i, normalizeDstBuffer.get_buffer(), normalizeDstBuffer.size(), &status); //canonization
    }
    if(unlikely(U_FAILURE(status))){
        return UTFCanonizer::STATUS_ERROR_ICU;
    }
    //copying data to output buffer
    int32_t dest_length;
    int32_t max_dest_length=actualDstLength*3;
    if((unsigned)max_dest_length>result_buffer.size()-result_string_size){ //realloc
        result_buffer.resize(result_string_size+max_dest_length);
        if ( unlikely(result_buffer.no_memory()) )
            return UTFCanonizer::STATUS_ERROR_ALLOC;
    }
    u_strToUTF8(result_buffer.get_buffer()+result_string_size, max_dest_length, &dest_length, normalizeDstBuffer.get_buffer(), actualDstLength, &status);
    if(U_FAILURE(status))
        return UTFCanonizer::STATUS_ERROR_ICU;

    //syncing to orig string
    UChar32 c;
    const char *d=orig_data+orig_offset;
    int32_t l=0;
    for(int i=0; i<char_count; ++i)
        U8_NEXT_UNSAFE(d, l, c);
    mapping.CreateMappingInterval(l, dest_length, false);

    //update offsets
    orig_offset+=l;
    result_string_size+=dest_length;
    return UTFCanonizer::STATUS_OK;
}

//Proper part canonization: the part of text contains only valid UTF8 codepoints
static inline UTFCanonizer::Status CanonizePart(const UNormalizer2 *unrm, const UCharSet *ucs, UTFCanonizer::CoordMapping& mapping,
                                         const char *orig_data, int32_t orig_offset, const UChar *data, int32_t data_size,
                                         avector<UChar>& normalizeDstBuffer, avector<char>& result_buffer, int32_t& result_string_size){
    //orig_data - is original, non-canonized UTF8 string, need to build mapping
    //orig_offset - is offset to corresponding byte in original string
    //normalizeDstBuffer - is reusable buffer in which unorm2_normalize will return canonized part of stirng
    UErrorCode status=U_ZERO_ERROR;
    UTFCanonizer::Status rc;

    //main loop
    while(data_size){
        //get normalization border
        int32_t norm_end=unorm2_spanQuickCheckYes(unrm, data, data_size, &status);

        //append normalized part (if any)
        if(norm_end>0){
            //append part to result buffer
            //require proper length: worst case is 2 bytes -> 3 bytes, data_size is size in uint16_t, dest_length is size in uint8_t
            int32_t max_dest_length=norm_end*3;
            if((unsigned)max_dest_length>result_buffer.size()-result_string_size){ //realloc
                result_buffer.resize(result_string_size+max_dest_length);
                if ( unlikely(result_buffer.no_memory()) )
                    return UTFCanonizer::STATUS_ERROR_ALLOC;
            }
            //convert canonized part to UTF8
            int32_t dest_length;
            u_strToUTF8(result_buffer.get_buffer()+result_string_size, max_dest_length, &dest_length, data, norm_end, &status);
            if(U_FAILURE(status))
                return UTFCanonizer::STATUS_ERROR_ICU;

            //append new canonization point
            mapping.CreateMappingInterval(dest_length, dest_length, true); //append equal interval

            //update variables
            data_size-=norm_end;
            data+=norm_end;
            result_string_size+=dest_length;
            orig_offset+=dest_length;
        }

        //processing unnormalized interval: from current posinion to next possible word border
        if(!data_size) //nothing to handle: we processed all string to the end
            break;
        int32_t processed=0;
        rc=BorderSync(unrm, ucs, mapping, orig_data, orig_offset, data, data_size, processed, normalizeDstBuffer, result_buffer, result_string_size);
        if(rc!=UTFCanonizer::STATUS_OK)
            return rc;
        data+=processed;
        data_size-=processed;
    }
    return UTFCanonizer::STATUS_OK;
}

UTFCanonizer::Status UTFCanonizer::CanonizeWithMapping(const UCharSet *ucs, const char *text, size_t textSize, CoordMapping& mapping){
    UErrorCode status=U_ZERO_ERROR;
    Status rc=UTFCanonizer::STATUS_OK;
    assert(customMappingsCount==0); //will not work with custom mappings

    if ( unlikely(ucs == 0) || unlikely(text == 0) )
        return UTFCanonizer::STATUS_ERROR_INVALID_ARG;

    if ( textSize == static_cast<size_t>(-1) )
        textSize = strlen( text );
    int32_t length = (int32_t) textSize; //because icu uses int32_t for representation string length

    //preparing coordinate mapping
    if(mapping.empty())
        mapping.CreateMappingPoint(0,0,false);

    //prepare result buffer
    resultBuffer.resize(textSize+1);
    if ( unlikely(resultBuffer.no_memory()) )
        return UTFCanonizer::STATUS_ERROR_ALLOC;
    //prepare normalize buffer
    normalizeSrcBuffer.resize(textSize+1);
    if ( unlikely(normalizeSrcBuffer.no_memory()) )
        return UTFCanonizer::STATUS_ERROR_ALLOC;

    //loop variables init
    UChar *normSrcBuf=normalizeSrcBuffer.get_buffer(); //pointer to UTF16 buffer
    int32_t normSrcBufIdx=0; //current index in UTF16 buffer
    int32_t result_size=0; //number of charaters written to result buffer
    int32_t i=0; //current index in string
    int32_t orig_part_start=0; //start of data transferred to CanonizePart

    //skipping BOM
    int32_t signatureLength=0;
    ucnv_detectUnicodeSignature( text, textSize, &signatureLength, &status );
    if ( unlikely(!U_SUCCESS(status)) )
        return STATUS_ERROR_ICU;
    if(signatureLength){
        i += signatureLength;
        orig_part_start+=signatureLength;
        mapping.CreateMappingInterval(signatureLength, 0, false);
    }

    //main loop
    while(i<length){
        int32_t i_new=i;
        UChar32 cur_char;
        U8_NEXT(text, i_new, length, cur_char); // move to next char
        //for each proper part
        if(cur_char<0){ //current character is wrong,
            //append all previous text (if any)
            if(normSrcBufIdx){
                rc=CanonizePart(unrm, ucs, mapping, text, orig_part_start, normSrcBuf, normSrcBufIdx, normalizeDstBuffer, resultBuffer, result_size);
                if(unlikely(rc!=UTFCanonizer::STATUS_OK))
                    return rc;
                normSrcBufIdx=0;
            }

            //append current character: map it to '?' and add interval
            if(resultBuffer.size()<(unsigned)result_size+1){
                resultBuffer.resize(result_size+1);
                if ( unlikely(resultBuffer.no_memory()) )
                    return UTFCanonizer::STATUS_ERROR_ALLOC;
            }
            resultBuffer.get_buffer()[result_size]='?';
            ++result_size;
            mapping.CreateMappingInterval(i_new-i, 1, false);
            //update variables
            orig_part_start=i_new;
        }
        else{
            //append current character
            U16_APPEND_UNSAFE(normSrcBuf, normSrcBufIdx, cur_char);
        }
        i=i_new;
    }
    //append last proper interval (if any)
    if(normSrcBufIdx){
        rc=CanonizePart(unrm, ucs, mapping, text, orig_part_start, normSrcBuf, normSrcBufIdx, normalizeDstBuffer, resultBuffer, result_size);
        if(unlikely(rc!=UTFCanonizer::STATUS_OK))
            return rc;
        normSrcBufIdx=0;
    }

    //set trailing \0
    resultBuffer.resize(result_size+1);
    if ( unlikely(resultBuffer.no_memory()) )
        return UTFCanonizer::STATUS_ERROR_ALLOC;
    resultBuffer.get_buffer()[result_size]='\0';
    resultBuffer.resize(result_size);

    return UTFCanonizer::STATUS_OK;
}
