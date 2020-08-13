/**
 * @file   FormFilter.h
 * @author swizard <me@swizard.info>
 * @date   Sun Oct 25 19:13:49 2009
 * 
 * @brief  Filtering settings for paradigm synthesing.
 * 
 * 
 */
#ifndef _FORMFILTER_H_
#define _FORMFILTER_H_

#include "FormGram.h"

struct SM_DLL FormFilter
{
    FormGram::Mstform  filterMstform;
    FormGram::Verbform filterVerbform;
    FormGram::Tense    filterTense;
    FormGram::Short    filterShort;
    FormGram::Refl     filterRefl;
    FormGram::Person   filterPerson;
    FormGram::Number   filterNumber;
    FormGram::Mood     filterMood;
    FormGram::Gender   filterGender;
    FormGram::Degree   filterDegree;
    FormGram::Case     filterCase;
    FormGram::Animate  filterAnimate;

    FormFilter() : 
        filterMstform( FormGram::MSTFORM_LAST ),
        filterVerbform( FormGram::VERBFORM_LAST ),
        filterTense( FormGram::TENSE_LAST ),
        filterShort( FormGram::SHORT_LAST ),
        filterRefl( FormGram::REFL_LAST ),
        filterPerson( FormGram::PERSON_LAST ),
        filterNumber( FormGram::NUMBER_LAST ),
        filterMood( FormGram::MOOD_LAST ),
        filterGender( FormGram::GENDER_LAST ),
        filterDegree( FormGram::DEGREE_LAST ),
        filterCase( FormGram::CASE_LAST ),
        filterAnimate( FormGram::ANIMATE_LAST ) { }
};

#endif /* _FORMFILTER_H_ */

