#include "SignatureUtils.hpp"

#include "SignatureFormat.hpp"
#include "Signature.hpp"
#include "BInfEngineInfo.hpp"
#include "BFunctions.hpp"
#include "Function.hpp"

#include <InfEngine2/Common.hpp>
#include <InfEngine2/Build.hpp>
#include <InfEngine2/InfEngine/InfFunctions/InternalFunctions/InternalFunction.hpp>
#include <InfEngine2/InfEngine/InfFunctions/InfInternalFunction.hpp>

#include <cstdio>


using namespace SignatureFormat;

#define TO_HEX( b ) ( (b) < 10 ? '0' + (b) : 'A' + ((b) - 10) )
#define TO_HEX_L( b ) TO_HEX( (b) & 0x0F )
#define TO_HEX_H( b ) TO_HEX_L( static_cast<unsigned char>(b) >> 4 )

#define FROM_HEX( c ) static_cast<unsigned char>( c < 'A' ? c - '0' : c - 'A' + 10 )

/**
 *  Сериализация сигнатуры. Результата - строка, содержащая только печатные символы.
 */
InfEngineErrors SignatureToString( const char * aSignature, unsigned int aSignatureSize, char *& aStr, unsigned int & aLen, nMemoryAllocator & aAllocator )
{
	aLen = 2 * aSignatureSize + 1;
	aStr = nAllocateObjects( aAllocator, char, aLen );
	if( !aStr )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	for( unsigned int byte_n = 0; byte_n < aSignatureSize; ++byte_n )
	{
		aStr[2 * byte_n] = TO_HEX_H( aSignature[byte_n] );
		aStr[2 * byte_n + 1] = TO_HEX_L( aSignature[byte_n] );
	}
	aStr[aLen] = '\0';

	return INF_ENGINE_SUCCESS;
}

/**
 *  Десериализация сигнатуры.
 */
InfEngineErrors StringToSignature( const char * aStr, unsigned int aLen, const char *& aSignature, unsigned int & aSignatureSize, nMemoryAllocator & aAllocator )
{
	aSignatureSize = aLen >> 1;
	unsigned char * buf = nAllocateObjects( aAllocator, unsigned char, aSignatureSize );
	if( !buf )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	for( unsigned int byte_n = 0; byte_n < aSignatureSize; ++byte_n )
		buf[byte_n] = (FROM_HEX( aStr[2 * byte_n] ) << 4) | FROM_HEX( aStr[2 * byte_n + 1] );
	aSignature = reinterpret_cast<char*>(buf);

	return INF_ENGINE_SUCCESS;
}


// Создание сигнатуры для исполняемых фалов InfEngine.
InfEngineErrors BuildBinarySignature( char *& aBuffer, unsigned int & aSize, nMemoryAllocator & aAllocator )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	aBuffer = nullptr;
	aSize = 0;

	Signature signature( SignatureVersion );

	// Создание блока с информацией о InfEngine.
	BInfEngineInfo b_inf_engine_inf( InfDataProtocolVersion, InfDataProtocolVersion, LongProcVersion, InfEngineReleaseVersion );
	// Добавление в сигнатуру блока с информацией о InfEngine.
	if( INF_ENGINE_SUCCESS != ( iee = signature.AddBlock( &b_inf_engine_inf ) ) )
		ReturnWithTrace( iee );

	// Создание описаний внутренних DL-функций.
	unsigned int functions_number = InternalFunctions::GetFunctionsNumber();
	const Function ** functions = nullptr;
	if( functions_number )
	{
		// Выделение памяти под список описаний функций.
		if( !(functions = nAllocateObjects( aAllocator, const Function*, functions_number ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		for( unsigned int func_n = 0; func_n < InternalFunctions::GetFunctionsNumber(); ++func_n )
		{
			// Выделение памяти под описание одной функции.
			functions[func_n] = nAllocateObject( aAllocator, Function );
			if( !functions[func_n] )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			// Создание описания функции
			new((nMemoryAllocatorElementPtr*)functions[func_n])Function( InternalFunctions::GetFunctionInfo( func_n ) );
		}
	}

	// Создание блока с информацией о внутренних DL-функциях.
	BFunctions b_functions( DLFunctionInterfaceVersion, DLFunctionInterfaceVersion, functions_number, functions );
	// Добавление в сигнатуру блока с информацией о внутренних DL-функциях.
	if( INF_ENGINE_SUCCESS != ( iee = signature.AddBlock( &b_functions ) ) )
		ReturnWithTrace( iee );

	// Выделение памяти под результат.
	aSize = signature.GetNeedMemorySize();
	aBuffer = nAllocateObjects( aAllocator, char, aSize );
	if( !aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Сохранение сигнатуры в буфер.
	unsigned int used;
	if( INF_ENGINE_SUCCESS != ( iee = signature.Save( aBuffer, aSize, used ) ) )
		ReturnWithTrace( iee );
	aSize = used;

	return INF_ENGINE_SUCCESS;
}

// Создание сигнатуры для лингвистических данных.
InfEngineErrors BuildDLDataSignature( Signature *& aSignature, const DLDataWR & aDLDataWR, nMemoryAllocator & aAllocator )
{
	aSignature = nullptr;
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Выделение памяти под сигнатуру.
	aSignature = nAllocateObject( aAllocator, Signature );
	// Создание сигнатуры.
	new((nMemoryAllocatorElementPtr*)aSignature)Signature( SignatureVersion );

	// Выделение памяти для блока с информацией о InfEngine.
	BInfEngineInfo * b_inf_engine_inf = nAllocateObject( aAllocator, BInfEngineInfo );
	// Создание блока с информацией о InfEngine.
	new((nMemoryAllocatorElementPtr*)b_inf_engine_inf)BInfEngineInfo( InfDataProtocolVersion, InfDataProtocolVersion, LongProcVersion, InfEngineReleaseVersion );
	// Добавление в сигнатуру блока с информацией о InfEngine.
	if( INF_ENGINE_SUCCESS != ( iee = aSignature->AddBlock( b_inf_engine_inf ) ) )
		ReturnWithTrace( iee );


	// Подсчёт количества DL-функций, использованных при компиляции в шаблонах.
	unsigned int functions_number = 0;
	const Function ** functions = nullptr;
	const FunctionsRegistry & functions_registry = aDLDataWR.GetFunctionsRegistry();
	for( unsigned int func_n = 0; func_n < functions_registry.GetFunctionsNumber(); ++func_n )
		if( aDLDataWR.FunctionIsUsed( func_n ) )
			++functions_number;

	// Создание описаний использованных DL-функций.
	if( functions_number )
	{
		// Выделение памяти под список описаний функций.
		if( !(functions = nAllocateObjects( aAllocator, const Function*, functions_number ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		unsigned int cur_func = 0;
		for( unsigned int func_n = 0; func_n < functions_registry.GetFunctionsNumber(); ++func_n )
			if( aDLDataWR.FunctionIsUsed( func_n ) )
			{
				// Выделение памяти под описание одной функции.
				functions[cur_func] = nAllocateObject( aAllocator, Function );
				if( !functions[cur_func] )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				// Создание описания функции.
				new((nMemoryAllocatorElementPtr*)functions[cur_func])Function( functions_registry.GetFunctionInfo( func_n ) );
				++cur_func;
			}
	}

	// Выделение памяти для блока с информацией о использованных DL-функциях.
	BFunctions * b_functions = nAllocateObject( aAllocator, BFunctions );
	// Создание блока с информацией о использованных DL-функциях.
	new((nMemoryAllocatorElementPtr*)b_functions)BFunctions( DLFunctionInterfaceVersion, DLFunctionInterfaceVersion, functions_number, functions );
	// Добавление в сигнатуру блока с информацией о использованных DL-функциях.
	if( INF_ENGINE_SUCCESS != ( iee = aSignature->AddBlock( b_functions ) ) )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}



InfEngineErrors PrintSignatureHeader( const SignatureManipulator & aSignature, FILE * out )
{
	fprintf( out, "    Signature size:  %u bytes\n", *aSignature.GetSize() );
	fprintf( out, "    Index size:      %u bytes\n", *aSignature.GetIndexSize() );
	fprintf( out, "    Blocks size:     %u bytes\n", *aSignature.GetBlocksSize() );
	fprintf( out, "    Blocks number:   %u\n", *aSignature.GetBlocksNumber() );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors PrintBSignatureInfo( const BSignatureInfoManipulator & aSignatureInfo, FILE * out )
{
	fprintf( out, "Signature Info\n" );
	//    fprintf( out, "    Block size:         %u bytes\n", *aSignatureInfo.GetSize() );
	fprintf( out, "    Signature version:  %u\n", *aSignatureInfo.GetVersion() );

	return INF_ENGINE_SUCCESS;
}


InfEngineErrors PrintBInfEngineInfo( const BInfEngineInfoManipulator & aInfEngineInfo, FILE * out )
{
	fprintf( out, "InfEngine Info\n" );
	//    fprintf( out, "    Block size:                        %u bytes\n", *aInfEngineInfo.GetSize() );
	fprintf( out, "    InfData protocol version:          %u\n", *aInfEngineInfo.GetInfDataProtoVer() );
	fprintf( out, "    Minimal InfData protocol version:  %u\n", *aInfEngineInfo.GetMinInfDataProtoVer() );
	fprintf( out, "    LingProc version:                  %u\n", *aInfEngineInfo.GetLingProcVer() );
	fprintf( out, "    Git tag:                           \"%s\"\n", (const char *)aInfEngineInfo.GetTag() );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors PrintBFunctions( const BFunctionsManipulator & aBFunctions, FILE * out )
{
	//    fprintf( out, "    Block size:           %u bytes\n", *aBFunctions.GetSize() );
	fprintf( out, "    API version:          %u\n", *aBFunctions.GetDLFuncInterfaceVer() );
	fprintf( out, "    Minimal API version:  %u\n", *aBFunctions.GetMinDLFuncInterfaceVer() );
	fprintf( out, "    Functions number:     %u\n", *aBFunctions.GetFunctionsNumber() );

	for( unsigned int func_n = 0; func_n < *aBFunctions.GetFunctionsNumber(); ++func_n )
	{
		fprintf( out, "\n    FUNCTION #%u\n", func_n + 1 );
		FunctionManipulator function( aBFunctions.GetFunctions( func_n ) );

		//        fprintf( out, "        Structure size:   %u bytes\n", *function.GetSize() );
		fprintf( out, "        Name:             \"%s\"\n", (const char *)function.GetName() );
		fprintf( out, "        Version:          %u.%u.%u\n", function.GetVersion()[0], function.GetVersion()[1], function.GetVersion()[2] );
		fprintf( out, "        Minimal version:  %u.%u.%u\n", function.GetMinVersion()[0], function.GetMinVersion()[1], function.GetMinVersion()[2] );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors PrintBInternalFunctions( const BFunctionsManipulator & aBFunctions, FILE * out )
{
	fprintf( out, "Internal Functions\n" );

	return PrintBFunctions( aBFunctions, out );
}

InfEngineErrors PrintBUsedFunctions( const BFunctionsManipulator & aBFunctions, FILE * out )
{
	fprintf( out, "Used Functions\n" );

	return PrintBFunctions( aBFunctions, out );
}

/**
 * Вывод на экран сигнатуры для исполняемых файлов.
 * @param aBuffer - буфер с сигнатурой.
 */

InfEngineErrors PrintBinarySignature( const char * aBuffer,  FILE * out )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	fprintf( stdout, "SIGNATURE\n" );

	// Вывод информации из заголовка сигнатуры.
	SignatureManipulator signature( aBuffer );
	if( INF_ENGINE_SUCCESS != ( iee = PrintSignatureHeader( signature, out ) ) )
		ReturnWithTrace( iee );

	// У сигнатуры обязательно должен быть хотя бы один блок.
	if( *signature.GetBlocksNumber() < 1 )
	{
		fprintf( stdout, "Signature format error: signature info block is not found\n" );
		ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
	}

	// Вывод блока с информацией о сигнатуре.
	printf( "\nBLOCK #1:   " );
	BSignatureInfoManipulator signature_info( signature.GetBlocks( 0 ) );
	if( INF_ENGINE_SUCCESS != ( iee = PrintBSignatureInfo( signature_info, out ) ) )
		ReturnWithTrace( iee );

	// Вывод блоков, содержащихся в сигнатуре версии 1.
	if( 1 == *signature_info.GetVersion() )
	{
		// Проверка наличия блока с информацией о InfEngine.
		if( *signature.GetBlocksNumber() < 2 )
		{
			fprintf( stdout, "Signature format error: InfEngine info block is not found\n" );
			ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		}
		// Вывод блока с информацией о InfEngine.
		printf( "\nBLOCK #2:   " );
		if( INF_ENGINE_SUCCESS != ( iee = PrintBInfEngineInfo( BInfEngineInfoManipulator( signature.GetBlocks( 1 ) ), out ) ) )
			ReturnWithTrace( iee );

		// Проверка наличия блока с информацией о внутренних функциях.
		if( *signature.GetBlocksNumber() < 3 )
		{
			fprintf( stdout, "Signature format error: Internal Functions block is not found\n" );
			ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		}
		// Вывод блока с информацией о внутренних функциях.
		printf( "\nBLOCK #3:   " );
		if( INF_ENGINE_SUCCESS != ( iee = PrintBInternalFunctions( BFunctionsManipulator( signature.GetBlocks( 2 ) ), out ) ) )
			ReturnWithTrace( iee );
	}
	else
	{
		printf( "\nSignatures of version %u are not supported!!!\n", *signature_info.GetVersion() );
		return INF_ENGINE_SUCCESS;
	}

	return INF_ENGINE_SUCCESS;
}

/**
 * Вывод на экран сигнатуры для лингвистических данных.
 * @param aBuffer - буфер с сигнатурой.
 */
InfEngineErrors PrintDLDataSignature( const char * aBuffer, FILE * out )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	fprintf( stdout, "SIGNATURE\n" );

	// Вывод информации из заголовка сигнатуры.
	SignatureManipulator signature( aBuffer );
	if( INF_ENGINE_SUCCESS != ( iee = PrintSignatureHeader( signature, out ) ) )
		ReturnWithTrace( iee );

	// У сигнатуры обязательно должен быть хотя бы один блок.
	if( *signature.GetBlocksNumber() < 1 )
	{
		fprintf( stdout, "Signature format error: signature info block is not found\n" );
		ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
	}

	// Вывод блока с информацией о сигнатуре.
	printf( "\nBLOCK #1:   " );
	BSignatureInfoManipulator signature_info( signature.GetBlocks( 0 ) );
	if( INF_ENGINE_SUCCESS != ( iee = PrintBSignatureInfo( signature_info, out ) ) )
		ReturnWithTrace( iee );

	// Вывод блоков, содержащихся в сигнатуре версии 1.
	if( 1 == *signature_info.GetVersion() )
	{
		// Проверка наличия блока с информацией о InfEngine.
		if( *signature.GetBlocksNumber() < 2 )
		{
			fprintf( stdout, "Signature format error: InfEngine info block is not found\n" );
			ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		}
		// Вывод блока с информацией о InfEngine.
		printf( "\nBLOCK #2:   " );
		if( INF_ENGINE_SUCCESS != ( iee = PrintBInfEngineInfo( BInfEngineInfoManipulator( signature.GetBlocks( 1 ) ), out ) ) )
			ReturnWithTrace( iee );

		// Проверка наличия блока с информацией о использованных функциях.
		if( *signature.GetBlocksNumber() < 3 )
		{
			fprintf( stdout, "Signature format error: Used Functions block is not found\n" );
			ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		}
		// Вывод блока с информацией о использованных функциях.
		printf( "\nBLOCK #3:   " );
		if( INF_ENGINE_SUCCESS != ( iee = PrintBUsedFunctions( BFunctionsManipulator( signature.GetBlocks( 2 ) ), out ) ) )
			ReturnWithTrace( iee );
	}
	else
	{
		printf( "\nSignatures of version %u are not supported!!!\n", *signature_info.GetVersion() );
		return INF_ENGINE_SUCCESS;
	}

	return INF_ENGINE_SUCCESS;
}




InfEngineErrors CompareBSignatureInfo( const BSignatureInfoManipulator & aBin, const BSignatureInfoManipulator & aDL, aTextString & aResultInfo )
{
	nlReturnCode rc = nlrcSuccess;

	if( *aBin.GetVersion() != *aDL.GetVersion() )
	{
		if( nlrcSuccess != ( rc = aResultInfo.Print( "Signature versions are incompatible. Engine(%u) <> DL(%u)\n", *aBin.GetVersion(), *aDL.GetVersion() ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
		return INF_ENGINE_WARN_UNSUCCESS;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors CompareBInfEngineInfo( const BInfEngineInfoManipulator & aBin, const BInfEngineInfoManipulator & aDL, aTextString & aResultInfo )
{
	nlReturnCode rc = nlrcSuccess;
	bool incompatible = false;

	if( *aBin.GetInfDataProtoVer() != *aDL.GetInfDataProtoVer() )
	{
		if( *aBin.GetInfDataProtoVer() < *aDL.GetInfDataProtoVer() )
		{
			if( nlrcSuccess != ( rc = aResultInfo.Print( "Inf Data Protocol Versions are incompatible. Engine(%u) < DL(%u)\n",
							*aBin.GetInfDataProtoVer(), *aDL.GetInfDataProtoVer() ) ) )
				ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
			incompatible = true;
		}

		if( *aBin.GetMinInfDataProtoVer() > *aDL.GetInfDataProtoVer() )
		{
			if( nlrcSuccess != ( rc = aResultInfo.Print( "Inf Data Protocol Versions are incompatible. Engine(mcv%u) > DL(v%u)\n",
							*aBin.GetMinInfDataProtoVer(), *aDL.GetInfDataProtoVer() ) ) )
				ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
			incompatible = true;
		}
	}

	if( *aBin.GetLingProcVer() != *aDL.GetLingProcVer() )
	{
		if( nlrcSuccess != ( rc = aResultInfo.Print( "LingProc Versions are incompatible. Engine(%u) <> DL(%u)\n",
						*aBin.GetLingProcVer(), *aDL.GetLingProcVer() ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
		incompatible = true;
	}

	return incompatible ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_SUCCESS;
}

InfEngineErrors CompareBFunctions( const BFunctionsManipulator & aBin, const BFunctionsManipulator & aDL, aTextString & aResultInfo )
{
	nlReturnCode rc = nlrcSuccess;
	bool incompatible = false;

	if( *aBin.GetDLFuncInterfaceVer() != *aDL.GetDLFuncInterfaceVer() )
	{
		if( *aBin.GetDLFuncInterfaceVer() < *aDL.GetDLFuncInterfaceVer() )
		{
			if( nlrcSuccess != ( rc = aResultInfo.Print( "DL Functions Interface Versions are incompatible. Engine(%u) < DL(%u)\n",
							*aBin.GetDLFuncInterfaceVer(), *aDL.GetDLFuncInterfaceVer() ) ) )
				ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
			incompatible = true;
		}

		if( *aBin.GetMinDLFuncInterfaceVer() > *aDL.GetDLFuncInterfaceVer() )
		{
			if( nlrcSuccess != ( rc = aResultInfo.Print( "DL Functions Interface Versions are incompatible. Engine(mcv%u) > DL(v%u)\n",
							*aBin.GetMinDLFuncInterfaceVer(), *aDL.GetDLFuncInterfaceVer() ) ) )
				ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
			incompatible = true;
		}
	}

	return incompatible ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_SUCCESS;
}
/**
 *  Проверка совместимости сигнатуры для исполняемых файлов и сигнатуры для лингвистических данных.
 * В случае неуспеха возвращает INF_ENGINE_WARN_UNSUCCESS и сообщение, описывающее результат сравнения сигнатур.
 */
InfEngineErrors CompareSignatures( const char * aBinarySignatureBuffer, const char * aDLSignatureBuffer, aTextString & aResultInfo )
{
	aResultInfo.clear();
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	bool incompatible = false;
	nlReturnCode rc = nlrcSuccess;

	// Инициализация манипуляторов сигнатур.
	SignatureManipulator sig_bin( aBinarySignatureBuffer );
	SignatureManipulator sig_dl( aDLSignatureBuffer );

	// У сигнатуры обязательно должен быть хотя бы один блок.
	if( *sig_bin.GetBlocksNumber() < 1 )
		ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
	if( *sig_dl.GetBlocksNumber() < 1 )
		ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Сравнение информации из заголовка сигнатуры.
	BSignatureInfoManipulator bin_signature_info( sig_bin.GetBlocks( 0 ) );
	BSignatureInfoManipulator dl_signature_info( sig_dl.GetBlocks( 0 ) );
	if( INF_ENGINE_SUCCESS != ( iee = CompareBSignatureInfo( bin_signature_info, dl_signature_info, aResultInfo ) ) )
	{
		// Сигнатуры не совместимы.
		if( INF_ENGINE_WARN_UNSUCCESS == iee )
			incompatible = true;
		else
			ReturnWithTrace( iee );
	}

	// Сравнение блоков, содержащихся в сигнатуре версии 1.
	if( 1 == *bin_signature_info.GetVersion() && 1 == *dl_signature_info.GetVersion() )
	{


		// Проверка наличия блока с информацией о InfEngine.
		if( *sig_bin.GetBlocksNumber() < 2 )
			ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		if( *sig_dl.GetBlocksNumber() < 2 )
			ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		// Сравнение блоков с информацией о InfEngine.
		if( INF_ENGINE_SUCCESS != ( iee = CompareBInfEngineInfo( BInfEngineInfoManipulator( sig_bin.GetBlocks( 1 ) ), BInfEngineInfoManipulator( sig_dl.GetBlocks( 1 ) ), aResultInfo ) ) )
		{
			if( INF_ENGINE_WARN_UNSUCCESS == iee )
				incompatible = true;
			else
				ReturnWithTrace( iee );
		}

		// Проверка наличия блока с информацией о внутренних функциях.
		if( *sig_bin.GetBlocksNumber() < 3 )
			ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		if( *sig_dl.GetBlocksNumber() < 3 )
			ReturnWithError( INF_ENGINE_ERROR_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
		// Сравнение блоков с информацией о внутренних функциях.
		if( INF_ENGINE_SUCCESS != ( iee = CompareBFunctions( BFunctionsManipulator( sig_bin.GetBlocks( 2 ) ), BFunctionsManipulator( sig_dl.GetBlocks( 2 ) ), aResultInfo ) ) )
		{
			if( INF_ENGINE_WARN_UNSUCCESS == iee )
				incompatible = true;
			else
				ReturnWithTrace( iee );
		}
	}
	else
	{
		if( nlrcSuccess != ( rc = aResultInfo.Print( "Signatures of version %u and %u can't be compared!!!\n", *bin_signature_info.GetVersion(), *dl_signature_info.GetVersion() ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
		incompatible = true;
	}

	return incompatible ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_SUCCESS;
}

/**
 *  Получение сигнатуры у InfServer.
 */
InfEngineErrors GetInfServerSignature( const char * aInfServerPath, const char *& aSignature, unsigned int & aSignatureSize, nMemoryAllocator & aAllocator )
{
	// Подготовка команды для вызова InfServer.
	aTextString cmd;
	nlReturnCode nlrc = cmd.assign( aInfServerPath );
	if( nlrcSuccess != nlrc )
		ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to compose shell command: error %u", nlrc );
	if( nlrcSuccess != ( nlrc = cmd.append( " --signature-bin" ) ) )
		ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to compose shell command: error %u", nlrc );

	// Вызов InfServer.
	FILE * pipe = popen( cmd.ToConstChar(), "r" );
	// Чтение полученной сигнатуры.
	aTextString str;
	char buffer[1024];
	while( !feof( pipe ) )
		if( fgets( buffer, 1023, pipe ) != nullptr )
			if( nlrcSuccess != ( nlrc = str.append( buffer ) ) )
				ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to read signature string: error %u", nlrc );
	pclose( pipe );

	// Десериализация сигнатуры.
	InfEngineErrors iee = StringToSignature( str.ToConstChar(), str.size(), aSignature, aSignatureSize, aAllocator );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

int func_ver_cmp( const uint32_t * v1, const uint32_t * v2 )
{
	if( v1[0] == v2[0] )
	{
		if( v1[1] == v2[1] )
		{
			if( v1[2] == v2[2] )
			{
				return 0;
			}
			else
				return v1[2] < v2[2] ? -1 : 1;
		}
		else
			return v1[1] < v2[1] ? -1 : 1;
	}
	else
		return v1[0] < v2[0] ? -1 : 1;
}

int func_ver_cmp( const DLFunctionVersion & v1, const uint32_t * v2 )
{
	if( v1.a == v2[0] )
	{
		if( v1.b == v2[1] )
		{
			if( v1.c == v2[2] )
			{
				return 0;
			}
			else
				return v1.c < v2[2] ? -1 : 1;
		}
		else
			return v1.b < v2[1] ? -1 : 1;
	}
	else
		return v1.a < v2[0] ? -1 : 1;
}

InfEngineErrors CheckFunctions( const char * aBinarySignatureBuffer, const char * aDLSignatureBuffer, const FunctionsRegistryWR * aFuncRegistry, aTextString & aResultInfo )
{
	nlReturnCode rc = nlrcSuccess;
	bool incompatible = false;
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	aResultInfo.clear();

	// Инициализация манипуляторов сигнатур.
	SignatureManipulator sig_bin( aBinarySignatureBuffer );
	SignatureManipulator sig_dl( aDLSignatureBuffer );

	// Инициализация манипулятора блока с информацией о сигнатуре.
	if( *sig_bin.GetBlocksNumber() < 1 )
	{
		if( nlrcSuccess != ( rc = aResultInfo.Print( "Invalid DL signature. Signature Info Block #0 not found.\n" ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
		return INF_ENGINE_WARN_UNSUCCESS;
	}
	BSignatureInfoManipulator bin_signature_info( sig_bin.GetBlocks( 0 ) );

	if( *sig_dl.GetBlocksNumber() < 1 )
	{
		if( nlrcSuccess != ( rc = aResultInfo.Print( "Invalid DL signature. Signature Info Block #0 not found.\n" ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
		return INF_ENGINE_WARN_UNSUCCESS;
	}
	BSignatureInfoManipulator dl_signature_info( sig_dl.GetBlocks( 0 ) );

	// Проверка версии сигнатуры.
	if( 1 != *bin_signature_info.GetVersion() )
	{
		if( nlrcSuccess != ( rc = aResultInfo.Print( "Binary Signature version %u is not supported.\n", *bin_signature_info.GetVersion() ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
		return INF_ENGINE_WARN_UNSUCCESS;
	}

	// Проверка версии сигнатуры.
	if( 1 != *dl_signature_info.GetVersion() )
	{
		if( nlrcSuccess != ( rc = aResultInfo.Print( "DL Signature version %u is not supported.\n", *dl_signature_info.GetVersion() ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
		return INF_ENGINE_WARN_UNSUCCESS;
	}

	// Инициализация манипулятора блока с информацией о внутренних DL-функциях.
	if( *sig_bin.GetBlocksNumber() < 3 )
	{
		if( nlrcSuccess != ( rc = aResultInfo.Print( "Invalid Binary signature. Used Functions Block #2 not found.\n" ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
		return INF_ENGINE_WARN_UNSUCCESS;
	}
	BFunctionsManipulator b_internal_functions( sig_bin.GetBlocks( 2 ) );

	// Инициализация манипулятора блока с информацией о использованных DL-функциях.
	if( *sig_dl.GetBlocksNumber() < 3 )
	{
		if( nlrcSuccess != ( rc = aResultInfo.Print( "Invalid DL signature. Used Functions Block #2 not found.\n" ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
		return INF_ENGINE_WARN_UNSUCCESS;
	}
	BFunctionsManipulator b_used_functions( sig_dl.GetBlocks( 2 ) );

	// Проверка совместимости DL-функций.
	for( unsigned int func_n = 0; func_n < *b_used_functions.GetFunctionsNumber(); ++func_n )
	{
		// Инициализация манипулятора для информации о DL-функции.
		FunctionManipulator func( b_used_functions.GetFunctions( func_n ) );
		const DLFunctionInfo * func_info = nullptr;
		bool found = false;

		// Поиск среди внешних DL-функций.
		unsigned int func_id = 0;
		aTextString func_name;
		if( nlrcSuccess != ( rc = func_name.assign( func.GetName(), *func.GetNameLength() ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to extract function name." );
		TextFuncs::ToLower( func_name.ToChar(), func_name.size() );
		if( INF_ENGINE_SUCCESS != ( iee = aFuncRegistry->Search( func_name.ToConstChar(), func_name.size(), func_id ) ) )
		{
			if( INF_ENGINE_WARN_UNSUCCESS != iee )
				ReturnWithTrace( iee );
		}

		// Среди внехних DL-функций нужной не обнаружено.
		if( INF_ENGINE_WARN_UNSUCCESS == iee )
		{
			// Поиск среди внутренних DL-функций.
			for( unsigned int internal_n = 0; internal_n < *b_internal_functions.GetFunctionsNumber(); ++internal_n )
			{
				FunctionManipulator internal_func( b_internal_functions.GetFunctions( internal_n ) );
				if( !strcasecmp( internal_func.GetName(), func.GetName() ) )
				{
					found = true;
					// Проверка совместимости версий функций.
					if( func_ver_cmp( internal_func.GetVersion(), func.GetVersion() ) )
					{
						if( func_ver_cmp( internal_func.GetVersion(), func.GetVersion() ) < 0 )
						{
							if( nlrcSuccess != ( rc = aResultInfo.Print( "DL-function \"%s\" versions are incompatible. Registered: %u.%u.%u  Necessary: %u.%u.%u\n",
											func.GetName(), internal_func.GetVersion()[0], internal_func.GetVersion()[1], internal_func.GetVersion()[2],
											func.GetVersion()[0], func.GetVersion()[1], func.GetVersion()[2] ) ) )
								ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
							incompatible = true;
						}

						if( func_ver_cmp( internal_func.GetMinVersion(), func.GetVersion() ) > 0 )
						{
							if( nlrcSuccess != ( rc = aResultInfo.Print( "DL-function \"%s\" versions are incompatible. Registered: mcv%u.%u.%u   Necessary: v%u.%u.%u\n",
											func.GetName(), internal_func.GetMinVersion()[0], internal_func.GetMinVersion()[1], internal_func.GetMinVersion()[2],
											func.GetVersion()[0], func.GetVersion()[1], func.GetVersion()[2] ) ) )
								ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
							incompatible = true;
						}
					}
				}
			}
		}
		else
		{
			// Получение описания функции.
			func_info = aFuncRegistry->GetFunctionInfo( func_id );
			if( func_info )
			{
				found = true;
				// Проверка совместимости версий функций.
				if( func_ver_cmp( func_info->version, func.GetVersion() ) )
				{
					if( func_ver_cmp( func_info->version, func.GetVersion() ) < 0 )
					{
						if( nlrcSuccess != ( rc = aResultInfo.Print( "DL-function \"%s\" versions are incompatible. Registered: %u.%u.%u  Necessary: %u.%u.%u\n",
										func.GetName(), func_info->version.a, func_info->version.b, func_info->version.c,
										func.GetVersion()[0], func.GetVersion()[1], func.GetVersion()[2] ) ) )
							ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
						incompatible = true;
					}

					if( func_ver_cmp( func_info->min_version, func.GetVersion() ) > 0 )
					{
						if( nlrcSuccess != ( rc = aResultInfo.Print( "DL-function \"%s\" versions are incompatible. Registered: mcv%u.%u.%u   Necessary: v%u.%u.%u\n",
										func.GetName(), func_info->min_version.a, func_info->min_version.b, func_info->min_version.c,
										func.GetVersion()[0], func.GetVersion()[1], func.GetVersion()[2] ) ) )
							ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
						incompatible = true;
					}
				}
			}
		}

		if( !found )
		{
			if( nlrcSuccess != ( rc = aResultInfo.Print( "Necessary DL-function \"%s\" was not found.\n", func.GetName() ) ) )
				ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Failed to create ResultInfo message." );
			incompatible = true;
			continue;
		}

	}


	return incompatible ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_SUCCESS;
}
