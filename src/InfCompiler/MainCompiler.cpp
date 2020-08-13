#include <unistd.h>
#include <getopt.h>
#include <climits>
#include <libgen.h>

#include <NanoLib/ConfigFile.hpp>
#include <NanoLib/nMemoryAllocator.hpp>

#include <InfEngine2/Build.hpp>

#include <InfEngine2/_Include/ConsoleOutput.hpp>

#include <InfEngine2/InfEngine/Synonyms/ListFileParser.hpp>

#include "InfCompiler.hpp"

#include <InfEngine2/InfEngine/InfFunctions/InternalFunctions/InternalFunction.hpp>

#include <InfEngine2/InfEngine/Signature/SignatureUtils.hpp>

#include <InfEngine2/InfEngine/Symbolyms/SymbolymTables.hpp>

#include <dirent.h>
#include <cerrno>


#define APPLICATION "InfCompiler"

#define VERSION "2.3"

#define USAGE_STRING "  USAGE: " APPLICATION " --help\n\n" \
	"    Give this help list\n\n\n" \
	"  USAGE: " APPLICATION " --version\n\n" \
	"    Print program version and build information\n\n\n" \
	"  USAGE: " APPLICATION " [ConfigFile] [OPTIONS]\n\n" \
	"    Compile DL data\n" \
	"      --root                =  path      Root path for compilation\n" \
	"      --encoding            =  name      Source's encoding. Possible values: [cp1251, utf8]. Default utf8\n" \
	"\n" \
	"      --functions-root      =  path      Functions directory\n" \
	"      --functions-config    =  path      List of functions\n" \
	"\n" \
	"      --swap-file           =  path      Swap file for compilation\n" \
	"      --swap-limit          =  number    Swap memory limit. Default: 0\n" \
	"\n" \
	"      --log-level           =  name      Logging level. Possible values: [none, error, warn, info, debug]. Default: debug\n" \
	"      --log-id              =  string    Log identificator.Default: \"InfEngine Compiler\"\n" \
	"\n" \
	"      --dldata-root         =  path      DL files root path\n" \
	"      --dlpatterns-list     =  path      Path to DL patterns files list\n" \
	"      --dldicts-list        =  path      Path to DL dicts files lists\n" \
	"      --dlvars-list         =  path      Path to DL variables list\n" \
	"      --target-path         =  path      Path to target base\n" \
	"      --dlaliases           =  path      DL aliases config file name\n" \
	"\n" \
	"      --synonyms-config     =  path      Synonyms files list\n" \
	"      --synonyms-root       =  path      Synonyms files root path\n" \
	"      --symbolyms-config    =  path      Symbolyms files list\n" \
	"      --symbolyms-root      =  path      Symbolyms files root path\n" \
	"\n" \
	"      --strict-mode         =  flag      Strict flag for compilation. Possible values: [true, false]. Default: true\n" \
	"\n" \
	"      --signature                        Print signature for this binary\n"


#define BUILDINFO \
    "      Release version: " InfEngineReleaseVersion " ( " __DATE__ " " __TIME__ " )\n" \
    "\n" \
    "      Build system: \n" \
    "       * Server: " ServerName " " BuildSystem " " ServerType "\n" \
    "       * GCC " __VERSION__ "\n"

#define ASSIGN_ARG_VALUE( aArgNumber, aArgName ) \
    do \
    { \
        if( option_index == (aArgNumber) ) \
        { \
            if( aArgName ) \
            { \
                printf( "\n" USAGE_STRING "\n\n" ); \
                return INF_ENGINE_ERROR_INV_ARGS; \
            } \
            else \
                aArgName = optarg; \
        } \
    } \
    while( 0 )

/** Memory allocator for parsing configs. */
nMemoryAllocator gAllocator;

/**
 *  Variables registration.
 * @param aInfCompiler - InfEngine compiler object.
 * @param aConfigFilePath - variables config file path.
 * @param aEncoding - source encoding.
 */
InfEngineErrors VariablesRegistration( InfCompiler &aInfCompiler, const char * aConfigFilePath, NanoLib::Encoding aEncoding ) {
	PrintHeader( "Variables registration" );
	
	// If vars configuration isn't set, than not variables to registrate.
	if( !aConfigFilePath ) {
		PrintSuccess( "There is no variables config file." );
		
		return INF_ENGINE_SUCCESS;
	}
	
	// Check variables config file accessibility.
	if( access( aConfigFilePath, R_OK ) == -1 ) {
		const char * ErrorMessage = nullptr;
		switch( errno ) {
			case ENOENT:
				ErrorMessage = "Variables config file doesn't exist.";
				PrintWarn( "%s", ErrorMessage );
				ReturnWithWarn( INF_ENGINE_WARN_UNSUCCESS, ErrorMessage );
			
			default:
				ErrorMessage = "Variables config file isn't accessible: %s";
				PrintError( ErrorMessage, strerror( errno ) );
				ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, ErrorMessage, strerror( errno ) );
		}
	}
	
	// Open variables config file.
	unsigned int SuccessCounter = 0, FailedCounter = 0;
	FILE * varsFH = fopen( aConfigFilePath, "r" );
	if( !varsFH ) {
		const char * ErrorMessage = "Can't open vars list file: %s";
		
		PrintError( ErrorMessage, strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, ErrorMessage, strerror( errno ) );
	}
	
	aTextString String;
	nlReturnCode nrc;
	while( (nrc = String.ReadString( varsFH )) == nlrcSuccess ) {
		if( NanoLib::ConvertEncoding( String, aEncoding, NanoLib::Encoding::UTF8 ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		
		// Normalize string.
		AllTrim( String );
		
		// Pass over comments and empty strings.
		if( String.empty() || String[0] == '#' )
			continue;
		
		// Get varable name.
		// @todo Need to standard for variable name.
		unsigned int VarNameEnd;
		for( VarNameEnd = 0; VarNameEnd < String.size() &&
		                     (TextFuncs::IsAlNum( String[VarNameEnd] ) || String[VarNameEnd] == '_' || String[VarNameEnd] == '-' || String[VarNameEnd] == '+' ||
		                      String[VarNameEnd] == '.'); VarNameEnd++ );
		if( VarNameEnd == 0 || (String[VarNameEnd] != ' ' && String[VarNameEnd] != '\0') ) {
			const char * WarningMessage = "Invalid variable defenition: %s";
			
			PrintWarn( WarningMessage, String.ToConstChar() );
			
			LogWarn( WarningMessage, String.ToConstChar() );
			
			FailedCounter++;
			
			continue;
		}
		
		// Convert string to lowercase.
		TextFuncs::ToLower( String.ToChar(), VarNameEnd );
		
		// Registrate variable.
		InfEngineErrors iee;
		if( VarNameEnd != String.size() )
			iee = aInfCompiler.AddVariable( String.ToConstChar(), VarNameEnd, String.ToConstChar() + VarNameEnd + 1, String.size() - VarNameEnd - 1 );
		else
			iee = aInfCompiler.AddVariable( String.ToConstChar(), VarNameEnd, nullptr, 0 );
		if( iee == INF_ENGINE_WARN_UNSUCCESS || iee == INF_ENGINE_WARN_ELEMENT_EXISTS ) {
			const char * WarningMessage = "Duplicate variable name: %s";
			
			PrintWarn( WarningMessage, String.ToConstChar() );
			
			LogWarn( WarningMessage, String.ToConstChar() );
			
			continue;
		} else if( iee != INF_ENGINE_SUCCESS ) {
			fclose( varsFH );
			
			PrintError( "Internal error." );
			
			ReturnWithTrace( iee );
		}
		
		SuccessCounter++;
	}
	
	fclose( varsFH );
	
	if( nrc == nlrcErrorNoFreeMemory ) {
		PrintError( "Internal error." );
		
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	} else if( nrc != nlrcEOF && nrc != nlrcSuccess ) {
		PrintError( "Internal error." );
		
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't read from variables config file." );
	}
	
	// Подведение итогов регистрации переменных.
	if( FailedCounter == 0 ) {
		const char * SuccessMessage = "%u variables have been successfully registred.";
		
		if( SuccessCounter )
			PrintSuccess( SuccessMessage, SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_SUCCESS, SuccessMessage, SuccessCounter );
	} else {
		const char * WarningMessage = "%.2u%% ( %u ) variables have been successfully registred.";
		
		PrintWarn( WarningMessage, (100 * SuccessCounter) / (SuccessCounter + FailedCounter), SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_WARN_UNSUCCESS, WarningMessage, ((100 * SuccessCounter) / (SuccessCounter + FailedCounter)) );
	}
}

/**
 *  Internal functions registration.
 * @param aInfCompiler - InfEngine compiler object.
 */
InfEngineErrors InternalFunctionsRegistration( InfCompiler &aInfCompiler ) {
	
	PrintHeader( "Internal functions registration" );
	unsigned int SuccessCounter = 0, FailedCounter = 0;
	
	for( unsigned int func_n = 0; func_n < InternalFunctions::FUNCTIONS_COUNT; ++func_n ) {
		const DLFunctionInfo * func = InternalFunctions::FUNCTIONS[func_n];
		
		// Functions registration.
		InfEngineErrors iee = aInfCompiler.AddFunction( func );
		if( iee == INF_ENGINE_ERROR_FUNC_VERSION ) {
			const char * ErrorMessage = "Incorrect function version: %s";
			
			PrintError( ErrorMessage, func->name );
			
			LogError( ErrorMessage, func->name );
			
			FailedCounter++;
			
			continue;
		} else if( iee == INF_ENGINE_WARN_UNSUCCESS ) {
			const char * ErrorMessage = "Can't registrate function: %s";
			
			PrintError( ErrorMessage, func->name );
			
			LogError( ErrorMessage, func->name );
			
			FailedCounter++;
			
			continue;
		} else if( iee != INF_ENGINE_SUCCESS ) {
			PrintError( "Internal error." );
			
			ReturnWithTrace( iee );
		}
		
		LogInfo( "Function is successfully loaded: %s", func->name );
		
		SuccessCounter++;
	}
	
	if( FailedCounter == 0 ) {
		const char * SuccessMessage = "All functions ( %u ) is successfully registred.";
		
		PrintSuccess( SuccessMessage, SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_SUCCESS, SuccessMessage, SuccessCounter );
	} else {
		const char * ErrorMessage = "%.2u%% ( %u ) functions have been successfully registred.";
		
		PrintError( ErrorMessage, (100 * SuccessCounter) / (SuccessCounter + FailedCounter), SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_WARN_UNSUCCESS, ErrorMessage, ((100 * SuccessCounter) / (SuccessCounter + FailedCounter)) );
	}
	
	return INF_ENGINE_SUCCESS;
}

/**
 *  External functions registration.
 * @param aInfCompiler - InfEngine compiler object.
 * @param aFuncListPath - path to configuration file.
 * @param aFuncRootPath - root path for functions' libraries paths.
 */
InfEngineErrors ExternalFunctionsRegistration( InfCompiler &aInfCompiler, const char * aConfigPath, const char * aFuncRootPath ) {
	PrintHeader( "External functions registration" );
	
	// No config file => no registrations.
	if( !aConfigPath ) {
		PrintSuccess( "There is no functions config file." );
		
		return INF_ENGINE_SUCCESS;
	}
	
	// Check access to config file.
	if( access( aConfigPath, R_OK ) == -1 ) {
		const char * ErrorMessage = nullptr;
		switch( errno ) {
			case ENOENT:
				ErrorMessage = "DL functions configuration file doesn't exist.";
				
				PrintWarn( "%s", ErrorMessage );
				
				ReturnWithWarn( INF_ENGINE_WARN_UNSUCCESS, "%s", ErrorMessage );
			
			default:
				ErrorMessage = "DL functions configuration file isn't accessible: %s";
				
				PrintError( ErrorMessage, strerror( errno ) );
				
				ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, ErrorMessage, strerror( errno ) );
		}
	}
	
	// Open functions configuration file.
	FILE * funcFH = fopen( aConfigPath, "r" );
	if( !funcFH ) {
		const char * ErrorMessage = "Can't open DL functions configuration file: %s";
		
		PrintError( ErrorMessage, strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, ErrorMessage, strerror( errno ) );
	}
	
	// Remember current directory.
	char CurrentDirBuffer[4096];
	if( !getcwd( CurrentDirBuffer, 4096 ) ) {
		const char * ErrorMessage = "Can't get current dir: %s";
		
		PrintError( ErrorMessage, strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_FAULT, ErrorMessage, strerror( errno ) );
	}
	
	// Change dir to root for functions.
	if( aFuncRootPath && chdir( aFuncRootPath ) == -1 ) {
		const char * ErrorMessage = "Can't chdir to %s: %s";
		
		PrintError( ErrorMessage, aFuncRootPath, strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, ErrorMessage, aFuncRootPath, strerror( errno ) );
	}
	
	// Read paths to functions' lib files.
	unsigned int SuccessCounter = 0, FailedCounter = 0;
	aTextString String;
	nlReturnCode nrc;
	while( (nrc = String.ReadString( funcFH )) == nlrcSuccess ) {
		AllTrim( String );
		
		// Skip comments and empty strings.
		if( String.empty() || String[0] == '#' )
			continue;
		
		// Functions registration.
		InfEngineErrors iee = aInfCompiler.AddFunction( String.ToConstChar() );
		if( iee == INF_ENGINE_ERROR_FUNC_VERSION ) {
			const char * ErrorMessage = "Incorrect function version: %s";
			
			PrintError( ErrorMessage, String.ToConstChar() );
			
			LogError( ErrorMessage, String.ToConstChar() );
			
			FailedCounter++;
			
			continue;
		} else if( iee == INF_ENGINE_WARN_UNSUCCESS ) {
			const char * ErrorMessage = "Can't registrate function: %s";
			
			PrintError( ErrorMessage, String.ToConstChar() );
			
			LogError( ErrorMessage, String.ToConstChar() );
			
			FailedCounter++;
			
			continue;
		} else if( iee != INF_ENGINE_SUCCESS ) {
			fclose( funcFH );
			
			PrintError( "Internal error." );
			
			ReturnWithTrace( iee );
		}
		
		LogInfo( "Function is successfully loaded: %s", String.ToConstChar() );
		
		SuccessCounter++;
	}
	fclose( funcFH );
	
	// Return to previous directory.
	if( aFuncRootPath && chdir( CurrentDirBuffer ) == -1 ) {
		const char * ErrorMessage = "Can't chdir to %s: %s";
		
		PrintError( ErrorMessage, CurrentDirBuffer, strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, ErrorMessage, CurrentDirBuffer, strerror( errno ) );
	}
	
	if( nrc == nlrcErrorNoFreeMemory ) {
		PrintError( "Internal error." );
		
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	} else if( nrc != nlrcEOF && nrc != nlrcSuccess ) {
		PrintError( "Internal error." );
		
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't read from fuctions config file." );
	}
	
	if( FailedCounter == 0 ) {
		const char * SuccessMessage = "All functions ( %u ) is successfully registred.";
		
		PrintSuccess( SuccessMessage, SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_SUCCESS, SuccessMessage, SuccessCounter );
	} else {
		const char * ErrorMessage = "%.2u%% ( %u ) functions have been successfully registred.";
		
		PrintError( ErrorMessage, (100 * SuccessCounter) / (SuccessCounter + FailedCounter), SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_WARN_UNSUCCESS, ErrorMessage, ((100 * SuccessCounter) / (SuccessCounter + FailedCounter)) );
	}
}

/**
 *  Functions registration.
 * @param aInfCompiler - InfEngine compiler object.
 * @param aFuncListPath - path to configuration file.
 * @param aFuncRootPath - root path for functions' libraries paths.
 */
InfEngineErrors FunctionsRegistration( InfCompiler &aInfCompiler, const char * aConfigPath, const char * aFuncRootPath ) {
	InfEngineErrors iee = ExternalFunctionsRegistration( aInfCompiler, aConfigPath, aFuncRootPath );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	
	if( INF_ENGINE_SUCCESS != (iee = InternalFunctionsRegistration( aInfCompiler )) )
		ReturnWithTrace( iee );
	
	return INF_ENGINE_SUCCESS;
}

/**
 *  Aliases registration.
 * @param aInfCompiler - InfEngine compiler object.
 * @param aConfigFile - path to configuration file.
 */
InfEngineErrors AliasRegistration( InfCompiler &aInfCompiler, const char * aConfigFile ) {
	PrintHeader( "Aliases compilation" );
	
	// No aliases config file => no need to registrate aliases.
	if( !aConfigFile ) {
		PrintSuccess( "There is no aliases config file." );
		
		return INF_ENGINE_SUCCESS;
	}
	
	// Check aliases config file accessibility.
	if( access( aConfigFile, R_OK ) == -1 ) {
		const char * ErrorMessage = nullptr;
		switch( errno ) {
			case ENOENT:
				ErrorMessage = "Aliases config file doesn't exist.";
				PrintWarn( "%s", ErrorMessage );
				ReturnWithWarn( INF_ENGINE_SUCCESS, "%s", ErrorMessage );
			
			default:
				ErrorMessage = "Aliases config file is not accessible: %s";
				PrintError( ErrorMessage, strerror( errno ) );
				ReturnWithError( INF_ENGINE_SUCCESS, ErrorMessage, strerror( errno ) );
		}
	}
	
	// Aliases registration.
	InfEngineErrors iee = aInfCompiler.CompileAliasesFromFile( aConfigFile );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	else
		return iee;
}

/**
 *  Synonyms compilation.
 * @param aInfCompiler - InfEngine compiler object.
 * @param aSynonymsConfigPath - path to configuration file.
 * @param aSynonymsRootPath - root path for synonyms dictionaries.
 * @param aEncoding - dl source encoding.
 */
InfEngineErrors
SynonymsDictsCompilation( InfCompiler &aInfCompiler, const char * aSynonymsConfigPath, const char * aSynonymsRootPath, NanoLib::Encoding aEncoding ) {
	PrintHeader( "Synonyms compilation" );
	
	const char * ErrorMessage = nullptr;
	
	// No config file => no registrations.
	if( !aSynonymsConfigPath ) {
		PrintSuccess( "There is no synonyms config file." );
		return INF_ENGINE_SUCCESS;
	}
	
	if( !aSynonymsRootPath ) {
		PrintSuccess( "There is no synonyms root path." );
		return INF_ENGINE_SUCCESS;
	}
	
	// Check access to config file.
	if( access( aSynonymsConfigPath, R_OK ) == -1 ) {
		switch( errno ) {
			case ENOENT:
				ErrorMessage = "Synonyms configuration file doesn't exist.";
				PrintWarn( "%s", ErrorMessage );
				ReturnWithWarn( INF_ENGINE_SUCCESS, "%s", ErrorMessage );
			default:
				ErrorMessage = "Synonyms configuration file isn't accessible: %s";
				PrintError( ErrorMessage, strerror( errno ) );
				ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, ErrorMessage, strerror( errno ) );
		}
	}
	
	// Create parser for files list.
	ListFileParser lf_parser;
	
	InfEngineErrors iee = lf_parser.ParseFromFile( aSynonymsConfigPath );
	if( iee != INF_ENGINE_SUCCESS ) {
		ErrorMessage = "Failed to parse synonyms configuration file.";
		PrintWarn( "%s", ErrorMessage );
		ReturnWithWarn( INF_ENGINE_WARN_UNSUCCESS, "%s", ErrorMessage );
	}
	
	if( !lf_parser.GetFilesCount() ) {
		ErrorMessage = "Synonims configuration file is empty.";
		PrintWarn( "%s", ErrorMessage );
		ReturnWithWarn( INF_ENGINE_WARN_UNSUCCESS, "%s", ErrorMessage );
	}
	
	// Get current dir name.
	char CurrentDirBuffer[4096];
	if( !getcwd( CurrentDirBuffer, 4096 ) ) {
		const char * ErrorMessage = "Can't get current dir: %s";
		
		PrintError( ErrorMessage, strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_FAULT, ErrorMessage, strerror( errno ) );
	}
	
	// Change dir to root for synonyms.
	if( aSynonymsRootPath && chdir( aSynonymsRootPath ) == -1 ) {
		const char * ErrorMessage = "Can't chdir to %s: %s";
		PrintError( ErrorMessage, aSynonymsRootPath, strerror( errno ) );
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, ErrorMessage, aSynonymsRootPath, strerror( errno ) );
	}
	
	unsigned int SuccessCounter = 0, FailedCounter = 0;
	for( unsigned int file_n = 0; file_n < lf_parser.GetFilesCount(); ++file_n ) {
		if( INF_ENGINE_SUCCESS !=
		    (iee = aInfCompiler.AddSynonymsDict( lf_parser.GetName( file_n ), strlen( lf_parser.GetName( file_n ) ), lf_parser.GetPath( file_n ),
		                                         aEncoding )) ) {
			if( iee == INF_ENGINE_WARN_UNSUCCESS ) {
				const char * ErrorMessage = "Can't compile synonym dictionary: %s";
				PrintError( ErrorMessage, lf_parser.GetName( file_n ) );
				LogError( ErrorMessage, lf_parser.GetName( file_n ) );
				FailedCounter++;
				continue;
			} else {
				PrintError( "Internal error." );
				ReturnWithTrace( iee );
			}
			// TODO: return something
			++FailedCounter;
		}
		
		++SuccessCounter;
	}
	
	// Return to previous directory.
	if( aSynonymsRootPath && chdir( CurrentDirBuffer ) == -1 ) {
		const char * ErrorMessage = "Can't chdir to %s: %s";
		
		PrintError( ErrorMessage, CurrentDirBuffer, strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, ErrorMessage, CurrentDirBuffer, strerror( errno ) );
	}
	
	if( FailedCounter == 0 ) {
		const char * SuccessMessage = "All synonym dictionaries ( %u ) is successfully registred.";
		PrintSuccess( SuccessMessage, SuccessCounter );
		ReturnWithInfo( INF_ENGINE_SUCCESS, SuccessMessage, SuccessCounter );
	} else {
		const char * ErrorMessage = "%.2u%% ( %u ) synonym dictionaries have been successfully registred.";
		PrintError( ErrorMessage, (100 * SuccessCounter) / (SuccessCounter + FailedCounter), SuccessCounter );
		ReturnWithInfo( INF_ENGINE_WARN_UNSUCCESS, ErrorMessage, ((100 * SuccessCounter) / (SuccessCounter + FailedCounter)) );
	}
	
	return INF_ENGINE_SUCCESS;
}

/**
 *  Symbolyms compilation.
 * @param aInfCompiler - InfEngine compiler object.
 * @param aSymbolymsConfigPath - path to configuration file.
 * @param aSymbolymsRootPath - root path for Symbolyms dictionaries.
 * @param aEncoding - dl source encoding.
 */
InfEngineErrors
SymbolymsDictsCompilation( InfCompiler &aInfCompiler, const char * aSymbolymsConfigPath, const char * aSymbolymsRootPath, NanoLib::Encoding aEncoding ) {
	PrintHeader( "Symbolyms compilation" );
	
	const char * ErrorMessage = nullptr;
	unsigned int SuccessCounter = 0, FailedCounter = 0;
	
	// No config file => use precombiled tables.
	bool use_precompiled_tables = false;
	if( !aSymbolymsConfigPath ) {
		PrintSuccess( "There is no symbolyms config file." );
		use_precompiled_tables = true;
	}
	
	if( !aSymbolymsRootPath ) {
		PrintSuccess( "There is no symbolyms root path." );
		use_precompiled_tables = true;
	}
	
	if( use_precompiled_tables ) {
		SymbolymTables symbolym_tables;
		for( unsigned int table_n = 0; table_n < symbolym_tables.GetTablesCount(); ++table_n ) {
			for( unsigned int group_n = 0; group_n < symbolym_tables.GetTableSize( table_n ); ++group_n ) {
				SynonymParser::SynonymGroup symbolym_group;
				InfEngineErrors iee = symbolym_tables.GetSymbolymGroup( table_n, group_n, symbolym_group );
				if( INF_ENGINE_SUCCESS != iee ) {
					PrintError( "Internal error." );
					ReturnWithTrace( iee );
				}
				
				if( INF_ENGINE_SUCCESS != (iee = aInfCompiler.AddSymbolymGroup( symbolym_group )) ) {
					PrintError( "Internal error." );
					ReturnWithTrace( iee );
				}
				++SuccessCounter;
			}
		}
	} else {
		// Check access to config file.
		if( access( aSymbolymsConfigPath, R_OK ) == -1 ) {
			switch( errno ) {
				case ENOENT:
					ErrorMessage = "Symbolyms configuration file doesn't exist.";
					PrintWarn( "%s", ErrorMessage );
					ReturnWithWarn( INF_ENGINE_SUCCESS, "%s", ErrorMessage );
				default:
					ErrorMessage = "Symbolyms configuration file isn't accessible: %s";
					PrintError( ErrorMessage, strerror( errno ) );
					ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, ErrorMessage, strerror( errno ) );
			}
		}
		
		// Create parser for files list.
		ListFileParser lf_parser;
		
		InfEngineErrors iee = lf_parser.ParseFromFile( aSymbolymsConfigPath );
		if( iee != INF_ENGINE_SUCCESS ) {
			ErrorMessage = "Failed to parse symbolyms configuration file.";
			PrintWarn( "%s", ErrorMessage );
			ReturnWithWarn( INF_ENGINE_WARN_UNSUCCESS, "%s", ErrorMessage );
		}
		
		if( !lf_parser.GetFilesCount() ) {
			ErrorMessage = "Symbolyms configuration file is empty.";
			PrintWarn( "%s", ErrorMessage );
			ReturnWithWarn( INF_ENGINE_WARN_UNSUCCESS, "%s", ErrorMessage );
		}
		
		// Get current dir name.
		char CurrentDirBuffer[4096];
		if( !getcwd( CurrentDirBuffer, 4096 ) ) {
			const char * ErrorMessage = "Can't get current dir: %s";
			
			PrintError( ErrorMessage, strerror( errno ) );
			
			ReturnWithError( INF_ENGINE_ERROR_FAULT, ErrorMessage, strerror( errno ) );
		}
		
		// Change dir to root for symbolyms.
		if( aSymbolymsRootPath && chdir( aSymbolymsRootPath ) == -1 ) {
			const char * ErrorMessage = "Can't chdir to %s: %s";
			PrintError( ErrorMessage, aSymbolymsRootPath, strerror( errno ) );
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, ErrorMessage, aSymbolymsRootPath, strerror( errno ) );
		}
		
		for( unsigned int file_n = 0; file_n < lf_parser.GetFilesCount(); ++file_n ) {
			if( INF_ENGINE_SUCCESS !=
			    (iee = aInfCompiler.AddSymbolymsDict( lf_parser.GetName( file_n ), strlen( lf_parser.GetName( file_n ) ), lf_parser.GetPath( file_n ),
			                                          aEncoding )) ) {
				if( iee == INF_ENGINE_WARN_UNSUCCESS ) {
					const char * ErrorMessage = "Can't compile symbolyms dictionary: %s";
					PrintError( ErrorMessage, lf_parser.GetName( file_n ) );
					LogError( ErrorMessage, lf_parser.GetName( file_n ) );
					FailedCounter++;
					continue;
				} else {
					PrintError( "Internal error." );
					ReturnWithTrace( iee );
				}
				// TODO: return something
				++FailedCounter;
			}
			
			++SuccessCounter;
		}
		
		// Return to previous directory.
		if( aSymbolymsRootPath && chdir( CurrentDirBuffer ) == -1 ) {
			const char * ErrorMessage = "Can't chdir to %s: %s";
			
			PrintError( ErrorMessage, CurrentDirBuffer, strerror( errno ) );
			
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, ErrorMessage, CurrentDirBuffer, strerror( errno ) );
		}
	}
	
	if( FailedCounter == 0 ) {
		const char * SuccessMessage = "All synonym dictionaries ( %u ) is successfully registred.";
		PrintSuccess( SuccessMessage, SuccessCounter );
		ReturnWithInfo( INF_ENGINE_SUCCESS, SuccessMessage, SuccessCounter );
	} else {
		const char * ErrorMessage = "%.2u%% ( %u ) synonym dictionaries have been successfully registred.";
		PrintError( ErrorMessage, (100 * SuccessCounter) / (SuccessCounter + FailedCounter), SuccessCounter );
		ReturnWithInfo( INF_ENGINE_WARN_UNSUCCESS, ErrorMessage, ((100 * SuccessCounter) / (SuccessCounter + FailedCounter)) );
	}
	
	return INF_ENGINE_SUCCESS;
}

struct ParseDictInfo {
	char * vName;
	unsigned int vNameLength;
	char * vPath;
	unsigned int vPathLength;
};

/**
 *  Dicts compilation.
 * @param aInfCompiler - InfEngine compiler object.
 * @param aConfigPath - path to configuration file.
 * @param aEncoding - source encoding.
 */
InfEngineErrors DictsCompilation( InfCompiler &aInfCompiler, const char * aConfigPath, NanoLib::Encoding aEncoding, nMemoryAllocator &aAllocator ) {
	PrintHeader( "Dicts compilation" );
	
	if( !aConfigPath ) {
		PrintSuccess( "There is no dicts config file." );
		
		return INF_ENGINE_SUCCESS;
	}
	
	// Check dicts config file accessibility.
	if( access( aConfigPath, R_OK ) == -1 ) {
		const char * ErrorMessage = nullptr;
		switch( errno ) {
			case ENOENT:
				ErrorMessage = "Dicts config file doesn't exist.";
				PrintWarn( "%s", ErrorMessage );
				ReturnWithWarn( INF_ENGINE_WARN_UNSUCCESS, "%s", ErrorMessage );
			
			default:
				ErrorMessage = "Dicts config file is not accessible: %s";
				PrintError( ErrorMessage, strerror( errno ) );
				ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, ErrorMessage, strerror( errno ) );
		}
	}
	
	// Open config file.
	FILE * dictlistFH = fopen( aConfigPath, "r" );
	if( !dictlistFH ) {
		const char * ErrorMessage = "Can't open dicts config file: %s";
		
		PrintError( ErrorMessage, strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, ErrorMessage, strerror( errno ) );
	}
	
	
	unsigned int SuccessCounter = 0, FailedCounter = 0;
	aTextString ErrorString, DescriptionError, String;
	nlReturnCode nrc;
	
	avector<ParseDictInfo> dicts_list;
	
	// Parse configuration.
	while( (nrc = String.ReadString( dictlistFH )) == nlrcSuccess ) {
		if( NanoLib::ConvertEncoding( String, aEncoding, NanoLib::Encoding::UTF8 ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		
		// Normalize string.
		AllTrim( String );
		
		// Pass over comments and empty strings.
		if( String.empty() || String[0] == '#' )
			continue;
		
		
		// Get dict name.
		unsigned int DictNameEnd = 0;
		unsigned int LastPos = 0;
		UChar32 ch;
		while( DictNameEnd < String.size() ) {
			LastPos = DictNameEnd;
			U8_NEXT_UNSAFE( String.ToConstChar(), DictNameEnd, ch );
			if( !TextFuncs::IsAlNumUTF8( ch ) && ch != '_' && ch != '-' && ch != '+' )
				break;
		}
		DictNameEnd = LastPos;
		
		if( DictNameEnd == 0 || (ch != ' ' && ch != '=') ) {
			const char * ErrorMessage = "Invalid dict defenition: %s";
			
			PrintError( ErrorMessage, String.ToConstChar() );
			
			LogError( ErrorMessage, String.ToConstChar() );
			
			FailedCounter++;
			
			continue;
		}
		
		// Get path to dict.
		unsigned int PathBeginPos = DictNameEnd;
		if( String[PathBeginPos] == ' ' )
			PathBeginPos++;
		
		if( String[PathBeginPos] != '=' ) {
			const char * ErrorMessage = "Invalid dict defenition: %s";
			
			PrintError( ErrorMessage, String.ToConstChar() );
			
			LogError( ErrorMessage, String.ToConstChar() );
			
			FailedCounter++;
			
			continue;
		}
		PathBeginPos++;
		
		if( String[PathBeginPos] == ' ' )
			PathBeginPos++;
		
		if( PathBeginPos == String.size() ) {
			const char * ErrorMessage = "Invalid dict defenition: %s";
			
			PrintError( ErrorMessage, String.ToConstChar() );
			
			LogError( ErrorMessage, String.ToConstChar() );
			
			FailedCounter++;
			
			continue;
		}
		
		String[DictNameEnd] = '\0';
		
		// Convert dict name to lowercase.
		TextFuncs::ToLower( String.ToChar(), DictNameEnd );
		
		// Registry new dict.
		InfEngineErrors iee = aInfCompiler.RegistryDictName( String.ToConstChar(), DictNameEnd );
		if( INF_ENGINE_SUCCESS == iee ) {
			ParseDictInfo * dict_info = dicts_list.grow();
			if( !dict_info )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			
			dict_info->vNameLength = DictNameEnd;
			dict_info->vPathLength = String.size() - PathBeginPos;
			
			dict_info->vName = nAllocateObjects( aAllocator, char, dict_info->vNameLength + 1 );
			if( !dict_info->vName )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			
			dict_info->vPath = nAllocateObjects( aAllocator, char, dict_info->vPathLength + 1 );
			if( !dict_info->vPath )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			
			memcpy( dict_info->vName, String.ToConstChar(), dict_info->vNameLength );
			dict_info->vName[dict_info->vNameLength] = '\0';
			
			memcpy( dict_info->vPath, String.ToConstChar() + PathBeginPos, dict_info->vPathLength );
			dict_info->vPath[dict_info->vPathLength] = '\0';
		} else {
			// If dict was already registered.
			if( INF_ENGINE_WARN_UNSUCCESS == iee ) {
				PrintError( "Duplicate dict name: %s", String.ToConstChar() );
				FailedCounter++;
			} else
				ReturnWithTrace( iee );
		}
	}
	
	fclose( dictlistFH );
	
	if( nrc == nlrcErrorNoFreeMemory ) {
		PrintError( "Internal error." );
		
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	} else if( nrc != nlrcEOF && nrc != nlrcSuccess ) {
		PrintError( "Internal error." );
		
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't read dicts config file." );
	}
	
	for( unsigned int dict_n = 0; dict_n < dicts_list.size(); ++dict_n ) {
		ParseDictInfo &dict_info = dicts_list[dict_n];
		
		// Compile dict.
		InfEngineErrors iee = aInfCompiler.CompileDictFromFile( dict_info.vPath, dict_info.vName, dict_info.vNameLength, DescriptionError, ErrorString,
		                                                        aEncoding );
		if( iee == INF_ENGINE_ERROR_CANT_OPEN ) {
			const char * ErrorMessage = "Failed to open file: %s";
			
			PrintError( ErrorMessage, dict_info.vPath );
			
			LogError( ErrorMessage, dict_info.vPath );
			
			FailedCounter++;
			
			break;
		} else if( iee == INF_ENGINE_ERROR_LIMIT ) {
			const char * ErrorMessage = "Dicts number limit is exceeded.";
			
			PrintError( "%s", ErrorMessage );
			
			LogError( "%s", ErrorMessage );
			
			FailedCounter++;
			
			break;
		} else if( iee == INF_ENGINE_ERROR_EMPTY ) {
			const char * ErrorMessage = "Dict is empty: %s";
			
			PrintError( ErrorMessage, dict_info.vName );
			
			LogError( ErrorMessage, dict_info.vName );
			
			FailedCounter++;
			
			continue;
		}
		if( INF_ENGINE_WARN_UNSUCCESS == iee ) {
			PrintError( "Dict '%s' wasn't registred", dict_info.vName );
			FailedCounter++;
		} else if( iee == INF_ENGINE_ERROR_CANT_PARSE ) {
			
			PrintError( "Can't parse dict %s: %s", DescriptionError.ToConstChar(), ErrorString.ToConstChar() );
			
			LogError( "Can't parse dict %s", DescriptionError.ToConstChar() );
			
			FailedCounter++;
			
			continue;
		} else if( iee != INF_ENGINE_SUCCESS ) {
			PrintError( "Internal error." );
			
			ReturnWithTrace( iee );
		}
		
		aTextString tmp;
		if( nlrcSuccess != (nrc = tmp.assign( dict_info.vName, dict_info.vNameLength )) ||
		    nlrcSuccess != (nrc = tmp.append( "=", 1 )) ||
		    nlrcSuccess != (nrc = tmp.append( dict_info.vPath, dict_info.vPathLength )) ) {
			PrintError( "Internal error." );
			
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
		
		LogInfo( "Dict is successfully loaded: %s", tmp.ToConstChar() );
		
		SuccessCounter++;
	}
	
	if( FailedCounter == 0 ) {
		const char * SuccessMessage = "All dicts ( %u ) is successfully loaded.";
		
		PrintSuccess( SuccessMessage, SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_SUCCESS, SuccessMessage, SuccessCounter );
	} else {
		const char * ErrorMessage = "%.2u%% ( %u ) dicts have been successfully registred.";
		
		PrintSuccess( ErrorMessage, (100 * SuccessCounter) / (SuccessCounter + FailedCounter), SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_WARN_UNSUCCESS, ErrorMessage, ((100 * SuccessCounter) / (SuccessCounter + FailedCounter)) );
	}
}

/**
 *  DL Patterns compilation.
 * @param aInfCompiler - InfEngine compiler object.
 * @param aPatternsListPath - patterns config file path.
 * @param aenc - dl source encoding.
 */
InfEngineErrors CompilePatterns( InfCompiler &aInfCompiler, const char * aPatternsListPath, NanoLib::Encoding aenc ) {
	PrintHeader( "Patterns compilation" );
	
	if( !aPatternsListPath ) {
		PrintSuccess( "There is no patterns config file." );
		
		return INF_ENGINE_SUCCESS;
	}
	
	// Parse patterns config file.
	unsigned int SuccessCounter = 0, FailedCounter = 0;
	FILE * dllistFH = fopen( aPatternsListPath, "r" );
	if( !dllistFH ) {
		const char * ErrorMessage = "Can't open patterns config file: %s";
		
		PrintError( ErrorMessage, strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, ErrorMessage, strerror( errno ) );
	}
	
	aTextString String;
	nlReturnCode nrc;
	while( (nrc = String.ReadString( dllistFH )) == nlrcSuccess ) {
		// Normalize string.
		AllTrim( String );
		
		// Pass over comments and empty strings.
		if( String.empty() || String[0] == '#' )
			continue;
		
		// Patterns compilation.
		InfEngineErrors iee = aInfCompiler.CompilePatternsFromFile( String.std_str(), aenc );
		if( iee == INF_ENGINE_ERROR_CANT_PARSE ) {
			FailedCounter++;
			
			continue;
		} else if( iee != INF_ENGINE_SUCCESS ) {
			fclose( dllistFH );
			
			ReturnWithTrace( iee );
		}
		SuccessCounter++;
	}
	
	fclose( dllistFH );
	
	if( nrc == nlrcErrorNoFreeMemory ) {
		PrintError( "Internal error." );
		
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	} else if( nrc != nlrcEOF && nrc != nlrcSuccess ) {
		PrintError( "Internal error." );
		
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Patterns config file read error." );
	}
	
	InfEngineErrors iee = aInfCompiler.MakeResolving();
	if( iee == INF_ENGINE_ERROR_CANT_PARSE ) {
		PrintError( "Internal error." );
		
		return INF_ENGINE_WARN_UNSUCCESS;
	}
	
	if( FailedCounter == 0 ) {
		const char * SuccessMessage = "All patterns ( %u ) files is successfully compiled.";
		
		PrintSuccess( SuccessMessage, SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_SUCCESS, SuccessMessage, SuccessCounter );
	} else {
		const char * ErrorMessage = "%.2u%% ( %u ) patterns files have been successfully compiled.";
		
		PrintError( ErrorMessage, (100 * SuccessCounter) / (SuccessCounter + FailedCounter), SuccessCounter );
		
		ReturnWithInfo( INF_ENGINE_WARN_UNSUCCESS, ErrorMessage, ((100 * SuccessCounter) / (SuccessCounter + FailedCounter)) );
	}
}

int main( int argc, char ** argv ) {
	// Print application header.
	printf( "\n    " APPLICATION " v." VERSION "\n\n" );
	
	// Print help.
	if( argc == 1 ) {
		printf( "\n" USAGE_STRING "\n\n" );
		return INF_ENGINE_SUCCESS;
	}
	
	// Turn off error logging for function getopt_long.
	opterr = 0;
	
	// Command line argument's descriptions.
	struct option long_options[] = {
			{ "help",             0, 0, 0 },
			{ "version",          0, 0, 0 },
			{ "root",             1, 0, 0 },
			{ "encoding",         1, 0, 0 },
			
			{ "functions-root",   1, 0, 0 },
			{ "functions-config", 1, 0, 0 },
			
			{ "swap-file",        1, 0, 0 },
			{ "swap-limit",       1, 0, 0 },
			
			{ "log-level",        1, 0, 0 },
			{ "log-id",           1, 0, 0 },
			
			{ "dldata-root",      1, 0, 0 },
			{ "dlpatterns-list",  1, 0, 0 },
			{ "dldicts-list",     1, 0, 0 },
			{ "dlvars-list",      1, 0, 0 },
			{ "target-path",      1, 0, 0 },
			{ "dlaliases",        1, 0, 0 },
			{ "synonyms-config",  1, 0, 0 },
			{ "synonyms-root",    1, 0, 0 },
			{ "symbolyms-config", 1, 0, 0 },
			{ "symbolyms-root",   1, 0, 0 },
			{ "strict-mode",      1, 0, 0 },
			
			{ "signature",        0, 0, 0 },
			
			{ nullptr,            0, 0, 0 }
	};
	
	const char * aMainRootDir = nullptr;
	const char * aEncoding = nullptr;
	
	const char * aFunctionsRootDir = nullptr;
	const char * aFunctionsConfigFile = nullptr;
	
	const char * aSwapFilePath = nullptr;
	const char * aSwapMemoryLimit = nullptr;
	
	const char * aLogLevel = nullptr;
	const char * aLogIdentificator = nullptr;
	
	const char * aDLDataDir = nullptr;
	const char * aDLPatternsList = nullptr;
	const char * aDLDictsList = nullptr;
	const char * aDLVarsList = nullptr;
	const char * aTargetPath = nullptr;
	const char * aDLAliases = nullptr;
	const char * aSynonymsRootDir = nullptr;
	const char * aSynonymsConfFile = nullptr;
	const char * aSymbolymsRootDir = nullptr;
	const char * aSymbolymsConfFile = nullptr;
	const char * aStrictMode = nullptr;
	
	// Parse command line arguments.
	while( 1 ) {
		int option_index = 0;
		int c = getopt_long_only( argc, argv, "", long_options, &option_index );
		if( c == -1 )
			break;
		
		switch( c ) {
			case 0:
				// --help
				if( option_index == 0 ) {
					printf( "\n" USAGE_STRING "\n\n" );
					return INF_ENGINE_SUCCESS;
				}
					// --version
				else if( option_index == 1 ) {
					printf( "\n" BUILDINFO "\n\n" );
					
					return INF_ENGINE_SUCCESS;
				}
				ASSIGN_ARG_VALUE( 2, aMainRootDir );            // --root
				ASSIGN_ARG_VALUE( 3, aEncoding );               // --encoding
				
				ASSIGN_ARG_VALUE( 4, aFunctionsRootDir );       // --functions-root
				ASSIGN_ARG_VALUE( 5, aFunctionsConfigFile );    // --functions-config
				
				ASSIGN_ARG_VALUE( 6, aSwapFilePath );           // --swap-file
				ASSIGN_ARG_VALUE( 7, aSwapMemoryLimit );        // --swap-limit
				
				ASSIGN_ARG_VALUE( 8, aLogLevel );              // --log-level
				ASSIGN_ARG_VALUE( 9, aLogIdentificator );      // --log-id
				
				ASSIGN_ARG_VALUE( 10, aDLDataDir );             // --dldata-root
				ASSIGN_ARG_VALUE( 11, aDLPatternsList );        // --dlpatterns-list
				ASSIGN_ARG_VALUE( 12, aDLDictsList );           // --dldicts-list
				ASSIGN_ARG_VALUE( 13, aDLVarsList );            // --dlvars-list
				ASSIGN_ARG_VALUE( 14, aTargetPath );            // --target-path
				ASSIGN_ARG_VALUE( 15, aDLAliases );             // --dlaliases
				ASSIGN_ARG_VALUE( 16, aSynonymsConfFile );      // --synonyms-config
				ASSIGN_ARG_VALUE( 17, aSynonymsRootDir );       // --synonyms-root
				ASSIGN_ARG_VALUE( 18, aSymbolymsConfFile );     // --symbolyms-config
				ASSIGN_ARG_VALUE( 19, aSymbolymsRootDir );      // --symbolyms-root
				ASSIGN_ARG_VALUE( 20, aStrictMode );            // --strict-mode
				
				// --signature
				if( option_index == 21 ) {
					NanoLib::SysLogLogger Logger( "InfCompiler" );
					NanoLib::LogSystem::SetLogger( Logger, LSL_DBG );
					
					char * signature = nullptr;
					unsigned int size = 0;
					nMemoryAllocator aAllocator;
					
					InfEngineErrors iee = BuildBinarySignature( signature, size, aAllocator );
					if( INF_ENGINE_SUCCESS != iee )
						ReturnWithTrace( iee );
					
					if( INF_ENGINE_SUCCESS != (iee = PrintBinarySignature( signature )) )
						ReturnWithTrace( iee );
					
					aAllocator.Reset();
					
					return INF_ENGINE_SUCCESS;
				}
				break;
			
			default:
				printf( "\n" USAGE_STRING "\n\n" );
				
				return INF_ENGINE_ERROR_INV_ARGS;
		}
	}
	
	// Check and prepare command line arguments.
	
	// Root path.
	char MainRootDirBuffer[4096];
	InfEngineErrors iee = PrepareDirPath( aMainRootDir, MainRootDirBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"root\": %s", strerror( errno ) );
		
		return iee;
	}
	
	// Remember current directory.
	char CurrentDirBuffer[4096];
	if( !getcwd( CurrentDirBuffer, 4096 ) ) {
		const char * ErrorMessage = "Can't get current dir: %s";
		PrintError( ErrorMessage, strerror( errno ) );
		ReturnWithError( INF_ENGINE_ERROR_FAULT, ErrorMessage, strerror( errno ) );
	}
	
	// Chdir to RootDir.
	if( aMainRootDir && chdir( aMainRootDir ) == -1 ) {
		PrintError( "Can't chdir to RootDir: %s", strerror( errno ) );
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", aMainRootDir, strerror( errno ) );
	}
	
	// Functions config file.
	char FunctionsConfigFileBuffer[4096];
	iee = PrepareFilePath( aFunctionsConfigFile, FunctionsConfigFileBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"functions-config\": %s", strerror( errno ) );
		return iee;
	}
	
	// Functions root path.
	char FunctionsRootDirBuffer[4096];
	iee = PrepareDirPath( aFunctionsRootDir, FunctionsRootDirBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"functions-root\": %s", strerror( errno ) );
		return iee;
	}
	
	// DLData root path.
	char InfCompilerDLDataDirBuffer[4096];
	iee = PrepareDirPath( aDLDataDir, InfCompilerDLDataDirBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"dldata-root\": %s", strerror( errno ) );
		return iee;
	}
	
	// Chdir to DLDataDir.
	if( aDLDataDir && chdir( aDLDataDir ) == -1 ) {
		PrintError( "Can't chdir to RootDir: %s", strerror( errno ) );
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", aDLDataDir, strerror( errno ) );
	}
	
	// DL patterns list file.
	char DLPatternsListBuffer[4096];
	iee = PrepareFilePath( aDLPatternsList, DLPatternsListBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"dlpatterns-list\": %s", strerror( errno ) );
		return iee;
	}
	
	// DL dictionaries list file.
	char DLDictsListBuffer[4096];
	iee = PrepareFilePath( aDLDictsList, DLDictsListBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"dldicts-list\": %s", strerror( errno ) );
		return iee;
	}
	
	// DL variables list file.
	char DLVarsListBuffer[4096];
	iee = PrepareFilePath( aDLVarsList, DLVarsListBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"dlvars-list\": %s", strerror( errno ) );
		return iee;
	}
	
	// DL aliases list file.
	char DLAliasesBuffer[4096];
	iee = PrepareFilePath( aDLAliases, DLAliasesBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"dlaliases\": %s", strerror( errno ) );
		return iee;
	}
	
	// Synonyms root path.
	char SynonymsRootDirBuffer[4096];
	iee = PrepareDirPath( aSynonymsRootDir, SynonymsRootDirBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"synonyms-root\": %s", strerror( errno ) );
		return iee;
	}
	
	// Synonyms config file.
	char SynonymsConfFileBuffer[4096];
	iee = PrepareFilePath( aSynonymsConfFile, SynonymsConfFileBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "'%s': Invalid argument \"synonyms-config\": %s", aSynonymsConfFile, strerror( errno ) );
		return iee;
	}
	
	// Symbolyms root path.
	char SymbolymsRootDirBuffer[4096];
	iee = PrepareDirPath( aSymbolymsRootDir, SymbolymsRootDirBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "Invalid argument \"symbolyms-root\": %s", strerror( errno ) );
		return iee;
	}
	
	// Symbolyms config file.
	char SymbolymsConfFileBuffer[4096];
	iee = PrepareFilePath( aSymbolymsConfFile, SymbolymsConfFileBuffer );
	if( iee != INF_ENGINE_SUCCESS ) {
		PrintError( "'%s': Invalid argument \"symbolyms-config\": %s", aSymbolymsConfFile, strerror( errno ) );
		return iee;
	}
	
	// Chdir to CurrentDirDir.
	if( chdir( CurrentDirBuffer ) == -1 ) {
		PrintError( "Can't chdir to CurrentDir: %s", strerror( errno ) );
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", aDLDataDir, strerror( errno ) );
	}
	
	// Get configuration arguments from config file.
	const char * cMainRootDir = nullptr;
	const char * cMainOutputLevel = nullptr;
	const char * cMainEncoding = nullptr;
	
	const char * cFunctionsRootDir = nullptr;
	const char * cFunctionsConfigFile = nullptr;
	
	const char * cLogLevel = nullptr;
	const char * cLogIdentificator = nullptr;
	
	const char * cSwapFilePath = nullptr;
	const char * cSwapMemoryLimit = nullptr;
	
	const char * cInfCompilerDLDataDir = nullptr;
	const char * cInfCompilerDLPatternsList = nullptr;
	const char * cInfCompilerDLDictsList = nullptr;
	const char * cInfCompilerDLVarsList = nullptr;
	const char * cInfCompilerDLAliases = nullptr;
	const char * cInfCompilerTargetPath = nullptr;
	const char * cInfCompilerStrictMode = nullptr;
	const char * cInfCompilerSynonymsRootDir = nullptr;
	const char * cInfCompilerSynonymsConfigFile = nullptr;
	const char * cInfCompilerSymbolymsRootDir = nullptr;
	const char * cInfCompilerSymbolymsConfigFile = nullptr;
	
	const char * ConfigFilePath = nullptr;
	char ConfigFilePathBuffer[4096];
	
	NanoLib::ConfigFile Config;
	
	if( argc - optind == 1 ) {
		ConfigFilePath = argv[optind];
		iee = PrepareFilePath( ConfigFilePath, ConfigFilePathBuffer );
		if( iee != INF_ENGINE_SUCCESS ) {
			PrintError( "Invalid Config file path: %s", strerror( errno ) );
			
			return iee;
		}
		
		// Parse config file.
		PrintHeader( "Load config file" );
		
		NanoLib::ConfigFile::ReturnCode cfrc = Config.ParseFile( ConfigFilePath );
		if( cfrc != NanoLib::ConfigFile::rcSuccess ) {
			switch( cfrc ) {
				case NanoLib::ConfigFile::rcSuccess:
					break;
				
				case NanoLib::ConfigFile::rcErrorNoFreeMemory:
					PrintError( "Internal error." );
					return INF_ENGINE_ERROR_NOFREE_MEMORY;
				
				case NanoLib::ConfigFile::rcErrorInvFileFormat:
					PrintError( "Invalid config file format." );
					return INF_ENGINE_ERROR_FILE;
				
				case NanoLib::ConfigFile::rcErrorFileAccess:
				case NanoLib::ConfigFile::rcErrorFileRead:
					PrintError( "Can't access config file." );
					return INF_ENGINE_ERROR_FILE;
				
				default:
					PrintError( "Internal error." );
					return INF_ENGINE_ERROR_FAULT;
			}
		}
		
		for( unsigned int i = 0; i < Config.GetSectionsNumber(); i++ ) {
			if( !strcasecmp( "Main", Config.GetSectionName( i ) ) ) {
				for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ ) {
					const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );
					
					if( !strcasecmp( "RootDir", Record->vName ) ) {
						if( !cMainRootDir )
							cMainRootDir = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "OutputLevel", Record->vName ) ) {
						if( !cMainOutputLevel )
							cMainOutputLevel = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "Encoding", Record->vName ) ) {
						if( !cMainEncoding )
							cMainEncoding = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else
						PrintWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
			} else if( !strcasecmp( "Functions", Config.GetSectionName( i ) ) ) {
				for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ ) {
					const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );
					
					if( !strcasecmp( "RootDir", Record->vName ) ) {
						if( !cFunctionsRootDir )
							cFunctionsRootDir = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "ConfigFile", Record->vName ) ) {
						if( !cFunctionsConfigFile )
							cFunctionsConfigFile = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else
						PrintWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
			} else if( !strcasecmp( "Log", Config.GetSectionName( i ) ) ) {
				for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ ) {
					const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );
					
					if( !strcasecmp( "Level", Record->vName ) ) {
						if( !cLogLevel )
							cLogLevel = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "Identificator", Record->vName ) ) {
						if( !cLogIdentificator )
							cLogIdentificator = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else
						PrintWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
			} else if( !strcasecmp( "Swap", Config.GetSectionName( i ) ) ) {
				for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ ) {
					const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );
					
					if( !strcasecmp( "FilePath", Record->vName ) ) {
						if( !cSwapFilePath )
							cSwapFilePath = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "MemoryLimit", Record->vName ) ) {
						if( !cSwapMemoryLimit )
							cSwapMemoryLimit = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else
						PrintWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
			} else if( !strcasecmp( "InfCompiler", Config.GetSectionName( i ) ) ) {
				for( unsigned int j = 0; j < Config.GetRecordsNumber( i ); j++ ) {
					const NanoLib::ConfigFile::Record * Record = Config.GetRecord( i, j );
					
					if( !strcasecmp( "DLDataDir", Record->vName ) ) {
						if( !cInfCompilerDLDataDir )
							cInfCompilerDLDataDir = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "DLPatternsList", Record->vName ) ) {
						if( !cInfCompilerDLPatternsList )
							cInfCompilerDLPatternsList = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "DLDictsList", Record->vName ) ) {
						if( !cInfCompilerDLDictsList )
							cInfCompilerDLDictsList = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "DLVarsList", Record->vName ) ) {
						if( !cInfCompilerDLVarsList )
							cInfCompilerDLVarsList = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "DLAliases", Record->vName ) ) {
						if( !cInfCompilerDLAliases )
							cInfCompilerDLAliases = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "TargetPath", Record->vName ) ) {
						if( !cInfCompilerTargetPath )
							cInfCompilerTargetPath = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "StrictMode", Record->vName ) ) {
						if( !cInfCompilerStrictMode )
							cInfCompilerStrictMode = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "SynonymsConfFile", Record->vName ) ) {
						if( !cInfCompilerSynonymsConfigFile )
							cInfCompilerSynonymsConfigFile = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "SynonymsRootDir", Record->vName ) ) {
						if( !cInfCompilerSynonymsRootDir )
							cInfCompilerSynonymsRootDir = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "SymbolymsConfFile", Record->vName ) ) {
						if( !cInfCompilerSymbolymsConfigFile )
							cInfCompilerSymbolymsConfigFile = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else if( !strcasecmp( "SymbolymsRootDir", Record->vName ) ) {
						if( !cInfCompilerSymbolymsRootDir )
							cInfCompilerSymbolymsRootDir = Record->vValue;
						else
							PrintWarn( "Duplicate record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
					} else
						PrintWarn( "Unexpected record in config file: [%s] %s", Config.GetSectionName( i ), Record->vName );
				}
			} else
				PrintWarn( "Unexpected section in config file: [%s]", Config.GetSectionName( i ) );
		}
	}
	
	// Переопределение настроек из файла настройками из командной строки;
	if( aMainRootDir ) cMainRootDir = aMainRootDir;
	if( aEncoding ) cMainEncoding = aEncoding;
	
	if( aFunctionsRootDir ) cFunctionsRootDir = aFunctionsRootDir;
	if( aFunctionsConfigFile ) cFunctionsConfigFile = aFunctionsConfigFile;
	
	if( aLogLevel ) cLogLevel = aLogLevel;
	if( aLogIdentificator ) cLogIdentificator = aLogIdentificator;
	
	if( aSwapFilePath ) cSwapFilePath = aSwapFilePath;
	if( aSwapMemoryLimit ) cSwapMemoryLimit = aSwapMemoryLimit;
	
	if( aDLDataDir ) cInfCompilerDLDataDir = aDLDataDir;
	if( aDLPatternsList ) cInfCompilerDLPatternsList = aDLPatternsList;
	if( aDLDictsList ) cInfCompilerDLDictsList = aDLDictsList;
	if( aDLVarsList ) cInfCompilerDLVarsList = aDLVarsList;
	if( aDLAliases ) cInfCompilerDLAliases = aDLAliases;
	if( aTargetPath ) cInfCompilerTargetPath = aTargetPath;
	if( aStrictMode ) cInfCompilerStrictMode = aStrictMode;
	if( aSynonymsRootDir ) cInfCompilerSynonymsRootDir = aSynonymsRootDir;
	if( aSynonymsConfFile ) cInfCompilerSynonymsConfigFile = aSynonymsConfFile;
	if( aSymbolymsRootDir ) cInfCompilerSymbolymsRootDir = aSymbolymsRootDir;
	if( aSymbolymsConfFile ) cInfCompilerSymbolymsConfigFile = aSymbolymsConfFile;
	
	// Logging system.
	NanoLib::LogSystem::SetLogLevel( LSL_ERR );
	
	// Logging level.
	LogLevels LogLevel = LSL_NONE;
	if( !cLogLevel || !strcasecmp( cLogLevel, "NONE" ) ) {
		LogLevel = LSL_NONE;
	} else if( !strcasecmp( cLogLevel, "ERROR" ) || !strcasecmp( cLogLevel, "ERRORS" ) ) {
		LogLevel = LSL_ERR;
	} else if( !strcasecmp( cLogLevel, "WARN" ) || !strcasecmp( cLogLevel, "WARNING" ) || !strcasecmp( cLogLevel, "WARNINGS" ) ) {
		LogLevel = LSL_WARN;
	} else if( !strcasecmp( cLogLevel, "INFO" ) ) {
		LogLevel = LSL_INFO;
	} else if( !strcasecmp( cLogLevel, "DEBUG" ) ) {
		LogLevel = LSL_DBG;
	} else {
		if( aLogLevel )
			PrintWarn( "Invalid value for --log-level parameter. Logging is turned off." );
		else
			PrintWarn( "Invalid record value in config file: [Log] Level. Logging is turned off." );
		
		LogLevel = LSL_NONE;
	}
	// Log identificator.
	if( !cLogIdentificator )
		cLogIdentificator = "InfCompiler";
	
	// Open log system.
	NanoLib::SysLogLogger Logger( cLogIdentificator );
	NanoLib::LogSystem::SetLogger( Logger, LogLevel );
	
	// Output level.
	OutputModes OutputLevel = OutputLow;
	if( !cMainOutputLevel || !strcasecmp( cMainOutputLevel, "Normal" ) )
		OutputLevel = OutputNormal;
	else if( !strcasecmp( cMainOutputLevel, "High" ) )
		OutputLevel = OutputHigh;
	else {
		OutputLevel = OutputNormal;
		
		const char * ErrorMessage = "Invalid record value in config file: [Main] OutputLevel. Set default value: \"NORMAL\".";
		
		PrintWarn( "%s", ErrorMessage );
		
		LogWarn( "%s", ErrorMessage );
	}
	
	// Sources encoding.
	NanoLib::Encoding Encoding{};
	if( cMainEncoding && !strcasecmp( cMainEncoding, "cp1251" ) )
		Encoding = NanoLib::Encoding::CP1251;
	else if( cMainEncoding && !strcasecmp( cMainEncoding, "utf8" ) )
		Encoding = NanoLib::Encoding::UTF8;
	else if( cMainEncoding ) {
		Encoding = NanoLib::Encoding::UTF8;
		
		const char * ErrorMessage = nullptr;
		if( aEncoding )
			ErrorMessage = "Invalid value for --encoding parameter. Set default value: \"utf8\".";
		else
			ErrorMessage = "Invalid record value in config file: [Main] Encoding. Set default value: \"utf8\".";
		
		PrintWarn( "%s", ErrorMessage );
		
		LogWarn( "%s", ErrorMessage );
	}
	
	// Root dir.
	if( !aMainRootDir ) {
		char MainRootDirTmpBuffer[4096];
		
		if( !cMainRootDir ) {
			if( ConfigFilePath ) {
				char * ConfigFileDir = dirname( const_cast<char *>( ConfigFilePath ) );
				if( sprintf( MainRootDirTmpBuffer, "%s/../", ConfigFileDir ) < 0 ) {
					PrintError( "Internal error." );
					
					ReturnWithError( INF_ENGINE_ERROR_FAULT, "%S: Can't copy string.", INF_ENGINE_STRING_ERROR_INTERNAL );
				}
				cMainRootDir = MainRootDirTmpBuffer;
			} else {
				PrintError( "Root dir doesn't specified." );
				
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Root dir doesn't specified", INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}
		
		iee = PrepareDirPath( cMainRootDir, MainRootDirBuffer );
		if( iee != INF_ENGINE_SUCCESS ) {
			const char * ErrorMessage = "Invalid record value in config file: [Main] RootDir: %s";
			
			PrintWarn( ErrorMessage, strerror( errno ) );
			
			ReturnWithError( iee, ErrorMessage, strerror( errno ) );
		}
	}
	
	// Chdir to RootDir.
	if( cMainRootDir && chdir( cMainRootDir ) == -1 ) {
		PrintError( "Can't chdir to RootDir: %s", strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", cMainRootDir, strerror( errno ) );
	}
	
	bool HasErrors = false;
	
	// Configuration for swap algorithm.
	unsigned int SwapMemoryLimit = 0;
	const char * SwapPatternsFilePath = nullptr;
	const char * SwapDictsFilePath = nullptr;
	char SwapDictsFilePathBuffer[4096];
	if( cSwapMemoryLimit ) {
		// Extract swap memory limit value.
		char * end = nullptr;
		SwapMemoryLimit = strtol( cSwapMemoryLimit, &end, 10 );
		if( (end && *end) || SwapMemoryLimit < 0 ) {
			SwapMemoryLimit = 0;
			
			const char * ErrorMessage = nullptr;
			if( aSwapMemoryLimit )
				ErrorMessage = "Invalid value for --swap-limit parameter. Set default value: \"0\" ( Swap is turned off ).";
			else
				ErrorMessage = "Invalid record value in config file: [Swap] MemoryLimit. Set default value: \"0\" ( Swap is turned off ).";
			
			PrintWarn( "%s", ErrorMessage );
			
			LogWarn( "%s", ErrorMessage );
		}
		
		if( !SwapMemoryLimit || !cSwapFilePath ) {
			// Turn off swap system.
			SwapMemoryLimit = 0;
			SwapPatternsFilePath = nullptr;
			SwapDictsFilePath = nullptr;
		} else {
			// Split swap path to dir and name part.
			char SwapDir[PATH_MAX];
			strcpy( SwapDir, cSwapFilePath );
			const char * pSwapDir = dirname( SwapDir );
			memcpy( SwapDir, pSwapDir, strlen( pSwapDir ) + 1 );
			
			char SwapName[PATH_MAX];
			strcpy( SwapName, cSwapFilePath );
			const char * pSwapName = basename( SwapName );
			memmove( SwapName, pSwapName, strlen( pSwapName ) + 1 );
			
			// Convert swap dir path to absolute value.
			char SwapFilePathBuffer[PATH_MAX];
			const char * tSwapFilePath = SwapDir;
			iee = PrepareDirPath( tSwapFilePath, SwapFilePathBuffer );
			if( iee != INF_ENGINE_SUCCESS ) {
				const char * ErrorMessage = nullptr;
				if( aSwapFilePath )
					ErrorMessage = "Invalid value for --swap-file parameter: %s";
				else
					ErrorMessage = "Invalid record value in config file: [Swap] FilePath: %s";
				
				PrintWarn( ErrorMessage, strerror( errno ) );
				
				ReturnWithError( iee, ErrorMessage, strerror( errno ) );
			}
			
			if( tSwapFilePath ) {
				unsigned int SwapFilePathLength = strlen( tSwapFilePath );
				unsigned int SwapNameLength = strlen( SwapName );
				
				// Delete old swap-files.
				DIR * dp;
				struct dirent * dirp;
				if( nullptr == (dp = opendir( tSwapFilePath )) ) {
					PrintWarn( "Failed to find swap directory. Old swap files aren't deleted. '%s'", strerror( errno ) );
				} else {
					// Look for files in swap directory.
					while( nullptr != (dirp = readdir( dp )) ) {
						// If it's swap file name delete it.
						if( !strncmp( dirp->d_name, SwapName, SwapNameLength ) && strlen( dirp->d_name ) == SwapNameLength + 6 )
							unlink( (std::string( tSwapFilePath ) + "/" + std::string( dirp->d_name )).c_str() );
					}
					closedir( dp );
				}
				
				// Prepare swap base swap path.
				if( PATH_MAX <= SwapFilePathLength + SwapNameLength + 8 ) {
					PrintError( "%s", INF_ENGINE_STRING_ERROR_INTERNAL );
					
					ReturnWithError( INF_ENGINE_ERROR_FAULT, "Absolute swap path is too long" );
				} else {
					SwapFilePathBuffer[SwapFilePathLength] = '/';
					strncpy( SwapFilePathBuffer + SwapFilePathLength + 1, SwapName, SwapNameLength );
					strncpy( SwapFilePathBuffer + SwapFilePathLength + SwapNameLength + 1, "XXXXXX", 6 );
					SwapFilePathBuffer[SwapFilePathLength + SwapNameLength + 7] = '\0';
				}
				
				// Create swap paths for patterns and dicts.
				memcpy( SwapDictsFilePathBuffer, tSwapFilePath, PATH_MAX );
				if( !mkstemp( SwapDictsFilePathBuffer ) ) {
					HasErrors = true;
					
					PrintError( "Can't create temporary swap file." );
					
					LogError( "Absolute swap path is too long" );
				}
				SwapDictsFilePath = SwapDictsFilePathBuffer;
			}
		}
	}
	
	
	// Compilation strict flag.
	bool StrictMode = true;
	if( cInfCompilerStrictMode ) {
		if( !strcasecmp( cInfCompilerStrictMode, "TRUE" ) || !strcasecmp( cInfCompilerStrictMode, "YES" ) )
			StrictMode = true;
		else if( !strcasecmp( cInfCompilerStrictMode, "FALSE" ) || !strcasecmp( cInfCompilerStrictMode, "NO" ) )
			StrictMode = false;
		else {
			StrictMode = true;
			
			const char * ErrorMessage = nullptr;
			if( aStrictMode )
				ErrorMessage = "Invalid value for --strict-mode parameter. Set default value: \"TRUE\".";
			else
				ErrorMessage = "Invalid record value in config file: [InfCompiler] StrictMode. Set default value: \"TRUE\".";
			
			PrintWarn( "%s", ErrorMessage );
			
			LogWarn( "%s", ErrorMessage );
		}
	}
	
	// Prepare functions config file path.
	if( !aFunctionsConfigFile && cFunctionsConfigFile ) {
		// Convert path to absolute values.
		iee = PrepareFilePath( cFunctionsConfigFile, FunctionsConfigFileBuffer );
		if( iee != INF_ENGINE_SUCCESS ) {
			const char * ErrorMessage = "Invalid record value in config file: [Functions] ConfigFile: %s";
			
			PrintWarn( ErrorMessage, strerror( errno ) );
			
			ReturnWithError( iee, ErrorMessage, strerror( errno ) );
		}
	}
	
	// Prepare functions root path.
	if( !aFunctionsRootDir && cFunctionsRootDir ) {
		// Convert path to absolute value.
		iee = PrepareDirPath( cFunctionsRootDir, FunctionsRootDirBuffer );
		if( iee != INF_ENGINE_SUCCESS ) {
			const char * ErrorMessage = "Invalid record value in config file: [Functions] RootDir: %s";
			
			PrintWarn( ErrorMessage, strerror( errno ) );
			
			ReturnWithError( iee, ErrorMessage, strerror( errno ) );
		}
	}
	
	
	// Prepare DLDataDir path.
	if( !aDLDataDir ) {
		// Set default value to path.
		if( !cInfCompilerDLDataDir )
			cInfCompilerDLDataDir = "data/dlstable";
		
		// Convert path to absolute value.
		iee = PrepareDirPath( cInfCompilerDLDataDir, InfCompilerDLDataDirBuffer );
		if( iee != INF_ENGINE_SUCCESS ) {
			const char * ErrorMessage = "Invalid record value in config file: [InfCompiler] DLDataDir: %s";
			
			PrintWarn( ErrorMessage, strerror( errno ) );
			
			ReturnWithError( iee, ErrorMessage, strerror( errno ) );
		}
	}
	
	// Chdir to dldata directory.
	if( cInfCompilerDLDataDir && chdir( cInfCompilerDLDataDir ) == -1 ) {
		PrintError( "Can't chdir to DLDataDir: %s", strerror( errno ) );
		
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't chdir to %s: %s", cInfCompilerDLDataDir, strerror( errno ) );
	}
	
	// Check target file path.
	if( !cInfCompilerTargetPath ) {
		// @todo main.ie2 by default.
		PrintError( "Target path is undefined." );
		
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Target path is undefined." );
	}
	
	// Проверка наличия необходимых параметров.
	if( !cInfCompilerDLVarsList )
		PrintError( "DL variables list is isn't specified" );
	if( !cFunctionsRootDir )
		PrintError( "DL functions root directory isn't specified" );
	if( !cFunctionsConfigFile )
		PrintError( "DL functions list file isn't specified" );
	if( !cInfCompilerDLDictsList )
		PrintError( "DL dictionaries list file isn't specified" );
	if( !cInfCompilerDLPatternsList )
		PrintError( "DL patterns list file isn't specified" );
	if( !(cInfCompilerDLVarsList && cFunctionsRootDir && cFunctionsConfigFile &&
	      cInfCompilerDLDictsList && cInfCompilerDLPatternsList) )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Not enough parameters" );
	
	
	// Create compiler.
	InfCompiler compiler( SwapDictsFilePath, SwapMemoryLimit );
	iee = compiler.Create();
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		HasErrors = true;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( INF_ENGINE_ERROR_FAULT );
	printf( "\n" );
	
	// Variables registartion.
	iee = VariablesRegistration( compiler, cInfCompilerDLVarsList, Encoding );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		HasErrors = true;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	printf( "\n" );
	
	// Functions registration.
	iee = FunctionsRegistration( compiler, cFunctionsConfigFile, cFunctionsRootDir );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		HasErrors = true;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	printf( "\n" );
	
	// Aliases compilation.
	iee = AliasRegistration( compiler, cInfCompilerDLAliases );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		HasErrors = true;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	printf( "\n" );
	
	// Symbolyms compilation.
	iee = SymbolymsDictsCompilation( compiler, cInfCompilerSymbolymsConfigFile, cInfCompilerSymbolymsRootDir, Encoding );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		HasErrors = true;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	printf( "\n" );
	// Symbolyms finalizaion.
	if( INF_ENGINE_SUCCESS != (iee = compiler.FinalizeSymbolyms()) )
		ReturnWithTrace( iee );
	
	// Synonyms compilation.
	iee = SynonymsDictsCompilation( compiler, cInfCompilerSynonymsConfigFile, cInfCompilerSynonymsRootDir, Encoding );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		HasErrors = true;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	printf( "\n" );
	// Synonyms finalizaion.
	if( INF_ENGINE_SUCCESS != (iee = compiler.FinalizeSynonyms()) )
		ReturnWithTrace( iee );
	
	// Dicts compilation.
	iee = DictsCompilation( compiler, cInfCompilerDLDictsList, Encoding, gAllocator );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		HasErrors = true;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	printf( "\n" );
	
	// Patterns compilation.
	iee = CompilePatterns( compiler, cInfCompilerDLPatternsList, Encoding );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		HasErrors = true;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	printf( "\n" );
	
	// Show index statistics.
	if( !HasErrors ) {
		unsigned int aRealSize;
		unsigned int aIndexedSize;
		unsigned int aTagFuncCount;
		unsigned int aUniqueTagFuncCount;
		unsigned int aMultipleTagFuncCount;
		unsigned int aHashTableMaxListLen;
		double aHashTableAvgListLen;
		compiler.GetTagFuncRegistryStat( aRealSize, aIndexedSize, aTagFuncCount, aUniqueTagFuncCount, aMultipleTagFuncCount, aHashTableMaxListLen,
		                                 aHashTableAvgListLen );
		
		if( OutputLevel == OutputHigh ) {
			printf( "\n  Tag-functins real size:    %9i bt    indexed size: %9i bt        rate: %f\n", aRealSize, aIndexedSize,
			        (double) aIndexedSize / aRealSize );
			printf( "  Tag-functins count:        %9i       unique:       %9i           rate: %f\n", aTagFuncCount, aUniqueTagFuncCount,
			        (double) aUniqueTagFuncCount / aTagFuncCount );
			printf( "  Unique tag-functins count: %9i       multiple:     %9i           rate: %f\n", aUniqueTagFuncCount, aMultipleTagFuncCount,
			        (double) aMultipleTagFuncCount / aUniqueTagFuncCount );
			
			printf( "\n  Hash table lists max length: %i    average lists len: %f\n\n", aHashTableMaxListLen, aHashTableAvgListLen );
		}
	}
	
	// Save result data to file.
	if( !StrictMode || !HasErrors ) {
		PrintHeader( "Saving result" );
		
		// Create temporary result file.
		char TargetPath[PATH_MAX];
		unsigned int TargetPathLength = strlen( cInfCompilerTargetPath );
		
		if( TargetPathLength + 7 >= PATH_MAX ) {
			PrintError( "Absolute value of target path is too long.\n" );
			
			ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Absolute value of target path is too long: %s", TargetPath );
		}
		memcpy( TargetPath, cInfCompilerTargetPath, TargetPathLength + 1 );
		memcpy( TargetPath + TargetPathLength, "XXXXXX", 7 );
		
		if( !mkstemp( TargetPath ) ) {
			PrintError( "Can't create temporary result file: %s\n", TargetPath );
			
			ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't create temporary result file: %s", TargetPath );
		}
		
		// Создание подписи для ядра.
		unsigned char Signature[16];
		MD5_CTX context;
		MD5Init( &context );
		MD5Update( &context, (const unsigned char *) InfEngineReleaseVersion, strlen( InfEngineReleaseVersion ) );
		MD5Final( Signature, &context );
		
		// Save result to temporary file.
		iee = compiler.Save( TargetPath, Signature );
		if( iee != INF_ENGINE_SUCCESS ) {
			PrintError( "Can't save data: %d", iee );
			
			unlink( TargetPath );
			
			ReturnWithTrace( iee );
		}
		
		// Move temporary file to target path.
		if( rename( TargetPath, cInfCompilerTargetPath ) != 0 ) {
			PrintError( "Can't move result to target path: %s", strerror( errno ) );
			
			unlink( TargetPath );
			unlink( cInfCompilerTargetPath );
			
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't move result to target path: %s", strerror( errno ) );
		}
		
		PrintSuccess( "Data is successfully saved." );
		
		LogInfo( "Data is successfully saved." );
	}
	
	if( !HasErrors ) {
		printf( "\n [SUCCESS] Compilation is successfully finished.\n\n" );
		
		ReturnWithInfo( INF_ENGINE_SUCCESS, "Compilation has been successfully finished." );
	} else {
		printf( "\n [FAILED] Compilation has been failed.\n\n" );
		
		ReturnWithError( StrictMode ? INF_ENGINE_WARN_UNSUCCESS : INF_ENGINE_SUCCESS, "Compilation has been failed." );
	}
}
