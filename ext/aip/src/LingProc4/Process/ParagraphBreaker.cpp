
#include <_include/cc_compat.h>

#include "../LingProc.h"
#include "../LingProcData.h"

LingProcErrors LingProc::ParagraphBreakDoc( DocText &docText, LingProc::ParaBreakMode mode ) const
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;
    
    // reset all if paragraph breaking was already performed
    if ( docText.checkParaBreak() )
    {
        docText.setWordBreak( false );
        docText.setLineBreak( false );
        docText.setParaBreak( false );
        docText.setPhraseBreak( false );
        docText.ResetData();
    }
    
    // perform word & line break first if need to 
    if ( !(docText.checkWordBreak() && docText.checkLineBreak()) )
    {
        LingProcErrors status = WordBreakDoc( docText );
        if ( unlikely(status != LP_OK) )
            return status;
    }
                
    switch ( mode )
    {
    case PARA_BREAK_FIT_LINE:     return shadow->ParagraphBreakDocLine( docText );
    case PARA_BREAK_SMART_DETECT: return shadow->ParagraphBreakDocSmart( docText );
    };

    return LP_ERROR_EINVAL;
}

LingProcErrors LingProcData::ParagraphBreakDocLine( DocText &docText ) const
{
    // loop over lines
    for ( size_t j = 0; j < docText.LinesCount(); j++ )
    {
        const DocTextLine *lineRef = docText.Line( j );
        if ( unlikely(lineRef == 0) )
            return LP_ERROR_ERANGE;
        const DocTextLine &line = *lineRef;

        DocTextPara *para = docText.NewPara();
        if ( unlikely(para == 0) )
            return LP_ERROR_ENOMEM;

        LingProcErrors status = para->Init( line.Offset(),         // offset
                                            line.Length(),         // length
                                            0,                     // attrs
                                            line.StartWordIndex(), // startWordIdx
                                            line.WordsCount(),     // wordsCount
                                            0,                     // startPhraseIdx
                                            0                      // phrasesCount
            );
        if ( unlikely(status != LP_OK) )
            return status;

        if ( likely(line.WordsCount() > 0) )
        {
            DocTextWord *wordRef = docText.Word( line.StartWordIndex() + line.WordsCount() - 1 );
            if ( unlikely(wordRef == 0) )
                return LP_ERROR_ERANGE;
            DocTextWord &word = *wordRef;
            
            status = word.Init( word.Offset(),
                                word.Length(),
                                word.Attrs() | LingProc::fWordIsLastInPara );
            if ( unlikely(status != LP_OK) )
                return status;
        }
    }
    
    // set flags
    docText.setParaBreak();
    
    return LP_OK;
}

LingProcErrors LingProcData::ParagraphBreakDocSmart( DocText &docText ) const
{
    // TODO: nothing yet, just dummy for ParagraphBreakDocLine
    return ParagraphBreakDocLine( docText );
}
