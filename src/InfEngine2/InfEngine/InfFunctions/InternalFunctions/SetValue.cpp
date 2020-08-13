#include "SetValue.hpp"
#include "InfEngine2/_Include/Errors.h"
#include <NanoLib/NumberConvertor.hpp>
#include "InternalFunction.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo SetValue::ARGS[] = {
	{ DLFAT_VARIABLE, nullptr, DLFAA_NONE },
	{ DLFAT_ANY, nullptr, DLFAA_NONE },
	{ DLFAT_TEXT, "", DLFAA_NONE }
};

const DLFunctionInfo SetValue::DLF_INFO = {
	"SetValue",
	"Set",
	"Set variable value. Print new value if third parametr is empty.",
	DLFRT_TEXT,
	3,
	ARGS,
	{ 3, 0, 1 },
	{ 3, 0, 1 },
	DLFO_NONE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

ExtICoreRO* SetValue::vBase = nullptr;

static const char EMPTY[] = "";
static const unsigned int EMPTY_LEN = 0;

InfEngineErrors SetValue::Apply( const char ** aArgs,
								 const char* & aResult,
								 unsigned int & aLength,
								 nMemoryAllocator * aMemoryAllocator,
								 FunctionContext * aFunctionContext  )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	if( !( ( aArgs[0] && aArgs[1] && !aArgs[2] ) || ( aArgs[0] && aArgs[1] && aArgs[2] && !aArgs[3] ) ) )
		return INF_ENGINE_ERROR_ARGC;

	aLength = strlen( aArgs[1] );
	char * buffer = static_cast<char*>( aMemoryAllocator->Allocate( aLength + 1 ) );
	if( !buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	strcpy( buffer, aArgs[1] );

	Vars::Id var_id { aArgs[0] };
	Vars::Vector::VarValue var = aFunctionContext->vSession->GetValue( var_id );

	if( var.IsNull() )
	{
		if( !aFunctionContext->vSession->VarExists( var_id ) )
			return INF_ENGINE_ERROR_INTERNAL;

		if( INF_ENGINE_SUCCESS != aFunctionContext->vSession->SetTextValue( var_id, "", 0 ) )
			throw std::bad_alloc();
		var = aFunctionContext->vSession->GetValue( var_id );
		if( !var )
			return INF_ENGINE_ERROR_INTERNAL;
	}

	if( var_id.is_tmp() )
	{
		if( INF_ENGINE_SUCCESS != ( iee = aFunctionContext->vSession->SetTextValue( var_id, buffer, aLength ) ) )
			return iee;
	}
	else
	{
		if( INF_ENGINE_SUCCESS != ( iee = aFunctionContext->vAnswer->AddInstruct( var_id, buffer, aLength, false ) ) )
			return iee;
	}

	if( aArgs[2] && aArgs[2][0] != '\0' )
	{
		aResult = EMPTY;
		aLength = EMPTY_LEN;
	}
	else
	{
		aResult = buffer;
	}

	return INF_ENGINE_SUCCESS;
}
