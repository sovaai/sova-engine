/**
 * @file   StrictMorphShadowFSRW.cpp
 * @author swizard <me@swizard.info>
 * @date   Fri Jun  6 20:55:55 2008
 * 
 * @brief  Strict (dictionary) morphology class (fstorage, read/write version) (implementation).
 * 
 * 
 */

#include <_include/cc_compat.h>

#include "StrictMorphFStorageIds.h"
#include "StrictMorphHeaderFSRW.h"
#include "DictionnaryInfoFSRW.h"
#include "StrictMorphShadowFSRW.h"

StrictMorphShadowFSRW::StrictMorphShadowFSRW() :
	StrictMorphShadowFS()
{
}

StrictMorphShadowFSRW::~StrictMorphShadowFSRW()
{
}

enum StrictMorphErrors StrictMorphShadowFSRW::save( fstorage			*fs,
						       fstorage_section_id	 startSection,
						       int			&underlyingError )
{
	/* Store the header to the primary section */
	StrictMorphHeaderFSRW *headerFS = reinterpret_cast<StrictMorphHeaderFSRW *>( &header );
	StrictMorphStorageChunk::Status headerStatus = headerFS->save( fs, startSection + SMHeader );
	if ( unlikely(headerStatus != StrictMorphStorageChunk::SUCCESS) )
	{
		underlyingError = static_cast<int>(headerStatus);
		return STRICT_ERROR_HEADER_SAVE_FAILURE;
	}
	
	/* Store the dictionary to the primary section */
	DictionnaryInfoFSRW *dictInfoFS = reinterpret_cast<DictionnaryInfoFSRW *>( &dictInfo );
	StrictMorphStorageChunk::Status dictStatus = dictInfoFS->save( fs, startSection + SMDictInfo );
	if ( unlikely(dictStatus != StrictMorphStorageChunk::SUCCESS) )
	{
		underlyingError = static_cast<int>(dictStatus);
		return STRICT_ERROR_DICT_INFO_SAVE_FAILURE;
	}

	/* Define storing information template for most of the data */
	struct StoringInfo
	{
		const void             *buffer;
		size_t                  size;
		fstorage_section_id     sectionModifier;
		enum StrictMorphErrors  sectionError;
		enum StrictMorphErrors  savingError;
	} storingInfos[] =
		  {
			  { paradigms,
			    dictInfo.paradigmsCount * sizeof(Paradigm),
			    SMParadigms,
			    STRICT_ERROR_PARADIGMS_SECTION_FAILURE,
			    STRICT_ERROR_PARADIGMS_SAVE_FAILURE },
			    
			  { paradigmForms,
			    dictInfo.paradigmFormsCount * sizeof(ParadigmForm),
			    SMParadigmForms,
			    STRICT_ERROR_PARADIGM_FORMS_SECTION_FAILURE,
			    STRICT_ERROR_PARADIGM_FORMS_SAVE_FAILURE },
			    
			  { wordForms,
			    dictInfo.wordFormsCount * sizeof(WordForm),
			    SMWordForms,
			    STRICT_ERROR_WORD_FORMS_SECTION_FAILURE,
			    STRICT_ERROR_WORD_FORMS_SAVE_FAILURE },
			    
			  { lexemes,
			    dictInfo.lexemesCount * sizeof(Lexeme),
			    SMLexemes,
			    STRICT_ERROR_LEXEMES_SECTION_FAILURE,
			    STRICT_ERROR_LEXEMES_SAVE_FAILURE },
			    
			  { lexGramDescsIdx,
			    dictInfo.lexGramDescListLength,
			    SMLexGramDescList,
			    STRICT_ERROR_LEX_GDESC_LIST_SECTION_FAILURE,
			    STRICT_ERROR_LEX_GDESC_LIST_SAVE_FAILURE },
			  
			  { lexStemsList,
			    dictInfo.lexStemsListLength * sizeof(StemId),
			    SMLexStemsList,
			    STRICT_ERROR_LEX_STEMS_LIST_SECTION_FAILURE,
			    STRICT_ERROR_LEX_STEMS_LIST_SAVE_FAILURE },
			  
			  { lexOrthVariants.revoke(),
			    dictInfo.lexOrthVarsListLength * sizeof(LexemeId),
			    SMLexOrthVariants,
			    STRICT_ERROR_LEX_ORTH_LIST_SECTION_FAILURE,
			    STRICT_ERROR_LEX_ORTH_LIST_SAVE_FAILURE },
			  
			  { flexions.revoke(),
			    dictInfo.flexCount * FlexionsArray::maxFlexLength,
			    SMFlexions,
			    STRICT_ERROR_FLEXIONS_SECTION_FAILURE,
			    STRICT_ERROR_FLEXIONS_SAVE_FAILURE },
			    
			  { flexDistribs.revoke(),
			    dictInfo.flexDistribCount * dictInfo.flexCount * sizeof(FlexDistrib),
			    SMFlexDistribs,
			    STRICT_ERROR_FLEX_DISTRIB_SECTION_FAILURE,
			    STRICT_ERROR_FLEX_DISTRIB_SAVE_FAILURE },

                          { excludeChains,
                            dictInfo.excludeChainsLength * sizeof(uint64_t),
                            SMExcludeChains,
                            STRICT_ERROR_EXCLUDE_CHAINS_SECTION_FAILURE,
                            STRICT_ERROR_EXCLUDE_CHAINS_SAVE_FAILURE },

                          { excludeIndex.getBuffer(),
                            excludeIndex.getBufferSize(),
                            SMExcludeIndex,
                            STRICT_ERROR_EXCLUDE_INDEX_SECTION_FAILURE,
                            STRICT_ERROR_EXCLUDE_INDEX_SAVE_FAILURE }
		  };
	
	/* Store the rest sections using StoringInfo template created */
	for (int i = (sizeof(storingInfos) / sizeof(StoringInfo)) - 1; i >= 0; i--)
	{
		/* Create a section */
		fstorage_section *section =
			fstorage_get_section(fs, startSection + storingInfos[i].sectionModifier);
		if ( unlikely(section == 0) )
			return storingInfos[i].sectionError;

		/* Set the data for the section */
		int fstResult = fstorage_section_set_data( section,
							   const_cast<void *>(storingInfos[i].buffer),
							   storingInfos[i].size );
		if ( unlikely(fstResult != FSTORAGE_OK) )
		{
			underlyingError = fstResult;
			return storingInfos[i].savingError;
		}
	}

	/* Store the patricia tree */
	PatriciaTreeFS *stemsTreeFS = dynamic_cast<PatriciaTreeFS *>(stemsTree);
	if ( unlikely(stemsTreeFS == 0) )
		return STRICT_ERROR_INTERNAL_ERROR;
	
	PatriciaTreeCore::ResultStatus patStatus = stemsTreeFS->save(fs, startSection + SMStemsTree);
	if ( unlikely(patStatus != PatriciaTreeCore::RESULT_STATUS_SUCCESS) )
	{
		underlyingError = static_cast<int>(patStatus);
		return STRICT_ERROR_STEMS_SAVE_FAILURE;
	}
	
	return STRICT_OK;
}


