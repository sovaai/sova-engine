#include <cstdio>
#include <getopt.h>
#include <cstring>

#include <NanoLib/LogSystem.hpp>

#include <InfEngine2/InfEngine/Signature/SignatureUtils.hpp>

#include "EllipsisCompiler.hpp"

/** Имя приложения. */
#define APPLICATION "EllipsisCompiler"

/** Версия приложения. */
#define VERSION "2.1"

/** Описание использования. */
#define USAGE_STRING "  USAGE: " APPLICATION " --help\n\n"\
	"    Give this help list\n\n\n"\
	"  USAGE: " APPLICATION " --version\n\n"\
	"    Print program version and build information\n\n\n"\
	"  USAGE: " APPLICATION " [ConfigPath] [OPTIONS]\n\n"\
	"    Compile DL data\n"\
	"      --root           =  path      Root path for compilation\n"\
	"      --encoding       =  name      Source's encoding. Possible values: [cp1251, utf8]. Default: utf8\n"\
	"\n"\
	"      --log-level      =  name      Logging level. Possible values: [none, error, warn, info, debug]. Default: debug\n"\
	"\n"\
	"      --sources-list   =  path      Path to DL patterns files list\n"\
	"      --target-path    =  path      Path to target base\n"\
	"\n"\
	"      --v/vv                        Set verbose level\n" \
	"      --signature                   Print signature for this binary\n"

using namespace NanoLib;

#define BUILDINFO APPLICATION"\n\n  Build date: " __DATE__ "\n  Build time: " __TIME__ ""

#define ASSIGN_ARG_VALUE( aArgNumber, aArgName )\
do\
{\
	if( option_index == (aArgNumber) )\
	{\
		if( aArgName )\
		{\
			printf( "\n%s\n\n", USAGE_STRING );\
			return rcErrorInvArgs;\
		}\
		else\
		{\
			(aArgName) = optarg;\
		}\
		break;\
	}\
} while(0)

/** Коды возврата компилятора. */
typedef enum
{
	/** Успешная компиляция. */
	rcSuccess = 0,

	/** Неверные аргументы командной строки. */
	rcErrorInvArgs = -1,

	/** Некорректный конфигурационный файл. */
	rcErrorInvConf = -2,

	/** Ошибка. */
	rcErrorFault = -4
} ReturnCode;

int main( int argc, char** argv )
{
	// Печать идентификатора приложения.
	printf( "\n    " APPLICATION " v." VERSION "\n\n" );

	// Печать подсказки.
	if( argc == 1 )
	{
		printf( "\n%s\n\n", USAGE_STRING );
		return rcSuccess;
	}

	// Отключаем запись ошибок в stderr для getopt_long.
	opterr = 0;

	// Показатель уровня вывода информации на экран.
	int verbose = 0;

	// Флаг логгирования в syslog.
	LogLevels SysLogLevel = LSL_NONE;

	// Описание аргументов.
	struct option long_options[] = {
		{ "v", 0, 0, 0 },
		{ "vv", 0, 0, 0 },
		{ "help", 0, 0, 0 },
		{ "root", 1, 0, 0 },
		{ "encoding", 1, 0, 0 },
		{ "sources-list", 1, 0, 0 },
		{ "target-path", 1, 0, 0 },
		{ "log-level", 1, 0, 0 },
		{ "version", 0, 0, 0 },
		{ "signature", 0, 0, 0 },
	};

	// Разбор аргументов.
	const char * aRootPath    = nullptr;
	const char * aEncoding    = nullptr;
	const char * aSourcesList = nullptr;
	const char * aTargetPath  = nullptr;
	const char * aLogLevel    = nullptr;
	while( 1 )
	{
		int option_index = 0;
		int c = getopt_long_only( argc, argv, "", long_options, &option_index );
		if( c == -1 )
			break;

		switch( c )
		{
		case 0:
			// --v
			if( option_index == 0 && verbose == 0 )
			{
				verbose = 1;
				break;
			}
			// --vv
			else if( option_index == 1 )
			{
				verbose = 2;
				break;
			}
			// --help
			else if( option_index == 2 )
			{
				printf( "\n%s\n\n", USAGE_STRING );
				return rcSuccess;
			}
			ASSIGN_ARG_VALUE( 3, aRootPath );       // --root
			ASSIGN_ARG_VALUE( 4, aEncoding );       // --encoding
			ASSIGN_ARG_VALUE( 5, aSourcesList );    // --sources-list
			ASSIGN_ARG_VALUE( 6, aTargetPath );     // --target-path
			ASSIGN_ARG_VALUE( 7, aLogLevel );       // --log-level

			if( option_index == 8 )
			{
				printf( "\n" BUILDINFO "\n\n" );
				return rcSuccess;
			}
			else if( option_index == 9 )
			{
				NanoLib::SysLogLogger Logger( "EllipsisCompiler" );
				NanoLib::LogSystem::SetLogger( Logger, LSL_DBG );

				char * signature = nullptr;
				unsigned int size = 0;
				nMemoryAllocator aAllocator;

				InfEngineErrors iee = BuildBinarySignature( signature, size, aAllocator );
				if( INF_ENGINE_SUCCESS != iee )
					ReturnWithTrace( iee );

				if( INF_ENGINE_SUCCESS != ( iee = PrintBinarySignature( signature ) ) )
					ReturnWithTrace( iee );

				aAllocator.Reset();

				return rcSuccess;
			}
			break;

		default:
			printf( "\n%s\n\n", USAGE_STRING );
			return rcErrorInvArgs;
		}
	}

	// Установка уровня логирования.
	if( aLogLevel )
	{
		if( SysLogLevel != LSL_NONE )
		{
			printf( "\n%s\n\n", USAGE_STRING );
			return rcErrorInvArgs;
		}
		else if( !strcasecmp( aLogLevel, "error" ) || !strcasecmp( aLogLevel, "errors" ) )
			SysLogLevel = LSL_ERR;
		else if( !strcasecmp( aLogLevel, "warning" ) || !strcasecmp( aLogLevel, "warnings" ) )
			SysLogLevel = LSL_WARN;
		else if( !strcasecmp( aLogLevel, "info" ) )
			SysLogLevel = LSL_INFO;
		else if( !strcasecmp( aLogLevel, "debug" ) )
			SysLogLevel = LSL_DBG;
		else
		{
			printf( "\nInvalid value for --log-level parameter\n\n" );
			return rcErrorInvArgs;
		}
	}

	const char * ConfigFilePath = nullptr;
	// Получение пути к конфигурационному файлу эллипсисов.
	if( argc - optind == 1 )
	{
		ConfigFilePath = argv[optind];
	}

	// Открытие логгирования в syslog.
	NanoLib::SysLogLogger Logger( APPLICATION );
	NanoLib::LogSystem::SetLogger( Logger, SysLogLevel );

	// Создание объета компилятора.
	EllipsisCompiler Compiler( nullptr, 0 );

	// Компиляция.
	EllipsisCompiler::ReturnCode ecrc;
	if( ( ecrc = Compiler.Compile( ConfigFilePath, aRootPath, aEncoding, aSourcesList, aTargetPath, verbose ) ) != EllipsisCompiler::rcSuccess )
	{
		if( !verbose )
			printf( " [FAILED] Some compilation errors.\n\n" );

		return rcErrorFault;
	}

	// Вывод на экране сообщения об успешном завершении компиляции.
	printf( " [SUCCESS]\n\n" );

	return rcSuccess;
}


