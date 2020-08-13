/**
 * @file   LanguageInfo.h
 * @author swizard <me@swizard.info>
 * @date   Fri Jun  6 16:09:12 2008
 * 
 * @brief  Language information tools.
 * 
 * 
 */
#ifndef _LANGUAGEINFO_H_
#define _LANGUAGEINFO_H_

#include <_include/_inttype.h>
#include <_include/language.h>

#include <lib/fstorage/fstorage.h>
#include <lib/fstorage/fstorage_id.h>

#include "LexID.h"

template< typename LexidType >
class LanguageInfoDb
{
public:
    LangCodes            language;          /**< The language itself */
    uint16_t             fuzzyNumber;       /**< Internal number for fuzzy morphology */
    uint16_t             strictNumber;      /**< Internal number for strict (dictionary) morphology */
    uint16_t             tildeNumber;       /**< Internal number for tilde (pseudo) morphology */
    uint16_t             tildeRevNumber;    /**< Internal number for tilde (pseudo, reversed) morphology */
    fstorage_section_id  fsectionOffset;    /**< Start section for morphology for fstorage serialization */
    const char          *shortName;         /**< The short name of the language */
    const char          *fullName;          /**< The full name of the language */
    float                avgWordSize;       /**< Average word size in octets in utf-8 text */
    
public:
    /** 
     * Returns the full information available for the language given.
     * 
     * @param lang A language for which to retrieve information
     * 
     * @return A language information
     */
    static const LanguageInfoDb< LexidType > *getLanguageInfo( LangCodes lang );
	
    /** 
     * Finds the full language information available by the short name given.
     * 
     * @param shortName A language short name (ru, en, ...)
     * 
     * @return A language information
     */
    static const LanguageInfoDb< LexidType > *getLanguageInfo( const char *shortName );

public: 
    /* Fstorage sections helpers */
	
    static fstorage_section_id getLPSection( fstorage_section_id langOffset )
    {
        return langOffset + static_cast<fstorage_section_id>(FSTORAGE_SECTION_LINGPROC);
    }

    static fstorage_section_id getLPSectionLang( fstorage_section_id langOffset )
    {
        return getLPSection(langOffset);
    }
	
    static fstorage_section_id getLPSectionDetect( fstorage_section_id langOffset )
    {
        return getLPSection(langOffset) + 0x10;
    }

    static fstorage_section_id getLPSectionCharset( fstorage_section_id langOffset )
    {
        return getLPSection(langOffset) + 0x20;
    }

    static fstorage_section_id getLPSectionFuzzy( fstorage_section_id langOffset )
    {
        return getLPSection(langOffset) + 0x30;
    }

    static fstorage_section_id getLPSectionStrict( fstorage_section_id langOffset )
    {
        return getLPSection(langOffset) + 0x40;
    }
        
    static fstorage_section_id getLPSectionTilde( fstorage_section_id langOffset )
    {
        return getLPSection(langOffset) + 0x60;
    }
        
    static fstorage_section_id getLPSectionTildeRev( fstorage_section_id langOffset )
    {
        return getLPSection(langOffset) + 0x70;
    }
};

typedef LanguageInfoDb< LEXID > LanguageInfo;

#endif /* _LANGUAGEINFO_H_ */
