#ifndef _DOCTEXTSTORAGE_H_
#define _DOCTEXTSTORAGE_H_

#include <lib/aptl/avector.h>

#include "DocText.h"
#include "../MarkupStorage.h"

class DocTextStorage : public DocText, protected DocTextBin::Field
{
protected:
    uint64_t get( uint64_t bitsOffset, uint64_t bitsLength ) const
    {
        return Field::get( data, static_cast<size_t>( bitsOffset ), static_cast<size_t>( bitsLength ) );
    }

    uint64_t offsetTextSize;
    uint64_t offsetWordsCount;
    uint64_t offsetPhrasesCount;
    uint64_t offsetLinesCount;
    uint64_t offsetParagraphCount;
    uint64_t offsetText;
    uint64_t offsetWords;
    uint64_t offsetPhrases;
    uint64_t offsetLines;
    uint64_t offsetParagraphs;
    uint64_t itemOffsetShift;
    uint64_t itemLengthShift;
    uint64_t itemAttrsShift;
    uint64_t itemStartWordIdxShift;
    uint64_t itemWordsCountShift;
    uint64_t itemStartPhraseIdxShift;
    uint64_t itemPhrasesCountShift;
    uint8_t  itemWordBits;
    uint8_t  itemPhraseBits;
    uint8_t  itemLineBits;
    uint8_t  itemParaBits;
    
private:
    // copy prevention
    DocTextStorage( const DocTextStorage &src ) : DocText( src ) { }
    DocTextStorage &operator=( const DocTextStorage & ) { assert(false); abort(); return *this; }
    
public:
    DocTextStorage() :
        DocText(),
        offsetTextSize( 0 ),
        offsetWordsCount( 0 ),
        offsetPhrasesCount( 0 ),
        offsetLinesCount( 0 ),
        offsetParagraphCount( 0 ),
        offsetText( 0 ),
        offsetWords( 0 ),
        offsetPhrases( 0 ),
        offsetLines( 0 ),
        offsetParagraphs( 0 ),
        itemOffsetShift( 0 ),
        itemLengthShift( 0 ),
        itemAttrsShift( 0 ),
        itemStartWordIdxShift( 0 ),
        itemWordsCountShift( 0 ),
        itemStartPhraseIdxShift( 0 ),
        itemPhrasesCountShift( 0 ),
        itemWordBits( 0 ),
        itemPhraseBits( 0 ),
        itemLineBits( 0 ),
        itemParaBits( 0 ),
        data( 0 ),
        dataSize( 0 )
    {
    }

    int AppendMarkup( const MarkupTag */* markup */, MarkupAttrArray */* attributes */ = 0 ) { return 0; }
    int ImportMarkup( const MarkupTag */* markup */, MarkupAttrArray */* attributes */ = 0 ) { return 0; }
    
    size_t serializeSize() const { return dataSize; }

    void serialize( uint8_t *buffer ) const
    {
        if ( likely(data != 0) )
            memcpy( buffer, data, dataSize );
    }
    
    LingProcErrors load( const uint8_t *source, size_t sourceSize )
    {
        if ( unlikely((sourceSize << 3) < offsetFixedEnd) )
            return LP_ERROR_NOT_ENOUGH_DATA;

        data     = reinterpret_cast<const uint64_t *>( source );
        dataSize = sourceSize;

        if ( unlikely(get( offsetMagic, widthMagic ) != magic) )
            return LP_ERROR_INVALID_FORMAT;

        if ( unlikely(get( offsetVersion, widthVersion ) != version) )
            return LP_ERROR_INVALID_FORMAT;
            
        if ( unlikely(get( offsetVariant, widthVariant ) > variant) )
            return LP_ERROR_INVALID_FORMAT;

        offsetTextSize       = offsetFixedEnd;
        offsetWordsCount     = offsetTextSize     + get( offsetTextSizeBits,     widthTextSizeBits );
        offsetPhrasesCount   = offsetWordsCount   + get( offsetWordsCountBits,   widthWordsCountBits );
        offsetLinesCount     = offsetPhrasesCount + get( offsetPhrasesCountBits, widthPhrasesCountBits );
        offsetParagraphCount = offsetLinesCount   + get( offsetLinesCountBits,   widthLinesCountBits );

        offsetText           = ((offsetParagraphCount + get( offsetParagraphCountBits, widthParagraphCountBits ) + 7) >> 3) << 3;

        if ( unlikely(offsetText > (sourceSize << 3)) )
            return LP_ERROR_NOT_ENOUGH_DATA;
        
        itemWordBits         = ( get( offsetItemOffsetBits, widthItemOffsetBits ) +
                                 get( offsetItemLengthBits, widthItemLengthBits ) +
                                 get( offsetItemAttrsBits,  widthItemAttrsBits  ) );
        itemPhraseBits       = ( itemWordBits +
                                 get( offsetItemStartWordIdxBits, widthItemStartWordIdxBits ) +
                                 get( offsetItemWordsCountBits,   widthWordsCountBits ) );
        itemLineBits         = itemPhraseBits;
        itemParaBits         = ( itemPhraseBits +
                                 get( offsetItemStartPhraseIdxBits, widthItemStartPhraseIdxBits ) +
                                 get( offsetItemPhrasesCountBits, widthItemPhrasesCountBits ) );

        offsetWords          = offsetText  + ( get( offsetTextSize, get( offsetTextSizeBits, widthTextSizeBits ) ) << 3 );
        offsetPhrases        = offsetWords + ( itemWordBits * get( offsetWordsCount, get( offsetWordsCountBits, widthWordsCountBits ) ) );
        offsetLines          = offsetPhrases + ( itemPhraseBits * get( offsetPhrasesCount, get( offsetPhrasesCountBits, widthPhrasesCountBits ) ) );
        offsetParagraphs     = offsetLines + ( itemLineBits * get( offsetLinesCount, get( offsetLinesCountBits, widthLinesCountBits ) ) );

        uint64_t totalSize   = ( offsetParagraphs +
                                 ( itemParaBits *
                                   get( offsetParagraphCount, get( offsetParagraphCountBits, widthParagraphCountBits ) ) ) );
        
        if ( unlikely(totalSize > (sourceSize << 3)) )
            return LP_ERROR_NOT_ENOUGH_DATA;

        itemOffsetShift         = 0;
        itemLengthShift         = itemOffsetShift         + get( offsetItemOffsetBits,         widthItemOffsetBits );
        itemAttrsShift          = itemLengthShift         + get( offsetItemLengthBits,         widthItemLengthBits );
        itemStartWordIdxShift   = itemAttrsShift          + get( offsetItemAttrsBits,          widthItemAttrsBits );
        itemWordsCountShift     = itemStartWordIdxShift   + get( offsetItemStartWordIdxBits,   widthItemStartWordIdxBits );
        itemStartPhraseIdxShift = itemWordsCountShift     + get( offsetItemWordsCountBits,     widthItemWordsCountBits );
        itemPhrasesCountShift   = itemStartPhraseIdxShift + get( offsetItemStartPhraseIdxBits, widthItemStartPhraseIdxBits );

        setLangDetect( get( offsetLangDetect, widthLangDetect ) != 0 );
        setLangSet( get( offsetLangSet, widthLangSet ) != 0 );
        setUseLangDetector( get( offsetUseLangDetector, widthUseLangDetector ) != 0 );

        PresetLangs( static_cast<LangCodes>( get( offsetPresetPrimLang, widthPresetPrimLang ) ),
                     static_cast<LangCodes>( get( offsetPresetSuppLang, widthPresetSuppLang ) ),
                     checkUseLangDetector() );
        SetDetectedLangs( static_cast<LangCodes>( get( offsetDetectedPrimLang, widthDetectedPrimLang ) ),
                          static_cast<LangCodes>( get( offsetDetectedSuppLang, widthDetectedSuppLang ) ),
                          checkLangDetect() );
        InstallDetectedLangs();

        /* Load markup and wordsMarkup since version 1.2 */
        if ( get( offsetVersion, widthVersion ) > 1 ||
             ( get( offsetVersion, widthVersion ) == 1 &&
               get( offsetVariant, widthVariant ) >= 2 ) )
        {
            uint64_t offset           = ( ( totalSize + 63 ) >> 6 ) << 6; /* uint64_t padding */
            uint64_t sizeWidth        = get( offset, widthMarkupSizeBits );
            offset                   += widthMarkupSizeBits;
            uint64_t markupSize       = get( offset, sizeWidth );
            offset                   += sizeWidth;
            sizeWidth                 = get( offset, widthMarkupSizeBits );
            offset                   += widthMarkupSizeBits;
            uint64_t wordsMarkupSize  = get( offset, sizeWidth );
            offset                   += sizeWidth;

            totalSize                 = ( ( offset + 63 ) >> 6 ) << 3; /* uint64_t padding, octets */
            
            LingProcErrors status;

            if ( sourceSize - totalSize < markupSize )
                return LP_ERROR_NOT_ENOUGH_DATA;
            status = markupStorage.load( source + totalSize, markupSize );
            if ( unlikely(status != LP_OK) )
                return status;

            totalSize                += markupSize;
            
            if ( sourceSize - totalSize < wordsMarkupSize )
                return LP_ERROR_NOT_ENOUGH_DATA;
            status = wordsMarkupStorage.load( source + totalSize, wordsMarkupSize );
            if ( unlikely(status != LP_OK) )
                return status;
        }
        
        return LP_OK;
    }

    void ResetProcessing()
    {
        ResetFlagsLang();
        ResetFlagsDetectedLang();
        
        presetPrimLang   = LNG_UNKNOWN;
        presetSuppLang   = LNG_UNKNOWN;
        useLangDetector  = true;
    }

    void ResetData( ClearMode /* mode */ ) { }
    
    void ResetAll()
    {
        ResetProcessing();
        markup.clear();
    }

    LingProcErrors AppendString(
        const char * /* string */,
        size_t       /* size */,
        CodePages    /* stringCodepage */,
        bool         /* canonizeUtf */ )
    {
        return LP_ERROR_INVALID_MODE;
    }
    
    LingProcErrors AssignString(
        const char * /* string */,
        size_t       /* size */,
        CodePages    /* stringCodepage */,
        bool         /* canonizeUtf */ )
    {
        return LP_ERROR_INVALID_MODE;
    }

    LingProcErrors ReserveWords()
    {
        return LP_ERROR_INVALID_MODE;
    }

public:
    // length and size
    size_t TextLength() const // text length in bytes
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( offsetTextSize, get( offsetTextSizeBits, widthTextSizeBits ) ) );
    }

    const char *Text() const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return reinterpret_cast<const char *>( data ) + (offsetText >> 3);
    }
    
    size_t WordsCount() const // number of words
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( offsetWordsCount, get( offsetWordsCountBits, widthWordsCountBits ) ) );
    }

    size_t PhrasesCount() const // number of phrases
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( offsetPhrasesCount, get( offsetPhrasesCountBits, widthPhrasesCountBits ) ) );
    }
    
    size_t LinesCount() const // number of lines
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( offsetLinesCount, get( offsetLinesCountBits, widthLinesCountBits ) ) );
    }
    
    size_t ParaCount() const // number of paragraphs
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( offsetParagraphCount, get( offsetParagraphCountBits, widthParagraphCountBits ) ) );
    }

    // word access
    const DocTextWord *Word( size_t /* n */ ) const { return 0; }

    DocTextWord *Word( size_t /* n */ ) { return 0; }

    DocTextWord *NewWord()  { return 0; }
    DocTextWord *LastWord() { return 0; }
    
    const char *WordText( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return Text() + get( (n * itemWordBits) + offsetWords, get( offsetItemOffsetBits, widthItemOffsetBits ) );
    }

    const char *WordText( const DocTextWord & /* word */ ) const { return 0; }
    
    size_t WordLength( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemWordBits) + offsetWords + itemLengthShift,
                                         get( offsetItemLengthBits, widthItemLengthBits ) ) );
    }
    
    uint16_t WordAttrs( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<uint16_t>( get( (n * itemWordBits) + offsetWords + itemAttrsShift,
                                           get( offsetItemAttrsBits, widthItemAttrsBits ) ) );
    }

    size_t WordOffset( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemWordBits) + offsetWords + itemOffsetShift,
                                         get( offsetItemOffsetBits, widthItemOffsetBits ) ) );
    }

    // phrase access
    const DocTextPhrase *Phrase( size_t /* n */ ) const { return 0; }
    
    DocTextPhrase *Phrase( size_t /* n */ ) { return 0; }

    DocTextPhrase *NewPhrase() { return 0; }
    
    const char *PhraseText( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return Text() + get( (n * itemPhraseBits) + offsetPhrases, get( offsetItemOffsetBits, widthItemOffsetBits ) );
    }
        
    size_t PhraseLength( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemPhraseBits) + offsetPhrases + itemLengthShift,
                                         get( offsetItemLengthBits, widthItemLengthBits ) ) );
    }
    
    uint16_t PhraseAttrs( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<uint16_t>( get( (n * itemPhraseBits) + offsetPhrases + itemAttrsShift,
                                           get( offsetItemAttrsBits, widthItemAttrsBits ) ) );
    }

    size_t PhraseOffset( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemPhraseBits) + offsetPhrases + itemOffsetShift,
                                         get( offsetItemOffsetBits, widthItemOffsetBits ) ) );
    }

    size_t PhraseStartWordIndex( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemPhraseBits) + offsetPhrases + itemStartWordIdxShift,
                                         get( offsetItemStartWordIdxBits, widthItemStartWordIdxBits ) ) );
    }

    size_t PhraseWordsCount( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemPhraseBits) + offsetPhrases + itemWordsCountShift,
                                         get( offsetItemWordsCountBits, widthItemWordsCountBits ) ) );
    }
    
    // lines access
    const DocTextLine *Line( size_t /* n */ ) const { return 0; }

    DocTextLine *Line( size_t /* n */ ) { return 0; }

    DocTextLine *NewLine() { return 0; }
        
    const char *LineText( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return Text() + get( (n * itemLineBits) + offsetLines, get( offsetItemOffsetBits, widthItemOffsetBits ) );
    }
        
    size_t LineLength( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemLineBits) + offsetLines + itemLengthShift,
                                         get( offsetItemLengthBits, widthItemLengthBits ) ) );
    }
    
    uint16_t LineAttrs( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<uint16_t>( get( (n * itemLineBits) + offsetLines + itemAttrsShift,
                                           get( offsetItemAttrsBits, widthItemAttrsBits ) ) );
    }

    size_t LineOffset( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemLineBits) + offsetLines + itemOffsetShift,
                                         get( offsetItemOffsetBits, widthItemOffsetBits ) ) );
    }

    size_t LineStartWordIndex( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemLineBits) + offsetLines + itemStartWordIdxShift,
                                         get( offsetItemStartWordIdxBits, widthItemStartWordIdxBits ) ) );
    }

    size_t LineWordsCount( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemLineBits) + offsetLines + itemWordsCountShift,
                                         get( offsetItemWordsCountBits, widthItemWordsCountBits ) ) );
    }
    
    // paragraphs access                                                                                                         
    const DocTextPara *Paragraph( size_t /* n */ ) const { return 0; }

    DocTextPara *Paragraph( size_t /* n */ ) { return 0; }

    DocTextPara *NewPara() { return 0; }
    
    const char *ParagraphText( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return Text() + get( (n * itemParaBits) + offsetParagraphs, get( offsetItemOffsetBits, widthItemOffsetBits ) );
    }
        
    size_t ParagraphLength( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemParaBits) + offsetParagraphs + itemLengthShift,
                                         get( offsetItemLengthBits, widthItemLengthBits ) ) );
    }
    
    uint16_t ParagraphAttrs( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<uint16_t>( get( (n * itemParaBits) + offsetParagraphs + itemAttrsShift,
                                           get( offsetItemAttrsBits, widthItemAttrsBits ) ) );
    }

    size_t ParagraphOffset( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemParaBits) + offsetParagraphs + itemOffsetShift,
                                         get( offsetItemOffsetBits, widthItemOffsetBits ) ) );
    }

    size_t ParagraphStartWordIndex( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemParaBits) + offsetParagraphs + itemStartWordIdxShift,
                                         get( offsetItemStartWordIdxBits, widthItemStartWordIdxBits ) ) );
    }

    size_t ParagraphWordsCount( size_t n ) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemParaBits) + offsetParagraphs + itemWordsCountShift,
                                         get( offsetItemWordsCountBits, widthItemWordsCountBits ) ) );
    }

    size_t ParagraphStartPhraseIndex(size_t n) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemParaBits) + offsetParagraphs + itemStartPhraseIdxShift,
                                         get( offsetItemStartPhraseIdxBits, widthItemStartPhraseIdxBits ) ) );
    }

    size_t ParagraphPhrasesCount(size_t n) const
    {
        if ( unlikely(data == 0) )
            return 0;
        
        return static_cast<size_t>( get( (n * itemParaBits) + offsetParagraphs + itemPhrasesCountShift,
                                         get( offsetItemPhrasesCountBits, widthItemPhrasesCountBits ) ) );
    }

    const LPMarkup &GetTextMarkup( MarkupType type = MARKUP_DEFAULT ) const
    {
        switch ( type )
        {
        case MARKUP_WORDS:
            return wordsMarkupStorage;

        case MARKUP_DEFAULT:
        default:
            return markupStorage;
        };
    }

public:
    DocText &setWordBreak( bool /* value */ )       { return *this; }
    DocText &setPhraseBreak( bool /* value */ )     { return *this; }
    DocText &setLineBreak( bool /* value */ )       { return *this; }
    DocText &setParaBreak( bool /* value */ )       { return *this; }

    bool checkWordBreak()   const
    {
        if ( unlikely(data == 0) )
            return false;
        
        return get( offsetWordBreak, widthWordBreak ) != 0;
    }
    
    bool checkPhraseBreak() const
    {
        if ( unlikely(data == 0) )
            return false;
        
        return get( offsetPhraseBreak, widthPhraseBreak ) != 0;
    }
    
    bool checkLineBreak()
    {
        if ( unlikely(data == 0) )
            return false;
        
        return get( offsetLineBreak, widthLineBreak ) != 0;
    }
    
    bool checkParaBreak()
    {
        if ( unlikely(data == 0) )
            return false;
        
        return get( offsetParaBreak, widthParaBreak ) != 0;
    }
    
protected:
    const uint64_t *data;
    size_t          dataSize;

    LPMarkupStorage markupStorage;
    LPMarkupStorage wordsMarkupStorage;
};

#endif /* _DOCTEXTSTORAGE_H_ */

