#include "Parser.hpp"

#include <fstream>

#include "Weight.hpp"

DLParser::DLParser( DLDataWR & aDLDataWR, BaseIndexer & aIndexer,
                    SymbolymBaseRO * aSymbolymBaseRO, SynonymBaseRO * aSynonymBaseRO, nMemoryAllocator & aAllocator ) :
	vDLDataWR( aDLDataWR ), vIndexer( aIndexer ),
    vSymbolymBaseRO( aSymbolymBaseRO ? *aSymbolymBaseRO : aIndexer.GetSymolymsBaseRO() ),
	vSynonymBaseRO( aSynonymBaseRO ? *aSynonymBaseRO : aIndexer.GetSynonymBaseRO() ),
	vAllocator( aAllocator ),
	vPatternStringParser( aDLDataWR, vTablePatternsId, vTableIndexId, vExtendAnswerErrors )
{
	vTablePatternsId.Create();
	vInstrLableTable.Create();
}

InfEngineErrors DLParser::ReadPatternsFromFile( const std::string & aFilePath, bool aEllipsisMode, bool aStrictMode, NanoLib::Encoding aEncoding )
{
	// Открываем файл на чтение.
	std::ifstream source;
	source.open( aFilePath );
	if( source.fail() )
		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open file: %s", aFilePath.c_str() );
	source.sync_with_stdio( false );

	return ReadPatternsFromStream( source, aEllipsisMode, aStrictMode, aFilePath, aEncoding );
}

InfEngineErrors DLParser::ReadPatternsFromStream( std::istream & aStream, bool aEllipsisMode, bool aStrictMode, const std::string & aStreamName, NanoLib::Encoding aEncoding )
{
	// Очистка внутренних данных.
	Reset( true );

	// Пропускаем все пустые строки.
	unsigned int string = 0;

	nlReturnCode nle;
	while( ( nle = vString.ReadString( aStream ) ) == nlrcSuccess )
	{
		string++;

		auto ret = NanoLib::ConvertEncoding( vString, aEncoding, NanoLib::Encoding::UTF8 );
		if( ret != nlrcSuccess ) {
			if( ret == nlrcErrorNoFreeMemory )
				ReturnWithTrace( INF_ENGINE_ERROR_NOFREE_MEMORY );
			else
				ReturnWithTrace( INF_ENGINE_ERROR_FAULT );
		}

		// Нормализация строки.
		AllTrim( vString );

		// Пропуск пустых строк.
		if( vString.empty() )
			continue;

		// Пропуск комментариев.
		if( vString.size() > 2 && vString[0] == '/' && vString[1] == '/' )
			continue;

		// Начало шаблона.
		if( vString.size() > 6 && strncasecmp( vString.ToConstChar(), "BEGIN ", 6 ) )
		{
			if( !vErrors.push_back( { DLErrorId::peInvalidPatternString, 0 }, vString, {} ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			return INF_ENGINE_ERROR_CANT_PARSE;
		}

		// Очистка буффера.
		vBuffer.clear();

		// Установка идентификатора.
		const char * ptr = vString.ToConstChar() + ( vString.size() ? 6 : 0 );
		nle = vObjectId.assign( ptr );
		if( nle != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		AllTrim( vObjectId );

		while( ( nle = vString.ReadString( aStream ) ) == nlrcSuccess )
		{
			string++;

			auto ret = NanoLib::ConvertEncoding( vString, aEncoding, NanoLib::Encoding::UTF8 );
			if( ret != nlrcSuccess ) {
				if( ret == nlrcErrorNoFreeMemory )
					ReturnWithTrace( INF_ENGINE_ERROR_NOFREE_MEMORY );
				else
					ReturnWithTrace( INF_ENGINE_ERROR_FAULT );
			}

			bool contstring = ( vString[0] == ' ' || vString[0] == '\t' ) ? true : false;
			AllTrim( vString );
			if( vString.empty() )
				continue;

			if( !strcasecmp( vString.ToConstChar(), "END" ) )
			{
				vBuffer.append( "\n" );

				InfPattern Pattern;
				InfEngineErrors iee = ParsePattern( vBuffer, Pattern, aEllipsisMode, aStrictMode, aStreamName );
				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				{
					vFailCounter++;

					break;
				}
				else if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

                if( ( iee = Pattern.ApplySynonyms( vSymbolymBaseRO, vSynonymBaseRO, vAllocator ) ) != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

				unsigned int id;
				iee = vIndexer.PreparePattern( Pattern );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

				iee = vDLDataWR.AddPattern( Pattern, id );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );
				else
				{
					iee = vIndexer.AddPattern( Pattern, id );
					if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					{
						if( !vErrors.push_back( { DLErrorId::peTextWithoutTerms, 0 }, {}, vObjectId ) )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

						vFailCounter++;

						break;
					}
					else if( iee != INF_ENGINE_SUCCESS )
						ReturnWithTrace( iee );

					// Регистрация идентификатора шаблона (для работы тэга ExtendAnswer).
					if( INF_ENGINE_SUCCESS != ( iee = AddIdLableToTable( vObjectId.ToConstChar(), id ) ) )
					{
						if( INF_ENGINE_WARN_UNSUCCESS == iee )
							break;
						else
							ReturnWithTrace( iee );
					}

					// Регистрация метки --Label (для работы тэга ExtendAnswer).
					if( Pattern.GetInstrLabel() )
					{
						if( INF_ENGINE_SUCCESS != ( iee = AddIdLableToTable( Pattern.GetInstrLabel(), id ) ) )
						{
							if( INF_ENGINE_WARN_UNSUCCESS == iee )
								break;
							else
								ReturnWithTrace( iee );
						}
					}
				}

				vSuccessCounter++;

				// Конец шаблона.
				break;
			}
			else if( vString[0] == '/' && vString[1] == '/' )
				continue;
			else if( contstring )
			{
				// Проверка на случайно вставленный перед служебным символом пробел.
				if( vString[0] == '+' || vString[0] == '$' || vString[0] == '@' || vString[0] == '#' )
					if( !vWarnings.push_back( { DLErrorId::pePatternUnnecessarySpace, 0 }, vString, vObjectId ) )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				if( !vBuffer.empty() )
				{
					nle = vBuffer.append( " " );
					if( nle != nlrcSuccess )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
				nle = vBuffer.append( vString );
				if( nle != nlrcSuccess )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			else
			{
				if( !vBuffer.empty() )
				{
					nle = vBuffer.append( "\n" );
					if( nle != nlrcSuccess )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
				nle = vBuffer.append( vString );
				if( nle != nlrcSuccess )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
		}
		if( nle != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_CANT_PARSE, "Incorrect file format( String number: %d ): %s", string, vString.ToConstChar() );
	}
	if( nle != nlrcEOF )
		ReturnWithError( INF_ENGINE_ERROR_CANT_PARSE, "Incorrect file format( String number: %d ): %s", string, vString.ToConstChar() );


	if( vFailCounter )
		return INF_ENGINE_ERROR_CANT_PARSE;
	else
		return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLParser::MakeResolving( unsigned int & aSuccessResolving, unsigned int & aFaultResolving, ErrorsExt & aErrors )
{
	aSuccessResolving = 0;
	aFaultResolving = 0;

	bool all_right = true;
	for( unsigned int id_n = 0; id_n < vTableIndexId.size(); ++id_n ) {
		if( static_cast<unsigned int>( -1 ) == vTableIndexId[id_n] ) {
			try {
				aErrors.push_back( vExtendAnswerErrors[id_n] );
			} catch( ... ) {
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			
			all_right = false;
			++aFaultResolving;
		} else
			++aSuccessResolving;
	}

	vDLDataWR.SetPatternIdIndex( &vTablePatternsId, &vTableIndexId );

	if( !all_right )
		return INF_ENGINE_ERROR_CANT_PARSE;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLParser::ReadPatternsFromBuffer( const char * aBuffer, unsigned int aBufferSize, bool aEllipsisMode, bool aStrictMode )
{
	// Проверка параметров.
	if( !aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Очистка внутренних данных.
	Reset( true );

	// Пропускаем все пустые строки.
	unsigned int string = 0;
	vFailCounter = 0;
	vSuccessCounter = 0;
	unsigned int pos = 0;
	while( 1 )
	{
		if( pos >= aBufferSize )
			break;

		unsigned int end;
		for( end = pos; end < aBufferSize && aBuffer[end] != '\n'; end++ );
		string++;

		nlReturnCode nle = vString.assign( aBuffer + pos, end - pos );
		if( nle != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		AllTrim( vString );

		if( vString.empty() )
		{
			pos = end + 1;
			continue;
		}

		// Пропуск комментариев.
		if( vString.size() > 2 && vString[0] == '/' && vString[1] == '/' )
		{
			pos = end + 1;
			continue;
		}

		// Начало шаблона.
		if( strncasecmp( vString.ToConstChar(), "BEGIN ", 6 ) )
			ReturnWithError( INF_ENGINE_ERROR_CANT_PARSE,
							 "Incorrect file format( String number: %d ): %s", string, vString.ToConstChar() );

		// Очистка буффера.
		vBuffer.clear();

		// Установка идентификатора.
		const char * ptr = vString.ToConstChar() + ( vString.size() ? 6 : 0 );
		nle = vObjectId.assign( ptr );
		if( nle != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		AllTrim( vObjectId );

		while( 1 )
		{
			if( pos >= aBufferSize )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't find END." );

			pos = end + 1;
			for( end = pos; end < aBufferSize && aBuffer[end] != '\n'; end++ );
			string++;

			nle = vString.assign( aBuffer + pos, end - pos );
			if( nle != nlrcSuccess )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			bool contstring = ( vString[0] == ' ' || vString[0] == '\t' || (unsigned char)vString[0] == 160 ) ? true : false;

			AllTrim( vString );

			if( vString.empty() )
				continue;

			if( !strcasecmp( vString.ToConstChar(), "END" ) )
			{
				vBuffer.append( "\n" );

				InfPattern Pattern;
				InfEngineErrors iee = ParsePattern( vBuffer, Pattern, aEllipsisMode, aStrictMode );
				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				{
					vFailCounter++;
					break;
				}
				else if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

				vSuccessCounter++;

                if( ( iee = Pattern.ApplySynonyms( vSymbolymBaseRO, vSynonymBaseRO, vAllocator ) ) != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

				unsigned int id = 0;
				vIndexer.PreparePattern( Pattern );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

				iee = vDLDataWR.AddPattern( Pattern, id );
				if( iee != INF_ENGINE_SUCCESS )
				{
					if( iee != INF_ENGINE_ERROR_CANT_PARSE )
						ReturnWithTrace( iee );

					return iee;
				}
				else
				{
					iee = vIndexer.AddPattern( Pattern, id );

					if( iee != INF_ENGINE_SUCCESS )
					{
						if( iee != INF_ENGINE_ERROR_CANT_PARSE )
							ReturnWithTrace( iee );

						if( aStrictMode )
							return iee;
					}
				}

				// Конец шаблона.
				break;
			}
			else if( vString[0] == '/' && vString[1] == '/' )
				continue;
			else if( contstring )
			{
				if( !vBuffer.empty() )
				{
					nle = vBuffer.append( " " );
					if( nle != nlrcSuccess )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
				nle = vBuffer.append( vString );
				if( nle != nlrcSuccess )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			else
			{
				if( !vBuffer.empty() )
				{
					nle = vBuffer.append( "\n" );
					if( nle != nlrcSuccess )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
				nle = vBuffer.append( vString );
				if( nle != nlrcSuccess )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
		}

		pos = end + 1;
	}

	if( vFailCounter )
		return INF_ENGINE_ERROR_CANT_PARSE;
	else
		return INF_ENGINE_SUCCESS;
}

#define ReturnDictParsingError( aErrorDescription, vErrorstring ) \
	do \
	{ \
		nlReturnCode nlrc = aErrorDescription.assign( aErrorDescription ); \
		if( nlrc == nlrcErrorNoFreeMemory ) \
			ReturnWithErrorStrict( vLogSystem, INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY  ); \
		nlReturnCode nlrc = vErrorstring.assign( aErrorDescription ); \
		if( nlrc == nlrcErrorNoFreeMemory ) \
			ReturnWithErrorStrict( vLogSystem, INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY  ); \
		return INF_ENGINE_ERROR_CANT_PARSE; \
	} while( 0 );

InfEngineErrors DLParser::ReadDictFromFile( const char * aFilePath, bool aStrictMode, const char * aDictName, unsigned int aDictNameLength,
											aTextString & aErrorDescription, aTextString & vErrorstring, NanoLib::Encoding aEncoding )
{
	// Проверка аргументов.
	if( !aFilePath )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Очистка данных.
	Reset( true );

	// Открытие файла.
	FILE * aFH = fopen( aFilePath, "r" );
	if( !aFH )
		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open file %s: %s", aFilePath, strerror( errno ) );

	// Очистка массива строк словаря.
	vDictStrings.clear();

	// Чтение из файла.
	nlReturnCode nlr;
	vBuffer.clear();

	while( ( nlr = vString.ReadString( aFH ) ) == nlrcSuccess )
	{
		// Добавление очередной считанной строки в буфер.
		if( nlrcSuccess != ( nlr = vBuffer.append( vString ) ) || nlrcSuccess != ( nlr = vBuffer.append( "\n" ) ) )
		{
			fclose( aFH );
			ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't read dict %s. Return code: %d", aFilePath, nlr );
		}

	}

	// Закрытие файла.
	fclose( aFH );

	if( nlr != nlrcEOF )
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't read dict %s. Return code: %d", aFilePath, nlr );

	// Добавление словаря из подготовленного буфера.
	InfEngineErrors iee = ReadDictFromBuffer( aDictName, aDictNameLength, vBuffer.ToConstChar(), vBuffer.size(),
											  aStrictMode, aErrorDescription, vErrorstring, aEncoding, false );
	if( INF_ENGINE_SUCCESS != iee )
	{
		if( INF_ENGINE_ERROR_CANT_PARSE == iee )
			return INF_ENGINE_ERROR_CANT_PARSE;
		ReturnWithTrace( iee );
	}


	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLParser::ReadDictFromBuffer( const char * aDictName, unsigned int aDictNameLength, const char * aDict, unsigned int aDictLen, bool aStrictMode,
											  aTextString & aErrorDescription, aTextString & vErrorstring, NanoLib::Encoding aEncoding, bool aUserDict )
{
	// Очистка данных.
	Reset( true );

	// Очистка массива строк словаря.
	vDictStrings.clear();

	// Чтение ловаря из буфера.
	unsigned pos = 0;
	unsigned int string = 0;
	bool first_line = true;

	while( 1 )
	{
		if( pos >= aDictLen )
			break;

		unsigned int end;
		for( end = pos; end < aDictLen && aDict[end] != '\n'; end++ )
			;
		string++;

		nlReturnCode nle = vString.assign( aDict + pos, end - pos );
		if( nle != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		if( NanoLib::ConvertEncoding( vString, aEncoding, NanoLib::Encoding::UTF8 ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		AllTrim( vString );

		if( vString.empty() )
		{
			pos = end + 1;
			continue;
		}

		// Нормализация строки.
		AllTrim( vString );

		// Пропуск пустых строк.
		if( vString.empty() )
			continue;

		// Комментарии.
		if( ( vString[0] == '\\' && vString[1] == '\\' ) || ( vString[0] == '/' && vString[1] == '/' ) )
		{
			/**
			 *  Замещаем комментарии пустой строкой для корректности вычисления номера строки при генерации
			 * ошибки разбора.
			 */
		}
		else
		{
			// Разбор инструкции словаря.
			InfDictInstruction * DictInstruction = nullptr;
			InfEngineErrors iee = vPatternStringParser.ParseDictInstruction( vString.ToConstChar(), vString.size(), DictInstruction, vAllocator, aErrorDescription, vErrorstring );
			if( INF_ENGINE_SUCCESS == iee )
			{
				vDictInstructions.push_back( DictInstruction );
				if( vDictInstructions.no_memory() )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			else if( INF_ENGINE_ERROR_CANT_PARSE == iee )
				return INF_ENGINE_ERROR_CANT_PARSE;
			else if( INF_ENGINE_WARN_UNSUCCESS == iee )
			{
				// Разбор строки словаря.
				InfDictString * DictString = nullptr;
				iee = vPatternStringParser.ParseDictString( vString.ToConstChar(), vString.size(), DictString, vAllocator, aErrorDescription, vErrorstring );
				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					return INF_ENGINE_ERROR_CANT_PARSE;

				// Подстановка синонимов.
                if( ( iee = DictString->ApplySynonyms( vSymbolymBaseRO, vSynonymBaseRO, vAllocator ) ) != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

				// Добавление разобранной строки в хранилище шаблонов для словарей.
				unsigned int pattern_id = 0;
				if( INF_ENGINE_SUCCESS != ( iee = vDLDataWR.AddPatternString( InfPatternItems::Array( DictString->GetItems(), DictString->GetItemsCount() ), pattern_id ) ) )
					ReturnWithTrace( iee );
				DictString->SetId( pattern_id );

				// Проверка дополнительных условий, налагаемых на первую строку словаря.
				if( first_line )
				{
					// Если первая строка помечена как починённая.
					if( !DictString->IsMain() )
					{
						if( aUserDict )
						{
							// Принудительно помечаем первую строку пользовательского словаря как главную.
							DictString->SetMainFlag( true );
						}
						else
						{
							// Первая строка словаря не может быть подчинённой.
							aErrorDescription.assign( " Dictionary can't starts with \"=>\" " );
							vErrorstring.assign( vString.ToConstChar() );
							return INF_ENGINE_ERROR_CANT_PARSE;
						}
					}

					first_line = false;
				}

				vDictStrings.push_back( DictString );
				if( vDictStrings.no_memory() )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
			else
				ReturnWithTrace( iee );
		}

		pos = end + 1;
	}

	// Создание словаря.
	InfDict * Dict = nAllocateObject( vAllocator, InfDict );
	if( !Dict )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new( (nMemoryAllocatorElementPtr*)Dict )InfDict;

	// Копирование списка строк.
	InfDictString ** dict_strings = nullptr;
	if( vDictStrings.size() )
	{
		dict_strings = nAllocateObjects( vAllocator, InfDictString*, vDictStrings.size() );
		if( !dict_strings )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( dict_strings, vDictStrings.get_buffer(), vDictStrings.size() * sizeof( InfDictString* ) );
	}

	// Копирование списка инструкций.
	InfDictInstruction ** dict_instructions = nullptr;
	if( vDictInstructions.size() )
	{
		dict_instructions = nAllocateObjects( vAllocator, InfDictInstruction*, vDictInstructions.size() );
		if( !dict_instructions )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( dict_instructions, vDictInstructions.get_buffer(), vDictInstructions.size() * sizeof( InfDictInstruction* ) );
	}

	// Копирование данных в словарь.
	Dict->SetDictStrings( (const InfDictString**)dict_strings, vDictStrings.size() );
	Dict->SetDictInstructions( (const InfDictInstruction**)dict_instructions, vDictInstructions.size() );
	Dict->SetName( aDictName, aDictNameLength );
	const unsigned int * dict_id = vDLDataWR.SearchDictName( aDictName, aDictNameLength );
	if( !dict_id )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Dictionary \"%s\" wasn't registered", aDictName );
	Dict->SetID( *dict_id );

	// Добавление словаря в базу.
	InfEngineErrors iee = aUserDict ? vDLDataWR.AddUserDict( Dict ) : vDLDataWR.AddMainDict( Dict );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Индексация словаря.
	iee = vIndexer.AddDict( Dict, vDLDataWR.GetPatternsStorage() );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLParser::ParsePattern( const aTextString & aPattern, InfPattern & aResult, bool aEllipsisMode, bool aStrictMode, const std::string & aFilePath )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Сброс внутренних данных.
	Reset();

	// Настройка парсера.
	vPatternStringParser.SetEllipsisMode( aEllipsisMode );
	vPatternStringParser.SetStrictMode( aStrictMode );
	vPatternStringParser.Reset();

	// Разбираем шаблон.
	unsigned int pos = 0;

	// Метка шаблона.
	char * PatternLabel = nullptr;
	unsigned int PatternLabelLength = 0;

	// Пропускаем пустые строки.
	for(; pos < aPattern.size() && aPattern[pos] == '\n'; pos++ ) ;

	// Идентификатор InfPerson.
	unsigned int infperson_id = 0;

	// Аргумент инструкции --Label.1
	aTextString Label;
	bool label_expected = true;

	// Флаги для ослеживания порядка следования шаблон-строк разных типов.
	bool question_was_meet = false;
	bool answer_was_meet = false;
	bool that_was_meet = false;
	bool condition_was_meet = false;

	while( pos < aPattern.size() )
	{
		// Получаем строку для разбора.
		unsigned int end = pos + 1;

		do
		{
			// Ищем конец строки.
			const char * ptr = std::strchr( aPattern.ToConstChar() + end, '\n' );
			if( !ptr )
			{
				// Если конца строки не найдено, значит шаблон был некорректно загружен.
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
			}
			// Сдвигаем указатель end на начало следующей строки.
			end = ptr - aPattern.c_str() + 1;

			// Поиск всех продолжений строки.
		} while( end < aPattern.size() && ( TextFuncs::IsSpace( aPattern[end] ) || aPattern[end] == '\n' ) );

		// Соединяем строку с ее продолжениями.
		if( vString.assign( aPattern.ToConstChar() + pos, end - pos - 1 ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Нормализация шаблон-строки.
		AllTrim( vString );

		// Инструкция.
		if( vString[0] == '-' && vString[1] == '-' )
		{
			// Инструкция --Label.
			aTextString tmp_label;
			iee = vPatternStringParser.ParseInstrLabel( vString.ToConstChar() + 2, vString.size() - 2, tmp_label );
			if( iee == INF_ENGINE_SUCCESS )
			{
				if( label_expected )
				{
					if( Label.empty() )
					{
						if( Label.assign( tmp_label ) != nlrcSuccess )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					}
					else if( aStrictMode )
					{
						if( !vErrors.push_back( { DLErrorId::peInstrLabelTooMach, 0 }, Label, vObjectId ) )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						return INF_ENGINE_ERROR_CANT_PARSE;
					}
					label_expected = false;
				}
				else if( aStrictMode )
				{
					if( !vErrors.push_back( { DLErrorId::peInstrLabelUnexpected, 0 }, Label, vObjectId ) )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					return INF_ENGINE_ERROR_CANT_PARSE;
				}
			}
			// Условный оператор уровня шаблона.
			// If
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
			{
				iee = vPatternStringParser.ParseTLCS_If( vString.ToConstChar() + 2, vString.size() - 2, vTLCS, vTmpAllocator );
				if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithTrace( iee );
			}

			// Elseif
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
			{
				iee = vPatternStringParser.ParseTLCS_Elsif( vString.ToConstChar() + 2, vString.size() - 2, vTLCS, vTmpAllocator );
				if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithTrace( iee );
			}

			// Else
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
			{
				iee = vPatternStringParser.ParseTLCS_Else( vString.ToConstChar() + 2, vString.size() - 2, vTLCS, vTmpAllocator );
				if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithTrace( iee );
			}

			// Endif
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
			{
				iee = vPatternStringParser.ParseTLCS_Endif( vString.ToConstChar() + 2, vString.size() - 2, vTLCS, vTmpAllocator );
				if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithTrace( iee );
			}

			// Switch
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
			{
				iee = vPatternStringParser.ParseTLCS_Switch( vString.ToConstChar() + 2, vString.size() - 2, vTLCS, vTmpAllocator );
				if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithTrace( iee );
			}

			// Case
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
			{
				iee = vPatternStringParser.ParseTLCS_Case( vString.ToConstChar() + 2, vString.size() - 2, vTLCS, vTmpAllocator );
				if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithTrace( iee );
			}

			// Default
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
			{
				iee = vPatternStringParser.ParseTLCS_Default( vString.ToConstChar() + 2, vString.size() - 2, vTLCS, vTmpAllocator );
				if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithTrace( iee );
			}

			// Endswitch
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
			{
				iee = vPatternStringParser.ParseTLCS_Endswitch( vString.ToConstChar() + 2, vString.size() - 2, vTLCS, vTmpAllocator );
				if( iee != INF_ENGINE_SUCCESS && iee != INF_ENGINE_WARN_UNSUCCESS && iee != INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithTrace( iee );
			}

			// Invalid pattern string
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
			{
				if( !vErrors.push_back( { DLErrorId::peInvalidPatternString, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}

			// Parsing error
			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			{
				pos = end;

				TryWithTrace( ExportErrors( vPatternStringParser ), INF_ENGINE_SUCCESS );

				// Если установлен флаг игнорирования ошибок, то просто пропускаем некорректную шаблон-строку.
				if( !aStrictMode || !vPatternStringParser.GetErrorsDescriptions().size() )
					continue;
				else
					return INF_ENGINE_ERROR_CANT_PARSE;

			}
		}
		// Шаблон-условие.
		else if( vString[0] == '+' )
		{
			// Проверка правильности порядка следования шаблон-строк разных типов.
			if( question_was_meet )
			{
				if( !vErrors.push_back( { DLErrorId::peOrderConditionAfterQuestion, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}
			if( that_was_meet )
			{
				if( !vErrors.push_back( { DLErrorId::peOrderConditionAfterThat, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}

			InfPatternItems::ConditionRW * Condition = nullptr;

            auto iee = vPatternStringParser.ParseCondTypePatternString( vString.ToConstChar(), vString.size(), 1, Condition, infperson_id, vTmpAllocator );

			TryWithTrace( ExportErrors( vPatternStringParser ), INF_ENGINE_SUCCESS );

			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			{
				pos = end;

				if( !aStrictMode || !vPatternStringParser.GetErrorsDescriptions().size() )
					continue;
				else
					return INF_ENGINE_ERROR_CANT_PARSE;
			}
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			// Вычисление веса шаблон-условия.
			PatternWeight ConditionWeight;
			if( !strcasecmp( vDLDataWR.GetVarsRegistry().GetVarNameById( Condition->vCondition->GetVarId() ), "topic" ) )
				ConditionWeight.SetTopicWeight();
			else if( !strncasecmp( vDLDataWR.GetVarsRegistry().GetVarNameById( Condition->vCondition->GetVarId() ), "tree_", 5 ) )
				ConditionWeight.SetTreeWeight();
			else if( Condition->vCondition->GetVarId() == InfEngineVarThatAnchor )
				ConditionWeight.SetAnchorWeight();
			Condition->vWeight = ConditionWeight;

			// Добавление условия в массив разобранных условий.
			vConditions.push_back( Condition );
			if( vConditions.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			condition_was_meet = true;
		}
		// Шаблон-вопрос.
		else if( vString[0] == '$' )
		{
			// Проверка правильности порядка следования шаблон-строк разных типов.
			if( answer_was_meet )
			{
				if( !vErrors.push_back( { DLErrorId::peOrderQuestionAfterAnswer, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}
			if( that_was_meet )
			{
				if( !vErrors.push_back( { DLErrorId::peOrderQestionAfterThat, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}

			// Проверяем, ограничение максимального числа шаблон-вопросов в одном шаблоне.
			if( vQuestions.size() >= 1 << 8 )
			{
				// Если установлен флаг игнорирования ошибок, то просто пропускаем лишние шаблон-вопросы.
				if( !aStrictMode )
					continue;

				if( !vErrors.push_back( { DLErrorId::peToManyQuestions, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				return INF_ENGINE_ERROR_CANT_PARSE;
			}

			// Проверка на событие.
			if( !strcasecmp( "$$REQUEST$$", vString.ToConstChar() ) )
				aResult.BindEvent( InfPattern::Events::Request );
			else if( !strcasecmp( "$$SETSESSION$$", vString.ToConstChar() ) )
				aResult.BindEvent( InfPattern::Events::SetSession );
			else if( !strcasecmp( "$$UPDATESESSION$$", vString.ToConstChar() ) )
				aResult.BindEvent( InfPattern::Events::UpdateSession );
			else
			{
				// Предупреждение о том, что начало шаблон-ответа похоже на описание события.
				if( vString.size() > 1 && !strncasecmp( "$$", vString.ToConstChar(), 2 ) )
					if( !vWarnings.push_back( { DLErrorId::peQuestionsLooksLikeEvent, 0 }, vString, vObjectId ) )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );


				// Разбор шаблон-вопроса.
                auto iee = vPatternStringParser.ParseQstTypePatternString( vString.ToConstChar(), vString.size(), 1, vItems, vReferences, vTmpAllocator );

				TryWithTrace( ExportErrors( vPatternStringParser ), INF_ENGINE_SUCCESS );

				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				{
					pos = end;

					// Если установлен флаг игнорирования ошибок или ошибок нет, то просто пропускаем некорректный шаблон-вопрос.
					if( !aStrictMode || !vPatternStringParser.GetErrorsDescriptions().size() )
						continue;
					else
						return INF_ENGINE_ERROR_CANT_PARSE;
				}
				else if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

				// Вычисление веса шаблона.
				PatternWeight Weight;
				for( unsigned int i = 0; i < vConditions.size(); i++ )
					Weight.AddWeight( vConditions[i]->vWeight );
				Weight.SetConditionsNumber( vConditions.size() );
				for( unsigned int i = 0; i < vItems.size(); i++ )
				{
					if( vItems[i]->GetType() == InfPatternItems::itStar || vItems[i]->GetType() == InfPatternItems::itEliStar )
						break;

					if( i == vItems.size() - 1 )
						Weight.SetUnstarred();
				}

				// Выделение памяти.
				nAllocateNewObjects( vTmpAllocator, InfPatternItems::Base*, vItems.size(), Items );
				if( !Items && vItems.size() )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				memcpy( Items, vItems.get_buffer(), vItems.size_bytes() );

				// Выделение памяти.
				nAllocateNewObject( vTmpAllocator, InfPatternItems::QuestionWR, Qst );
				if( !Qst )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				new( (nMemoryAllocatorElementPtr*)Qst ) InfPatternItems::QuestionWR( Items, vItems.size(), Weight );
				Qst->SetReferences( vReferences.get_buffer(), vReferences.size() );

				// Добавление шаблон-вопроса в массив элементов шаблона.
				vQuestions.push_back( Qst );
				if( vQuestions.no_memory() )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}

			label_expected = false;
			question_was_meet = true;
		}
		// That-строка.
		else if( vString[0] == '@' )
		{
			// Использовать that-строки в привязанных к событиям шаблонах не разрешатся.
			if( aResult.IsBoundToEvent() )
			{
				if( !vErrors.push_back( { DLErrorId::peModifierThatAndEvent, 0 }, {}, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}

			// Проверка правильности порядка следования шаблон-строк разных типов.
			if( answer_was_meet )
			{
				if( !vErrors.push_back( { DLErrorId::peOrderThatAfterAnswer, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}
			if( !question_was_meet )
			{
				if( !vErrors.push_back( { DLErrorId::peOrderThatBeforeQuestion, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}
			if( that_was_meet )
			{
				if( !vErrors.push_back( { DLErrorId::peOneThatOnly, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}

            auto iee = vPatternStringParser.ParseThatTypePatternString( vString.ToConstChar(), vString.size(), 1, vItems, vTmpAllocator );

			TryWithTrace( ExportErrors( vPatternStringParser ), INF_ENGINE_SUCCESS );

			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			{
				pos = end;

				if( !aStrictMode || !vPatternStringParser.GetErrorsDescriptions().size() )
					continue;
				else
					return INF_ENGINE_ERROR_CANT_PARSE;

				// Если установлен флаг игнорирования ошибок, то просто пропускаем некорректный шаблон-that.
				if( !aStrictMode )
					continue;

				return iee;
			}
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			// Выделение памяти.
			nAllocateNewObjects( vTmpAllocator, InfPatternItems::Base*, vItems.size(), Items );
			if( !Items )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			memcpy( Items, vItems.get_buffer(), vItems.size_bytes() );

			// Выделение памяти.
			vThat = nAllocateObject( vTmpAllocator, InfPatternItems::QuestionWR );
			if( !vThat )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			new ( (nMemoryAllocatorElementPtr*)vThat ) InfPatternItems::QuestionWR( Items, vItems.size() );

			for( unsigned int i = 0; i < vConditions.size(); i++ )
			{
				PatternWeight Weight( vConditions[i]->vWeight );
				Weight.SetAnchorWeight();
				vConditions[i]->vWeight = Weight;
			}

			label_expected = false;
			that_was_meet = true;
		}
		// Ответы.
		else if( vString[0] == '#' )
		{
			// Проверка правильности порядка следования шаблон-строк разных типов.
			if( !question_was_meet )
			{
				if( !vErrors.push_back( { DLErrorId::peOrderAnswerBeforeQuestion, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}

			bool IsMultiple;

			InfPatternItems::AnswerRW Answer;

			// Разбор модификаторов.
			unsigned int local_pos = 0;
			local_pos = TextFuncs::SkipSpaces( vString.ToConstChar(), local_pos + 1, vString.size() );

			while( vString.size() >= local_pos + 2 && vString[local_pos] == ':' )
			{
				if( !strncasecmp( vString.ToConstChar() + local_pos, ":l:", 3 ) )
					Answer.SetModifier( InfPatternItems::AnswerRW::Modifiers::Last ), local_pos += 2;
				else if( !strncasecmp( vString.ToConstChar() + local_pos, ":u:", 3 ) )
					Answer.SetModifier( InfPatternItems::AnswerRW::Modifiers::Uniq ), local_pos += 2;
				else if( !strncasecmp( vString.ToConstChar() + local_pos, ":lu:", 4 ) )
					Answer.SetModifier( InfPatternItems::AnswerRW::Modifiers::LightUniq ), local_pos += 3;
				else if( !strncasecmp( vString.ToConstChar() + local_pos, ":f:", 3 ) )
				{
					if( !aResult.CheckEvent( InfPattern::Events::Request ) )
					{
						if( !vErrors.push_back( { DLErrorId::peModifierFirstWithoutRequestEvent, 0 }, {}, vObjectId ) )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						return INF_ENGINE_ERROR_CANT_PARSE;
					}
					Answer.SetModifier( InfPatternItems::AnswerRW::Modifiers::First );
					local_pos += 2;
				}
				else if( vString.size() >= local_pos + 5 && !strncasecmp( vString.ToConstChar() + local_pos, ":last:", 6 ) )
					Answer.SetModifier( InfPatternItems::AnswerRW::Modifiers::Last ), local_pos += 5;
				else if( vString.size() >= local_pos + 5 && !strncasecmp( vString.ToConstChar() + local_pos, ":uniq:", 6 ) )
					Answer.SetModifier( InfPatternItems::AnswerRW::Modifiers::Uniq ), local_pos += 5;
				else if( vString.size() >= local_pos + 6 && !strncasecmp( vString.ToConstChar() + local_pos, ":luniq:", 7 ) )
					Answer.SetModifier( InfPatternItems::AnswerRW::Modifiers::LightUniq ), local_pos += 6;
				else if( vString.size() >= local_pos + 6 && !strncasecmp( vString.ToConstChar() + local_pos, ":first:", 7 ) )
				{
					if( !aResult.CheckEvent( InfPattern::Events::Request ) )
					{
						if( !vErrors.push_back( { DLErrorId::peModifierFirstWithoutRequestEvent, 0 }, {}, vObjectId ) )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						return INF_ENGINE_ERROR_CANT_PARSE;
					}
					Answer.SetModifier( InfPatternItems::AnswerRW::Modifiers::First );
					local_pos += 6;
				}
				else if( vString[local_pos + 1] == '+' || vString[local_pos + 1] == '-' )
				{
					// Поиск конца модификатора частичного веса шаблон-ответа.
					unsigned int end = local_pos + 1;
					while( vString.size() > local_pos && vString[end] != ':' )
						++end;
					bool parsed = false;
					if( end < vString.size() )
					{
						// Разбор числа.
						aTextString num_str;
						if( nlrcSuccess != num_str.assign( vString.ToConstChar() + local_pos + 1, end - local_pos - 1 ) )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						int num = TextFuncs::ToInteger( num_str.ToConstChar(), &parsed );
						if( parsed )
						{
							Answer.SetWeightModifier( num );
							local_pos = end;
						}
					}

					if( !parsed )
					{
						if( Answer.IsModified() )
							local_pos++;
						break;
					}
				}
				else
				{
					if( Answer.IsModified() )
						local_pos++;
					break;
				}
			}

			if( Answer.IsLast() && Answer.IsLUniq() )
			{
				if( !vErrors.push_back( { DLErrorId::peInvalidModifier, 0 }, vString, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}


			Answer.SetTLCS( vPatternStringParser.GetTLCSStack().vTLCS.size() ? vPatternStringParser.GetTLCSStack().vTLCS.back().vId : static_cast<unsigned int>( -1 ) );

			// Очистка вспомогательного массива.
			vItems.clear();
			vWaitingForResolvingTmp.clear();
            auto iee = vPatternStringParser.ParseAnsTypePatternString( vString.ToConstChar(), vString.size(), local_pos, vItems, vWaitingForResolvingTmp, vTmpAllocator, IsMultiple,
																	   false, vObjectId.std_str(), aFilePath );

			TryWithTrace( ExportErrors( vPatternStringParser ), INF_ENGINE_SUCCESS );

			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			{
				pos = end;

				if( !aStrictMode || !vPatternStringParser.GetErrorsDescriptions().size() )
					continue;
				else
					return INF_ENGINE_ERROR_CANT_PARSE;

				// Если установлен флаг игнорирования ошибок, то просто пропускаем некорректный шаблон-ответ.
				if( !aStrictMode )
					continue;

				return iee;
			}
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );

			// Выделение памяти.
			nAllocateNewObjects( vTmpAllocator, InfPatternItems::Base*, vItems.size(), Items );
			if( !vItems.empty() && !Items )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			memcpy( Items, vItems.get_buffer(), vItems.size_bytes() );
			Answer.Set( Items, vItems.size() );

			vAnswers.push_back( Answer );
			if( vAnswers.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Выделение памяти.
			nAllocateNewObject( vTmpAllocator, InfPatternItems::Array, wfr );
			if( !wfr )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			new( (nMemoryAllocatorElementPtr*)wfr ) InfPatternItems::Array();

			// Выделение памяти.
			nAllocateNewObjects( vTmpAllocator, InfPatternItems::Base*, vWaitingForResolvingTmp.size(), wfr_buffer );
			if( !vWaitingForResolvingTmp.empty() && !wfr_buffer )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			memcpy( wfr_buffer, vWaitingForResolvingTmp.get_buffer(), vWaitingForResolvingTmp.size_bytes() );

			wfr->Set( wfr_buffer, vWaitingForResolvingTmp.size() );

			vWaitingForResolving.push_back( wfr );
			if( vWaitingForResolving.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			label_expected = false;
			answer_was_meet = true;
		}
		else
		{
			if( !vErrors.push_back( { DLErrorId::peInvalidPatternString, 0 }, vString, vObjectId ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			return INF_ENGINE_ERROR_CANT_PARSE;
		}

		pos = end;
	}

	// Проверка завершенности всех условных операторов уровня шаблона.
	if( vPatternStringParser.GetTLCSStack().vTLCS.size() ) {
		/** @todo Нужно добавить информацию о строке с описанием открывающегося оператора для корректного описания ошибки. */
		switch( vPatternStringParser.GetTLCSStack().vTLCS.back().vType ) {
			case InfPatternItems::TLCS_WR::Type::If:
			case InfPatternItems::TLCS_WR::Type::Elsif:
			case InfPatternItems::TLCS_WR::Type::Else:
			case InfPatternItems::TLCS_WR::Type::Endif:
				if( !vErrors.push_back( { DLErrorId::peTLCS_IncompleteOperatorIf, 0 }, {}, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				break;

			case InfPatternItems::TLCS_WR::Type::Switch:
			case InfPatternItems::TLCS_WR::Type::Case:
			case InfPatternItems::TLCS_WR::Type::Default:
			case InfPatternItems::TLCS_WR::Type::Endswitch:
				if( !vErrors.push_back( { DLErrorId::peTLCS_IncompleteOperatorSwitch, 0 }, {}, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				break;

			case InfPatternItems::TLCS_WR::Type::Unknown:
				if( !vErrors.push_back( { DLErrorId::peTLCS_IncompleteOperator, 0 }, {}, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				break;
		}
		return INF_ENGINE_ERROR_CANT_PARSE;
	}

	// Проверка метки на уникальность.
	if( !Label.empty() )
	{
		const unsigned int * id_num_p = vInstrLableTable.Search( Label.ToConstChar() );
		if( id_num_p )
		{
			// Метка уже использовалась ранее в другом шаблоне.
			if( aStrictMode )
			{
				// Подготовка сообщения об ошибке.
				aTextString error_msg;
				if( nlrcSuccess != error_msg.assign( Label ) ||
					nlrcSuccess != error_msg.append( " (метка с таким названием впервые встречена в шаблоне ( Id: " ) ||
					nlrcSuccess != error_msg.append( vInstrLablePatternNames[*id_num_p] ) ||
					nlrcSuccess != error_msg.append( " ) )" ) ||
					!vErrors.push_back( { DLErrorId::peInstrLabelIsNotUniq, 0 }, error_msg, vObjectId ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				return INF_ENGINE_ERROR_CANT_PARSE;
			}
			else
				// Сброс повторно использованной метки.
				Label.clear();
		}
		else
		{
			// Регистрация новой метки.
			unsigned int lbl_id;
			NanoLib::NameIndex::ReturnCode rt = vInstrLableTable.AddName( Label.ToConstChar(), lbl_id );
			if( rt != NanoLib::NameIndex::rcSuccess )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Code: %i when adding \"%s\"", rt, Label.ToConstChar() );

			// Сохранение названия шаблона, в котором была зарегистрирована данная метка.
			char * pattern_name = nAllocateObjects( vAllocator, char, vObjectId.size() + 1 );
			if( !pattern_name )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			memcpy( pattern_name, vObjectId.ToConstChar(), vObjectId.size() );
			pattern_name[vObjectId.size()] = '\0';
			vInstrLablePatternNames.push_back( pattern_name );
			if( vInstrLablePatternNames.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
	}

	aResult.SetLabel( PatternLabel, PatternLabelLength );

	aResult.SetInfPerson( infperson_id );

	// Выделение памяти под условные операторы уровня шаблона.
	if( vTLCS.size() )
	{
		nAllocateNewObjects( vTmpAllocator, InfPatternItems::TLCS_WR*, vTLCS.size(), tlcs );
		if( !tlcs )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		// Копирование ссылок на условные операторы уровня шаблона.
		memcpy( tlcs, vTLCS.get_buffer(), vTLCS.size() * sizeof( InfPatternItems::TLCS_WR* ) );
		// Проставление в шаблоне ссылок на условные операторы уровня шаблона.
		aResult.SetTLCS( tlcs, vTLCS.size() );
	}


	if( vConditions.size() )
	{
		// Выделение памяти под шаблон-условия.
		nAllocateNewObjects( vTmpAllocator, InfPatternItems::ConditionRW*, vConditions.size(), conds );
		if( !conds )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		// Копирование ссылок на шаблон-условия.
		memcpy( conds, vConditions.get_buffer(), vConditions.size() * sizeof( InfPatternItems::ConditionRW* ) );

		// Проставление ссылок на шаблон-условия в шаблоне.
		aResult.SetConditions( conds, vConditions.size() );
	}

	// Проверка на корректность шаблона.
	if( !vQuestions.size() && !aResult.CheckEvents() )
	{
		if( !vErrors.push_back( { DLErrorId::pePatternWithoutQuestions, 0 }, {}, vObjectId ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		return INF_ENGINE_ERROR_CANT_PARSE;
	}
	else if( vQuestions.size() && aResult.CheckEvents() )
	{
		if( !vErrors.push_back( { DLErrorId::peQuestionsAndEvents, 0 }, {}, vObjectId ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		return INF_ENGINE_ERROR_CANT_PARSE;
	}

	// Выделение памяти под шаблон-вопросы.
	if( vQuestions.size() )
	{
		nAllocateNewObjects( vTmpAllocator, InfPatternItems::QuestionWR*, vQuestions.size(), qsts );
		if( !qsts )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		// Копирование ссылок на шаблон-вопросы.
		memcpy( qsts, vQuestions.get_buffer(), vQuestions.size() * sizeof( InfPatternItems::QuestionWR* ) );

		// Проставление ссылок на шаблон-вопросы в шаблоне.
		aResult.SetQuestions( qsts, vQuestions.size() );
	}
	else
		aResult.SetQuestions( nullptr, 0 );

	// Проставление ссылки на шаблон-that в шаблоне.
	if( vThat )
		aResult.SetThat( vThat );

	// Проверка на корректность шаблона.
	if( !vAnswers.size() )
	{
		if( !vErrors.push_back( { DLErrorId::pePatternWithoutAnswers, 0 }, vString, vObjectId ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		return INF_ENGINE_ERROR_CANT_PARSE;
	}
	// Выделение памяти под шаблон-ответы.
	nAllocateNewObjects( vTmpAllocator, InfPatternItems::AnswerRW, vAnswers.size(), anses );
	if( !anses )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	// Копирование шаблон-ответов.
	for( unsigned int i = 0; i < vAnswers.size(); i++ )
		new( (nMemoryAllocatorElementPtr*)( anses + i ) ) InfPatternItems::AnswerRW( vAnswers[i] );

	// Выделение памяти под элементы, требующие разбора всех шаблонов для своей компиляции.
	unsigned int wfr_count = 0;
	InfPatternItems::Base ** wfr = nullptr;
	for( unsigned int n = 0; n < vWaitingForResolving.size(); ++n )
		wfr_count += vWaitingForResolving[n]->GetItemsNumber();
	if( wfr_count )
	{
		wfr = nAllocateObjects( vTmpAllocator, InfPatternItems::Base*, wfr_count );
		if( !wfr )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		unsigned int cnt = 0;
		for( unsigned int n = 0; n < vWaitingForResolving.size(); ++n )
			for( unsigned int item_n = 0; item_n < vWaitingForResolving[n]->GetItemsNumber(); ++item_n )
				wfr[cnt++] =  vWaitingForResolving[n]->GetItem( item_n );
	}

	// Проставление ссылок на шаблон-вопросы в шаблоне.
	if( INF_ENGINE_SUCCESS != ( iee = aResult.SetAnswers( anses, vAnswers.size(), wfr, wfr_count ) ) )
	{
		if( INF_ENGINE_ERROR_INV_ARGS == iee )
			ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
		else if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		else
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	}

	// Выделение памяти под идентификатор шаблона.
	char * id = nAllocateObjects( vTmpAllocator, char, vObjectId.size() + 1 );
	if( !id )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	if( vObjectId.size() )
		memcpy( id, vObjectId.ToConstChar(), vObjectId.size() );
	id[vObjectId.size()] = '\0';

	// Установка идентификатора шаблона.
	aResult.SetPatternId( id, vObjectId.size() );

	// Установка метки шаблона.
	if( !Label.empty() )
	{
		char * label = nAllocateObjects( vTmpAllocator, char, Label.size() + 1 );
		if( !label )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( label, Label.ToConstChar(), Label.size() );
		label[Label.size()] = '\0';
		aResult.SetInstrLabel( label, Label.size() );
	}

	return INF_ENGINE_SUCCESS;
}

void DLParser::Reset( bool full )
{
	if( full )
	{
		vErrors.clear();
		vWarnings.clear();
		vSuccessCounter = 0;
		vFailCounter = 0;
	}
	vConditions.clear();
	vQuestions.clear();
	vThat = nullptr;
	vAnswers.clear();
	vWaitingForResolving.clear();
	vTmpAllocator.Reset();
	vDictInstructions.clear();
	vTLCS.clear();
}

InfEngineErrors DLParser::AddIdLableToTable( const char * aIdLable, unsigned int aId )
{
	const unsigned int * id_num_p = vTablePatternsId.Search( aIdLable );
	if( id_num_p )
	{
		if( *id_num_p < vTableIndexId.size() )
		{
			if( vTableIndexId[*id_num_p] != static_cast<unsigned int>( -1 ) )
				return INF_ENGINE_WARN_UNSUCCESS;
			else
				vTableIndexId[*id_num_p] = aId;
		}
		else
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	}
	else
	{
		unsigned int id_num = 0;

		NanoLib::NameIndex::ReturnCode rt = vTablePatternsId.AddName( aIdLable, id_num );
		if( rt != NanoLib::NameIndex::rcSuccess && rt != NanoLib::NameIndex::rcElementExists )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Code: %i when adding \"%s\"", rt, aIdLable );
		if( vTableIndexId.size() != id_num )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		vTableIndexId.push_back( aId );
		if( vTableIndexId.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		if( vExtendAnswerErrors.size() != id_num )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		try
		{
			vExtendAnswerErrors.push_back( {} );
		}
		catch( ... )
		{
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLParser::ExtractErrors( Errors & aErrors )
{
	for( auto & err : vErrors )
		if( !aErrors.push_back( std::move( err ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	vErrors.clear();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLParser::ExtractWarnings( Errors & aWarnings )
{
	for( auto & err : vWarnings )
		if( !aWarnings.push_back( std::move( err ) ) )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	vWarnings.clear();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLParser::ExtractEW( Errors & aErrors, Errors & aWarnings )
{
	TryWithTrace( ExtractErrors( aErrors ), INF_ENGINE_SUCCESS );
	TryWithTrace( ExtractWarnings( aWarnings ), INF_ENGINE_SUCCESS );
	return INF_ENGINE_SUCCESS;
}


InfEngineErrors DLParser::ExportErrors( const DLStringParser & aParser )
{
	if( !vErrors.push_back( aParser.GetErrorsDescriptions(), vString, vObjectId ) ||
		!vWarnings.push_back( aParser.GetWarningsDescriptions(), vString, vObjectId ) )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	else
		return INF_ENGINE_SUCCESS;
}
