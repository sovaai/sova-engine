#ifndef _PHRASEBREAKERDICT_H_
#define _PHRASEBREAKERDICT_H_

#include "../LingProcDict.h"
#include "../Constants.h"
#include "../StringsReceiver.h"
#include "../WordNormalizerInterface.h"

#include "../DocText/DocText.h"

class PhraseBreakerDict : public LingProcDict
{
public:
    enum Type
    {
        PBDICT_ABBR     = 0,
        PBDICT_COMPEND  = 1,
        PBDICT_STOPWORD = 2
    };
    
public:
    PhraseBreakerDict(): LingProcDict(), normalizedStorage() {}
    ~PhraseBreakerDict() { }
    
    void Clear();
    LingProcErrors InitEmpty( const UCharSet* charset );
    LingProcErrors Init( Type dictType, WordNormalizer *normalizer, const UCharSet *charset );
    LingProcErrors Init( const char * const *words, const UCharSet *charset)
    {
        return LingProcDict::Init( words, charset );
    }
    
    /* returns 0 - not found, 1 - found, LingProcErrors - on errors */
    int CheckWord( const char *word, size_t len ) const;
    LingProcErrors AppendDict( const char * const *words, WordNormalizer *normalizer, uint32_t normalizerFlags, const UCharSet *charset, bool savemem );
private:
    // copy prevention
    PhraseBreakerDict( const PhraseBreakerDict &src ) : LingProcDict( src ) { }
    PhraseBreakerDict &operator=(const PhraseBreakerDict &) { assert(false); abort(); return *this; }

private:
    MultipleStringsReceiver normalizedStorage; // normalized versions of dictionaries
};

class PhraseBreakerData
{
public:
    PhraseBreakerData() : abbrs(), compends(), stopwords(), normalizer( 0 ) { }
    ~PhraseBreakerData() { Clear(); }
 
    void Clear()
    {
        abbrs.Clear();
        compends.Clear();
        stopwords.Clear();
        if ( normalizer != 0 )
        {
            delete normalizer;
            normalizer = 0;
        }
    }
    
    LingProcErrors Init( const UCharSet *charset );
    LingProcErrors Load( fstorage *fs, fstorage_section_id sec_id, const UCharSet *charset );
    LingProcErrors Save( fstorage *fs, fstorage_section_id sec_id );

    int IsAbbr( const DocText &docText, const DocTextWord &word ) const
    {
        return CheckWordNormalized( docText, word, abbrs );
    }

    int IsCompend( const DocText &docText, const DocTextWord &word ) const
    {
        return CheckWordNormalized( docText, word, compends );
    }

    int IsStopWord( const DocText &docText, const DocTextWord &word ) const
    {
        return CheckWordNormalized( docText, word, stopwords );
    }

private:
    // copy prevention
    PhraseBreakerData(const PhraseBreakerData&) { assert(false); abort(); }
    PhraseBreakerData& operator = (const PhraseBreakerData&) { assert(false); abort(); return *this; }

    int CheckWordNormalized( const DocText &docText, const DocTextWord &word, const PhraseBreakerDict &dict ) const;

protected:
    PhraseBreakerDict  abbrs;
    PhraseBreakerDict  compends;
    PhraseBreakerDict  stopwords;
    WordNormalizer    *normalizer;
};

#endif /* _PHRASEBREAKERDICT_H_ */

