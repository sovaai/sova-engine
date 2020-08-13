#include "Dec.hpp"
#include "InfEngine2/_Include/Errors.h"
#include <NanoLib/NumberConvertor.hpp>
#include <InfEngine2/InfEngine/Vars/Vector.hpp>
#include "InternalFunction.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo Dec::ARGS[] = {
	{ DLFAT_VARIABLE, nullptr, DLFAA_NONE },
	{ DLFAT_EXTENDED, "1", DLFAA_NONE }
};

const DLFunctionInfo Dec::DLF_INFO = {
	"Dec",
	"",
	"Adds integer value given in second parameter to variable given in first parameter.",
	DLFRT_TEXT,
	2,
	ARGS,
	{ 3, 0, 1 },
	{ 3, 0, 1 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

ExtICoreRO* Dec::vBase = nullptr;

static const char EMPTY[] = "";
static const unsigned int EMPTY_LEN = 0;

InfEngineErrors Dec::Apply( const char ** aArgs,
							const char* & aResult,
							unsigned int & aLength,
							nMemoryAllocator * aMemoryAllocator,
							FunctionContext * aFunctionContext  )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	if( !( ( aArgs[0] && !aArgs[1] ) || ( aArgs[0] && aArgs[1] && !aArgs[2] ) ) )
		return INF_ENGINE_ERROR_ARGC;

	// Конвертор чисел.
	static NumberConvertor Convertor;

	Vars::Id var_id { aArgs[0] };
	Vars::Vector::VarValue var = aFunctionContext->vSession->GetValue( var_id );
	

	LongFloat sum = 0;
	if( !var.IsNull() && var.AsText().GetValue() )
		if( Convertor.s2lf( var.AsText().GetValue(), sum ) != nlrcSuccess )
			return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;

	LongFloat inc = 1;
	if( aArgs[1] && aArgs[1][0] != '\0' )
		if( Convertor.s2lf( aArgs[1], inc ) != nlrcSuccess )
			return INF_ENGINE_ERROR_DLF_SKIP_ANSWER;

	sum = sum - inc;

	char * buffer = static_cast<char*>( aMemoryAllocator->Allocate( 256 ) );
	if( !buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	unsigned int len = sum.snprintf( buffer, 255, 20 );

	if( var_id.is_tmp() )
	{
		if( INF_ENGINE_SUCCESS != ( iee = aFunctionContext->vSession->SetTextValue( var_id, buffer, len ) ) )
			return iee;
	}
	else
	{
		if( INF_ENGINE_SUCCESS != ( iee = aFunctionContext->vAnswer->AddInstruct( var_id, buffer, len, false ) ) )
			return iee;
	}

	aLength = EMPTY_LEN;
	aResult = EMPTY;

	return INF_ENGINE_SUCCESS;
}
