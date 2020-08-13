#include <stdlib.h>
#include <string.h>

#include <_include/Cp2Uni.h>
#include <_include/CodePages.h>

#define MASK1 ~0x0000007F
#define MASK2 ~0x000007FF
#define MASK3 ~0x0000FFFF
#define MASK4 ~0x001FFFFF

#define SEQ1_CHAR_MASK  0x7F // 01111111

#define SEQ2_START_MASK 0xE0 // 11100000
#define SEQ2_START_BITS 0xC0 // 110xxxxx
#define SEQ2_CHAR_MASK  0x1F // 00011111 - 5 bits

#define SEQ3_START_MASK 0xF0 // 11110000
#define SEQ3_START_BITS 0xE0 // 1110xxxx
#define SEQ3_CHAR_MASK  0x0F // 00001111 - 4 bits

#define SEQ4_START_MASK 0xF8 // 11111000
#define SEQ4_START_BITS 0xF0 // 11110xxx
#define SEQ4_CHAR_MASK  0x07 // 00000111 - 3 bits

#define SEQN_START_MASK 0xC0 // 11000000
#define SEQN_START_BITS 0x80 // 10xxxxxx
#define SEQN_CHAR_MASK  0x3F // 00111111 - 6 bits

//forward declaration for CP_HIER_UTF case
void CopyUTF(const unsigned char *pSrc, unsigned char *pDst, int nDstSize, int nLen, int *nBytesCopied, int *nCharsCopied, bool ignore_err=false);

int UCS2UTF8(unsigned wchar, unsigned char * pDst, int ccDst)
{
        // create utf-8 sequence
        if(!(wchar & MASK1))
        {
            if(ccDst<1)
                return 0;
            pDst[0] = (uint8_t)(wchar & SEQ1_CHAR_MASK);
            return 1;
        }

        if(!(wchar & MASK2))
        {
            if(ccDst<2)
                return 0;
            pDst[1] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            pDst[0] = (uint8_t)(SEQ2_START_BITS | (wchar & SEQ2_CHAR_MASK ));
            return 2;
        }

        if(!(wchar & MASK3))
        {
            if(ccDst<3)
                return 0;
            pDst[2] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            pDst[1] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            pDst[0] = (uint8_t)(SEQ3_START_BITS | (wchar & SEQ3_CHAR_MASK));
            return 3;
        }

        if(!(wchar & MASK4))
        {
            if(ccDst<4)
                return 0;
            pDst[3] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            pDst[2] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            pDst[1] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            pDst[0] = (uint8_t)(SEQ4_START_BITS | (wchar & SEQ4_CHAR_MASK));
            return 4;
        }

        // invalid
        if(ccDst<1)
            return 0;
        pDst[0] = '?';
        return 1;
}

int Convert2UCS2(const unsigned char * pSrc, int ccSrc, int nSrcCodePage, unsigned char * pDst, int ccDst)
{
    if(!pSrc || !ccSrc)
        return 0;
    if(!pDst)
        ccDst = 0;

    const uint16_t* recode_table = NULL;
    switch (nSrcCodePage)
    {
    case CP_CYRILLIC: recode_table = tabWin2Usc; break;
    case CP_KOI8R: recode_table = tabKoi2Usc; break;
    case CP_CYRILLIC_MAC: recode_table = tabMac2Usc; break;
    case CP_ISO8859_5: recode_table = tabIso2Usc; break;
    case CP_OEM_RUSSIAN: recode_table = tabDos2Usc; break;
    case CP_VIETNAMESE: break;
    default: recode_table = tabLat12Usc; break;
    }

    size_t dst_size = 0;
    int chars_converted = 0;
    unsigned char* dst_pt = (unsigned char*)pDst;
    const unsigned char* src_pt = (const unsigned char*)pSrc;

    while(ccSrc /*&& *src_pt*/)
    {
        int srcBytes = 1;
        
        // get unicode char
        uint16_t wchar;
        
        wchar = recode_table[*src_pt];
        
        // create utf-16 sequence
        unsigned int oct_num = 2;
        // copy to pDst
        if((unsigned)ccDst > dst_size + oct_num)
        {
            memcpy(dst_pt, &wchar, oct_num);
            dst_pt += oct_num;
            chars_converted++;
        }
        dst_size += oct_num;

        // go to next char
        src_pt += srcBytes; ccSrc -= srcBytes;
        
    } // while

    // terminating zero makes no sense here

    return chars_converted;
}

inline int DefineUTFSize(unsigned char c){
    if(!(c&0x80))//0xxxxxxx
        return 1;
    if((c&0xE0)==0xC0)//110xxxxx
        return 2;
    if((c&0xF0)==0xE0)//1110xxxx
        return 3;
    if((c&0xF8)==0xF0)//11110xxx
        return 4;
    return 0;//consider error
}

int Convert2UTF_len(const unsigned char * pSrc, int ccSrc, int nSrcCodePage, unsigned char * pDst, int ccDst, size_t * pWrt){
    return Convert2UTF_lengths(pSrc, ccSrc, nSrcCodePage, pDst, ccDst, pWrt, NULL);
}

int Convert2UTF_lengths(const unsigned char * pSrc, int ccSrc, int nSrcCodePage, unsigned char * pDst, int ccDst, size_t * pWrt, unsigned char * pLenPairs){
//size_t CCharSet::ConvertToUTF(enum CodePages codepage, char* dest, size_t dest_max_size,
//                          const char* src, size_t src_max_size /* = size_t(-1)*/) const

    if(pWrt)
        *pWrt = 0;
    if(!pSrc || !ccSrc)
        return 0;
    if(!pDst)
        ccDst = 0;

    if(nSrcCodePage==CP_HIER_UTF){    //not transform, just copy and count UTF characters
        //int bc,cc;
        //CopyUTF(pSrc,pDst,ccDst,ccSrc,&bc,&cc,true);
        //copy characters
        int nLen=ccSrc<ccDst?ccSrc:ccDst;
        memcpy(pDst,pSrc, nLen);
        //count UTF characters
        int i=0,cc=0;
        while(i<nLen){
            int s=DefineUTFSize(pDst[i]);
            if(s==0) s=1;
            //zero terminating conditions
            if( (ccDst-i)<s ){
                pDst[i]='\0';
                break;
            }
            if(pLenPairs)
            {
                *(pLenPairs++) = s;
                *(pLenPairs++) = s;
            }
            i+=s;
            ++cc;
        }
        if(pWrt)
            *pWrt = nLen;
        if(i<ccDst)
        {
            pDst[i]='\0';
            if(pWrt)
                *pWrt += 1;
        }
        return cc;
    }

    const uint16_t* recode_table = NULL;
    switch (nSrcCodePage)
    {
    case CP_CYRILLIC: recode_table = tabWin2Usc; break;
    case CP_KOI8R: recode_table = tabKoi2Usc; break;
    case CP_CYRILLIC_MAC: recode_table = tabMac2Usc; break;
    case CP_ISO8859_5: recode_table = tabIso2Usc; break;
    case CP_OEM_RUSSIAN: recode_table = tabDos2Usc; break;
    case CP_EUC_JP: break;
    case CP_VIETNAMESE: break;
    case CP_ARABIC: recode_table = tabArab2Ucs; break;
    default: recode_table = tabLat12Usc; break;
    }

    size_t dst_size = 0;
    int chars_converted = 0;
    unsigned char* dst_pt = (unsigned char*)pDst;
    const unsigned char* src_pt = (const unsigned char*)pSrc;

    while(ccSrc /*&& *src_pt*/)
    {
        int srcBytes = 1;
        
        // get unicode char
        uint32_t wchar;
        switch ( nSrcCodePage )
        {
        case CP_VIETNAMESE:
            wchar = cp2UcCollapsingSeqViet( src_pt, ccSrc, &srcBytes );
            break;
        case CP_EUC_JP:
            wchar = EucJpSeq2Unicode( src_pt, ccSrc, &srcBytes );
            break;
        default: wchar = recode_table[*src_pt]; break;
        }
        

        // create utf-8 sequence
        uint8_t out_seq[4];
        memset(&out_seq, 0, sizeof(out_seq));
        unsigned int oct_num;
        if(!(wchar & MASK1))
        {
            oct_num = 1;
            out_seq[0] = (uint8_t)(wchar & SEQ1_CHAR_MASK);
        } 
        else if(!(wchar & MASK2))
        {
            oct_num = 2;
            out_seq[1] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            out_seq[0] = (uint8_t)(SEQ2_START_BITS | (wchar & SEQ2_CHAR_MASK ));
        }
        else if(!(wchar & MASK3))
        {
            oct_num = 3;
            out_seq[2] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            out_seq[1] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            out_seq[0] = (uint8_t)(SEQ3_START_BITS | (wchar & SEQ3_CHAR_MASK));
        }
        else if(!(wchar & MASK4))
        {
            oct_num = 4;
            out_seq[3] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            out_seq[2] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            out_seq[1] = (uint8_t)(SEQN_START_BITS | (wchar & SEQN_CHAR_MASK));
            wchar >>= 6;
            out_seq[0] = (uint8_t)(SEQ4_START_BITS | (wchar & SEQ4_CHAR_MASK));
        }
        else
        {
            oct_num = 1; // invalid
            out_seq[0] = '?';
        }

        // copy to pDst
        if((unsigned)ccDst > dst_size + oct_num)
        {
            memcpy(dst_pt, out_seq, oct_num);
            dst_pt += oct_num;
            chars_converted++;
            if(pLenPairs)
            {
                *(pLenPairs++) = srcBytes;
                *(pLenPairs++) = oct_num;
            }
        }
        dst_size += oct_num;

        // go to next char
        src_pt += srcBytes; ccSrc -= srcBytes;
    }

    // terminating zero
    if( dst_size < (size_t)ccDst )
    {
        *dst_pt = '\0';
        ++dst_size;
    }
    if(pWrt)
        *pWrt=dst_size;

    //return dst_size;
    return chars_converted;
}

int Convert2UTF(const unsigned char * pSrc, int ccSrc, int nSrcCodePage, unsigned char * pDst, int ccDst){
    return Convert2UTF_lengths(pSrc, ccSrc, nSrcCodePage, pDst, ccDst, NULL, NULL);
}

//Convert2UTF_Ex internal buffer size - 256 KB
#define CONV2UTFEX_BUFSIZE 262144

void CopyUTF(const unsigned char *pSrc, unsigned char *pDst, int nDstSize, int nLen, int *nBytesCopied, int *nCharsCopied, bool ignore_err /*=false*/){
    *nBytesCopied=0;
    *nCharsCopied=0;
    int size;
    while(nLen&&nDstSize){
        //define the next char size
        size=DefineUTFSize(*pSrc);
        if(!size){//consider error
            if(ignore_err)  //ignore mode: copy chars as is
                size=1;
            else            //if ignore_err is false, we stop on error
                break;
        }
        //conditions for writing \0 because of lack of memory
        if(nDstSize-size<0){
            *pDst='\0';
            break;
        }
        if(nDstSize-size==0&&nLen>1){
            *pDst='\0';
            break;
        }
        //update indicies and info
        *nBytesCopied+=size;
        nDstSize-=size;
        nLen--;
        (*nCharsCopied)++;
        for(;size>0;size--){
            *pDst=*pSrc;
            *pDst++; *pSrc++;
        }
    }
}
//always add '\0' to the end
int Convert2UTF_Ex(unsigned char * pSrc, int nSize, int nLen, int nSrcCodePage){//q:assume that input text is in single-byte encoding?
    if(!pSrc||!nSize||!nLen)
        return 0;
    if(nLen<0)//q: ifnLen==-1,and sitring is not zero-terminated and pSrc filled?
        nLen=int(strlen((const char*)pSrc));
    //internal buffer
    unsigned char buf[CONV2UTFEX_BUFSIZE];
    int len;
    int bytes_copied, chars_copied;
    len=Convert2UTF(pSrc, nLen, nSrcCodePage, buf, CONV2UTFEX_BUFSIZE);
    if(len==nLen){//or try to change
        CopyUTF(buf,pSrc, nSize, nLen, &bytes_copied, &chars_copied);
        return len;
    }
    else{
        unsigned char *buf2=(unsigned char *)malloc((nLen-len)*4);
        if(!buf2)
            return 0;
        //convert the rest of the chars
        Convert2UTF(pSrc, nSize, nSrcCodePage, buf2, (nLen-len)*4);
        //copy first part
        CopyUTF(buf, pSrc, nSize, len, &bytes_copied, &chars_copied);
        //copy second part
        CopyUTF(buf2, &pSrc[bytes_copied], nSize-bytes_copied, nLen-len, &bytes_copied, &chars_copied);
        free(buf2);
        return len+chars_copied;
    }
}
