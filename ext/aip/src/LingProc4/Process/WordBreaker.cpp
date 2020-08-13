
#include <_include/cc_compat.h>

#include "../LingProc.h"
#include "../LingProcData.h"

#include "WordBreaker/DefaultWordBreakerSM.h"
#include "WordBreaker/DefaultLineBreakerSM.h"
#include "WordBreaker/SequenceSM.h"
#include "WordBreaker/RunSM.h"

class DocTextFiller
{
public:
    DocTextFiller() : docText( 0 ), wordsCounter( 0 ), linesCounter( 0 ) { }
    
    void setDocText( DocText &docText ) { this->docText = &docText; }
    
    LingProcErrors receiveWord( const DocTextWord &word )
    {
        if ( unlikely(docText == 0) )
            return LP_ERROR_EFAULT;

        if( unlikely(docText->AddWord(word)!=LP_OK) )
            return LP_ERROR_ENOMEM;

        wordsCounter++;
        return LP_OK;
    }

    LingProcErrors receiveLine( const DocTextLine &line )
    {
        if ( unlikely(docText == 0) )
            return LP_ERROR_EFAULT;

        if ( unlikely(docText->AddLine(line)!=LP_OK ) )
            return LP_ERROR_ENOMEM;

        linesCounter++;
        return LP_OK;
    }

    LingProcErrors flushDocText()
    {        
        // mark words to lines belonging and word 'last-in-line' attributes

        LingProcErrors  status;
        size_t          wordIdx      = 0;
        size_t          lineIdx      = 0;
        DocTextWord    *prevWord     = 0;
        DocTextWord    *currWord     = 0;
        DocTextLine    *currLine     = 0;
        size_t          startWordIdx = 0;
        
        for ( ; lineIdx < docText->LinesCount(); lineIdx++, startWordIdx = wordIdx )
        {
            currLine = docText->Line( lineIdx );

            for ( ; wordIdx <= docText->WordsCount(); wordIdx++, prevWord = currWord )
            {
                currWord = 0;
                if ( wordIdx < docText->WordsCount() )
                    currWord = docText->Word( wordIdx );
                if ( currWord == 0 || currWord->Offset() >= currLine->Offset() + currLine->Length() )
                    break;
            }

            if ( prevWord != 0 )
            {
                status = prevWord->Init( prevWord->Offset(),
                                         prevWord->Length(),
                                         prevWord->Attrs() | LingProc::fWordIsLastInLine );
                if ( unlikely(status != LP_OK) )
                    return status;
            }

            status = currLine->Init( currLine->Offset(),
                                     currLine->Length(),
                                     currLine->Attrs(),
                                     startWordIdx,
                                     wordIdx - startWordIdx );
            if ( unlikely(status != LP_OK) )
                return status;
        }

        docText = 0;
        return LP_OK;
    }

private:
    DocText *docText;
    size_t   wordsCounter;
    size_t   linesCounter;
};

class DocTextFillerWordReceiver : public DocTextFiller,
                                  public DocTextWordBreaker::WordReceiver
{
public:
    DocTextFillerWordReceiver() : DocTextFiller(), DocTextWordBreaker::WordReceiver() { }

    LingProcErrors apply( const DocTextWord &word )
    {
        return receiveWord( word );
    }
};

class WordLineSM : public RunSM< SequenceSM2< DefaultWordBreakerSM< WordLineSM >,
                                              DefaultLineBreakerSM< WordLineSM > > >,
                   public DocTextFiller
{
public:
    WordLineSM( const UCharSet &_genericCharset ) :
        RunSM< SequenceSM2< DefaultWordBreakerSM< WordLineSM >,
                            DefaultLineBreakerSM< WordLineSM > > >( _genericCharset ),
        DocTextFiller()
    {
    }
};

class CustomWordBreakerLineSM : public RunSM< DefaultLineBreakerSM< CustomWordBreakerLineSM > >,
                                public DocTextFillerWordReceiver
{
public:
    CustomWordBreakerLineSM( const UCharSet &_genericCharset ) :
        RunSM< DefaultLineBreakerSM< CustomWordBreakerLineSM > >( _genericCharset ),
        DocTextFillerWordReceiver(),
        customBreaker( 0 )
    {
    }

    void setCustomBreaker( DocTextWordBreaker &customBreaker ) { this->customBreaker = &customBreaker; };
    
    LingProcErrors run( const char *text, size_t textLength )
    {
        if ( unlikely(customBreaker == 0) )
            return LP_ERROR_EFAULT;
        
        LingProcErrors status = customBreaker->performWordBreaking( text, textLength, *this );
        if ( unlikely(status != LP_OK) )
            return status;

        return RunSM< DefaultLineBreakerSM< CustomWordBreakerLineSM > >::run( text, textLength );
    }
    
private:
    DocTextWordBreaker *customBreaker;
};

class CustomWordReceiverWordSM : public RunSM< DefaultWordBreakerSM< CustomWordReceiverWordSM > >
{
public:
    CustomWordReceiverWordSM( const UCharSet &_genericCharset ) :
        RunSM< DefaultWordBreakerSM< CustomWordReceiverWordSM > >( _genericCharset ),
        receiver( 0 )
    {
    }

    void setWordReceiver( DocTextWordBreaker::WordReceiver &receiver ) { this->receiver = &receiver; }
    void setDocText( DocText & /* docText */ ) { }
    LingProcErrors flushDocText() { return LP_OK; }
    
    LingProcErrors receiveWord( const DocTextWord &word )
    {
        if ( unlikely(receiver == 0) )
            return LP_ERROR_EFAULT;
        
        return receiver->apply( word );
    }

private:
    DocTextWordBreaker::WordReceiver *receiver;
};

template< typename CustomSM >
class TextWordBreaker : public CustomSM
{
public:
    TextWordBreaker( const UCharSet &_genericCharset ) : CustomSM( _genericCharset ) { }

    LingProcErrors run( DocText &docText )
    {
        const char  *text       = docText.Text();
        size_t       textLength = docText.TextLength();

        LingProcErrors status = docText.ReserveWords();
        if ( unlikely(status != LP_OK) )
            return status;
            
        CustomSM::setDocText( docText );

        status = CustomSM::run( text, textLength );
        if ( unlikely(status != LP_OK) )
            return status;

        status = CustomSM::flushDocText();
        if ( unlikely(status != LP_OK) )
            return status;

        return LP_OK;
    }
};

LingProcErrors LingProc::WordBreakDoc( DocText &docText ) const
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;

    shadow->WordBreakDocPrepare( docText );
    
    TextWordBreaker< WordLineSM > specificBreaker( *shadow->getGenericCharset() );
    
    LingProcErrors status = specificBreaker.run( docText );
    if ( unlikely(status != LP_OK) )
        return status;
    
    return shadow->WordBreakDocFinish( docText );
}

LingProcErrors LingProc::WordBreakDoc( DocText &docText, DocTextWordBreaker &breaker ) const
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;

    shadow->WordBreakDocPrepare( docText );
    
    TextWordBreaker< CustomWordBreakerLineSM > specificBreaker( *shadow->getGenericCharset() );
    
    specificBreaker.setCustomBreaker( breaker );

    LingProcErrors status = specificBreaker.run( docText );
    if ( unlikely(status != LP_OK) )
        return status;
    
    return shadow->WordBreakDocFinish( docText );
};

LingProcErrors LingProc::WordBreakDoc( DocText &docText, DocTextWordBreaker::WordReceiver &wordReceiver ) const
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;

    TextWordBreaker< CustomWordReceiverWordSM > specificBreaker( *shadow->getGenericCharset() );
    
    specificBreaker.setWordReceiver( wordReceiver );

    return specificBreaker.run( docText );
}

void LingProcData::WordBreakDocPrepare( DocText &docText ) const
{
    // clear break data (do not clear detected langs)
    docText.setWordBreak( false );
    docText.setLineBreak( false );
    docText.setParaBreak( false );
    docText.setPhraseBreak( false );
    docText.ResetData();
}

LingProcErrors LingProcData::WordBreakDocFinish( DocText &docText ) const
{
    docText.setWordBreak().setLineBreak();

    DocTextWord *lastWord = docText.LastWord();
    if ( lastWord != 0 )
        return lastWord->Init( lastWord->Offset(),
                               lastWord->Length(),
                               lastWord->Attrs() | LingProc::fWordIsLast );
    
    return LP_OK;
}

