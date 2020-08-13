/**
 * @file   StrictMorphFStorageIds.h
 * @author swizard <me@swizard.info>
 * @date   Sun Jun  8 16:14:55 2008
 * 
 * @brief  Contains the fstorage sections numbers (increment to base) for Strict Morph data.
 * 
 * 
 */
#ifndef _STRICTMORPHFSTORAGEIDS_H_
#define _STRICTMORPHFSTORAGEIDS_H_

#include <lib/fstorage/fstorage.h>

const fstorage_section_id SMHeader               = 0;
const fstorage_section_id SMDictInfo             = 1;
const fstorage_section_id SMParadigms            = 2;
const fstorage_section_id SMParadigmForms        = 3;
const fstorage_section_id SMWordForms            = 4;
const fstorage_section_id SMFlexions             = 5;
const fstorage_section_id SMFlexDistribs         = 6;
const fstorage_section_id SMLexemes              = 7;
const fstorage_section_id SMLexGramDescList      = 8;
const fstorage_section_id SMLexStemsList         = 9;
const fstorage_section_id SMLexOrthVariants      = 10;
 
const fstorage_section_id SMStemsTree            = 11;
const fstorage_section_id SMStemsTreeHeader      = SMStemsTree;
const fstorage_section_id SMStemsTreeData        = SMStemsTree + 1;
const fstorage_section_id SMStemsTreeCrosslinks  = SMStemsTree + 2;

const fstorage_section_id SMExcludeChains        = 15;
const fstorage_section_id SMExcludeIndex         = 16;

#endif /* _STRICTMORPHFSTORAGEIDS_H_ */

