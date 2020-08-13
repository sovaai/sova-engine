/**
 * @file   Pack32.cpp
 * @author swizard <me@swizard.info>
 * @date   Fri Jun 20 18:18:56 2008
 * 
 * @brief  Pack/upack routines for LexUnit structure: 32 bit result version (limited).
 * 
 * 
 */

#ifdef FORCE_LEXID64
# undef FORCE_LEXID64
#endif

#ifndef FORCE_LEXID32
# define FORCE_LEXID32
#endif

#include <LingProc4/LexID32.h>

#include <_include/cc_compat.h>

#include <LingProc4/BinaryData.h>
#include <LingProc4/StrictMorphGram/StrictLex.h>

#include "LexUnit.h"
#include "StrictMorphShadow.h"

StrictMorphErrors StrictMorphShadow::pack32( const StrictLex &lex, uint32_t &result, BinaryData *additionalData ) const
{
	/* Obtain the lexeme form */
	class FormRetrieverFunctor : public LexUnit::PositionFunctor
	{
	public:
		FormRetrieverFunctor( const WordForm  *_wordForms ) :
			wordForms( _wordForms ),
			form( 0 ),
                        formVariant( 0 ) { }

		StrictMorphErrors apply(const MorphPosition &position)
		{
			/* Position to the word forms list */
			WordFormsArray  list( wordForms );

			/* Search in forms for number == 0 */ 
			bool            found      = false;
			WordFormListIdx currentIdx = position.listIdx;
			for ( const WordForm *e = list.getAt(currentIdx);
			      e != 0;
			      currentIdx++, e = list.getNext(e, currentIdx) )
			{
				if ( e->getFormVariantNumber() == 0 )
				{
					form  = static_cast<unsigned char>( e->getFormNumber() );
					found = true;
					break;
				}
			}

			/* Check if found */
			if ( unlikely(!found) )
			{
				/* Null form is not found, set zero form */
				const WordForm *e = list.getAt( position.listIdx );
				form        = static_cast<unsigned char>( e->getFormNumber() );
                                formVariant = static_cast<unsigned char>( e->getFormVariantNumber() );
			}

			return STRICT_ERROR_ENUMERATION_STOPPED;
		}
		
		unsigned char getForm()        { return form; }
		unsigned char getFormVariant() { return formVariant; }
		
	private:
		const WordForm *wordForms;
		unsigned char   form;
		unsigned char   formVariant;
	} formRetriever( wordForms );
        
        const LexUnit *unit = reinterpret_cast<const LexUnit *>( lex.getShadow() );
        
	StrictMorphErrors status = unit->callWithMorphPosition( formRetriever );
	if ( unlikely(status != STRICT_OK && status != STRICT_ERROR_ENUMERATION_STOPPED) )
		return status;

	LexemeId lexId = unit->lexId;
	if ( unit->lexOrthVariant != 0 )
	{
		status = getOrthVariant(lexId, unit->lexOrthVariant);
		if ( unlikely(status != STRICT_OK) )
			return status;
	}
	
	/* Set the lex number for the result */
	result = static_cast<uint32_t>(lexId);

	/* Set morphology number */
	result = LexSetNoMrph( result, unit->morphNumber );
	
	/* Set the form */
	result = LexSetForm( result, formRetriever.getForm() );

	/* Set the lexeme capitalization */
	switch ( unit->lexCaps )
	{
	case StrictLex::CAPS_NO_CASE:
		result = LexSetNoCase(result);
		break;
		
	case StrictLex::CAPS_LOWER_CASE:
		result = LexSetLower(result);
		break;
		
	case StrictLex::CAPS_TITLE_CASE:
		result = LexSetTitle(result);
		break;
		
	case StrictLex::CAPS_UPPER_CASE:
		result = LexSetUpper(result);
		break;

	default:
		return STRICT_ERROR_INVALID_LEX_UNIT_CAPS;
	};
        
        if ( additionalData != 0 )
        {
                additionalData->set( static_cast<uint64_t>(unit->lexOrthVariant), 4, 4 );
                additionalData->set( static_cast<uint64_t>(formRetriever.getFormVariant()), 0, 4 );
        }
        
	return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::callWithUnpack32( uint32_t packedLex, StrictMorph::UnpackFunctor &functor, const BinaryData *additionalData ) const
{
        StrictLex::DictCaps lexCaps = StrictLex::CAPS_NO_CASE;
	
	if ( LexIsUpper(packedLex) )
		lexCaps = StrictLex::CAPS_UPPER_CASE;
	else if ( LexIsTitle(packedLex) )
		lexCaps = StrictLex::CAPS_TITLE_CASE;
	else if ( LexIsLower(packedLex) )
		lexCaps = StrictLex::CAPS_LOWER_CASE;

        unsigned char      lexForm = LexIdForm(packedLex);
        ParadigmFormNumber form    = ( _LexSetDictForm(packedLex) == packedLex ?
                                      PARADIGM_FORM_EMPTY : static_cast<ParadigmFormNumber>(lexForm));
        
	LexUnit lex( *this,
                     static_cast<LexemeId>( LexId(packedLex) ),
		     ( additionalData == 0 ? 0 : static_cast<size_t>( additionalData->get( 4, 4 ) ) ),
		     lexCaps,
		     static_cast<unsigned int>( LexNoMrph(packedLex) ),
		     form,
		     ( additionalData == 0 ? WORD_FORM_NUMBER_EMPTY : static_cast<WordFormNumber>( additionalData->get( 0, 4 ) ) ) );

        StrictLex strictLex;
        strictLex.assignShadow( &lex );
        
	return functor.apply( strictLex );
}


