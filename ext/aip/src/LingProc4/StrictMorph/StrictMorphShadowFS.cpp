/**
 * @file   StrictMorphShadowFS.cpp
 * @author swizard <me@swizard.info>
 * @date   Wed May 21 17:10:55 2008
 * 
 * @brief  Strict (dictionary) morphology class (fstorage version) (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include "StrictMorphFStorageIds.h"
#include "StrictMorphShadowFS.h"

StrictMorphShadowFS::StrictMorphShadowFS() :
	StrictMorphShadow()
{
}

StrictMorphShadowFS::~StrictMorphShadowFS()
{
}

static int sectionToHandler( fstorage *fs, fstorage_section_id sectionId, StrictMorphStorageChunk &chunk )
{
	fstorage_section *section = fstorage_find_section( fs, sectionId );
	if ( unlikely(section == 0) )
		return 1;
	chunk.buffer = fstorage_section_get_all_data( section );
	if ( unlikely(chunk.buffer == 0) )
		return 1;
	chunk.size   = fstorage_section_get_size( section );

	return 0;
}

enum StrictMorphErrors StrictMorphShadowFS::load( const UCharSet          *charset,
						  const StrictMorphParams &params,
						  fstorage                *fs,
						  fstorage_section_id      startSection,
						  int                     &underlyingError )
{
        enum StrictMorphErrors status;
        underlyingError = STRICT_OK;
	
        if ( unlikely(charset == 0) )
		return STRICT_ERROR_INVALID_CHARSET;
	this->charset = charset;
	
	/* Create and fill the handler for strict morphology creation */
	StrictMorphMemoryHandler  handler;

	if ( unlikely( sectionToHandler(fs, startSection + SMHeader,          handler.header) )          != 0 )
		return STRICT_ERROR_HEADER_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMDictInfo,        handler.dictInfo) )        != 0 )
		return STRICT_ERROR_DICT_INFO_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMParadigms,       handler.paradigms) )       != 0 )
		return STRICT_ERROR_PARADIGMS_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMParadigmForms,   handler.paradigmForms) )   != 0 )
		return STRICT_ERROR_PARADIGM_FORMS_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMWordForms,       handler.wordForms) )       != 0 )
		return STRICT_ERROR_WORD_FORMS_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMFlexions,        handler.flexions) )        != 0 )
		return STRICT_ERROR_FLEXIONS_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMFlexDistribs,    handler.flexDistribs) )    != 0 )
		return STRICT_ERROR_FLEX_DISTRIB_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMLexemes,         handler.lexemes) )         != 0 )
		return STRICT_ERROR_LEXEMES_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMLexGramDescList, handler.lexGramDescList) ) != 0 )
		return STRICT_ERROR_LEX_GDESC_LIST_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMLexStemsList,    handler.lexStemsList) )    != 0 )
		return STRICT_ERROR_LEX_STEMS_LIST_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMLexOrthVariants, handler.lexOrthVariants) ) != 0 )
		return STRICT_ERROR_LEX_ORTH_LIST_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMExcludeChains,   handler.excludeChains) )   != 0 )
		return STRICT_ERROR_EXCLUDE_CHAINS_SECTION_FAILURE;
	if ( unlikely( sectionToHandler(fs, startSection + SMExcludeIndex,    handler.excludeIndex) )    != 0 )
		return STRICT_ERROR_EXCLUDE_INDEX_SECTION_FAILURE;
        
	/* Create the common part */
	status = createCommon(handler, params, underlyingError);
	if ( unlikely(status != STRICT_OK) )
		return status;

	/* Create the stems tree */
	status = createStemsTree(fs, startSection + SMStemsTree, underlyingError);
	if ( unlikely(status != STRICT_OK) )
		return status;
	
	return STRICT_OK;
}

PatriciaTree *StrictMorphShadowFS::createPatriciaTree()
{
	return new PatriciaTreeFS();
}

enum StrictMorphErrors StrictMorphShadowFS::createStemsTree( fstorage		*fs,
								fstorage_section_id	 startSection,
								int			&underlyingError )
{
	underlyingError = STRICT_OK;

	PatriciaTreeFS *stemsTreeFS = dynamic_cast<PatriciaTreeFS *>( createPatriciaTree() );
	stemsTree = stemsTreeFS;
	
	/* Try to load stems information */
	PatriciaTreeCore::ResultStatus status = stemsTreeFS->load( StrictMorphShadow::patriciaCreateFlags,
								   fs,
								   startSection );
	if ( unlikely(status != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
	{
		underlyingError = static_cast<int>(status);
		return STRICT_ERROR_STEMS_LOAD_FAILURE;
	}
	
	return STRICT_OK;
}


