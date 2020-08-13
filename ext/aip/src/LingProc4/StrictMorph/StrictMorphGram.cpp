/**
 * @file   StrictMorphGram.cpp
 * @author swizard <me@swizard.info>
 * @date   Sun Oct 25 21:46:19 2009
 * 
 * @brief  StrictMorphGram common API implementation.
 * 
 * 
 */

#include <_include/cc_compat.h>
#include <_include/_inttype.h>

#include <LingProc4/StrictMorphGram/StrictLex.h>
#include <LingProc4/StrictMorphGram/StrictWordForm.h>

#include <LingProc4/LexID64.h>

#include "LexUnit.h"
#include "StrictMorphShadow.h"

size_t StrictLex::getLexNo() const
{
        if ( unlikely(getShadow() == 0) )
                return 0;

        return static_cast<size_t>
                (reinterpret_cast<const LexUnit *>(getShadow())->lexId);
}

StrictLex::DictCaps StrictLex::getDictCaps() const
{
        if ( unlikely(getShadow() == 0) )
                return CAPS_LAST;

        return reinterpret_cast<const LexUnit *>(getShadow())->lexCaps;
}

static const StrictLexOrthVariant getOrthVariant( const void *shadow,
                                                  size_t      orthVariant )
{
        StrictLexOrthVariant result;
        
        if ( unlikely(shadow == 0) )
                return result;
        
        const LexUnit *unit = reinterpret_cast<const LexUnit *>(shadow);
        
        result.orthVariant = orthVariant;
        LexemeId  lexNo    = unit->lexId;

        if ( result.orthVariant == 0 )
        {
            bool hasMore = false;
            StrictMorphErrors status = unit->morph.hasOrthVariants( lexNo, hasMore );
            if ( unlikely(status != STRICT_OK) )
            {
                result.type = StrictLexOrthVariant::INVALID;
                return result;
            }
                
            if ( hasMore )
                result.type = StrictLexOrthVariant::MAIN_HAS_MORE;
            else
                result.type = StrictLexOrthVariant::MAIN_ONLY;
        }
        else
        {
            StrictMorphErrors status = unit->morph.getOrthVariant( lexNo, result.orthVariant );
            if ( unlikely(status != STRICT_OK) )
            {
                result.type = StrictLexOrthVariant::INVALID;
                return result;
            }
        
            result.type = StrictLexOrthVariant::SUPPLEMENT;
        }

        result.lexNo        = static_cast<size_t>( lexNo );
        result.usageAttribs = unit->morph.getUsageAttribs( unit->lexId, result.orthVariant );
                
        return result;
}

const StrictLexOrthVariant StrictLex::getVariant() const
{
        return getOrthVariant( getShadow(),
                               ( getShadow() == 0 ? 0 : reinterpret_cast<const LexUnit *>(getShadow())->lexOrthVariant ) );
}

const StrictLexOrthVariant StrictLex::getVariant( size_t orthVariant ) const
{
        return getOrthVariant( getShadow(), orthVariant );
}

StrictMorphErrors StrictLex::queryVariants( StrictLex::OrthEnumFunctor &functor ) const
{
        const StrictLexOrthVariant var = getVariant();
        if ( unlikely(var.type == StrictLexOrthVariant::INVALID) )
                return STRICT_ERROR_INVALID_ORTH_VARIANT;

        if ( var.type == StrictLexOrthVariant::MAIN_ONLY )
                return STRICT_OK;

        if ( var.type == StrictLexOrthVariant::SUPPLEMENT )
                return STRICT_ERROR_SUPPLEMENT_VARIANT_PROVIDED;
        
        const LexUnit *unit = reinterpret_cast<const LexUnit *>(getShadow());

	return unit->morph.queryOrthVariants( functor, *unit );
}

StrictMorphErrors StrictLex::queryLexGrams( StrictLex::GramEnumFunctor &functor ) const
{
        if ( unlikely(getShadow() == 0) )
                return STRICT_ERROR_NOT_CREATED;

        const LexUnit *unit = reinterpret_cast<const LexUnit *>(getShadow());
        
        return unit->morph.getGramDesc( functor, unit->lexId, unit->lexOrthVariant );
}

StrictMorphErrors StrictLex::queryFormsSets( StrictLex::FormsSetFunctor &functor ) const
{
        if ( unlikely(getShadow() == 0) )
                return STRICT_ERROR_NOT_CREATED;
        
        const LexUnit *unit = reinterpret_cast<const LexUnit *>(getShadow());

	return unit->morph.queryFormsSets( functor, *unit );
}
   
StrictMorphErrors StrictLex::synthesizeParadigm( StrictWordFormsSet::EnumerateFunctor &functor,
                                                 StrictLex::SynthesisFlags             flags,
                                                 const FormFilter                     *filter ) const
{
        if ( unlikely(getShadow() == 0) )
                return STRICT_ERROR_NOT_CREATED;
        
        const LexUnit *unit = reinterpret_cast<const LexUnit *>(getShadow());

	return unit->morph.synthesizeParadigm( functor, flags, filter, *unit );
}

StrictMorphErrors StrictLex::restoreText( StrictLex::TextFunctor &functor ) const
{
        if ( unlikely(getShadow() == 0) )
                return STRICT_ERROR_NOT_CREATED;

        const LexUnit *unit = reinterpret_cast<const LexUnit *>(getShadow());
        
        return unit->morph.getLexText( functor, *unit );
}

StrictMorphErrors StrictLex::restoreText( StrictLex::TextFunctor &functor, const StrictWordForm &customForm ) const
{
        if ( unlikely(getShadow() == 0) )
                return STRICT_ERROR_NOT_CREATED;

        const LexUnit *unit = reinterpret_cast<const LexUnit *>(getShadow());

        LexUnit customUnit( unit->morph,
                            unit->lexId,
                            unit->lexOrthVariant,
                            unit->lexCaps,
                            unit->morphNumber,
                            static_cast<ParadigmFormNumber>( customForm.getFormNo() ),
                            static_cast<WordFormNumber>( customForm.getFormVariantNo() ) );
        
        return unit->morph.getLexText( functor, customUnit );
}

const StrictLex *StrictLex::getLeftCompound() const
{
        if ( unlikely(getShadow() == 0) )
                return 0;

        const LexUnit *unit = reinterpret_cast<const LexUnit *>(getShadow());

        return unit->leftCompound;
}

size_t StrictLex::getCompoundBound() const
{
        if ( unlikely(getShadow() == 0) )
                return static_cast<size_t>(-1);
        
        const LexUnit *unit = reinterpret_cast<const LexUnit *>(getShadow());

        return unit->compoundBound;
}

unsigned int StrictLex::getMorphNumber() const
{
        if ( unlikely(getShadow() == 0) )
                return static_cast<unsigned int>(-1);
        
        const LexUnit *unit = reinterpret_cast<const LexUnit *>(getShadow());

        return unit->morphNumber;
}

static StrictMorphErrors reqStrictLex( StrictLex::BuildFunctor &functor,
                                       size_t                   lexNo,
                                       size_t                   orthVariant,
                                       size_t                   formNo,
                                       size_t                   formVariant,
                                       bool                     forceDictForm,
                                       const void              *morphShadow )
{
        if ( unlikely(morphShadow == 0) )
                return STRICT_ERROR_NOT_CREATED;
        
        const StrictMorphShadow *morph = reinterpret_cast<const StrictMorphShadow *>(morphShadow);
        
        LEXID lex = static_cast<LEXID>(lexNo);

        lex = LexSetNoCase( lex );
        lex = LexSetNoMrph( lex, morph->getMorphNumber() );
        lex = LexSetOrthVar( lex, orthVariant );
        if ( forceDictForm )
                lex = _LexSetDictForm( lex );
        else
        {
                lex = LexSetForm( lex, formNo );
                lex = LexSetFormVariant( lex, formVariant );
        }

        class Mapper : public StrictMorph::UnpackFunctor
        {
        public:
                Mapper( StrictLex::BuildFunctor &_functor ) : functor( _functor ) { }
                
                StrictMorphErrors apply( const StrictLex &lex )
                {
                        return functor.apply( lex );
                }

        private:
                StrictLex::BuildFunctor &functor;
        } mapper( functor );
        
        return morph->callWithUnpack64( lex, mapper );
}

static StrictMorphErrors reqStrictLexText( StrictLex::TextFunctor &functor,
                                           size_t                  lexNo,
                                           size_t                  orthVariant,
                                           size_t                  formNo,
                                           size_t                  formVariant,
                                           const void             *morphShadow )
{
        class Receiver : public StrictLex::BuildFunctor
        {
        public:
                Receiver( StrictLex::TextFunctor &_functor ) : functor( _functor ) { }
                
                StrictMorphErrors apply( const StrictLex &lex )
                {
                        return lex.restoreText( functor );
                }

        private:
                StrictLex::TextFunctor &functor;
        } f( functor );

        return reqStrictLex( f, lexNo, orthVariant, formNo, formVariant, false, morphShadow );
}

template<> SM_DLL
StrictMorphErrors StrictLex::restoreText<StrictMorph>( const StrictMorph      &morph,
                                                       StrictLex::TextFunctor &functor,
                                                       size_t             lexNo,
                                                       size_t             orthVariant,
                                                       size_t             formNo,
                                                       size_t             formVariant )
{
        return reqStrictLexText( functor, lexNo, orthVariant, formNo, formVariant, morph.getShadow() );
}

template<> SM_DLL
StrictMorphErrors StrictLex::restoreText<StrictLex>( const StrictLex &lex,
                                                     StrictLex::TextFunctor &functor,
                                                     size_t           lexNo,
                                                     size_t           orthVariant,
                                                     size_t           formNo,
                                                     size_t           formVariant )
{
        return reqStrictLexText( functor, lexNo, orthVariant, formNo, formVariant,
                                 ( lex.getShadow() == 0 ? 0 : &(reinterpret_cast<const LexUnit *>( lex.getShadow() )->morph) ) );
}

template<> SM_DLL
StrictMorphErrors StrictLex::buildStrictLex<StrictMorph>( const StrictMorph       &morph,
                                                          StrictLex::BuildFunctor &functor,
                                                          size_t                   lexNo,
                                                          size_t                   orthVariant,
                                                          size_t                   formNo,
                                                          size_t                   formVariant )
{
        return reqStrictLex( functor, lexNo, orthVariant, formNo, formVariant, false, morph.getShadow() );
}

template<> SM_DLL
StrictMorphErrors StrictLex::buildStrictLex<StrictLex>( const StrictLex         &lex,
                                                        StrictLex::BuildFunctor &functor,
                                                        size_t                   lexNo,
                                                        size_t                   orthVariant,
                                                        size_t                   formNo,
                                                        size_t                   formVariant )
{
        return reqStrictLex( functor,
                             lexNo,
                             orthVariant,
                             formNo,
                             formVariant,
                             false,
                             ( lex.getShadow() == 0 ? 0 : &(reinterpret_cast<const LexUnit *>( lex.getShadow() )->morph) ) );
}

template<> SM_DLL
StrictMorphErrors StrictLex::buildStrictLex<StrictMorph>( const StrictMorph       &morph,
                                                          StrictLex::BuildFunctor &functor,
                                                          size_t                   lexNo,
                                                          size_t                   orthVariant )
{
        return reqStrictLex( functor, lexNo, orthVariant, 0, 0, true, morph.getShadow() );
}

template<> SM_DLL
StrictMorphErrors StrictLex::buildStrictLex<StrictLex>( const StrictLex         &lex,
                                                        StrictLex::BuildFunctor &functor,
                                                        size_t                   lexNo,
                                                        size_t                   orthVariant )
{
        return reqStrictLex( functor,
                             lexNo,
                             orthVariant,
                             0,
                             0,
                             true,
                             ( lex.getShadow() == 0 ? 0 : &(reinterpret_cast<const LexUnit *>( lex.getShadow() )->morph) ) );
}

static const FormGram reqFormGram( const StrictWordForm &sform, const void *morphShadow )
{
        FormGram gram;
        
        if ( unlikely(sform.getShadow() == 0 || morphShadow == 0) )
                return gram;
        
        const WordForm *form = reinterpret_cast<const WordForm *>( sform.getShadow() );
        const StrictMorphShadow *morph = reinterpret_cast<const StrictMorphShadow *>( morphShadow );

        const ParadigmForm *pform = morph->getParadigmForm( form->getParadigmFormId() );
        gram.assignShadow( pform );

        return gram;
}

template<> SM_DLL
const FormGram StrictWordForm::getFormGram<StrictMorph>( const StrictMorph &morph ) const
{
        return reqFormGram( *this, morph.getShadow() );
}

template<> SM_DLL
const FormGram StrictWordForm::getFormGram<StrictLex>( const StrictLex &lex ) const
{
        return reqFormGram( *this, ( lex.getShadow() == 0 ? 0 : &(reinterpret_cast<const LexUnit *>( lex.getShadow() )->morph) ) );
}

static const char *reqParadigmName( const StrictWordForm &sform, const void *morphShadow )
{
        if ( unlikely(sform.getShadow() == 0 || morphShadow == 0) )
                return 0;

        const WordForm *form = reinterpret_cast<const WordForm *>( sform.getShadow() );
        const StrictMorphShadow *morph = reinterpret_cast<const StrictMorphShadow *>( morphShadow );

        return morph->getParadigm( morph->getParadigmForm(form->getParadigmFormId())->getParadigmType() )->getTypeName();
}

template<> SM_DLL
const char *StrictWordForm::getParadigmName<StrictLex>( const StrictLex &lex ) const
{
        return reqParadigmName( *this, ( lex.getShadow() == 0 ? 0 : &(reinterpret_cast<const LexUnit *>( lex.getShadow() )->morph) ) );
}

template<> SM_DLL
const char *StrictWordForm::getParadigmName<StrictMorph>( const StrictMorph &morph ) const
{
        return reqParadigmName( *this, morph.getShadow() );
}

size_t StrictWordForm::getFormNo() const
{
        if ( unlikely(getShadow() == 0) )
                return static_cast<size_t>(-1);
        
        const WordForm *form = reinterpret_cast<const WordForm *>(getShadow());

        return static_cast<size_t>( form->getFormNumber() );
}

size_t StrictWordForm::getFormVariantNo() const
{
        if ( unlikely(getShadow() == 0) )
                return static_cast<size_t>(-1);

        const WordForm *form = reinterpret_cast<const WordForm *>(getShadow());
        
        return static_cast<size_t>( form->getFormVariantNumber() );
}

StrictWordForm::UsageAttribs StrictWordForm::getUsageAttribs() const
{
        if ( unlikely(getShadow() == 0) )
                return static_cast<UsageAttribs>(USAGE_ATTR_NONE);

        const WordForm *form = reinterpret_cast<const WordForm *>(getShadow());

        return static_cast<UsageAttribs>( form->getAttribs() );
}

const char *StrictWordForm::getName( StrictWordForm::UsageAttribs attr )
{
        switch ( attr )
        {
        case USAGE_ATTR_NONE: return "";
                
        case USAGE_ATTR_RARE: return "rare"; // 00001
        case USAGE_ATTR_FORM2: return "form2"; // 00010
        case USAGE_ATTR_RARE | USAGE_ATTR_FORM2: return "rare+form2"; // 00011
        case USAGE_ATTR_CASE2: return "case2"; // 00100
        case USAGE_ATTR_RARE | USAGE_ATTR_CASE2: return "rare+case2"; // 00101
        case USAGE_ATTR_FORM2 | USAGE_ATTR_CASE2: return "form2+case2"; // 00110
        case USAGE_ATTR_RARE | USAGE_ATTR_FORM2 | USAGE_ATTR_CASE2: return "rare+form2+case2"; // 00111
        case USAGE_ATTR_JO: return "jo"; // 01000
        case USAGE_ATTR_RARE | USAGE_ATTR_JO: return "rare+jo"; // 01001
        case USAGE_ATTR_FORM2 | USAGE_ATTR_JO: return "form2+jo"; // 01010
        case USAGE_ATTR_RARE | USAGE_ATTR_FORM2 | USAGE_ATTR_JO: return "rare+form2+jo"; // 01011
        case USAGE_ATTR_CASE2 | USAGE_ATTR_JO: return "case2+jo"; // 01100
        case USAGE_ATTR_RARE | USAGE_ATTR_CASE2 | USAGE_ATTR_JO: return "rare+case2+jo"; // 01101
        case USAGE_ATTR_FORM2 | USAGE_ATTR_CASE2 | USAGE_ATTR_JO: return "form2+case2+jo"; // 01110
        case USAGE_ATTR_RARE | USAGE_ATTR_FORM2 | USAGE_ATTR_CASE2 | USAGE_ATTR_JO: return "rare+form2+case2+jo"; // 01111
        case USAGE_ATTR_EXTRA: return "extra"; // 10000
        case USAGE_ATTR_EXTRA | USAGE_ATTR_RARE: return "rare+extra"; // 10001
        case USAGE_ATTR_EXTRA | USAGE_ATTR_FORM2: return "form2+extra"; // 10010
        case USAGE_ATTR_EXTRA | USAGE_ATTR_RARE | USAGE_ATTR_FORM2: return "rare+form2+extra"; // 10011
        case USAGE_ATTR_EXTRA | USAGE_ATTR_CASE2: return "case2+extra"; // 10100
        case USAGE_ATTR_EXTRA | USAGE_ATTR_RARE | USAGE_ATTR_CASE2: return "rare+case2+extra"; // 10101
        case USAGE_ATTR_EXTRA | USAGE_ATTR_FORM2 | USAGE_ATTR_CASE2: return "form2+case2+extra"; // 10110
        case USAGE_ATTR_EXTRA | USAGE_ATTR_RARE | USAGE_ATTR_FORM2 | USAGE_ATTR_CASE2: return "rare+form2+case2+extra"; // 10111
        case USAGE_ATTR_EXTRA | USAGE_ATTR_JO: return "jo+extra"; // 11000
        case USAGE_ATTR_EXTRA | USAGE_ATTR_RARE | USAGE_ATTR_JO: return "rare+jo+extra"; // 11001
        case USAGE_ATTR_EXTRA | USAGE_ATTR_FORM2 | USAGE_ATTR_JO: return "form2+jo+extra"; // 11010
        case USAGE_ATTR_EXTRA | USAGE_ATTR_RARE | USAGE_ATTR_FORM2 | USAGE_ATTR_JO: return "rare+form2+jo+extra"; // 11011
        case USAGE_ATTR_EXTRA | USAGE_ATTR_CASE2 | USAGE_ATTR_JO: return "case2+jo+extra"; // 11100
        case USAGE_ATTR_EXTRA | USAGE_ATTR_RARE | USAGE_ATTR_CASE2 | USAGE_ATTR_JO: return "rare+case2+jo+extra"; // 11101
        case USAGE_ATTR_EXTRA | USAGE_ATTR_FORM2 | USAGE_ATTR_CASE2 | USAGE_ATTR_JO: return "form2+case2+jo+extra"; // 11110
        case USAGE_ATTR_EXTRA | USAGE_ATTR_RARE | USAGE_ATTR_FORM2 | USAGE_ATTR_CASE2 | USAGE_ATTR_JO: return "rare+form2+case2+jo+extra"; // 11111
                
        default: return "UNKNOWN";
        };
}

const char *StrictLexOrthVariant::getName( StrictLexOrthVariant::UsageAttribs attr )
{
        switch ( attr )
        {
        case USAGE_ATTR_NONE: return "";
        case USAGE_ATTR_NEW: return "new";
        case USAGE_ATTR_OLD: return "old";
                
        default:  return "UNKNOWN";
        };
}

StrictMorphErrors StrictWordFormsSet::enumerate( StrictWordFormsSet::EnumerateFunctor &functor ) const
{
        if ( unlikely(getShadow() == 0) )
                return STRICT_ERROR_NOT_CREATED;
        
        WordFormsArray list( reinterpret_cast<const WordForm *>(getShadow()) );

        /* Enumerate the list with a functor given */
        WordFormListIdx idx = 0;
        StrictWordForm  wordForm;
        for ( const WordForm *e = list.getAt(idx); e != 0; idx++, e = list.getNext(e, idx) )
        {
                wordForm.assignShadow(e);
                StrictMorphErrors status = functor.apply( wordForm );
                if ( unlikely(status != STRICT_OK) )
                        return status;
        }
        
        return STRICT_OK;
}

