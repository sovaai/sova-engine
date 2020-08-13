#ifndef _LIBBUCKWALTER_LPARABICMORPH_H_
#define _LIBBUCKWALTER_LPARABICMORPH_H_
#include "morphology.h"
#include <LingProc4/SimpleMorph/LPMorphInterface.h>
#include <LingProc4/WordNormalizerInterface.h>

static const size_t kLengthUnknown  =   static_cast<size_t>( -1 );

class TLPArabicMorph : public LPMorphInterface {
public:
    TLPArabicMorph();
    LingProcErrors Init( const UCharSet *charset, const LPMorphDictInfo &_info );
    LingProcErrors ReadInfo( const UCharSet* charset
                           , fstorage* fs
                           , fstorage_section_id sectionId );
    LingProcErrors create( const UCharSet *charset, const LPMorphCreateParams *params );
    LingProcErrors FSSave( fstorage* fs
                         , fstorage_section_id sectionId
                         , bool transport );
    LingProcErrors FSOpenRW( const UCharSet* charset
                           , fstorage* fs
                           , fstorage_section_id sectionId );
    LingProcErrors FSOpenRO( const UCharSet* charset
                           , fstorage* fs
                           , fstorage_section_id sectionId );
    LingProcErrors close();
    LingProcErrors flush()
    {
        //we do not allocate memory
        return LP_OK;
    }

    LingProcErrors FSAttachStrictMorph( fstorage* fs );

    int lemmatize( LemmatizeResultFunctor &functor
                 , const char             *word
                 , size_t                 wordLength = kLengthUnknown
                 , unsigned int           flags = 0 );

    int stemmatize( StemmatizeResultFunctor &functor
                  , const char              *word
                  , size_t                  wordLength = kLengthUnknown
                  , unsigned int            flags = 0 );
    int normalize( NormalizeResultFunctor &functor
                 , const char              *word
                 , size_t                  wordLength = kLengthUnknown
                 , unsigned int            flags = 0 );
    int getLexText( LEXID lex, char *buffer, size_t bufferSize, const BinaryData *additionalData = 0 );
    int getLexText( LEXID partialLex, const char *stem, char *buffer, size_t bufferSize, const BinaryData *additionalData = 0 );
    LingProcErrors enumerate( MorphEnumerator &enumerator );
    size_t size();
    static const TBundledForms::TIndexShift     KDictionaryFormCommand;

    typedef avector<TBundledForms::TIndexShift> TFormMapper;
private:
    int getLexText(TBundledStemsPacks::TShift lexemeId, TBundledForms::TIndexShift formId, char *buffer, size_t bufferSize);
    TBuckwalterMorphology   Morphology;
    uint16_t                LangInfo;
    //sometimes lingproc user want to get dictionary form for word not used in processed text
    //e.g. dictionary form for "кнопку" is "кнопка"
    //not always first form it is a dictionary form
    //e.g. first form for "ножницы" is "ножница", but this is not dictionary form
    //so lingproc user send special form id (as command) to get exactly dictionary form
    //in arabic we have form ids which are overiden by this command
    //so here it is workaround
    TFormMapper InternalToExternal;
    TFormMapper ExternalToInternal;

    //as fstorage use deffered write
    //we need to store header in some long lived buffer
    avector<uint32_t>           HeaderStorage;

    TUniqPtr<WordNormalizer>    Normalizer;
};

#endif /* _LIBBUCKWALTER_LPARABICMORPH_H_ */
