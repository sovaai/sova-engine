/**
 * @file   LexUnit.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed Jul 23 18:12:30 2008
 * 
 * @brief  One lexical unit: contains enough data to restore any morphology information about it (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include <LingProc4/StrictMorphGram/StrictMorphErrors.h>

#include "LexUnit.h"
#include "StrictMorphShadow.h"

StrictMorphErrors LexUnit::callWithMorphPosition( LexUnit::PositionFunctor &functor ) const
{
	/* If the current lex unit contains direct morphological position, invoke the functor with it */
	if ( isDirectPosition )
		return functor.apply( *this );

	/* Otherwise perform a scanning */
	return morph.scanForms(functor, lexId, lexOrthVariant, formNumber, formVariant);
}

