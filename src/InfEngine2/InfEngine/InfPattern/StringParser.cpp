#include "StringParser.hpp"

#include "Items/TagExtendAnswer.hpp"
#include "Items/TagPre.hpp"

using namespace InfPatternItems;

char * CopyString( const char * aString, unsigned int aLength, nMemoryAllocator & vMemoryAllocator )
{
	auto res = new( vMemoryAllocator ) char[aLength + 1];
	if( !res )
		return nullptr;
	memcpy( res, aString, aLength );
	res[aLength] = '\0';
	return res;
}

static unsigned int GetDictName( const char * aBuffer, unsigned int aBufferLength, unsigned int aStartPos )
{
	// Проверка аргументов.
	if( !aBuffer )
		return aStartPos;

	// Поиск первого стоп символа.
	unsigned int res = aStartPos;
	unsigned int last_pos = res;
	UChar32 ch;
	while( res < aBufferLength )
	{
		last_pos = res;
		U8_NEXT_UNSAFE( aBuffer, res, ch );
		if( !TextFuncs::IsAlNumUTF8( ch ) && ch != '_' && ch != '-' && ch != '+' )
			break;
	}

	return last_pos;
}

static unsigned int GetVarName( const char * aBuffer, unsigned int aBufferLen, unsigned int aStartPos )
{
	// Проверка аргументов.
	if( !aBuffer )
		return aStartPos;

	// Поиск первого стоп символа.
	unsigned int res = aStartPos;
	for(; res < aBufferLen && ( ( 'a' <= aBuffer[res] && aBuffer[res] <= 'z' ) ||
								( 'A' <= aBuffer[res] && aBuffer[res] <= 'Z' ) ||
								( '0' <= aBuffer[res] && aBuffer[res] <= '9' ) ||
								aBuffer[res] == '_' || aBuffer[res] == '-' || aBuffer[res] == '+' || aBuffer[res] == '.' ); res++ );

	return res;
}

static unsigned int GetText( const char * aBuffer, unsigned int aBufferLen, unsigned int aStartPos,
							 char aBeginDelimiter, char aEndDelimiter )
{
	// Проверка аргументов.
	if( !aBuffer )
		return aStartPos;

	unsigned int pos = aStartPos;

	// Поиск начального ограничителя.
	if( aBufferLen <= aStartPos || aBuffer[aStartPos] != aBeginDelimiter )
		return aStartPos;

	pos++;
	// Поиск завершающего ограничителя.
	for(; pos < aBufferLen; pos++ )
	{
		// Найден конец текстовой строки.
		if( aBuffer[pos] == aEndDelimiter )
			return pos + 1;

		// Специальные символы.
		if( aBuffer[pos] == '\\' )
		{
			if( pos + 1 >= aBufferLen )
				return aStartPos;
			else if( aBuffer[pos + 1] == '\\' || aBuffer[pos + 1] == aEndDelimiter )
				pos++;
			else
				return aStartPos;
		}
	}

	return aStartPos;
}

#define AddParseWarning( aWarning, aPos )  \
	do \
	{ \
		if( !vWarningDescriptions.push_back( { ( aWarning ), ( aPos ) } ) ) \
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY ); \
	} while( 0 )


#define AddParseError( aError, aPos )  \
	do \
	{ \
		if( !vErrorDescriptions.push_back( DLStringParser::Error{ aError, aPos } ) ) \
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY ); \
	} while( 0 )

#define ReturnWithParseWarning( aWarning, aPos ) \
	do \
	{ \
		AddParseWarning( aWarning, aPos ); \
								\
		return INF_ENGINE_ERROR_CANT_PARSE; \
	} while( 0 )

#define ReturnWithParseError( aError, aPos ) \
	do \
	{ \
		AddParseError( aError, aPos ); \
								\
		return INF_ENGINE_ERROR_CANT_PARSE; \
	} while( 0 )

DLStringParser::DLStringParser( DLDataWR & aDLDataWR,
								NanoLib::NameIndex & aTablePatternsId, avector<unsigned int> & aTableIndexId,
								ErrorReports & aExtendAnswerErrors ) :
	vDLDataWR( aDLDataWR ),
	vTablePatternsId( aTablePatternsId ),
	vTableIndexId( aTableIndexId ),
	vExtendAnswerErrors( aExtendAnswerErrors )
{}

InfEngineErrors DLStringParser::ParseInstrLabel( const char * aString, unsigned int aStringLength, aTextString & aLabel )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, 0, aStringLength );

	// Проверка ключевого слова Label.
	if( pos + 5 > aStringLength || strncasecmp( aString + pos, "Label", 5 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Проверка пробела после ключевого слова Label.
	pos += 5;
	if( !TextFuncs::IsSpace( aString[pos] ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );

	// Чтение метки.
	unsigned int begin = pos;
	while( pos < aStringLength && !TextFuncs::IsSpace( aString[pos] ) )
		++pos;

	// Проверка длины метки.
	if( pos == begin )
		ReturnWithParseError( DLErrorId::peInstrLabelIsEmpty, pos );

	// Проверка окончания строки после метки.
	unsigned int end = pos;
	if( pos < aStringLength )
	{
		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
		if( pos != aStringLength )
			ReturnWithParseError( DLErrorId::peInstrLabelIncorrectName, pos );
	}

	// Подготовка результата.
	nlReturnCode nlrc = aLabel.assign( aString + begin, end - begin );
	if( nlrcSuccess != nlrc  )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTLCS_If( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, 0, aStringLength );

	// Проверка ключевого слова if
	if( pos + 2 > aStringLength || strncasecmp( aString + pos, "if", 2 ) )
		return INF_ENGINE_WARN_UNSUCCESS;
	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 2, aStringLength );

	// Проверка на начало условия.
	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peIfOpenRoundBrace, pos );

	// Разбор условия.
	InfPatternItems::TagFunction * Condition = nullptr;
	bool ArgIsMultiple = false;
	bool IsFunctionArg = false;
	auto iee = ParseCondition( aString, aStringLength, pos, Condition, aAllocator, ArgIsMultiple, IsFunctionArg );
	if( iee != INF_ENGINE_SUCCESS )
		return iee;

	// Проверка на окончание условия.
	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peIfCloseRoundBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка окончания строки с условием уровня шаблона.
	if( pos != aStringLength )
		ReturnWithParseError( DLErrorId::peTLCS_EOLExpected, pos );

	// Добавление нового УОУШ в список УОУШ шаблона.
	auto cond_ptr = new( aAllocator ) InfPatternItems::Base *;
	if( !cond_ptr )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	cond_ptr[0] = Condition;

	auto tlcs = new( aAllocator ) TLCS_WR( cond_ptr, TLCS_WR::Type::If );
	if( !tlcs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aTLCS.push_back( tlcs );
	if( aTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Добавление нового УОУШ в стек действующих в текущий момент.
	vTLCS_stack.vTLCS.push_back( TLCS_Node( aTLCS.size() - 1, tlcs->GetTLCSType() ) );
	if( vTLCS_stack.vTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTLCS_Elsif( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, 0, aStringLength );

	// Проверка ключевого слова elsif
	if( pos + 5 > aStringLength || strncasecmp( aString + pos, "elsif", 5 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Проверка, предшествовал ли данному Elsif какой-нибудь If или Elsif.
	if( !vTLCS_stack.vTLCS.size() || ( vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::If && vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::Elsif ) )
		ReturnWithParseError( DLErrorId::peTLCS_ElsifWithoutIf, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 5, aStringLength );

	// Проверка на начало условия.
	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peIfOpenRoundBrace, pos );

	// Разбор условия.
	InfPatternItems::TagFunction * Condition = nullptr;
	bool ArgIsMultiple = false;
	bool IsFunctionArg = false;
	auto iee = ParseCondition( aString, aStringLength, pos, Condition, aAllocator, ArgIsMultiple, IsFunctionArg );
	if( iee != INF_ENGINE_SUCCESS )
		return iee;

	// Проверка на окончание условия.
	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peIfCloseRoundBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка окончания строки с условием уровня шаблона.
	if( pos != aStringLength )
		ReturnWithParseError( DLErrorId::peTLCS_EOLExpected, pos );

	// Добавление нового УОУШ в список УОУШ шаблона.
	auto cond_ptr = new( aAllocator ) InfPatternItems::Base *;
	if( !cond_ptr )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	cond_ptr[0] = Condition;

	auto tlcs = new( aAllocator ) TLCS_WR( cond_ptr, TLCS_WR::Type::Elsif );
	if( !tlcs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aTLCS.push_back( tlcs );
	if( aTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Добавление нового УОУШ в стек действующих в текущий момент.
	if( !vTLCS_stack.vTLCS.size() )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	vTLCS_stack.vTLCS.pop_back();
	vTLCS_stack.vTLCS.push_back( TLCS_Node( aTLCS.size() - 1, tlcs->GetTLCSType() ) );
	if( vTLCS_stack.vTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTLCS_Else( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, 0, aStringLength );

	// Проверка ключевого слова else
	if( pos + 4 > aStringLength || strncasecmp( aString + pos, "else", 4 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Проверка, предшествовал ли данному Else какой-нибудь If или Elsif.
	if( !vTLCS_stack.vTLCS.size() ||
		(
			vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::If &&
			vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::Elsif
		) )
		ReturnWithParseError( DLErrorId::peTLCS_ElseWithoutIf, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 4, aStringLength );

	// Проверка окончания строки с условием уровня шаблона.
	if( pos != aStringLength )
		ReturnWithParseError( DLErrorId::peTLCS_EOLExpected, pos );

	InfPatternItems::TagFunction * Condition = nullptr;
	auto iee = CreateTrueCondition( Condition, aAllocator );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Добавление нового УОУШ в список УОУШ шаблона.
	auto cond_ptr = new( aAllocator ) InfPatternItems::Base *;
	if( !cond_ptr )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	cond_ptr[0] = Condition;

	auto tlcs = new( aAllocator ) TLCS_WR( cond_ptr, TLCS_WR::Type::Else );
	if( !tlcs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aTLCS.push_back( tlcs );
	if( aTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Добавление нового УОУШ в стек действующих в текущий момент.
	if( !vTLCS_stack.vTLCS.size() )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	vTLCS_stack.vTLCS.pop_back();
	vTLCS_stack.vTLCS.push_back( TLCS_Node( aTLCS.size() - 1, tlcs->GetTLCSType() ) );
	if( vTLCS_stack.vTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTLCS_Endif( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, 0, aStringLength );
	// Проверка ключевого слова endif
	if( pos + 5 > aStringLength || strncasecmp( aString + pos, "endif", 5 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Проверка, предшествовал ли данному Endif какой-нибудь If, Elsif или Else.
	if( !vTLCS_stack.vTLCS.size() ||
		(
			vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::If &&
			vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::Elsif &&
			vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::Else
		) )
		ReturnWithParseError( DLErrorId::peTLCS_ElsifWithoutIf, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 5, aStringLength );

	// Проверка окончания строки с условием уровня шаблона.
	if( pos != aStringLength )
		ReturnWithParseError( DLErrorId::peTLCS_EOLExpected, pos );

	// Добавление нового УОУШ в список УОУШ шаблона.
	auto tlcs = new( aAllocator ) TLCS_WR( nullptr, TLCS_WR::Type::Endif );
	if( !tlcs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aTLCS.push_back( tlcs );
	if( aTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Уменьшаем текущий уровень вложенности.
	if( !vTLCS_stack.vTLCS.size() )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
	vTLCS_stack.vTLCS.pop_back();

	return INF_ENGINE_SUCCESS;

}

InfEngineErrors DLStringParser::ParseTLCS_Switch( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, 0, aStringLength );

	// Проверка ключевого слова switch
	if( pos + 6 > aStringLength || strncasecmp( aString + pos, "switch", 6 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Проверка, предшествовал ли данному Switch другой Switch.
	if( vTLCS_stack.vTLCS.size() && vTLCS_stack.vTLCS.back().vType == TLCS_WR::Type::Switch )
		ReturnWithParseError( DLErrorId::peTLCS_SwitchAfterSwitch, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 6, aStringLength );

	// Проверка на начало условия.
	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peIfOpenRoundBrace, pos );

	// Разбор условия.
	bool ArgIsMultiple = false;
	bool IsFunctionArg = true;
	avector<InfPatternItems::Base *> WaitingForResolving;
	avector<InfPatternItems::Base *> Arguments;
	auto iee = ParseTagFunctionArg( aString, aStringLength, pos, Arguments, WaitingForResolving, aAllocator,
									'(', ')', DLFAT_EXTENDED, ArgIsMultiple, IsFunctionArg, false );
	if( iee != INF_ENGINE_SUCCESS )
		return iee;

	// Проверка на окончание условия.
	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peIfCloseRoundBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка окончания строки с условием уровня шаблона.
	if( pos != aStringLength )
		ReturnWithParseError( DLErrorId::peTLCS_EOLExpected, pos );

	// Добавление нового УОУШ в список УОУШ шаблона.
	auto tlcs = new( aAllocator ) TLCS_WR( nullptr, TLCS_WR::Type::Switch );
	if( !tlcs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aTLCS.push_back( tlcs );
	if( aTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Добавление нового УОУШ в стек действующих в текущий момент.
	vTLCS_stack.vTLCS.push_back( TLCS_Node( static_cast<unsigned int>( -1 ), tlcs->GetTLCSType() ) );
	if( vTLCS_stack.vTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Добавление элемента в стэк действующих операторов switch.
	vTLCS_stack.vSwitch.push_back( Arguments[0] );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTLCS_Case( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, 0, aStringLength );

	// Проверка ключевого слова case
	if( pos + 4 > aStringLength || strncasecmp( aString + pos, "case", 4 ) )
		return INF_ENGINE_WARN_UNSUCCESS;
	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 4, aStringLength );

	// Проверка на начало условия.
	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peIfOpenRoundBrace, pos );

	// Проверка, предшествовал ли данному оператору Case оператор Switch или Case.
	if( !vTLCS_stack.vSwitch.size() || !vTLCS_stack.vTLCS.size() ||
		(
			vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::Switch &&
			vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::Case
		) )
		ReturnWithParseError( DLErrorId::peTLCS_CaseWithoutSwitch, pos );

	// Разбор условия.
	bool ArgIsMultiple = false;
	bool IsFunctionArg = true;
	avector<InfPatternItems::Base *> WaitingForResolving;
	avector<InfPatternItems::Base *> Arguments;
	auto iee = ParseTagFunctionArg( aString, aStringLength, pos, Arguments, WaitingForResolving, aAllocator,
									'(', ')', DLFAT_EXTENDED, ArgIsMultiple, IsFunctionArg, false );
	if( iee != INF_ENGINE_SUCCESS )
		return iee;

	// Проверка на окончание условия.
	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peIfCloseRoundBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка окончания строки с условием уровня шаблона.
	if( pos != aStringLength )
		ReturnWithParseError( DLErrorId::peTLCS_EOLExpected, pos );

	// Создание функции для проверки условия.
	InfPatternItems::TagFunction * Condition = nullptr;
	iee = CreateIsEqualCondition( Condition, vTLCS_stack.vSwitch.back(), Arguments[0], aAllocator );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Добавление нового УОУШ в список УОУШ шаблона.
	auto cond_ptr = new( aAllocator ) InfPatternItems::Base *;
	if( !cond_ptr )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	cond_ptr[0] = Condition;

	auto tlcs = new( aAllocator ) TLCS_WR( cond_ptr, TLCS_WR::Type::Case );
	if( !tlcs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aTLCS.push_back( tlcs );
	if( aTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Добавление нового УОУШ в стек действующих в текущий момент.
	vTLCS_stack.vTLCS.pop_back();
	vTLCS_stack.vTLCS.push_back( TLCS_Node( aTLCS.size() - 1, tlcs->GetTLCSType() ) );
	if( vTLCS_stack.vTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTLCS_Default( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, 0, aStringLength );

	// Проверка ключевого слова case
	if( pos + 7 > aStringLength || strncasecmp( aString + pos, "default", 7 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 7, aStringLength );

	// Проверка, предшествовал ли данному оператору Default оператор Case.
	if( !vTLCS_stack.vSwitch.size() || !vTLCS_stack.vTLCS.size() || vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::Case )
		ReturnWithParseError( DLErrorId::peTLCS_DefaultWithoutCase, pos );

	// Создание условия.
	InfPatternItems::TagFunction * Condition = nullptr;
	auto iee = CreateTrueCondition( Condition, aAllocator );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Проверка окончания строки с условием уровня шаблона.
	if( pos != aStringLength )
		ReturnWithParseError( DLErrorId::peTLCS_EOLExpected, pos );

	// Добавление нового УОУШ в список УОУШ шаблона.
	auto cond_ptr = new( aAllocator ) InfPatternItems::Base *;
	if( !cond_ptr )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	cond_ptr[0] = Condition;

	auto tlcs = new( aAllocator ) TLCS_WR( cond_ptr, TLCS_WR::Type::Default );
	if( !tlcs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aTLCS.push_back( tlcs );
	if( aTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Добавление нового УОУШ в стек действующих в текущий момент.
	vTLCS_stack.vTLCS.pop_back();
	vTLCS_stack.vTLCS.push_back( TLCS_Node( aTLCS.size() - 1, tlcs->GetTLCSType() ) );
	if( vTLCS_stack.vTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTLCS_Endswitch( const char * aString, unsigned int aStringLength, TLCS_WRs & aTLCS, nMemoryAllocator & aAllocator )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, 0, aStringLength );

	// Проверка ключевого слова case
	if( pos + 9 > aStringLength || strncasecmp( aString + pos, "endswitch", 9 ) )
		return INF_ENGINE_WARN_UNSUCCESS;
	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 9, aStringLength );

	// Проверка, предшествовал ли данному оператору Endswitch оператор Case или Default.
	if( !vTLCS_stack.vSwitch.size() || !vTLCS_stack.vTLCS.size() ||
		(
			vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::Case &&
			vTLCS_stack.vTLCS.back().vType != TLCS_WR::Type::Default
		) )
		ReturnWithParseError( DLErrorId::peTLCS_DefaultWithoutCase, pos );

	// Проверка окончания строки с условием уровня шаблона.
	if( pos != aStringLength )
		ReturnWithParseError( DLErrorId::peTLCS_EOLExpected, pos );

	auto tlcs = new( aAllocator ) TLCS_WR( nullptr, TLCS_WR::Type::Endswitch );
	if( !tlcs )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aTLCS.push_back( tlcs );
	if( aTLCS.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Удаление ткущего УОУШ из стека.
	vTLCS_stack.vTLCS.pop_back();
	vTLCS_stack.vSwitch.pop_back();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseCondTypePatternString( const char * aString, unsigned int aStringLength, unsigned int aPos,
															InfPatternItems::ConditionRW * & aResult, unsigned int & aInfPerson, nMemoryAllocator & aAllocator )
{
	// Проверка аргументов.
	if( !aString && aStringLength )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Очистка списка ошибок.
	vErrorDescriptions.clear();
	// Очистка списка предупреждений.
	vWarningDescriptions.clear();

	// Шаблон-условие может состоять только из базового условия.

	// Разбор базового условия.
    unsigned int pos = aPos;
	InfPatternItems::BaseCondition * Condition = nullptr;
	bool IsMultiply;
	auto iee = ParseBaseCondition( aString, aStringLength, pos, Condition, aAllocator, IsMultiply, aInfPerson );
	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Выделение памяти под шаблон-условие.
	aResult = new( aAllocator ) InfPatternItems::ConditionRW( Condition, 0 );
	if( !aResult )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseQstTypePatternString( const char * aString, unsigned int aStringLength, unsigned int aPos,
														   avector<InfPatternItems::Base *> & aResult, avector<Segment> &aReferences,
														   nMemoryAllocator & aAllocator )
{
	// Сбрасываем счётчик словарей в текущем шаблон-впросе.
	vLastQstDictsNumber = 0;
	bool IsMultiple = false;
	bool ArgIsMultiple;

	// Проверка аргументов.
	if( !aString && aStringLength != 0 )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Очистка контейнера для результата.
	aResult.clear();

	// Очистка карты референций.
	aReferences.clear();

	// Очистка списка ошибок.
	vErrorDescriptions.clear();
	// Очистка списка предупреждений.
	vWarningDescriptions.clear();

	// Очистка списка открытых референций.
	vOpenReferences.clear();

	// Проверка на пустоту шаблон-вопроса.
    if( aStringLength - aPos == 0 )
		ReturnWithParseError( DLErrorId::peQuestionIsEmpty, 0 );

	// Разбор входной строки.
    for( unsigned int pos = aPos; pos < aStringLength; pos++ )
	{
		// Разбор звездочки и суперзвездочки.
		if( aString[pos] == '*' )
		{
			// Выделение памяти для элемента звездочки.
			auto TagStar = new( aAllocator ) InfPatternItems::Star;
			if( !TagStar )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Проверка на суперзвездочку.
			if( aString[pos + 1] == '*' )
			{
				TagStar->Set( 0, true );
				pos++;
			}
			else
				TagStar->Set( 0, false );

			// Добавление тэга в контейнер для результата.
			aResult.push_back( TagStar );
			if( aResult.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
		else if( aString[pos] == '[' )
		{
			// Разбор тэга.

			// Начало референции.
			if( pos + 1 < aStringLength && aString[pos + 1] == '-' )
			{
				// Добавление референции в карту референций.
				aReferences.grow();
				if( aReferences.no_memory() )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				aReferences.back().first = aResult.size();

				// Добавление референции в список открытых референций.
				vOpenReferences.push_back( aReferences.size() - 1 );
				if( vOpenReferences.no_memory() )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				pos++;
			}
			else
			{
				// Тэг [dict].                
				InfEngineErrors iee = ParseTagDict( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
				if( iee == INF_ENGINE_SUCCESS )
				{
					IsMultiple |= ArgIsMultiple;
					continue;
				}
				else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					return iee;
				else if( iee != INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithTrace( iee );

				// Inline-словарь.
				iee = ParseTagDictInline( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple, false );
				if( iee == INF_ENGINE_SUCCESS )
				{
					IsMultiple |= ArgIsMultiple;
					continue;
				}
				else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					return iee;
				else if( iee != INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithTrace( iee );

				// Тэг либо запрещен к использованию в шаблон-вопросе, либо является некорректным.
				ReturnWithParseError( DLErrorId::peQuestionInvalidTag, pos );
			}
		}
		else if( aString[pos] == '{' || aString[pos] == '<' )
		{
			// Inline-словарь.
			InfEngineErrors iee = ParseTagDictInline( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple, false );
			if( iee == INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithParseError( DLErrorId::peInlineDictWithoutEndDelimiter, pos );
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			IsMultiple |= ArgIsMultiple;
		}
		else if( TextFuncs::IsSpace( aString[pos] ) )
		{
			// Пропуск пробельного символа.
		}
		else
		{
			// Текстовый элемент.
			InfEngineErrors iee = ParseText( aString, aStringLength, pos, "*{[<", aResult, aAllocator, true, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMultiple |= ArgIsMultiple;

				// Завершение референции.
				if( pos + 1 < aStringLength && aString[pos + 1] == '-' )
				{
					// Проверка на наличие открытой референции.
					if( vOpenReferences.empty() )
					{
						AddParseError( DLErrorId::peReferenceWithoutStartPos, pos + 1 );

						if( vStrictMode )
							return INF_ENGINE_ERROR_CANT_PARSE;
						else
						{
							// Добавление -] в список разобранных элементов в виде словаря.

							// Выделение памяти.
							nAllocateNewObject( aAllocator, InfPatternItems::TagDictInline, tag );
							if( !tag )
								ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
							new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagDictInline;

							// Массив указателей на варианты инлайн словаря.
							nAllocateNewObjects( aAllocator, char *, 1, Strings );
							if( !Strings )
								ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

							// Массив длин элементов inline-словаря.
							nAllocateNewObjects( aAllocator, unsigned int, 1, StringsLengths );
							if( !StringsLengths )
								ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

							// Буффер для данных inline-словаря.
							char * Buffer = static_cast<char *>( aAllocator.Allocate( 3 ) );
							if( !Buffer )
								ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
							strcpy( Buffer, "-]" );

							// Формирование тэга inline-словаря.
							Strings[0] = Buffer;
							StringsLengths[0] = strlen( "-]" );

							tag->Set( Strings, StringsLengths, 1 );

							// Добавление пустого варианта.
							tag->AddEmptyVariant();

							aResult.push_back( tag );
							if( aResult.no_memory() )
								ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

							// Пропускаем символы -]
							pos += 2;
						}
					}
					else
					{
						// Завершение референции.
						aReferences[vOpenReferences.back()].second = aResult.size();

						// Проверка на пустую референцию.
						if( aReferences[vOpenReferences.back()].first == aReferences[vOpenReferences.back()].second )
							AddParseWarning( DLErrorId::peReferenceIsEmpty, pos );

						// Удаление референции из списка открытых.
						vOpenReferences.pop_back();

						// Сдвиг позиции.
						pos += 2;
					}
				}
			}
			else if( iee == INF_ENGINE_ERROR_EMPTY )
				ReturnWithParseError( DLErrorId::peQuestionIsEmpty, 0 );
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else
				ReturnWithTrace( iee );
		}
	}

	// Проверка на незакрытые референции.
	if( !vOpenReferences.empty() )
	{
		for( unsigned int i = 0; i < vOpenReferences.size(); i++ )
		{
			// Ошибка разбора.
			AddParseError( DLErrorId::peReferenceWithoutEndPos, aReferences[vOpenReferences[i]].first );

			if( vStrictMode )
				return INF_ENGINE_ERROR_CANT_PARSE;
			else
				aReferences[vOpenReferences[i]].second = aResult.size();
		}
	}

	// Сбрасываем счётчики словарей в текущем шаблон.
	vMinQstDictsNumber = std::min( vMinQstDictsNumber, vLastQstDictsNumber );
	vMaxQstDictsNumber = std::max( vMaxQstDictsNumber, vLastQstDictsNumber );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseThatTypePatternString (
        const char * aString, unsigned int aStringLength,
        unsigned int aPos,
        avector<InfPatternItems::Base *> & aResult,
        nMemoryAllocator & aAllocator
) {
	bool IsMultiple = false;
	bool ArgIsMultiple;

	// Проверка аргументов.
    if( !aString && aStringLength - aPos != 0 )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Очистка контейнера для результата.
	aResult.clear();

	// Очистка списка ошибок.
	vErrorDescriptions.clear();
	// Очистка списка предупреждений.
	vWarningDescriptions.clear();

	// Разбор входной строки.
    for( unsigned int pos = aPos; pos < aStringLength; pos++ )
	{
		// Разбор звездочки и суперзвездочки.
		if( aString[pos] == '*' )
		{
			// Выделение памяти для элемента звездочки.
			nAllocateNewObject( aAllocator, InfPatternItems::Star, TagStar );
			if( !TagStar )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			new((nMemoryAllocatorElementPtr *)TagStar ) InfPatternItems::Star;

			// Проверка на суперзвездочку.
			if( aString[pos + 1] == '*' )
			{
				TagStar->Set( 0, true );
				pos++;
			}
			else
				TagStar->Set( 0, false );

			// Добавление тэга в контейнер для результата.
			aResult.push_back( TagStar );
			if( aResult.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
		else if( aString[pos] == '[' )
		{
			// Тэг [dict].
//			InfEngineErrors iee = ParseTagDict( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
//			if( iee == INF_ENGINE_SUCCESS )
//			{
//				IsMultiple |= ArgIsMultiple;
//				continue;
//			}
//			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
//				return iee;
//			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
//				ReturnWithTrace( iee );

			// Inline-словарь.
            InfEngineErrors iee = ParseTagDictInline( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple, false );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMultiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			// Тэг либо запрещен для использования в шаблон-that'е, либо является некорректным.
			ReturnWithParseError( DLErrorId::peThatInvalidTag, pos );
		}
		else if( aString[pos] == '{' || aString[pos] == '<' )
		{
			// Inline-словарь.
			InfEngineErrors iee = ParseTagDictInline( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple, false );
			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			IsMultiple |= ArgIsMultiple;
		}
		else if( TextFuncs::IsSpace( aString[pos] ) )
		{
			// Пропуск пробельного символа.
		}
		else
		{
			// Текстовый элемент.
			InfEngineErrors iee = ParseText( aString, aStringLength, pos, "*{[<", aResult, aAllocator, false, ArgIsMultiple );
			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			IsMultiple |= ArgIsMultiple;
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseAnsTypePatternString( const char * aString, unsigned int aStringLength, unsigned int aPos,
														   avector<InfPatternItems::Base *> & aResult,
														   avector<InfPatternItems::Base *> & aWaitingForResolving,
														   nMemoryAllocator & aAllocator,
														   bool &aIsMultiple, bool aIsFunctionArg,
														   const std::string & aPatternId, const std::string & aFileName )
{
	aIsMultiple = false;
	bool ArgIsMultiple;

	vTmpVarsRegistry.clear();

	// Сохранение информации для формирования сообщений об ошибках.
	try
	{
		vPatternId = aPatternId;
		vFileName = aFileName;
	}
	catch( ... )
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	// Проверка аргументов.
	if( !aString && aStringLength - aPos != 0 )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Очистка списка ошибок.
	vErrorDescriptions.clear();
	// Очистка списка предупреждений.
	vWarningDescriptions.clear();

	auto original_size = aResult.size();

	// Разбор шаблон-ответа.
	for( unsigned int pos = aPos; pos < aStringLength; pos++ )
	{
		if( aString[pos] == '[' )
		{
			// Разбор тэга.
			InfEngineErrors iee = ParseTagsTypeAns( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, ArgIsMultiple, aIsFunctionArg );
			if( iee == INF_ENGINE_SUCCESS )
			{
				aIsMultiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			// Либо тэг запрещен для использования в шаблон-ответе, либо является некорректным.
			ReturnWithParseError( DLErrorId::peAnswerInvalidTag, pos );
		}
		else if( aString[pos] == '{' )
		{
			// Inline-словарь.
			InfEngineErrors iee = ParseTagDictInline( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple, false );
			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			aIsMultiple |= ArgIsMultiple;
		}
		else if( TextFuncs::IsSpace( aString[pos] ) )
		{
			// Пробельный символ пропускаем.
		}
		else
		{
			// Разбор текста.
			InfEngineErrors iee = ParseText( aString, aStringLength, pos, "[{", aResult, aAllocator, false, ArgIsMultiple );
			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			{
				aIsMultiple |= ArgIsMultiple;
				return iee;
			}
			else if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
		}
	}

	// Проверка корректности тэгов разметки.
	if( aResult.size() > original_size )
	{
        unsigned int counter = 0;
		TryWithTrace( CheckLists( &aResult[original_size], &aResult.back() + 1, counter ), INF_ENGINE_SUCCESS );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseDictInstrInclude( const char * aDict, unsigned int aDictLength,
													   InfDictInstrInclude * & aResult,nMemoryAllocator & aDictCompilationAllocator,
													   aTextString & aErrorDescription, aTextString & aErrorString )
{
	// Проверка аргументов.
	if( !aDict )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка начала интсрукции.
	if( aDictLength < 2 || aDict[0] != '-' || aDict[1] != '-' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропускаем пробелы.
	unsigned int pos = TextFuncs::SkipSpaces( aDict, 2, aDictLength );

	// Проверка ключевого слова.
	if( aDictLength - pos < 7 || strncasecmp( aDict + pos, "include", 7 ) )
		return INF_ENGINE_WARN_UNSUCCESS;
	pos += 7;

	// Пропускаем пробелы.
	pos = TextFuncs::SkipSpaces( aDict, pos, aDictLength );

	// Открывающая скобка.
	if( pos >= aDictLength || aDict[pos] != '(' )
	{
		aErrorDescription.assign( "'(' expected in instruction '--include'" );
		aErrorString.assign( aDict );
		return INF_ENGINE_ERROR_CANT_PARSE;
	}

	// Пропускаем пробелы.
	pos = TextFuncs::SkipSpaces( aDict, pos + 1, aDictLength );
	unsigned int dict_name_begin = pos;
	unsigned int dict_name_end = pos;

	// Выделяем имя словаря.
	while( dict_name_end < aDictLength && ')' != aDict[dict_name_end] && !TextFuncs::IsSpace( aDict[dict_name_end] ) )
		++dict_name_end;

	// Пропускаем пробелы.
	pos = TextFuncs::SkipSpaces( aDict, dict_name_end, aDictLength );

	// Проверка закрывающей скобки.
	if( pos >= aDictLength || aDict[pos] != ')' )
	{
		aErrorDescription.assign( "')' expected in instruction '--include'" );
		aErrorString.assign( aDict );
		return INF_ENGINE_ERROR_CANT_PARSE;
	}

	// Подготовка имени словаря.
	aTextString dict_name;
	nlReturnCode nlrc = dict_name.assign( aDict + dict_name_begin, dict_name_end - dict_name_begin );
	if( nlrcSuccess != nlrc )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	TextFuncs::ToLower( dict_name.ToChar(), dict_name.size() );

	// Поиск имени словаря в реестре.
	const unsigned int * dict_id = vDLDataWR.GetDictId( dict_name.ToConstChar(), dict_name.size() );
	if( !dict_id )
	{
		aErrorDescription.assign( "Unknown dictionary name \"" );
		aErrorDescription.append( dict_name.ToConstChar() );
		aErrorDescription.append( "\" in --include instruction" );
		aErrorString.assign( aDict );
		return INF_ENGINE_ERROR_CANT_PARSE;
	}

	// Создание результата.
	aResult = nAllocateObject( aDictCompilationAllocator, InfDictInstrInclude );
	new((nMemoryAllocatorElementPtr *)aResult )InfDictInstrInclude( *dict_id );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseDictInstruction( const char * aDict, unsigned int aDictLength,
													  InfDictInstruction * & aResult, nMemoryAllocator &aDictCompilationAllocator,
													  aTextString & aErrorDescription, aTextString & aErrorString )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Проверка аргументов.
	if( !aDict )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка начала интсрукции.
	if( aDictLength < 2 || aDict[0] != '-' || aDict[1] != '-' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Разбор инструкции типа include.
	InfDictInstrInclude * instr_include = nullptr;
	iee = ParseDictInstrInclude( aDict, aDictLength, instr_include, aDictCompilationAllocator, aErrorDescription, aErrorString );
	if( iee == INF_ENGINE_SUCCESS )
	{
		aResult = instr_include;
		return INF_ENGINE_SUCCESS;
	}
	else if( INF_ENGINE_ERROR_CANT_PARSE == iee )
		return INF_ENGINE_ERROR_CANT_PARSE;
	else if( INF_ENGINE_WARN_UNSUCCESS != iee )
		ReturnWithTrace( iee );

	aErrorDescription.assign( " Unknown instruction " );
	aErrorString.assign( aDict );

	return INF_ENGINE_ERROR_CANT_PARSE;
}

InfEngineErrors DLStringParser::ParseDictString( const char * aDict, unsigned int aDictLength,
												 InfDictString * & aResult, nMemoryAllocator &aDictCompilationAllocator,
												 aTextString & aErrorDescription, aTextString & aErrorString )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	// Проверка аргументов.
	if( !aDict )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	unsigned int terms_number = 0;
	bool is_main = true;
	unsigned int begin = 0;
	bool is_multiple = false;

	// Подчиненённая строка.
	if( aDictLength > 1 && '=' == aDict[0] && '>' == aDict[1] )
	{
		// Данная строка помечена как подчинённая.
		is_main = false;
		// Пропускаем ключевую последовательность сиволов, обозначающую подчинение.
		begin = 2;
	}
	else if( aDictLength > 2 && '\\' == aDict[0] && '=' == aDict[1] && '>' == aDict[2] )
	{
		// Встречена экранированная последовательность сиволов, обозначающая подчинение.
		begin = 1;
	}

	// Временный массив для хранения элементов словаря.
	static avector<InfPatternItems::Base *> vItems;
	vItems.clear();

	// Разбор шаблон-словаря.
	for( unsigned int iptr = begin; iptr < aDictLength; iptr++ )
	{
		bool dict_just_parsed = false;
		// Разбор inline-словаря.
		if( INF_ENGINE_SUCCESS == ( iee = ParseTagDictInline( aDict, aDictLength, iptr, vItems, aDictCompilationAllocator, is_multiple, true ) ) )
		{
			++terms_number;
			dict_just_parsed = true;
		}
		else if( INF_ENGINE_ERROR_CANT_PARSE == iee )
		{
			// Возможно, в словаре есть строка с синтаксисом похожим на inline-словарь.
			// do nothing
		}
		else if( INF_ENGINE_WARN_UNSUCCESS != iee )
			ReturnWithTrace( iee );

		if( !dict_just_parsed )
		{
			if( aDict[iptr] == '*' )
			{
				// Optimization: optimization.
				InfPatternItems::Star * tag = nAllocateObject( aDictCompilationAllocator, InfPatternItems::Star );
				if( !tag )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::Star;

				if( aDict[iptr + 1] == '*' )
				{
					tag->Set( 0, true );
					iptr++;
				}
				else
					tag->Set( 0, false );

				vItems.push_back( tag );
				if( vItems.no_memory() )
				{
					delete tag;

					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}
			}
			else if( aDict[iptr] == ' ' )
			{
				// Пробельный символ пропускаем.
			}
			else
			{
				// Выделение текста.
				iee = ParseDictText( aDict, aDictLength, iptr, iptr, "*{<[", vItems, aDictCompilationAllocator, aErrorDescription, aErrorString );
				if( iee == INF_ENGINE_SUCCESS )
				{
					terms_number++;

					continue;
				}
				else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					return INF_ENGINE_ERROR_CANT_PARSE;
				else
					ReturnWithTrace( iee );
			}
		}
	}

	if( terms_number == 0 )
	{
		aErrorDescription.assign( "Strings without text is restricted." );
		aErrorString.assign( aDict );

		return INF_ENGINE_ERROR_CANT_PARSE;
	}

	// Создание строки словаря.
	aResult = nAllocateObject( aDictCompilationAllocator, InfDictString );
	if( !aResult )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new( (nMemoryAllocatorElementPtr *)aResult )InfDictString;

	// Сохранение строки словаря.
	InfPatternItems::Base ** ItemsBuffer = nAllocateObjects( aDictCompilationAllocator, InfPatternItems::Base *, vItems.size() );
	if( !ItemsBuffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( ItemsBuffer, vItems.get_buffer(), vItems.size() * sizeof( InfPatternItems::Base * ) );
	aResult->SetItems( ItemsBuffer, vItems.size() );

	// Сохранение текста строки.
	char * OriginalBuffer = nAllocateObjects( aDictCompilationAllocator, char, aDictLength - begin + 1 );
	if( !OriginalBuffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( OriginalBuffer, aDict + begin, aDictLength - begin );
	OriginalBuffer[aDictLength - begin] = '\0';
	aResult->SetOriginal( OriginalBuffer, aDictLength - begin );
	aResult->SetMainFlag( is_main );

	return INF_ENGINE_SUCCESS;

}

InfEngineErrors DLStringParser::ParseDictText( const char * aString, unsigned int aStringLength, unsigned int aStartPos, unsigned int & aFinishPos,
											   const char * aEndDelimiters, avector<InfPatternItems::Base *> & aItems,
											   nMemoryAllocator &aDictCompilationAllocator, aTextString & aErrorDescription, aTextString & aErrorString )
{
	nlReturnCode nle;

	unsigned int pos = aStartPos;

	// Выделение текста.
	unsigned int begin = TextFuncs::SkipSpaces( aString, pos );
	for( pos = begin; pos < aStringLength; pos++ )
	{
		if( pos > aStartPos && strchr( aEndDelimiters, aString[pos] ) )
		{
			aFinishPos = pos - 1;
			break;
		}

		if( aString[pos] == '\\' )
		{
			if( pos + 1 < aStringLength && aString[pos + 1] == '\\' )
			{}
			else if( pos + 1 < aStringLength && aString[pos + 1] == '[' )
			{}
			else if( pos + 1 < aStringLength && strchr( aEndDelimiters, aString[pos + 1] ) )
			{}
			else
			{
				aErrorDescription.assign( "Символ '\\' может предварять только следующие символы: '" );
				for( unsigned int pos = 0; aEndDelimiters && aEndDelimiters[pos]; ++pos )
				{
					if( pos )
						aErrorDescription.append( "', '" );
					aErrorDescription.append( aEndDelimiters + pos, 1 );
				}

				aErrorDescription.append( "'" );
				aErrorString.assign( aString + aStartPos );

				return INF_ENGINE_ERROR_CANT_PARSE;
			}
			pos++;
		}
	}

	// Выделение памяти.
	InfPatternItems::Text * tag = nAllocateObject( aDictCompilationAllocator, InfPatternItems::Text );
	if( !tag )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::Text;

	aTextString * tmp = nAllocateObject( aDictCompilationAllocator, aTextString );
	if( !tmp )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	new((nMemoryAllocatorElementPtr *)tmp )aTextString;

	nle = tmp->assign( aString + begin, pos - begin );
	if( nle != nlrcSuccess )
		return INF_ENGINE_ERROR_FAULT;

	// Разэскейпливание.
	RemoveEscapeSymbols( *tmp );

	AllTrim( *tmp );

	// Проверка на пустоту.
	if( !tmp->empty() )
	{
		tag->Set( tmp->ToConstChar(), tmp->size() );

		// Добавление разобранного текста.
		aItems.push_back( tag );
		if( aItems.no_memory() )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;
	}
	aFinishPos = pos - 1;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseCondition( const char * aString, unsigned int aStringLength,
												unsigned int & aPos, InfPatternItems::TagFunction * & aCondition,
												nMemoryAllocator & aAllocator, bool & aIsMultiple, bool IsFunctionArg )
{
	bool IsMultuple = false;
	bool ArgIsMultiple;

	//TODO: reimplement for If function.

	unsigned int ConditionBegin = aPos;

	// Проверка на начало условия.
	if( aString[ConditionBegin] != '(' )
		return INF_ENGINE_WARN_UNSUCCESS;

	unsigned int pos = aPos;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка пустоты строки шаблона.
	if( pos >= aStringLength )
		return INF_ENGINE_WARN_UNSUCCESS;

	vtmp.clear();

	if( aStringLength && ( aString[pos] == '[' || aString[pos] == '!' || aString[pos] == '%' ) )
	{
		// Парсинг условия - функции.
		avector<InfPatternItems::Base *> aResult;
		avector<InfPatternItems::Base *> aWaitingForResolving;
		InfEngineErrors iee = ParseTagFunctionArg( aString, aStringLength, ConditionBegin, aResult, aWaitingForResolving, aAllocator, '(', ')',
												   DLFAT_BOOL, ArgIsMultiple, IsFunctionArg, true );
		if( iee != INF_ENGINE_SUCCESS )
			return iee;
		IsMultuple |= ArgIsMultiple;

		// Условие должно состоять ровно из одного аргумента.
		if( aResult.size() != 1 )
			return INF_ENGINE_ERROR_CANT_PARSE;

		// Аргумент условия должен состоять из одного элемента.
		InfPatternItems::TagFunction::Argument * Condition =
			reinterpret_cast<InfPatternItems::TagFunction::Argument *>( aResult.back() );
		if( Condition->GetItemsCount() != 1 )
			ReturnWithParseError( DLErrorId::peConditionInvalid, ConditionBegin );

		// Этим элемментом должна быть функция или тэг, представляемый как функция.
		InfPatternItems::Base * Item = static_cast<InfPatternItems::Base *>( Condition->GetItem( 0 ) );

		if( !Item || Item->GetType() != InfPatternItems::itFunction )
			return INF_ENGINE_ERROR_CANT_PARSE;

		// Если это функция, то функция должна иметь тип BOOL.
		InfPatternItems::TagFunction * Function = static_cast<InfPatternItems::TagFunction *>( Item );
		aCondition = Function;
		pos = ConditionBegin;

		// Проверка на окончание условия.
		if( pos >= aStringLength || aString[pos] != ')' )
			ReturnWithParseError( DLErrorId::peConditionInvalid, pos );
	}
	else
		return INF_ENGINE_WARN_UNSUCCESS;

	aPos = pos;

	aIsMultiple = IsMultuple;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseBaseCondition( const char * aString, unsigned int aStringLength,
													unsigned int &aPos, InfPatternItems::BaseCondition * & aResult,
													nMemoryAllocator & aAllocator, bool &aIsMultiple, unsigned int &aInfPerson )
{
	unsigned int pos = aPos;

	pos = TextFuncs::SkipSpaces( aString, pos );

	// Проверка пустоты строки шаблона.
	if( pos >= aStringLength )
		return INF_ENGINE_WARN_UNSUCCESS;

	Vars::Id VarId;
	InfBaseConditionType ConditionType;

	// Очистка вспомогательной строки.
	vtmp.clear();

	if( aString[pos] == '!' && aString[pos + 1] == '%' )
	{
		// Выделение имени переменной.
		unsigned int end = GetVarName( aString, aStringLength, pos + 2 );
		if( end == pos + 2 )
			ReturnWithParseError( DLErrorId::peConditionVarNameInvalid, ( pos + 2 ) );

		// Поиск переменной в реестре.
		nlReturnCode nlrc = vtmp.assign( aString + pos + 2, end - pos - 2 );
		if( nlrc != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		if( SetLower( vtmp ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		InfEngineErrors iee = CheckVariable( vtmp.ToConstChar(), vtmp.size(), VarId );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithParseError( DLErrorId::peConditionUnregistredVarName, pos + 2 );
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		ConditionType = InfConditionDontExist;

		// Проверка на наличие лишних нерпобельных символов после условия.
		pos = TextFuncs::SkipSpaces( aString, end, aStringLength );
		if( pos < aStringLength )
			ReturnWithParseError( DLErrorId::peConditionInvalid, end );

		aPos = pos;
	}
	else if( aString[pos] == '%' )
	{
		// Выделение имени переменной.
		unsigned int end = GetVarName( aString, aStringLength, pos + 1 );
		if( end == pos + 1 )
			ReturnWithParseError( DLErrorId::peConditionVarNameInvalid, pos + 1 );

		// Поиск переменной в реестре.
		nlReturnCode nlrc = vtmp.assign( aString + pos + 1, end - pos - 1 );
		if( nlrc != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		if( SetLower( vtmp ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		InfEngineErrors iee = CheckVariable( vtmp.ToConstChar(), vtmp.size(), VarId );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithParseError( DLErrorId::peConditionUnregistredVarName, pos + 1 );
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

		if( pos >= aStringLength )
		{
			aPos = pos;
			ConditionType = InfConditionExist;
		}
		else if( aString[pos] != '=' && !( pos + 1 < aStringLength && aString[pos] == '!' && aString[pos + 1] == '=' ) )
		{
			// Наличие лишних нерпобельных символов после условия.
			ReturnWithParseError( DLErrorId::peConditionInvalid, end );
		}
		else
		{
			if( aString[pos] == '=' )
			{
				ConditionType = InfConditionEqual;

				// Пропуск пробелов после знака равенства.
				pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
			}
			else if( aString[pos] == '!' && aString[pos + 1] == '=' )
			{
				ConditionType = InfConditionNotEqual;

				// Пропуск пробелов после знака равенства.
				pos = TextFuncs::SkipSpaces( aString, pos + 2, aStringLength );
			}
			else
				ReturnWithParseError( DLErrorId::peConditionInvalid, end );

			// Выделение значения.
			unsigned int end = GetText( aString, aStringLength, pos, '"', '"' );
			if( end == pos )
				ReturnWithParseError( DLErrorId::peConditionInvalidValue, end );

			nlReturnCode nlrc = vtmp.assign( aString + pos + 1, end - pos - 2 );
			if( nlrc != nlrcSuccess )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			pos = end;

			// Обработка эскейп символов.
			end = TextFuncs::FindRestrictedSymbol( vtmp.ToConstChar(), vtmp.size(), "\"", 0 );
			if( end != vtmp.size() )
				ReturnWithParseError( DLErrorId::peConditionInvalidValue, end );

			RemoveEscapeSymbols( vtmp );

			// Нормализация значения.
			AllTrim( vtmp );

			// Проверка на условие, связанное с InfPerson.
			if( ConditionType == InfConditionEqual && VarId == InfEngineVarInfPerson )
			{
				iee = vDLDataWR.RegistrateInfPerson( vtmp.ToConstChar(), vtmp.size(), aInfPerson );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );
			}

			// Проверка на сравнение с универсальным обозначением.
			if( !strcasecmp( vtmp.ToConstChar(), InfConditionAny ) || !strcasecmp( vtmp.ToConstChar(), InfConditionAnyShort ) )
			{
				if( ConditionType == InfConditionDontExist )
					ReturnWithParseError( DLErrorId::peConditionInvalidValue, end );
				else
					ConditionType = InfConditionAnyValue;
			}

			// Проверка на наличие лишних нерпобельных символов после условия.
			pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );
			if( pos < aStringLength )
				ReturnWithParseError( DLErrorId::peConditionInvalid, end );

			aPos = pos;
		}
	}
	else
		ReturnWithParseError( DLErrorId::peConditionInvalid, pos );

	// Выделение памяти.
	aResult = nAllocateObject( aAllocator, InfPatternItems::BaseCondition );
	if( !aResult )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)aResult ) InfPatternItems::BaseCondition;

	// Установка значений.
	char * Value = nullptr;
	Value = static_cast<char *>( aAllocator.Allocate( vtmp.size() + 1 ) );
	if( !Value )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( Value, vtmp.ToConstChar(), vtmp.size() );
	Value[vtmp.size()] = '\0';

	aResult->SetVarId( VarId );

	// Получение идентификаторов всех базовых условий из реестра условий.
	unsigned int ConditionId;
	InfEngineErrors iee = vDLDataWR.RegistrateCondition( ConditionType, VarId, Value, ConditionId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	aResult->SetConditionId( ConditionId );

	aIsMultiple = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagsTypeAns( const char * aString, unsigned int aStringLength,
												  unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
												  avector<InfPatternItems::Base *> & aWaitingForResolving,
												  nMemoryAllocator & aAllocator,
												  bool & aIsMultiple, bool aIsFunctionArg )
{
	// Тэг должен начинаться с открывающейся квадратной скобки.
	if( aString[aPos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Перебор возможных тэгов.
	InfEngineErrors iee = ParseDictRef( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseReference( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagBreak( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagBR( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagNothing( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagDisableautovars( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseVarAndInstruct( aString, aStringLength, aPos, aResult, aAllocator, true, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagInf( aString, aStringLength, aPos, aWaitingForResolving, aResult, aAllocator, aIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagDict( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagGoTo( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagHref( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagOpenWindow( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagIf( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagSwitch( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagStar( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagDictInline( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple, false );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseExternalService( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagFunction( aString, aStringLength, aPos, aResult,aWaitingForResolving, aAllocator, aIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagRSS( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagExtendAnswer( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseTagPre( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseStartList( aString, aStringLength, aPos, aResult, aAllocator );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseListItem( aString, aStringLength, aPos, aResult, aAllocator );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	iee = ParseEndList( aString, aStringLength, aPos, aResult, aAllocator );
	if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	// Тэгов не обнаружено.
	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLStringParser::ParseTagDict( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											  avector<InfPatternItems::Base *> & aResult,
											  nMemoryAllocator & aAllocator, bool & aIsMultiple )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1 );

	// Определение наличия у словря пустого варианта сопоставления.
	bool has_empty_variant = false;
	if( pos >= aStringLength )
		return INF_ENGINE_WARN_UNSUCCESS;
	if( aString[pos] == '<' )
	{
		has_empty_variant = true;
		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos + 1 );
	}

	// Определение типа словря.
	enum InfPatternItems::DictType dict_type;
	if( pos + 4 >= aStringLength )
		return INF_ENGINE_WARN_UNSUCCESS;

	if( !strncasecmp( aString + pos, "dict", 4 ) )
	{
		dict_type = InfPatternItems::DT_DICT;
		pos += 4;
	}
	else
	{
		if( pos + 5 >= aStringLength )
			return INF_ENGINE_WARN_UNSUCCESS;

		if( !strncasecmp( aString + pos, "udict", 5 ) )
		{
			dict_type = InfPatternItems::DT_UDICT;
			pos += 5;
		}
		else
		{
			if( pos + 6 >= aStringLength )
				return INF_ENGINE_WARN_UNSUCCESS;

			if( !strncasecmp( aString + pos, "sudict", 6 ) )
			{
				dict_type = InfPatternItems::DT_SUDICT;
				pos += 6;
			}
			else
			{
				if( pos + 6 >= aStringLength )
					return INF_ENGINE_WARN_UNSUCCESS;

				if( !strncasecmp( aString + pos, "lcdict", 6 ) )
				{
					dict_type = InfPatternItems::DT_LCDICT;
					pos += 6;
				}
				else
					return INF_ENGINE_WARN_UNSUCCESS;
			}
		}
	}

	pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );

	if( aString[pos] != '(' )
        ReturnWithParseError( DLErrorId::peDictWithoutOpenRoundBrace, aPos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Выделение имени.
	unsigned int end = GetDictName( aString, aStringLength, pos );
	if( end == pos )
        ReturnWithParseError( DLErrorId::peDictNameInvalid, aPos );

	nlReturnCode nlrc = vtmp.assign( aString + pos, end - pos );
	if( nlrc != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	AllTrim( vtmp );
	if( SetLower( vtmp ) != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

	if( aString[pos] != ')' )
        ReturnWithParseError( DLErrorId::peDictWithoutCloseRoundBrace, aPos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( has_empty_variant )
	{
		if( aString[pos] != '>' )
            ReturnWithParseError( DLErrorId::peDictWithoutCloseTriangularBrace, aPos );
		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	}

	if( aString[pos] != ']' )
        ReturnWithParseError( DLErrorId::peDictWithoutCloseSquareBrace, aPos );

	const unsigned int * DictId = nullptr;
	if( ( DictId = vDLDataWR.GetDictId( vtmp.ToConstChar(), vtmp.size() ) ) == nullptr )
        ReturnWithParseError( DLErrorId::peDictUnregistredName, aPos );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::TagDict, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagDict( dict_type, has_empty_variant );

	// Устанавливаем идентификатор.
	tag->Set( *DictId );

	// Добавление элемента в массив.
	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMultiple = true;

	// Инкрементируем счётчик словарей в текущем шаблон-вопросе.
	++vLastQstDictsNumber;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagGoTo( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											  avector<InfPatternItems::Base *> & aResult,
											  nMemoryAllocator & aAllocator, bool & aIsMultiple )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1 );

	if( pos + 9 >= aStringLength || strncasecmp( aString + pos, "GetAnswer", 9 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 9, aStringLength );

	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peGoToOpenRoundBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Выделение имени.
	unsigned int end = GetDictName( aString, aStringLength, pos );
	if( end == pos )
		ReturnWithParseError( DLErrorId::peGoToInvalidPatternName, pos );

	nlReturnCode nle = vtmp.assign( aString + pos, end - pos );
	if( nle != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	AllTrim( vtmp );
	if( SetLower( vtmp ) != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peGoToCloseRoundBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peGoToCloseSquareBrace, pos );

	unsigned int LabelId = 0;
	// @todo Разобраться с метками.
	//    NanoLib::NameIndex::ReturnCode nlnie = aGoToLables.AddName( vTmp.ToConstChar(), vTmp.GetLen(), LabelId );
	//    if( nlnie != NanoLib::NameIndex::rcSuccess && nlnie != NanoLib::NameIndex::rcElementExists )
	//        ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Выделяем память.
	nAllocateNewObject( aAllocator, InfPatternItems::TagGetAnswer, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagGetAnswer;

	// Устанавливаем идентификатор.
	tag->Set( LabelId );

	// Добавление элемента в массив.
	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMultiple = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagDictInline( const char * aString, unsigned int aStringLength,
													unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
													nMemoryAllocator & aAllocator, bool & aIsMultiple, bool aUseDictSyntax )
{
	// Пропуск пробелов.
	unsigned int pos = TextFuncs::SkipSpaces( aString, aPos, aStringLength );

	// Символ-ограничитель inline-словаря. Символ, который завершает тэг.
	char EndDelimiter;
	//  Указатель на строку символов, которые разбивают inline словарь на части,
	// включая символ-ограничитель.
	const char * Delimiters = nullptr;
	// Указатель на строку символов, которые необходимо эскейпить внутри inline-словаря.
	const char * EscapeSymbols = nullptr;

	// Запоминаем начальную позицию.
	unsigned int startpos = pos;

	// Флаг, показывающий, что в inline-словарь необходимо добавить пустой вариант, если такого нет.
	bool AddEmpty = false;

	// Допустимы три типа синтаксиса.
	if( ( pos + 1 < aStringLength && aString[pos] == '{' ) )
	{
		startpos = TextFuncs::SkipSpaces( aString, pos + 1 );
		Delimiters = "/}";
		EndDelimiter = '}';
		EscapeSymbols = "/}{";
	}
	else if( !aUseDictSyntax )
	{
		if( ( pos + 1 < aStringLength && aString[pos] == '<' ) )
		{
			startpos = TextFuncs::SkipSpaces( aString, pos + 1 );
			Delimiters = "/>";
			EndDelimiter = '>';
			EscapeSymbols = "/><";
			AddEmpty = true;
		}
		else if( pos + 3 < aStringLength && !strncasecmp( "[=>", aString + pos, 3 ) )
		{
			startpos = TextFuncs::SkipSpaces( aString, pos + 3 );
			Delimiters = "/]";
			EndDelimiter = ']';
			EscapeSymbols = "/][";
		}
		else
			return INF_ENGINE_WARN_UNSUCCESS;
	}
	else
		return INF_ENGINE_WARN_UNSUCCESS;

	//  Устанавливаем указатель в первую позицию за инициализирующими символами.
	// Символами, которые определют начало тэга inline-словаря.
	pos = startpos;

	// Подсчет числа элементов в inline-словаре.
	unsigned int elements = 0;
	do
	{
		elements++;

		// Поиск первого незаэскейпленного ограничителя.
		unsigned int sym = TextFuncs::FindSymbolTrue( aString, aStringLength, Delimiters, pos );
		if( sym == aStringLength )
			ReturnWithParseError( DLErrorId::peInlineDictWithoutEndDelimiter, pos );

		// Перескакиваем через ограничитель.
		pos = sym + 1;
	} while( aString[pos - 1] != EndDelimiter );

	// Если необходимо добавить пустой элемент, учитываем это при выделениии памяти.
	if( AddEmpty )
		elements++;

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::TagDictInline, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagDictInline;

	// Массив указателей на варианты инлайн словаря. Возможно реально вариантов будет меньше.
	nAllocateNewObjects( aAllocator, char *, elements, Strings );
	if( !Strings )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Массив длин элементов inline-словаря.
	nAllocateNewObjects( aAllocator, unsigned int, elements, StringsLengths );
	if( !StringsLengths )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Буффер для данных inline-словаря.
	char * Buffer = static_cast<char *>( aAllocator.Allocate( pos - startpos + ( AddEmpty ? 1 : 0 ) ) );
	if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Счетчик реальных элементов inline-словаря.
	unsigned int StringsNum = 0;

	// Разбор inline-словаря.
	pos = startpos;
	do
	{
		// Поиск ближайшего незаэскейпленного ограничителя.
		unsigned int sym = TextFuncs::FindSymbolTrue( aString, aStringLength, Delimiters, pos );
		if( sym == aStringLength )
			ReturnWithParseError( DLErrorId::peInlineDictWithoutEndDelimiter, pos );

		// Копирование варианта значения.
		memcpy( Buffer, aString + pos, sym - pos );
		// Дописываем символ конца строки.
		Buffer[sym - pos] = '\0';

		// Очистка варианта.
		unsigned int len = TextFuncs::AllTrim( Buffer, sym - pos );

		// Проверка обозначения пустой строки.
		if( !strcmp( Buffer, "--" ) )
		{
			Buffer[0] = '\0';
			len = 0;
		}
		// Проверка на обозначение двух дефисов.
		else if( !strcmp( Buffer, "\\--" ) )
		{
			Buffer[0] = '-';
			Buffer[1] = '-';
			Buffer[2] = '\0';
			len = 2;
		}
		else if( Buffer[0] == '\0' )
		{
			// Ошибка разбора.
			AddParseError( DLErrorId::peInlineDictEmptyVariant, pos );

			if( vStrictMode )
				return INF_ENGINE_ERROR_CANT_PARSE;
		}

		if( len == 0 )
		{
			tag->AddEmptyVariant();

			if( AddEmpty )
				AddEmpty = false;
		}

		// Проверка эскейп символов в варианте.
		unsigned int rest = TextFuncs::FindSymbolTrue( Buffer, len, EscapeSymbols, 0 );
		if( rest != len )
			ReturnWithParseError( DLErrorId::peInlineDictRestrictedSymbol, pos );

		rest = TextFuncs::FindRestrictedSymbol( Buffer, len, EscapeSymbols, 0 );
		if( rest < len )
			ReturnWithParseError( DLErrorId::peInlineDictRestrictedSymbol, pos );

		// Удаление эскейп символов.
		len = TextFuncs::RemoveEscapeSymbols( Buffer, len );

		// Сохранение данных о варианте.
		Strings[StringsNum] = Buffer;
		StringsLengths[StringsNum] = len;
		Buffer += len + 1;
		StringsNum++;

		// Перескакиваем через ограничитель.
		pos = sym + 1;
	} while( aString[pos - 1] != EndDelimiter );

	// Если необходимо, добавляем пустой вариант.
	if( AddEmpty )
	{
		*Buffer = '\0';
		Strings[StringsNum] = Buffer;
		StringsLengths[StringsNum] = 0;
		Buffer += 1;
		StringsNum++;

		tag->AddEmptyVariant();
	}

	// Завершаем создание объекта inline-словаря.
	tag->Set( Strings, StringsLengths, StringsNum );

	// Добавление объекта inline-словаря.
	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Установка позиции ограничителя тэга inline-словаря.
	aPos = pos - 1;

	aIsMultiple = true;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseText( const char * aString, unsigned int aStringLength, unsigned int & aPos,
										   const char * aEndDelimiters, avector<InfPatternItems::Base *> & aResult,
										   nMemoryAllocator & aAllocator, bool aQstType, bool & aIsMultiple )
{
	unsigned int Pos = aPos;

	// Выделение текста.
	unsigned int Begin = TextFuncs::SkipSpaces( aString, Pos, aStringLength );
	for( Pos = Begin; Pos < aStringLength; Pos++ )
	{
		if( strchr( aEndDelimiters, aString[Pos] ) )
		{
			aPos = Pos - 1;
			break;
		}
		else if( aString[Pos] == '\\' )
		{
			// Экранированный символ.
			if( Pos + 1 < aStringLength && aString[Pos + 1] == '\\' ) {}
			else if( Pos + 1 < aStringLength && aString[Pos + 1] == '[' ) {}
			else if( Pos + 1 < aStringLength && strchr( aEndDelimiters, aString[Pos + 1] ) ) {}
			else
				ReturnWithParseError( DLErrorId::peTextRestrictedSymbol, Pos );

			Pos++;
		}
		else if( aQstType && aString[Pos] == '-' && Pos + 1 < aStringLength && aString[Pos + 1] == ']' )
		{
			aPos = Pos - 1;
			break;
		}
	}

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::Text, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::Text;

	// Optimization: optimization point.
	nlReturnCode nle = vtmp.assign( aString + Begin, Pos - Begin );
	if( nle != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Разэскейпливание.
	RemoveEscapeSymbols( vtmp );

	AllTrim( vtmp );

	if( vtmp.empty() )
	{
		aIsMultiple = false;
		return INF_ENGINE_SUCCESS;
	}

	unsigned int textlen = vtmp.size();
	char * text = static_cast<char *>( aAllocator.Allocate( textlen + 1 ) );
	if( !text )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	memcpy( text, vtmp.ToConstChar(), textlen );
	text[textlen] = '\0';

	tag->Set( text, textlen );

	// Добавление элемента в массив.
	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = Pos - 1;

	aIsMultiple = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseDictRef( const char * aString, unsigned int aStringLength,
											  unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
											  nMemoryAllocator & aAllocator, bool & aIsMutiple )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Ключевое слово.
	if( pos + 8 < aStringLength && !strncasecmp( aString + pos, "dict-ref", 8 ) )
		pos = TextFuncs::SkipSpaces( aString, pos + 8, aStringLength );
	else if( pos + 5 < aStringLength && !strncasecmp( aString + pos, "&dict", 5 ) )
		pos = TextFuncs::SkipSpaces( aString, pos + 5, aStringLength );
	else
		return INF_ENGINE_WARN_UNSUCCESS;

	// Открывающая скобка.
	if( '(' != aString[pos] )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Считывание номера ссылки.
	unsigned int end = pos + 1;
	int ref_number = TextFuncs::GetInteger( aString, aStringLength, end );
	if( pos == end )
		ReturnWithParseError( DLErrorId::peDictRefInvalidNumber, pos );

	if( ref_number > vMinQstDictsNumber )
	{
		if( ref_number > vMaxQstDictsNumber )
			ReturnWithParseError( DLErrorId::peDictRefNumberIsTooBig, pos );
		else
			AddParseWarning( DLErrorId::peDictRefNumberMayBeTooBig, aPos );
	}

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, end, aStringLength );
	// Запятая.
	if( ',' != aString[pos] )
		ReturnWithParseError( DLErrorId::peDictRefCommaExpected, pos );
	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Начало описания типа ссылки.
	unsigned int begin = pos;
	// Конец описания типа ссылки.
	end = pos;
	while( end < aStringLength && TextFuncs::IsAlpha( aString[end] ) )
		++end;

	DictRef::RefType ref_type;
	if( !strncasecmp( "name", aString + begin, end - begin ) )
		ref_type = DictRef::RT_NAME;
	else if( !strncasecmp( "value", aString + begin, end - begin ) )
		ref_type = DictRef::RT_VALUE;
	else if( !strncasecmp( "origin", aString + begin, end - begin ) )
		ref_type = DictRef::RT_ORIGIN;
	else if( !strncasecmp( "pos", aString + begin, end - begin ) )
		ref_type = DictRef::RT_POS;
	else if( !strncasecmp( "norm", aString + begin, end - begin ) )
		ref_type = DictRef::RT_NORM;
	else
		ReturnWithParseError( DLErrorId::peDictRefInvalidType, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

	// Закрывающая круглая скобка.
	if( ')' != aString[pos] )
		ReturnWithParseError( DLErrorId::peDictRefWithoutCloseBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Закрывающая квадратная скобка.
	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peDictRefWithoutCloseSquareBrace, pos );

	// Выделение памяти.
	InfPatternItems::Base * tag = nullptr;
	tag = nAllocateObject( aAllocator, InfPatternItems::DictRef );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::DictRef;

	// Запись результата.
	static_cast<InfPatternItems::DictRef *>( tag )->Set( ref_number, ref_type );
	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = true;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseReference( const char * aString, unsigned int aStringLength,
												unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
												nMemoryAllocator & aAllocator, bool & aIsMutiple )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos >= aStringLength || aString[pos] != '&' )
		return INF_ENGINE_WARN_UNSUCCESS;
	if( pos + 1 >= aStringLength || aString[pos + 1] < '1' || aString[pos + 1] > '9' )
		return INF_ENGINE_WARN_UNSUCCESS;

	unsigned int refnum = aString[pos + 1] - '1';

	InfPatternItems::EliReference::Binding Binding = InfPatternItems::EliReference::bUserRequest;
	if( vEllipsisMode )
	{
		if( aString[pos + 2] == '-' )
		{
			if( aString[pos + 3] == '-' )
			{
				Binding = InfPatternItems::EliReference::bUserPreRequest;
				pos += 2;
			}
			else
			{
				Binding = InfPatternItems::EliReference::bInfPreResponse;
				pos += 1;
			}
		}
	}


	pos = TextFuncs::SkipSpaces( aString, pos + 2, aStringLength );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peReferenceInvalid, pos );

	InfPatternItems::Base * tag = nullptr;
	if( vEllipsisMode )
	{
		// Выделение памяти.
		tag = nAllocateObject( aAllocator, InfPatternItems::EliReference );
		if( !tag )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::EliReference;

		static_cast<InfPatternItems::EliReference *>( tag )->Set( Binding, refnum );
	}
	else
	{
		tag = nAllocateObject( aAllocator, InfPatternItems::Reference );
		if( !tag )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::Reference;

		static_cast<InfPatternItems::Reference *>( tag )->Set( refnum );
	}

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = true;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseExternalService( const char * aString, unsigned int aStringLength,
													  unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
													  nMemoryAllocator & aAllocator, bool & aIsMultiple )
{
	// Тэг разбирается но полностью игнорируется.

	bool ArgIsMultiple;

	// Проверка аргументов.
	if( !aString || aPos >= aStringLength )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Разбор внешнего сервиса.
	unsigned int pos = TextFuncs::SkipSpaces( aString, aPos );
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 1 );

	if( strncmp( "ExternalService", aString + pos, 15 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 15 );

	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peExternalServiceOpenRoundBrace, pos );

	// Аргументы запроса.
	InfPatternItems::Array * rubric = nullptr;
	InfPatternItems::Array * filter = nullptr;
	InfPatternItems::Array * keywords = nullptr;
	const char * flags = nullptr;

	const char * ServiceName = nullptr;
	unsigned int ServiceNameLength = 0;

	while( pos < aStringLength )
	{
		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos + 1 );

		if( aString[pos] == ')' )
			break;

		// Считываем имя параметра.
		unsigned int p;
		for( p = pos; p < aStringLength && TextFuncs::IsAlpha( aString[p] ); p++ ) ;
		if( p >= aStringLength )
			ReturnWithParseError( DLErrorId::peExternalServiceCloseSquareBrace, pos );

		// Пропускаем пробелы.
		unsigned int end = TextFuncs::SkipSpaces( aString, p );
		if( aString[end] != '=' )
			ReturnWithParseError( DLErrorId::peExternalServiceArgumentEqual, pos );

		end = TextFuncs::SkipSpaces( aString, end + 1 );

		if( aString[end] != '"' )
			ReturnWithParseError( DLErrorId::peExternalServiceArgumentQuote, pos );

		end++;

		if( p - pos == 7 && !strncasecmp( "service", aString + pos, 7 ) )
		{
			// Параметр - имя сервиса.
			pos = end;

			// Выделение имени сервиса.
			for( p = pos; p < aStringLength && TextFuncs::IsAlpha( aString[p] ); p++ ) ;
			if( p >= aStringLength )
				ReturnWithParseError( DLErrorId::peExternalServiceCloseSquareBrace, pos );

			// Пропускаем пробелы.
			unsigned int end = TextFuncs::SkipSpaces( aString, p );
			if( aString[end] != '"' )
				ReturnWithParseError( DLErrorId::peExternalServiceArgumentQuote, pos );

			// Проверка имени сервиса.
			if( ServiceName )
				ReturnWithParseError( DLErrorId::peExternalServiceArgumentDuplicate, pos );

			// Выделение имени сервиса.
			ServiceName = aString + pos;
			ServiceNameLength = p - pos;

			// Проверка имени модели.
			if( !( ServiceNameLength == 8 && !strncasecmp( ServiceName, "superinf", 8 ) ) &&
				!( ServiceNameLength == 4 && !strncasecmp( ServiceName, "dict", 4 ) ) &&
				!( ServiceNameLength == 7 && !strncasecmp( ServiceName, "bashorg", 7 ) ) )
				ReturnWithParseError( DLErrorId::peExternalServiceUnregistredService, pos );

			pos = end;
		}
		else if( p - pos == 8 && !strncasecmp( "keywords", aString + pos, 8 ) )
		{
			// Параметр - ключевые слова.
			pos = end;

			// Запоминаем размер массива с элементами.
			unsigned int aItemsSize = aResult.size();

			// Разбор значения.
			for(; pos < aStringLength; pos++ )
			{
				if( aString[pos] == '"' )
					break;
				else if( aString[pos] == '[' )                                  // Управляющий символ.
				{
					// Переменная.
					InfEngineErrors iee = ParseVarAndInstruct( aString, aStringLength, pos,
															   aResult, aAllocator, false, ArgIsMultiple );
					if( iee == INF_ENGINE_SUCCESS )
						continue;
					else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
						return iee;
					else if( iee != INF_ENGINE_WARN_UNSUCCESS )
						ReturnWithTrace( iee );

					// Звездочка.
					iee = ParseTagStar( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
					if( iee == INF_ENGINE_SUCCESS )
						continue;
					else if( iee != INF_ENGINE_WARN_UNSUCCESS )
						ReturnWithTrace( iee );

					ReturnWithParseError( DLErrorId::peExternalServiceArgumentRestrictedTag, pos );
				}
				else if( TextFuncs::IsSpace( aString[pos] ) )
				{
					// Пробельный символ пропускаем.
				}
				else
				{
					// Выделение текста.
					InfEngineErrors iee = ParseText( aString, aStringLength, pos, "[\"",
													 aResult, aAllocator, false, ArgIsMultiple );
					if( iee == INF_ENGINE_SUCCESS )
						continue;
					else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
						return iee;
					else
						ReturnWithTrace( iee );
				}
			}

			if( keywords )
				ReturnWithParseError( DLErrorId::peExternalServiceArgumentDuplicate, pos );

			// Выделение памяти.
			keywords = nAllocateObject( aAllocator, InfPatternItems::Array );
			if( !keywords )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			new((nMemoryAllocatorElementPtr *)keywords ) InfPatternItems::Array;

			if( aResult.size() - aItemsSize )
			{
				nAllocateNewObjects( aAllocator, InfPatternItems::Base *, aResult.size() - aItemsSize, items );
				if( !items )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				memcpy( items, aResult.get_buffer() + aItemsSize,
						( aResult.size() - aItemsSize ) * sizeof( InfPatternItems::Base * ) );

				keywords->Set( items, aResult.size() - aItemsSize );

				aResult.resize( aItemsSize );
			}
		}
		else if( p - pos == 6 && !strncasecmp( "rubric", aString + pos, 6 ) )
		{
			// Параметр - имя рубрики.
			pos = end;

			// Запоминаем размер массива с элементами.
			unsigned int aItemsSize = aResult.size();

			// Разбор значения.
			for(; pos < aStringLength; pos++ )
			{
				if( aString[pos] == '"' )
					break;
				else if( aString[pos] == '[' )                                  // Управляющий символ.
				{
					// Переменная.
					InfEngineErrors iee = ParseVarAndInstruct( aString, aStringLength, pos,
															   aResult, aAllocator, false, ArgIsMultiple );
					if( iee == INF_ENGINE_SUCCESS )
						continue;
					else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
						return iee;
					else if( iee != INF_ENGINE_WARN_UNSUCCESS )
						ReturnWithTrace( iee );

					// Звездочка.
					iee = ParseTagStar( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
					if( iee == INF_ENGINE_SUCCESS )
						continue;
					else if( iee != INF_ENGINE_WARN_UNSUCCESS )
						ReturnWithTrace( iee );

					ReturnWithParseError( DLErrorId::peExternalServiceArgumentRestrictedTag, pos );
				}
				else if( TextFuncs::IsSpace( aString[pos] ) )
				{
					// Пробельный символ пропускаем.
				}
				else
				{
					// Выделение текста.
					InfEngineErrors iee = ParseText( aString, aStringLength, pos, "[\"",
													 aResult, aAllocator, false, ArgIsMultiple );
					if( iee == INF_ENGINE_SUCCESS )
						continue;
					else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
						return iee;
					else
						ReturnWithTrace( iee );
				}
			}

			if( rubric )
				ReturnWithParseError( DLErrorId::peExternalServiceArgumentDuplicate, pos );

			// Выделение памяти.
			rubric = nAllocateObject( aAllocator, InfPatternItems::Array );
			if( !rubric )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			new((nMemoryAllocatorElementPtr *)rubric ) InfPatternItems::Array;

			if( aResult.size() - aItemsSize )
			{
				nAllocateNewObjects( aAllocator, InfPatternItems::Base *, aResult.size() - aItemsSize, items );
				if( !items )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				memcpy( items, aResult.get_buffer() + aItemsSize,
						( aResult.size() - aItemsSize ) * sizeof( InfPatternItems::Base * ) );

				rubric->Set( items, aResult.size() - aItemsSize );

				aResult.resize( aItemsSize );
			}
		}
		else if( p - pos == 6 && !strncasecmp( "filter", aString + pos, 6 ) )
		{
			// Параметр - имя рубрики.
			pos = end;

			// Запоминаем размер массива с элементами.
			unsigned int aItemsSize = aResult.size();

			// Разбор значения.
			for(; pos < aStringLength; pos++ )
			{
				if( aString[pos] == '"' )
					break;
				else if( aString[pos] == '[' )                                  // Управляющий символ.
				{
					// Переменная.
					InfEngineErrors iee = ParseVarAndInstruct( aString, aStringLength, pos,
															   aResult, aAllocator, false, ArgIsMultiple );
					if( iee == INF_ENGINE_SUCCESS )
						continue;
					else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
						return iee;
					else if( iee != INF_ENGINE_WARN_UNSUCCESS )
						ReturnWithTrace( iee );

					// Звездочка.
					iee = ParseTagStar( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
					if( iee == INF_ENGINE_SUCCESS )
						continue;
					else if( iee != INF_ENGINE_WARN_UNSUCCESS )
						ReturnWithTrace( iee );

					ReturnWithParseError( DLErrorId::peExternalServiceArgumentRestrictedTag, pos );
				}
				else if( TextFuncs::IsSpace( aString[pos] ) )
				{
					// Пробельный символ пропускаем.
				}
				else
				{
					// Выделение текста.
					InfEngineErrors iee = ParseText( aString, aStringLength, pos, "[\"",
													 aResult, aAllocator, false, ArgIsMultiple );
					if( iee == INF_ENGINE_SUCCESS )
						continue;
					else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
						return iee;
					else
						ReturnWithTrace( iee );
				}
			}

			if( filter )
				ReturnWithParseError( DLErrorId::peExternalServiceArgumentDuplicate, pos );

			// Выделение памяти.
			filter = nAllocateObject( aAllocator, InfPatternItems::Array );
			if( !rubric )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			new((nMemoryAllocatorElementPtr *)filter ) InfPatternItems::Array( nullptr, 0 );

			if( aResult.size() - aItemsSize )
			{
				nAllocateNewObjects( aAllocator, InfPatternItems::Base *, aResult.size() - aItemsSize, items );
				if( !items )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				memcpy( items, aResult.get_buffer() + aItemsSize,
						( aResult.size() - aItemsSize ) * sizeof( InfPatternItems::Base * ) );

				filter->Set( items, aResult.size() - aItemsSize );

				aResult.resize( aItemsSize );
			}
		}
		else if( p - pos == 5 && !strncasecmp( "flags", aString + pos, 5 ) )
		{
			// Параметр - имя рубрики.
			pos = end;

			// Проверка повторного объявления флагов:
			if( flags )
				ReturnWithParseError( DLErrorId::peExternalServiceArgumentDuplicate, pos );

			if( !ServiceName )
				ReturnWithParseError( DLErrorId::peExternalServiceArgumentOrder,     pos );

			unsigned int argend = pos;
			for(; argend < aStringLength && aString[argend] != '"'; argend++ ) ;

			if( argend >= aStringLength )
				ReturnWithParseError( DLErrorId::peExternalServiceArgumentQuote, pos );

			if( !( ServiceNameLength == 7 && !strncasecmp( ServiceName, "bashorg", 7 ) ) )
				pos = argend;
			else
			{
				flags = aString + pos;

				pos = argend;
			}
		}
		else
			ReturnWithParseError( DLErrorId::peExternalServiceArgumentInvalid, pos );
	}

	// Пропускаем пробелы.
	pos = TextFuncs::SkipSpaces( aString, pos );

	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peExternalServiceCloseRoundBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1 );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peExternalServiceCloseSquareBrace,   pos );

	if( !ServiceName )
		ReturnWithParseError( DLErrorId::peExternalServiceInvalidServiceName, pos );

	aPos = pos;

	aIsMultiple = true;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagBR( const char * aString, unsigned int aStringLength,
											unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
											nMemoryAllocator & aAllocator, bool & aIsMutiple )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos + 2 >= aStringLength || strncasecmp( "br", aString + pos, 2 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 2, aStringLength );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peBrCloseSquareBrace, pos );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::Br, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::Br;

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagNothing( const char * aString, unsigned int aStringLength, unsigned int & aPos, avector<InfPatternItems::Base *> & aResult, nMemoryAllocator & aAllocator, bool & aIsMutiple )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos + 7 >= aStringLength || strncasecmp( "nothing", aString + pos, 7 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 7, aStringLength );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peNothingCloseSquareBrace, pos );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::Nothing, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::Nothing;

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagDisableautovars( const char * aString, unsigned int aStringLength, unsigned int & aPos, avector<InfPatternItems::Base *> & aResult, nMemoryAllocator & aAllocator, bool & aIsMutiple )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos + 15 >= aStringLength || strncasecmp( "disableautovars", aString + pos, 15 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 15, aStringLength );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peDisableautovarsCloseSquareBrace, pos );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::Disableautovars, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::Disableautovars;

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagBreak( const char * aString, unsigned int aStringLength,
											   unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
											   nMemoryAllocator & aAllocator, bool & aIsMutiple )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos + 5 >= aStringLength || strncasecmp( "break", aString + pos, 5 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 5, aStringLength );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peBreakCloseSquareBrace, pos );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::TagBreak, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagBreak;

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseVarAndInstruct( const char * aString, unsigned int aStringLength,
													 unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
													 nMemoryAllocator & aAllocator, bool aAllowInstruct, bool & aIsMutiple )
{
	bool IsMutiple = false;
	bool ArgIsMultiple;

	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( aString[pos] != '%' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Выделение имени переменной.
	unsigned int end = GetVarName( aString, aStringLength, pos + 1 );
	if( end == pos )
		ReturnWithParseError( DLErrorId::peVariableName, pos );

	nlReturnCode nle = vtmp.assign( aString + pos + 1, end - pos - 1 );
	if( nle != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	AllTrim( vtmp );
	if( SetLower( vtmp ) != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Поиск переменной в реестре.
	Vars::Id VarId;
	InfEngineErrors iee = CheckVariable( vtmp.ToConstChar(), vtmp.size(), VarId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithParseError( DLErrorId::peVariableUnregistred, aPos );

	pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

	if( pos < aStringLength && aString[pos] == ']' )
	{
		// Разбор использования значения переменной.

		// Выделение памяти.
		InfPatternItems::TagVar * tag = nAllocateObject( aAllocator, InfPatternItems::TagVar );
		if( !tag )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new( (nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagVar;

		// Установка идентификатора.
		tag->Set( VarId );

		aResult.push_back( tag );
		if( aResult.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		aPos = pos;

		aIsMutiple = IsMutiple;

		return INF_ENGINE_SUCCESS;
	}
	else if( !aAllowInstruct )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Выделение памяти.
	InfPatternItems::Instruct * tag = nAllocateObject( aAllocator, InfPatternItems::Instruct );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::Instruct;

	if( pos >= aStringLength || aString[pos] != '=' )
		ReturnWithParseError( DLErrorId::peInstructEqual, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( pos >= aStringLength || aString[pos] != '"' )
		ReturnWithParseError( DLErrorId::peInstructQuote, pos );

	pos++;

	// Запоминаем размер массива с элементами.
	unsigned int aItemsSize = aResult.size();

	// Разбор значения.
	for(; pos < aStringLength; pos++ )
	{
		if( aString[pos] == '"' )
			break;
		else if( aString[pos] == '[' )                  // Управляющий символ.
		{
			InfEngineErrors iee = ParseDictRef( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			iee = ParseReference( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			iee = ParseVarAndInstruct( aString, aStringLength, pos, aResult, aAllocator, false, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			iee = ParseTagStar( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			ReturnWithParseError( DLErrorId::peInstructValue, pos );
		}
		else if( TextFuncs::IsSpace( aString[pos] ) )
		{
			// Пробельный символ пропускаем.
		}
		else
		{
			// Выделение текста.
			InfEngineErrors iee = ParseText( aString, aStringLength, pos, "[\"", aResult, aAllocator, false, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else
				ReturnWithTrace( iee );
		}
	}

	if( pos >= aStringLength || aString[pos] != '"' )
		ReturnWithParseError( DLErrorId::peInstructQuote, pos );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::Array, array );
	if( !array )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)array ) InfPatternItems::Array( nullptr, 0 );

	if( aResult.size() - aItemsSize )
	{
		nAllocateNewObjects( aAllocator, InfPatternItems::Base *, aResult.size() - aItemsSize, items );
		if( !items )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
							 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		memcpy( items, aResult.get_buffer() + aItemsSize,
				( aResult.size() - aItemsSize ) * sizeof( InfPatternItems::Base * ) );

		array->Set( items, aResult.size() - aItemsSize );

		aResult.resize( aItemsSize );
	}

	tag->Set( VarId, array );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( pos >= aStringLength || aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peInstructCloseSquareBrace, pos );

	if( vDLDataWR.GetVarsRegistry().GetProperties( VarId ) > Vars::Options::ReadOnly )
		ReturnWithParseError( DLErrorId::peInstructReadOnlyVariable, aPos );

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagInf( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											 avector<InfPatternItems::Base*> & aWaitingForResolving,
											 avector<InfPatternItems::Base *> & aResult, nMemoryAllocator & aAllocator, bool & aIsMutiple, bool aIsFunctionArg ) {
	unsigned int pos { aPos };

	aIsMutiple = false;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( pos + 3 >= aStringLength || strncasecmp( aString + pos, "inf", 3 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 3, aStringLength );

	unsigned int ResultSize = aResult.size();
	if( aString[pos] == '(' ) {
		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

		if( aString[pos] != '"' )
			ReturnWithParseError( DLErrorId::peInfOpenQuote, pos );

		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

		do {
			bool ArgIsMultiple { false };
			auto iee = ParseFunctionExtendedArg( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, '"', '"', DLFAT_DYNAMIC, ArgIsMultiple, aIsFunctionArg );
			if( iee == INF_ENGINE_SUCCESS )
				aIsMutiple |= ArgIsMultiple;
			else {
				aResult.resize( ResultSize );

				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithErrorStrict( iee, "Failed to parse argument for tag inf" );
				else if( iee == INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

				ReturnWithTrace( iee );
			}
			pos++;
		} while( pos < aStringLength && aString[pos] != '"' );

		if( aString[pos] != '"' )
			ReturnWithParseError( DLErrorId::peInfCloseQuote, pos );

		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

		if( aString[pos] != ')' )
			ReturnWithParseError( DLErrorId::peInfInvalidRequest, pos );

		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

		// Проверка request'а на пустоту.
		if( aResult.size() == ResultSize )
			ReturnWithParseError( DLErrorId::peInfEmptyRequest, pos );
	}

	// Выделение памяти под request.
	nAllocateNewObject( aAllocator, InfPatternItems::Array, request );
	if( !request )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
	new( (nMemoryAllocatorElementPtr *) request ) InfPatternItems::Array{ nullptr, 0 };

	nAllocateNewObjects( aAllocator, InfPatternItems::Base *, aResult.size() - ResultSize, items );
	if( aResult.size() != ResultSize && !items )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
	memcpy( items, aResult.get_buffer() + ResultSize, sizeof( InfPatternItems::Base * ) * ( aResult.size() - ResultSize ) );

	request->Set( items, aResult.size() - ResultSize );
	aResult.resize( ResultSize );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peInfCloseSquareBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( aString[pos] != '{' )
		ReturnWithParseError( DLErrorId::peInfOpenBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Выделение значения.
	do
	{
		bool ArgIsMultiple = false;
		auto iee = ParseFunctionExtendedArg( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, '{', '}', DLFAT_DYNAMIC, ArgIsMultiple, aIsFunctionArg );
		if( iee == INF_ENGINE_SUCCESS )
			aIsMutiple |= ArgIsMultiple;
		else
		{
			aResult.resize( ResultSize );

			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				ReturnWithErrorStrict( iee, "Failed to parse argument for tag inf" );
			else if( iee == INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

			ReturnWithTrace( iee );
		}
		pos++;
	} while( pos < aStringLength && aString[pos] != '}' );

	if( aString[pos] != '}' )
		ReturnWithParseError( DLErrorId::peInfCloseBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка значения на пустоту.
	if( aResult.size() == ResultSize )
		ReturnWithParseError( DLErrorId::peInfValue, pos );

	// Выделение памяти под значение.
	nAllocateNewObject( aAllocator, InfPatternItems::Array, string );
	if( !string )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
	new( (nMemoryAllocatorElementPtr *) string ) InfPatternItems::Array{ nullptr, 0 };

	items = nAllocateObjects( aAllocator, InfPatternItems::Base *, aResult.size() - ResultSize );
	if( aResult.size() != ResultSize && !items )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
	memcpy( items, aResult.get_buffer() + ResultSize, sizeof( InfPatternItems::Base * ) * ( aResult.size() - ResultSize ) );

	string->Set( items, aResult.size() - ResultSize );
	aResult.resize( ResultSize );

	// Выделение памяти под тэг.
	nAllocateNewObject( aAllocator, InfPatternItems::TagInf, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagInf;

	// Выделение памяти под запрос.
	char * text = static_cast<char *>( aAllocator.Allocate( vtmp.size() + 1 ) );
	if( !text )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	tag->Set( string->GetItemsNumber() ? string : nullptr, request->GetItemsNumber() ? request : nullptr );

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos - 1;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagHref( const char * aString, unsigned int aStringLength, unsigned int &aPos, avector<InfPatternItems::Base *> & aResult,
											  avector<InfPatternItems::Base *> & aWaitingForResolving, nMemoryAllocator & aAllocator, bool &aIsMutiple )
{
	bool IsMutiple = false;

	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( pos + 4 >= aStringLength || strncasecmp( aString + pos, "href", 4 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 4, aStringLength );

	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peHrefOpenRoundBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Выделение URLа.
	unsigned int itemsnum = aResult.size();
	for(; pos < aStringLength; pos++ )
	{
		// Конец URL'а.
		bool ArgIsMutable;
		if( aString[pos] == ')' )
			break;
		else if( aString[pos] == '[' )                  // Управляющий символ.
		{
			// Ссылка на словарь.
			InfEngineErrors iee = ParseDictRef( aString, aStringLength, pos, aResult, aAllocator, ArgIsMutable );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMutable;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			// Референция.
			iee = ParseReference( aString, aStringLength, pos, aResult, aAllocator, ArgIsMutable );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMutable;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			// Переменная.
			iee = ParseVarAndInstruct( aString, aStringLength, pos, aResult, aAllocator, false, ArgIsMutable );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMutable;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			// Звездочка.
			iee = ParseTagStar( aString, aStringLength, pos, aResult, aAllocator, ArgIsMutable );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMutable;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				return iee;

			iee = ParseTagFunction( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, ArgIsMutable, false );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMutable;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				return iee;

			ReturnWithParseError( DLErrorId::peHrefURL, pos );
		}
		else if( TextFuncs::IsSpace( aString[pos] ) )
		{
			// Пробельный символ пропускаем.
		}
		else
		{
			// Выделение текста.
			unsigned int begin = pos;
			InfEngineErrors iee = ParseText( aString, aStringLength, pos, "[)",
											 aResult, aAllocator, false, ArgIsMutable );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMutable;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else
				ReturnWithTrace( iee );
		}
	}

	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peHrefCloseRoundBrace, pos );


	// Проверка URL'а на пустоту.
	if( aResult.size() == itemsnum )
		ReturnWithParseError( DLErrorId::peHrefURL, pos );

	// Выделение памяти под URL.
	nAllocateNewObject( aAllocator, InfPatternItems::Array, url );
	if( !url )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
	new((nMemoryAllocatorElementPtr *)url ) InfPatternItems::Array( nullptr, 0 );

	nAllocateNewObjects( aAllocator, InfPatternItems::Base *, aResult.size() - itemsnum, items );
	if( aResult.size() != itemsnum && !items )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );
	memcpy( items, aResult.get_buffer() + itemsnum, sizeof( InfPatternItems::Base * ) * ( aResult.size() - itemsnum ) );

	url->Set( items, aResult.size() - itemsnum );
	aResult.resize( itemsnum );
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Выделение target'а.
	char * target = nullptr;
	unsigned int targetlen = 0;
	if( aString[pos] == '(' )
	{
		unsigned int end = GetText( aString, aStringLength, pos, '(', ')' );
		if( pos == end )
			ReturnWithParseError( DLErrorId::peHrefTarget, pos );

		if( vtmp.assign( aString + pos + 1, end - pos - 2 ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		RemoveEscapeSymbols( vtmp );
		AllTrim( vtmp );

		target = static_cast<char *>( aAllocator.Allocate( vtmp.size() + 1 ) );
		if( !target )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		memcpy( target, vtmp.ToConstChar(), vtmp.size() );
		target[vtmp.size()] = '\0';
		targetlen = vtmp.size();

		pos = TextFuncs::SkipSpaces( aString, end, aStringLength );
	}

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peHrefCloseSquareBrace, pos );

	// Проверка на возможное отстутсвие закрывающей круглой скобки после первого аргумента тэга href,
	// которое приводит к интерпретации воторого аргумента как конца первого.
	if( !target )
	{
		// Поиск конца второго аргумента.
		unsigned int end = pos;
		while( end > aPos && aString[end] != ')' )
			--end;
		while( end > aPos && TextFuncs::IsSpace( aString[end - 1] ) )
			--end;

		// Поиск начала второго аргумента.
		unsigned int begin = end;
		while( begin > aPos && aString[begin - 1] != '(' )
			--begin;
		while( begin < aStringLength && TextFuncs::IsSpace( aString[begin] ) )
			++begin;

		// Проверка значения второго аргумента.
		if( !strncasecmp( aString + begin, "_blank", end - begin ) ||
			!strncasecmp( aString + begin, "_self", end - begin ) ||
			!strncasecmp( aString + begin, "_parent", end - begin ) ||
			!strncasecmp( aString + begin, "_top", end - begin ) ||
			!strncasecmp( aString + begin, "link_target", end - begin ) )
			AddParseWarning( DLErrorId::peMaybeCloseRoundBraceMissed, begin );
	}

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( aString[pos] != '{' )
		ReturnWithParseError( DLErrorId::peHrefOpenCurlyBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Выделение Link'а.
	itemsnum = aResult.size();
	for(; pos < aStringLength; pos++ )
	{
		// Конец URL'а.
		bool ArgIsMultiple;
		if( aString[pos] == '}' )
			break;
		else if( aString[pos] == '[' )                  // Управляющий символ.
		{
			// Ссылка на словарь.
			InfEngineErrors iee = ParseDictRef( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			// Референция.
			iee = ParseReference( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			// Переменная.
			iee = ParseVarAndInstruct( aString, aStringLength, pos, aResult, aAllocator, false, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			// Звездочка.
			iee = ParseTagStar( aString, aStringLength, pos, aResult, aAllocator, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				ReturnWithTrace( iee );

			// Функция.
			iee = ParseTagFunction( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, ArgIsMultiple, false );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;
				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else if( iee != INF_ENGINE_WARN_UNSUCCESS )
				return iee;

			ReturnWithParseError( DLErrorId::peHrefLink, pos );
		}
		else if( aString[pos] == ' ' )
		{
			// Пробельный символ пропускаем.
		}
		else
		{
			// Выделение текста.
			unsigned int begin = pos;
			InfEngineErrors iee = ParseText( aString, aStringLength, pos, "[}",
											 aResult, aAllocator, false, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
			{
				IsMutiple |= ArgIsMultiple;

				// Проверка на недопустимое использование инлайн-словаря в теле тэга href.
				for( unsigned int n = begin; n < pos; ++n )
					if( '{' == aString[n] )
					{
						AddParseWarning( DLErrorId::peMaybeInlineDictUsed, begin );
						break;
					}

				continue;
			}
			else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
				return iee;
			else
				ReturnWithTrace( iee );
		}
	}

	if( aString[pos] != '}' )
		ReturnWithParseError( DLErrorId::peHrefCloseCurlyBrace, pos );

	// Выделение памяти под Link.
	nAllocateNewObject( aAllocator, InfPatternItems::Array, link );
	if( !link )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)link ) InfPatternItems::Array( nullptr, 0 );

	items = nAllocateObjects( aAllocator, InfPatternItems::Base *, aResult.size() - itemsnum );
	if( aResult.size() != itemsnum && !items )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( items, aResult.get_buffer() + itemsnum, sizeof( InfPatternItems::Base * ) * ( aResult.size() - itemsnum ) );

	link->Set( items, aResult.size() - itemsnum );

	aResult.resize( itemsnum );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::TagHref, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagHref;

	tag->Set( url, target, targetlen, link );

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = IsMutiple;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagOpenWindow( const char * aString, unsigned int aStringLength,
													unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
													nMemoryAllocator & aAllocator, bool & aIsMutiple )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( pos + 4 >= aStringLength || strncasecmp( aString + pos, "openwindow", 10 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 10, aStringLength );

	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peOpenWindowOpenRoundBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Выделение URLа.
	avector<InfPatternItems::Base *> Arguments;
	avector<InfPatternItems::Base *> aWaitingForResolving;

	if( ( iee = ParseTagFunctionArg( aString, aStringLength, pos, Arguments, aWaitingForResolving, aAllocator, '"', '"', DLFAT_ANY,
									 aIsMutiple, false, false ) ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Проверка окончания URL.
	if( aString[pos] != '"' )
		ReturnWithParseError( DLErrorId::peOpenWindowURL, pos );
	++pos;

	pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );

	unsigned int end = pos;
	// Выделение Target'а.
	int OpenType = InfPatternItems::TagOpenWindow::OPEN_IN_NEW_WINDOW;
	if( aString[pos] == ',' )
	{
		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

		end = GetText( aString, aStringLength, pos, '"', '"' );
		if( pos == end )
			ReturnWithParseError( DLErrorId::peOpenWindowTarget, pos );

		nlReturnCode nle = vtmp.assign( aString + pos + 1, end - pos - 2 );
		if( nle != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Нормализация строки.
		RemoveEscapeSymbols( vtmp );
		AllTrim( vtmp );

		if( !strcasecmp( vtmp.ToConstChar(), "new" ) )
			OpenType = InfPatternItems::TagOpenWindow::OPEN_IN_NEW_WINDOW;
		else if( !strcmp( vtmp.ToConstChar(), "parent" ) )
			OpenType = InfPatternItems::TagOpenWindow::OPEN_IN_PARENT_WINDOW;
		else
			ReturnWithParseError( DLErrorId::peOpenWindowTarget, pos );
	}

	pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peOpenWindowCloseRoundBrace, pos );

	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peOpenWindowCloseSquareBrace, pos );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::TagOpenWindow, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagOpenWindow;

	InfPatternItems::Base ** url = nAllocateObjects( aAllocator, InfPatternItems::Base *, Arguments.size() );
	if( !url )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( url, Arguments.get_buffer(), sizeof( InfPatternItems::Base * ) * Arguments.size() );
	tag->Set( url, Arguments.size(), OpenType );

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = false;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagElsif( const char * aString, unsigned int aStringLength,
											   unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
											   avector<InfPatternItems::Base *> & aWaitingForResolving,
											   nMemoryAllocator & aAllocator, bool & aIsMultiple, bool aIsFunctionArg )
{
	unsigned int pos = aPos;
	// Соханение размера массива с результатами.
	unsigned int ResultSize = aResult.size();

	// Выделение памяти под аргумент функции.
	nAllocateNewObject( aAllocator, InfPatternItems::TagFunction::Argument, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagFunction::Argument;

	// Парсинг ветки elsif.
	bool IsMultiple = false;
	InfEngineErrors iee = ParseTagIf( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, IsMultiple, aIsFunctionArg, true );
	if( INF_ENGINE_SUCCESS != iee )
		return iee;

	// Упаковка результата в тэг TagFunction::Argument.
	unsigned int items_count = aResult.size() - ResultSize;
	if( items_count )
	{
		// Копируем элементы.
		InfPatternItems::Base ** items = nAllocateObjects( aAllocator, InfPatternItems::Base *, items_count );
		if( !items )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( items, aResult.get_buffer() + ResultSize, items_count * sizeof( InfPatternItems::Base * ) );
		tag->SetItems( items, items_count );
	}
	else
		tag->SetItems( nullptr, 0 );

	// Удаляем из результата временно сохранённые в нём элементы.
	aResult.resize( ResultSize );

	if( pos >= aStringLength )
		return INF_ENGINE_WARN_UNSUCCESS;

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMultiple = IsMultiple;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagSwitch( const char * aString, unsigned int aStringLength,
												unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
												avector<InfPatternItems::Base *> & aWaitingForResolving,
												nMemoryAllocator & aAllocator, bool & aIsMutiple, bool aIsFunctionArg )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;
	bool IsMultiple = false;
	bool ArgIsMultiple;

	avector<InfPatternItems::Base *> blocks;

	unsigned int pos = TextFuncs::SkipSpaces( aString, aPos, aStringLength );

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка на ключевое слово switch.
	if( pos + 6 >= aStringLength || strncasecmp( aString + pos, "switch", 6 ) )
		return INF_ENGINE_WARN_UNSUCCESS;
	pos = TextFuncs::SkipSpaces( aString, pos + 6, aStringLength );

	// Проверка на начало аргумента switch.
	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peSwitchOpenRoundBrace, pos );

	// Парсинг аргумента switch.
	if( ( iee = ParseTagFunctionArg( aString, aStringLength, pos, blocks, aWaitingForResolving, aAllocator, '(', ')', DLFAT_EXTENDED,
									 ArgIsMultiple, aIsFunctionArg, false ) ) != INF_ENGINE_SUCCESS )
		return iee;
	IsMultiple |= ArgIsMultiple;

	// Проверка корректности аргумента switch.
	if( blocks.size() < 1 || static_cast<const TagFunction::Argument *>( blocks[0] )->GetItemsCount() != 1 )
		ReturnWithParseError( DLErrorId::peSwitchTooManyArgs, aPos );

	// Проверка окончания аргумента switch.
	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peSwitchCloseRoundBrace, pos );
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка окончания тэга switch.
	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peSwitchCloseSquareBrace, pos );
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Парсинг блоков case и default.
	bool default_found = false;
	while( !default_found ) {
		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );

		// Позиция начала очередного блока.
		unsigned int begin_pos = pos;

		// Проверка на открывающуюся квадратную скобку.
		if( aString[pos] != '[' )
			ReturnWithParseError( DLErrorId::peDefaultBlockExpected, pos );
		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

		// Определение типа блока.
		bool case_block = true;
		if( pos + 4 < aStringLength && !strncasecmp( aString + pos, "case", 4 ) )
		{
			pos = TextFuncs::SkipSpaces( aString, pos + 4, aStringLength );
		}
		else if( pos + 7 < aStringLength && !strncasecmp( aString + pos, "default", 7 ) )
		{
			case_block = false;
			pos = TextFuncs::SkipSpaces( aString, pos + 7, aStringLength );
		}
		else
			ReturnWithParseError( DLErrorId::peSwitchBlockExpected, pos );

		//
		if( case_block )
		{
			// Проверка на начало аргумента case.
			if( aString[pos] != '(' )
				ReturnWithParseError( DLErrorId::peCaseOpenRoundBrace, pos );

			// Парсинг аргумента case.
			unsigned int prev_size = blocks.size();
			if( ( iee = ParseTagFunctionArg( aString, aStringLength, pos, blocks, aWaitingForResolving, aAllocator, '(', ')', DLFAT_EXTENDED,
											 ArgIsMultiple, aIsFunctionArg, false ) ) != INF_ENGINE_SUCCESS )
				return iee;
			IsMultiple |= ArgIsMultiple;
			// Проверка корректности аргумента case.
			if( blocks.size() - prev_size != 1 || static_cast<const TagFunction::Argument *>( blocks[blocks.size() - 1] )->GetItemsCount() != 1 )
				ReturnWithParseError( DLErrorId::peCaseTooManyArgs, begin_pos );

			// Проверка окончания аргумента case.
			if( aString[pos] != ')' )
				ReturnWithParseError( DLErrorId::peCaseCloseCurlyBrace, pos );
			pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

			// Проверка окончания тэга case.
			if( aString[pos] != ']' )
				ReturnWithParseError( DLErrorId::peCaseCloseSquareBrace, pos );
			pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
		}
		else
		{
			// Проверка окончания тэга default.
			if( aString[pos] != ']' )
				ReturnWithParseError( DLErrorId::peDefaultCloseSquareBrace, pos );
			pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

			// Это последний блок.
			default_found = true;
		}

		// Проверка на начало тела блока.
		if( aString[pos] != '{' )
			ReturnWithParseError( DLErrorId::peSwitchOpenRoundBrace, pos );

		if( ( iee = ParseTagFunctionArg( aString, aStringLength, pos, blocks, aWaitingForResolving, aAllocator, '{', '}', DLFAT_IF,
										 ArgIsMultiple, aIsFunctionArg, false ) ) != INF_ENGINE_SUCCESS )
			return iee;
		IsMultiple |= ArgIsMultiple;

		// Проверка окончания тела блока.
		if( aString[pos] != '}' )
			ReturnWithParseError( DLErrorId::peCaseCloseCurlyBrace, pos );
		pos += 1;
	}

	// Проверка количества разобранных блоков.
	if( blocks.size() < 2 || ( blocks.size() & 1 ) )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Проверка типа разобранных блоков.
	for( unsigned int block_n = 0; block_n < blocks.size(); ++block_n )
		if( InfPatternItems::itFunctionArg != blocks[block_n]->GetType() )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Создание тэга switch.
	InfPatternItems::TagSwitch * tag = nAllocateObject( aAllocator, InfPatternItems::TagSwitch );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagSwitch;

	// Заполнение тэга разобранными данными.
	const TagFunction::Argument * switch_arg = static_cast<TagFunction::Argument *>( blocks[0] );
	const TagFunction::Argument * default_body = static_cast<TagFunction::Argument *>( blocks[blocks.size() - 1] );
	unsigned int case_number = ( blocks.size() - 2 ) >> 1;

	const TagFunction::Argument ** case_arg = nAllocateObjects( aAllocator, const TagFunction::Argument *, case_number );
	const TagFunction::Argument ** case_body = nAllocateObjects( aAllocator, const TagFunction::Argument *, case_number );
	if( !case_arg || !case_body )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	for( unsigned int case_n = 0; case_n < case_number; ++case_n )
	{
		case_arg[case_n] = static_cast<TagFunction::Argument *>( blocks[1 + 2 * case_n] );
		case_body[case_n] = static_cast<TagFunction::Argument *>( blocks[2 + 2 * case_n] );
	}

	tag->Set( switch_arg, case_arg, case_body, case_number, default_body );

	// Добавление тэга switch к результатам разбора.
	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos - 1;

	aIsMutiple = IsMultiple;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagIf( const char * aString, unsigned int aStringLength,
											unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
											avector<InfPatternItems::Base *> & aWaitingForResolving,
											nMemoryAllocator & aAllocator, bool & aIsMutiple, bool aIsFunctionArg, bool aElseifBranch )
{
	bool IsMultiple = false;
	bool ArgIsMultiple;

	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( aElseifBranch )
	{
		if( pos + 5 >= aStringLength || strncasecmp( aString + pos, "elsif", 5 ) )
			return INF_ENGINE_WARN_UNSUCCESS;
		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos + 5, aStringLength );
	}
	else
	{
		if( pos + 2 >= aStringLength || strncasecmp( aString + pos, "if", 2 ) )
			return INF_ENGINE_WARN_UNSUCCESS;
		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos + 2, aStringLength );
	}

	// Проверка на начало условия.
	if( aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peIfOpenRoundBrace, pos );

	// Разбор условия.
	InfPatternItems::TagFunction * Condition;
	InfEngineErrors iee = ParseCondition( aString, aStringLength, pos, Condition, aAllocator, ArgIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_SUCCESS )
		return iee;
	IsMultiple |= ArgIsMultiple;

	// Проверка на окончание условия.
	if( aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peIfCloseRoundBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка окончания основной части тэга.
	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peIfCloseSquareBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Описание варианта, соответствующего верному условию.
	if( aString[pos] != '{' )
		ReturnWithParseError( DLErrorId::peIfOpenCurlyBrace, pos );

	// Парсинг аргумента "then".
	avector<InfPatternItems::Base *> Arguments;
	if( ( iee = ParseTagFunctionArg( aString, aStringLength, pos, Arguments, aWaitingForResolving, aAllocator, '{', '}', DLFAT_IF, ArgIsMultiple, aIsFunctionArg, false ) ) != INF_ENGINE_SUCCESS )
		return iee;
	IsMultiple |= ArgIsMultiple;

	// Проверка окончания варианта.
	if( aString[pos] != '}' )
		ReturnWithParseError( DLErrorId::peIfCloseCurlyBrace, pos );

	// Пропуск пробелов.
	unsigned int tpos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка на альтернативный вараинт.
	if( aString[tpos] == '[' )
	{
		// Пропуск пробелов.
		unsigned int end = TextFuncs::SkipSpaces( aString, tpos + 1, aStringLength );

		// Ветка elsif.
		if( end + 5 < aStringLength && !strncasecmp( aString + end, "elsif", 5 ) )
		{
			// Парсинг ветки Elseif.
			if( INF_ENGINE_SUCCESS != ( iee = ParseTagElsif( aString, aStringLength, tpos, Arguments, aWaitingForResolving, aAllocator, ArgIsMultiple, aIsFunctionArg ) ) )
				return iee;
			IsMultiple |= ArgIsMultiple;
			pos = tpos;

			// Проверка на символ завершения варианта.
			if( aString[pos] != '}' )
				ReturnWithParseError( DLErrorId::peIfCloseCurlyBrace, pos );

		}
		// Ветка else.
		else if( end + 4 < aStringLength && !strncasecmp( aString + end, "else", 4 ) )
		{
			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aString, end + 4, aStringLength );

			// Проверка на символ окончания тэга.
			if( aString[pos] != ']' )
				ReturnWithParseError( DLErrorId::peIfCloseSquareBrace, pos );

			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

			// Проверка на символ начала варианта.
			if( aString[pos] != '{' )
				ReturnWithParseError( DLErrorId::peIfOpenCurlyBrace, pos );

			// Парсинг аргумента "else".
			if( ( iee = ParseTagFunctionArg( aString, aStringLength, pos, Arguments, aWaitingForResolving, aAllocator, '{', '}', DLFAT_IF, ArgIsMultiple, aIsFunctionArg, false ) ) != INF_ENGINE_SUCCESS )
				return iee;
			IsMultiple |= ArgIsMultiple;

			// Проверка на символ завершения варианта.
			if( aString[pos] != '}' )
				ReturnWithParseError( DLErrorId::peIfCloseCurlyBrace, pos );
		}
	}

	InfPatternItems::TagIf * tag = nAllocateObject( aAllocator, InfPatternItems::TagIf );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new( (nMemoryAllocatorElementPtr*)tag ) InfPatternItems::TagIf { aPos };

	if( Arguments[0]->GetType() != InfPatternItems::itFunctionArg )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Wrong type of If operator Then branch" );

	if( Arguments.size() == 2 && Arguments[1]->GetType() != InfPatternItems::itFunctionArg )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Wrong type of If operator Else branch" );

	tag->Set( Condition, dynamic_cast<InfPatternItems::TagFunction::Argument *>( Arguments[0] ),
			  Arguments.size() == 2 ? dynamic_cast<InfPatternItems::TagFunction::Argument *>( Arguments[1] ) : nullptr );

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMutiple = IsMultiple;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagStar( const char * aString, unsigned int aStringLength,
											  unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
											  nMemoryAllocator & aAllocator, bool & aIsMultiple )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( aString[pos] == '*' )
	{
		pos++;

		// Проверка на указание номера звездоки.
		char StarNum = 1;
		if( '1' <= aString[pos] && aString[pos] <= '9' )
		{
			StarNum = aString[pos] - '0';
			pos++;
		}

		// Разбор указания фразы для шаблонов эллипсиса.
		InfPatternItems::EliStar::Binding Binding = InfPatternItems::EliStar::bUserRequest;
		if( vEllipsisMode )
		{
			if( aString[pos] == '-' )
			{
				if( aString[pos + 1] == '-' )
				{
					Binding = InfPatternItems::EliStar::bUserPreRequest;
					pos += 2;
				}
				else
				{
					Binding = InfPatternItems::EliStar::bInfPreResponse;
					pos += 1;
				}
			}
		}

		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );

		// Проверка на символ завершающий тэг.
		if( aString[pos] != ']' )
			ReturnWithParseError( DLErrorId::peStarCloseSquareBrace, pos );

		// Выделение памяти под элемент.
		InfPatternItems::Base * tag = nullptr;
		if( !vEllipsisMode )
		{
			tag = nAllocateObject( aAllocator, InfPatternItems::Star );
			if( !tag )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::Star;

			// Установка значения.
			static_cast<InfPatternItems::Star *>( tag )->Set( StarNum, false );
		}
		else
		{
			tag = nAllocateObject( aAllocator, InfPatternItems::EliStar );
			if( !tag )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::EliStar;

			// Установка значения.
			static_cast<InfPatternItems::EliStar *>( tag )->Set( Binding, StarNum, false );
		}

		// Добавление разобранного элемента в результат.
		aResult.push_back( tag );
		if( aResult.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		aPos = pos;

		aIsMultiple = true;

		return INF_ENGINE_SUCCESS;
	}

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLStringParser::ParseFunctionTextArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
													  avector<InfPatternItems::Base *> & aResult, nMemoryAllocator & aAllocator,
													  char aTagBegin, char aTagEnd, DLFunctionArgType aArgType, bool & aIsMultiple )
{
	unsigned int pos = aPos;

	const char TEXT_DELIMITERS[] = { aTagEnd, '\0' };
	const char EXTENDED_DELIMITERS[] = { '[', aTagEnd, '\0' };
	const char THEN_ELSE_DELIMITERS[] = { '[', aTagEnd, '{', '\0' };

	// Выбор разделетелей в зависимости от типа аргумента функции.
	const char * Delimiters = aArgType == DLFAT_TEXT ? TEXT_DELIMITERS : EXTENDED_DELIMITERS;

	// Если идёт обработка блоков оператора if.
	if( aTagEnd == '}' )
		Delimiters = THEN_ELSE_DELIMITERS;

	// Рабираем текст.
	InfEngineErrors iee = ParseText( aString, aStringLength, pos, Delimiters, aResult, aAllocator, false, aIsMultiple );

	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return INF_ENGINE_ERROR_CANT_PARSE;
	else
		ReturnWithTrace( iee );
}

InfEngineErrors DLStringParser::ParseFunctionDynamicArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
														 avector<InfPatternItems::Base *> & aResult, nMemoryAllocator & aAllocator,
														 char aTagBegin, char aTagEnd, DLFunctionArgType aArgType, bool & aIsMultiple )
{
	unsigned int pos = aPos;

	// Ссылка на словарь.
	InfEngineErrors iee = ParseDictRef( aString, aStringLength, pos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		ReturnWithErrorStrict( iee, "Invalid reference argument" );
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	// Референция.
	iee = ParseReference( aString, aStringLength, pos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		ReturnWithErrorStrict( iee, "Invalid reference argument" );
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );


	// Звёздочка.
	iee = ParseTagStar( aString, aStringLength, pos, aResult, aAllocator, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		ReturnWithErrorStrict( iee, "Invalid start argument" );
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	// Переменная.
	iee = ParseVarAndInstruct( aString, aStringLength, pos, aResult, aAllocator, false, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		ReturnWithErrorStrict( iee, "Invalid variable argument" );
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	// Разбираем аргумент как аргумент типа TEXT.
	iee = ParseFunctionTextArg( aString, aStringLength, pos, aResult, aAllocator, aTagBegin, aTagEnd, aArgType, aIsMultiple );

	if( pos < aPos && pos + 1 < aStringLength && aString[pos + 1] != aTagEnd )
		return INF_ENGINE_WARN_UNSUCCESS;

	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}
	else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLStringParser::ParseFunctionExtendedArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
														  avector<InfPatternItems::Base *> & aResult, avector<InfPatternItems::Base *> & aWaitingForResolving,
														  nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, DLFunctionArgType aArgType,
														  bool & aIsMultiple, bool aIsFunctionArg )
{
	unsigned int pos = aPos;

	// Функция.
	InfEngineErrors iee = ParseTagFunction( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, aIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		ReturnWithErrorStrict( iee, "Invalid function argument" );
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	// Разбираем аргумент как аргумент типа DYNAMIC.
	iee = ParseFunctionDynamicArg( aString, aStringLength, pos, aResult, aAllocator, aTagBegin, aTagEnd, aArgType, aIsMultiple );
	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}
	else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLStringParser::ParseFunctionBoolArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
													  avector<InfPatternItems::Base *> & aResult, avector<InfPatternItems::Base *> & aWaitingForResolving,
													  nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, DLFunctionArgType aArgType,
													  bool & aIsMultiple, bool aIsFunctionArg, bool aInterpretTextAsBool )
{
	unsigned int pos = aPos;

	// Проверка значения переменной, обёрнутая в соответствующую функцию.
	InfEngineErrors iee = ParseTagCheckVarValueLikeFunction( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, aTagBegin, aTagEnd, aIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		ReturnWithErrorStrict( iee, "Invalid function argument" );
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	// Функция.
	unsigned int result_size = aResult.size();
	iee = ParseTagFunction( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, aIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_SUCCESS )
	{
		// Условие должно состоять ровно из одного аргумента.
		if( aResult.size() - result_size != 1 )
			return INF_ENGINE_ERROR_CANT_PARSE;

		// Этим элемментом должна быть функция или тэг, представляемый как функция.
		const InfPatternItems::Base * Item = static_cast<const InfPatternItems::Base *>( aResult.back() );

		if( !Item || Item->GetType() != InfPatternItems::itFunction )
			return INF_ENGINE_ERROR_CANT_PARSE;

		// Если это функция, то функция должна иметь тип BOOL.
		const InfPatternItems::TagFunction * Function = static_cast<const InfPatternItems::TagFunction *>( Item );
		const DLFunctionResType * FuncResType =
			vDLDataWR.GetFunctionsRegistry().GetFunctionResType( Function->GetId() );
		if( !FuncResType )
			return INF_ENGINE_ERROR_CANT_PARSE;

		if( *FuncResType != DLFRT_BOOL && !( aInterpretTextAsBool && *FuncResType == DLFRT_TEXT ) )
			ReturnWithParseError( DLErrorId::peFunctionInvalidResType, pos );

		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		ReturnWithErrorStrict( iee, "Invalid function argument" );
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLStringParser::ParseFunctionAnyArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
													 avector<InfPatternItems::Base *> & aResult, avector<InfPatternItems::Base *> & aWaitingForResolving,
													 nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, DLFunctionArgType aArgType, bool & aIsMultiple, bool aIsFunctionArg )
{
	//TODO: как запретить интерфейсные тэги во вложенных в аргумент тэгах????


	// Тэг должен начинаться с открывающейся квадратной скобки.
	if( aString[aPos] == '[' )
	{

		// Перебор возможных тэгов.
		InfEngineErrors iee = ParseDictRef( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseReference( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseVarAndInstruct( aString, aStringLength, aPos, aResult, aAllocator, !aIsFunctionArg, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagDict( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagGoTo( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagIf( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple, aIsFunctionArg );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagSwitch( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple, aIsFunctionArg );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagStar( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagDictInline( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple, false );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseExternalService( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagFunction( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple, aIsFunctionArg );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagRSS( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagExtendAnswer( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagPre( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseStartList( aString, aStringLength, aPos, aResult, aAllocator );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseListItem( aString, aStringLength, aPos, aResult, aAllocator );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseEndList( aString, aStringLength, aPos, aResult, aAllocator );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );
	}
	// Inline-словарь.
	else if( aString[aPos] == '{' )
	{
		InfEngineErrors iee = ParseTagDictInline( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple, false );

		if( iee == INF_ENGINE_SUCCESS )
			return INF_ENGINE_SUCCESS;
		else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			ReturnWithErrorStrict( iee, "Invalid inline-dict argument" );
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );
	}

	// Разбираем аргумент как аргумент типа EXTENDED.
	InfEngineErrors iee = ParseFunctionExtendedArg( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator,
													aTagBegin, aTagEnd, aArgType, aIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_SUCCESS )
		return INF_ENGINE_SUCCESS;
	else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLStringParser::ParseFunctionIfArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
													avector<InfPatternItems::Base *> & aResult, avector<InfPatternItems::Base *> & aWaitingForResolving,
													nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, DLFunctionArgType aArgType,
													bool & aIsMultiple, bool aIsFunctionArg )
{
	// Тэг должен начинаться с открывающейся квадратной скобки.
	if( aString[aPos] == '[' )
	{
		// Перебор возможных тэгов.
		InfEngineErrors iee = ParseTagBreak( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagBR( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagNothing( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagDisableautovars( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagInf( aString, aStringLength, aPos, aWaitingForResolving, aResult, aAllocator, aIsMultiple, aIsFunctionArg );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagHref( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );

		iee = ParseTagOpenWindow( aString, aStringLength, aPos, aResult, aAllocator, aIsMultiple );
		if( iee == INF_ENGINE_SUCCESS || iee == INF_ENGINE_ERROR_CANT_PARSE )
			return iee;
		else if( iee != INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithTrace( iee );
	}

	// Разбираем аргумент как аргумент типа ANY.
	InfEngineErrors iee = ParseFunctionAnyArg( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aTagBegin, aTagEnd, aArgType, aIsMultiple, aIsFunctionArg );
	if( iee == INF_ENGINE_SUCCESS )
		return INF_ENGINE_SUCCESS;
	else if( iee == INF_ENGINE_ERROR_CANT_PARSE )
		return iee;
	else if( iee != INF_ENGINE_WARN_UNSUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLStringParser::ParseFunctionVariableArg( const char * aString, unsigned int aStringLength, unsigned int & aPos,
														  avector<InfPatternItems::Base *> & aResult, nMemoryAllocator & aAllocator,
														  char aTagBegin, char aTagEnd, DLFunctionArgType aArgType, bool & aIsMultiple )
{
	// Переменная, переданная по ссылке.
	unsigned int pos = aPos;

	unsigned int end = GetVarName( aString, aStringLength, pos );
	if( end == pos )
		ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

	nlReturnCode nle = vtmp.assign( aString + pos, end - pos );
	if( nle != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	end = TextFuncs::SkipSpaces( aString, end, aStringLength );
	if( aString[end] != aTagEnd )
		ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

	AllTrim( vtmp );
	if( SetLower( vtmp ) != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	pos = end;

	// Поиск переменной в реестре.
	Vars::Id VarId;
	InfEngineErrors iee = CheckVariable( vtmp.ToConstChar(), vtmp.size(), VarId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithParseError( DLErrorId::peVariableUnregistred, pos );

	// Выделение памяти.
	InfPatternItems::TagVar * tag = nAllocateObject( aAllocator, InfPatternItems::TagVar );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagVar;

	// Установка идентификатора.
	tag->Set( VarId );

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagPre( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											 avector<InfPatternItems::Base *> & aResult, nMemoryAllocator & aAllocator,
											 bool & aIsMultiple )
{
	aIsMultiple = false;
	unsigned int pos = TextFuncs::SkipSpaces( aString, aPos, aStringLength );

	// Открывающая скобка
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Имя тэга.
	if( strncasecmp( aString + pos, "pre", 3 ) )
		return INF_ENGINE_WARN_UNSUCCESS;
	pos = TextFuncs::SkipSpaces( aString, pos + 3, aStringLength );

	// Закрывающая скобка
	if( aString[pos] != ']' )
		return INF_ENGINE_WARN_UNSUCCESS;
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Начало неинтерпретирумого текста
	if( aString[pos] != '{' )
		ReturnWithParseError( DLErrorId::pePreOpenBrace, aPos );
	pos += 1;

	// Текст.
	unsigned int begin = pos;
	unsigned int end = pos;
	avector<unsigned int> shift;
	while( pos < aStringLength && !( aString[pos] == '}' && aString[pos - 1] != '\\' ) )
	{
		if( '}' == aString[pos] )
		{
			shift.push_back( pos - 1 - begin );
			if( shift.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
		++pos;
	}

	// Конец неинтерпретирумого текста
	if( pos >= aStringLength || aString[pos] != '}' )
		ReturnWithParseError( DLErrorId::pePreCloseBrace, aPos );
	end = pos;

	// Првоерка текста на пустоту.
	if( begin == end )
		ReturnWithParseError( DLErrorId::pePreEmpty, aPos );

	// Создание тэга.
	InfPatternItems::TagPre * pre = nAllocateObject( aAllocator, TagPre );
	if( !pre )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)pre ) InfPatternItems::TagPre;

	// Копирование текста.
	unsigned int len = end - begin;
	char * text = nAllocateObjects( aAllocator, char, len );
	if( !text )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( text, aString + begin, end - begin );

	// Удаление экранирующих символов.
	for( unsigned int n = 0; n < shift.size(); ++n )
		memmove( text + shift[n] - n, text + shift[n] - n + 1, --len - shift[n] + n );
	text[len] = 0;

	pre->Set( text, len );

	// Добавление тэга к результату.
	aResult.push_back( pre );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseStartList( const char * aString, unsigned int aStringLength, unsigned int & aPos,
												avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	bool ordered {false};	
    if( pos + 4 < aStringLength && !strncasecmp( "list", aString + pos, 4 ) )
		pos = TextFuncs::SkipSpaces( aString, pos + 4, aStringLength );
	else if( pos + 5 < aStringLength && !strncasecmp( "ulist", aString + pos, 5 ) )
		pos = TextFuncs::SkipSpaces( aString, pos + 5, aStringLength );
	else if( pos + 5 < aStringLength && !strncasecmp( "olist", aString + pos, 5 ) )
	{
		pos = TextFuncs::SkipSpaces( aString, pos + 5, aStringLength );
		ordered = true;
	}
    else if( pos + 2 < aStringLength && !strncasecmp( "ul", aString + pos, 2 ) )
        pos = TextFuncs::SkipSpaces( aString, pos + 2, aStringLength );
    else if( pos + 2 < aStringLength && !strncasecmp( "ol", aString + pos, 2 ) )
    {
        pos = TextFuncs::SkipSpaces( aString, pos + 2, aStringLength );
        ordered = true;
    }
	else
		return INF_ENGINE_WARN_UNSUCCESS;

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peStartListCloseSquareBrace, pos );

	// Выделение памяти.
	if( ordered == false )
	{
		nAllocateNewObject( aAllocator, InfPatternItems::StartUList, tag );
		if( !tag )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new( (nMemoryAllocatorElementPtr*)tag ) InfPatternItems::StartUList { aPos };

		aResult.push_back( tag );
		if( aResult.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}
	else
	{
		nAllocateNewObject( aAllocator, InfPatternItems::StartOList, tag );
		if( !tag )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new( (nMemoryAllocatorElementPtr*)tag ) InfPatternItems::StartOList { aPos };

		aResult.push_back( tag );
		if( aResult.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	aPos = pos;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseListItem( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											   avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos + 2 >= aStringLength || strncasecmp( "li", aString + pos, 2 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	pos = TextFuncs::SkipSpaces( aString, pos + 2, aStringLength );

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peListItemCloseSquareBrace, pos );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::ListItem, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new( (nMemoryAllocatorElementPtr*)tag ) InfPatternItems::ListItem { aPos };

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseEndList( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											  avector<InfPatternItems::Base*> & aResult, nMemoryAllocator & aAllocator )
{
	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos + 2 < aStringLength && !strncasecmp( "el", aString + pos, 2 ) )
		pos = TextFuncs::SkipSpaces( aString, pos + 2, aStringLength );
	else if( pos + 7 < aStringLength && !strncasecmp( "endlist", aString + pos, 7 ) )
		pos = TextFuncs::SkipSpaces( aString, pos + 7, aStringLength );
	else
		return INF_ENGINE_WARN_UNSUCCESS;

	if( aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peEndListCloseSquareBrace, pos );

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::EndList, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new( (nMemoryAllocatorElementPtr*)tag ) InfPatternItems::EndList { aPos };

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagFunctionArg( const char * aString, unsigned int aStringLength,
													 unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
													 avector<InfPatternItems::Base *> & aWaitingForResolving,
													 nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd,
													 DLFunctionArgType aArgType, bool & aIsMultiple,
													 bool aIsFunctionArg, bool aInterpretTextAsBool, DLFunctionArgAttrs aAttrs )
{
	bool IsMultiple = false;
	bool ArgIsMultiple;

	unsigned int pos = aPos;

	InfEngineErrors iee;

	// Проверка на начало аргумента.
	if( aString[pos] != aTagBegin )
		return INF_ENGINE_ERROR_CANT_PARSE;

	// Выделение памяти под аргумент функции.
	nAllocateNewObject( aAllocator, InfPatternItems::TagFunction::Argument, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagFunction::Argument;

	unsigned int ResultSize = aResult.size();

	// Флаг наличия начального пробельного символа.
	bool first_space = false;
	// Флаг наличия конечного пробельного символа.
	bool last_space = false;

	// Проверка на наличие начального пробельного символа.
	++pos;
	if( pos < aStringLength && TextFuncs::IsSpace( aString[pos] ) )
		first_space = true;

	for(; pos < aStringLength; pos++ )
	{
		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );
		if( pos >= aStringLength )
		{
			aResult.resize( ResultSize );
			ReturnWithParseError( DLErrorId::peFunctionInvalidArgumentValue, aPos );
		}

		// Проверяем, достигнут ли конец аргумента.
		if( aString[pos] == aTagEnd )
			break;


		if( aArgType == DLFAT_TEXT )
		{
			iee = ParseFunctionTextArg( aString, aStringLength, pos, aResult, aAllocator, aTagBegin, aTagEnd, aArgType, ArgIsMultiple );

			if( iee == INF_ENGINE_SUCCESS )
			{
				++pos;
				IsMultiple |= ArgIsMultiple;
			}
			else
			{
				aResult.resize( ResultSize );

				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithErrorStrict( iee, "Failed to parse TEXT function argument" );
				else if( iee == INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

				ReturnWithTrace( iee );
			}

			if( !( aString[pos] == aTagEnd && pos && aString[pos - 1] != '\\' ) )
			{
				aResult.resize( ResultSize );
				ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );
			}

			break;

		}
		else if( aArgType == DLFAT_DYNAMIC )
		{
			iee = ParseFunctionDynamicArg( aString, aStringLength, pos, aResult, aAllocator, aTagBegin, aTagEnd, aArgType, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
				IsMultiple |= ArgIsMultiple;
			else
			{
				aResult.resize( ResultSize );

				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithErrorStrict( iee, "Failed to parse DYNAMIC function argument" );
				else if( iee == INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

				ReturnWithTrace( iee );
			}
		}
		else if( aArgType == DLFAT_EXTENDED )
		{
			const char * tmp = aString + pos;

			iee = ParseFunctionExtendedArg( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, aTagBegin, aTagEnd, aArgType, ArgIsMultiple, true );
			if( iee == INF_ENGINE_SUCCESS )
				IsMultiple |= ArgIsMultiple;
			else
			{
				aResult.resize( ResultSize );

				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithErrorStrict( iee, "Failed to parse EXTENDED function argument" );
				else if( iee == INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

				ReturnWithTrace( iee );
			}
		}
		else if( aArgType == DLFAT_ANY )
		{
			iee = ParseFunctionAnyArg( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, aTagBegin, aTagEnd, aArgType, ArgIsMultiple, true );
			if( iee == INF_ENGINE_SUCCESS )
				IsMultiple |= ArgIsMultiple;
			else
			{
				aResult.resize( ResultSize );

				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithErrorStrict( iee, "Failed to parse ANY function argument" );
				else if( iee == INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

				ReturnWithTrace( iee );
			}
		}
		else if( aArgType == DLFAT_IF )
		{
			if( aIsFunctionArg )
				iee = ParseFunctionAnyArg( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, aTagBegin, aTagEnd, aArgType, ArgIsMultiple, true );
			else
				iee = ParseFunctionIfArg( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, aTagBegin, aTagEnd, aArgType, ArgIsMultiple, aIsFunctionArg );

			if( iee == INF_ENGINE_SUCCESS )
				IsMultiple |= ArgIsMultiple;
			else
			{
				aResult.resize( ResultSize );

				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithErrorStrict( iee, "Failed to parse IF function argument" );
				else if( iee == INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

				ReturnWithTrace( iee );
			}
		}
		else if( aArgType == DLFAT_BOOL )
		{
			iee = ParseFunctionBoolArg( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, aTagBegin, aTagEnd, aArgType, ArgIsMultiple, true, aInterpretTextAsBool );
			if( iee == INF_ENGINE_SUCCESS )
				IsMultiple |= ArgIsMultiple;
			else
			{
				aResult.resize( ResultSize );

				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithErrorStrict( iee, "Failed to parse BOOL function argument" );
				else if( iee == INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

				ReturnWithTrace( iee );
			}
		}
		else if( aArgType == DLFAT_VARIABLE )
		{
			iee = ParseFunctionVariableArg( aString, aStringLength, pos, aResult, aAllocator, aTagBegin, aTagEnd, aArgType, ArgIsMultiple );
			if( iee == INF_ENGINE_SUCCESS )
				IsMultiple |= ArgIsMultiple;
			else
			{
				aResult.resize( ResultSize );

				if( iee == INF_ENGINE_ERROR_CANT_PARSE )
					ReturnWithErrorStrict( iee, "Failed to parse VARIABLE function argument" );
				else if( iee == INF_ENGINE_WARN_UNSUCCESS )
					ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );

				ReturnWithTrace( iee );
			}

			if( !( aString[pos] == aTagEnd && pos && aString[pos - 1] != '\\' ) )
			{
				aResult.resize( ResultSize );
				ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, pos );
			}

			break;
		}
		else
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Unknown Function Argument Type: %i", aArgType );
	}

	// Проверка на наличие конечного пробельного символа.
	if( pos - 1 > aPos && TextFuncs::IsSpace( aString[pos - 1] ) )
		last_space = true;

	static char SpaceText[] = " ";
	unsigned int items_count = aResult.size() - ResultSize;
	InfPatternItems::Space * FirstSpace = nullptr;
	InfPatternItems::Space * LastSpace = nullptr;


	// Подготовка ведущего пробела.
	if( first_space && ( aAttrs & DLFAA_LEFT_SPACE ) )
	{
		InfPatternItems::Space * FirstSpace = nAllocateObject( aAllocator, InfPatternItems::Space );
		if( !FirstSpace )
		{
			aResult.resize( ResultSize );
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
		new((nMemoryAllocatorElementPtr *)FirstSpace ) InfPatternItems::Space;

		FirstSpace->Set( SpaceText, 1 );
	}

	// Подготовка конечного пробела.
	if( last_space && ( aAttrs & DLFAA_RIGHT_SPACE ) )
	{
		InfPatternItems::Space * LastSpace = nAllocateObject( aAllocator, InfPatternItems::Space );
		if( !LastSpace )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new((nMemoryAllocatorElementPtr *)LastSpace ) InfPatternItems::Space;

		LastSpace->Set( SpaceText, 1 );
	}

	unsigned int items_count_ex = items_count + ( FirstSpace ? 1 : 0 ) + ( LastSpace ? 1 : 0 );

	if( items_count_ex )
	{
		// Копируем элементы.
		InfPatternItems::Base ** items = nAllocateObjects( aAllocator, InfPatternItems::Base *, items_count_ex );
		if( !items )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( items + ( FirstSpace ? 1 : 0 ), aResult.get_buffer() + ResultSize, items_count * sizeof( InfPatternItems::Base * ) );

		// Добавляем ведущий пробел.
		if( FirstSpace )
			items[0] = FirstSpace;

		// Добавляем конечный пробел.
		if( LastSpace )
			items[items_count + ( FirstSpace ? 1 : 0 )] = LastSpace;
		tag->SetItems( items, items_count_ex );
	}
	else
		tag->SetItems( nullptr, 0 );

	// Удаляем из результата временно сохранённые в нём элементы.
	aResult.resize( ResultSize );

	if( pos >= aStringLength || aString[pos] != aTagEnd )
		return INF_ENGINE_WARN_UNSUCCESS;

	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	aIsMultiple = IsMultiple;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::CheckTagLikeFinction( InfPatternItems::TagFunction * aTagFunction, unsigned int aPos )
{
	InfEngineErrors iee = CheckTagDictsMatchLF( aTagFunction, aPos );
	if( INF_ENGINE_SUCCESS == iee || INF_ENGINE_ERROR_CANT_PARSE == iee )
		return iee;
	else if( INF_ENGINE_WARN_UNSUCCESS != iee )
		ReturnWithTrace( iee );

	iee = CheckTagDictsNotMatchLF( aTagFunction, aPos );
	if( INF_ENGINE_SUCCESS == iee || INF_ENGINE_ERROR_CANT_PARSE == iee )
		return iee;
	else if( INF_ENGINE_WARN_UNSUCCESS != iee )
		ReturnWithTrace( iee );

	iee = CheckTagDictsMatchExtLF( aTagFunction, aPos );
	if( INF_ENGINE_SUCCESS == iee || INF_ENGINE_ERROR_CANT_PARSE == iee )
		return iee;
	else if( INF_ENGINE_WARN_UNSUCCESS != iee )
		ReturnWithTrace( iee );

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLStringParser::CheckTagDictsMatchLF( InfPatternItems::TagFunction * aTagFunction, unsigned int aPos )
{
	// Проверка имени функции.
	if( strcasecmp( "DictsMatch", vDLDataWR.GetFunctionsRegistry().GetFuncNameById( aTagFunction->GetId() ) ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Проверка количества аргументов.
	if( aTagFunction->GetArgsCount() < 2 )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "To few arguments in tag DictsMatch: %i", aTagFunction->GetArgsCount() );

	// Получение первого аргумента тэга-функции.
	const TagFunction::Argument * arg = aTagFunction->GetArg( 0 );
	if( !arg )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Первый аргумент DictsMatch должен состоять из одного элемента.
	if( 1 != arg->GetItemsCount() )
		ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, aPos );

	// Проверка типа первого аргумента тэга-функции.
	if( InfPatternItems::itStar != arg->GetItem( 0 )->GetType() && InfPatternItems::itReference != arg->GetItem( 0 )->GetType() )
		ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, aPos );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::CheckTagDictsNotMatchLF( InfPatternItems::TagFunction * aTagFunction, unsigned int aPos )
{
	// Проверка имени функции.
	if( strcasecmp( "DictsNotMatch", vDLDataWR.GetFunctionsRegistry().GetFuncNameById( aTagFunction->GetId() ) ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Проверка количества аргументов.
	if( aTagFunction->GetArgsCount() < 2 )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "To few arguments in tag DictsNotMatch: %i", aTagFunction->GetArgsCount() );

	// Получение первого аргумента тэга-функции.
	const TagFunction::Argument * arg = aTagFunction->GetArg( 0 );
	if( !arg )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Первый аргумент DictsNotMatch должен состоять из одного элемента.
	if( 1 != arg->GetItemsCount() )
		ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, aPos );

	// Проверка типа первого аргумента тэга-функции.
	if( InfPatternItems::itStar != arg->GetItem( 0 )->GetType() && InfPatternItems::itReference != arg->GetItem( 0 )->GetType() )
		ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, aPos );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::CheckTagDictsMatchExtLF( InfPatternItems::TagFunction * aTagFunction, unsigned int aPos )
{
	// Проверка имени функции.
	if( strcasecmp( "DictsMatchExt", vDLDataWR.GetFunctionsRegistry().GetFuncNameById( aTagFunction->GetId() ) ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Проверка количества аргументов.
	if( aTagFunction->GetArgsCount() < 2 )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "To few arguments in tag DictsMatchExt: %i", aTagFunction->GetArgsCount() );

	// Получение первого аргумента тэга-функции.
	const TagFunction::Argument * arg = aTagFunction->GetArg( 0 );
	if( !arg )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Первый аргумент DictsMatchExt должен состоять из одного элемента.
	if( 1 != arg->GetItemsCount() )
		ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, aPos );

	// Проверка типа первого аргумента тэга-функции.
	if( InfPatternItems::itStar != arg->GetItem( 0 )->GetType() && InfPatternItems::itReference != arg->GetItem( 0 )->GetType() )
		ReturnWithParseError( DLErrorId::peFunctionInvalidArgType, aPos );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagFunction( const char * aString, unsigned int aStringLength,
												  unsigned int & aPos, avector<InfPatternItems::Base *> & aResult,
												  avector<InfPatternItems::Base *> & aWaitingForResolving,
												  nMemoryAllocator & aAllocator, bool & aIsMultiple, bool aIsFunctionArg )
{
	// Обработка алиаса.
	InfEngineErrors iee = ParseTagAlias( aString, aStringLength, aPos, aResult, aWaitingForResolving, aAllocator, aIsMultiple, aIsFunctionArg );

	if( iee == INF_ENGINE_SUCCESS )
		return INF_ENGINE_SUCCESS;
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return INF_ENGINE_WARN_UNSUCCESS;
	if( iee != INF_ENGINE_ERROR_UNKNOWN_ALIAS )
		ReturnWithTrace( iee );

	unsigned int pos = aPos;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos + 1 >= aStringLength || aString[pos] != '@' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Выделение имени функции и приведение к нижнему регистру.
	++pos;
	aTextString FuncName;
	unsigned int end = TextFuncs::FindSymbol( aString, aStringLength, "(", pos );
	if( pos >= aStringLength )
		return INF_ENGINE_ERROR_CANT_PARSE;

	nlReturnCode nlrc = FuncName.assign( aString + pos, end - pos );
	if( nlrc == nlrcErrorNoFreeMemory )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
						 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	else if( nlrc != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Failed to parse function name" );

	AllTrim( FuncName );
	TextFuncs::ToLower( FuncName.ToChar(), FuncName.size() );

	// Получение идентификатора функции.
	unsigned int FuncId;
	const FunctionsRegistry & aFunctionsRegistry = vDLDataWR.GetFunctionsRegistry();
	iee = aFunctionsRegistry.Search( FuncName.ToConstChar(), FuncName.size(), FuncId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithParseError( DLErrorId::peFunctionInvalidName, pos );

	// Проверка возможности кэширования значения тэга-функции.
	const DLFunctionOptions * Options = aFunctionsRegistry.GetOptions( FuncId );
	if( !Options )
		ReturnWithErrorStrict( INF_ENGINE_ERROR_CANT_PARSE, "Can't get function \"%s\" options", FuncName.ToConstChar() );
	bool IsMultiple = !( *Options & DLFO_CACHE );
	bool ArgIsMultiple;

	// Выделение памяти.
	nAllocateNewObject( aAllocator, InfPatternItems::TagFunction, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagFunction;

	// Установка идентификатора.
	tag->Set( FuncId );

	const unsigned int * ArgumentsNumber = aFunctionsRegistry.GetFunctionArgCount( FuncId );
	if( !ArgumentsNumber )
		ReturnWithErrorStrict( INF_ENGINE_WARN_UNSUCCESS, "Cant get Function arguments number: \"%s\"", FuncName.ToConstChar() );

	// Разбор аргументов функции.
	pos = end + 1;

	bool CommaExpexted = false;
	unsigned int ArgCnt = 0;
	const DLFunctionOptions * FuncOptions = aFunctionsRegistry.GetOptions( FuncId );
	if( !FuncOptions )
		ReturnWithErrorStrict( INF_ENGINE_WARN_UNSUCCESS, "Cant get Function options: \"%s\"", FuncName.ToConstChar() );

	avector<InfPatternItems::Base *> Arguments;

	for( unsigned int pos = end + 1; pos < aStringLength; pos++ )
	{
		// Пропускаем пробелы перед аргументом.
		pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );
		if( pos >= aStringLength )
			return INF_ENGINE_ERROR_CANT_PARSE;

		// Проверяем на окончание списка аргументов.
		if( aString[pos] == ')' )
		{
			// Проверка на минимальное необходимое число аргументов у функции.
			if( *ArgumentsNumber > ArgCnt )
			{
				const DLFucntionArgInfo * ArgInfo = aFunctionsRegistry.GetFunctionArgInfo( FuncId, ArgCnt );
				if( !ArgInfo )
					ReturnWithErrorStrict( INF_ENGINE_WARN_UNSUCCESS, "Failed to check function \"%s\" arguments", FuncName.ToConstChar() );

				// Проверка на наличие у нехватающих аргументов значений по умолчанию.
				if( !ArgInfo->default_value )
					ReturnWithErrorStrict( INF_ENGINE_WARN_UNSUCCESS, "Not enough arguments were given to function \"%s\"", FuncName.ToConstChar() );
			}

			// Проверка аргументов.
			for( size_t arg_n = 0; arg_n < Arguments.size(); ++arg_n )
			{
				if( Arguments[arg_n]->GetType() != InfPatternItems::itFunctionArg )
					ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Object of class TagFunction::Argument expected." );
			}

			// Копирование и добавление аргументов к функции.
			if( Arguments.size() )
			{
				InfPatternItems::TagFunction::Argument ** args = nAllocateObjects( aAllocator, InfPatternItems::TagFunction::Argument *, Arguments.size() );
				if( !args )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				memcpy( args, Arguments.get_buffer(), Arguments.size() * sizeof( InfPatternItems::TagFunction::Base * ) );
				tag->SetArgs( args, Arguments.size() );
			}
			else
				tag->SetArgs( nullptr, 0 );

			if( vStrictMode )
			{
				unsigned int aFuncTagId;
				vDLDataWR.RegistrateTagFunction( tag, aFuncTagId, IsMultiple );
			}

			// Дополнительноая проверка тэгов, представляемых в виде функций.
			iee = CheckTagLikeFinction( tag, aPos );
			if( INF_ENGINE_SUCCESS != iee && INF_ENGINE_WARN_UNSUCCESS != iee )
				return iee;

			aResult.push_back( tag );
			if( aResult.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
			aIsMultiple = IsMultiple;

			// Проверяем корректность окончания тэга-функции.
			if( aString[pos] != ']' )
				ReturnWithParseError( DLErrorId::peFunctionCloseBraketMissed, pos );

			aPos = pos;

			if( INF_ENGINE_SUCCESS != ( iee = vDLDataWR.SetFunctionUsed( FuncId ) ) )
				ReturnWithTrace( iee );

			return INF_ENGINE_SUCCESS;
		}

		// Проверяем на наличие запятой между аргументами.
		if( CommaExpexted && aString[pos] == ',' )
		{
			CommaExpexted = false;
			continue;
		}

		// Проверяем на наличие очередного аргумента.
		if( aString[pos] != '"' )
			ReturnWithParseError( DLErrorId::peFunctionInvalidFormat, pos );

		// Проверяем не привышено ли количество аргументов у функции.
		if( ArgCnt >= *ArgumentsNumber && !aFunctionsRegistry.HasVarArgs( FuncId ) )
			ReturnWithParseError( DLErrorId::peFunctionInvalidArgMax, pos );


		// Разбираем очередной аргумент.
		const DLFucntionArgInfo * ArgInfo = aFunctionsRegistry.GetFunctionArgInfo( FuncId, ArgCnt );
		if( !ArgInfo )
			ReturnWithErrorStrict( INF_ENGINE_WARN_UNSUCCESS, "Failed to get function \"%s\" %ith argument", FuncName.ToConstChar(), ArgCnt );

		if( ( iee = ParseTagFunctionArg( aString, aStringLength, pos, Arguments, aWaitingForResolving, aAllocator, '"', '"',
										 aFunctionsRegistry.GetFunctionArgInfo( FuncId, ArgCnt )->type,
										 ArgIsMultiple, aIsFunctionArg, false,
										 *FuncOptions & DLFO_ARGS_ATTRS ? ArgInfo->attrs : DLFAA_NONE ) ) != INF_ENGINE_SUCCESS )
			return iee;

		IsMultiple |= ArgIsMultiple;

		CommaExpexted = true;

		++ArgCnt;
	}

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DLStringParser::ParseTagCheckVarValueLikeFunction( const char * aString, unsigned int aStringLength, unsigned int & aPos,
																   avector<InfPatternItems::Base *> & aResult, avector<InfPatternItems::Base *> & aWaitingForResolving,
																   nMemoryAllocator & aAllocator, char aTagBegin, char aTagEnd, bool & aIsMultiple, bool aIsFunctionArg )
{
	bool IsMultuple = false;
	bool ArgIsMultiple;

	unsigned int pos = aPos;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );

	// Проверка пустоты строки шаблона.
	if( pos >= aStringLength )
		return INF_ENGINE_WARN_UNSUCCESS;

	vtmp.clear();

	InfBaseConditionType ConditionType = InfConditionUndefined;
	bool is_negated = false;
	avector<InfPatternItems::Base *> Arguments;
	if( pos < aStringLength && aString[pos] == '!' )
	{
		is_negated = true;
		++pos;
	}
	if( pos < aStringLength && aString[pos] == '%' )
	{
		// Выделение имени переменной.
		++pos;
		unsigned int end = GetVarName( aString, aStringLength, pos );
		if( end == pos )
			ReturnWithParseError( DLErrorId::peConditionVarNameInvalid, pos );

		nlReturnCode nlrc = vtmp.assign( aString + pos, end - pos );
		if( nlrc != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Понижение регистра.
		if( SetLower( vtmp ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Поиск переменной в реестре.
		Vars::Id VarId;
		InfEngineErrors iee = CheckVariable( vtmp.ToConstChar(), vtmp.size(), VarId );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithParseError( DLErrorId::peConditionUnregistredVarName, pos );
		else if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );


		// Выделение памяти под тэг-переменную.
		InfPatternItems::TagVar * tag = nAllocateObject( aAllocator, InfPatternItems::TagVar );
		if( !tag )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagVar;

		// Установка идентификатора переменной в тэг.
		tag->Set( VarId );

		// Выделение памяти под аргумент функции.
		nAllocateNewObject( aAllocator, InfPatternItems::TagFunction::Argument, TagArument );
		if( !TagArument )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new((nMemoryAllocatorElementPtr *)TagArument ) InfPatternItems::TagFunction::Argument;

		// Упаковка тэга-переменной в тэг-аргумент-функции.
		InfPatternItems::Base ** items = nAllocateObjects( aAllocator, InfPatternItems::Base *, 1 );
		if( !items )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		items[0] = tag;
		TagArument->SetItems( items, 1 );

		// Добавление аргумента в список аргументов.
		Arguments.push_back( TagArument );
		if( Arguments.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, end, aStringLength );
		// Если разбираемая строка закончилась - ошибка.
		if( pos >= aStringLength )
			ReturnWithParseError( DLErrorId::peParseUnexpectedEndOfLine, pos );

		// Проверка на закрывающую скобку (в случае проверки на [не]пустоту).
		if( aString[pos] == aTagEnd )
		{
			ConditionType = is_negated ? InfConditionDontExist : InfConditionExist;
		}
		else if( is_negated )
		{
			// После проверки переменной на пустоту встречен лишний текст.
			ReturnWithParseError( DLErrorId::peParseUnexpectedCharacterAfterCheckEmpty, pos );
		}
		// Проверка на равенство.
		else if( aString[pos] == '=' )
		{
			ConditionType = InfConditionEqual;
			++pos;
		}
		// Проверка на неравенство.
		else if( pos + 1 < aStringLength && aString[pos] == '!' && aString[pos + 1] == '=' )
		{
			ConditionType = InfConditionNotEqual;
			pos += 2;
		}
		else
			ReturnWithParseError( DLErrorId::peParseUnexpectedCharacter, pos );

		if( ConditionType == InfConditionEqual || ConditionType == InfConditionNotEqual )
		{
			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );
			// Если разбираемая строка закончилась - ошибка.
			if( pos >= aStringLength || aString[pos] != '"' )
				ReturnWithParseError( DLErrorId::peParseSecondConditionArgExpected, pos );

			// Разбор второго аргумента.
			if( ParseTagFunctionArg( aString, aStringLength, pos, Arguments, aWaitingForResolving, aAllocator,
									 '"', '"', DLFAT_EXTENDED, ArgIsMultiple, aIsFunctionArg, false ) != INF_ENGINE_SUCCESS )
				ReturnWithParseError( DLErrorId::peParseSecondConditionArgExpected, pos );
			IsMultuple |= ArgIsMultiple;

			// Если второй аргумент отсутсвует - ошибка.
			if( Arguments.size() == 1 )
				ReturnWithParseError( DLErrorId::peParseSecondConditionArgIsEmpty, pos );

			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
			// Проверка на окончание условия.
			if( pos >= aStringLength || aString[pos] != aTagEnd )
				ReturnWithParseError( DLErrorId::peIfCloseRoundBrace, pos );
		}

		// Создание функции.
		InfPatternItems::TagFunction * TagFucntion = nAllocateObject( aAllocator, InfPatternItems::TagFunction );
		if( !TagFucntion )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new((nMemoryAllocatorElementPtr *)TagFucntion ) InfPatternItems::TagFunction;

		unsigned int FuncId;

		// Выбор функции, которая станет условием в операторе If.
		switch( ConditionType )
		{
		case InfConditionExist:
			// Получение идентификатора функции IsNotEmpty.
			iee = vDLDataWR.GetFunctionsRegistry().Search( "isnotempty", 10, FuncId );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithParseError( DLErrorId::peFunctionInvalidName, pos );
			break;

		case InfConditionDontExist:
			// Получение идентификатора функции IsEmpty.
			iee = vDLDataWR.GetFunctionsRegistry().Search( "isempty", 7, FuncId );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithParseError( DLErrorId::peFunctionInvalidName, pos );
			break;

		case InfConditionEqual:
			// Получение идентификатора функции IsEqual.
			iee = vDLDataWR.GetFunctionsRegistry().Search( "isequal", 7, FuncId );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithParseError( DLErrorId::peFunctionInvalidName, pos );
			break;

		case InfConditionNotEqual:
			// Получение идентификатора функции IsNotEqual.
			iee = vDLDataWR.GetFunctionsRegistry().Search( "isnotequal", 10, FuncId );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithParseError( DLErrorId::peFunctionInvalidName, pos );
			break;

		case InfConditionFunction:
		case InfConditionAnyValue:
			// Такое условие не могло тут появиться.
			ReturnWithError( INF_ENGINE_ERROR_FAULT, "Invalid condition type" );

		default:
			ReturnWithParseError( DLErrorId::peConditionInvalid, pos );
		}

		// Установка идентификатора функции.
		TagFucntion->Set( FuncId );

		const DLFunctionOptions * Options = vDLDataWR.GetFunctionsRegistry().GetOptions( FuncId );
		if( !Options )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Can't get function options" );
		if( VarId.is_tmp() )
			IsMultuple = true;
		else
			IsMultuple |= !( *Options & DLFO_CACHE );

		// Проверка аргументов.
		for( size_t arg_n = 0; arg_n < Arguments.size(); ++arg_n )
		{
			if( Arguments[arg_n]->GetType() != InfPatternItems::itFunctionArg )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Object of class TagFunction::Argument expected." );
		}

		// Копирование и добавление аргументов к функции.
		if( Arguments.size() )
		{
			InfPatternItems::TagFunction::Argument ** args = nAllocateObjects( aAllocator, InfPatternItems::TagFunction::Argument *, Arguments.size() );
			if( !args )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			memcpy( args, Arguments.get_buffer(), Arguments.size() * sizeof( InfPatternItems::TagFunction::Base * ) );
			TagFucntion->SetArgs( args, Arguments.size() );
		}
		else
			TagFucntion->SetArgs( nullptr, 0 );

		if( vStrictMode )
		{
			unsigned int aFuncTagId;
			vDLDataWR.RegistrateTagFunction( TagFucntion, aFuncTagId, IsMultuple );
		}

		// Копирование в результат.
		aResult.push_back( TagFucntion );
		if( aResult.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}
	else
		return INF_ENGINE_WARN_UNSUCCESS;

	aPos = pos - 1;

	aIsMultiple = IsMultuple;

	return INF_ENGINE_SUCCESS;
}


InfEngineErrors DLStringParser::ParseTagAlias( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											   avector<InfPatternItems::Base *> & aResult, avector<InfPatternItems::Base *> & aWaitingForResolving,
											   nMemoryAllocator & aAllocator, bool & IsMultiple, bool aIsFunctionArg )
{
	// Запоминаем размер списка ошибок.
	unsigned int errors_cnt = vErrorDescriptions.size();

	// Текущая позиция разбора.
	unsigned int pos = aPos;

	// Код ошибки.
	InfEngineErrors iee;

	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos + 1 >= aStringLength || aString[pos] != '@' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Выделение имени алиаса и приведение к нижнему регистру.
	++pos;
	aTextString AliasName;
	unsigned int end = TextFuncs::FindSymbol( aString, aStringLength, "(", pos );
	if( pos >= aStringLength )
		return INF_ENGINE_ERROR_CANT_PARSE;

	nlReturnCode nlrc = AliasName.assign( aString + pos, end - pos );
	if( nlrc == nlrcErrorNoFreeMemory )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	else if( nlrc != nlrcSuccess )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL,      "Failed to parse alias name" );

	AllTrim( AliasName );
	TextFuncs::ToLower( AliasName.ToChar(), AliasName.size() );

	// Поиск алиаса в реестре алиасов.
	const AliasRegistry & aAliasRegistry = vDLDataWR.GetAliasRegistry();
	const Alias * aAlias = aAliasRegistry.GetAlias( AliasName.ToConstChar(), AliasName.size() );
	if( !aAlias )
		return INF_ENGINE_ERROR_UNKNOWN_ALIAS;


	pos = end + 1;
	// Сдвиги к аргументам алиаса.
	unsigned int * ArgShifts = nAllocateObjects( aAllocator, unsigned int, aAlias->GetArgumentsCount() * 2 );
	if( aAlias->GetArgumentsCount() && !ArgShifts )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Номер разбираемого в текущий момент аргументаю
	unsigned int ArgNum = 0;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );
	if( pos >= aStringLength )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Запоминаем количество элементов в результате.
	unsigned int ResultSize = aResult.size();

	// Значение этой переменной игнорируется.
	bool ArgIsMultiple;

	// Разбор агрументво алиаса.
	while( pos < aStringLength && aString[pos] != ')' )
	{
		// Если разбирается не первый аргумент.
		if( ArgNum )
		{
			if( aString[pos] == ',' )
			{
				// Пропускаем запятую.
				++pos;

				// Пропуск пробелов.
				pos = TextFuncs::SkipSpaces( aString, pos, aStringLength );
				if( pos >= aStringLength )
				{
					aResult.resize( ResultSize );
					return INF_ENGINE_WARN_UNSUCCESS;
				}
			}
			// Неверный формат: ожидается запятая, разделяющая аргументы.
			else
			{
				aResult.resize( ResultSize );
				ReturnWithParseError( DLErrorId::peFuncOrAliasArgComma, pos );
			}
		}

		// Проверяем на наличие очередного аргумента.
		if( aString[pos] != '"' )
		{
			aResult.resize( ResultSize );
			ReturnWithParseError( DLErrorId::peFuncOrAliasArgOpenQuote, pos );
		}

		// Проверяем не привышено ли количество аргументов у функции.
		if( ArgNum >= aAlias->GetArgumentsCount() )
		{
			aResult.resize( ResultSize );
			ReturnWithParseWarning( DLErrorId::peAliasInvalidArgMax, pos );
		}

		// Запоминаем позицию начала аргумента.
		ArgShifts[2 * ArgNum] = pos + 1;

		// Разбираем очередной аргумент.
		if( ( iee = ParseTagFunctionArg( aString, aStringLength, pos, aResult, aWaitingForResolving, aAllocator, '"', '"',
										 *aAlias->GetArgumentType( ArgNum ), ArgIsMultiple, aIsFunctionArg, false ) ) != INF_ENGINE_SUCCESS )
		{
			if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			{
				// Отмечаем все полученных в ходе разбора алиаса ошибки как предупреждения.
				if( vErrorDescriptions.size() > errors_cnt )
				{
					if( !vWarningDescriptions.push_back( std::move( vErrorDescriptions ) ) )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				}

				// Пропускаем алиас.
				unsigned int braket = 1;
				while( pos < aStringLength )
				{
					if( aString[pos] == '[' )
						++braket;
					else if( aString[pos] == ']' )
						--braket;
					if( !braket )
						break;
					++pos;
				}

				// Удаяляем из вектора результатов временно размещённые элементы.
				aResult.resize( ResultSize );

				aPos = pos;
				return INF_ENGINE_SUCCESS;
			}
			else
			{
				aResult.resize( ResultSize );
				return iee;
			}
		}

		// Запоминаем позицию конца аргумента.
		ArgShifts[2 * ArgNum + 1] = pos;

		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
		if( pos >= aStringLength )
		{
			aResult.resize( ResultSize );
			return INF_ENGINE_WARN_UNSUCCESS;
		}

		++ArgNum;
	}

	// Удаяляем из вектора результатов временно размещённые элементы.
	aResult.resize( ResultSize );

	// Проверяем, верное ли передано алиасу количество аргументов.
	if( ArgNum < aAlias->GetMinArgumentsCount() )
		ReturnWithParseWarning( DLErrorId::peAliasInvalidArgMin, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
	if( pos >= aStringLength )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Проверяем корректность окончания тэга-алиаса.
	if( aString[pos] != ']' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Подготовка аргументов алиаса.
	const char ** ArgVal = nAllocateObjects( aAllocator, const char *, ArgNum );
	if( ArgNum && !ArgVal )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	unsigned int * ArgLen = nAllocateObjects( aAllocator, unsigned int, ArgNum );
	if( ArgNum && !ArgLen )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	for( unsigned int arg_n = 0; arg_n < ArgNum; ++arg_n )
	{
		ArgVal[arg_n] = aString + ArgShifts[2 * arg_n];
		ArgLen[arg_n] = ArgShifts[2 * arg_n + 1] - ArgShifts[2 * arg_n];
	}

	// Результат раскрытия алиаса.
	char * Res = nullptr;
	unsigned int ResLength = 0;

	// Раскрываем алиас.
	iee = aAlias->Apply( ArgVal, ArgLen, ArgNum, aAllocator, Res, ResLength );
	if( iee == INF_ENGINE_ERROR_INVALID_ARG )
		ReturnWithParseWarning( DLErrorId::peAliasInvalidArgValue, pos );
	if( iee == INF_ENGINE_ERROR_NOFREE_MEMORY )
		ReturnWithWarn( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	if( iee != INF_ENGINE_SUCCESS || !Res )
		ReturnWithWarn( iee,                            "Failed to apply alias" );

	// Компилируем код, в который раскрылся алиас.
    iee = ParseAnsTypePatternString( Res, ResLength, 0, aResult, aWaitingForResolving, aAllocator, IsMultiple, aIsFunctionArg );

	if( iee == INF_ENGINE_ERROR_CANT_PARSE )
	{
		// Отмечаем все полученных в ходе разбора алиаса ошибки как предупреждения.
		if( vErrorDescriptions.size() > errors_cnt )
		{
			if( !vWarningDescriptions.push_back( std::move( vErrorDescriptions ) ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}

		// Удаяляем из вектора результатов временно размещённые элементы.
		aResult.resize( ResultSize );
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return iee;

	if( iee == INF_ENGINE_ERROR_INTERNAL ||
		iee == INF_ENGINE_ERROR_NOFREE_MEMORY )
		ReturnWithTrace( iee );

	if( iee == INF_ENGINE_SUCCESS )
	{
		aPos = pos;
		return INF_ENGINE_SUCCESS;
	}

	ReturnWithError( iee, "Failed to compile applied alias" );
}

InfEngineErrors DLStringParser::ParseTagExtendAnswer( const char * aString, unsigned int aStringLength, unsigned int & aPos,
													  avector<InfPatternItems::Base *> & aResult, avector<InfPatternItems::Base *> & aWaitingForResolving, nMemoryAllocator & aAllocator, bool & aIsMultiple )
{
	if( !vStrictMode )
		return INF_ENGINE_WARN_UNSUCCESS;

	aIsMultiple = false;

	// Проверка аргументов.
	if( !aString || aPos >= aStringLength )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	unsigned int pos = aPos;
	unsigned int tag_begin = aPos;
	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка имени тэга.
	if( pos + 12 >= aStringLength || strncasecmp( aString + pos, "ExtendAnswer", 12 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 12, aStringLength );

	// Проверка на открывающуюся круглую скобку.
	if( pos >= aStringLength || aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peExternAnswerOpenRoundBrace, pos );

	avector<char *> ids;
	while( pos < aStringLength )
	{
		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

		// Конец списка идентификаторов шаблонов.
		if( aString[pos] == ')' )
			break;

		// Проверка запятой между аргументами тэга.
		if( ids.size() )
		{
			if( aString[pos] != ',' )
				ReturnWithParseError( DLErrorId::peExternAnswerOpenQuote, pos );
			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );
		}

		// Проверка кавычки, предваряющей идентификатор шаблона.
		if( aString[pos] != '"' )
			ReturnWithParseError( DLErrorId::peExternAnswerOpenQuote, pos );

		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

		// Выделение идентификатора шаблона.
		unsigned int begin = pos;
		unsigned int end = pos;
		while( pos < aStringLength && aString[pos] != '"'  )
		{
			if( !TextFuncs::IsSpace( aString[pos] ) )
				end = pos;
			++pos;
		}
		if( begin == end )
			ReturnWithParseError( DLErrorId::peExternAnswerEmptyId, begin );

		// Копирование идентификатора шаблона.
		char ** id = ids.grow();
		if( !id )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		*id = nAllocateObjects( aAllocator, char, end - begin + 2 );
		if( !*id )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( *id, aString + begin, end - begin + 1 );
		( *id )[end - begin + 1] = '\0';

		// Проверка кавычки, завершающей идентификатор шаблона.
		if( aString[pos] != '"' )
			ReturnWithParseError( DLErrorId::peExternAnswerCloseQuote, pos );
	}

	if( pos >= aStringLength || aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peExternAnswerCloseRoundBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка закрывающей квадратной скобки.
	if( pos >= aStringLength || aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peExternAnswerCloseSquareBrace, pos );

	// Проверка тэга на пустоту.
	if( !ids.size() )
		ReturnWithParseError( DLErrorId::peExternAnswerEmptyId, aPos );

	// Связывание идентификаторов шаблонов с их внутренними индексами.
	unsigned int * internal_ids = nAllocateObjects( aAllocator, unsigned int, ids.size() );
	if( !internal_ids )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	for( unsigned int id_n = 0; id_n < ids.size(); ++id_n )
	{
		const unsigned int * int_id = vTablePatternsId.Search( ids[id_n] );
		if( int_id )
		{
			// Шаблон с такин названием уже известен на текущий момент.
			internal_ids[id_n] = *int_id;
		}
		else
		{
			// Занесение впервые встреченного названия шаблона в индекс.
			unsigned int id_num = 0;
			NanoLib::NameIndex::ReturnCode rt = vTablePatternsId.AddName( ids[id_n], id_num );
			if( rt != NanoLib::NameIndex::rcSuccess )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

			internal_ids[id_n] = id_num;

			if( vTableIndexId.size() != id_num )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

			// Установка значения по умолчанию, которое будет заменено при компиляции шаблона с данным названием.
			vTableIndexId.push_back( static_cast<unsigned int>( -1 ) );
			if( vTableIndexId.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Сохранение информации, для возможного сообщения об ошибке.
			if( vExtendAnswerErrors.size() != id_num )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

			ErrorReport err;
			try
			{
				err.vPos = tag_begin;
				err.vCode = DLErrorId::peExternAnswerUnregistredId;
				err.vFileName.assign( vFileName );
				err.vString.assign( aString, aStringLength );
				err.vPatternId.assign( vPatternId );
				vExtendAnswerErrors.push_back( std::move( err ) );
			}
			catch( ... )
			{
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
		}
	}

	// Создание тэга.
	InfPatternItems::TagExtendAnswer * tag = nAllocateObject( aAllocator, TagExtendAnswer );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagExtendAnswer;

	const char ** buffer = nAllocateObjects( aAllocator, const char *, ids.size() );
	if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( buffer, ids.get_buffer(), ids.size() * sizeof( char * ) );
	tag->SetPatternsId( buffer, internal_ids, ids.size() );

	// Добавление тэга к результату.
	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Добавление тэга к списку тэгов, требующих разбора всех шаблоново для своей компиляции.
	aWaitingForResolving.push_back( tag );
	if( aWaitingForResolving.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::ParseTagRSS( const char * aString, unsigned int aStringLength, unsigned int & aPos,
											 avector<InfPatternItems::Base *> & aResult, nMemoryAllocator & aAllocator,
											 bool & aIsMultiple )
{
	aIsMultiple = false;

	// Проверка аргументов.
	if( !aString || aPos >= aStringLength )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	unsigned int pos = aPos;
	// Проверка на открывающуюся квадратную скобку.
	if( aString[pos] != '[' )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка имени тэга.
	if( pos + 3 >= aStringLength || strncasecmp( aString + pos, "rss", 3 ) )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 3, aStringLength );

	// Проверка на открывающуюся круглую скобку.
	if( pos >= aStringLength || aString[pos] != '(' )
		ReturnWithParseError( DLErrorId::peRSSOpenRoundBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	if( pos > aStringLength )
		ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );

	// Разбор аргументов.
	char * ArgURL = nullptr;
	unsigned int ArgURLLength = 0;
	char * ArgAlt = nullptr;
	unsigned int AltLength = 0;
	unsigned int ArgOffset = 1;
	bool ArgShowTitle = false;
	bool ArgShowLink = false;
	unsigned int ArgUpdatePeriod = 5;

	// Разбор аргумента URL.
	unsigned int end = GetText( aString, aStringLength, pos, '"', '"' );
	if( pos == end )
		ReturnWithParseError( DLErrorId::peRSSURLInvalid, pos );

	// Выделение памяти.
	ArgURL = nAllocateObjects( aAllocator, char, end - pos - 1 );
	if( !ArgURL )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( ArgURL, aString + pos + 1, end - pos - 2 );
	ArgURLLength = end - pos - 2;
	ArgURL[ArgURLLength] = '\0';

	// Обработка эскейп символов.
	ArgURLLength = TextFuncs::RemoveEscapeSymbols( ArgURL, ArgURLLength );

	// Очистка начальных пробельных символов.
	for(; ArgURLLength && TextFuncs::IsSpace( *ArgURL ); ArgURL++, ArgURLLength-- ) ;

	// Очистка завершаущих пробельных символов.
	for(; ArgURLLength && TextFuncs::IsSpace( ArgURL[ArgURLLength - 1] ); ArgURLLength-- ) ;

	// Проверка на пустоту.
	if( !ArgURLLength )
		ReturnWithParseError( DLErrorId::peRSSURLInvalid, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

	// Разбор необязательных аргументов.
	if( pos >= aStringLength )
		ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );
	else if( aString[pos] == ',' )
	{
		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

		if( pos > aStringLength )
			ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );

		// Разбор аргумента Alt.
		unsigned int end = GetText( aString, aStringLength, pos, '"', '"' );
		if( pos == end )
			ReturnWithParseError( DLErrorId::peRSSAltInvalid, pos );

		// Выделение памяти.
		ArgAlt = nAllocateObjects( aAllocator, char, end - pos - 1 );
		if( !ArgAlt )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		memcpy( ArgAlt, aString + pos + 1, end - pos - 2 );
		AltLength = end - pos - 2;
		ArgAlt[AltLength] = '\0';

		// Обработка эскейп символов.
		AltLength = TextFuncs::RemoveEscapeSymbols( ArgAlt, AltLength );

		// Очистка начальных пробельных символов.
		for(; AltLength && TextFuncs::IsSpace( *ArgAlt ); ArgAlt++, AltLength-- ) ;

		// Очистка завершаущих пробельных символов.
		for(; AltLength && TextFuncs::IsSpace( ArgAlt[AltLength - 1] ); AltLength-- ) ;

		// Пропуск пробелов.
		pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

		if( pos >= aStringLength )
			ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );
		else if( aString[pos] == ',' )
		{
			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

			if( pos > aStringLength )
				ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );

			// Разбор аргумента Offset.
			unsigned int end = GetText( aString, aStringLength, pos, '"', '"' );
			if( pos == end )
				ReturnWithParseError( DLErrorId::peRSSOffsetInvalid, pos );

			// Выделение памяти.
			if( vtmp.assign( aString + pos + 1, end - pos - 2 ) != nlrcSuccess )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Обработка эскейп символов.
			RemoveEscapeSymbols( vtmp );
			AllTrim( vtmp );

			// Проверка на пустоту.
			if( vtmp.empty() )
				ReturnWithParseError( DLErrorId::peRSSOffsetInvalid, pos );

			// Преобразование в число.
			ArgOffset = 0;
			const char * Buffer = vtmp.ToConstChar();
			unsigned int BufferLength = vtmp.size();
			for(; BufferLength && TextFuncs::IsDigit( *Buffer ); Buffer++, BufferLength-- )
			{
				ArgOffset *= 10;
				ArgOffset += *Buffer - '0';
			}

			if( BufferLength )
				ReturnWithParseError( DLErrorId::peRSSOffsetInvalid, pos );

			// Пропуск пробелов.
			pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

			if( pos >= aStringLength )
				ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );
			else if( aString[pos] == ',' )
			{
				// Пропуск пробелов.
				pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

				if( pos > aStringLength )
					ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );

				// Разбор аргумента ShowTitle.
				unsigned int end = GetText( aString, aStringLength, pos, '"', '"' );
				if( pos == end )
					ReturnWithParseError( DLErrorId::peRSSShowTitleInvalid, pos );

				// Выделение памяти.
				if( vtmp.assign( aString + pos + 1, end - pos - 2 ) != nlrcSuccess )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

				// Обработка эскейп символов.
				RemoveEscapeSymbols( vtmp );

				AllTrim( vtmp );

				// Проверка на пустоту.
				if( !strcasecmp( vtmp.ToConstChar(), "t" ) || !strcasecmp( vtmp.ToConstChar(), "true" ) || !strcasecmp( vtmp.ToConstChar(), "1" ) )
					ArgShowTitle = true;
				else if( !strcasecmp( vtmp.ToConstChar(), "f" ) || !strcasecmp( vtmp.ToConstChar(), "false" ) || !strcasecmp( vtmp.ToConstChar(), "0" ) )
					ArgShowTitle = false;
				else
					ReturnWithParseError( DLErrorId::peRSSShowTitleInvalid, pos );

				// Пропуск пробелов.
				pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

				if( pos >= aStringLength )
					ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );
				else if( aString[pos] == ',' )
				{
					// Пропуск пробелов.
					pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

					if( pos > aStringLength )
						ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );

					// Разбор аргумента ShowLink.
					unsigned int end = GetText( aString, aStringLength, pos, '"', '"' );
					if( pos == end )
						ReturnWithParseError( DLErrorId::peRSSShowLinkInvalid, pos );

					// Выделение памяти.
					if( vtmp.assign( aString + pos + 1, end - pos - 2 ) != nlrcSuccess )
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

					// Обработка эскейп символов.
					RemoveEscapeSymbols( vtmp );

					AllTrim( vtmp );

					// Проверка на пустоту.
					if( !strcasecmp( vtmp.ToConstChar(), "t" ) || !strcasecmp( vtmp.ToConstChar(), "true" ) || !strcasecmp( vtmp.ToConstChar(), "1" ) )
						ArgShowLink = true;
					else if( !strcasecmp( vtmp.ToConstChar(), "f" ) || !strcasecmp( vtmp.ToConstChar(), "false" ) || !strcasecmp( vtmp.ToConstChar(), "0" ) )
						ArgShowLink = false;
					else
						ReturnWithParseError( DLErrorId::peRSSShowLinkInvalid, pos );

					// Пропуск пробелов.
					pos = TextFuncs::SkipSpaces( aString, end, aStringLength );

					if( pos >= aStringLength )
						ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );
					else if( aString[pos] == ',' )
					{
						// Пропуск пробелов.
						pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

						if( pos > aStringLength )
							ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );

						// Разбор аргумента UpdatePeriod.
						unsigned int end = GetText( aString, aStringLength, pos, '"', '"' );
						if( pos == end )
							ReturnWithParseError( DLErrorId::peRSSUpdatePeriodInvalid, pos );

						// Выделение памяти.
						if( vtmp.assign( aString + pos + 1, end - pos - 2 ) != nlrcSuccess )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

						// Обработка эскейп символов.
						RemoveEscapeSymbols( vtmp );

						AllTrim( vtmp );

						// Проверка на пустоту.
						if( vtmp.empty() )
							ReturnWithParseError( DLErrorId::peRSSUpdatePeriodInvalid, pos );

						// Преобразование в число.
						ArgUpdatePeriod = 0;
						const char * Buffer = vtmp.ToConstChar();
						unsigned int BufferLength = vtmp.size();
						for(; BufferLength && TextFuncs::IsDigit( *Buffer ); Buffer++, BufferLength-- )
						{
							ArgUpdatePeriod *= 10;
							ArgUpdatePeriod += *Buffer - '0';
						}

						if( BufferLength )
							ReturnWithParseError( DLErrorId::peRSSUpdatePeriodInvalid, pos );

						// Пропуск пробелов.
						pos = TextFuncs::SkipSpaces( aString, end, aStringLength );
					}
				}
			}
		}
	}

	// Проверка на закрывающуюся круглую скобку.
	if( pos >= aStringLength || aString[pos] != ')' )
		ReturnWithParseError( DLErrorId::peRSSCloseRoundBrace, pos );

	// Пропуск пробелов.
	pos = TextFuncs::SkipSpaces( aString, pos + 1, aStringLength );

	// Проверка на закрывающуюся квадратную скобку.
	if( pos >= aStringLength || aString[pos] != ']' )
		ReturnWithParseError( DLErrorId::peRSSCloseSquareBrace, pos );

	// Выделение памяти под тэг.
	nAllocateNewObject( aAllocator, InfPatternItems::TagRSS, tag );
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr *)tag ) InfPatternItems::TagRSS;

	// Установка аргументов.
	tag->SetURL( ArgURL, ArgURLLength );
	tag->SetAlt( ArgAlt, AltLength );
	tag->SetOffset( ArgOffset );
	tag->SetShowTitle( ArgShowTitle );
	tag->SetShowLink( ArgShowLink );
	tag->SetUpdatePeriod( ArgUpdatePeriod );

	// Добавление разобранного тэга в контейнер для результата.
	aResult.push_back( tag );
	if( aResult.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	aPos = pos;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::CreateTrueCondition( InfPatternItems::TagFunction * & aCondition, nMemoryAllocator & aAllocator )
{
	// Получение идентификатора функции.
	unsigned int FuncId;
	InfEngineErrors iee = vDLDataWR.GetFunctionsRegistry().Search( "true", strlen( "true" ), FuncId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Выделение памяти.
	aCondition = new( aAllocator ) InfPatternItems::TagFunction;
	if( !aCondition )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Установка идентификатора.
	aCondition->Set( FuncId );

	if( vStrictMode )
	{
		unsigned int aFuncTagId;
		vDLDataWR.RegistrateTagFunction( aCondition, aFuncTagId, false );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::CreateIsEqualCondition( InfPatternItems::TagFunction * & aCondition,
														InfPatternItems::Base * aArg1, InfPatternItems::Base * aArg2,
														nMemoryAllocator & aAllocator )
{
	// Проверка аргументов.
	if( InfPatternItems::itFunctionArg != aArg1->GetType() || InfPatternItems::itFunctionArg != aArg2->GetType() )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "itFunctionArg expected" );

	// Получение идентификатора функции.
	unsigned int FuncId;
	InfEngineErrors iee = vDLDataWR.GetFunctionsRegistry().Search( "isequal", strlen( "isequal" ), FuncId );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );

	// Выделение памяти.
	auto tag = new( aAllocator ) InfPatternItems::TagFunction;
	if( !tag )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Установка идентификатора.
	tag->Set( FuncId );

	// Установка аргументов функции.
	auto args = new( aAllocator ) InfPatternItems::TagFunction::Argument *[2];
	if( !args )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	args[0] = static_cast<InfPatternItems::TagFunction::Argument *>( aArg1 );
	args[1] = static_cast<InfPatternItems::TagFunction::Argument *>( aArg2 );
	tag->SetArgs( args, 2 );

	if( vStrictMode )
	{
		unsigned int aFuncTagId;
		vDLDataWR.RegistrateTagFunction( tag, aFuncTagId, false );
	}

	aCondition = tag;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DLStringParser::CheckVariable( const char * aName, unsigned int aNameLength, Vars::Id & aVarId )
{
	// Проверка на временную переменную.
	if( vtmp.size() >= 3 && vtmp[0] == '_' && vtmp[vtmp.size() - 1] == '_' )
	{
		// Добавление имени переменной в реестр временных переменных.
		try
		{
			aVarId = { vTmpVarsRegistry.insert( std::pair<std::string,unsigned int>( vtmp.ToConstChar(), (unsigned int)vTmpVarsRegistry.size() ) ).first->second, true };
		}
		catch( ... )
		{
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
		return INF_ENGINE_SUCCESS;
	}

	// Поиск переменной в реестре.
	aVarId = vDLDataWR.GetVarsRegistry().Search( aName, aNameLength );
	if( !aVarId )
	{
		// Проверка на авторегистрируемую переменную.
		if( aNameLength > 13 && !strncasecmp( aName, "auto_reg_var_", 13 ) )
		{
			auto iee = vDLDataWR.GetVarsRegistry().Registrate( aName, aNameLength, aVarId, false );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
			else
				return iee;
		}

		return INF_ENGINE_WARN_UNSUCCESS;
	}
	else
		return INF_ENGINE_SUCCESS;
}


InfEngineErrors DLStringParser::CheckLists( InfPatternItems::Base ** aFrom, InfPatternItems::Base ** aTo, unsigned int & aListsCount )
{
	auto iee = INF_ENGINE_SUCCESS;

	while( aFrom < aTo )
	{
		switch( aFrom[0]->GetType() )
		{
		case itStartUList:
		case itStartOList:
			aListsCount++;
			break;
		case itListItem:
			if( !aListsCount )
                aListsCount = 1;
			break;
		case itEndList:
			if( aListsCount )
				aListsCount--;
			else
				AddParseWarning( DLErrorId::peEndListToMuch, aFrom[0]->GetPos() );
			break;
		case itIf:
			{
				auto tagif = static_cast<InfPatternItems::TagIf*>( aFrom[0] );

				unsigned int tlists = aListsCount, flists = aListsCount;
				TryWithTrace( CheckLists( tagif->GetSuccessBody().begin(), tagif->GetSuccessBody().end(), tlists ), INF_ENGINE_SUCCESS );
				TryWithTrace( CheckLists( tagif->GetFailedBody().begin(), tagif->GetFailedBody().end(), flists ), INF_ENGINE_SUCCESS );

				if( tlists != flists )
				{
					AddParseError( DLErrorId::peDisbalanseIfLists, aFrom[0]->GetPos() );
					iee = INF_ENGINE_ERROR_CANT_PARSE;
				}
				else
					aListsCount = tlists;

				break;
			}
		case itSwitch:
			{
				auto tagswitch = static_cast<InfPatternItems::TagSwitch*>( aFrom[0] );

				unsigned int plists = aListsCount;

				for( unsigned int i = 0; i < tagswitch->GetCasesNumber(); i++ )
				{
                    unsigned int glists = 0;
					TryWithTrace( CheckLists( tagswitch->GetCaseBody( i )->begin(), tagswitch->GetCaseBody( i )->end(), glists ), INF_ENGINE_SUCCESS );
					if( i == 0 )
						plists = glists;
					else
					{
						if( plists != glists )
							AddParseError( DLErrorId::peDisbalanseSwicthLists, aFrom[0]->GetPos() );
					}
				}
				if( tagswitch->GetDefault() && tagswitch->GetCasesNumber() )
				{
                    unsigned int glists = 0;
                    TryWithTrace( CheckLists( tagswitch->GetDefault()->begin(), tagswitch->GetDefault()->end(), glists ), INF_ENGINE_SUCCESS );
					if( plists != glists )
						AddParseError( DLErrorId::peDisbalanseSwicthLists, aFrom[0]->GetPos() );
				}

				aListsCount = plists;

				break;
			}
		default:
			break;
		}
		aFrom++;
	}
	return iee;
}
