#include "CoreWR.hpp"

#include <cmath>

#include "Signature/SignatureUtils.hpp"

BCoreWR::BCoreWR( const BCoreRO * aMainCore, BaseIndexer & aInfIndexer,
                  SymbolymBaseRO * aSymbolymsBaseRO, SynonymBaseRO * aSynonymBaseRO,
                  const char * aTmpFilePath2, unsigned int aMemoryLimit ) :
	vIndexer( aInfIndexer ),
	vDLData( aMainCore ? &( aMainCore->GetDLData() ) : nullptr, aTmpFilePath2, aMemoryLimit ),
    vDLParser( vDLData, aInfIndexer, aSymbolymsBaseRO, aSynonymBaseRO, vDataAllocator ),
	vAliasParser( vDLData,  vAliasAllocator ),
	vSynonymParser( vSynonymAllocator ) {}

InfEngineErrors BCoreWR::Create()
{
	// Проверка состояния.
	if( vState != stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	InfEngineErrors iee = vDLData.Create();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BCoreWR::Save( fstorage* aFStorage )
{
	// Проверка состояния ядра.
	if( vState == stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	InfEngineErrors iee = vDLData.Save( aFStorage );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

void BCoreWR::Destroy()
{
	vDLData.Destroy();

	vDLParser.Reset();

	vDataAllocator.Reset();

	vRuntimeAllocator.Reset();

	vSynonymParser.Reset();

	vSynonymAllocator.Reset();

	vState = stClosed;
}

InfEngineErrors BCoreWR::FinalizeSynonyms()
{
	InfEngineErrors iee = vIndexer.FinalizeSynonyms();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BCoreWR::FinalizeSymbolyms()
{
	InfEngineErrors iee = vIndexer.FinalizeSymbolyms();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

#define SetErrorAdv( aVerbose, aFormat,  ... ) \
	{ \
		if( aVerbose ) \
		{ \
			printf( "\n [FAILED] " ); \
			printf( aFormat, __VA_ARGS__ ); \
			printf( "\n\n" ); \
		} \
	\
		LogError( aFormat, __VA_ARGS__ ); \
	\
		LogInfo( "   %s", __PRETTY_FUNCTION__ ); \
		LogInfo( "     File: %s [ %d ]", __FILE__, __LINE__ ); \
	}


EliCoreWR::EliCoreWR( const char * aTmpFilePath2, unsigned int aMemoryLimit ) :
    BCoreWR( nullptr, vIndexer, nullptr, nullptr, aTmpFilePath2, aMemoryLimit ),
	vIndexer( vDataAllocator )
{}

InfEngineErrors EliCoreWR::Create()
{
	// Создание базовых элементов.
	InfEngineErrors iee = BCoreWR::Create();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Создание индексатора для шаблонов эллипсисов.
	iee = vIndexer.Create();
	if( iee != INF_ENGINE_SUCCESS )
	{
		Destroy();

		ReturnWithTrace( iee );
	}

	vState = stReady;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors EliCoreWR::Save( fstorage* aFStorage )
{
	InfEngineErrors iee = BCoreWR::Save( aFStorage );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = vIndexer.Save( aFStorage );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Создание сигнатуры для лингвистических данных.
	Signature * signature = nullptr;
	nMemoryAllocator aAllocator;
	if( INF_ENGINE_SUCCESS != ( iee = BuildDLDataSignature( signature, vDLData, aAllocator ) ) )
		ReturnWithTrace( iee );
	// Сохранение сигнатуры для лингвистических данных.
	if( INF_ENGINE_SUCCESS != ( iee = signature->Save( aFStorage ) ) )
		ReturnWithTrace( iee );
	aAllocator.Reset();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors EliCoreWR::CompilePatternFromBuffer( const char * aBuffer, unsigned int aBufferSize, const aTextString & aPatternName, bool aStrictMode )
{
	// Проверка аргументов.
	if( !aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка на пустой шаблон.
	if( aBufferSize == 0 )
		return INF_ENGINE_SUCCESS;

	// Очистка вспомогательных переменных.
	vPattern1.clear();
	vPattern2.clear();
	vPattern3.clear();

	const char * begin = aBuffer;
	unsigned int stage = 0;

	// Преобразование шаблона эллипсисов в 3 стандартных шаблона.
	while( 1 )
	{
		// Поиск конца строки.
		const char * end = strchr( begin, '\n' );
		if( !end )
			end = aBuffer+aBufferSize;

		if( stage <= 1 && end-begin >= 3 && begin[0] == '$' && begin[1] == '-' && begin[2] == '-' )
		{
			// Разбор строки с характеристикой удаленности: 2.

			if( vPattern1.empty() &&
				( vPattern1.assign( "BEGIN 1 " ) != nlrcSuccess ||
				  vPattern1.append( aPatternName ) != nlrcSuccess ||
				  vPattern1.append( "\n" ) != nlrcSuccess ) )
			{
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			if( vPattern1.append( "$" ) != nlrcSuccess ||
				vPattern1.append( begin+3, (end-begin)-3 ) != nlrcSuccess ||
				vPattern1.append( "\n" ) != nlrcSuccess )
			{
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			stage = 1;
		} else if (stage <= 2 && end - begin >= 2 && begin[0] == '$' && begin[1] == '-')
		{
			// Создание первого шаблона, если он не был создан.
			if( stage == 0 )
			{
				if( vPattern1.assign( "BEGIN 1 " ) != nlrcSuccess ||
					vPattern1.append( aPatternName ) != nlrcSuccess ||
					vPattern1.append( "\n$ *\n" ) != nlrcSuccess )
				{
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
			}

			// Разбор строки с характеристикой удаленности: 1.
			if( vPattern2.empty() && (
					vPattern2.assign( "BEGIN 2 " ) != nlrcSuccess ||
					vPattern2.append( aPatternName ) != nlrcSuccess ||
					vPattern2.append( "\n" ) != nlrcSuccess ) )
			{
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			if( vPattern2.append( "$" ) != nlrcSuccess ||
				vPattern2.append( begin+2, (end-begin)-2 ) != nlrcSuccess ||
				vPattern2.append( "\n" ) != nlrcSuccess )
			{
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			stage = 2;
		} else if (stage <= 3 && ((end - begin == 1 && begin[0] == '$') ||
								  ( end-begin >= 2 && begin[0] == '$' && begin[1] != '+' ) ) )
		{
			// Создание первого шаблона, если он не был создан.
			if( stage == 0 )
			{
				if( vPattern1.assign( "BEGIN 1 " ) != nlrcSuccess ||
					vPattern1.append( aPatternName ) != nlrcSuccess ||
					vPattern1.append( "\n$ *\n" ) != nlrcSuccess )
				{
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
			}

			// Создание второго шаблона, если он не был создан.
			if( stage < 2 )
			{
				if( vPattern2.assign( "BEGIN 2 " ) != nlrcSuccess ||
					vPattern2.append( aPatternName ) != nlrcSuccess ||
					vPattern2.append( "\n$ *\n" ) != nlrcSuccess )
				{
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
			}

			// Разбор строки с характеристикой удаленности: 0.
			if( vPattern3.empty() && (
					vPattern3.assign( "BEGIN 3 " ) != nlrcSuccess ||
					vPattern3.append( aPatternName ) != nlrcSuccess ||
					vPattern3.append( "\n" ) != nlrcSuccess ) )
			{
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			if( vPattern3.append( "$" ) != nlrcSuccess ||
				vPattern3.append( begin+1, (end-begin)-1 ) != nlrcSuccess ||
				vPattern3.append( "\n" ) != nlrcSuccess )
			{
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			stage = 3;
		} else if (stage <= 4 && end - begin >= 2 && begin[0] == '$' && begin[1] == '+')
		{
			// Создание первого шаблона, если он не был создан.
			if( stage == 0 )
			{
				if( vPattern1.assign( "BEGIN 1 " ) != nlrcSuccess ||
					vPattern1.append( aPatternName ) != nlrcSuccess ||
					vPattern1.append( "\n$ *\n" ) != nlrcSuccess )
				{
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
			}

			// Создание второго шаблона, если он не был создан.
			if( stage < 2 )
			{
				if( vPattern2.assign( "BEGIN 2 " ) != nlrcSuccess ||
					vPattern2.append( aPatternName ) != nlrcSuccess ||
					vPattern2.append( "\n$ *\n" ) != nlrcSuccess )
				{
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
			}

			// Создание третьего шаблона, если он не был создан.
			if( stage < 3 )
			{
				if( vPattern3.assign( "BEGIN 3 " ) != nlrcSuccess ||
					vPattern3.append( aPatternName ) != nlrcSuccess ||
					vPattern3.append( "\n$ *\n" ) != nlrcSuccess )
				{
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
			}

			// Разбор строки с результатом.
			if( vPattern3.append( "#" ) != nlrcSuccess ||
				vPattern3.append( begin+2, (end-begin)-2 ) != nlrcSuccess ||
				vPattern3.append( "\n" ) != nlrcSuccess )
			{
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			stage = 4;
		}
		else if (end - begin)
		{
			DLParser::Error err;
			if( !err.assign( { DLErrorId::peInvalidPatternString, 0 } ) ||
				!err.assign_string( begin, aBufferSize-(begin-aBuffer) ) ||
				!err.assign_object_id( aPatternName.c_std_str() ) ||
				!vCompilerErrors.push_back( std::move( err ) ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			return INF_ENGINE_ERROR_CANT_PARSE;
		}

		// Достигнут конец буффера.
		if( (unsigned int)(end - aBuffer) >= aBufferSize )
		{
			if( stage != 4 )
			{
				DLParser::Error err;
				if( !err.assign( { DLErrorId::peInvalidPatternString, 0 } ) ||
					!err.assign_string( begin, aBufferSize - ( begin - aBuffer ) ) ||
					!err.assign_object_id( aPatternName.c_std_str() ) ||
					!vCompilerErrors.push_back( std::move( err ) ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				return INF_ENGINE_ERROR_CANT_PARSE;
			}

			break;
		}

		// Переход к следующей строке.
		begin = end+1;
	}

	if( vPattern1.append( "# 1\nEND\n" ) != nlrcSuccess ||
		vPattern2.append( "# 2\nEND\n" ) != nlrcSuccess ||
		vPattern3.append( "\nEND\n" ) != nlrcSuccess )
	{
		SetErrorAdv( 0, "%s", INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	}

	// Компиляция первого шаблона.
	vIndexer.SetMode( EllipsisIndexer::etFirst );
	vCompilerErrors.clear();
	vCompilerWarnings.clear();
	InfEngineErrors iee = vDLParser.ReadPatternsFromBuffer( vPattern1.ToConstChar(), vPattern1.size(), true, aStrictMode );
	if( vDLParser.ExtractEW( vCompilerErrors, vCompilerWarnings ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( INF_ENGINE_SUCCESS );
	if( iee != INF_ENGINE_SUCCESS )
	{
		if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			// Ошибка в шаблоне.
			return iee;
		else
			ReturnWithTrace( iee );
	}


	// Компиляция второго шаблона.
	vIndexer.SetMode( EllipsisIndexer::etSecond );
	iee = vDLParser.ReadPatternsFromBuffer( vPattern2.ToConstChar(), vPattern2.size(), true, aStrictMode );
	if( vDLParser.ExtractEW( vCompilerErrors, vCompilerWarnings ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( INF_ENGINE_SUCCESS );
	if( iee != INF_ENGINE_SUCCESS )
	{
		if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			// Ошибка в шаблоне.
			return iee;
		else
			ReturnWithTrace( iee );
	}

	// Компиляция третьго шаблона.
	vIndexer.SetMode( EllipsisIndexer::etThird );
	iee = vDLParser.ReadPatternsFromBuffer( vPattern3.ToConstChar(), vPattern3.size(), true, aStrictMode );
	if( vDLParser.ExtractEW( vCompilerErrors, vCompilerWarnings ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( INF_ENGINE_SUCCESS );
	if( iee != INF_ENGINE_SUCCESS )
	{
		if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			// Ошибка в шаблоне.
			return iee;
		else
			ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

ExtICoreWR::ExtICoreWR( const ICoreRO * aMainCore,
                        SymbolymBaseRO * aSymbolymBaseRO, SynonymBaseRO * aSynonymBaseRO,
						const char * aTmpFilePath2, unsigned int aMemoryLimit ) :
	vMainCore( aMainCore ),
    BCoreWR( aMainCore, vIndexer, aSymbolymBaseRO, aSynonymBaseRO, aTmpFilePath2, aMemoryLimit ),
	vIndexer( vDataAllocator )
{}

InfEngineErrors ExtICoreWR::Create()
{
	// Создание базовых элементов.
	InfEngineErrors iee = BCoreWR::Create();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Создание индексатора.
	iee = vIndexer.Create();
	if( iee != INF_ENGINE_SUCCESS )
	{
		Destroy();

		ReturnWithTrace( iee );
	}

	// Установка состояния.
	vState = stReady;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::Save( fstorage* aFStorage )
{
	InfEngineErrors iee = BCoreWR::Save( aFStorage );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	iee = vIndexer.Save( aFStorage );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Создание сигнатуры для лингвистических данных.
	Signature * signature = nullptr;
	nMemoryAllocator aAllocator;
	if( INF_ENGINE_SUCCESS != ( iee = BuildDLDataSignature( signature, vDLData, aAllocator ) ) )
		ReturnWithTrace( iee );
	// Сохранение сигнатуры для лингвистических данных.
	if( INF_ENGINE_SUCCESS != ( iee = signature->Save( aFStorage ) ) )
		ReturnWithTrace( iee );
	aAllocator.Reset();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::SetDefaultVariableValue( Vars::Id aVarId, const char * aVarValue, unsigned int aVarValueLength )
{
	// Проверка состояния ядра.
	if( vState == stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Установка значения по умолчанию.
	if( aVarValue )
	{
		InfEngineErrors iee = vDLData.AddVariable( aVarId, aVarValue, aVarValueLength );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::RegistryNewVariable( const char * aVarName, unsigned int aVarNameLength, Vars::Id & aVarId )
{
	InfEngineErrors iee = vDLData.GetVarsRegistry().Registrate( aVarName, aVarNameLength, aVarId );
	if( iee == INF_ENGINE_WARN_UNSUCCESS || iee == INF_ENGINE_WARN_ELEMENT_EXISTS )
		return iee;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::RegistryNewFunction( const char * aFuncPath, unsigned int & aFuncId )
{
	InfEngineErrors iee = vDLData.RegistrateFunction(aFuncPath, aFuncId);
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::RegistryNewFunction( const DLFunctionInfo * aFuncInfo, unsigned int & aFuncId )
{
	InfEngineErrors iee = vDLData.RegistrateFunction(aFuncInfo, aFuncId);
	if (iee != INF_ENGINE_SUCCESS)
		ReturnWithTrace( iee);

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::CompilePatternsFromFile( const std::string & aFilePath, unsigned int & aSuccessPatterns,
													 unsigned int & aFaultPatterns, NanoLib::Encoding aenc )
{
	// Проверка состояния ядра.
	if( vState == stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	aSuccessPatterns = 0;
	aFaultPatterns = 0;
	vCompilerErrors.clear();
	vCompilerWarnings.clear();
	InfEngineErrors iee = vDLParser.ReadPatternsFromFile( aFilePath, false, true, aenc );
	if( vDLParser.ExtractEW( vCompilerErrors, vCompilerWarnings ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( INF_ENGINE_ERROR_NOFREE_MEMORY );
	aSuccessPatterns = vDLParser.GetSuccessCount();
	aFaultPatterns = vDLParser.GetFailCounter();
	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return INF_ENGINE_ERROR_CANT_PARSE;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::MakeResolving( unsigned int & aSuccessResolving, unsigned int & aFaultResolving )
{
	// Проверка состояния ядра.
	if( vState == stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	vResolvingErrors.clear();
	aSuccessResolving = 0;
	aFaultResolving = 0;
	InfEngineErrors iee = vDLParser.MakeResolving( aSuccessResolving, aFaultResolving, vResolvingErrors );
	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return INF_ENGINE_ERROR_CANT_PARSE;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );


	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::CompilePatternsFromBuffer( const char * aBuffer, unsigned int aBufferLength, bool aIgnoreErrors )
{
	// Проверка состояния ядра.
	if( vState == stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	vCompilerErrors.clear();
	vCompilerWarnings.clear();
	InfEngineErrors iee = vDLParser.ReadPatternsFromBuffer( aBuffer, aBufferLength, false, !aIgnoreErrors );
	if( vDLParser.ExtractEW( vCompilerErrors, vCompilerWarnings ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( INF_ENGINE_SUCCESS );
	return iee;
}

InfEngineErrors ExtICoreWR::CompileAliasesFromFile( const char * aFilePath, unsigned int & aSuccessAliases,
													unsigned int & aFaultAliases, AliasParser::Errors & aErrors )
{
	aSuccessAliases = 0;
	aFaultAliases = 0;
	InfEngineErrors iee = vAliasParser.ReadAliasesFromFile( aFilePath, true, aSuccessAliases, aFaultAliases, aErrors );
	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return INF_ENGINE_ERROR_CANT_PARSE;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::RegistryDictName( const char * aDictName, unsigned int aDictNameLength )
{
	InfEngineErrors iee = vDLData.RegistryDictName( aDictName, aDictNameLength );
	if( INF_ENGINE_SUCCESS != iee )
	{
		if( INF_ENGINE_WARN_UNSUCCESS == iee )
			return INF_ENGINE_WARN_UNSUCCESS;

		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::CompileDictFromFile( const char * aFilePath, const char * aDictName, unsigned int aDictNameLength,
												 aTextString & aErrorDescription, aTextString & aErrorString, NanoLib::Encoding aEncoding )
{
	aErrorDescription.clear();
	aErrorString.clear();

	InfEngineErrors iee = vDLParser.ReadDictFromFile( aFilePath, true, aDictName, aDictNameLength, aErrorDescription, aErrorString, aEncoding );
	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return INF_ENGINE_ERROR_CANT_PARSE;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::CompileDictFromBuffer( const char * aDictName, unsigned int aDictNameLength, const char * aDict, unsigned int aDictLen,
												   aTextString & aErrorDescription, aTextString & aErrorString, NanoLib::Encoding aEncoding, bool aUserDict )
{
	InfEngineErrors iee = vDLParser.ReadDictFromBuffer( aDictName, aDictNameLength, aDict, aDictLen, false, aErrorDescription, aErrorString, aEncoding, aUserDict );
	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return INF_ENGINE_ERROR_CANT_PARSE;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::CompileSynonymsFromFile( const char * aDictName, unsigned int aDictNameLen,
													 const char * aDictPath, aTextString & aErrors, NanoLib::Encoding aEncoding )
{
	// Разбираем словарь синонимов.
	vSynonymParser.Reset();
	InfEngineErrors iee = vSynonymParser.ParseFromFile( aDictPath, aEncoding );
	if (iee == INF_ENGINE_ERROR_CANT_PARSE)
	{
		aErrors.assign(vSynonymParser.GetErrors());
		return INF_ENGINE_ERROR_CANT_PARSE;
	} else if ( iee == INF_ENGINE_ERROR_FILE )
		return INF_ENGINE_ERROR_FILE;
	else if (iee != INF_ENGINE_SUCCESS)
		ReturnWithTrace( iee);

	// Индексируем синонимы.
	bool there_are_warning = false;
	for (unsigned int group_n = 0; group_n < vSynonymParser.GetGroupsCount(); ++group_n)
	{
		const char * error_msg = nullptr;
		if (INF_ENGINE_SUCCESS != (iee = vIndexer.AddSynonymGroup(vSynonymParser.GetSynonymGroup(group_n), group_n, aDictName, aDictNameLen, error_msg)))
		{
			// Обрабатываем ошибки индексации синонимов.
			if( iee == INF_ENGINE_WARN_SYNONYM_DUPLICATE && error_msg )
			{
				there_are_warning = true;
				if( aErrors.size())
					if( nlrcSuccess != aErrors.append("\n") )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				if( nlrcSuccess != aErrors.append("\t") ||
					nlrcSuccess != aErrors.append( error_msg ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			else
				ReturnWithTrace( iee);
		}
	}

	return there_are_warning ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::CompileSymbolymsFromFile( const char * aDictName, unsigned int aDictNameLen,
													  const char * aDictPath, aTextString & aErrors, NanoLib::Encoding aEncoding )
{
	// Разбираем словарь заменяемых символов.
	vSynonymParser.Reset();
	InfEngineErrors iee = vSynonymParser.ParseFromFile( aDictPath, aEncoding );
	if (iee == INF_ENGINE_ERROR_CANT_PARSE)
	{
		aErrors.assign(vSynonymParser.GetErrors());
		return INF_ENGINE_ERROR_CANT_PARSE;
	} else if ( iee == INF_ENGINE_ERROR_FILE )
		return INF_ENGINE_ERROR_FILE;
	else if (iee != INF_ENGINE_SUCCESS)
		ReturnWithTrace( iee);

	// Индексируем заменяемы символы.
	bool there_are_warning = false;
	for (unsigned int group_n = 0; group_n < vSynonymParser.GetGroupsCount(); ++group_n)
	{
		const char * error_msg = nullptr;
        if (INF_ENGINE_SUCCESS != (iee = vIndexer.AddSymbolymGroup(vSynonymParser.GetSynonymGroup(group_n), group_n, error_msg)))
		{
			// Обрабатываем ошибки индексации заменяемых символов.
			if( iee == INF_ENGINE_WARN_SYNONYM_DUPLICATE && error_msg )
			{
				there_are_warning = true;
				if( aErrors.size())
					if( nlrcSuccess != aErrors.append("\n") )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				if( nlrcSuccess != aErrors.append("\t") ||
					nlrcSuccess != aErrors.append( error_msg ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			else
				ReturnWithTrace( iee);
		}
	}

	return there_are_warning ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_SUCCESS;
}

InfEngineErrors ExtICoreWR::AddSymbolymGroup(const SynonymParser::SynonymGroup & aGroup , aTextString & aErrors)
{
    InfEngineErrors iee = INF_ENGINE_SUCCESS;
    bool there_are_warning = false;
    const char * error_msg = nullptr;

    if (INF_ENGINE_SUCCESS != (iee = vIndexer.AddSymbolymGroup(aGroup, 0, error_msg)))
    {
        // Обрабатываем ошибки индексации заменяемых символов.
        if( iee == INF_ENGINE_WARN_SYNONYM_DUPLICATE && error_msg )
        {
            there_are_warning = true;
            if( aErrors.size())
                if( nlrcSuccess != aErrors.append("\n") )
                    ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
            if( nlrcSuccess != aErrors.append("\t") ||
                nlrcSuccess != aErrors.append( error_msg ) )
                ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
        }
        else
            ReturnWithTrace( iee);
    }

    return there_are_warning ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_SUCCESS;
}
