#include <InfEngine2/Build.hpp>
#include <InfEngine2/_Include/Errors.h>
#include <getopt.h>
#include <cstdio>
#include <InfEngine2/InfEngine/SearchEngine.hpp>
#include <InfEngine2/InfEngine/Signature/SignatureUtils.hpp>

#define ApplicationName "CheckSignature"

#define ApplicationVersion "1.0"

#define BuildInfo "    " ApplicationName " v." ApplicationVersion "\n"\
	"\n"\
	"      Release version: " InfEngineReleaseVersion " ( " __DATE__ " " __TIME__ " )\n"\
	"\n"\
	"      Build system: \n"\
	"       * Server: " ServerName " " BuildSystem " " ServerType "\n"\
	"       * GCC " __VERSION__ "\n"

/** Описание командной строки. */
#define UsageString "    " ApplicationName " v." InfEngineReleaseVersion "\n"\
    "\n\n"\
    "  USAGE: " ApplicationName " --help\n\n"\
    "    Give this help list\n\n\n"\
    "  USAGE: " ApplicationName " --version\n\n"\
    "    Print program version and build information\n\n\n"\
    "  USAGE: " ApplicationName " --inf-server=InfServer   --[base|ellibase]=main.ie2   [--functions=functions.conf --functions-root=path]\n\n"\
    "    Check compatibility of InfServer and linguistic data base and availability of necessary functions\n\n\n"\
    "  USAGE: " ApplicationName " [OPTIONS]\n\n"\
    "      --inf-server = path        InfServer binary file\n" \
    "      --base = path              linguistic data base\n\n" \
    "      --ellibase = path          ellipsis linguistic data base\n\n" \
    "      --functions = path         functions config\n" \
    "      --functions-root = path    functions root directory\n\n"\

void PrintVersion()
{
	printf( "\n%s\n\n", BuildInfo );
}

int main( int argc, char * argv[] )
{
	enum RETURN_CODE  {
		RC_SUCCESS = 0,
		RC_WARNING = -1,
		RC_INCOMPATIBLE = -2,
		RC_INTERNAL_PROBLEM = -3
	} return_code = RC_SUCCESS;

	// Отключение записи ошибок в stderr для getopt_long.
	//    opterr = 0;

	bool FlagVersion = false;
	
	struct option long_options[] = {
			{ "help",           0, nullptr, 0 },
			{ "version",        0, nullptr, 0 },
			{ "inf-server",     1, nullptr, 0 },
			{ "base",           1, nullptr, 0 },
			{ "functions",      1, nullptr, 0 },
			{ "functions-root", 1, nullptr, 0 },
			{ "ellibase",       1, nullptr, 0 }
	};

	char * InfServerPath = nullptr;
	char * BasePath = nullptr;
	char * FunctionsPath = nullptr;
	char * FunctionsRoot = nullptr;
	bool ellipsis = false;

	// Разбор аргументов командной строки.
	while( true ) {
		int option_index = 0;
		int c = getopt_long_only( argc, argv, "", long_options, &option_index );
		
		if( c == -1 )
			break;
		
		switch( c ) {
			case 0:
				switch( option_index ) {
					case 0:
						// --help
						printf( "\n%s\n\n", UsageString );
						return RC_SUCCESS;
					
					case 1:
						// --version
						FlagVersion = true;
						break;
						
					case 2:
						// --inf-server
						if( InfServerPath ) {
							printf( "\n%s\n\n", UsageString );
							return RC_INTERNAL_PROBLEM;
						}
						InfServerPath = optarg;
						break;
					
					case 3:
						// --base
						if( BasePath ) {
							printf( "\n%s\n\n", UsageString );
							return RC_INTERNAL_PROBLEM;
						}
						BasePath = optarg;
						ellipsis = false;
						break;
					
					case 4:
						// --functions
						if( FunctionsPath ) {
							printf( "\n%s\n\n", UsageString );
							return RC_INTERNAL_PROBLEM;
						}
						FunctionsPath = optarg;
						break;
					
					case 5:
						// --functions-root
						if( FunctionsRoot ) {
							printf( "\n%s\n\n", UsageString );
							return RC_INTERNAL_PROBLEM;
						}
						FunctionsRoot = optarg;
						break;
					
					case 6:
						// --ellibase
						if( BasePath ) {
							printf( "\n%s\n\n", UsageString );
							return RC_INTERNAL_PROBLEM;
						}
						BasePath = optarg;
						ellipsis = true;
						break;
					default:
						return RC_INTERNAL_PROBLEM;
				}
				break;
			
			default:
				return RC_INTERNAL_PROBLEM;
		}
	}

	if( FlagVersion )
	{
		PrintVersion();
		return RC_SUCCESS;
	}

	NanoLib::LogSystem::SetLogLevel( LSL_ERR );
	ISearchEngine Engine;
	nMemoryAllocator Allocator;

	if( InfServerPath && BasePath )
	{
		InfEngineErrors iee;

		// Сигнатура движка.
		const char * bin_signature = nullptr;
		unsigned int bin_signature_size = 0;

		// Сигнатура лингвистических данных.
		const char * dl_signature = nullptr;
		unsigned int dl_signature_size = 0;

		// Загрузка сигнатуры движка.
		printf("\n\tLoading InfServer signature ");
		if( INF_ENGINE_SUCCESS != GetInfServerSignature( InfServerPath, bin_signature, bin_signature_size, Allocator ) )
		{
			printf("[FAILED]\n\n");
			return RC_INTERNAL_PROBLEM;
		}
		else
			printf("[SUCCESS]\n\n");

		// Загрузка сигнатуры лингвистических данных из скомпилированной базы.
		printf("\n\tLoading linguistic data signature ");
		if( INF_ENGINE_SUCCESS != Engine.GetSignature( BasePath, dl_signature, dl_signature_size, ellipsis ) )
		{
			printf("[FAILED]\n\n");
			return RC_INTERNAL_PROBLEM;
		}
		else
			printf("[SUCCESS]\n\n");

		// Проверка совместимости сигнатур.
		aTextString check_result;
		printf("\n\tCompare protocols and APIs ");
		if( INF_ENGINE_SUCCESS != ( iee = CompareSignatures( bin_signature, dl_signature, check_result ) ) )
		{
			if( INF_ENGINE_WARN_UNSUCCESS == iee )
			{
				printf("[FAILED]\n\n%s\n\n", check_result.ToConstChar() );
				return RC_INCOMPATIBLE;
			}
			return RC_INTERNAL_PROBLEM;
		}
		else
			printf("[SUCCESS]\n\n");

		// Проверка наличия необходимых для работы базы функций.
		if( FunctionsPath )
		{

		}

		// Проверка наличия необходимых для работы базы DL-функций.
		if( FunctionsPath )
		{
			FunctionsRegistryWR func_registry;
			// Загрузка реестра функций.
			printf("\n\tLoad Functions ");
			if( INF_ENGINE_SUCCESS != func_registry.RegistryAll( FunctionsPath, FunctionsRoot ) )
			{
				printf("[FAILED]\n\n");
				return RC_INTERNAL_PROBLEM;
			}
			else
				printf("[SUCCESS]\n\n");

			printf("\n\tCheck for availability of necessary DL-functions ");
			if( INF_ENGINE_SUCCESS != ( iee = CheckFunctions( bin_signature, dl_signature, &func_registry, check_result ) ) )
			{
				if( INF_ENGINE_WARN_UNSUCCESS == iee )
				{
					return_code = RC_WARNING;
					printf("[FAILED]\n\n%s\n\n", check_result.ToConstChar() );
				}
				else
					return RC_INTERNAL_PROBLEM;
			}
			else
				printf("[SUCCESS]\n\n");
		}
	}
	else
	{
		printf( "\n%s\n\n", UsageString );
		return return_code;
	}

	return return_code;
}
