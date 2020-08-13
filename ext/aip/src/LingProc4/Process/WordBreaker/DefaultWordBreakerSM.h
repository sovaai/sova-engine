#ifndef _DEFAULTWORDBREAKERSM_H_
#define _DEFAULTWORDBREAKERSM_H_

#include <_include/cc_compat.h>

#include <LingProc4/LingProc.h>
#include "../../LingProcErrors.h"
#include "../../UCharSet/UCharSet.h"
#include "../../DocText/DocTextWord.h"

template< typename OwnerClass >
class DefaultWordBreakerSM
{
public:
    DefaultWordBreakerSM( const UCharSet &_genericCharset ) :
        genericCharset( _genericCharset ),
        wbState( WORD_BREAK_START ),
        wordStart( 0 ),
        wordEnd( 0 ),
        wordAttrs( 0 ),
        wordCreated( false ),
        spacePrev( false ),
        spaceCurr( false ),
        needFlush( false ),
        wordCurr( &wordA ),
        wordPrev( &wordB ),
        wordA(),
        wordB(),
        punctuationAtEnd(&firstPostionStorage),
        punctuationAtPrevEnd(&secondPostionStorage)
    {
    }

    void reset()
    {
        wbState                     =   WORD_BREAK_START;
        wordStart                   =   0;
        wordEnd                     =   0;
        wordAttrs                   =   0;
        wordCreated                 =   false;
        spacePrev                   =   false;
        spaceCurr                   =   false;
        needFlush                   =   false;
        wordCurr                    =   &wordA;
        wordPrev                    =   &wordB;
        punctuationAtEnd->clear();
        punctuationAtPrevEnd->clear();
    }

private:
    enum WordBreakState
    {
        WORD_BREAK_START,
        WORD_BREAK_WORD,
        WORD_BREAK_SYM,
        WORD_BREAK_DONE
    };

public:
    LingProcErrors nextChar( size_t current, uint32_t ch )
    {
        LingProcErrors  status;
        DocTextWord    *tmpWord;

        spaceCurr = genericCharset.IsSpace( ch );

        // word construction
        for ( ;; )
        {
            switch ( wbState )
            {
            case WORD_BREAK_START:
                wordStart = current;

                wordAttrs = 0;
                if ( spacePrev )
                    wordAttrs |= LingProc::fWordHasSpaceBefore;

                if ( wordCreated )
                {
                    wordCreated = false;
                    if ( spaceCurr )
                    {
                        status = wordPrev->Init( wordPrev->Offset(),
                                                 wordPrev->Length(),
                                                 wordPrev->Attrs() | LingProc::fWordHasSpaceAfter );
                        if ( unlikely(status != LP_OK) )
                            return status;
                    }
                }

                if ( ch == static_cast<uint32_t>('\n') )
                {
                    status = wordPrev->Init( wordPrev->Offset(),
                                             wordPrev->Length(),
                                             wordPrev->Attrs() | LingProc::fWordHasNewLineAfter );
                    if ( unlikely(status != LP_OK) )
                        return status;
                    break;
                }

                if ( ch == static_cast<uint32_t>(-1) )
                    break;

                if ( genericCharset.IsWordIgnore( ch ) )
                    break;

                if ( spaceCurr )
                    break;

                //words do not start wth punctuation
                if ( genericCharset.IsWord( ch ) && !genericCharset.IsInternalOnly( ch ) )
                    wbState = WORD_BREAK_WORD; // starting alphanumeric word
                else
                    wbState = WORD_BREAK_SYM; // starting punctuation or symbol

                continue;

            case WORD_BREAK_WORD:
                if ( !genericCharset.IsWord( ch ) )
                {
                    wbState = WORD_BREAK_DONE;
                    continue;
                }

                if ( genericCharset.IsInternalOnly( ch ) ) {
                    punctuationAtEnd->push_back(current);
                }
                else {
                    punctuationAtEnd->clear();
                }

                break;

            case WORD_BREAK_SYM:
                wbState = WORD_BREAK_DONE; // read exactly one symbol
                break;

            case WORD_BREAK_DONE:
                wordEnd = current;

                status = wordCurr->Init( static_cast<uint32_t>(wordStart),
                                         static_cast<uint16_t>(wordEnd - wordStart),
                                         wordAttrs );
                if ( unlikely(status != LP_OK) )
                    return status;
                wordCreated = true;
                wbState     = WORD_BREAK_START;
                status      = flush();
                if ( unlikely(status != LP_OK) ) {
                    return status;
                }

                // switch current and previous word
                tmpWord                     =   wordCurr;
                wordCurr                    =   wordPrev;
                wordPrev                    =   tmpWord;
                needFlush                   =   true;

                TPositions* swap    =   NULL;

                swap                    =   punctuationAtPrevEnd;
                punctuationAtPrevEnd    =   punctuationAtEnd;
                punctuationAtEnd        =   swap;
                punctuationAtEnd->clear();

                continue;
            };

            break;
        }

        spacePrev = spaceCurr;

        return LP_OK;
    }

    LingProcErrors flush()
    {
        static const uint16_t   kNotAfterAttrs  =   LingProc::fWordHasSpaceBefore;

        if ( !needFlush ) {
            return LP_OK;
        }

        OwnerClass& reciever(*static_cast<OwnerClass *>(this));

        if ( true == punctuationAtPrevEnd->empty() ) {
            return reciever.receiveWord( *wordPrev );
        }


        DocTextWord     woTailPunctuation;
        woTailPunctuation.Init( wordPrev->Offset()
                              , (*punctuationAtPrevEnd)[0] - wordPrev->Offset()
                              , wordPrev->Attrs() & kNotAfterAttrs );
        LingProcErrors  status  =   reciever.receiveWord( woTailPunctuation );

        if ( LP_OK != status ) {
            return status;
        }

        //return all punctuation at end of word
        size_t  punctCount  =   punctuationAtPrevEnd->size();
        size_t  punctLast   =   punctCount - 1;
        size_t  punctIdx    =   0;

        while ( ( LP_OK == status ) && ( punctCount != punctIdx ) ) {
            DocTextWord punctWord;

            uint16_t    attrs   =   wordPrev->Attrs() & ~LingProc::fWordHasSpaceBefore;

            if (punctLast != punctIdx) {
                attrs   =   attrs & kNotAfterAttrs;
            }

            size_t  punctStart  =   (*punctuationAtPrevEnd)[punctIdx];
            size_t  punctEnd    =   wordPrev->Offset() + wordPrev->Length();

            if (punctLast != punctIdx) {
                punctEnd    =   (*punctuationAtPrevEnd)[punctIdx + 1];
            }

            punctWord.Init( punctStart
                          , punctEnd - punctStart
                          , attrs );

            status      =   reciever.receiveWord( punctWord );
            punctIdx    +=  1;
        }


        size_t  punctLastStart  =   (*punctuationAtPrevEnd)[punctLast];
        wordPrev->Init( punctLastStart
                      , wordPrev->Offset() + wordPrev->Length() - punctLastStart
                      , wordPrev->Attrs() & ~LingProc::fWordHasSpaceBefore );

        punctuationAtPrevEnd->clear();

        return status;
    }

private:
    const UCharSet &genericCharset;
    WordBreakState  wbState;
    size_t          wordStart;
    size_t          wordEnd;
    uint16_t        wordAttrs;
    bool            wordCreated;
    bool            spacePrev;
    bool            spaceCurr;
    bool            needFlush;
    DocTextWord    *wordCurr;
    DocTextWord    *wordPrev;
    DocTextWord     wordA;
    DocTextWord     wordB;

    typedef         avector<size_t> TPositions;
    TPositions*     punctuationAtEnd;
    TPositions*     punctuationAtPrevEnd;
    TPositions      firstPostionStorage;
    TPositions      secondPostionStorage;
};

#endif /* _DEFAULTWORDBREAKERSM_H_ */

