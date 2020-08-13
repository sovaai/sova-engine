#include "DocText.h"

LingProcErrors DocText::AppendString(
    const char *string,
    size_t      size,
    CodePages   stringCodepage,
    bool        canonizeUtf )
{
    if ( size == static_cast<size_t>(-1) )
        size = strlen( string );

    if ( stringCodepage != CPG_UTF8 )
    {
        size_t new_size = size * 4 + 1;
        if(new_size > convertBuffer.size())
            convertBuffer.uninitialized_grow( new_size - convertBuffer.size() );
        if ( unlikely(convertBuffer.no_memory()) )
            return LP_ERROR_ENOMEM;

        Convert2UTF( reinterpret_cast<const unsigned char *>( string ),
                     size,
                     stringCodepage,
                     reinterpret_cast<unsigned char *>( convertBuffer.get_buffer() ),
                     convertBuffer.size() );
        string = convertBuffer.get_buffer();
        size   = strlen( string );
    }

    // create canonizer if need to
    if ( canonizeUtf )
    {
        UTFCanonizer::Status status = UTFCanonizer::STATUS_OK;
        if ( !canonizer.isCreated() )
            status = canonizer.create();
        if ( likely(status == UTFCanonizer::STATUS_OK) )
            status = canonizer.canonize( string, size );
        if ( unlikely(status != UTFCanonizer::STATUS_OK) )
        {
            switch ( status )
            {
            case UTFCanonizer::STATUS_ERROR_ALLOC: return LP_ERROR_ENOMEM;
            default: return LP_ERROR_EFAULT;
            };
        }
        string = canonizer.getResultText();
        size   = canonizer.getResultTextSize();
    }

    size_t offset = buffer.size();
    buffer.uninitialized_grow( size );
    if ( unlikely(buffer.no_memory()) )
        return LP_ERROR_ENOMEM;
        
    memcpy( buffer.get_buffer() + offset, string, size );
    return LP_OK;
}

LingProcErrors DocText::AppendStringSaveCoords(const char *string, size_t size, const UCharSet *ucs, UTFCanonizer::CoordMapping& mapping){
    if ( size == static_cast<size_t>(-1) )
        size = strlen( string );
    if(ucs==NULL)
        return LP_ERROR_EINVAL;
    //create canonizer
    UTFCanonizer::Status status = UTFCanonizer::STATUS_OK;
    if ( !canonizer.isCreated() )
        status = canonizer.create();
    if ( unlikely(status != UTFCanonizer::STATUS_OK) ){
        switch ( status ){
            case UTFCanonizer::STATUS_ERROR_ALLOC: return LP_ERROR_ENOMEM;
            default: return LP_ERROR_EFAULT;
        }
    }
    //canonize
    status = canonizer.CanonizeWithMapping(ucs, string, size, mapping);
    if ( unlikely(status != UTFCanonizer::STATUS_OK) ){
        switch ( status ){
            case UTFCanonizer::STATUS_ERROR_ALLOC: return LP_ERROR_ENOMEM;
            default: return LP_ERROR_EFAULT;
        }
    }
    //append
    return AppendString(canonizer.getResultText(), canonizer.getResultTextSize(), CPG_UTF8, false);
}

int DocText::AppendMarkup( const MarkupTag *markup, MarkupAttrArray *attributes )
{
    if ( markup == 0 )
        return 0;

    size_t docLen = TextLength();

    for ( const MarkupTag *beg = markup; !IsMarkupLast( *beg ); beg++ )
    {
        // skip until the markup opening
        while ( !IsMarkupLast( *beg ) && !IsOpeningTag( *beg ) )
            beg++;
        if ( IsMarkupLast( *beg ) )
            break;

        // set pointer to markup start
        const MarkupTag *cur = beg;
        unsigned int level = 1;
        do
        {
            cur++;
            if ( IsEqualTag( *beg, *cur ) )
            {
                if ( IsOpeningTag( *cur ) )
                    level++;
                else if ( --level == 0 )
                {
                    // Pair found!
                    bool pairFound = allowEmptyMarkup ? cur->m_nBeg >= beg->m_nBeg
                                                      : cur->m_nBeg > beg->m_nBeg ;
                    if ( pairFound )
                    {
                        LPMarkupItem mi( beg->m_Type, beg->m_nBeg,
                                         cur->m_nBeg - beg->m_nBeg,
                                         (unsigned short)( beg->m_Aux & ~MARKUP_LAST ) );
                        if ( (docLen == 0) || (mi.m_nBeg < docLen) )
                        {
                            if ( (docLen != 0) && (mi.m_nBeg + mi.m_nLen > docLen) )
                                mi.m_nLen = docLen - mi.m_nBeg;
                            this->markup.append( mi );
              
                            // and now append attributes for this tag
                            // using pBeg - pMarkup as attributes index
                            if ( attributes != 0 )
                                markupAttrs.add( attributes[ beg - markup ] );
                        }
                    }
                    break;
                }
            }
        } while( !IsMarkupLast( *cur ) );
    }

    return this->markup.size();
}

inline int BinSearch( const DocText &docText, size_t offset, size_t total, size_t val )
{
    int l = 0;
    int h = static_cast<int>(total) - 1;
    int m;
    while ( l <= h )
    {
        m = ( l + h ) >> 1;
        size_t test = docText.WordOffset( static_cast<size_t>(m + offset) );
        if ( test < val )
            l = m + 1;
        else if ( test == val )
            return m;
        else
            h = m - 1;
    }

    return l;
}

LingProcErrors DocText::MarkupOffsetToWords( LPMarkup &targetMarkup ) const
{
    targetMarkup.clear();

    const LPMarkup &src = GetTextMarkup();

    size_t ml = src.size();
    size_t wl = WordsCount();

    for ( size_t i = 0; i < ml; i++ )
    {
        LPMarkupItem mup = src[ i ];

        size_t ofs = mup.m_nBeg;
        mup.m_nBeg = BinSearch( *this, 0, wl, ofs );

        if ( mup.m_nBeg >= wl ) { // after last word
            if ( !allowEmptyMarkup )
                continue;
            mup.m_nBeg=wl;
            mup.m_nLen=0;
        } else {
            mup.m_nLen = BinSearch( *this, mup.m_nBeg, wl - mup.m_nBeg, ofs + mup.m_nLen );
            if (( mup.m_nLen == 0 ) && ( !allowEmptyMarkup ))   // no words in markup
                continue;
        }

        if ( targetMarkup.append( mup.m_Type, mup.m_nBeg, mup.m_nLen ) != 0 )
            return LP_ERROR_ENOMEM;
    }

    return LP_OK;
}

DocTextBin::FieldsBits DocText::prepareSerialize() const
{
    DocTextPara max;
        
    for ( size_t i = 0; i < WordsCount(); i++ )
    {
        if ( WordOffset( i ) > max.Offset() )
            max.Init( WordOffset( i ), max.Length(), max.Attrs(), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( WordLength( i ) > max.Length() )
            max.Init( max.Offset(), WordLength( i ), max.Attrs(), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( WordAttrs( i ) > max.Attrs() )
            max.Init( max.Offset(), max.Length(), WordAttrs( i ), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
    }

    for ( size_t i = 0; i < LinesCount(); i++ )
    {
        if ( LineLength( i ) > max.Length() )
            max.Init( max.Offset(), LineLength( i ), max.Attrs(), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( LineAttrs( i ) > max.Attrs() )
            max.Init( max.Offset(), max.Length(), LineAttrs( i ), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( LineStartWordIndex( i ) > max.StartWordIndex() )
            max.Init( max.Offset(), max.Length(), max.Attrs(), LineStartWordIndex( i ), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( LineWordsCount( i ) > max.WordsCount() )
            max.Init( max.Offset(), max.Length(), max.Attrs(), max.StartWordIndex(), LineWordsCount( i ), max.StartPhraseIndex(), max.PhrasesCount() );
    }

    for ( size_t i = 0; i < PhrasesCount(); i++ )
    {
        if ( PhraseLength( i ) > max.Length() )
            max.Init( max.Offset(), PhraseLength( i ), max.Attrs(), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( PhraseAttrs( i ) > max.Attrs() )
            max.Init( max.Offset(), max.Length(), PhraseAttrs( i ), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( PhraseStartWordIndex( i ) > max.StartWordIndex() )
            max.Init( max.Offset(), max.Length(), max.Attrs(), PhraseStartWordIndex( i ), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( PhraseWordsCount( i ) > max.WordsCount() )
            max.Init( max.Offset(), max.Length(), max.Attrs(), max.StartWordIndex(), PhraseWordsCount( i ), max.StartPhraseIndex(), max.PhrasesCount() );
    }

    for ( size_t i = 0; i < ParaCount(); i++ )
    {
        if ( ParagraphLength( i ) > max.Length() )
            max.Init( max.Offset(), ParagraphLength( i ), max.Attrs(), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( ParagraphAttrs( i ) > max.Attrs() )
            max.Init( max.Offset(), max.Length(), ParagraphAttrs( i ), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( ParagraphStartWordIndex( i ) > max.StartWordIndex() )
            max.Init( max.Offset(), max.Length(), max.Attrs(), ParagraphStartWordIndex( i ), max.WordsCount(), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( ParagraphWordsCount( i ) > max.WordsCount() )
            max.Init( max.Offset(), max.Length(), max.Attrs(), max.StartWordIndex(), ParagraphWordsCount( i ), max.StartPhraseIndex(), max.PhrasesCount() );
        if ( ParagraphStartPhraseIndex( i ) > max.StartPhraseIndex() )
            max.Init( max.Offset(), max.Length(), max.Attrs(), max.StartWordIndex(), max.WordsCount(), ParagraphStartPhraseIndex( i ), max.PhrasesCount() );
        if ( ParagraphPhrasesCount( i ) > max.PhrasesCount() )
            max.Init( max.Offset(), max.Length(), max.Attrs(), max.StartWordIndex(), max.WordsCount(), max.StartPhraseIndex(), ParagraphPhrasesCount( i ) );
    }
        
    FieldsBits bits;
    bits.textSizeBits           = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( TextLength() ) ) );
    bits.wordsCountBits         = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( WordsCount() ) ) );
    bits.phrasesCountBits       = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( PhrasesCount() ) ) );
    bits.linesCountBits         = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( LinesCount() ) ) );
    bits.paragraphCountBits     = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( ParaCount() ) ) );
    bits.itemOffsetBits         = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.Offset() ) ) );
    bits.itemLengthBits         = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.Length() ) ) );
    bits.itemAttrsBits          = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.Attrs() ) ) );
    bits.itemStartWordIdxBits   = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.StartWordIndex() ) ) );
    bits.itemWordsCountBits     = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.WordsCount() ) ) );
    bits.itemStartPhraseIdxBits = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.StartPhraseIndex() ) ) );
    bits.itemPhrasesCountBits   = static_cast<uint8_t>( widthBits( static_cast<uint64_t>( max.PhrasesCount() ) ) );
    bits.itemWordBits           = bits.itemOffsetBits + bits.itemLengthBits + bits.itemAttrsBits;
    bits.itemPhraseBits         = bits.itemWordBits + bits.itemStartWordIdxBits + bits.itemWordsCountBits;
    bits.itemLineBits           = bits.itemPhraseBits;
    bits.itemParaBits           = bits.itemPhraseBits + bits.itemStartPhraseIdxBits + bits.itemPhrasesCountBits;

    if ( !wordsMarkupConverted )
    {
        MarkupOffsetToWords( const_cast< LPMarkupArray & >( wordsMarkup ) );
        const_cast< DocText * >(this)->wordsMarkupConverted = true;
    }

    bits.markupSize           = markup.serializeSize();
    bits.wordsMarkupSize      = wordsMarkup.serializeSize();
    bits.widthMarkupSize      = static_cast<uint8_t>( widthBits( bits.markupSize ) );
    bits.widthWordsMarkupSize = static_cast<uint8_t>( widthBits( bits.wordsMarkupSize ) );
    
    return bits;
}

size_t DocText::serializeSize() const
{
    FieldsBits bits = prepareSerialize();

    return static_cast<size_t>( ( ( ( ( ( ( ( offsetFixedEnd + 
                                              bits.textSizeBits +
                                              bits.wordsCountBits +
                                              bits.phrasesCountBits +
                                              bits.linesCountBits +
                                              bits.paragraphCountBits ) + 7 ) >> 3 ) << 3 ) +
                                      ( TextLength() << 3 ) +
                                      ( static_cast<uint64_t>( bits.itemWordBits ) * WordsCount() ) +
                                      ( static_cast<uint64_t>( bits.itemPhraseBits ) * PhrasesCount() ) +
                                      ( static_cast<uint64_t>( bits.itemLineBits ) * LinesCount() ) +
                                      ( static_cast<uint64_t>( bits.itemParaBits ) * ParaCount() ) +
                                      63 /* uint64_t padding */ ) >> 6 ) << 3 ) + /* main part */
                                ( ( ( widthMarkupSizeBits + bits.widthMarkupSize +
                                      widthMarkupSizeBits + bits.widthWordsMarkupSize
                                      + 63 ) >> 6 ) << 3 ) + /* markups sizes */
                                bits.markupSize +
                                bits.wordsMarkupSize );
}

void DocText::serialize( uint8_t *buffer ) const
{
    FieldsBits  bits = prepareSerialize();
    uint64_t   *data = reinterpret_cast<uint64_t *>( buffer );

    DocTextBin::Field f;
        
    f.set( data, magic, offsetMagic, widthMagic );
    f.set( data, version, offsetVersion, widthVersion );
    f.set( data, variant, offsetVariant, widthVariant );

    f.set( data, bits.textSizeBits, offsetTextSizeBits, widthTextSizeBits );
    f.set( data, bits.wordsCountBits, offsetWordsCountBits, widthWordsCountBits );
    f.set( data, bits.phrasesCountBits, offsetPhrasesCountBits, widthPhrasesCountBits );
    f.set( data, bits.linesCountBits, offsetLinesCountBits, widthLinesCountBits );
    f.set( data, bits.paragraphCountBits, offsetParagraphCountBits, widthParagraphCountBits );
    f.set( data, ( checkWordBreak() ? 1 : 0 ), offsetWordBreak, widthWordBreak );
    f.set( data, ( checkPhraseBreak() ? 1 : 0 ), offsetPhraseBreak, widthPhraseBreak );
    f.set( data, ( checkLineBreak() ? 1 : 0 ), offsetLineBreak, widthLineBreak );
    f.set( data, ( checkParaBreak() ? 1 : 0 ), offsetParaBreak, widthParaBreak );
    f.set( data, ( checkLangDetect() ? 1 : 0 ), offsetLangDetect, widthLangDetect );
    f.set( data, ( checkLangSet() ? 1 : 0 ), offsetLangSet, widthLangSet );
    f.set( data, ( checkUseLangDetector() ? 1 : 0 ), offsetUseLangDetector, widthUseLangDetector );
    f.set( data, bits.itemOffsetBits, offsetItemOffsetBits, widthItemOffsetBits );
    f.set( data, bits.itemLengthBits, offsetItemLengthBits, widthItemLengthBits );
    f.set( data, bits.itemAttrsBits, offsetItemAttrsBits, widthItemAttrsBits );
    f.set( data, bits.itemStartWordIdxBits, offsetItemStartWordIdxBits, widthItemStartWordIdxBits );
    f.set( data, bits.itemWordsCountBits, offsetItemWordsCountBits, widthItemWordsCountBits );
    f.set( data, bits.itemStartPhraseIdxBits, offsetItemStartPhraseIdxBits, widthItemStartPhraseIdxBits );
    f.set( data, bits.itemPhrasesCountBits, offsetItemPhrasesCountBits, widthItemPhrasesCountBits );
    f.set( data, static_cast<uint64_t>( getDetectedPrimLang() ), offsetDetectedPrimLang, widthDetectedPrimLang );
    f.set( data, static_cast<uint64_t>( getDetectedSuppLang() ), offsetDetectedSuppLang, widthDetectedSuppLang );
    f.set( data, static_cast<uint64_t>( getPresetPrimLang() ), offsetPresetPrimLang, widthPresetPrimLang );
    f.set( data, static_cast<uint64_t>( getPresetSuppLang() ), offsetPresetSuppLang, widthPresetSuppLang );

    uint64_t offset = offsetFixedEnd;
        
    f.set( data, TextLength(), offset, bits.textSizeBits );
    offset += bits.textSizeBits;
    f.set( data, WordsCount(), offset, bits.wordsCountBits );
    offset += bits.wordsCountBits;
    f.set( data, PhrasesCount(), offset, bits.phrasesCountBits );
    offset += bits.phrasesCountBits;
    f.set( data, LinesCount(), offset, bits.linesCountBits );
    offset += bits.linesCountBits;
    f.set( data, ParaCount(), offset, bits.paragraphCountBits );
    offset += bits.paragraphCountBits;

    offset = ( ( offset + 7 ) >> 3 ) << 3;
    memcpy( buffer + ( offset >> 3 ), Text(), TextLength() );
    offset += TextLength() << 3;

    for ( size_t i = 0; i < WordsCount(); i++ )
    {
        f.set( data, WordOffset( i ), offset, bits.itemOffsetBits );
        offset += bits.itemOffsetBits;
        f.set( data, WordLength( i ), offset, bits.itemLengthBits );
        offset += bits.itemLengthBits;
        f.set( data, WordAttrs( i ), offset, bits.itemAttrsBits );
        offset += bits.itemAttrsBits;
    }

    for ( size_t i = 0; i < PhrasesCount(); i++ )
    {
        f.set( data, PhraseOffset( i ), offset, bits.itemOffsetBits );
        offset += bits.itemOffsetBits;
        f.set( data, PhraseLength( i ), offset, bits.itemLengthBits );
        offset += bits.itemLengthBits;
        f.set( data, PhraseAttrs( i ), offset, bits.itemAttrsBits );
        offset += bits.itemAttrsBits;
        f.set( data, PhraseStartWordIndex( i ), offset, bits.itemStartWordIdxBits );
        offset += bits.itemStartWordIdxBits;
        f.set( data, PhraseWordsCount( i ), offset, bits.itemWordsCountBits );
        offset += bits.itemWordsCountBits;
    }

    for ( size_t i = 0; i < LinesCount(); i++ )
    {
        f.set( data, LineOffset( i ), offset, bits.itemOffsetBits );
        offset += bits.itemOffsetBits;
        f.set( data, LineLength( i ), offset, bits.itemLengthBits );
        offset += bits.itemLengthBits;
        f.set( data, LineAttrs( i ), offset, bits.itemAttrsBits );
        offset += bits.itemAttrsBits;
        f.set( data, LineStartWordIndex( i ), offset, bits.itemStartWordIdxBits );
        offset += bits.itemStartWordIdxBits;
        f.set( data, LineWordsCount( i ), offset, bits.itemWordsCountBits );
        offset += bits.itemWordsCountBits;
    }
        
    for ( size_t i = 0; i < ParaCount(); i++ )
    {
        f.set( data, ParagraphOffset( i ), offset, bits.itemOffsetBits );
        offset += bits.itemOffsetBits;
        f.set( data, ParagraphLength( i ), offset, bits.itemLengthBits );
        offset += bits.itemLengthBits;
        f.set( data, ParagraphAttrs( i ), offset, bits.itemAttrsBits );
        offset += bits.itemAttrsBits;
        f.set( data, ParagraphStartWordIndex( i ), offset, bits.itemStartWordIdxBits );
        offset += bits.itemStartWordIdxBits;
        f.set( data, ParagraphWordsCount( i ), offset, bits.itemWordsCountBits );
        offset += bits.itemWordsCountBits;
        f.set( data, ParagraphStartPhraseIndex( i ), offset, bits.itemStartPhraseIdxBits );
        offset += bits.itemStartPhraseIdxBits;
        f.set( data, ParagraphPhrasesCount( i ), offset, bits.itemPhrasesCountBits );
        offset += bits.itemPhrasesCountBits;
    }

    buffer += ( ( offset + 63 ) >> 6 ) << 3; /* main part, uint64_t padding */
    data    = reinterpret_cast<uint64_t *>( buffer );
    offset  = 0;

    f.set( data, bits.widthMarkupSize, offset, widthMarkupSizeBits );
    offset += widthMarkupSizeBits;
    f.set( data, bits.markupSize, offset, bits.widthMarkupSize );
    offset += bits.widthMarkupSize;
    f.set( data, bits.widthWordsMarkupSize, offset, widthMarkupSizeBits );
    offset += widthMarkupSizeBits;
    f.set( data, bits.wordsMarkupSize, offset, bits.widthWordsMarkupSize );
    offset += bits.widthWordsMarkupSize;

    buffer += ( ( offset + 63 ) >> 6 ) << 3; /* markups sizes part, uint64_t padding */
    markup.serialize( buffer );

    buffer += bits.markupSize;
    wordsMarkup.serialize( buffer );

    buffer += bits.wordsMarkupSize;
}

