#include "VoiceNumberParser.hpp"
#include "KeyboardNumberParser.hpp"

#include <NanoLib/TextFuncs.hpp>

nlReturnCode VoiceNumberParser::Parse( LongFloat & aResult, const char * aStr, unsigned int aStrLen )
{
	//    printf( "\nLOG:\t VoiceNumberParser::Parse BEGIN  \"%s\"\n", aStr );

	// Вычисление длины входной строки.
	if( !aStrLen )
		aStrLen = strlen( aStr );

	// Приведение к нижнему регистру.
	aTextString lower_input;
	nlReturnCode nlrc = lower_input.assign( aStr, aStrLen );
	if( nlrcSuccess != nlrc )
		return nlrc;
	if( nlrcSuccess != ( nlrc = TextFuncs::ToLower( lower_input.ToChar(), lower_input.size() ) ) )
		return nlrc;

	// Замена числес, записанных цифрами, на прописной эквивалент.
	aTextString prepared_input;
	//    printf( "\nLOG:\t VoiceNumberParser::Parepare ... \n" );
	if( nlrcSuccess != ( nlrc = NumberParser::PrepareVoiceNumber( prepared_input, lower_input.ToConstChar(), lower_input.size() ) ) )
		return nlrc;

	//    printf( "\nLOG:\t VoiceNumberParser::Parepared \"%s\"\n", prepared_input.ToConstChar() );

	// Разбор прописной записи числа.
	KeyboardNumberParser keyboard_number_parser;
	if( nlrcSuccess != ( nlrc = keyboard_number_parser.Parse( aResult, prepared_input.ToConstChar(), prepared_input.size() ) ) )
		return nlrc;

	//    printf( "\nLOG:\t VoiceNumberParser::Parse END\n");

	return nlrcSuccess;
}
