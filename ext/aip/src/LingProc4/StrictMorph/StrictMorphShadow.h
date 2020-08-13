/**
 * @file   StrictMorphShadow.h
 * @author swizard <me@swizard.info>
 * @date   Wed May 14 19:21:15 2008
 * 
 * @brief  Strict (dictionary) morphology class.
 * 
 * 
 */
#ifndef _STRICTMORPHSHADOW_H_
#define _STRICTMORPHSHADOW_H_

#ifdef _MSC_VER
    #pragma warning(disable:4512) // assignment operator could not be generated
#endif

#include <sys/types.h>
#include <_include/_inttype.h>

#include <lib/libpts2/PatriciaTree.h>

#include <LingProc4/WordNormalizerInterface.h>

#include "API/StrictMorph.h"

#include "LexUnit.h"
#include "Paradigm.h"
#include "WordForm.h"
#include "Flexion.h"
#include "Stem.h"
#include "Lexeme.h"
#include "StemCommon.h"
#include "LexGramDescShadow.h"
#include "OrthVariantList.h"
#include "DictionnaryInfo.h"
#include "StrictMorphHeader.h"
#include "CompoundExclude.h"

class DecompositionProcessor;

class StrictMorphShadow
{
public:
	enum PublicConstants
        {
                maxLemmatizeWordLength = 1024,
                maxCompoundRecurseDepth = 64
        };
	
protected:
        enum ProtectedConstants
        {
                patriciaCreateFlags =
		PatriciaTree::TREE_COMPACT    |
		PatriciaTree::TREE_CROSSLINKS |
		PatriciaTree::TREE_READ_ONLY
        };
	
public:
	StrictMorphShadow();
	virtual ~StrictMorphShadow();

public:
	/** 
	 * Initializes the StrictMorphShadow instance based on data in handler provided.
	 * 
	 * @param charset A charset to use for the current morphology
	 * @param params  A structure with creation parameters filled
	 * @param handler A handler with morphology data
	 * @param underlyingError The error lifted if failure
	 * 
	 * @return Creation result as StrictMorphErrors
	 */
	StrictMorphErrors create( const UCharSet                 *charset,
				  const StrictMorphParams        &params,
				  const StrictMorphMemoryHandler &handler,
				  int                            &underlyingError );

public:
	/** 
	 * Given the paradigm form id returns the ParadigmForm object.
	 * 
	 * @param id An id of the object
	 * 
	 * @return ParadigmForm object pointer or null pointer if fail
	 */
	const ParadigmForm *getParadigmForm(ParadigmFormId id) const;
        
	/** 
	 * Given the paradigm type returns the Paradigm object.
	 * 
	 * @param type A type of the paradigm
	 * 
	 * @return Paradigm object pointer or null pointer if fail
	 */
	const Paradigm *getParadigm(ParadigmType type) const;
        
public:
	/** 
	 * For each lexeme within a dictionary creates its paradigm and invokes the
	 * given enumerator for each LEXID from paradigm.
	 * 
	 * @param enumerator A functor to invoke with the LEXID constructed
	 * 
	 * @return Enumeration result as StrictMorphErrors
	 */
	StrictMorphErrors enumerate( StrictLex::LemmatizeFunctor &enumerator ) const;

	/** 
	 * Performs the word lemmatization.
	 * 
	 * @param functor A functor object which will receive each lemmatization result
	 * @param word The word to lemmatize
	 * @param wordLength The length of the word
         * @param flags StrictMorphParams::FlagUseDefault or special flags.
	 * 
	 * @return Lemmatization result as StrictMorphErrors
	 */
	StrictMorphErrors lemmatize( StrictLex::LemmatizeFunctor &functor,
				     const char                  *word,
				     size_t                       wordLength,
                                     unsigned int                 flags ) const;
        
        /** 
         * Invokes the functor given for all forms sets available for the lex unit given.
         * 
         * @param functor A functor to invoke for each forms set
         * @param unit A lex unit to obtain forms sets for
         * 
         * @return Forms sets obtaining result as StrictMorphErrors
         */
        StrictMorphErrors queryFormsSets( StrictLex::FormsSetFunctor &functor, const LexUnit &unit  ) const;
        
	/** 
	 * Retrieves the original text of the lexeme given.
	 * 
	 * @param functor A functor which receives the text (or several possible if cannot restore exactly)
	 * @param unit A lex unit to retrieve text for
	 * 
	 * @return Text retrieval as StrictMorphErrors
	 */
	StrictMorphErrors getLexText( StrictLex::TextFunctor &functor, const LexUnit &unit ) const;

	/** 
	 * Searches the given orthographic variant of a given lexeme and returns its lex number.
	 * 
	 * @param lexNum A lex number of a main variant to search. Upon return contains the result
	 * @param orthVar An orthographic variant to search
	 * 
	 * @return Lexeme orthographic variant request result as StrictMorphErrors
	 */
	StrictMorphErrors getOrthVariant( LexemeId &lexNum, size_t orthVar ) const;

        /** 
         * Checks if the given lexeme has some orthographic variants.
         * 
         * @param lexNum A lex number of a main variant to check.
         * @param hasMore Return value: true if they are some variants, and false otherwise.
         * 
         * @return Lexeme orthographic variants check result as StrictMorphErrors
         */
        StrictMorphErrors hasOrthVariants( LexemeId lexNum, bool &hasMore ) const;
        
        /** 
         * Invokes the functor provided with all orthographic variants available for a lexeme given.
         * 
         * @param functor A functor for results receiving
         * @param lex A lexeme for request variants for
         * 
         * @return Orthographic variants retrieval as StrictMorphErrors
         */
        StrictMorphErrors queryOrthVariants( StrictLex::OrthEnumFunctor &functor, const LexUnit &lex ) const;
        
        /** 
         * Synthesis the paradigm for the given lexeme.
         * 
         * @param functor A functor for results receiving (StrictWordForm each call)
         * @param flags Synthesis mode
         * @param filter Results filtering
         * @param lex A lexeme to synthesis paradigm for
         * 
         * @return Synthesis results as StrictMorphErrors
         */
        StrictMorphErrors synthesizeParadigm( StrictWordFormsSet::EnumerateFunctor &functor,
                                              StrictLex::SynthesisFlags             flags,
                                              const FormFilter                     *filter,
                                              const LexUnit                        &lex ) const;
        
        /** 
         * Requests usage attributes of a lexeme.
         * 
         * @param lexNo Lex number of the main orth variant of the lexeme
         * @param orthVar Orth variant of the lexeme
         * 
         * @return Result as StrictLexOrthVariant::UsageAttribs
         */
        StrictLexOrthVariant::UsageAttribs getUsageAttribs( LexemeId lexNo, size_t orthVar ) const;
        
        /** 
         * Uses the builtin morphology normalizer to normalize a given word.
         * 
         * @param functor A functor for results receiving
         * @param word A word to normalize
         * @param wordLength The length of the word
         * @param flags StrictMorphParams::FlagUseDefault or special flags.
         * 
         * @return Normalization status as StrictMorphErrors
         */
        StrictMorphErrors normalizeWord( WordNormalizerFunctor    &functor,
                                         const char               *word,
                                         size_t                    wordLength,
                                         unsigned int              flags ) const;

        /** 
         * Requests dictionary file information.
         * 
         * 
         * @return NULL if failure or dictionary info structure
         * @return 
         */
        const StrictDictInfo *getDictInfo() const;
        
        /** 
         * Requests morphology default flags, given on creation.
         * 
         * 
         * @return Flags value or StrictMorphParams::FlagInvalid on error
         */
        unsigned int getFlags() const;
        
public:
        /* API Utilities */
        StrictMorphErrors getGramDesc( StrictLex::GramEnumFunctor &functor, LexemeId lexId, size_t lexOrthVariant ) const;
        
        /* Misc: internal morphology number accessor */
        unsigned int getMorphNumber() const;
        
public:
	/* LexUnit serialization */
	StrictMorphErrors pack32( const StrictLex &lex, uint32_t &result, BinaryData *additionalData = 0 ) const;
	StrictMorphErrors pack64( const StrictLex &lex, uint64_t &result, BinaryData *additionalData = 0 ) const;
	StrictMorphErrors callWithUnpack32( uint32_t packedLex, StrictMorph::UnpackFunctor &functor, const BinaryData *additionalData = 0 ) const;
	StrictMorphErrors callWithUnpack64( uint64_t packedLex, StrictMorph::UnpackFunctor &functor, const BinaryData *additionalData = 0 ) const;
                
protected:
	StrictMorphErrors createCommon( const StrictMorphMemoryHandler &handler,
                                        const StrictMorphParams        &params,
                                        int                            &underlyingError );

        StrictMorphErrors lemmatizeCompound( StrictLex::LemmatizeFunctor &functor,
                                             const char                  *word,
                                             size_t                       wordLength,
                                             unsigned int                 localFlags,
                                             const StrictLex             *leftCompound,
                                             size_t                       compoundBound ) const;
        
        StrictMorphErrors lemmatizeCompound( StrictLex::LemmatizeFunctor &functor,
                                             const uint8_t               *key,
                                             StrictLex::DictCaps          currentCaps,
                                             size_t                       wordLength,
                                             unsigned int                 localFlags,
                                             const StrictLex             *leftCompound,
                                             size_t                       compoundBound,
                                             const uint64_t              *excludeFilters,
                                             size_t                       currentDepth ) const;
        
private:
        /* Utility functions */
	StrictMorphErrors createStemsTree( const StrictMorphMemoryHandler &handler,
                                           int                            &underlyingError );
	StrictMorphErrors requestLexeme( LexemeId lexId, const Lexeme *&lexeme ) const;
	StrictMorphErrors requestTreeNode( StemId          stemId,
                                           const uint8_t *&nodeData,
                                           size_t         &nodeDataSize ) const;
        StrictMorphErrors getOrthVariantsList( LexemeId        &lexNum,
                                               size_t           orthVar,
                                               const LexemeId *&orthList ) const;
        

        /* Utility functions - synthesis */
        StrictMorphErrors getLexemeOrth( LexemeId       lexId,
					 size_t         orthVariant,
					 const Lexeme *&lexeme ) const;

public:
	StrictMorphErrors scanForms( LexUnit::PositionFunctor &functor,
				     LexemeId                  lexId,
				     size_t                    orthVariant,
				     ParadigmFormNumber        formNumber,
				     WordFormNumber            formVariant ) const;

private:
	StrictMorphErrors scanFormsStems( LexUnit::PositionFunctor &functor,
					  const Lexeme             &lexeme ) const;
	
	StrictMorphErrors scanFormsFlexions( LexUnit::PositionFunctor &functor,
					     StemId                    stemId ) const;
	
protected:
	virtual PatriciaTree *createPatriciaTree();

protected:
	bool ready;

protected:
	StrictMorphHeader  header;
	DictionnaryInfo    dictInfo;

        const UCharSet    *charset;
        WordNormalizer    *normalizer;
        
public:
        unsigned long getDataSerial() const;
        unsigned long getDataMajor() const;
        unsigned long getDataMinor() const;
        
protected:
	const Paradigm       *paradigms;
	const ParadigmForm   *paradigmForms;
	const WordForm       *wordForms;
	const Lexeme         *lexemes;
	const StemId         *lexStemsList;
        
	const CommonReference<uint32_t> *lexGramDescsIdx;
        const LexGramDescShadow         *lexGramDescs;
        
	PatriciaTree         *stemsTree;
	FlexDistribArray      flexDistribs;
	FlexionsArray         flexions;
	OrthVariantList       lexOrthVariants;

        ExcludeIndex          excludeIndex;
        ExcludeChain          excludeChains;
};

#endif /* _STRICTMORPHSHADOW_H_ */

