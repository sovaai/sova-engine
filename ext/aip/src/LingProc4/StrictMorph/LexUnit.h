/**
 * @file   LexUnit.h
 * @author swizard <me@swizard.info>
 * @date   Wed Jun 11 14:00:55 2008
 * 
 * @brief  One lexical unit: contains enough data to restore any morphology information about it.
 * 
 * 
 */
#ifndef _LEXUNIT_H_
#define _LEXUNIT_H_

#include <sys/types.h>

#include "API/StrictMorph.h"

#include "Stem.h"
#include "Lexeme.h"
#include "Flexion.h"

class MorphPosition : public StemRecordsArray
{
public:
	MorphPosition( const uint8_t   *_data,
		       size_t           _dataSize ) :
		StemRecordsArray( _data, _dataSize ),
		stemIndex( 0 ),
		flexId( 0 ),
		listIdx( 0 ),
		stemText( 0 ) { }
	
	MorphPosition() :
		StemRecordsArray(),
		stemIndex( 0 ),
		flexId( 0 ),
		listIdx( 0 ),
		stemText( 0 ) { }

public:
	MorphPosition &operator=(const MorphPosition &position)
	{
		stemIndex    = position.stemIndex;
		flexId       = position.flexId;
		listIdx      = position.listIdx;
		stemText     = position.stemText;
		data         = position.data;
		recordsCount = position.recordsCount;

		return *this;
	}
        
public:
	size_t           stemIndex;
	FlexId           flexId;
	WordFormListIdx  listIdx;
	const char      *stemText;
};

class LexUnit : public MorphPosition
{
public:
	typedef Functor<const MorphPosition &, StrictMorphErrors> PositionFunctor;

public:
	LexUnit( const StrictMorphShadow &_morph,
                 const uint8_t           *_data,
		 size_t                   _dataSize ) :
		MorphPosition( _data, _dataSize ),
                morph( _morph ),
                leftCompound( 0 ),
                compoundBound( 0 ),
		lexId( 0 ),
		lexOrthVariant( 0 ),
		lexCaps( StrictLex::CAPS_NO_CASE ),
		morphNumber( 0 ),
		isDirectPosition( true ),
		formNumber( 0 ),
		formVariant( 0 ) { }

	LexUnit( const StrictMorphShadow &_morph,
                 LexemeId                 _lexId,
		 size_t                   _lexOrthVariant,
		 StrictLex::DictCaps      _lexCaps,
		 unsigned int             _morphNumber,
		 ParadigmFormNumber       _formNumber,
		 WordFormNumber           _formVariant ) :
		MorphPosition(),
                morph( _morph ),
                leftCompound( 0 ),
                compoundBound( 0 ),        
		lexId( _lexId ),
		lexOrthVariant( _lexOrthVariant ),
		lexCaps( _lexCaps ),
		morphNumber( _morphNumber ),
		isDirectPosition( false ),
		formNumber( _formNumber ),
		formVariant( _formVariant ) { }
		
public:
	LexUnit &operator=( const LexUnit &lex )
	{
		MorphPosition::operator=( lex );
		leftCompound     = lex.leftCompound;
                compoundBound    = lex.compoundBound;
		lexId            = lex.lexId;
		lexOrthVariant   = lex.lexOrthVariant;
		lexCaps          = lex.lexCaps;
		morphNumber      = lex.morphNumber;
		isDirectPosition = lex.isDirectPosition;
		formNumber       = lex.formNumber;
		formVariant      = lex.formVariant;
		
		return *this;
	}
		
public:		 
	StrictMorphErrors callWithMorphPosition( PositionFunctor &functor ) const;
	
public:
        const StrictMorphShadow &morph;
        
	/* Lex information: permanent part with direct access */
	const StrictLex     *leftCompound;
        size_t               compoundBound;
	LexemeId             lexId;
	size_t               lexOrthVariant;
	StrictLex::DictCaps  lexCaps;
	unsigned int         morphNumber;

protected:
	/* Lex information: direct position */
	bool                 isDirectPosition;

	/* Lex information: indirect position, a scan needed */
	ParadigmFormNumber   formNumber;
	WordFormNumber       formVariant;
};

#endif /* _LEXUNIT_H_ */

