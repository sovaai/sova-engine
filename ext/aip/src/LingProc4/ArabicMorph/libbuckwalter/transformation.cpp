#include "transformation.h"
#include <LingProc4/IndirectTable.h>
#include <unicode/utf8.h>

static IndirectTable buildLatinToArabicTable()
{
    IndirectTable   latinToArabic;
#define BUCKWALTER_CONNECT(ARAB, LAT) \
    latinToArabic.setValue(LAT, ARAB);
#include "buckwalter.table"
#undef BUCKWALTER_CONNECT
    return latinToArabic;
}

static const IndirectTable& getLatinToArabicTable()
{
    static const IndirectTable   latinToArabic(buildLatinToArabicTable());
    return latinToArabic;
}

uint32_t buckwalterFromLatinToArabic(uint32_t symbol)
{
    const IndirectTable&    translator  =   getLatinToArabicTable();
    uint32_t                ret         =   translator.getValue(static_cast<uint32_t>(symbol));
    return ret;
}

static IndirectTable buildArabicToLatinTable()
{
    IndirectTable   arabicToLatin;
#define BUCKWALTER_CONNECT(ARAB, LAT) \
    arabicToLatin.setValue(ARAB, LAT);
#include "buckwalter.table"
#undef BUCKWALTER_CONNECT
    return arabicToLatin;
}

static const IndirectTable& getArabicToLatinTable()
{
    static const IndirectTable   arabicToLatin(buildArabicToLatinTable());
    return arabicToLatin;
}

uint32_t buckwalterFromArabicToLatin(uint32_t symbol)
{
    const IndirectTable&    translator  =   getArabicToLatinTable();
    uint32_t                ret         =   translator.getValue(symbol);
    return static_cast<char>(ret);
}

void buckwalterFromArabicToLatin(const char* str, size_t strLen, avector<char>& latin)
{
    //from arabic to latin size is only decreased
    latin.resize(strLen);
    size_t  strOffset   =   0;
    size_t  outOffset   =   0;
    while (strOffset < strLen)
    {
        uint32_t ch;
        U8_NEXT_UNSAFE(str, strOffset, ch);
        uint32_t latinCh    =   buckwalterFromArabicToLatin(ch);
        if (!latinCh) {
            latinCh =   ch;
        }
        U8_APPEND_UNSAFE(latin.get_buffer(), outOffset, latinCh);
    }
    latin.resize(outOffset);
}

void buckwalterFromLatinToArabic(const char* str, size_t strLen, avector<char>& arabic)
{
    //one arabic symbol takes 2 bytes for latin which takes 1
    arabic.resize(strLen * 2);
    size_t  strOffset   =   0;
    size_t  outOffset   =   0;
    while (strOffset < strLen)
    {
        uint32_t ch;
        U8_NEXT_UNSAFE(str, strOffset, ch);
        uint32_t arabicCh    =   buckwalterFromLatinToArabic(ch);
        if (!arabicCh) {
            arabicCh =   ch;
        }
        U8_APPEND_UNSAFE(arabic.get_buffer(), outOffset, arabicCh);
    }
    arabic.resize(outOffset);
}
