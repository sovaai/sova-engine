#ifndef _DOCTEXTBIN_H_
#define _DOCTEXTBIN_H_

#include "../BinaryData.h"

class DocTextBin
{
public:
    typedef BinaryFieldRaw< uint64_t > Field;
    
protected:    
    static const uint32_t    magic   = 0x6a8cc1f4;
    static const uint8_t     version = 1;
    static const uint8_t     variant = 2;
    // static const uint6_t  textSizeBits;
    // static const uint5_t  wordsCountBits;
    // static const uint5_t  phrasesCountBits;
    // static const uint5_t  linesCountBits;
    // static const uint5_t  paragraphCountBits;
    // static const uint1_t  wordBreak;
    // static const uint1_t  phraseBreak;
    // static const uint1_t  lineBreak;
    // static const uint1_t  paraBreak;
    // static const uint1_t  langDetect;
    // static const uint1_t  langSet;
    // static const uint1_t  useLangDetector;
    // static const uint6_t  itemOffsetBits;
    // static const uint6_t  itemLengthBits;
    // static const uint6_t  itemAttrsBits;
    // static const uint5_t  itemStartWordIdxBits;
    // static const uint5_t  itemWordsCountBits;
    // static const uint5_t  itemStartPhraseIdxBits;
    // static const uint5_t  itemPhrasesCountBits;
    // static const uint16_t detectedPrimLang;
    // static const uint16_t detectedSuppLang;
    // static const uint16_t presetPrimLang;
    // static const uint16_t presetSuppLang;
    
    static const uint64_t offsetMagic                  = 0;
    static const uint64_t widthMagic                   = (sizeof( uint32_t ) << 3);
    static const uint64_t offsetVersion                = offsetMagic + widthMagic;
    static const uint64_t widthVersion                 = (sizeof( uint8_t ) << 3);
    static const uint64_t offsetVariant                = offsetVersion + widthVersion;
    static const uint64_t widthVariant                 = (sizeof( uint8_t ) << 3);
    static const uint64_t offsetTextSizeBits           = offsetVariant + widthVariant;
    static const uint64_t widthTextSizeBits            = 6;
    static const uint64_t offsetWordsCountBits         = offsetTextSizeBits + widthTextSizeBits;
    static const uint64_t widthWordsCountBits          = 5;
    static const uint64_t offsetPhrasesCountBits       = offsetWordsCountBits + widthWordsCountBits;
    static const uint64_t widthPhrasesCountBits        = 5;
    static const uint64_t offsetLinesCountBits         = offsetPhrasesCountBits + widthPhrasesCountBits;
    static const uint64_t widthLinesCountBits          = 5;
    static const uint64_t offsetParagraphCountBits     = offsetLinesCountBits + widthLinesCountBits;
    static const uint64_t widthParagraphCountBits      = 5;
    static const uint64_t offsetWordBreak              = offsetParagraphCountBits + widthParagraphCountBits;
    static const uint64_t widthWordBreak               = 1;
    static const uint64_t offsetPhraseBreak            = offsetWordBreak + widthWordBreak;
    static const uint64_t widthPhraseBreak             = 1;
    static const uint64_t offsetLineBreak              = offsetPhraseBreak + widthPhraseBreak;
    static const uint64_t widthLineBreak               = 1;
    static const uint64_t offsetParaBreak              = offsetLineBreak + widthLineBreak;
    static const uint64_t widthParaBreak               = 1;
    static const uint64_t offsetLangDetect             = offsetParaBreak + widthParaBreak;
    static const uint64_t widthLangDetect              = 1;
    static const uint64_t offsetLangSet                = offsetLangDetect + widthLangDetect;
    static const uint64_t widthLangSet                 = 1;
    static const uint64_t offsetUseLangDetector        = offsetLangSet + widthLangSet;
    static const uint64_t widthUseLangDetector         = 1;
    static const uint64_t offsetItemOffsetBits         = offsetUseLangDetector + widthUseLangDetector;
    static const uint64_t widthItemOffsetBits          = 6;
    static const uint64_t offsetItemLengthBits         = offsetItemOffsetBits + widthItemOffsetBits;
    static const uint64_t widthItemLengthBits          = 6;
    static const uint64_t offsetItemAttrsBits          = offsetItemLengthBits + widthItemLengthBits;
    static const uint64_t widthItemAttrsBits           = 6;
    static const uint64_t offsetItemStartWordIdxBits   = offsetItemAttrsBits + widthItemAttrsBits;
    static const uint64_t widthItemStartWordIdxBits    = 5;
    static const uint64_t offsetItemWordsCountBits     = offsetItemStartWordIdxBits + widthItemStartWordIdxBits;
    static const uint64_t widthItemWordsCountBits      = 5;
    static const uint64_t offsetItemStartPhraseIdxBits = offsetItemWordsCountBits + widthItemWordsCountBits;
    static const uint64_t widthItemStartPhraseIdxBits  = 5;
    static const uint64_t offsetItemPhrasesCountBits   = offsetItemStartPhraseIdxBits + widthItemStartPhraseIdxBits;
    static const uint64_t widthItemPhrasesCountBits    = 5;
    static const uint64_t offsetDetectedPrimLang       = offsetItemPhrasesCountBits + widthItemPhrasesCountBits;
    static const uint64_t widthDetectedPrimLang        = 16;
    static const uint64_t offsetDetectedSuppLang       = offsetDetectedPrimLang + widthDetectedPrimLang;
    static const uint64_t widthDetectedSuppLang        = 16;
    static const uint64_t offsetPresetPrimLang         = offsetDetectedSuppLang + widthDetectedSuppLang;
    static const uint64_t widthPresetPrimLang          = 16;
    static const uint64_t offsetPresetSuppLang         = offsetPresetPrimLang + widthPresetPrimLang;
    static const uint64_t widthPresetSuppLang          = 16;

    static const uint64_t offsetFixedEnd               = offsetPresetSuppLang + widthPresetSuppLang;

    static const uint64_t widthMarkupSizeBits          = 6;
    
protected:
    size_t widthBits( uint64_t value ) const
    {
        size_t width = 0;
        for ( ; value != 0; value >>= 1, width++ );
        return width;
    }

    struct FieldsBits
    {
        uint8_t  textSizeBits;
        uint8_t  wordsCountBits;
        uint8_t  phrasesCountBits;
        uint8_t  linesCountBits;
        uint8_t  paragraphCountBits;
        uint8_t  itemOffsetBits;
        uint8_t  itemLengthBits;
        uint8_t  itemAttrsBits;
        uint8_t  itemStartWordIdxBits;
        uint8_t  itemWordsCountBits;
        uint8_t  itemStartPhraseIdxBits;
        uint8_t  itemPhrasesCountBits;
        uint8_t  itemWordBits;
        uint8_t  itemPhraseBits;
        uint8_t  itemLineBits;
        uint8_t  itemParaBits;
        uint8_t  widthMarkupSize;
        uint8_t  widthWordsMarkupSize;
        uint64_t markupSize;
        uint64_t wordsMarkupSize;

        FieldsBits() :
            textSizeBits( 0 ),
            wordsCountBits( 0 ),
            phrasesCountBits( 0 ),
            linesCountBits( 0 ),
            paragraphCountBits( 0 ),
            itemOffsetBits( 0 ),
            itemLengthBits( 0 ),
            itemAttrsBits( 0 ),
            itemStartWordIdxBits( 0 ),
            itemWordsCountBits( 0 ),
            itemStartPhraseIdxBits( 0 ),
            itemPhrasesCountBits( 0 ),
            itemWordBits( 0 ),
            itemPhraseBits( 0 ),
            itemLineBits( 0 ),
            itemParaBits( 0 ),
            widthMarkupSize( 0 ),
            widthWordsMarkupSize( 0 ),
            markupSize( 0 ),
            wordsMarkupSize( 0 )
        {
        }
    };
};

#endif /* _DOCTEXTBIN_H_ */

