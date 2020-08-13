#include "AliasParser.hpp"

#include <cerrno>

#include <NanoLib/TextFuncs.hpp>

#include "Alias.hpp"

#define ReturnWithParsingError( aErrorDescription, aPos, aAliasName, aAliasNameLength ) \
	do { \
		InfEngineErrors iee = AddParsingError( aErrors, aErrorDescription, aPos, aAlias, aAliasLength, aAliasName, aAliasNameLength ); \
		if( iee != INF_ENGINE_SUCCESS ) \
			ReturnWithTrace( iee ); \
		return INF_ENGINE_ERROR_CANT_PARSE; \
	} while( false )


AliasParser::AliasParser( DLDataWR & aDLDataWR, nMemoryAllocator & aMemoryAllocator ) :
	vDLDataWR( aDLDataWR ), vMemoryAllocator( aMemoryAllocator )
{}

InfEngineErrors AliasParser::ReadAliasesFromFile( const char * aFilePath, bool aStrictMode, unsigned int &aSuccessAliases, unsigned int &aFaultAliases, Errors & aErrors )
{
	// Проверка параметров.
	if( !aFilePath )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Открываем файл на чтение.
	FILE* sourceFH = fopen( aFilePath, "r" );
	if( !sourceFH )
		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open file %s: %s", aFilePath, strerror( errno ) );

	// Пропускаем все пустые строки.
	unsigned int string = 0;
	aSuccessAliases = 0;
	aFaultAliases = 0;

	nlReturnCode nle;
	InfEngineErrors iee;
	aTextString vTmp;
	aTextString vBuffer;

	while( ( nle = vTmp.ReadString( sourceFH ) ) == nlrcSuccess )
	{
		string++;

		// Нормализация строки.
		AllTrim( vTmp );

		// Пропуск пустых строк.
		if( vTmp.empty() )
		{
			// Если в буфере есть код алиаса.
			if( !vBuffer.empty() )
			{
				iee = Parse( vBuffer.ToConstChar(), vBuffer.size(), aStrictMode, aErrors );
				if( iee == INF_ENGINE_SUCCESS )
				{
					++aSuccessAliases;
				}
				else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					++aFaultAliases;
				else if( iee != INF_ENGINE_SUCCESS )
				{
					fclose( sourceFH );
					ReturnWithTrace( iee );
				}
			}

			// Очистка буфеера.
			vBuffer.clear();
			continue;
		}

		// Пропуск комментариев.
		if( vTmp.size()> 2 && vTmp[0] == '/' && vTmp[1] == '/' )
			continue;

		nle = vBuffer.append( " " );
		if( nle == nlrcErrorNoFreeMemory )
		{
			fclose( sourceFH );
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
		nle = vBuffer.append( vTmp );
		if( nle == nlrcErrorNoFreeMemory )
		{
			fclose( sourceFH );
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
		else if( nle != nlrcSuccess )
		{
			fclose( sourceFH );
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Failed to read alias" );
		}
	}

	fclose( sourceFH );

	// Если в буфере есть код алиаса.
	if( !vBuffer.empty() )
	{
		iee = Parse( vBuffer.ToConstChar(), vBuffer.size(), aStrictMode, aErrors );
		if( iee == INF_ENGINE_SUCCESS )
		{
			++aSuccessAliases;
		}
		else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			++aFaultAliases;
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	// Очистка буфеера.
	vBuffer.clear();

	if( aFaultAliases )
		return INF_ENGINE_ERROR_CANT_PARSE;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors AliasParser::Parse( const char * aAlias, unsigned int aAliasLength, bool aStrictMode, Errors & aErrors )
{
	InfEngineErrors iee;
	unsigned int pos = 0;

	Alias * NewAlias = nAllocateObject( vMemoryAllocator, Alias );
	if( !NewAlias )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr*)NewAlias)Alias;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
	if( pos >= aAliasLength )
		return INF_ENGINE_ERROR_CANT_PARSE;

	// Определение типа возвращаемого алисом значения.
	enum {ALIAS_TYPE_UNKNOWN = -1u, ALIAS_TYPE_TEXT = 0, ALIAS_TYPE_BOOL = 1};
	static const char * Types[2] = {"TEXT", "BOOL"};
	static const unsigned int TypeLengths[2] = {4, 4};
	bool MatchedTypes[2];

	unsigned int AliasTypeNum = TextFuncs::ParseWord( aAlias + pos, aAliasLength - pos, Types, TypeLengths, 2, MatchedTypes );
	switch( AliasTypeNum )
	{
	case ALIAS_TYPE_UNKNOWN:
		ReturnWithParsingError( DLErrorId::peParseUnknownAliasResultType, pos, nullptr, 0 );
		break;

	case ALIAS_TYPE_TEXT:
		NewAlias->SetType( Alias::TEXT );
		break;

	case ALIAS_TYPE_BOOL:
		NewAlias->SetType( Alias::BOOL );
		break;

	default:
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Fatal error: parsing alias result type" );
	}
	pos += TypeLengths[AliasTypeNum];

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
	if( pos >= aAliasLength )
		return INF_ENGINE_ERROR_CANT_PARSE;

	// Выделение имени алиаса.
	unsigned int NameEnd = ParseAliasName( aAlias, aAliasLength, pos );
	if( NameEnd == pos )
		ReturnWithParsingError( DLErrorId::peParseAliasNameExpected, pos, nullptr, 0 );

	// Копирование имени алиаса.
	char * Buffer = nAllocateObjects( vMemoryAllocator, char, NameEnd - pos + 1 );
	if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( Buffer, aAlias + pos, NameEnd - pos );
	Buffer[NameEnd - pos] = '\0';
	// Приведение имени алиаса к нижнему регистру.
	TextFuncs::ToLower( Buffer, NameEnd - pos );
	iee = NewAlias->SetName( Buffer, NameEnd - pos );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( iee, "Failed to set alias name" );
	pos = NameEnd;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
	if( pos >= aAliasLength )
		return INF_ENGINE_ERROR_CANT_PARSE;

	// Проверка начала списка аргументов.
	if( aAlias[pos] != '(' )
	{
		ReturnWithParsingError( DLErrorId::peAliasOpenRoundBrace, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		return INF_ENGINE_ERROR_CANT_PARSE;
	}
	++pos;

	// Разбор аргументов алиаса.
	enum {ARG_TYPE_UNKNOWN = -1u, ARG_TYPE_TEXT = 0, ARG_TYPE_DYNAMIC = 1, ARG_TYPE_EXTENDED = 2, ARG_TYPE_ANY = 3, ARG_TYPE_VARIABLE = 4 };
	static const char * ArgTypes[5] = { "TEXT", "DYNAMIC", "EXTENDED", "ANY", "VARIABLE" };
	static const unsigned int ArgTypeLengths[5] = { 4, 7, 8, 3, 8 };
	bool MatchedArgTypes[5];
	unsigned int ArgTypeNum;
	DLFunctionArgType ArgType;

	while( pos < aAliasLength && aAlias[pos] != ')' )
	{
		// Имя аргумента алиаса.
		char * NameBuffer = nullptr;
		// Имя значения аргумента по умолчанию.
		char * DefaultValue = nullptr;
		// Длина имени значения аргумента по умолчанию.
		unsigned char DefaultValueLen = 0;
		// Список допустимых значений аргумента (только для аргументов типа TEXT).
		avector<char *> ValidValues;

		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
		if( pos >= aAliasLength )
			return INF_ENGINE_ERROR_CANT_PARSE;

		// Если не конец списка аргументов алиаса.
		if( aAlias[pos] != ')' )
		{
			// Если считываемый аргумент не первый.
			if( NewAlias->GetArgumentsCount() )
			{
				// Прверяем наличие запятой, разделяющей описания аргументов алиаса.
				if( aAlias[pos] != ',' )
					ReturnWithParsingError( DLErrorId::peParseUnknownAliasArgType, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );
				++pos;

				// Пропуск пробелов.
				pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
				if( pos >= aAliasLength )
					return INF_ENGINE_ERROR_CANT_PARSE;
			}

			ArgTypeNum = TextFuncs::ParseWord( aAlias + pos, aAliasLength - pos, ArgTypes, ArgTypeLengths, 5, MatchedArgTypes );
			switch( ArgTypeNum )
			{
			case ARG_TYPE_UNKNOWN:
				ReturnWithParsingError( DLErrorId::peAliasCommaExpexted, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );
				break;

			case ARG_TYPE_TEXT:
				ArgType = DLFAT_TEXT;
				break;

			case ARG_TYPE_DYNAMIC:
				ArgType = DLFAT_DYNAMIC;
				break;

			case ARG_TYPE_EXTENDED:
				ArgType = DLFAT_EXTENDED;
				break;

			case ARG_TYPE_ANY:
				ArgType = DLFAT_ANY;
				break;

			case ARG_TYPE_VARIABLE:
				ArgType = DLFAT_VARIABLE;
				break;

			default:
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Fatal error: parsing alias argument type" );
			}


			pos += ArgTypeLengths[ArgTypeNum];

			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
			if( pos >= aAliasLength )
				return INF_ENGINE_ERROR_CANT_PARSE;

			// Выделение имени аргумента.
			NameEnd = ParseAliasName( aAlias, aAliasLength, pos );
			if( NameEnd == pos )
				ReturnWithParsingError( DLErrorId::peParseInvalidAliasName, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );

			// Копирование имени аргумента.
			NameBuffer = nAllocateObjects( vMemoryAllocator, char, NameEnd - pos + 1 );
			if( !NameBuffer )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			memcpy( NameBuffer, aAlias + pos, NameEnd - pos );
			NameBuffer[NameEnd - pos] = '\0';
			pos = NameEnd;

			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
			if( pos >= aAliasLength )
				return INF_ENGINE_ERROR_CANT_PARSE;

			// Если аргумент имеет тип TEXT, проверяем на наличие предопределённых значений.
			if( ArgType == DLFAT_TEXT && aAlias[pos] == '{' )
			{
				++pos;

				// Пропуск пробелов.
				pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
				if( pos >= aAliasLength )
					return INF_ENGINE_ERROR_CANT_PARSE;

				// Разбор предопределённых значений.
				while( pos < aAliasLength && aAlias[pos] != '}' )
				{
					char * ValueBuffer;

					// Если текущее выделяемое значение не первое.
					if( ValidValues.size()  )
					{
						// Пропуск пробелов.
						pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
						if( pos >= aAliasLength )
							return INF_ENGINE_ERROR_CANT_PARSE;

						// Проверка наличия запятой, разделяющей описания допустимых значений.
						if( aAlias[pos] != ',' )
						{
							ReturnWithParsingError( DLErrorId::peAliasCommaExpexted, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );
						}
						++pos;
					}

					// Выделяем очередного значения.
					iee = ParseText( aAlias, aAliasLength, pos, ValueBuffer );
					if( iee == INF_ENGINE_ERROR_NOFREE_MEMORY )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
						ReturnWithParsingError( DLErrorId::peAliasInvalidValue, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );
					else if( iee != INF_ENGINE_SUCCESS )
						ReturnWithError( iee, "Failed to parse alias argument valid value" );

					// Добавляем значение в список значений.
					ValidValues.push_back( ValueBuffer );
					if( ValidValues.no_memory() )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

					// Пропуск пробелов.
					pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
					if( pos >= aAliasLength )
						return INF_ENGINE_ERROR_CANT_PARSE;

				}
				++pos;

				// Если список допустимых значений пуст.
				if( !ValidValues.size() )
					ReturnWithParsingError( DLErrorId::peAliasCommaExpexted, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );
			}
		}

		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
		if( pos >= aAliasLength )
			return INF_ENGINE_ERROR_CANT_PARSE;

		// Если для аргумента указано значение по усолчанию.
		if( aAlias[pos] == '=' )
		{
			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aAlias, pos + 1, aAliasLength );
			if( pos >= aAliasLength )
				return INF_ENGINE_ERROR_CANT_PARSE;

			// Проверка начала значения по умолчанию.
			if( aAlias[pos] != '"' )
				return INF_ENGINE_ERROR_CANT_PARSE;
			unsigned int begin = pos + 1;

			// Поиск окончания значения по умолчанию.
			++pos;
			bool escaped = false;
			while( pos < aAliasLength )
			{
				if( !escaped )
				{
					if( aAlias[pos] == '"' )
						break;
					if( aAlias[pos] == '\\' )
						escaped = true;
				}
				else
					escaped = false;

				++pos;
			}
			if( pos >= aAliasLength )
				return INF_ENGINE_ERROR_CANT_PARSE;

			// Копирование значения по умолчанию.
			DefaultValueLen = pos - begin;
			DefaultValue = nAllocateObjects( vMemoryAllocator, char, DefaultValueLen + 1 );
			if( !DefaultValue )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			memcpy( DefaultValue, aAlias + begin, DefaultValueLen );
			DefaultValue[DefaultValueLen] = '\0';

			++pos;
		}

		// Если для текущего элемента определён список допустимых значений.
		if( ValidValues.size() )
		{
			// Копируем список допустимых значений.
			const char ** ValidValuesBuf = nAllocateObjects( vMemoryAllocator, const char *, ValidValues.size() );
			if( !ValidValuesBuf )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			memcpy( ValidValuesBuf, ValidValues.get_buffer(), sizeof(const char *) * ValidValues.size() );

			NewAlias->RegisterArgument( NameBuffer, ArgType, ValidValuesBuf, ValidValues.size(), DefaultValue, DefaultValueLen );
		}
		else NewAlias->RegisterArgument( NameBuffer, ArgType, nullptr, 0, DefaultValue, DefaultValueLen );

	}
	++pos;

	// Начало очередного фрагмента тела алиаса.
	unsigned int Begin = pos;
	// Рабираем тело алиаса.
	while( pos < aAliasLength )
	{
		// Пропускаем экранированные символы.
		if( aAlias[pos] == '\\' )
		{
			pos += 2;
			continue;
		}
		else
		{
			// Если в теле алиаса встречена ссылка на аргумент алиаса.
			if( aAlias[pos] == '[' && pos + 1 < aAliasLength && aAlias[pos + 1] == '$' )
			{
				// Если ссылке на аргумент предшествовала текстовая часть.
				if( Begin < pos )
				{

					// Копируем фрагмент текста.
					char * TextBuffer = nAllocateObjects( vMemoryAllocator, char, pos - Begin + 1 );
					if( !TextBuffer )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					memcpy( TextBuffer, aAlias + Begin, pos - Begin );
					TextBuffer[pos - Begin] = '\0';

					// Добавляем фрагмент текста к телу алиаса.
					iee = NewAlias->AddTextItem( TextBuffer, pos - Begin );
					if( iee == INF_ENGINE_ERROR_NOFREE_MEMORY )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					if( iee != INF_ENGINE_SUCCESS )
						ReturnWithError( iee, "Failed to add text item to alias body" );

					Begin = pos;
				}
				pos += 2;

				// Пропуск пробелов.
				pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
				if( pos >= aAliasLength )
					return INF_ENGINE_ERROR_CANT_PARSE;

				// Выделяем имя аргумента.
				pos = ParseAliasName( aAlias, aAliasLength, pos );
				if( Begin == pos )
					ReturnWithParsingError( DLErrorId::peParseInvalidAliasArgName, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );

				// Пропуск пробелов.
				pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
				if( pos >= aAliasLength )
					return INF_ENGINE_ERROR_CANT_PARSE;

				// Проверка на закрывающую скобку в конце ссылки на аргумент алиаса.
				if( aAlias[pos] != ']' )
					ReturnWithParsingError( DLErrorId::peParseInvalidAliasArgName, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );

				// Добавляем ссылку на аргумент к телу алиаса.
				iee = NewAlias->AddVariableItem( aAlias + Begin + 2, pos - (Begin + 2) );
				if( iee == INF_ENGINE_ERROR_NOFREE_MEMORY )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				if( iee == INF_ENGINE_ERROR_INVALID_ARG )
					ReturnWithParsingError( DLErrorId::peParseUnknownAliasArgName, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithError( iee, "Failed to add argument item to alias body" );

				++pos;

				Begin = pos;
			}
			// Если в теле алиаса встречено использоване функции или алиаса.
			else if( aAlias[pos] == '[' && pos + 1 < aAliasLength && aAlias[pos + 1] == '@' )
			{
				pos += 2;
				// Ищем конец имени функции/алиаса.
				const char * NameEndPtr = strstr( aAlias + pos, "(" );

				// Если конец имени не найден - ошибка.
				if( !NameEndPtr )
					ReturnWithParsingError( DLErrorId::peFuncOrAliasInvalidName, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );

				// Выделяем имя функции/алиаса.
				aTextString FuncName;
				nlReturnCode nlrt = FuncName.assign( aAlias + pos, NameEndPtr - aAlias - pos );
				if( nlrt == nlrcErrorNoFreeMemory )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				// Удаляем лишние пробелы из имени функции/алиаса.
				AllTrim( FuncName );

				// Приведение имени к нижнему регистру.
				TextFuncs::ToLower( FuncName.ToChar(), FuncName.size() );

				// Поиск функции с таким именем в реестре функций.
				unsigned int FuncId;
				iee = vDLDataWR.GetFunctionsRegistry().Search( FuncName.ToConstChar(), FuncName.size(), FuncId );

				// Если такая функция найдена - парсим дальше.
				if( iee == INF_ENGINE_SUCCESS )
					continue;

				// Если такая функция не найдена.
				if( iee == INF_ENGINE_WARN_UNSUCCESS )
				{
					// Ищем алиас с таким имененм.
					if( !vDLDataWR.GetAliasRegistry().GetAlias( FuncName.ToConstChar(), FuncName.size() ) )
						ReturnWithParsingError( DLErrorId::peFuncOrAliasUnknownName, pos, NewAlias->GetName(), strlen( NewAlias->GetName() ) );
				}
				else
					ReturnWithTrace( iee );
			}
			else
				++pos;
		}
	}

	// Если ссылке на аргумент предшествовала текстовая часть.
	if( Begin < pos )
	{
		// Копируем фрагмент текста.
		char * TextBuffer = nAllocateObjects( vMemoryAllocator, char, pos - Begin + 1 );
		if( !TextBuffer )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( TextBuffer, aAlias + Begin, pos - Begin );
		TextBuffer[pos - Begin] = '\0';

		// Добавляем фрагмент текста к телу алиаса.
		iee = NewAlias->AddTextItem( TextBuffer, pos - Begin );
		if( iee == INF_ENGINE_ERROR_NOFREE_MEMORY )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithError( iee, "Failed to add text item to alias body" );
	}

	// Регистрация разобранного алиаса в реестре алиасов.
	iee = vDLDataWR.RegisterAlias( NewAlias );

	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors AliasParser::ParseText( const char * aAlias, unsigned int aAliasLength, unsigned int & aStartPos, char *& aText )
{
	unsigned int pos = aStartPos;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aAlias, pos, aAliasLength );
	if( pos >= aAliasLength )
		return INF_ENGINE_ERROR_CANT_PARSE;

	// Проверка наличия открывающей кавычки.
	if( aAlias[pos] != '\"' )
		return INF_ENGINE_ERROR_CANT_PARSE;
	++pos;

	// Поиск закрывающей кавычки.
	unsigned int End = pos;
	while( End < aAliasLength && aAlias[End] != '\"' )
		End += aAlias[End] == '\\' ? 2 : 1;
	if( End >= aAliasLength )
		return INF_ENGINE_ERROR_CANT_PARSE;

	// Копирование выделенного текста.
	aText = nAllocateObjects( vMemoryAllocator, char, End - pos + 1 );
	if( !aText )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	memcpy( aText, aAlias + pos, End - pos );
	aText[End - pos] = '\0';

	aStartPos = End + 1;

	return INF_ENGINE_SUCCESS;

}

InfEngineErrors AliasParser::AddParsingError( Errors & aErrors, DLErrorId aErrorId, unsigned int aErrorPos, const char * aString, unsigned int aStringLength, const char * aAliasName, unsigned int aAliasNameLength )
{
	char * String = nAllocateObjects( vMemoryAllocator, char, aStringLength + 1 );
	if( !String )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	char * AliasName = nAllocateObjects( vMemoryAllocator, char, aAliasNameLength + 1 );
	if( !AliasName )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	Error * Err = aErrors.grow();
	if( !Err )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	char * StrCopy = nAllocateObjects( vMemoryAllocator, char, aStringLength + 1 );
	if( !StrCopy )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	strcpy( String, aString );
	strcpy( AliasName, aAliasName );
	strncpy( StrCopy, aString, aStringLength );
	StrCopy[aStringLength] = '\0';

	Err->assign( aErrorId, aErrorPos );
	Err->vString = StrCopy;
	Err->vStringLength = aStringLength;
	Err->vAliasName = AliasName;
	Err->vAliasNameLength = aAliasNameLength;

	return INF_ENGINE_SUCCESS;
}

unsigned int AliasParser::ParseAliasName( const char * aAlias, unsigned int aAliasLength, unsigned int aStartPos ) const
{
	// Имя алиаса может состоять из символов латинского алфавита, цифр, символа подчёркивания и знаков арифметических операций.
	while( aStartPos < aAliasLength && ( TextFuncs::IsAlNum( aAlias[aStartPos] ) || aAlias[aStartPos] == '_' ||
			aAlias[aStartPos] == '*' || aAlias[aStartPos] == '/' || aAlias[aStartPos] == '+' || aAlias[aStartPos] == '-' || aAlias[aStartPos] == '%'  ) )
		++aStartPos;

	return aStartPos;
}
