#include "InfCompiler.hpp"

#include <fcntl.h>
#include <cmath>

#include <InfEngine2/_Include/ConsoleOutput.hpp>

InfEngineErrors InfCompiler::Create( )
{
	// Создание ядра компиляции.
	PrintHeader( "Create compiler" );

	InfEngineErrors iee = vCoreWR.Create( );
	if( iee != INF_ENGINE_SUCCESS )
	{
		PrintError( "Can't create compiler.\n" );

		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::Save( const char * aFilePath, const unsigned char aSignature[16] )
{
	// Проверка аргументов.
	if( !aFilePath )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Открытие fstorage.
	fstorage* fscmn = fstorage_create( );
	if( !fscmn )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't create fstorage." );

	fstorage_connect_config fscc;
	memset( &fscc, 0, sizeof ( fstorage_connect_config ) );
	fscc.pid = FSTORAGE_PID_INF_ENGINE;
	int ret = fstorage_connect( fscmn, aFilePath, O_RDWR | O_CREAT | O_TRUNC, 0644, FSTORAGE_OPEN_READ_WRITE, &fscc );
	if( ret != 0 )
	{
		fstorage_destroy( fscmn );

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't open fstorage. Return code: %d", ret );
	}

	// Установка подписи ядра.
	vCoreWR.SetSignature( aSignature );

	// Сохранение ядра.
	InfEngineErrors iee = vCoreWR.Save( fscmn );
	if( iee != INF_ENGINE_SUCCESS )
	{
		fstorage_close( fscmn );
		fstorage_destroy( fscmn );

		ReturnWithTrace( iee );
	}

	// Закрытие fstorage.
	fstorage_close( fscmn );
	fstorage_destroy( fscmn );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::CompilePatternsFromFile( const std::string & aFilePath, NanoLib::Encoding aenc )
{
	printf( "      file: %s", aFilePath.c_str() );

	unsigned int SuccessPatterns = 0;
	unsigned int FaultPatterns = 0;
	InfEngineErrors iee = vCoreWR.CompilePatternsFromFile( aFilePath, SuccessPatterns, FaultPatterns, aenc );

	if( vCoreWR.GetWarnings( ).size( ) )
	{
		for( unsigned int i = 0; i < vCoreWR.GetWarnings( ).size( ); i++ )
		{
			printf( "\n        WARNING: %s\n", vCoreWR.GetWarnings( )[i].description() );
			printf( "        Id: %s\n", vCoreWR.GetWarnings( )[i].GetObjectId().c_str() );
			if( vCoreWR.GetWarnings( )[i].GetString().size() )
                printf( "        STRING: %s\n", vCoreWR.GetWarnings( )[i].GetString().c_str() + vCoreWR.GetWarnings( )[i].GetPos() );
		}
	}

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
	{
		for( unsigned int i = 0; i < vCoreWR.GetErrors( ).size( ); i++ )
		{
			printf( "\n        ERROR: %s\n", vCoreWR.GetErrors( )[i].description() );
			printf( "        Id: %s\n", vCoreWR.GetErrors( )[i].GetObjectId().c_str() );
			if( vCoreWR.GetErrors( )[i].GetString().size() )
                printf( "        STRING: %s\n", vCoreWR.GetErrors( )[i].GetString().c_str() + vCoreWR.GetErrors( )[i].GetPos() );
		}

		printf( "      [WARN]    %u from %u patterns are not compiled( %d%% )\n\n", FaultPatterns, SuccessPatterns + FaultPatterns, (int)ceil( (double)( 100 * FaultPatterns ) / ( SuccessPatterns + FaultPatterns ) ) );

		return INF_ENGINE_ERROR_CANT_PARSE;
	}
	else if( iee != INF_ENGINE_SUCCESS )
	{
		PrintError( "Internal error." );

		ReturnWithTrace( iee );
	}
	else
	{
		PrintSuccess( " " );

		return INF_ENGINE_SUCCESS;
	}
}

InfEngineErrors InfCompiler::CompilePatternsFromBuffer( const char * aBuffer, unsigned int aBufferSize, bool aIgnoreErrors )
{
	InfEngineErrors iee = vCoreWR.CompilePatternsFromBuffer( aBuffer, aBufferSize, aIgnoreErrors );

	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::CompileAliasesFromFile( const char * aFilePath )
{
	unsigned int SuccessAliases = 0;
	unsigned int FaultAliases = 0;

	AliasParser::Errors Errors;

	InfEngineErrors iee = vCoreWR.CompileAliasesFromFile( aFilePath, SuccessAliases, FaultAliases, Errors );

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
	{
		for( unsigned int err_n = 0; err_n < Errors.size( ); ++err_n )
		{
			AliasParser::Error & Error = Errors[err_n];
			if( Error.vAliasName )
				printf( "\n      ALIAS: %s\n", Error.vAliasName );
			else
				printf( "\n      ALIAS: unknown\n" );
			printf( "      ERROR: %s\n", Error.description() );
			if( Error.vStringLength )
				printf( "      STRING: %s.\n\n", Error.vString );
		}

		printf( "   %u from %u aliases are not compiled( %d%% )\n\n",
				FaultAliases, SuccessAliases + FaultAliases,
				(int)ceil( (double)( 100 * FaultAliases ) / ( SuccessAliases + FaultAliases ) ) );

		return INF_ENGINE_ERROR_CANT_PARSE;
	}
	else if( iee != INF_ENGINE_SUCCESS )
	{
		printf( "\n    ERROR: Internal error: %i.\n", iee );

		ReturnWithTrace( iee );
	}
	else
	{
		PrintSuccess( "%s", "Aliases have been successfully compiled." );

		return INF_ENGINE_SUCCESS;
	}
}

InfEngineErrors InfCompiler::RegistryDictName( const char * aDictName, unsigned int aDicNameLength )
{
	InfEngineErrors iee = vCoreWR.RegistryDictName( aDictName, aDicNameLength );

	if( INF_ENGINE_SUCCESS != iee )
	{
		if( INF_ENGINE_WARN_UNSUCCESS == iee )
			return INF_ENGINE_WARN_UNSUCCESS;
		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::CompileDictFromFile( const char * aFilePath, const char * aDictName, unsigned int aDictNameLength,
												  aTextString & aErrorDescription, aTextString & aErrorString, NanoLib::Encoding aEncoding )
{
	InfEngineErrors iee = vCoreWR.CompileDictFromFile( aFilePath, aDictName, aDictNameLength, aErrorDescription, aErrorString, aEncoding );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::AddVariable( const char * aVarName, unsigned int aVarNameLength, const char * aVarValue, unsigned int aVarValueLength )
{
	Vars::Id VarId;
	InfEngineErrors iee = vCoreWR.RegistryNewVariable( aVarName, aVarNameLength, VarId );
	if( iee == INF_ENGINE_WARN_UNSUCCESS || iee == INF_ENGINE_WARN_ELEMENT_EXISTS )
		return iee;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return vCoreWR.SetDefaultVariableValue( VarId, aVarValue, aVarValueLength );
}

InfEngineErrors InfCompiler::AddFunction( const char * aFuncPath )
{
	unsigned int FuncId;
	InfEngineErrors iee = vCoreWR.RegistryNewFunction( aFuncPath, FuncId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::AddFunction( const DLFunctionInfo * aFuncInfo )
{
	unsigned int FuncId;
	InfEngineErrors iee = vCoreWR.RegistryNewFunction( aFuncInfo, FuncId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::AddSynonymsDict( const char * aDictName, unsigned int aDictNameLen, const char * aDictPath, NanoLib::Encoding aEncoding )
{
	aTextString Errors;

	InfEngineErrors iee = vCoreWR.CompileSynonymsFromFile( aDictName, aDictNameLen, aDictPath, Errors, aEncoding );
	if( iee != INF_ENGINE_SUCCESS )
	{
		if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		{
			printf( "      ERROR: %s\n", Errors.ToConstChar() );
			return INF_ENGINE_WARN_UNSUCCESS;
		}
		else if( iee == INF_ENGINE_ERROR_FILE )
		{
			printf( "      ERROR: failed to open file \"%s\"\n", aDictName );
			return INF_ENGINE_WARN_UNSUCCESS;
		}
		else if( iee == INF_ENGINE_WARN_UNSUCCESS )
		{
			printf( "      WARNING: %s\n", Errors.ToConstChar() );
			return INF_ENGINE_WARN_UNSUCCESS;
		}
		else
			ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::FinalizeSynonyms()
{
	InfEngineErrors iee = vCoreWR.FinalizeSynonyms();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::FinalizeSymbolyms()
{
	InfEngineErrors iee = vCoreWR.FinalizeSymbolyms();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::AddSymbolymsDict( const char * aDictName, unsigned int aDictNameLen, const char * aDictPath, NanoLib::Encoding aEncoding )
{
	aTextString Errors;
	InfEngineErrors iee = vCoreWR.CompileSymbolymsFromFile( aDictName, aDictNameLen, aDictPath, Errors, aEncoding );
	if( iee != INF_ENGINE_SUCCESS )
	{
		if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		{
			printf( "      ERROR: %s\n", Errors.ToConstChar() );
			return INF_ENGINE_WARN_UNSUCCESS;
		}
		else if( iee == INF_ENGINE_ERROR_FILE )
		{
			printf( "      ERROR: failed to open file \"%s\"\n", aDictName );
			return INF_ENGINE_WARN_UNSUCCESS;
		}
		else if( iee == INF_ENGINE_WARN_UNSUCCESS )
		{
			printf( "      WARNING: %s\n", Errors.ToConstChar() );
			return INF_ENGINE_WARN_UNSUCCESS;
		}
		else
			ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::AddSymbolymGroup( const SynonymParser::SynonymGroup & aGroup )
{
    aTextString Errors;
    InfEngineErrors iee = vCoreWR.AddSymbolymGroup( aGroup, Errors );
    if( iee != INF_ENGINE_SUCCESS )
    {
        if( iee == INF_ENGINE_WARN_UNSUCCESS )
        {
            printf( "      WARNING: %s\n", Errors.ToConstChar() );
            return INF_ENGINE_WARN_UNSUCCESS;
        }
        else
            ReturnWithTrace( iee );
    }

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfCompiler::MakeResolving()
{
	printf( "      resolving dependencies" );

	unsigned int SuccessResolving = 0;
	unsigned int FaultResolving = 0;
	InfEngineErrors iee = vCoreWR.MakeResolving( SuccessResolving, FaultResolving );
	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
	{
		for( unsigned int i = 0; i < vCoreWR.GetResolvigErrors( ).size( ); i++ )
		{
			printf( "\n        ERROR: %s\n", ConvertErrorDescriptionToString( vCoreWR.GetResolvigErrors()[i].vCode ) );
			printf( "        File: %s\n", vCoreWR.GetResolvigErrors()[i].vFileName.c_str() );
			printf( "        Id: %s\n", vCoreWR.GetResolvigErrors()[i].vPatternId.c_str() );
			if( !vCoreWR.GetResolvigErrors()[i].vString.empty() )
				printf( "        STRING: %s\n", vCoreWR.GetResolvigErrors()[i].vString.c_str() );
		}

		printf( "      [WARN]    %u from %u dependencies are not resolved ( %d%% )\n\n", FaultResolving, SuccessResolving + FaultResolving,
				(int)ceil( (double)( 100 * FaultResolving ) / ( SuccessResolving + FaultResolving ) ) );

		return INF_ENGINE_ERROR_CANT_PARSE;
	}
	else if( iee != INF_ENGINE_SUCCESS )
	{
		PrintError( "Internal error." );

		ReturnWithTrace( iee );
	}
	else
	{
		PrintSuccess( " " );

		return INF_ENGINE_SUCCESS;
	}
}
