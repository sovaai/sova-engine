#include "KeyboardNumberParser.hpp"
#include "TextFuncs.hpp"
#include "NanoLib/ReturnCode.hpp"

/******************************************************************************/
/*                                                                            */
/*  SF  - знаковое вещественное                                               */
/*  UF  - беззнаковое вещественное                                            */
/*  N   - беззнаковое целое                                                   */
/*  D1  - "целых" / ""                                                        */
/*  D2  - "десятых" / "сотых" / "тысячных" / ...                              */
/*  DOT - "и" / ""                                                            */
/*  K   - беззнаковое целое 1 .. 999  цифрами или прописью                    */
/*  K3  - беззнаковое целое 001 .. 999 тремя цифрами или прописью             */
/*  R   - основание "тысяч" / "миллионов" / ...                               */
/*  D   - беззнаковое целое 1 .. 999 цифрами                                  */
/*  D3  - беззнаковое целое 001 .. 999 цифрами, обязательно три цифры         */
/*  S   - беззнаковое целое 1 .. 999 прописью                                 */
/*  KK  - беззнаковое целое больше 0 цифрами или от 1 до 999 прописью         */
/*  NN  - беззнаковое целое больше 0                                          */
/*  Z   -  0 / ноль                                                           */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      BEGIN -> SF END                                                       */
/*      SF -> +UF END | -UF END | UF END                                      */
/*      UF -> N D1 DOT N D2 END | N D1 END | N D2 END | N END                 */
/*      D1 -> целых |                                                         */
/*      D2 -> сотых | тысячных | ... |                                        */
/*      DOT -> и                                                              */
/*      N -> T K3 | T | K | Z                                                 */
/*      K -> D | S                                                            */
/*      K3 -> D3 | S                                                          */
/*      D -> 1 | 2 | ... | 999                                                */
/*      D3 -> 001 | 002 | ... | 999                                           */
/*      S -> один | два | ... | девятьсот девяносто девять                    */
/*      T -> KK R T | KK R | NN | R T | R                                     */
/*      R -> тысяч | миллионов | милиардов | ...                              */
/*      KK -> S | NN                                                          */
/*      NN -> целое беззнаковое, записанное цифрами                           */
/*                                                                            */
/******************************************************************************/

static const char * STR_DOT = "и";

static const char * STR_INTEGRAL = "целых";


// Знаковое вещественное.
nlReturnCode SF( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое вещественное.
nlReturnCode UF( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое, записанное цифрами и числительными.
nlReturnCode N( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое, записанное цифрами.
nlReturnCode NN( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое от 1 до 999, записанное цифрами или числительными (но не цифрами и числительными вперемешку!).
nlReturnCode K( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое больше 1, записанное цифрами, или от 1 до 999, записанное прописью.
nlReturnCode KK( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое от 001 до 999, записанное цифрами или числительными (но не цифрами и числительными вперемешку!).
nlReturnCode K3( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое от 1 до 999, записанное цифрами.
nlReturnCode D( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое от 1 до 999, записанное прописью.
nlReturnCode S( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// 0 / ноль.
nlReturnCode Z( const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое от 001 до 999, записанное цифрами, обязательно три цифры.
nlReturnCode D3( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое от 1 до 999, записанное прописью.
nlReturnCode S( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Беззнаковое целое больше 999, записанное цифрами и числительными.
nlReturnCode T( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Основание кратное 1000 ( "тысяч", "миллионов", "миллиардов", ... ).
nlReturnCode R( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Числительное, стоящее перед десятичной частью ("целых", "").
nlReturnCode D1( const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Числительное, стоящее после десятичной части ("десятых", "сотых", "тысячных" ...).
nlReturnCode D2( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Десятичный разделитель ( "и", "" ).
nlReturnCode DOT( const char * aStr, unsigned int aStrLen, unsigned int & aPos );

// Терминальный символ.
nlReturnCode END( const char * aStr, unsigned int aStrLen, unsigned int & aPos );





// Терминальный символ.
nlReturnCode END( const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t END:  aPos=%i  aStrLen=%i   Str=\"%s\"\n", aPos, aStrLen, aStr );
    if( aPos > aStrLen )
        return nlrcUnsuccess;
    
    return aStr[aPos] == '\0' ? nlrcSuccess : nlrcUnsuccess;
}


// Десятичный разделитель ( "и", "" ).
nlReturnCode DOT( const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
    if( aPos >= aStrLen )
        return nlrcUnsuccess;
    
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    if( !strncasecmp( aStr + pos, STR_DOT, strlen( STR_DOT ) ) )
        aPos = pos + strlen( STR_DOT );
    
    return nlrcSuccess;
}


// Числительное, стоящее после десятичной части ("десятых", "сотых", "тысячных" ...).
nlReturnCode D2( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t\t\t D2:  BEGIN for \"%s\"\n", aStr + aPos );
    if( aPos >= aStrLen )
        return nlrcUnsuccess;
    
    // Пропуск пробелов.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    unsigned int end = pos;    
    
    // Выделение слова, в котором, предположительно, находится основание.
    while( end < aStrLen && !TextFuncs::IsSpace( aStr[end] ) )
        ++end;    
    if( end == pos )
    {
//        printf( "\nLOG:\t\t\t D2:  FAIL1\n" );
        return nlrcUnsuccess;    
    }
    
    // Проверка основания и его преобразование к LongFloat. 
    nlReturnCode nlrc = NumberParser::FractionalBase2LongFloat( aStr + pos, end - pos, aResult );
    if( nlrcSuccess != nlrc )
    {
//        printf( "\nLOG:\t\t\t D2:  FAIL2\n" );
        return nlrc;
    }
    
    aPos = end;    
//    printf( "\nLOG:\t\t\t D2:  SUCCESS1\n" );
    return nlrcSuccess;
}


// Числительное, стоящее перед десятичной частью ("целых", "").
nlReturnCode D1( const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
    if( aPos >= aStrLen )
        return nlrcUnsuccess;
    
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    if( !strncasecmp( aStr + pos, STR_INTEGRAL, strlen( STR_INTEGRAL ) ) )
        aPos = pos + strlen( STR_INTEGRAL );
    
    return nlrcSuccess;
}


// Основание кратное 1000 ( "тысяч", "миллионов", "миллиардов", ... ).
nlReturnCode R( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
    if( aPos >= aStrLen )
        return nlrcUnsuccess;
    
    // Пропуск пробелов.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    unsigned int end = pos;    
    
    // Выделение слова, в котором, предположительно, находится основание.
    while( end < aStrLen && !TextFuncs::IsSpace( aStr[end] ) )
        ++end;    
    if( end == pos )
    {
        return nlrcUnsuccess;    
    }
    
    // Проверка основания и его преобразование к LongFloat. 
    nlReturnCode nlrc = NumberParser::Base2LongFloat( aStr + pos, end - pos, aResult );
    if( nlrcSuccess != nlrc )
        return nlrc;
    
    aPos = end + ( end < aStrLen ? 1 : 0 );

//    printf( "\nLOG:\t\t\t\t   R:  SUCCESS1\n" );    
    return nlrcSuccess;
}


nlReturnCode T( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t\t\t\t T:  BEGIN for \"%s\"\n", aStr + aPos );
    
    // T -> KK R T | KK R | NN | R T | R
    nlReturnCode nlrc = nlrcSuccess;
    LongFloat kk_lf = 0;
    LongFloat r_lf = 0;
    LongFloat t_lf = 0;
    
    // Пропуск пробелов.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    unsigned int pos1 = pos;
    // T -> KK R T
    if( nlrcSuccess == ( nlrc = KK( kk_lf, aStr, aStrLen, pos1 ) ) )
    {
        unsigned int pos2 = pos1;
        if( nlrcSuccess == ( nlrc = R( r_lf, aStr, aStrLen, pos2 ) ) )
        {
            if( kk_lf < r_lf )
            {
                unsigned int pos3 = pos2;
                if( nlrcSuccess == ( nlrc = T( t_lf, aStr, aStrLen, pos3 ) ) )
                {
                    if( r_lf > t_lf )
                    {
                        aPos = pos3;
                        aResult = kk_lf * r_lf + t_lf;
//                        printf( "\nLOG:\t\t\t\t T:  SUCCESS1\n" );
                        return nlrcSuccess;
                    }
                    else
                    {
//                        printf( "\nLOG:\t\t\t\t T:  FAIL1\n" );
                        return nlrcUnsuccess;
                    }
                }
                // T -> KK R
                else
                {
                    aPos = pos2;
                    aResult = kk_lf * r_lf;
//                    printf( "\nLOG:\t\t\t\t T:  SUCCESS2\n" );
                    return nlrcSuccess;
                }
            }
            else
            {
//                printf( "\nLOG:\t\t\t\t T:  FAIL2\n" );
                return nlrcUnsuccess;                 
            }
        }    
        else
        {
            // T -> NN
            pos1 = pos;
            if( nlrcSuccess == ( nlrc = NN( kk_lf, aStr, aStrLen, pos1 ) ) )
            {
                aPos = pos1;
                aResult = kk_lf;
//                printf( "\nLOG:\t\t\t\t T:  SUCCESS3: \n" );
                return nlrcSuccess;
            }
        }
    }    
    // T -> R T
    else if( nlrcSuccess == ( nlrc = R( r_lf, aStr, aStrLen, pos1 ) ) )
    {
        unsigned int pos2 = pos1;
        if( nlrcSuccess == ( nlrc = T( t_lf, aStr, aStrLen, pos2 ) ) )
        {
            if( r_lf > t_lf )
            {
                aPos = pos2;
                aResult = r_lf + t_lf;
//                printf( "\nLOG:\t\t\t\t T:  SUCCESS4\n" );
                return nlrcSuccess;
            }
            else
            {
//                printf( "\nLOG:\t\t\t\t T:  FAIL3\n" );
                return nlrcUnsuccess;
            }
        }
        // T -> R
        else
        {
            aPos = pos1;
            aResult = r_lf;
//            printf( "\nLOG:\t\t\t\t T:  SUCCESS5\n" );
            return nlrcSuccess;
        }
    }
    else
    {
//        printf( "\nLOG:\t\t\t\t T:  FAIL4\n" );
        return nlrcUnsuccess;
    }
    
//    printf( "\nLOG:\t\t\t\t T:  FAIL5\n" );
    
    return nlrcUnsuccess;
}


nlReturnCode D3( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    if( pos >= aStrLen || !TextFuncs::IsDigit( aStr[pos] ) )
        return nlrcUnsuccess;
    
    int res = 0;
    for( int i = 0; i < 3; ++i )
    {
        if( pos < aStrLen && TextFuncs::IsDigit( aStr[pos] ) )
        {
            res = res * 10 + ( aStr[pos] - '0' );
            ++pos;
        }
        else
            return nlrcUnsuccess;
    }
    aPos = pos;
    aResult = res;
    
    return nlrcSuccess;
}


nlReturnCode D( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    if( pos >= aStrLen || !TextFuncs::IsDigit( aStr[pos] ) )
        return nlrcUnsuccess;
    
    int res = 0;
    for( int i = 0; i < 3 && pos < aStrLen && TextFuncs::IsDigit( aStr[pos] ); ++i )
    {
        res = res * 10 + ( aStr[pos] - '0' );
        ++pos;
    }
    aPos = pos;
    aResult = res;    
    
    return nlrcSuccess;
}

nlReturnCode Z( const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t\t\t\t\t Z:  BEGIN for \"%s\"\n", aStr + aPos );    
    
    // Пропуск ведущих пробелов.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    if( pos >= aStrLen )
    {
//        printf( "\nLOG:\t\t\t\t\t Z:  FAIL1\n" );    
        return nlrcUnsuccess;
    }
    
    // Выделение слова.
    unsigned int begin = pos;
    unsigned int end = pos;
    while( end < aStrLen && !TextFuncs::IsSpace( aStr[end] ) )
        ++end;
    
    if( NumberParser::IsZero( aStr + pos, end - pos ) )
    {
//        printf( "\nLOG:\t\t\t\t\t Z:  SUCCESS1\n" );    
        aPos = end;
        return nlrcSuccess;
    }
    
//    printf( "\nLOG:\t\t\t\t\t Z:  FAIL2\n" );    
    return nlrcUnsuccess;
}

nlReturnCode S( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t\t\t\t\t S:  BEGIN for \"%s\"\n", aStr + aPos );    
    
    // Обнуление результата.
    aResult.Zero();
    nlReturnCode nlrc = nlrcSuccess;
    
    // Пропуск ведущих пробелов.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    if( pos >= aStrLen )
    {
//        printf( "\nLOG:\t\t\t\t\t S:  FAIL1\n" );    
        return nlrcUnsuccess;
    }
    
    // Выделение слова.
    bool parsed = false;
    unsigned int begin = pos;
    unsigned int end = pos;
    while( end < aStrLen && !TextFuncs::IsSpace( aStr[end] ) )
        ++end;
    
    // Выделение  сотни.
    LongFloat tmp_lf = 0;
    if( nlrcSuccess == ( nlrc = NumberParser::Hundred2LongFloat( aStr + pos, end - pos, tmp_lf ) ) )
    {
//        printf( "\nLOG:\t\t\t\t\t S:  Hundred( \"%s\":%u, %u ) - Ok\n", aStr+begin, end, (unsigned int)tmp_lf.operator  long int() );
        aResult += tmp_lf;
        parsed = true;
        
        // Выделение слова.
        begin = end < aStrLen ? end + 1 : aStrLen;
        end = begin;
        while( end < aStrLen && !TextFuncs::IsSpace( aStr[end] ) )
            ++end;
    }
    else if( nlrcUnsuccess != nlrc )
    {
//        printf( "\nLOG:\t\t\t\t\t S:  FAIL2\n" );    
        return nlrc;
    }
    
    // Выделение десятка.
    if( nlrcSuccess == ( nlrc = NumberParser::Decade2LongFloat( aStr + begin, end - begin, tmp_lf ) ) )
    {
//        printf( "\nLOG:\t\t\t\t\t S:  Decade( \"%s\":%u, %u ) - Ok\n", aStr+begin, end, (unsigned int)tmp_lf.operator  long int() );
        
        aResult += tmp_lf;
        parsed = true;
        
        // Выделение слова.
        begin = end < aStrLen ? end + 1 : aStrLen;
        end = begin;
        while( end < aStrLen && !TextFuncs::IsSpace( aStr[end] ) )
            ++end;
    }
    else if( nlrcUnsuccess != nlrc )
    {
//        printf( "\nLOG:\t\t\t\t\t S:  FAIL3\n" );
        return nlrc;
    }
    
    // Выделение единиц.
    if( nlrcSuccess == ( nlrc = NumberParser::Units2LongFloat( aStr + begin, end - begin, tmp_lf ) ) )
    {
//        printf( "\nLOG:\t\t\t\t\t S:  Unit( \"%s\":%u, %u ) - Ok\n", aStr+begin, end, (unsigned int)tmp_lf.operator  long int() );
        
        aResult += tmp_lf;
        parsed = true;
        begin = end < aStrLen ? end + 1 : aStrLen;
    }
    else if( nlrcUnsuccess != nlrc )
    {
//        printf( "\nLOG:\t\t\t\t\t S:  FAIL4\n" );
        return nlrc;
    }
    
    // Проверка того, что хотя бы один разряд был распознан.
    if( parsed )
    {
        aPos = begin;
//        printf( "\nLOG:\t\t\t\t\t S:  SUCCESS1\n" );
        return nlrcSuccess;
    }
    
//    printf( "\nLOG:\t\t\t\t\t S:  FAIL5\n" );
    
    return nlrcUnsuccess;
}


nlReturnCode K3( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
    nlReturnCode nlrc = nlrcSuccess;
    
    // Пропуск ведущих пробелов.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    if( pos >= aStrLen )
        return nlrcUnsuccess;
    
    // K3 -> D3
    // Запись цифрами.
    if( nlrcSuccess == ( nlrc = D3( aResult, aStr, aStrLen, pos ) ) )
    {
        aPos = pos;
        return nlrcSuccess;
    }
    // K3 -> S
    // Запись прописью.
    else if( nlrcSuccess == ( nlrc = S( aResult, aStr, aStrLen, pos ) ) )
    {
        aPos = pos;
        return nlrcSuccess;
    }
    
    return nlrcUnsuccess;
}


// Беззнаковое целое, записанное цифрами.
nlReturnCode NN( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t\t\t\t\t NN:  BEGIN for \"%s\"\n", aStr + aPos );
    
    nlReturnCode nlrc = nlrcSuccess;
    
    // Пропуск пробелов.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    if( pos >= aStrLen )
    {
//        printf( "\nLOG:\t\t\t\t\t NN: FAIL1\n" );
        return nlrcUnsuccess;
    }
    
    // Выделение слова.
    unsigned int end = pos;
    while( end < aStrLen && TextFuncs::IsDigit( aStr[end] ) )
        ++end;    
    if( end == pos )
    {
//        printf( "\nLOG:\t\t\t\t\t NN: FAIL2\n" );
        return nlrcUnsuccess;
    }
    aTextString word;    
    if( nlrcSuccess!= ( nlrc = word.assign( aStr + pos, end - pos ) ) )
    {
//        printf( "\nLOG:\t\t\t\t\t NN: FAIL3\n" );
        return nlrc;
    }
    
    // Преобразование слова в число.    
    if( !aResult.sscanf( word.ToConstChar() ) )
    {
//        printf( "\nLOG:\t\t\t\t\t NN: FAIL4 (word=\"%s\"   pos=%i end=%i)\n", word.ToConstChar(), pos, end );
        return nlrcErrorInternal;
    }
    
//    printf( "\nLOG:\t\t\t\t\t NN: SUCCESS1\n" );    
    aPos = end;
    return nlrcSuccess;
}


// Беззнаковое целое больше 1, записанное цифрами, или от 1 до 999, записанное прописью.
nlReturnCode KK( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t\t\t\t   KK:  BEGIN for \"%s\"\n", aStr + aPos );
    
    // KK -> S | NN
    nlReturnCode nlrc = nlrcSuccess;
    
    // Пропуск пробелов.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    
    if( nlrcSuccess == ( nlrc = S( aResult, aStr, aStrLen, pos ) ) )
    {
        aPos = pos;
//        printf( "\nLOG:\t\t\t\t   KK:  SUCCESS1\n" );
        return nlrcSuccess;
    }
    else if( nlrcSuccess == ( nlrc = NN( aResult, aStr, aStrLen, pos ) ) )
    {
        aPos = pos;
//        printf( "\nLOG:\t\t\t\t   KK:  SUCCESS2\n" );
        return nlrcSuccess;
    }
    
//    printf( "\nLOG:\t\t\t\t   KK:  FAIL1\n" );
    return nlrcUnsuccess;
}


nlReturnCode K( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t\t\t\t K:  BEGIN for \"%s\"\n", aStr + aPos );    
    
    nlReturnCode nlrc = nlrcSuccess;
    
    // Пропуск ведущих пробелов.
    unsigned int pos = TextFuncs::SkipSpaces( aStr, aPos, aStrLen );
    if( pos >= aStrLen )
    {
//        printf( "\nLOG:\t\t\t\t K:  FAIL1\n");
        return nlrcUnsuccess;
    }
    
    // K3 -> D3
    // Запись цифрами.
    if( nlrcSuccess == ( nlrc = D( aResult, aStr, aStrLen, pos ) ) )
    {
        aPos = pos;
//        printf( "\nLOG:\t\t\t\t K:  SUCCESS1\n");
        return nlrcSuccess;
    }
    // K3 -> S
    // Запись прописью.
    else if( nlrcSuccess == ( nlrc = S( aResult, aStr, aStrLen, pos ) ) )
    {
        aPos = pos;
//        printf( "\nLOG:\t\t\t\t K:  SUCCESS2\n");
        return nlrcSuccess;
    }
    
//    printf( "\nLOG:\t\t\t\t K:  FAIL2\n");
    return nlrcUnsuccess;
}


nlReturnCode N( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t\t\t N:  BEGIN for \"%s\"\n", aStr + aPos );
    
    unsigned pos1 = aPos;
    nlReturnCode nlrc = nlrcSuccess;
    LongFloat t_value = 0;
    LongFloat k_value = 0;
    
    // N -> T K3 
    if( nlrcSuccess == ( nlrc = T( t_value, aStr, aStrLen, pos1 ) ) )
    {
        unsigned int pos2 = pos1;
        if( nlrcSuccess == ( nlrc = K3( k_value, aStr, aStrLen, pos2 ) ) )
        {
            aResult = t_value + k_value;
            aPos  = pos2;
            
//            printf( "\nLOG:\t\t\t N:  SUCCESS1\n");
            return nlrcSuccess;
        }
        // N -> T
        else 
        {
            aResult = t_value;
            aPos  = pos1;
//            printf( "\nLOG:\t\t\t N:  SUCCESS2\n");
            return nlrcSuccess;
        }
    }
    // N -> K
    else if( nlrcSuccess == ( nlrc = K( k_value, aStr, aStrLen, pos1 ) ) )
    {
        aResult = k_value;
        aPos  = pos1;
//        printf( "\nLOG:\t\t\t N:  SUCCESS3\n");
        return nlrcSuccess;
    }
    // N -> Z
    else if( nlrcSuccess == ( nlrc = Z(aStr, aStrLen, pos1 ) ) )
    {
        aResult = 0;
        aPos  = pos1;
//        printf( "\nLOG:\t\t\t N:  SUCCESS4\n");
        return nlrcSuccess;
    }
    else
    {
//        printf( "\nLOG:\t\t\t N:  FAIL1\n");
        return nlrcUnsuccess;
    }
    
//    printf( "\nLOG:\t\t\t N:  FAIL2\n");
    return nlrcUnsuccess;
}


// Беззнаковое вещественное.
nlReturnCode UF( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t\t UF:  BEGIN\n");
    
    if( aPos > aStrLen )
        return nlrcUnsuccess;
    
    // Инициализация результата.
    aResult.Zero();
    nlReturnCode nlrc = nlrcSuccess;
    unsigned int pos1 = aPos;
    unsigned int base = 0;
    LongFloat integral_part, fractional_part, fractional_base;
    
    // UF -> N D1 DOT N D2 END
    if( nlrcSuccess == ( nlrc = N( integral_part, aStr, aStrLen, pos1 ) ) )
    {
        unsigned pos2 = pos1;
        if( nlrcSuccess == ( nlrc = D1( aStr, aStrLen, pos2 ) ) )
        {
            unsigned pos3 = pos2;
            if( nlrcSuccess == ( nlrc = DOT( aStr, aStrLen, pos3 ) ) )
            {
                unsigned pos4 = pos3;
                if( nlrcSuccess == ( nlrc = N( fractional_part, aStr, aStrLen, pos4 ) ) )
                {
                    unsigned pos5 = pos4;
                    if( nlrcSuccess == ( nlrc = D2( fractional_base, aStr, aStrLen, pos5 ) ) )
                    {
                        if( nlrcSuccess == ( nlrc = END( aStr, aStrLen, pos5 ) ) )
                        {
                            aResult = integral_part + fractional_part / fractional_base;
                            aPos = pos5;
                            
//                            printf( "\nLOG:\t\t UF:  SUCCESS1\n");
                            return nlrcSuccess;
                        }
                        else
                        {
//                            printf( "\nLOG:\t\t UF:  FAIL1\n");
                            return nlrc;
                        }
                    }
                    else
                    {
//                        printf( "\nLOG:\t\t UF:  FAIL2\n");
                        return nlrc;
                    }
                }
            }
            else // UF -> N D1 END
            {                
                if( nlrcSuccess == ( nlrc = END( aStr, aStrLen, pos2 ) ) )
                {
                    aResult = integral_part;
                    aPos = pos2;                 
                    
//                    printf( "\nLOG:\t\t UF:  SUCCESS2\n");
                    return nlrcSuccess;
                }
                else
                {
//                    printf( "\nLOG:\t\t UF:  FAIL3\n");
                    return nlrc;
                }
            }
        }        

        pos2 = pos1;
        // UF -> N D2 END
        if( nlrcSuccess == ( nlrc = D2( fractional_base, aStr, aStrLen, pos2 ) ) )
        {
            if( nlrcSuccess == ( nlrc = END( aStr, aStrLen, pos2 ) ) )
            {
                aResult = integral_part / fractional_base;
                aPos = pos2;                    

//                printf( "\nLOG:\t\t UF:  SUCCESS3\n");
                return nlrcSuccess;
            }
            else
            {
//                printf( "\nLOG:\t\t UF:  FAIL4\n");
                return nlrc;
            }
        }
        // UF -> N END
        else if( nlrcSuccess == ( nlrc = END( aStr, aStrLen, pos1 ) ) )
        {
            aResult = integral_part;
            aPos = pos1;                    
//            printf( "\nLOG:\t\t UF:  SUCCESS4\n");
            return nlrcSuccess;
        }
        else
        {
//            printf( "\nLOG:\t\t UF:  FAIL5 (END <- \"%s\")\n", aStr + pos1 );
            return nlrc;
        }
    }
    else
    {
//        printf( "\nLOG:\t\t UF:  FAIL6\n");
        return nlrcUnsuccess;    
    }
    
//    printf( "\nLOG:\t\t UF:  FAIL7\n");
    return nlrcUnsuccess;    
}


// Знаковое вещественное.
nlReturnCode SF( LongFloat & aResult, const char * aStr, unsigned int aStrLen, unsigned int & aPos )
{
//    printf( "\nLOG:\t SF:  BEGIN\n");
    // Инициализация результата.
    aResult.Zero();
    
    // SF -> +UF END | -UF END | UF END
        
    // Определение знака числа.
    if( aStrLen < 1 )
        return nlrcUnsuccess;
    bool is_negated = aStr[0] == '-';
    nlReturnCode nlrc = nlrcSuccess;
    unsigned int pos1 = aStr[0] == '-' || aStr[0] == '+' ? 1 : 0;
    
    // Разбор числа без знака.    
    if( nlrcSuccess == ( nlrc = UF( aResult, aStr, aStrLen, pos1 ) ) )
    {
        unsigned int pos2 = pos1;
        if( nlrcSuccess == ( nlrc = END( aStr, aStrLen, pos2 ) ) )
        {
            // Выставление знака.
            if( is_negated )
                aResult *= -1;
            aPos = pos2;
//            printf( "\nLOG:\t SF:  SUCCESS 1\n");
            return nlrcSuccess;
        }
        else
        {
//            printf( "\nLOG:\t SF:  FAIL 1\n");
            return nlrc;
        }
    }
    else
    {
//        printf( "\nLOG:\t SF:  FAIL 2\n");
        return nlrc;
    }
                         
//    printf( "\nLOG:\t SF:  FAIL 3\n");
    return nlrcUnsuccess;    
}

nlReturnCode KeyboardNumberParser::Parse( LongFloat & aResult, const char * aStr, unsigned int aStrLen )
{
//    printf( "\nLOG:\t KeyboardNumberParser::Parse BEGIN\n");
    
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
    
    // Нормализация числительных, десятичного разделителя и знака.
    aTextString normalized_input;
    if( nlrcSuccess != ( nlrc = NumberParser::NormalizeNumerals( normalized_input, lower_input.ToConstChar(), lower_input.size() ) ) )
        return nlrc;
    
//    printf( "\nLOG:\t KeyboardNumberParser::Parse STRING NORMALIZED: \"%s\"\n", normalized_input.ToConstChar() );
    
    // Удаление пробелов между цифрами.
    aTextString input;
    if( nlrcSuccess != ( nlrc = NumberParser::DeleteSpaces( input, normalized_input.ToConstChar(), normalized_input.size() ) ) )
        return nlrc;
      
//    printf( "\nLOG:\t KeyboardNumberParser::Parse SPACES DELETED: \"%s\"\n", input.ToConstChar() );
    
    // Проверка на наличие числительных в записи числа.
    if( NumberParser::NoNumerals( input ) )
    {
//        printf( "\nLOG:\t KeyboardNumberParser::Parse  PARSE AS NUMBER\n");
        // Число записоно цифрами, считываем его стандартными средствами.
        if( !aResult.sscanf( input.ToConstChar() ) )
            return nlrcUnsuccess;
        
        return nlrcSuccess;
    }
    
//    printf( "\nLOG:\t KeyboardNumberParser::Parse  PARSE STRING\n");
    
    // Разбор записи числа.
    unsigned int pos = 0;
    if( nlrcSuccess != ( nlrc = SF( aResult, input.ToConstChar(), input.size(), pos ) ) )
        return nlrc;
    
//    printf( "\nLOG:\t KeyboardNumberParser::Parse END\n");
    
    return nlrcSuccess;    
}

