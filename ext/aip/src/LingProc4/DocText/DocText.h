#ifndef _DOCTEXT_H_
#define _DOCTEXT_H_

#include <assert.h>

#include <_include/_string.h>
#include <_include/language.h>
#include <_include/CodePages.h>

#include <lib/aptl/avector.h>

#include "../UTFCanonizer.h"
#include "../MarkupAttr.h"
#include "../Markup.h"
#include "../UCharSet/UCharSet.h"

#include "DocTextBin.h"
#include "DocTextWord.h"
#include "DocTextPhrase.h"
#include "DocTextLine.h"
#include "DocTextPara.h"

class DocText : public DocTextBin
{
protected:
    // copy prevention
    DocText( const DocText & ) { assert(false); abort(); }
    DocText &operator=( const DocText & ) { assert(false); abort(); return *this; }

public:
    DocText() :
        words(),
        phrases(),
        lines(),
        paragraphs(),
        buffer(),
        markup(),
        markupAttrs(),
        wordsMarkup(),
        wordsMarkupAttrs(),
        wordsMarkupConverted( false ),
        allowEmptyMarkup( false ),
        wordBreak( false ),
        phraseBreak( false ),
        lineBreak( false ),
        paraBreak( false ),
        langDetect( false ),
        langSet( false ),
        detectedPrimLang( LNG_UNKNOWN ),
        detectedSuppLang( LNG_UNKNOWN ),
        presetPrimLang( LNG_UNKNOWN ),
        presetSuppLang( LNG_UNKNOWN ),
        primLang( LNG_UNKNOWN ),
        suppLang( LNG_UNKNOWN ),
        useLangDetector( true ),
        canonizer(),
        convertBuffer()
    {
    }
      
    // resetting
    void ResetFlagsDetectedLang()
    {
        setLangDetect( false );
        detectedPrimLang = LNG_UNKNOWN;
        detectedSuppLang = LNG_UNKNOWN;
    }

    void ResetFlagsLang()
    {
        setLangSet( false );
        primLang = LNG_UNKNOWN;
        suppLang = LNG_UNKNOWN;
    }
    
    virtual void ResetProcessing()
    {
        setWordBreak( false );
        setPhraseBreak( false );
        setLineBreak( false );
        setParaBreak( false );

        ResetFlagsLang();
        ResetFlagsDetectedLang();
        
        presetPrimLang   = LNG_UNKNOWN;
        presetSuppLang   = LNG_UNKNOWN;
        useLangDetector  = true;
    }

    enum ClearMode
    {
        CLEAR_NONE        = 0x00,
        CLEAR_WORDS_LINES = 0x01,
        CLEAR_PHRASES     = 0x02,
        CLEAR_PARAGRAPHS  = 0x04,
        CLEAR_ALL         = 0x07
    };
    
    virtual void ResetData( ClearMode mode = CLEAR_ALL )
    {
        if ( mode & CLEAR_WORDS_LINES )
        {
            words.clear();
            lines.clear();
        }
        if ( mode & CLEAR_PHRASES )
            phrases.clear();
        if ( mode & CLEAR_PARAGRAPHS )
            paragraphs.clear();
    }
    
    virtual void ResetAll()
    {
        ResetData();
        ResetProcessing();
        buffer.clear();
        markup.clear();
        markupAttrs.clear();
        wordsMarkup.clear();
        wordsMarkupAttrs.clear();
        wordsMarkupConverted = false;
    }

    virtual LingProcErrors AppendString(
        const char *string,
        size_t      size = static_cast<size_t>(-1),
        CodePages   stringCodepage = CPG_UTF8,
        bool        canonizeUtf = false );

    virtual LingProcErrors AssignString(
        const char *string,
        size_t      size = static_cast<size_t>(-1),
        CodePages   stringCodepage = CPG_UTF8,
        bool        canonizeUtf = false )
    {
        ResetAll();
        return AppendString( string, size, stringCodepage, canonizeUtf );
    }

    //Appends string in UTF-8, canonizes it, and saves coordinate mapping from original string to canonized one
    //also it has offset argument, which must contain size of the strings which were added before
    LingProcErrors AppendStringSaveCoords(
        const char *string,
        size_t      size,
        const UCharSet *ucs,
        UTFCanonizer::CoordMapping& mapping
    );

    virtual LingProcErrors ReserveWords()
    {
        size_t approxWordsCount = buffer.size() / 4;
        words.reserve( approxWordsCount );
        if ( unlikely(words.no_memory()) )
            return LP_ERROR_ENOMEM;

        return LP_OK;
    }
    
    virtual int AppendMarkup( const MarkupTag *markup, MarkupAttrArray *attributes = 0 );
    
    virtual int ImportMarkup( const MarkupTag *markup, MarkupAttrArray *attributes = 0, bool allowEmpty = false )
    {
        this->markup.clear();
        this->markupAttrs.clear();
        this->wordsMarkup.clear();
        this->wordsMarkupAttrs.clear();
        this->wordsMarkupConverted = false;
        this->allowEmptyMarkup = allowEmpty;
        return AppendMarkup( markup, attributes );
    }

    LingProcErrors MarkupOffsetToWords( LPMarkup &targetMarkup ) const;
    
protected:
    FieldsBits prepareSerialize() const;

public:
    virtual size_t serializeSize() const;
    virtual void   serialize( uint8_t *buffer ) const;
    
public:
    // length and size
    virtual size_t TextLength() const // text length in bytes
    {
        return buffer.size();
    }

    virtual const char *Text() const
    {
        return buffer.get_buffer();
    }
    
    virtual size_t WordsCount() const // number of words
    {
        return words.size();
    }

    virtual size_t PhrasesCount() const // number of phrases
    {
        return phrases.size();
    }
    
    virtual size_t LinesCount() const // number of lines
    {
        return lines.size();
    }
    
    virtual size_t ParaCount() const // number of paragraphs
    {
        return paragraphs.size();
    }

    // word access
    virtual const DocTextWord *Word( size_t n ) const
    {
        if ( unlikely(n >= words.size()) )
            return 0;
        return &words[ n ];
    }

    virtual DocTextWord *Word( size_t n )
    {
        if ( unlikely(n >= words.size()) )
            return 0;
        return &words[ n ];
    }

    virtual DocTextWord *NewWord()  { return words.grow();             }
    virtual LingProcErrors AddWord(const DocTextWord& w)
    {
        words.push_back(w);
        if( unlikely(words.no_memory()) )
            return LP_ERROR_ENOMEM;
        return LP_OK;
    }

    virtual DocTextWord *LastWord() { return Word( words.size() - 1 ); }
    
    virtual const char *WordText( size_t n ) const
    {
        const DocTextWord *word = Word( n );
        if ( unlikely(word == 0) )
            return 0;

        return WordText( *word );
    }

    virtual const char *WordText( const DocTextWord &word ) const
    {
        return buffer.get_buffer() + word.Offset();
    }
    
    virtual size_t WordLength( size_t n ) const
    {
        const DocTextWord *word = Word( n );
        if ( unlikely(word == 0) )
            return 0;

        return word->Length();
    }
    
    virtual uint16_t WordAttrs( size_t n ) const
    {
        const DocTextWord *word = Word( n );
        if ( unlikely(word == 0) )
            return 0;

        return word->Attrs();
    }

    virtual size_t WordOffset( size_t n ) const
    {
        const DocTextWord *word = Word( n );
        if ( unlikely(word == 0) )
            return 0;

        return word->Offset();
    }

    // phrase access
    virtual const DocTextPhrase *Phrase( size_t n ) const
    {
        if ( unlikely(n >= phrases.size()) )
            return 0;

        return &phrases[ n ];
    }
    
    virtual DocTextPhrase *Phrase( size_t n )
    {
        if ( unlikely(n >= phrases.size()) )
            return 0;

        return &phrases[ n ];
    }

    virtual DocTextPhrase *NewPhrase() { return phrases.grow(); }
    
    virtual const char *PhraseText( size_t n ) const
    {
        const DocTextPhrase *phrase = Phrase( n );
        if ( unlikely(phrase == 0) )
            return 0;

        return buffer.get_buffer() + phrase->Offset();
    }
        
    virtual size_t PhraseLength( size_t n ) const
    {
        const DocTextPhrase *phrase = Phrase( n );
        if ( unlikely(phrase == 0) )
            return 0;

        return phrase->Length();
    }
    
    virtual uint16_t PhraseAttrs( size_t n ) const
    {
        const DocTextPhrase *phrase = Phrase( n );
        if ( unlikely(phrase == 0) )
            return 0;

        return phrase->Attrs();
    }

    virtual size_t PhraseOffset( size_t n ) const
    {
        const DocTextPhrase *phrase = Phrase( n );
        if ( unlikely(phrase == 0) )
            return 0;

        return phrase->Offset();
    }

    virtual size_t PhraseStartWordIndex( size_t n ) const
    {
        const DocTextPhrase *phrase = Phrase( n );
        if ( unlikely(phrase == 0) )
            return 0;

        return phrase->StartWordIndex();
    }

    virtual size_t PhraseWordsCount( size_t n ) const
    {
        const DocTextPhrase *phrase = Phrase( n );
        if ( unlikely(phrase == 0) )
            return 0;

        return phrase->WordsCount();
    }
    
    // lines access
    virtual const DocTextLine *Line( size_t n ) const
    {
        if ( unlikely(n >= lines.size()) )
            return 0;

        return &lines[ n ];
    }

    virtual DocTextLine *Line( size_t n )
    {
        if ( unlikely(n >= lines.size()) )
            return 0;

        return &lines[ n ];
    }

    virtual DocTextLine *NewLine() { return lines.grow(); }
    virtual LingProcErrors AddLine(const DocTextLine& line)
    {
        lines.push_back(line);
        if( unlikely(lines.no_memory()) )
            return LP_ERROR_ENOMEM;
        return LP_OK;
    }
        
    virtual const char *LineText( size_t n ) const
    {
        const DocTextLine *line = Line( n );
        if ( unlikely(line == 0) )
            return 0;

        return buffer.get_buffer() + line->Offset();
    }
        
    virtual size_t LineLength( size_t n ) const
    {
        const DocTextLine *line = Line( n );
        if ( unlikely(line == 0) )
            return 0;

        return line->Length();
    }
    
    virtual uint16_t LineAttrs( size_t n ) const
    {
        const DocTextLine *line = Line( n );
        if ( unlikely(line == 0) )
            return 0;

        return line->Attrs();
    }

    virtual size_t LineOffset(size_t n) const
    {
        const DocTextLine *line = Line( n );
        if ( unlikely(line == 0) )
            return 0;

        return line->Offset();
    }

    virtual size_t LineStartWordIndex(size_t n) const
    {
        const DocTextLine *line = Line( n );
        if ( unlikely(line == 0) )
            return 0;

        return line->StartWordIndex();
    }

    virtual size_t LineWordsCount(size_t n) const
    {
        const DocTextLine *line = Line( n );
        if ( unlikely(line == 0) )
            return 0;

        return line->WordsCount();
    }
    
    // paragraphs access                                                                                                         
    virtual const DocTextPara *Paragraph( size_t n ) const
    {
        if ( unlikely(n >= paragraphs.size()) )
            return 0;

        return &paragraphs[ n ];
    }

    virtual DocTextPara *Paragraph( size_t n )
    {
        if ( unlikely(n >= paragraphs.size()) )
            return 0;

        return &paragraphs[ n ];
    }

    virtual DocTextPara *NewPara() { return paragraphs.grow(); }
    
    virtual const char *ParagraphText( size_t n ) const
    {
        const DocTextPara *para = Paragraph( n );
        if ( unlikely(para == 0) )
            return 0;

        return buffer.get_buffer() + para->Offset();
    }
        
    virtual size_t ParagraphLength( size_t n ) const
    {
        const DocTextPara *para = Paragraph( n );
        if ( unlikely(para == 0) )
            return 0;

        return para->Length();
    }
    
    virtual uint16_t ParagraphAttrs( size_t n ) const
    {
        const DocTextPara *para = Paragraph( n );
        if ( unlikely(para == 0) )
            return 0;

        return para->Attrs();
    }

    virtual size_t ParagraphOffset(size_t n) const
    {
        const DocTextPara *para = Paragraph( n );
        if ( unlikely(para == 0) )
            return 0;

        return para->Offset();
    }

    virtual size_t ParagraphStartWordIndex(size_t n) const
    {
        const DocTextPara *para = Paragraph( n );
        if ( unlikely(para == 0) )
            return 0;

        return para->StartWordIndex();
    }

    virtual size_t ParagraphWordsCount(size_t n) const
    {
        const DocTextPara *para = Paragraph( n );
        if ( unlikely(para == 0) )
            return 0;

        return para->WordsCount();
    }

    virtual size_t ParagraphStartPhraseIndex(size_t n) const
    {
        const DocTextPara *para = Paragraph( n );
        if ( unlikely(para == 0) )
            return 0;

        return para->StartPhraseIndex();
    }

    virtual size_t ParagraphPhrasesCount(size_t n) const
    {
        const DocTextPara *para = Paragraph( n );
        if ( unlikely(para == 0) )
            return 0;

        return para->PhrasesCount();
    }

    enum MarkupType
    {
        MARKUP_DEFAULT,
        MARKUP_WORDS
    };
    
    virtual const LPMarkup &GetTextMarkup( MarkupType type = MARKUP_DEFAULT ) const
    {
        switch ( type )
        {
        case MARKUP_WORDS:
            if ( !wordsMarkupConverted )
            {
                MarkupOffsetToWords( const_cast< LPMarkupArray & >( wordsMarkup ) );
                const_cast< DocText * >(this)->wordsMarkupConverted = true;
            }
            return wordsMarkup;

        case MARKUP_DEFAULT:
        default:
            return markup;
        };
    }
  
    virtual const LPMarkupAttrs &GetMarkupAttributes( MarkupType type = MARKUP_DEFAULT ) const
    {
        switch ( type )
        {
        case MARKUP_WORDS:
            if ( !wordsMarkupConverted )
            {
                MarkupOffsetToWords( const_cast< LPMarkupArray & >( wordsMarkup ) );
                const_cast< DocText * >(this)->wordsMarkupConverted = true;
            }
            return wordsMarkupAttrs;

        case MARKUP_DEFAULT:
        default:
            return markupAttrs;
        };
    }

public:
    // langs

    void PresetLangs( LangCodes primLang, LangCodes suppLang, bool useLangDetector )
    {
        this->presetPrimLang  = primLang;
        this->presetSuppLang  = suppLang;
        this->useLangDetector = useLangDetector;
    }

    void SetDetectedLangs( LangCodes detectedPrimLang, LangCodes detectedSuppLang, bool langDetect )
    {
        this->detectedPrimLang = detectedPrimLang;
        this->detectedSuppLang = detectedSuppLang;
        this->langDetect       = langDetect;
    }
    
    void InstallDetectedLangs()
    {
        primLang = detectedPrimLang;
        suppLang = detectedSuppLang;

        // use lang detect suggestion
        if ( primLang == LNG_UNKNOWN )
        {
            primLang = presetPrimLang;
            suppLang = presetSuppLang;
        }
        else if ( suppLang == LNG_UNKNOWN )
        {
            if ( primLang != presetPrimLang )
                suppLang = presetPrimLang;
            else
                suppLang = presetSuppLang;
        }

        // adjust Russian
        if ( primLang == LNG_UNKNOWN )
        {
            primLang = LNG_RUSSIAN;
            suppLang = LNG_ENGLISH;
        }
        else if ( primLang == LNG_RUSSIAN && suppLang == LNG_UNKNOWN )
            suppLang = LNG_ENGLISH;

        langSet = true;
    }
    
public:
    virtual DocText &setWordBreak( bool value = true )       { wordBreak       = value; return *this; }
    virtual DocText &setPhraseBreak( bool value = true )     { phraseBreak     = value; return *this; }
    virtual DocText &setLineBreak( bool value = true )       { lineBreak       = value; return *this; }
    virtual DocText &setParaBreak( bool value = true )       { paraBreak       = value; return *this; }
    DocText &setLangDetect( bool value = true )      { langDetect      = value; return *this; }
    DocText &setLangSet( bool value = true )         { langSet         = value; return *this; }
    DocText &setUseLangDetector( bool value = true ) { useLangDetector = value; return *this; }

    virtual bool checkWordBreak()       const { return wordBreak; }
    virtual bool checkPhraseBreak()     const { return phraseBreak; }
    virtual bool checkLineBreak()       const { return lineBreak; }
    virtual bool checkParaBreak()       const { return paraBreak; }
    bool checkLangDetect()      const { return langDetect; }
    bool checkLangSet()         const { return langSet; }
    bool checkUseLangDetector() const { return useLangDetector; }

    LangCodes getDetectedPrimLang() const { return detectedPrimLang; }
    LangCodes getDetectedSuppLang() const { return detectedSuppLang; }
    LangCodes getPresetPrimLang()   const { return presetPrimLang;   }
    LangCodes getPresetSuppLang()   const { return presetSuppLang;   }
    LangCodes getPrimLang()         const { return primLang;         }
    LangCodes getSuppLang()         const { return suppLang;         }
    
protected:
    DocTextWordSet    words;
    DocTextPhraseSet  phrases;
    DocTextLinesSet   lines;
    DocTextParaSet    paragraphs;

    avector< char >   buffer;
    
    LPMarkupArray  markup;
    LPMarkupAttrs  markupAttrs;
    LPMarkupArray  wordsMarkup;
    LPMarkupAttrs  wordsMarkupAttrs;
    bool           wordsMarkupConverted;
    bool           allowEmptyMarkup;
    
    bool wordBreak;
    bool phraseBreak;
    bool lineBreak;
    bool paraBreak;
    bool langDetect;
    bool langSet;

    LangCodes detectedPrimLang;
    LangCodes detectedSuppLang;
    LangCodes presetPrimLang;
    LangCodes presetSuppLang;
    LangCodes primLang;
    LangCodes suppLang;
      
    bool useLangDetector;

    UTFCanonizer    canonizer;
    avector< char > convertBuffer;
};

#endif /* _DOCTEXT_H_ */

