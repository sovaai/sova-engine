/**
 * @file   LPStrict.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 27 14:08:05 2009
 * 
 * @brief  LingProc strict (dictionary) morphology support.
 * 
 * 
 */
#ifndef _LPSTRICT_H_
#define _LPSTRICT_H_

#include <_include/language.h>

#include <lib/aptl/Functor.h>

#include "StrictMorphGram/StrictLex.h"

#include "BinaryData.h"
#include "LexID.h"

class LingProc;

namespace LPStrict
{
    struct FormsSetsRetriever
    {
        virtual StrictMorphErrors retrieveFormsSets( StrictLex::FormsSetFunctor &functor ) const = 0;
    };
    
    /* Dictionary information handler: received after lemmatization */
    class Info
    {
    public:
        Info( const StrictLex &_lex, FormsSetsRetriever &_formsSetsRetriever ) :
            lex( _lex ), additionalData(), formsSetsRetriever( _formsSetsRetriever )
        {
        }
        
    public:
        const StrictLex    &lex;
        BinaryData          additionalData;
        FormsSetsRetriever &formsSetsRetriever;
    };
    
    typedef Functor< const Info & > ResolverFunctor;
    class Resolver : public ResolverFunctor {};
    
    LEXID buildWithLexNo( const LingProc &lp, LangCodes lang, unsigned long lexNo );

    LexGram::PartOfSpeech getLexPartOfSpeech( const LingProc &lp, LEXID lex );
    LexGram::Subtype      getLexSubtype( const LingProc &lp, LEXID lex );
    LexGram::Suppl        getLexSuppl( const LingProc &lp, LEXID lex );
}


#endif /* _LPSTRICT_H_ */

