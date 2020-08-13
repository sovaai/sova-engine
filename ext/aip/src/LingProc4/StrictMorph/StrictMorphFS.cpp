/**
 * @file   StrictMorphFS.cpp
 * @author swizard <me@swizard.info>
 * @date   Tue May 27 00:21:59 2008
 * 
 * @brief  Strict (dictionary) morphology user interface implementation (fstorage version).
 * 
 * 
 */

#include "API/StrictMorphFS.h"
#include "StrictMorphShadowFS.h"

StrictMorphFS::StrictMorphFS() :
	StrictMorph()
{
}

StrictMorphFS::~StrictMorphFS()
{
}

StrictMorphErrors StrictMorphFS::load( const UCharSet           *charset,
                                       const StrictMorphParams  &params,
                                       fstorage			*fs,
                                       fstorage_section_id	 startSection,
                                       int			&underlyingError )
{
	StrictMorphShadowFS *shadowFS = new StrictMorphShadowFS();
	shadow = shadowFS;

	if ( unlikely(shadowFS == 0) )
		return STRICT_ERROR_INTERNAL_ERROR;
	
	return shadowFS->load(charset, params, fs, startSection, underlyingError);
}

