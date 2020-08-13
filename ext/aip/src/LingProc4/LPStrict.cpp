/**
 * @file   LPStrict.cpp
 * @author swizard <me@swizard.info>
 * @date   Thu May 28 03:16:08 2009
 * 
 * @brief  LingProc strict (dictionary) morphology support (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include "LPStrict.h"
#include "LingProc.h"
#include "LingProcData.h"

LEXID LPStrict::buildWithLexNo( const LingProc &lp, LangCodes lang, unsigned long lexNo )
{
    LEXID lex = LEXINVALID;

    unsigned int morphNo;
    LingProcErrors status = lp.MorphNo( lang, MORPH_TYPE_STRICT, morphNo );
    if ( unlikely(status != LP_OK) )
        return lex;
    
    lex = static_cast<LEXID>(lexNo);
    lex = LexSetNoMrph( lex, morphNo );
    lex = LexSetNoCase( lex );
    lex = LexSetForm( lex, 0 );

    return lex;
}

LingProcErrors LingProc::resolveLPStrict( LEXID lex, LPStrict::Resolver &resolver ) const
{
    // check mode
    if ( unlikely(shadow == 0) )
        return LP_ERROR_INVALID_MODE;
    if ( unlikely(shadow->isClosed()) )
        return LP_ERROR_INVALID_MODE;

    if ( lex == LEXNONE || lex == LEXINVALID )
        return LP_ERROR_INVALID_LEXID;

    const LPMorphInterface *mp = shadow->getMorph( LexNoMrph(lex) );
    if ( unlikely(mp == 0) )
        return LP_ERROR_UNSUPPORTED_MORPH;
    
    return mp->resolveLPStrict( lex, resolver );
}

const StrictDictInfo *LingProc::getStrictDictInfo( LangCodes lang ) const
{
    unsigned int morphNo;
    LingProcErrors status = MorphNo( lang, MORPH_TYPE_STRICT, morphNo );
    if ( unlikely(status != LP_OK) )
        return 0;

    const LPMorphInterface *mp = shadow->getMorph( morphNo );
    if ( unlikely(mp == 0) )
        return 0;

    return mp->getStrictDictInfo();
}

template<typename GramType> struct GramAttrExtractor { GramType extractGram( const LexGram &gram ) const; };

template<> struct GramAttrExtractor<LexGram::PartOfSpeech>
{ LexGram::PartOfSpeech extractGram( const LexGram &gram ) const { return gram.getPartOfSpeech(); } };

template<> struct GramAttrExtractor<LexGram::Subtype>
{ LexGram::Subtype extractGram( const LexGram &gram ) const { return gram.getSubtype(); } };

template<> struct GramAttrExtractor<LexGram::Suppl>
{ LexGram::Suppl extractGram( const LexGram &gram ) const { return gram.getSuppl(); } };

template<typename GramType, int lastValue>
class SpecificGramResolver : public LPStrict::Resolver,
                             public StrictLex::GramEnumFunctor,
                             public GramAttrExtractor<GramType>
{
public:
    SpecificGramResolver() : result( static_cast<GramType>(lastValue) ) { }
        
    int apply( const LPStrict::Info &info )
    {
        StrictMorphErrors status = info.lex.queryLexGrams( *this );
        if ( unlikely(status != STRICT_OK && status != STRICT_ERROR_ENUMERATION_STOPPED) )
            return 1;
        return 0;
    }

    StrictMorphErrors apply( const LexGram &gram )
    {
        result = GramAttrExtractor<GramType>::extractGram( gram );
        return STRICT_ERROR_ENUMERATION_STOPPED;
    }
        
    GramType getResult() const { return result; }
        
private:
    GramType result;
};

LexGram::PartOfSpeech LPStrict::getLexPartOfSpeech( const LingProc &lp, LEXID lex )
{
    SpecificGramResolver<LexGram::PartOfSpeech, LexGram::PART_LAST> resolver;
    LingProcErrors status = lp.resolveLPStrict( lex, resolver );
    if ( unlikely(status != LP_OK) )
        return LexGram::PART_LAST;
    
    return resolver.getResult();
}

LexGram::Subtype LPStrict::getLexSubtype( const LingProc &lp, LEXID lex )
{
    SpecificGramResolver<LexGram::Subtype, LexGram::SUBTYPE_LAST> resolver;
    LingProcErrors status = lp.resolveLPStrict( lex, resolver );
    if ( unlikely(status != LP_OK) )
        return LexGram::SUBTYPE_LAST;
    
    return resolver.getResult();
}

LexGram::Suppl LPStrict::getLexSuppl( const LingProc &lp, LEXID lex )
{
    SpecificGramResolver<LexGram::Suppl, LexGram::SUPPL_LAST> resolver;
    LingProcErrors status = lp.resolveLPStrict( lex, resolver );
    if ( unlikely(status != LP_OK) )
        return LexGram::SUPPL_LAST;
    
    return resolver.getResult();
}

