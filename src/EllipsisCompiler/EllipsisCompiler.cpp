#include "EllipsisCompiler.hpp"

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <fcntl.h>

#include <NanoLib/ConfigFile.hpp>

unsigned int GetVarName( const char * aBuffer, unsigned int aBufferLen, unsigned int aStartPos )
{
	// Проверка аргументов.
	if( !aBuffer )
		return aStartPos;

	// Поиск первого стоп символа.
	unsigned int res = aStartPos;
	for(; res < aBufferLen && (
				( 'a' <= aBuffer[res] && aBuffer[res] <= 'z' ) ||
				( 'A' <= aBuffer[res] && aBuffer[res] <= 'Z' ) ||
				( '0' <= aBuffer[res] && aBuffer[res] <= '9' ) ||
				aBuffer[res] == '_' || aBuffer[res] == '-' ||
				aBuffer[res] == '+'  ); res++ )
		;

	return res;
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

const char * EllipsisCompiler::ConvertReturnCodeToString( ReturnCode aReturnCode ) const
{
	switch( aReturnCode )
	{
	case rcSuccess:
		return "Success";
	case rcUnsuccess:
		return "Unsuccess";
	case rcErrorNoFreeMemory:
		return "Can't allocate memory";
	case rcErrorFault:
		return "Unknown fault";
	case rcErrorInvArgs:
		return "Invalid arguments";
	case rcErrorAccess:
		return "Access error";
	case rcErrorInvConfigFormat:
		return "Invalid config file format";
	case rcErrorFStorage:
		return "FStorage error";
	}

	return nullptr;
}

EllipsisCompiler::ReturnCode EllipsisCompiler::Create()
{
	// Создание ядра компиляции эллпсисов.
	InfEngineErrors iee = vCore.Create();
	if( iee != INF_ENGINE_SUCCESS )
	{
		switch( iee )
		{
		case INF_ENGINE_ERROR_NOFREE_MEMORY:
			ReturnWithTrace( rcErrorNoFreeMemory );
		default:
			ReturnWithTrace( rcErrorFault );
		}
	}

	return rcSuccess;
}

EllipsisCompiler::ReturnCode EllipsisCompiler::Compile( const char * aConfigFilePath, const char * aRootPath, const char * aEncoding,
		const char * aSourcesList, const char * aTargetPath, int aVerbose )
{
	// Установка уровня вывода на экран.
	vVerbose = aVerbose;

	// Создание и инициализация необходимых объектов.
	ReturnCode rc = Create();
	if( rc != rcSuccess )
		ReturnWithTrace( rc );

	NanoLib::ConfigFile config;

	if( vVerbose )
		printf( " * Parsing configuration file\n" );

	// Разбор конфигурационного файла.
	const char * cMainRootDir = nullptr;
	const char * cMainEncoding = nullptr;
	const char * cEllipsisSourcesList = nullptr;
	const char * cEllipsisTargetPath = nullptr;

	if( aConfigFilePath )
	{
		NanoLib::ConfigFile::ReturnCode cfrc = config.ParseFile( aConfigFilePath );
		if( cfrc != NanoLib::ConfigFile::rcSuccess )
		{
			SetErrorAdv( vVerbose, "Can't parse config file: %s", config.ReturnCodeToString( cfrc ) );

			Destroy();

			switch( cfrc )
			{
			case NanoLib::ConfigFile::rcErrorNoFreeMemory:
				return rcErrorNoFreeMemory;
			case NanoLib::ConfigFile::rcErrorInvFileFormat:
				return rcErrorInvConfigFormat;
			default:
				return rcErrorFault;
			}
		}


		for( unsigned int i = 0; i < config.GetSectionsNumber(); i++ )
		{
			if( !strcasecmp( config.GetSectionName( i ), "Main" ) )
			{
				for( unsigned int j = 0; j < config.GetRecordsNumber( i ); j++ )
				{
					const NanoLib::ConfigFile::Record* record = config.GetRecord( i, j );
					if( !strcasecmp( record->vName, "RootDir" ) )
					{
						if( !cMainRootDir )
							cMainRootDir = record->vValue;
						else
							printf( "  [WARN]    Duplicate record in config file: [%s] %s\n", config.GetSectionName( i ), record->vName );
					}
					else if( !strcasecmp( "Encoding", record->vName ) )
					{
						if( !cMainEncoding )
							cMainEncoding = record->vValue;
						else
							printf( "  [WARN]    Duplicate record in config file: [%s] %s\n", config.GetSectionName( i ), record->vName );
					}
					else
						printf( "  [WARN]    Unexpected record in config file: [%s] %s\n", config.GetSectionName( i ), record->vName );
				}
			}
			else if( !strcasecmp( config.GetSectionName( i ), "Ellipsis" ) )
			{
				// Данные эллипсисов.
				for( unsigned int j = 0; j < config.GetRecordsNumber( i ); j++ )
				{
					const NanoLib::ConfigFile::Record* record = config.GetRecord( i, j );
					if( !strcasecmp( record->vName, "SourcesList" ) )
					{
						if( !cEllipsisSourcesList )
							cEllipsisSourcesList = record->vValue;
						else
							printf( "  [WARN]    Duplicate record in config file: [%s] %s\n", config.GetSectionName( i ), record->vName );
					}
					else if( !strcasecmp( record->vName, "TargetPath" ) )
					{
						if( !cEllipsisTargetPath )
							cEllipsisTargetPath = record->vValue;
						else
							printf( "  [WARN]    Duplicate record in config file: [%s] %s\n", config.GetSectionName( i ), record->vName );
					}
					else
						printf( "  [WARN]    Unexpected record in config file: [%s] %s\n", config.GetSectionName( i ), record->vName );
				}
			}
			else
			{
				// Игнорируем все лишние секции конфигурационного файла.
				printf( "  [WARN]    Unexpected section in config file: [%s]\n", config.GetSectionName( i ) );
			}
		}
	}

	// Переопределение настроек из конфигурационного файла настройкаи из командной строки.
	if( aRootPath ) cMainRootDir = aRootPath;
	if( aEncoding ) cMainEncoding = aEncoding;
	if( aSourcesList ) cEllipsisSourcesList = aSourcesList;
	if( aTargetPath ) cEllipsisTargetPath = aTargetPath;

	// Кодировка исходников.
	if( !cMainEncoding || !strcasecmp( cMainEncoding, "cp1251" ) )
		vEncoding = NanoLib::Encoding::CP1251;
	else if( !strcasecmp( cMainEncoding, "utf8" ) )
		vEncoding = NanoLib::Encoding::UTF8;
	else
	{
		printf( "    [WARN]    Invalid value for encoding parameter. Default value: utf8\n" );

		vEncoding = NanoLib::Encoding::UTF8;
	}

	// Установка значений по умолчанию.
	if( !cEllipsisTargetPath )
		cEllipsisTargetPath = "ellipsis.ie2";

	// Смена текущего каталога на RootPath.
	char tmp[FILENAME_MAX];
	if( cMainRootDir )
	{
		if( chdir( cMainRootDir ) == -1 )
		{
			SetErrorAdv( vVerbose, "Can't change dir to %s: %s", vRootPath, strerror( errno ) );

			Destroy();

			return rcErrorAccess;
		}
		else if( vVerbose >= 2 )
			printf( " * Change dir to: %s\n", cMainRootDir );

		vRootPath = cMainRootDir;
	}
	else
	{
		vRootPath = getcwd( tmp, FILENAME_MAX );
	}

	// Synonyms finalizaion.
	InfEngineErrors iee = vCore.FinalizeSynonyms();
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( rcErrorFault );

	if( !cEllipsisSourcesList )
	{
		printf( "    [ERROR]   List of dl patterns files isn't specified\n" );
		ReturnWithError( rcErrorFault, "List of dl patterns files isn't specified" );
	}

	// Компиляция эллипсисов.
	if( cEllipsisSourcesList && ( rc = ProcessEllipsisList( cEllipsisSourcesList ) ) != rcSuccess )
	{
		Destroy();

		if( rcUnsuccess )
			return rc;
		else
			ReturnWithTrace( rc );
	}

	// Сохранение базы.
	if( vVerbose > 1 )
		printf( " * Saving result\n" );
	if( ( rc = Save( cEllipsisTargetPath ) ) != rcSuccess )
	{
		Destroy();

		ReturnWithTrace( rc );
	}

	// Уничтожение и деинициализация использованных при компиляции объектов.
	Destroy();

	return rcSuccess;
}

EllipsisCompiler::ReturnCode EllipsisCompiler::ProcessVarsList( const char * aVarsListPath )
{
	// Пока не поддержана.
	if( vVerbose )
		printf( "\n   * Variables registaration.\n"
				"      * Unsupported function!\n\n" );

	return rcUnsuccess;

	// Вывод на экран информации о начале процесса регистрации переменных.
	if( vVerbose )
		printf( "\n   * Variables registaration.\n" );

	unsigned int VarsSuccess = 0, VarsErrors = 0;
	FILE* varsFH = fopen( aVarsListPath, "r" );
	if( !varsFH )
	{
		SetErrorAdv( vVerbose, "Can't open file: %s", aVarsListPath );

		return rcErrorAccess;
	}

	aTextString string, original;
	while( string.ReadString( varsFH ) == nlrcSuccess )
	{
		nlReturnCode nle = original.assign( string );
		if( nle != nlrcSuccess )
		{
			fclose( varsFH );

			SetErrorAdv( vVerbose, "%s.", ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

			return rcErrorNoFreeMemory;
		}
		AllTrim( string );

		// Пропускаем комментарии.
		if( string.empty() || string[0] == '#'  )
			continue;

		if( SetLower( string ) != nlrcSuccess )
		{
			fclose( varsFH );

			SetErrorAdv( vVerbose, "%s.", ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

			return rcErrorNoFreeMemory;
		}

		size_t end = GetVarName( string.ToConstChar(), string.size(), 0 );
		if( end == 0 )
		{
			if( vVerbose )
				printf( "     * Can't parse vars: %s", original.ToConstChar() );

			VarsErrors++;

			continue;
		}
		size_t value = end;
		if( string[end] == ' ' )
		{
			value++;
			string[end] = '\0';
		}

		VarsSuccess++;
	}
	fclose( varsFH );

	if( VarsErrors == 0 )
	{
		if( vVerbose )
		{
			printf( "     * Result:\n" );
			printf( "         Successfull registrations: %u\n", VarsSuccess );
		}
	}

	return rcSuccess;
}

EllipsisCompiler::ReturnCode EllipsisCompiler::ProcessDictsList( const char * aDictsListPath )
{
	// Вывод на экран информации о начале процесса компиляции словарей.
	if( vVerbose )
		printf( "\n   * Dictionaries compilation.\n"
				"       * Unsupported function!\n\n" );

	return rcUnsuccess;
}

EllipsisCompiler::ReturnCode EllipsisCompiler::ProcessEllipsisList( const char * aEllipsisListPath )
{
	// Разбор конфигурационного файла.
	NanoLib::ConfigFile config;

	if( vVerbose )
		printf( " * Parsing ellipsis config\n" );

	NanoLib::ConfigFile::ReturnCode cfrc = config.ParseFile( aEllipsisListPath );
	if( cfrc != NanoLib::ConfigFile::rcSuccess )
	{
		SetErrorAdv( vVerbose, "Can't parse config file: %s", config.ReturnCodeToString( cfrc ) );

		Destroy();

		switch( cfrc )
		{
		case NanoLib::ConfigFile::rcErrorNoFreeMemory:
			return rcErrorNoFreeMemory;
		case NanoLib::ConfigFile::rcErrorInvFileFormat:
			return rcErrorInvConfigFormat;
		default:
			return rcErrorFault;

		}
	}

	const char * RootPath = nullptr;
	avector<unsigned int> SourcesSections;
	for( unsigned int i = 0; i < config.GetSectionsNumber(); i++ )
	{
		if( !strcasecmp( config.GetSectionName( i ), "Main" ) )
		{
			for( unsigned int j = 0; j < config.GetRecordsNumber( i ); j++ )
			{
				const NanoLib::ConfigFile::Record* record = config.GetRecord( i, j );
				if( !strcasecmp( record->vName, "RootPath" ) )
				{
					if( RootPath != nullptr )
					{
						SetErrorAdv( vVerbose, "Invalid config file %s: there are two 'RootPath' records.", aEllipsisListPath );

						Destroy();

						return rcErrorInvConfigFormat;
					}

					RootPath = record->vValue;
				}
				else
				{
					SetErrorAdv( vVerbose, "Invalid config file %s: there is restricted record: %s", aEllipsisListPath, record->vName );

					Destroy();

					return rcErrorInvConfigFormat;
				}
			}
		}
		else if( !strcasecmp( config.GetSectionName( i ), "Files" ) )
		{
			SourcesSections.push_back( i );
			if( SourcesSections.no_memory() )
			{
				SetErrorAdv( vVerbose, "%s.", ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

				return rcErrorNoFreeMemory;
			}
		}
		else
		{
			SetErrorAdv( vVerbose, "Invalid config file %s: there is restricted section: %s", aEllipsisListPath, config.GetSectionName( i ) );

			Destroy();

			return rcErrorInvConfigFormat;
		}
	}

	// Смена текущего каталога на RootPath.
	if( RootPath )
	{
		if( chdir( RootPath ) == -1 )
		{
			SetErrorAdv( vVerbose, "Can't change dir to %s: %s", RootPath, strerror( errno ) );

			Destroy();

			return rcErrorAccess;
		}
		else if( vVerbose >= 2 )
			printf( "   * Change dir to: %s\n", RootPath );
	}

	// Разбор файлов с шаблонами эллипсисов.
	ReturnCode rc, result = rcSuccess;
	if( vVerbose )
		printf( " * Patterns compilation\n" );
	for( unsigned int i = 0; i < SourcesSections.size(); i++ )
	{
		for( unsigned int j = 0; j < config.GetRecordsNumber( SourcesSections[i] ); j++ )
		{
			const NanoLib::ConfigFile::Record* record = config.GetRecord( SourcesSections[i], j );

			if( ( rc = CompilePatternsFile( record->vValue ) ) != rcSuccess )
			{
				if( rc == rcUnsuccess )
					result = rcUnsuccess;
				else
					ReturnWithTrace( rc );
			}
		}
	}

	// Смена текущего каталога на vRootPath.
	if( RootPath )
	{
		if( chdir( vRootPath ) == -1 )
		{
			SetErrorAdv( vVerbose, "Can't change dir to %s: %s", vRootPath, strerror( errno ) );

			Destroy();

			return rcErrorAccess;
		}
		else if( vVerbose >= 2 )
			printf( "   * Change dir to: %s\n", vRootPath );
	}

	return result;
}

EllipsisCompiler::ReturnCode EllipsisCompiler::CompilePatternsFile( const char * aEllipsisPath )
{
	// Открытие файла с шаблонами на чтение.
	FILE* sourceFH = fopen( aEllipsisPath, "r" );
	if( !sourceFH )
	{
		SetErrorAdv( vVerbose, "Can't open file: %s", aEllipsisPath );

		return rcErrorAccess;
	}

	if( vVerbose >= 2 )
		printf( "   * %s\n", aEllipsisPath );

	aTextString tmp, buffer, id;
	unsigned int string = 0;
	unsigned int patterns = 0;
	unsigned int errors = 0;
	unsigned int warnings = 0;

	// Выделение шаблонов эллипсисов.
	nlReturnCode nle;
	while( ( nle = tmp.ReadString( sourceFH ) ) == nlrcSuccess )
	{
		if( NanoLib::ConvertEncoding( tmp, vEncoding, NanoLib::Encoding::UTF8 ) != nlrcSuccess )
			ReturnWithError( rcErrorNoFreeMemory, ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

		string++;

		// Нормализация строки.
		AllTrim( tmp );

		// Пропуск пустых строк.
		if( tmp.empty() )
			continue;

		// Начало шаблона.
		if( strncasecmp( tmp.ToConstChar(), "BEGIN ", 6 ) )
		{
			SetErrorAdv( vVerbose, "Incorrect file format( String number: %d ): %s", string, tmp.ToConstChar() );

			return rcErrorFault;
		}

		// Очистка буффера для шаблона.
		buffer.clear();

		// Установка идентификатора.
		const char * ptr = tmp.ToConstChar() + (tmp.size() ? 6 : 0);
		if( ( nle = id.assign( ptr ) ) != nlrcSuccess )
		{
			if( vVerbose )
				printf( " [FAILED] Internal Error\n\n" );

			SetErrorAdv( 0, "%s.", ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

			return rcErrorNoFreeMemory;
		}
		AllTrim( id );

		while( ( nle = tmp.ReadString( sourceFH ) ) == nlrcSuccess )
		{
			if( NanoLib::ConvertEncoding( tmp, vEncoding, NanoLib::Encoding::UTF8 ) != nlrcSuccess )
				ReturnWithError( rcErrorNoFreeMemory, ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

			string++;

			// Проверка, является ли текущая строка логическим продолжением предыдущей.
			bool contstring = ( tmp[0] == ' ' || tmp[0] == '\t' || (unsigned char)tmp[0] == 160 ) ? true : false;

			// Нормализация строки.
			AllTrim( tmp );

			// Пропуск пустых строк.
			if( tmp.empty() )
				continue;

			if( !strcasecmp( tmp.ToConstChar(), "END" ) )
			{
				buffer.append( "\n" );
				patterns++;

				// Компиляция шаблона.
				InfEngineErrors iee = vCore.CompilePatternFromBuffer( buffer.ToConstChar(), buffer.size(), id, true );

				if( vCore.GetWarnings().size() )
				{
					if( warnings == 0 )
					{
						if( vVerbose == 1 )
							printf( "   * %s\n", aEllipsisPath );
					}

					printf( "\n   Warnings:" );
					for( unsigned int i = 0; i < vCore.GetWarnings().size(); i++ )
					{
						printf( "     > Id: %s\n", vCore.GetWarnings()[i].GetObjectId().c_str() );
						printf( "     > Description: %s\n", vCore.GetWarnings()[i].description() );
						if( vCore.GetWarnings()[i].GetString().c_str() )
                            printf( "     > String: %s\n", vCore.GetWarnings()[i].GetString().c_str() + vCore.GetWarnings()[i].GetPos() );
					}

					++warnings;
				}


				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				{
					if( errors == 0 )
					{
						if( vVerbose == 1 )
							printf( "\n   * %s\n", aEllipsisPath );
					}

					for( unsigned int i = 0; i < vCore.GetErrors().size(); i++ )
					{
						printf( "\n     > Id: %s\n", vCore.GetErrors()[i].GetObjectId().c_str() );
						printf( "     > Description: %s\n", vCore.GetErrors()[i].description() );
						if( vCore.GetErrors()[i].GetString().size() )
                            printf( "     > String: %s\n", vCore.GetErrors()[i].GetString().c_str() + vCore.GetErrors()[i].GetPos() );
					}

					errors++;
				}
				else if( iee != INF_ENGINE_SUCCESS )
				{
					switch( iee )
					{
					case INF_ENGINE_ERROR_NOFREE_MEMORY:
						ReturnWithTrace( rcErrorNoFreeMemory );
					default:
						ReturnWithTrace( rcErrorFault );
					}
				}

				// Конец шаблона.
				break;
			}
			else if( tmp[0] == '/' && tmp[1] == '/' )
				// Пропуск комментариев.
				continue;
			else if( contstring )
			{
				// Обработка строки, являющейся логическим продолжением предыдущей.
				if( !buffer.empty() && ( nle = buffer.append( " " ) ) != nlrcSuccess )
				{
					SetErrorAdv( vVerbose, "%s.", ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

					return rcErrorNoFreeMemory;
				}
				if( ( nle = buffer.append( tmp ) ) != nlrcSuccess )
				{
					SetErrorAdv( vVerbose, "%s.", ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

					return rcErrorNoFreeMemory;
				}
			}
			else
			{
				// Обработка независимой строки.
				if( !buffer.empty() && ( nle = buffer.append( "\n" ) ) != nlrcSuccess )
				{
					SetErrorAdv( vVerbose, "%s.", ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

					return rcErrorNoFreeMemory;
				}
				if( ( nle = buffer.append( tmp ) ) != nlrcSuccess )
				{
					SetErrorAdv( vVerbose, "%s.", ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

					return rcErrorNoFreeMemory;
				}
			}
		}
		if( nle != nlrcSuccess )
		{
			SetErrorAdv( vVerbose, "Incorrect file format( String number: %d ): %s", string, tmp.ToConstChar() );

			return rcErrorFault;
		}
	}
	if( nle != nlrcEOF )
	{
		SetErrorAdv( vVerbose, "Incorrect file format( String number: %d ): %s", string, tmp.ToConstChar() );

		return rcErrorFault;
	}

	// Сообщение об ошибках, найденных в процессе компиляции.
	if( errors )
	{
		if( vVerbose )
			printf( "\n    [FAILED] %u from %u patterns are not compiled( %d%% )\n\n", errors, patterns,  (int)ceil( (double)(100 * errors) / patterns ) );
		return rcUnsuccess;
	}

	return rcSuccess;
}

EllipsisCompiler::ReturnCode EllipsisCompiler::Save( const char * aResultPath )
{
	// Открытие fstorage.
	fstorage* fscmn = fstorage_create();
	if( !fscmn )
	{
		SetErrorAdv( vVerbose, "%s.", "Can't create fstorage" );

		return rcErrorFStorage;
	}

	fstorage_connect_config fscc;
	memset( &fscc, 0, sizeof( fstorage_connect_config ) );
	fscc.pid = FSTORAGE_PID_ELLIPSIS;
	int ret = fstorage_connect( fscmn, aResultPath, O_RDWR | O_CREAT | O_TRUNC, 0644, FSTORAGE_OPEN_READ_WRITE, &fscc );
	if( ret != 0 )
	{
		fstorage_destroy( fscmn );

		SetErrorAdv( vVerbose, "Can't open fstorage. Return code: %d", ret );

		return rcErrorFStorage;
	}

	// Сохранение ядра.
	InfEngineErrors iee = vCore.Save( fscmn );
	if( iee != INF_ENGINE_SUCCESS )
	{
		fstorage_close( fscmn );
		fstorage_destroy( fscmn );

		switch( iee )
		{
		case INF_ENGINE_ERROR_NOFREE_MEMORY:
			ReturnWithTrace( rcErrorNoFreeMemory );
		case INF_ENGINE_ERROR_FSTORAGE:
			ReturnWithTrace( rcErrorFStorage );
		default:
			ReturnWithTrace( rcErrorFault );
		}
	}

	// Закрытие fstorage.
	fstorage_close( fscmn );
	fstorage_destroy( fscmn );

	return rcSuccess;
}
