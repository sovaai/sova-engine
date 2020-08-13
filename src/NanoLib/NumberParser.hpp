#ifndef __NumberParser_hpp__
#define __NumberParser_hpp__

#include "aTextString.hpp"
#include "LongFloat.hpp"


namespace NumberParser
{
    
class CommonParser
{
public:
    CommonParser();
    
    nlReturnCode Init();

private:
    static bool vInitialized;
    
    static nlReturnCode vStatusCode;
};


/**
 *  Нормализация числительных, десятичного разделителя и знака.
 * @param aResult
 * @param aStr
 * @param aStrLen
 */
nlReturnCode NormalizeNumerals( aTextString & aResult, const char * aStr, unsigned int aStrLen = 0 );

/**
 *  Удаление пробелов между цифрами.
 * @param aResult
 * @param aStr
 * @param aStrLen
 * @return 
 */
nlReturnCode DeleteSpaces( aTextString & aResult, const char * aStr, unsigned int aStrLen = 0 );


/**
 *  Проверка на отсутствие числительны.
 * @param aString
 * @return 
 */
bool NoNumerals( const aTextString & aString );


/**
 *  Преобразует основание ("десятых", "сотых", "тысячных", ...) к LongFloat (10, 100, 1000, ...)
 * @param aFractionalBase
 * @param aResult
 */
nlReturnCode FractionalBase2LongFloat( const char * aFractionalBase, unsigned int aLength, LongFloat & aResult );

/**
 *  Преобразует основание ("тысяч", "миллионов", "миллиардов", ...) к LongFloat (1000, 1000000, 1000000000, ...)
 * @param aBase
 * @param aResult
 */
nlReturnCode Base2LongFloat( const char * aBase, unsigned int aLength, LongFloat & aResult );

/**
 *  Преобразует числительное, обозначающе сотню ("сто", "двести", "триста", ...), к LongFloat (100, 200, 300, ...)
 * @param aHundred
 * @param aResult
 */
nlReturnCode Hundred2LongFloat( const char * aHundred, unsigned int aLength, LongFloat & aResult );

/**
 *  Преобразует числительное, обозначающе десяток ("двадцать", "тридцать", "сорок", ...), к LongFloat (20, 30, 40, ...)
 * @param aDecade
 * @param aResult
 */
nlReturnCode Decade2LongFloat( const char * aDecade, unsigned int aLength, LongFloat & aResult );

/**
 *  Преобразует числительное, единицы ("один", "два", "три", ..., "девятнадцать"), к LongFloat (1, 2, 3, ..., 19)
 * @param aUnit
 * @param aResult
 */
nlReturnCode Units2LongFloat( const char * aUnit, unsigned int aLength, LongFloat & aResult );

/**
 *  Проверяет, является ли заданое слово записью нуля. 
 */
bool IsZero( const char * aWord, unsigned int aLength );

/**
 *  Замена чисел, записанных цифрами, строчной записью (без согласования числительных).
 */
nlReturnCode PrepareVoiceNumber( aTextString & aResult, const char * aVoiceNumber, unsigned int aLen );

/**
 *  Представление вещественного числа в виде записи прописью.
 * @param aNumber
 * @param aNumeral
 */
nlReturnCode NumberToString( const aTextString & aNumber, aTextString & aNumeral );

};

#endif // __NumberParser_hpp__
