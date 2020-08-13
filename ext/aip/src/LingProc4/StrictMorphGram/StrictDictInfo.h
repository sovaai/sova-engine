/**
 * @file   StrictDictInfo.h
 * @author swizard <me@swizard.info>
 * @date   Fri Oct 30 23:34:50 2009
 * 
 * @brief  Strict morphology dictionary file data.
 * 
 * 
 */
#ifndef _STRICTDICTINFO_H_
#define _STRICTDICTINFO_H_

#include <time.h>
#include <stdlib.h>

struct StrictDictInfo
{
    size_t paradigmsCount;
    size_t paradigmFormsCount;
    size_t wordFormsCount;
    size_t flexCount;
    size_t flexDistribCount;
    size_t maxFlexLength;
    size_t lexemesCount;
    size_t lexGramDescListLength;
    size_t lexStemsListLength;
    size_t lexOrthVarsListLength;
    size_t excludeChainsLength;
    time_t datasetBuildTime;

public:
    enum Constants
    {
        infoFieldsCount = 12
    };

public:
    StrictDictInfo() :
	paradigmsCount( 0 ),
	paradigmFormsCount( 0 ),
	wordFormsCount( 0 ),
	flexCount( 0 ),
	flexDistribCount( 0 ),
	lexemesCount( 0 ),
	lexGramDescListLength( 0 ),
	lexStemsListLength( 0 ),
	lexOrthVarsListLength( 0 ),
        excludeChainsLength( 0 ),
        datasetBuildTime( 0 )
    { }
};

#endif /* _STRICTDICTINFO_H_ */

