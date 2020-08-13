#ifndef _DOCIMAGE_H_
#define _DOCIMAGE_H_

#include <_include/language.h>
#include <_include/_inttype.h>

#include <lib/aptl/avector.h>

#include "../LexID.h"
#include "../Markup.h"
#include "../DocText/DocText.h"
#include "../StrictMorphGram/StrictWordFormsSet.h"

#include "DocImageFunctor.h"

class DocImageIterator;

class DocImage : public DocImageTypes
{
private:
    // copy prevention
    DocImage( const DocImage & ) { assert(false); abort(); }
    DocImage &operator=( const DocImage & ) { assert(false); abort(); return *this; }

public:
    DocImage() :
        image( 0, 3, 200 * 1024 ),
        words( 0, 3, 200 * 1024 ),
        compoundLexPool( 256, 2 ),
        compoundChainsPool( 64, 2 ),
        imageForms( 0, 2, 200 * 1024 ),
        compoundForms( 256, 2 ),
        markup(),
        primLang( LNG_UNKNOWN ),
        suppLang( LNG_UNKNOWN ),
        docText( 0 )
    {
    }
    
    ~DocImage() {}
  
    void Reset();

    size_t Size() const { return words.size(); }
    const LPMarkup &GetTextMarkup() const { return markup; }

    // simple image access
    LEXID  GetLex( size_t wordNo, size_t homoNo = 0 ) const;
    size_t GetHomoNumber( size_t wordNo ) const;

    // simple image access - low level, no size check
    const LEXID *GetWordImage( size_t wordNo ) const
    {
        return image.get_buffer() + words[ wordNo ].imgIndex;
    }
    
    const StrictWordFormsSet *GetWordForms( size_t wordNo ) const
    {
        if ( unlikely(imageForms.size() == 0) )
            return 0;
        return imageForms.get_buffer() + words[ wordNo ].imgIndex;
    }
    
    size_t GetWordImageLen( size_t wordNo ) const
    {
        return words[ wordNo ].imgLength;
    }
    
    uint16_t WordAttrs( size_t wordNo ) const
    {
        return words[ wordNo ].attrs;
    }

    // compound data access - low level, no size check
    size_t GetCompoundChainsNumber( size_t wordNo ) const
    {
        return words[ wordNo ].chainsCount;
    }
    
    size_t GetCompoundChainLength( size_t wordNo, size_t chainNo ) const
    {
        return compoundChainsPool[ words[ wordNo ].chainsIndex + chainNo ].chainLength;
    }

    LEXID  GetCompoundLex( size_t wordNo, size_t chainNo, size_t positionNo, size_t homoNo ) const;
    size_t GetCompoundHomoNumber( size_t wordNo, size_t chainNo, size_t positionNo ) const;
    const LEXID *GetCompoundWordImage( size_t wordNo, size_t chainNo, size_t positionNo ) const;
    const StrictWordFormsSet *GetCompoundWordForms( size_t wordNo, size_t chainNo, size_t positionNo ) const;

    // iterator access
    LEXID GetLex( const DocImageIterator &it, size_t homoNo = 0 ) const;
    size_t GetHomoNumber( const DocImageIterator &it ) const;
    size_t GetCompoundHomoNumber( const DocImageIterator &it ) const;
    const LEXID *GetLexPtr( const DocImageIterator &it, size_t homoNo ) const;
    const StrictWordFormsSet *GetWordForms( const DocImageIterator &it, size_t homoNo ) const;
    
    // attributes
    LangCodes      LangPrim() const   { return primLang; }
    LangCodes      LangSupp() const   { return suppLang; }
    const DocText *GetDocText() const { return docText; }
    void           SetDocText( const DocText *docText ) { this->docText = docText; }

    LingProcErrors ConvertMarkup( const DocText &docText )
    {
        return docText.MarkupOffsetToWords( markup );
    }
    
    void ConvertLanguages( const DocText &docText )
    {
        // set and adjust langs
        primLang = docText.getDetectedPrimLang();
        suppLang = docText.getDetectedSuppLang();

        // use lang detect suggestion
        if ( primLang == LNG_UNKNOWN )
        {
            primLang = docText.getPresetPrimLang();
            suppLang = docText.getPresetSuppLang();
        }
        else if ( suppLang == LNG_UNKNOWN )
        {
            if ( primLang != docText.getPresetPrimLang() )
                suppLang = docText.getPresetPrimLang();
            else
                suppLang = docText.getPresetSuppLang();
        }

        // adjust Russian
        if ( primLang  == LNG_UNKNOWN )
        {
            primLang = LNG_RUSSIAN;
            suppLang = LNG_ENGLISH;
        }
        else if ( primLang == LNG_RUSSIAN && suppLang == LNG_UNKNOWN )
            suppLang = LNG_ENGLISH;
    }

    void AssignPresetLanguages( const DocText &docText )
    {
        primLang = docText.getPresetPrimLang();
        suppLang = docText.getPresetSuppLang();
    }

    LingProcErrors PrepareReserve( const DocText &docText )
    {
        size_t wordsCount = docText.WordsCount();
        
        words.reserve( wordsCount );
        if ( unlikely(words.no_memory()) )
            return LP_ERROR_ENOMEM;

        image.reserve( wordsCount * 3 );
        if ( unlikely(image.no_memory()) )
            return LP_ERROR_ENOMEM;

        return LP_OK;
    }

    LemmatizeDocImageFunctor MakeLemmatizeFunctor( uint16_t wordAttrs, bool forceForms )
    {
        LemmatizeDocImageFunctor functor( image,
                                          words,
                                          compoundLexPool,
                                          compoundChainsPool,
                                          imageForms,
                                          compoundForms,
                                          wordAttrs,
                                          forceForms );
        return functor;
    }
    
protected:
    // PRIVATE DATA
    ImageArray      image;
    WordsArray      words;
    CompoundLexPool compoundLexPool;
    ChainsPool      compoundChainsPool;
    
    FormsSetsArray  imageForms;
    FormsSetsArray  compoundForms;

    LPMarkupArray   markup;

    LangCodes       primLang;
    LangCodes       suppLang;

    const DocText  *docText;
};

#endif /* _DOCIMAGE_H_ */

