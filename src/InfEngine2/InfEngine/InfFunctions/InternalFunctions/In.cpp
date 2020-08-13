#include "In.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo In::ARGS[] = {
    { DLFAT_VARIABLE, 0 },    
};
    
const DLFucntionArgInfo In::VAR_ARGS = { DLFAT_EXTENDED, 0 };
    
const DLFunctionInfo In::DLF_INFO = {
    "In",
    "",
    "Check if variable value gived in first parameter is equal to at least one of strings gived in other parameters.",
    DLFRT_BOOL,
    1,
    In::ARGS,
    { 1, 0, 0 },
    { 1, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,
    InfDataProtocolVersion,
    &VAR_ARGS,
};

ExtICoreRO * In::vBase = nullptr;

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;


bool In::_apply( const char * aSerializedVarId, const char ** aStrings, FunctionContext * aFunctionContext )
{
	if( !aSerializedVarId )
		return false;

	Vars::Id var_id{ aSerializedVarId };
	Vars::Vector::VarValue var = aFunctionContext->vSession->GetValue( var_id );
	if( !var )
		return false;
	const char * aTheWord = var.AsText().GetValue();

	if( !aTheWord )
		return false;
    
    // Поиск заданного текста в заданных строках.
    for( const char ** s = aStrings; *s; ++s )
        if( !strcmp( aTheWord, *s ) )
            return true;
    
    // Совпадений нет.
    return false;
}

InfEngineErrors In::Apply( const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * /*aMemoryAllocator*/, 
        FunctionContext * aFunctionContext )
{    
	if( !vBase )
        return INF_ENGINE_ERROR_FUNC_NOT_INITIALIZED;
    
    if( !aArgs[0] || !aArgs[1] )
        return INF_ENGINE_ERROR_ARGC;
    
    bool is_in = _apply( aArgs[0], aArgs + 1, aFunctionContext );            
            
    // Подготовка результата.
    if( is_in )
    {
        aResult = CTRUE;
        aLength = CTRUE_LEN;
    }
    else
    {
        aResult = CFALSE;
        aLength = CFALSE_LEN;
    }
    
    return INF_ENGINE_SUCCESS;
}
