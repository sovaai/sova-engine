#include "InternalFunction.hpp"
#include "IsEqual.hpp"
#include "IsNotEqual.hpp"
#include "IsNotEmpty.hpp"
#include "IsEmpty.hpp"
#include "Date.hpp"
#include "Time.hpp"
#include "DayOfWeek.hpp"
#include "CalcTime.hpp"
#include "BelongsTo.hpp"
#include "BelongsToExt.hpp"
#include "NotBelongsTo.hpp"
#include "MeasConv.hpp"
#include "StringToNumber.hpp"
#include "NumberToString.hpp"
#include "DumpVar.hpp"
#include "ListTree.hpp"
#include "FindContacts/FindPhone.hpp"
#include "FindContacts/FindEmail.hpp"
#include "In.hpp"
#include "NotIn.hpp"
#include "DictsMatch.hpp"
#include "DictsNotMatch.hpp"
#include "DictsMatchExt.hpp"
#include "True.hpp"
#include "Capitalize.hpp"
#include "Dec.hpp"
#include "Inc.hpp"
#include "SetValue.hpp"
#include "length.hpp"

namespace InternalFunctions
{

	const DLFunctionInfo * FUNCTIONS[] =
	{
		&InternalFunctions::BelongsTo::DLF_INFO,
		&InternalFunctions::NotBelongsTo::DLF_INFO,
		&InternalFunctions::BelongsToExt::DLF_INFO,
		&InternalFunctions::IsEqual::DLF_INFO,
		&InternalFunctions::IsNotEqual::DLF_INFO,
		&InternalFunctions::IsEmpty::DLF_INFO,
		&InternalFunctions::IsNotEmpty::DLF_INFO,
		&InternalFunctions::Date::DLF_INFO,
		&InternalFunctions::Time::DLF_INFO,
		&InternalFunctions::DayOfWeek::DLF_INFO,
		&InternalFunctions::CalcTime::DLF_INFO,
		&InternalFunctions::MeasConv::DLF_INFO,
		&InternalFunctions::StringToNumber::DLF_INFO,
		&InternalFunctions::NumberToString::DLF_INFO,
		&InternalFunctions::DumpVar::DLF_INFO,
		&InternalFunctions::ListTree::DLF_INFO,
		&InternalFunctions::FindPhone::DLF_INFO,
		&InternalFunctions::FindEmail::DLF_INFO,                
        &InternalFunctions::In::DLF_INFO,
        &InternalFunctions::NotIn::DLF_INFO,
        &InternalFunctions::DictsMatch::DLF_INFO,
        &InternalFunctions::DictsNotMatch::DLF_INFO,
        &InternalFunctions::DictsMatchExt::DLF_INFO,
        &InternalFunctions::True::DLF_INFO,
        &InternalFunctions::Capitalize::DLF_INFO,
        &InternalFunctions::Dec::DLF_INFO,
        &InternalFunctions::Inc::DLF_INFO,
        &InternalFunctions::SetValue::DLF_INFO,
		&InternalFunctions::Length::DLF_INFO
	};

	unsigned int FUNCTIONS_COUNT = sizeof (FUNCTIONS) / sizeof (const DLFunctionInfo*);


	void InitFunctions( ExtICoreRO & aBase )
	{
		InternalFunctions::BelongsTo::Init( aBase );
		InternalFunctions::NotBelongsTo::Init( aBase );
		InternalFunctions::BelongsToExt::Init( aBase );
		InternalFunctions::IsEqual::Init( aBase );
		InternalFunctions::IsNotEqual::Init( aBase );
		InternalFunctions::IsEmpty::Init( aBase );
		InternalFunctions::IsNotEmpty::Init( aBase );
		InternalFunctions::Date::Init( aBase );
		InternalFunctions::Time::Init( aBase );
		InternalFunctions::DayOfWeek::Init( aBase );
		InternalFunctions::CalcTime::Init( aBase );
		InternalFunctions::MeasConv::Init( aBase );
		InternalFunctions::StringToNumber::Init( aBase );
		InternalFunctions::NumberToString::Init( aBase );
		InternalFunctions::DumpVar::Init( aBase );
		InternalFunctions::ListTree::Init( aBase );
		InternalFunctions::FindPhone::Init( aBase );
		InternalFunctions::FindEmail::Init( aBase );
		InternalFunctions::In::Init( aBase );
		InternalFunctions::NotIn::Init( aBase );
		InternalFunctions::DictsMatch::Init( aBase );
		InternalFunctions::DictsNotMatch::Init( aBase );
		InternalFunctions::DictsMatchExt::Init( aBase );
        InternalFunctions::True::Init( aBase );
        InternalFunctions::Capitalize::Init( aBase );
        InternalFunctions::Inc::Init( aBase );
        InternalFunctions::Dec::Init( aBase );
        InternalFunctions::SetValue::Init( aBase );
	}

	FDLFucntion GetFunctionEntryPoint( const char * aFunctionName )
	{
		if( !strcasecmp( aFunctionName, "BelongsTo" ) )
			return InternalFunctions::BelongsTo::Apply;
		else if( !strcasecmp( aFunctionName, "NotBelongsTo" ) )
			return InternalFunctions::NotBelongsTo::Apply;
		else if( !strcasecmp( aFunctionName, "BelongsToExt" ) )
			return InternalFunctions::BelongsToExt::Apply;
		else if( !strcasecmp( aFunctionName, "IsEqual" ) )
			return InternalFunctions::IsEqual::Apply;
		else if( !strcasecmp( aFunctionName, "IsNotEqual" ) )
			return InternalFunctions::IsNotEqual::Apply;
		else if( !strcasecmp( aFunctionName, "IsEmpty" ) )
			return InternalFunctions::IsEmpty::Apply;
		else if( !strcasecmp( aFunctionName, "IsNotEmpty" ) )
			return InternalFunctions::IsNotEmpty::Apply;
		else if( !strcasecmp( aFunctionName, "Date" ) )
			return InternalFunctions::Date::Apply;
		else if( !strcasecmp( aFunctionName, "Time" ) )
			return InternalFunctions::Time::Apply;
		else if( !strcasecmp( aFunctionName, "DayOfWeek" ) )
			return InternalFunctions::DayOfWeek::Apply;
		else if( !strcasecmp( aFunctionName, "CalcTime" ) )
			return InternalFunctions::CalcTime::Apply;
		else if( !strcasecmp( aFunctionName, "MeasConv" ) )
			return InternalFunctions::MeasConv::Apply;
		else if( !strcasecmp( aFunctionName, "StringToNumber" ) )
			return InternalFunctions::StringToNumber::Apply;
		else if( !strcasecmp( aFunctionName, "NumberToString" ) )
			return InternalFunctions::NumberToString::Apply;
		else if( !strcasecmp( aFunctionName, "DumpVar" ) )
			return InternalFunctions::DumpVar::Apply;
		else if( !strcasecmp( aFunctionName, "ListTree" ) )
			return InternalFunctions::ListTree::Apply;
		else if( !strcasecmp( aFunctionName, "FindPhone" ) )
			return InternalFunctions::FindPhone::Apply;
		else if( !strcasecmp( aFunctionName, "FindEmail" ) )
            return InternalFunctions::FindEmail::Apply;
        else if( !strcasecmp( aFunctionName, "In" ) )
            return InternalFunctions::In::Apply;
        else if( !strcasecmp( aFunctionName, "NotIn" ) )
            return InternalFunctions::NotIn::Apply;
        else if( !strcasecmp( aFunctionName, "DictsMatch" ) )
            return InternalFunctions::DictsMatch::Apply;
        else if( !strcasecmp( aFunctionName, "DictsNotMatch" ) )
            return InternalFunctions::DictsNotMatch::Apply;
        else if( !strcasecmp( aFunctionName, "DictsMatchExt" ) )
            return InternalFunctions::DictsMatchExt::Apply;
        else if( !strcasecmp( aFunctionName, "True" ) )
            return InternalFunctions::True::Apply;
        else if( !strcasecmp( aFunctionName, "Capitalize" ) )
            return InternalFunctions::Capitalize::Apply;
        else if( !strcasecmp( aFunctionName, "Inc" ) )
            return InternalFunctions::Inc::Apply;
        else if( !strcasecmp( aFunctionName, "Dec" ) )
            return InternalFunctions::Dec::Apply;
        else if( !strcasecmp( aFunctionName, "SetValue" ) )
            return InternalFunctions::SetValue::Apply;
		else if( !strcasecmp( aFunctionName, "Length" ) )
			return InternalFunctions::Length::Apply;

		return nullptr;
	}
    
    unsigned int GetFunctionsNumber() 
    {
        return FUNCTIONS_COUNT;
    }
    
    const DLFunctionInfo *GetFunctionInfo( unsigned int aFunctionN )
    {
        return aFunctionN < FUNCTIONS_COUNT ? FUNCTIONS[aFunctionN] : nullptr;
    }

	const DLFunctionInfo *GetFunctionInfo( const char * aFunctionName )
	{
		for( unsigned int func_n = 0; func_n < FUNCTIONS_COUNT; ++func_n )
			if( !strcasecmp( aFunctionName, FUNCTIONS[func_n]->name ) )
				return FUNCTIONS[func_n];

		return nullptr;
	}

}
