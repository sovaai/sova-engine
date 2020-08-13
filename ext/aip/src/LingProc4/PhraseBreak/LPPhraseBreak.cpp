#include "../LingProc.h"
#include "../LingProcData.h"

// phrase breaker
inline bool is_number( const char *src, size_t len, const UCharSet *charset )
{
    for ( size_t i = 0; i < len ; )
    {
        uint32_t ch;
        U8_NEXT_UNSAFE( src, i, ch);

        if ( !charset->IsDigit( ch ) )
            return false;
    }
    return true;
}

inline bool IgnoreMarkup( unsigned short markup_tag )
{
    markup_tag &= ~(MARKUP_CLOSING);

    return  markup_tag == MARKUP_LINK     || markup_tag == MARKUP_LINK_IMG  || markup_tag == MARKUP_ALT       ||
            markup_tag == MARKUP_KEYWORDS || markup_tag == MARKUP_AUTHOR    || markup_tag == MARKUP_ABSTRACT  ||
            markup_tag == MARKUP_INPUT    || markup_tag == MARKUP_FILTEROUT || markup_tag == MARKUP_COMMENT   ||
            markup_tag == MARKUP_BASE_LINK;
}

/* gets the next word after idx, skips all markup, mk_idx - is index in markup, used for speeding up */
static int GetNextWord( const DocText &docText, size_t paraIdx, size_t &idx, size_t &nextMkIdx )
{
    const LPMarkup &markup = docText.GetTextMarkup();

    size_t newParaIdx = paraIdx;
    size_t &mkIdx     = nextMkIdx;

    if ( idx < docText.WordsCount() && docText.WordAttrs( idx ) & LingProc::fWordIsLastInPara )
    {
        newParaIdx++;
        for ( ;
              newParaIdx < docText.ParaCount() && docText.ParagraphWordsCount( newParaIdx ) == 0;
              newParaIdx++ );
    }
    else
    {
        if ( idx == static_cast<size_t>(-1) ) // skip paragraphs before the first word
            for ( ;
                  newParaIdx < docText.ParaCount() && docText.ParagraphWordsCount( newParaIdx ) == 0;
                  newParaIdx++ );
    }

    if ( idx == docText.WordsCount() - 1 ) // if we have the last word in text
    {
        idx = -1;
        return newParaIdx - paraIdx; // returns paragraph increment
    }

    idx++;
    if ( markup.size() == 0 )
        return newParaIdx - paraIdx; // returns paragraph increment

    bool wordIgnore = true;
    // skip words
    while ( idx < docText.WordsCount() )
    {
        // skip markup
        size_t border = docText.WordOffset( idx ) + docText.WordLength( idx );

        // skip markup which ends before the word
        wordIgnore = false; // if nothing found, the word is considered non-ignored
        while ( mkIdx < markup.size() )
        {
            if ( markup[ mkIdx ].m_nBeg >= border ) // ignored markup for current word not found
                break;
            if ( markup[ mkIdx ].m_nBeg + markup[ mkIdx ].m_nLen >= border )
            {
                wordIgnore = IgnoreMarkup( markup[ mkIdx ].m_Type );
                if ( wordIgnore )
                    break;
            }
            mkIdx++;
        }

        // move to next word if current ignored
        if( wordIgnore )
        {
            // skip paragraphs not containing words
            if ( docText.WordAttrs( idx ) & LingProc::fWordIsLastInPara )
            {
                newParaIdx++;
                for ( ;
                      newParaIdx < docText.ParaCount() && docText.ParagraphWordsCount( newParaIdx ) == 0;
                      newParaIdx++ );
            }

            // move to the next word
            idx++;
        }
        else
            break;
    }

    return newParaIdx - paraIdx; // returns paragraph increment
}

static LingProcErrors CheckPhraseEnd( const DocText           &docText,
                                      const UCharSet          *charset,
                                      const PhraseBreakerData &dicts,
                                      size_t                   leftWordIdx,
                                      size_t                   rightWordIdx,
                                      size_t                   punctIdx,
                                      bool                     quoteOnLeft,
                                      bool                     quoteOnRight,
                                      bool                    &result )
{
    const char *text;
    size_t      offset = 0;
    uint32_t    punctSymbol;
    // current word which is tested to be the end of sentence
    text = docText.WordText( punctIdx );
    U8_NEXT_UNSAFE( text, offset, punctSymbol );
    if ( unlikely(offset != docText.WordLength( punctIdx )) )
        return LP_ERROR_EFAULT;

    bool spaceAfter = (docText.WordAttrs( punctIdx ) & LingProc::fWordHasSpaceAfter) != 0;

    // process left word
    const DocTextWord *leftWord = docText.Word( leftWordIdx );
    if ( unlikely(leftWord == 0) )
        return LP_ERROR_ERANGE;
    const char *leftWordText = docText.WordText( *leftWord );
    uint32_t leftWordFirstSymbol;
    offset = 0;
    U8_NEXT_UNSAFE( leftWordText, offset, leftWordFirstSymbol );
    if ( unlikely(offset > leftWord->Length()) )
        return LP_ERROR_EFAULT;

    // process right word
    const DocTextWord *rightWord = docText.Word( rightWordIdx );
    if ( unlikely(rightWord == 0) )
        return LP_ERROR_ERANGE;
    const char *rightWordText = docText.WordText( *rightWord );
    uint32_t rightWordFirstSymbol;
    offset = 0;
    U8_NEXT_UNSAFE( rightWordText, offset, rightWordFirstSymbol );
    if ( unlikely(offset > rightWord->Length()) )
        return LP_ERROR_EFAULT;

    if ( punctSymbol == static_cast<uint32_t>('!') ||
         punctSymbol == static_cast<uint32_t>('?') )
    {
        // question or exclamation phrase
        if ( charset->IsUpper( leftWordFirstSymbol ) ||
             charset->IsDigit( rightWordFirstSymbol ) )
        {
            // Next word is in titlecase or is a digit
            result = true;
            return LP_OK;
        }
        result = false;
        return LP_OK;
    }

    if ( spaceAfter ) // declarative sentence, splitted by space
    {
        if ( !quoteOnLeft && dicts.IsAbbr( docText, *leftWord ) ) // if left word is an abbreviation
        {
            if ( dicts.IsStopWord( docText, *rightWord ) && !charset->IsLower( rightWordFirstSymbol ) )
            {
                // if second word is in stop dict and isn't in lowercase, then assume point as end of the sentence
                result = true;
                return LP_OK;
            }
            else
            {
                // if second word is not in stop dict or is in lowercase, then we cannot assume point as end of the sentence
                result = false;
                return LP_OK;
            }
        }

        if ( !quoteOnRight && dicts.IsCompend( docText, *rightWord ) )
        {
            // if right word is the part of a compound word, then we cannot assume point as end of the sentence
            result = false;
            return LP_OK;
        }

        if ( is_number( leftWordText, leftWord->Length(), charset ) &&
             is_number( rightWordText, rightWord->Length(), charset) )
        {
            // if both words contains only digits, then we cannot assume point as end of the sentence
            result = false;
            return LP_OK;
        }

        if ( !charset->IsUpper( rightWordFirstSymbol ) &&
             !charset->IsDigit( rightWordFirstSymbol ) )
        {
            // if word doesn't start from uppercase letter and is not a number, then we cannot assume point as end of the sentence
            result = false;
            return LP_OK;
        }

        result = true;
        return LP_OK;
    }
    else
    {
        // bound sentences
        if ( charset->IsUpper( rightWordFirstSymbol ) ||
             charset->IsDigit( rightWordFirstSymbol ) )
        {
            // if second word starts from uppercase letter or is a digit

            uint32_t leftWordLastSymbol;
            offset = leftWord->Length();
            U8_PREV_UNSAFE( leftWordText, offset, leftWordLastSymbol );

            if ( charset->IsDigit( leftWordLastSymbol ) &&
                 charset->IsDigit( rightWordFirstSymbol ) )
            {
                // if second word first symbol is a digit and first word last symbol is also a digit, then we cannot assume point as end of the sentence
                result = false;
                return LP_OK;
            }

            if ( !quoteOnLeft && dicts.IsAbbr( docText, *leftWord) )
            {
                // if first word is an abbreviation
                if ( dicts.IsStopWord( docText, *rightWord ) )
                {
                    // if second word is not in stop dict, then we can assume point as the end of the sentence
                    result = true;
                    return LP_OK;
                }
                else
                {
                    // if second word is in stop dict, then we cannot assume point as the end of the sentence
                    result = false;
                    return LP_OK;
                }
            }
            if ( !quoteOnRight && dicts.IsCompend( docText, *rightWord ) )
            {
                // if right word is the part of a compound word, then we cannot assume point as end of the sentence
                result = false;
                return LP_OK;
            }

            result = true; // in all rest cases we can assume point as the end of the sentence
            return LP_OK;
        }
        result = false;
        return LP_OK;
    }
}

#if 0
// not used static function
static LingProcErrors CheckPhraseEndForCaseInsensitiveLanguage( const DocText           &docText,
                                                                const UCharSet          *charset,
                                                                const PhraseBreakerData &dicts,
                                                                size_t                   leftWordIdx,
                                                                size_t                   rightWordIdx,
                                                                size_t                   punctIdx,
                                                                bool                     quoteOnLeft,
                                                                bool                     quoteOnRight,
                                                                bool                    &result )
{
    const char *text;
    size_t      offset = 0;
    uint32_t    punctSymbol;
    // current word which is tested to be the end of sentence
    text = docText.WordText( punctIdx );
    U8_NEXT_UNSAFE( text, offset, punctSymbol );
    if ( unlikely(offset != docText.WordLength( punctIdx )) )
        return LP_ERROR_EFAULT;

    bool spaceAfter = (docText.WordAttrs( punctIdx ) & LingProc::fWordHasSpaceAfter) != 0;

    // process left word
    const DocTextWord *leftWord = docText.Word( leftWordIdx );
    if ( unlikely(leftWord == 0) )
        return LP_ERROR_ERANGE;
    const char *leftWordText = docText.WordText( *leftWord );
    uint32_t leftWordFirstSymbol;
    offset = 0;
    U8_NEXT_UNSAFE( leftWordText, offset, leftWordFirstSymbol );
    if ( unlikely(offset > leftWord->Length()) )
        return LP_ERROR_EFAULT;

    // process right word
    const DocTextWord *rightWord = docText.Word( rightWordIdx );
    if ( unlikely(rightWord == 0) )
        return LP_ERROR_ERANGE;
    const char *rightWordText = docText.WordText( *rightWord );
    uint32_t rightWordFirstSymbol;
    offset = 0;
    U8_NEXT_UNSAFE( rightWordText, offset, rightWordFirstSymbol );
    if ( unlikely(offset > rightWord->Length()) )
        return LP_ERROR_EFAULT;

    if ( punctSymbol == static_cast<uint32_t>('!') ||
         punctSymbol == static_cast<uint32_t>('?') )
    {
        result = true;
        return LP_OK;
    }

    if ( spaceAfter ) // declarative sentence, splitted by space
    {
        if ( !quoteOnLeft && dicts.IsAbbr( docText, *leftWord ) ) // if left word is an abbreviation
        {
            if ( dicts.IsStopWord( docText, *rightWord ) && !charset->IsLower( rightWordFirstSymbol ) )
            {
                // if second word is in stop dict and isn't in lowercase, then assume point as end of the sentence
                result = true;
                return LP_OK;
            }
            else
            {
                // if second word is not in stop dict or is in lowercase, then we cannot assume point as end of the sentence
                result = false;
                return LP_OK;
            }
        }

        if ( !quoteOnRight && dicts.IsCompend( docText, *rightWord ) )
        {
            // if right word is the part of a compound word, then we cannot assume point as end of the sentence
            result = false;
            return LP_OK;
        }

        if ( is_number( leftWordText, leftWord->Length(), charset ) &&
             is_number( rightWordText, rightWord->Length(), charset) )
        {
            // if both words contains only digits, then we cannot assume point as end of the sentence
            result = false;
            return LP_OK;
        }

        result = true;
        return LP_OK;
    }
    else
    {
        uint32_t leftWordLastSymbol;
        offset = leftWord->Length();
        U8_PREV_UNSAFE( leftWordText, offset, leftWordLastSymbol );

        if ( charset->IsDigit( leftWordLastSymbol ) &&
             charset->IsDigit( rightWordFirstSymbol ) )
        {
            // if second word first symbol is a digit and first word last symbol is also a digit, then we cannot assume point as end of the sentence
            result = false;
            return LP_OK;
        }

        if ( !quoteOnLeft && dicts.IsAbbr( docText, *leftWord) )
        {
            // if first word is an abbreviation
            if ( dicts.IsStopWord( docText, *rightWord ) )
            {
                // if second word is not in stop dict, then we can assume point as the end of the sentence
                result = true;
                return LP_OK;
            }
            else
            {
                // if second word is in stop dict, then we cannot assume point as the end of the sentence
                result = false;
                return LP_OK;
            }
        }
        if ( !quoteOnRight && dicts.IsCompend( docText, *rightWord ) )
        {
            // if right word is the part of a compound word, then we cannot assume point as end of the sentence
            result = false;
            return LP_OK;
        }

        result = true; // in all rest cases we can assume point as the end of the sentence
        return LP_OK;
    }
}
#endif

LingProcErrors LingProc::PhraseBreakDoc( DocText &docText ) const
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;

    return shadow->PhraseBreakDoc( docText );
}

inline LingProcErrors modifyParagraph( DocText &docText, size_t i, size_t startPhraseIdx, size_t phrasesCount )
{
    DocTextPara *para = docText.Paragraph( i );
    if ( unlikely(para == 0) )
        return LP_ERROR_ERANGE;

    if ( startPhraseIdx == static_cast<size_t>(-1) )
        startPhraseIdx = para->StartPhraseIndex();
    if ( phrasesCount == static_cast<size_t>(-1) )
        phrasesCount = para->PhrasesCount();

    return para->Init( para->Offset(),
                       para->Length(),
                       para->Attrs(),
                       para->StartWordIndex(),
                       para->WordsCount(),
                       startPhraseIdx,
                       phrasesCount );
}

inline LingProcErrors modifyWord( DocText &docText, size_t i, uint16_t attrs )
{
    DocTextWord *word = docText.Word( i );
    if ( unlikely(word == 0) )
        return LP_ERROR_ERANGE;

    return word->Init( word->Offset(),
                       word->Length(),
                       word->Attrs() | attrs );
}

inline LingProcErrors appendPhrase( DocText  &docText,
                                    uint32_t  phraseOffset,
                                    uint32_t  phraseLength,
                                    uint16_t  phraseAttrs,
                                    size_t    phraseStartWordIdx,
                                    size_t    phraseWordsCount )
{
    DocTextWord *word = docText.Word( phraseStartWordIdx );
    if ( unlikely(word == 0) )
        return LP_ERROR_ERANGE;

    DocTextPhrase *phrase = docText.NewPhrase();
    if ( unlikely(phrase == 0) )
        return LP_ERROR_ENOMEM;

    return phrase->Init( phraseOffset,
                         phraseLength,
                         phraseAttrs,
                         phraseStartWordIdx,
                         phraseWordsCount );
}

LingProcErrors LingProcData::PhraseBreakDoc( DocText &docText ) const
{
    const UCharSet *charset = &genericCharset;

    // getting phrasebreak data
    const PhraseBreakerData *pbdata;
    if ( pbreakerDataIsExternal )
    {
        pbdata = pbreakerData;
        if ( unlikely(pbdata == 0) )
            return LP_ERROR_EFAULT;
    }
    else
        pbdata = &pbreakerDataInternal;

    // reset all if phrase breaking was already performed
    if ( docText.checkPhraseBreak() )
    {
        docText.setWordBreak( false );
        docText.setLineBreak( false );
        docText.setParaBreak( false );
        docText.setPhraseBreak( false );
        docText.ResetData();
    }

    LingProcErrors status;

    // ensure paragraphs break
    if ( !docText.checkParaBreak() )
    {
        status = ParagraphBreakDocLine( docText );
        if ( unlikely(status != LP_OK) )
            return status;
    }

    bool atParaBegin  = true;
    bool phraseBegin  = true;
    bool quoteOnLeft  = false;
    bool quoteOnRight = false;
    bool checkPhrase  = false;

    // index of current word
    size_t wordIdx = static_cast<size_t>(-1);

    // index of previous word, introduced due to markup
    size_t prevWordIdx = 0;

    // coordinates needed for end of sentence checking
    size_t leftWordIdx  = 0;
    size_t punctIdx     = 0;
    size_t rightWordIdx = 0;

    // coordinates of the begin of new phrase and the end of old phrase, introduced due to markup
    size_t potentialBeginIdx = 0;
    size_t potentialEndIdx   = 0;

    // current markup index, for speeding up word searching
    size_t markupIdx = 0;

    // index of current paragraph
    size_t paraIdx = 0;
    size_t prevParaIdx = 0;
    size_t paraIncr = 0;

    // new phrase construction
    uint32_t phraseOffset       = 0;
    uint32_t phraseLength       = 0;
    uint16_t phraseAttrs        = 0;
    size_t   phraseStartWordIdx = 0;
    size_t   phraseWordsCount   = 0;

    // initialize indicies
    paraIdx = GetNextWord( docText, paraIdx, wordIdx, markupIdx );

    while ( wordIdx < docText.WordsCount() && static_cast<ssize_t>( wordIdx ) >= 0 )
    {
        // Here we skip punctuation at the beginning of paragraph.
        // If we have entire paragraph, consisting of punctuation, we skip it.
        // Otherwise, all punctuation is included into sentence.

        if ( atParaBegin )
        {
            for ( size_t i = prevParaIdx; i < paraIdx; i++ )
            {
                status = modifyParagraph( docText, i, docText.PhrasesCount(), 0 );
                if ( unlikely(status != LP_OK) )
                    return status;
            }

            prevParaIdx = paraIdx;

            status = modifyParagraph( docText, paraIdx, docText.PhrasesCount(), static_cast<size_t>(-1) );
            if ( unlikely(status != LP_OK) )
                return status;

            phraseStartWordIdx = wordIdx;
            phraseOffset       = docText.WordOffset( wordIdx );
            phraseAttrs        = 0;

            for ( ;; )
            {
                uint32_t firstWordChar;
                size_t   offset = 0;
                U8_NEXT_UNSAFE( docText.WordText( wordIdx ), offset, firstWordChar );
                if ( unlikely(offset > docText.WordLength( wordIdx )) )
                    return LP_ERROR_EFAULT;

                if ( charset->IsWord( firstWordChar ) )
                {
                    atParaBegin = false;
                    phraseBegin = false;
                    break;
                }

                paraIncr     = GetNextWord( docText, prevParaIdx, wordIdx, markupIdx );
                prevParaIdx  = paraIdx;
                paraIdx     += paraIncr;

                if ( paraIncr != 0 )
                {
                    phraseBegin = true;
                    atParaBegin = true;
                    break;
                }

                if ( wordIdx >= docText.WordsCount() || wordIdx <= 0 )
                    break;
            }
        }
        else
        {
            if ( phraseBegin )
            {
                phraseBegin        = false;
                phraseStartWordIdx = potentialBeginIdx;
                phraseOffset       = docText.WordOffset( potentialBeginIdx );
                phraseAttrs        = 0;
            }

            quoteOnLeft       = false;
            quoteOnRight      = false;
            bool pointReached = false;
            // search [.!?]
            for ( ;; )
            {
                uint32_t ch;
                size_t   offset = 0;
                U8_NEXT_UNSAFE( docText.WordText( wordIdx ), offset, ch );
                if ( unlikely(offset > docText.WordLength( wordIdx )) )
                    return LP_ERROR_EFAULT;

                // detect point index and quote
                if ( !pointReached )
                {
                    if ( charset->IsWord( ch ) )
                    {
                        leftWordIdx = wordIdx; // potentially left word
                        quoteOnLeft = false;
                    }
                    else
                    {
                        ch = charset->ToLoPunct( ch );
                        if ( ch == static_cast<uint32_t>('!') ||
                             ch == static_cast<uint32_t>('?') ||
                             ch == static_cast<uint32_t>('.') )
                        {
                            punctIdx        = wordIdx;
                            potentialEndIdx = wordIdx;
                            pointReached    = true;
                        }
                        else
                        {
                            if ( ch == static_cast<uint32_t>('\"') ||
                                 ch == static_cast<uint32_t>('\'') )
                                quoteOnLeft = true;
                        }
                    }
                }
                else
                {
                    if ( !quoteOnRight )
                        potentialEndIdx = prevWordIdx;

                    if ( charset->IsWord( ch ) )
                    {
                        rightWordIdx = wordIdx; // right word
                        if ( !quoteOnRight )
                            potentialBeginIdx = wordIdx;
                        else
                        {
                            if (potentialBeginIdx != prevWordIdx ) // if there are divisors between quote and first word of a second sentence
                            {
                                potentialBeginIdx = rightWordIdx;
                                potentialEndIdx   = prevWordIdx;
                            }
                        }

                        checkPhrase = true;
                        break;
                    }
                    ch = charset->ToLoPunct( ch );

                    if ( ch == static_cast<uint32_t>('\"') ||
                         ch == static_cast<uint32_t>('\'') )
                    {
                        quoteOnRight      = true;
                        potentialBeginIdx = wordIdx;
                        potentialEndIdx   = prevWordIdx;
                    }
                }

                prevWordIdx  = wordIdx;
                paraIncr     = GetNextWord( docText, prevParaIdx, wordIdx, markupIdx );
                prevParaIdx  = paraIdx;
                paraIdx     += paraIncr;

                // here phrase at the end of paragraph
                if ( paraIncr != 0 ||
                     !(wordIdx < docText.WordsCount() && wordIdx > 0) )
                {
                    phraseWordsCount  = prevWordIdx - phraseStartWordIdx + 1;
                    phraseLength      = docText.WordOffset( prevWordIdx ) + docText.WordLength( prevWordIdx ) - phraseOffset;
                    phraseAttrs      |= LingProc::fWordIsLastInPara;

                    status = appendPhrase( docText,
                                           phraseOffset,
                                           phraseLength,
                                           phraseAttrs,
                                           phraseStartWordIdx,
                                           phraseWordsCount );
                    if ( unlikely(status != LP_OK) )
                        return status;

                    status = modifyWord( docText, prevWordIdx, LingProc::fWordIsLastInPhrase );
                    if ( unlikely(status != LP_OK) )
                        return status;

                    atParaBegin  = true;
                    phraseBegin  = true;

                    status = modifyParagraph( docText,
                                              prevParaIdx,
                                              static_cast<size_t>(-1),
                                              docText.PhrasesCount() - docText.ParagraphStartPhraseIndex( prevParaIdx ) );
                    if ( unlikely(status != LP_OK) )
                        return status;

                    for (size_t i = prevParaIdx + 1; i < paraIdx; i++ )
                    {
                        status = modifyParagraph( docText, i, docText.PhrasesCount(), 0 );
                        if ( unlikely(status != LP_OK) )
                            return status;
                    }
                    if ( paraIdx < docText.ParaCount() )
                    {
                        status = modifyParagraph( docText,
                                                  paraIdx,
                                                  docText.PhrasesCount(),
                                                  static_cast<size_t>(-1) );
                        if ( unlikely(status != LP_OK) )
                            return status;
                    }

                    prevParaIdx = paraIdx;
                    break;
                }
            }
        }

        if ( checkPhrase )
        {
            checkPhrase = false;
            bool result = false;

            status = CheckPhraseEnd( docText,
                                     charset,
                                     *pbdata,
                                     leftWordIdx,
                                     rightWordIdx,
                                     punctIdx,
                                     quoteOnLeft,
                                     quoteOnRight,
                                     result );
            if ( unlikely(status != LP_OK) )
                return status;

            if ( result )
            {
                phraseWordsCount = potentialEndIdx - phraseStartWordIdx + 1;
                phraseLength     = docText.WordOffset( potentialEndIdx ) + docText.WordLength( potentialEndIdx ) - phraseOffset;

                status = appendPhrase( docText,
                                       phraseOffset,
                                       phraseLength,
                                       phraseAttrs,
                                       phraseStartWordIdx,
                                       phraseWordsCount );
                if ( unlikely(status != LP_OK) )
                    return status;

                status = modifyWord( docText, potentialEndIdx, LingProc::fWordIsLastInPhrase );
                if ( unlikely(status != LP_OK) )
                    return status;

                phraseBegin = true;
            }
        }
    }

    // set flags
    docText.setPhraseBreak();

    return LP_OK;
}

