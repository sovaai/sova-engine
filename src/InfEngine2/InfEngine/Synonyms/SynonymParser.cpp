#include "SynonymParser.hpp"
#include <NanoLib/TextFuncs.hpp>
#include <ios>

#define MONAD( aResult, aStateType, aState, aProcess ) \
	aStateType monad_state = aState; \
	aStateType good_state = aState; \
	aTextString & result = aResult; \
	auto res = aProcess;

#define M_APPEND( aStr ) good_state == (monad_state = result.append( aStr ))

#define ADD_ERROR_MESSAGE( aLineNumber, aMessage, aString ) \
	do { \
		MONAD( vErrors, \
			   nlReturnCode, \
			   nlrcSuccess, \
			   M_APPEND( "Line " ) && \
			   M_APPEND( aLineNumber ) && \
			   M_APPEND( ": " ) && \
			   M_APPEND( aMessage ) && \
			   M_APPEND( aString ) && \
			   M_APPEND( "\n" ) \
			 ); \
		if( !res ) \
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Internal error. Can't add error message." ); \
    \
		if( monad_state != nlrcSuccess ) \
		{ \
			if( monad_state == nlrcErrorNoFreeMemory ) \
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY ); \
			else \
				ReturnWithError( INF_ENGINE_ERROR_NANOLIB, "Internal error. Can't add error message." ); \
		} \
	} while( 0 )

InfEngineErrors SynonymParser::ParseFromFile( const char * aFilePath, NanoLib::Encoding aEncoding )
{
	// Текущая строка файла.
	aTextString aLine;

	// Открытие файла.
	FILE * aFH = fopen( aFilePath, "r" );
	if( !aFH )
		return INF_ENGINE_ERROR_FILE;

	// Чтение из файла.
	nlReturnCode nlr = nlrcSuccess;
	unsigned int aLineNumber = 0;
	while((nlr = aLine.ReadString( aFH )) == nlrcSuccess )
	{
		if( NanoLib::ConvertEncoding( aLine, aEncoding, NanoLib::Encoding::UTF8 ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Нормализация строки.
		AllTrim( aLine );

		++aLineNumber;
		InfEngineErrors iee = ParseString( aLine.ToConstChar(), aLine.size(), aLineNumber );
		// Если текущая строка пустая - пропускаем её, не считая.
		if( INF_ENGINE_ERROR_EMPTY_STRING == iee )
			--aLineNumber;
		// Если текущая строка содержит ошибку - пропускаем её.
		else if( INF_ENGINE_ERROR_CANT_PARSE == iee )
			continue;
		else if( INF_ENGINE_SUCCESS != iee )
			ReturnWithTrace( iee );
	}
	fclose( aFH );

	if( nlr != nlrcEOF )
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't read file list from %s. Return code: %d",
						 aFilePath, nlr );

	return vErrors.size() ? INF_ENGINE_ERROR_CANT_PARSE : INF_ENGINE_SUCCESS;
}

InfEngineErrors SynonymParser::ParseString( const char * aString, unsigned int aLength, unsigned int aLineNumber )
{
	unsigned int pos = 0;
	SynonymGroup * sgroup = vSynonyms.grow();
	if( !sgroup )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Выделяем главный синоним.
	InfEngineErrors iee = ParseSynonym( aString, pos, aLength, sgroup->Major, aLineNumber );
	if( iee != INF_ENGINE_SUCCESS )
	{
		vSynonyms.pop_back();

		if( iee == INF_ENGINE_ERROR_EMPTY_STRING )
			return INF_ENGINE_ERROR_EMPTY_STRING;

		if( iee == INF_ENGINE_ERROR_CANT_PARSE )
			return INF_ENGINE_ERROR_CANT_PARSE;

		ReturnWithTrace( iee );
	}

	// Выделяем знак равенства.
	pos = TextFuncs::SkipSpaces( aString, pos, aLength );
	if( aString[pos] != '=' )
	{
		vSynonyms.pop_back();
		ADD_ERROR_MESSAGE( aLineNumber, "Symbol = expected in line: ", aString );
		return INF_ENGINE_ERROR_CANT_PARSE;
	}
	++pos;

	if( pos >= aLength )
	{
		ADD_ERROR_MESSAGE( aLineNumber, "At least one synonym expected: ", aString );
		return INF_ENGINE_ERROR_CANT_PARSE;
	}

	// Выделяем неглавные синонимы.
	while( pos < aLength )
	{
		Synonym * minor = sgroup->Minors.grow();
		if( !minor )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		if( INF_ENGINE_SUCCESS != (iee = ParseSynonym( aString, pos, aLength, *minor, aLineNumber )))
		{
			vSynonyms.pop_back();
			if( iee == INF_ENGINE_ERROR_EMPTY_STRING )
			{
				ADD_ERROR_MESSAGE( aLineNumber, "Empty synonym: ", aString );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}
			ReturnWithTrace( iee );
		}

		pos = TextFuncs::SkipSpaces( aString, pos, aLength );
		if( pos < aLength )
		{
			if( aString[pos] == '/' )
			{
				++pos;
			}
			else
			{
				vSynonyms.pop_back();
				ADD_ERROR_MESSAGE( aLineNumber, "Symbol '/' expected instead: ", aString + pos );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors SynonymParser::ParseSynonym( const char * aString, unsigned int & aPos, unsigned int aLength,
											 Synonym & aSynonym, unsigned int aLineNumber )
{
	// Пропускаем пробельные символы перед синонимом.
	aPos = TextFuncs::SkipSpaces( aString, aPos, aLength );

	unsigned int syn_begin = aPos;
	unsigned int syn_end = aPos;
	unsigned int pos = aPos;

	// Находим позицию конца синонима.
	while( pos < aLength )
	{
		UChar32 ch;
		U8_NEXT_UNSAFE( aString, pos, ch );
		//        ch = u_toupper(ch);
		if( ch != '_' && ch != '-' && !u_isalnum( ch ))

		{
			if( ch == '/' || ch == '=' || TextFuncs::IsSpace( ch ))
				break;
			else
			{
				ADD_ERROR_MESSAGE( aLineNumber, "Unexpected symbol: ", aString + syn_end );
				return INF_ENGINE_ERROR_CANT_PARSE;
			}
		}
		syn_end = pos;
	}
	if( syn_end == syn_begin )
		return INF_ENGINE_ERROR_EMPTY_STRING;

	// Выделяем память и копируем в неё синоним.
	aPos = syn_end;
	aSynonym.Length = syn_end - syn_begin;
	aSynonym.Text = nAllocateObjects( vMemoryAllocator, char, aSynonym.Length + 1 );
	if( !aSynonym.Text )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( aSynonym.Text, aString + syn_begin, aSynonym.Length );
	aSynonym.Text[aSynonym.Length] = '\0';

	return INF_ENGINE_SUCCESS;
}

void SynonymParser::Reset()
{
	vSynonyms.clear();
	vErrors.clear();
	vMemoryAllocator.Reset();
}
