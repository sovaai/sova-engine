/**
 * @file   StrictMorphSynthesis.cpp
 * @author swizard <me@swizard.info>
 * @date   Tue Jul 22 01:56:07 2008
 * 
 * @brief  Strict (dictionary) morphology implementation related to synthesis.
 * 
 * 
 */

#include <_include/cc_compat.h>

#include <lib/aptl/avector.h>

#include "LexUnit.h"
#include "StrictMorphShadow.h"

StrictMorphErrors StrictMorphShadow::getLexemeOrth( LexemeId       lexId,
						    size_t         orthVariant,
						    const Lexeme *&lexeme ) const
{
	/* Switch to orthographic variant given */
	if ( unlikely(orthVariant != 0) )
	{
		StrictMorphErrors status = getOrthVariant(lexId, orthVariant);
		if ( unlikely(status != STRICT_OK) )
			return status;
	}

	/* Return the lexeme */
	return requestLexeme(lexId, lexeme);
}

StrictMorphErrors StrictMorphShadow::scanForms( LexUnit::PositionFunctor &functor,
						LexemeId                  lexId,
						size_t                    orthVariant,
						ParadigmFormNumber        formNumber,
						WordFormNumber            formVariant ) const
{
	/* Obtain the lexeme */
	const Lexeme *lexeme = NULL;
	StrictMorphErrors status = getLexemeOrth(lexId, orthVariant, lexeme);
	if ( unlikely(status != STRICT_OK) )
		return status;

	/** 
	 * A filtering wrapper over LexUnit::PositionFunctor.
	 * 
	 */
	class FilterFunctor : public LexUnit::PositionFunctor
	{
	public:
		FilterFunctor( LexUnit::PositionFunctor &_functor, const WordForm *_wordForms ) :
			functor( _functor ),
			wordForms( _wordForms ),
			invokeFlag( false ) { }

		StrictMorphErrors apply(const MorphPosition &position)
		{
			invokeFlag = true;
			return functor.apply(position);
		}

		bool hasInvoked() { return invokeFlag; }

		virtual StrictMorphErrors finish() { return STRICT_OK; }
		
	protected:
		LexUnit::PositionFunctor &functor;
		const WordForm           *wordForms;
		bool                      invokeFlag;
	};

	/** 
	 * A specific form filtering wrapper.
	 * 
	 */
	class SpecificFormFunctor : public FilterFunctor
	{
	public:
		SpecificFormFunctor( LexUnit::PositionFunctor &_functor,
				     const WordForm           *_wordForms,
				     ParadigmFormNumber        _formNumber,
				     WordFormNumber            _formVariant ) :
			FilterFunctor( _functor, _wordForms ),
			formNumber( _formNumber ),
			formVariant( _formVariant ) { }

		StrictMorphErrors apply(const MorphPosition &position)
		{
			/* Scan forms list */
			WordFormsArray  list( wordForms + position.listIdx );
			WordFormListIdx idx   = 0;
			for ( const WordForm *e = list.getAt(idx); e != 0; idx++, e = list.getNext(e, idx) )
			{
				if ( formNumber == e->getFormNumber() &&
				     (formVariant == WORD_FORM_NUMBER_EMPTY ||
				      formVariant == e->getFormVariantNumber()) )
					return FilterFunctor::apply( position );
			}

			return STRICT_OK;
		}
		
	private:
		ParadigmFormNumber formNumber;
		WordFormNumber     formVariant;
	} specificFunctor( functor, wordForms, formNumber, formVariant );

	class MinimumFormFunctor : public FilterFunctor
	{
	public:
		MinimumFormFunctor( LexUnit::PositionFunctor &_functor,
				    const WordForm           *_wordForms ) :
			FilterFunctor( _functor, _wordForms ),
			minForm( static_cast<ParadigmFormNumber>(-1) ),
			minPosition() { }

		StrictMorphErrors apply(const MorphPosition &position)
		{
			/* Scan forms list */
			WordFormsArray  list( wordForms + position.listIdx );
			WordFormListIdx idx   = 0;
			for ( const WordForm *e = list.getAt(idx); e != 0; idx++, e = list.getNext(e, idx) )
			{
				ParadigmFormNumber  currentForm = e->getFormNumber();
			
				if (e->getFormVariantNumber() == 0 && currentForm < minForm)
				{
					minForm     = currentForm;
					minPosition = position;
				}
			}

			return STRICT_OK;
		}

		StrictMorphErrors finish()
		{
			if ( likely(minForm != static_cast<ParadigmFormNumber>(-1)) )
				return FilterFunctor::apply( minPosition );
			
			return STRICT_OK;
		}

	private:
		ParadigmFormNumber minForm;
		MorphPosition      minPosition;
	} minimumFunctor( functor, wordForms );

	/* Choose appropriate filter functor */
	FilterFunctor &filter = *( formNumber == PARADIGM_FORM_EMPTY ?
				   static_cast<FilterFunctor *>(&minimumFunctor) :
				   static_cast<FilterFunctor *>(&specificFunctor) );
	
	/* Perform a scan over stems */
	status = scanFormsStems(filter, *lexeme);
	if ( unlikely(status != STRICT_OK) )
		return status;

	/* Finish filtering */
	status = filter.finish();
	if ( unlikely(status != STRICT_OK) )
		return status;

	/* Check if functor has not been invoked */
	if ( unlikely(!filter.hasInvoked()) )
		return STRICT_ERROR_INVALID_LEX_UNIT_FORM_NO;
	
	return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::scanFormsStems( LexUnit::PositionFunctor &functor,
						     const Lexeme             &lexeme ) const
{
	const CommonReference<StemId> *stemRef = lexeme.getStemIdReference();
	
	if (stemRef->isListReference())
	{
		for (size_t stemIdx = static_cast<size_t>(stemRef->getReference()); ; stemIdx++)
		{
			StemId            stemId = lexStemsList[stemIdx];
			StrictMorphErrors status = scanFormsFlexions(functor, stemId);
			if ( unlikely(status != STRICT_OK) )
				return status;
			
			if (stemListEndMark(stemId)) /* Last stem in the list */
				break;
		}
	}
	else
	{
		StemId            stemId = stemRef->getReference();
		StrictMorphErrors status = scanFormsFlexions(functor, stemId);
		if ( unlikely(status != STRICT_OK) )
			return status;
	}

	return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::scanFormsFlexions( LexUnit::PositionFunctor &functor,
							StemId                    stemId ) const
{
	/* Access the data within a node in patricia tree */
	const uint8_t  *nodeData     = 0;
	size_t          nodeDataSize = 0;
	StrictMorphErrors status = requestTreeNode(stemId, nodeData, nodeDataSize);
	if ( unlikely(status != STRICT_OK) )
		return status;

	/* Obtain node reference */
	StemNodeRef nodeRef = static_cast<StemNodeRef>(stemsTree->dataAddressToLink(nodeData));
	if ( unlikely(nodeRef == 0) )
		return STRICT_ERROR_TREE_NODE_ACCESS_FAILED;

	/* Create the morphological position object */
	MorphPosition position(nodeData, nodeDataSize);
	
	/* Request the text of the stem */
	position.stemText = reinterpret_cast<const char *>
		(stemsTree->linkToKeyAddress(static_cast<uint32_t>(nodeRef)));
	if ( unlikely(position.stemText == 0) )
		return STRICT_ERROR_TREE_NODE_ACCESS_FAILED;

	/* Get flex distribution id for the stem */
	size_t        homoNumber = static_cast<size_t>( getStemHomoNumber(stemId) );
	FlexDistribId distribId  = position.getFlexDistribId(homoNumber);
	position.stemIndex       = homoNumber;

	/* Pass among flex distribution */
	FlexId limit = static_cast<FlexId>(dictInfo.flexCount);
	for (FlexId flexId = 0; flexId < limit; flexId++)
	{
		/* Get word forms index for a flexion */
		WordFormListIdx listIdx = flexDistribs.getWordFormListIdx( distribId, flexId );
		if ( likely(listIdx == static_cast<WordFormListIdx>(-1)) )
			continue;

		position.flexId  = flexId;
		position.listIdx = listIdx;
		
		status           = functor.apply(position);
		if ( unlikely(status != STRICT_OK) )
			return status;
	}

	return STRICT_OK;
}

StrictMorphErrors StrictMorphShadow::queryFormsSets( StrictLex::FormsSetFunctor &functor, const LexUnit &unit ) const
{
	/* Obtain the lexeme */
	const Lexeme *lexeme = NULL;
	StrictMorphErrors status = getLexemeOrth( unit.lexId, unit.lexOrthVariant, lexeme );
	if ( unlikely(status != STRICT_OK) )
		return status;
        
        class LocalFunctor : public LexUnit::PositionFunctor
        {
        public:
                LocalFunctor( StrictLex::FormsSetFunctor &_functor,
                              const WordForm             *_wordForms ) :
                        functor( _functor ),
                        wordForms( _wordForms ) { }
                
                StrictMorphErrors apply( const MorphPosition &position )
                {
                        StrictWordFormsSet set;
                        set.assignShadow( wordForms + position.listIdx );

                        return functor.apply( set );
                }
                
        private:
                StrictLex::FormsSetFunctor &functor;
                const WordForm             *wordForms;
        } localFunctor( functor, wordForms );
        
	return unit.callWithMorphPosition( localFunctor );
}

static int compareForms(const void *a, const void *b)
{
        const StrictWordForm *af = reinterpret_cast<const StrictWordForm *>(a);
        const StrictWordForm *bf = reinterpret_cast<const StrictWordForm *>(b);
        
        if (af->getFormNo() < bf->getFormNo())
                return -1;
        if (af->getFormNo() > bf->getFormNo())
                return +1;

        if (af->getFormVariantNo() < bf->getFormVariantNo())
                return -1;
        if (af->getFormVariantNo() > bf->getFormVariantNo())
                return +1;

        return 0;
}

StrictMorphErrors StrictMorphShadow::synthesizeParadigm( StrictWordFormsSet::EnumerateFunctor &functor,
                                                         StrictLex::SynthesisFlags             flags,
                                                         const FormFilter                     *filter,
							 const LexUnit                        &lex ) const
{
        StrictLex strictLex;
        strictLex.assignShadow( &lex );
        
	/* Obtain the lexeme */
	const Lexeme *lexeme = NULL;
	StrictMorphErrors status = getLexemeOrth( lex.lexId, lex.lexOrthVariant, lexeme );
	if ( unlikely(status != STRICT_OK) )
		return status;
        
        class LocalFunctor : public LexUnit::PositionFunctor, public StrictWordFormsSet::EnumerateFunctor
        {
        public:
                LocalFunctor( const WordForm *_wordForms ) :
                        pool(),
                        wordForms( _wordForms ) { }
                
                StrictMorphErrors apply( const MorphPosition &position )
                {
                        StrictWordFormsSet set;
                        set.assignShadow( wordForms + position.listIdx );
                        
                        return set.enumerate( *this );
                }

                StrictMorphErrors apply( const StrictWordForm &form )
                {
                        pool.push_back( form );
                        if ( unlikely(pool.no_memory()) )
                                return STRICT_ERROR_ALLOCATION_ERROR;
                        
                        return STRICT_OK;
                }

        public:
                avector<StrictWordForm> pool;
                
        private:
                const WordForm         *wordForms;
        } localFunctor( wordForms );
        
	status = scanFormsStems( localFunctor, *lexeme );
        if ( unlikely(status != STRICT_OK) )
                return status;

	/* Sort result buffer */
	qsort( localFunctor.pool.get_buffer(),
	       localFunctor.pool.size(),
	       sizeof(StrictWordForm),
	       compareForms );
        
        StrictWordForm emptyForm;
        size_t         lastForm = 0;
        
        for ( size_t i = 0; i < localFunctor.pool.size(); i++ )
        {
                const StrictWordForm &form = localFunctor.pool[i];

                if ( (flags & StrictLex::SYNTH_WITH_BLANKS) && (filter == 0) )
                {
                        for ( ; lastForm + 1 < form.getFormNo(); lastForm++ )
                        {
                                StrictMorphErrors status = functor.apply( emptyForm );
                                if ( unlikely(status != STRICT_OK) )
                                        return status;
                        }
                }
                lastForm = form.getFormNo();

                if ( filter != 0 )
                {
                        const FormGram gram = form.getFormGram( strictLex );
                        
                        if ( filter->filterMstform  != FormGram::MSTFORM_LAST  && filter->filterMstform  != gram.getMstform() )
                                continue;
                        if ( filter->filterVerbform != FormGram::VERBFORM_LAST && filter->filterVerbform != gram.getVerbform() )
                                continue;
                        if ( filter->filterTense    != FormGram::TENSE_LAST    && filter->filterTense    != gram.getTense() )
                                continue;
                        if ( filter->filterShort    != FormGram::SHORT_LAST    && filter->filterShort    != gram.getShort() )
                                continue;
                        if ( filter->filterRefl     != FormGram::REFL_LAST     && filter->filterRefl     != gram.getRefl() )
                                continue;
                        if ( filter->filterPerson   != FormGram::PERSON_LAST   && filter->filterPerson   != gram.getPerson() )
                                continue;
                        if ( filter->filterNumber   != FormGram::NUMBER_LAST   && filter->filterNumber   != gram.getNumber() )
                                continue;
                        if ( filter->filterMood     != FormGram::MOOD_LAST     && filter->filterMood     != gram.getMood() )
                                continue;
                        if ( filter->filterGender   != FormGram::GENDER_LAST   && filter->filterGender   != gram.getGender() )
                                continue;
                        if ( filter->filterDegree   != FormGram::DEGREE_LAST   && filter->filterDegree   != gram.getDegree() )
                                continue;
                        if ( filter->filterCase     != FormGram::CASE_LAST     && filter->filterCase     != gram.getCase() )
                                continue;
                        if ( filter->filterAnimate  != FormGram::ANIMATE_LAST  && filter->filterAnimate  != gram.getAnimate() )
                                continue;
                }

                if ( !(flags & StrictLex::SYNTH_WITH_EXTRA_FORMS) &&
                     (form.getUsageAttribs() & StrictWordForm::USAGE_ATTR_EXTRA) )
                        continue;

                StrictMorphErrors status = functor.apply( form );
                if ( unlikely(status != STRICT_OK) )
                        return status;
        }
        
        return STRICT_OK;
}


