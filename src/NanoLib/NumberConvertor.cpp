#include "NumberConvertor.hpp"
#include "NanoLib/LongFloat.hpp"

#include <cmath>

static const double MAX_INT_TO_STR = 1E+15;

const char * RU[] = {
	"минус",				// 0
	"ноль",					// 1
	"один",					// 2
	"два",
	"три",
	"четыре",
	"пять",
	"шесть",
	"семь",
	"восемь",
	"девять",
	"десять",					// 11
	"одиннадцать",
	"двенадцать",
	"тринадцать",
	"четырнадцать",
	"пятнадцать",
	"шестнадцать",
	"семнадцать",
	"восемнадцать",
	"девятнадцать",
	"двадцать",					// 21
	"тридцать",
	"сорок",
	"пятьдесят",
	"шестьдесят",
	"семьдесят",
	"восемьдесят",
	"девяноста",
	"сто",					// 29
	"двести",
	"триста",
	"четыреста",
	"пятьсот",
	"шестьсот",
	"семьсот",
	"восемьсот",
	"девятьсот",
	"тысяча",					// 38
	"тысячи",
	"тысяч",
	"миллион",					// 41
	"миллиона",
	"миллионов",
	"миллиард",					// 44
	"миллиарда",
	"миллиардов",
	"триллион",					// 47
	"триллиона",
	"триллионов",

	"тысяч",				// 50
	"одна тысяча",
	"две тысячи",
	"три тысячи",
	"четыре тысячи",
	"пять тысяч",
	"шесть тысяч",
	"семь тысяч",
	"восемь тысяч",
	"девять тысяч",

	"миллионов",				// 60
	"один миллион",
	"два миллиона",
	"три миллиона",
	"четыре миллиона",
	"пять миллионов",
	"шесть миллионов",
	"семь миллионов",
	"восемь миллионов",
	"девять миллионов",

	"миллиардов",					// 70
	"один миллиард",
	"два миллиарда",
	"три миллиарда",
	"четыре миллиарда",
	"пять миллиардов",
	"шесть миллиардов",
	"семь миллиардов",
	"восемь миллиардов",
	"девять миллиардов",

	"триллионов",					// 80
	"один триллион",
	"два триллиона",
	"три триллиона",
	"четыре триллиона",
	"пять триллионов",
	"шесть триллионов",
	"семь триллионов",
	"восемь триллионов",
	"девять триллионов",

	"одна",					// 90
	"две",

	"и",				// 92
	"сотых",				// 93
	"десятых"					// 94
};

const char * EN[] = {
	"minus",				// 0
	"zero",					// 1
	"one",					// 2
	"two",
	"three",
	"four",
	"five",
	"six",
	"seven",
	"eight",
	"nine",
	"ten",					// 11
	"elleven",
	"twelve",
	"thirteen",
	"fourteen",
	"fifteen",
	"sixteen",
	"seventeen",
	"eighteen",
	"nineteen",
	"twenty",					// 21
	"thirty",
	"forty",
	"fifty",
	"sixty",
	"seventy",
	"eighty",
	"ninety",
	"hundred",					// 29
	"two hundred",
	"three hundred",
	"four hundred",
	"five hundred",
	"six hundred",
	"seven hundred",
	"eight hundred",
	"nine hundred",
	"thousand",					// 38
	"thousand",
	"",
	"million",					// 41
	"milloins",
	"",
	"billion",					// 44
	"billions",
	"",
	"trillion",
	"trillions",
	"",

	"thousand",					// 50
	"one thousand",
	"two thousand",
	"three thousand",
	"four thousand",
	"five thousand",
	"six thousand",
	"seven thousand",
	"eight thousand",
	"nine thousand",

	"million",					// 60
	"one million",
	"two million",
	"three million",
	"four million",
	"five million",
	"six million",
	"seven million",
	"eight million",
	"nine million",

	"billion",					// 70
	"one billion",
	"two billion",
	"three billion",
	"four billion",
	"five billion",
	"six billion",
	"seven billion",
	"eight billion",
	"nine billion",

	"trillion",					// 80
	"one trillion",
	"two trillion",
	"three trillion",
	"four trillion",
	"five trillion",
	"six trillion",
	"seven trillion",
	"eight trillion",
	"nine trillion",

	"",
	""
};

NumberConvertor::NumberConvertor() :
	vRU( RU ),
	vEN( EN )
{}

bool NumberConvertor::Compare( const char * aWord, unsigned int aInd ) const
{
	if( strcasecmp( aWord, vRU[aInd] ) == 0 )
		return true;
	else if( strcasecmp( aWord, vEN[aInd] ) == 0 )
		return true;
	else
		return false;
}

nlReturnCode NumberConvertor::SetLang( LANGUAGE aLang ) const
{
	// Установка языка.
	vLang = nullptr;
	if( aLang == lRU )
		vLang = vRU;
	else if( aLang == lEN )
		vLang = vEN;
	else
		return nlrcErrorInvArgs;

	return nlrcSuccess;
}

nlReturnCode NumberConvertor::int_i2s( long int aNumber, aTextString& aResult ) const
{
	// Проверка на ноль.
	if( aNumber == 0 )
	{
		if( aResult.append( vLang[1] ) != nlrcSuccess )
			return nlrcErrorNoFreeMemory;

		return nlrcSuccess;
	}

	// Проверка на отрицательное число.
	if( aNumber < 0 )
	{
		if( aResult.append( vLang[0] ) != nlrcSuccess )
			return nlrcErrorNoFreeMemory;

		// Добалвение идентификацию отрицательного числа и преобразование числа в положительное.
		aNumber *= -1;
	}

	// Разбираем положительное число.
	for( unsigned int power = 5; power > 0; power-- )
	{
		// Выделение старших разрядов ( миллиарды/миллионы/тысячи/числа до тысячи ).
		unsigned int res = floor( ( (double)aNumber ) / pow( 1000, power - 1 ) );

		// Проверка выделенных разрядов.
		if( res == 0 )
			continue;

		// Удаление выделенных разрядов из разбираемого числа.
		aNumber -= res * pow( 1000, power - 1 );

		// Обработка числа сотен в выделенных разрядах.
		if( res >= 100 )
		{
			unsigned int hundreds = floor( ( (double)res ) / 100 );

			if( !aResult.empty() && aResult.append( " " ) != nlrcSuccess )
				return nlrcErrorNoFreeMemory;
			if( aResult.append( vLang[28 + hundreds] ) != nlrcSuccess )
				return nlrcErrorNoFreeMemory;

			res -= 100 * hundreds;
		}

		// Обработка выделенных разрядов без учета сотен.
		if( res == 0 )
		{
			if( power > 1 &&
					( aResult.append( " " ) != nlrcSuccess ||
					aResult.append( vLang[50 + 10 * ( power - 2 )] ) != nlrcSuccess ) )
			{
				return nlrcErrorNoFreeMemory;
			}
		}
		else if( res < 10 )
		{
			if( !aResult.empty() && aResult.append( " " ) != nlrcSuccess )
				return nlrcErrorNoFreeMemory;

			if( power > 1 )
			{
				if( aResult.append( vLang[res + 50 + 10 * ( power - 2 )] ) != nlrcSuccess )
					return nlrcErrorNoFreeMemory;
			}
			else
			{
				if( aResult.append( vLang[res + 1] ) != nlrcSuccess )
					return nlrcErrorNoFreeMemory;
			}
		}
		else if( 10 <= res && res < 20 )
		{
			if( !aResult.empty() && aResult.append( " " ) != nlrcSuccess )
				return nlrcErrorNoFreeMemory;
			if( aResult.append( vLang[res + 1] ) != nlrcSuccess || aResult.append( " " ) != nlrcSuccess )
				return nlrcErrorNoFreeMemory;
			if( power > 1 && aResult.append( vLang[50 + 10 * ( power - 2 )] ) != nlrcSuccess )
				return nlrcErrorNoFreeMemory;
		}
		else
		{
			unsigned int tens = floor( ( (double)res ) / 10 );

			if( !aResult.empty() && aResult.append( " " ) != nlrcSuccess )
				return nlrcErrorNoFreeMemory;
			if( aResult.append( vLang[19 + tens] ) != nlrcSuccess )
				return nlrcErrorNoFreeMemory;

			res -= 10 * tens;

			if( res != 0 )
			{
				if( aResult.append( " " ) != nlrcSuccess )
					return nlrcErrorNoFreeMemory;

				if( power > 1 )
				{
					if( aResult.append( vLang[res + 50 + 10 * ( power - 2 )] ) != nlrcSuccess )
						return nlrcErrorNoFreeMemory;
				}
				else
				{
					if( aResult.append( vLang[res + 1] ) != nlrcSuccess )
						return nlrcErrorNoFreeMemory;
				}
			}
			else if( power > 1 )
			{
				if( aResult.append( " " ) != nlrcSuccess || aResult.append( vLang[50 + 10 * ( power - 2 )] ) != nlrcSuccess )
					return nlrcErrorNoFreeMemory;
			}
		}
	}

	return nlrcSuccess;
}

nlReturnCode NumberConvertor::int_d2s( LongFloat aNumber, aTextString& aResult ) const
{
	// Разделение числа на целую и дробную части.
	long double intg;
	long double frac = modfl( aNumber.operator double(), &intg );

	// Конвертация целой части.
	nlReturnCode nlrc = int_i2s( intg, aResult );
	if( nlrc != nlrcSuccess )
		return nlrc;

	// Конвертация дробной части.
	frac = floorl( frac * 100 );
	if( ( (long int)frac ) % 10 == 0 )
		frac /= 10;

	if( (int)frac != 0 )
	{
		if( aResult.append( " " ) != nlrcSuccess || aResult.append( vLang[92] ) != nlrcSuccess )
		{
			return nlrcErrorNoFreeMemory;
		}

		nlrc = int_i2s( (int)frac, aResult );
		if( nlrc != nlrcSuccess )
			return nlrc;

		if( frac >= 10 )
		{
			if( aResult.append( " " ) != nlrcSuccess || aResult.append( vLang[93] ) )
				return nlrcErrorNoFreeMemory;
		}
		else
		{
			if( aResult.append( " " ) != nlrcSuccess || aResult.append( vLang[94] ) )
				return nlrcErrorNoFreeMemory;
		}
	}

	return nlrcSuccess;
}

nlReturnCode NumberConvertor::i2s( long int aNumber, aTextString& aResult, LANGUAGE aLang ) const
{
	// Установка языка.
	nlReturnCode nlrc = SetLang( aLang );
	if( nlrc != nlrcSuccess )
		return nlrc;

	// Очистка контейнера для результата.
	aResult.clear();

	return int_i2s( aNumber, aResult );
}

nlReturnCode NumberConvertor::d2s( LongFloat aNumber, aTextString& aResult, LANGUAGE aLang ) const
{
	// Проверка возможности представления числа прописью.
	if( aNumber >= LongFloat( MAX_INT_TO_STR ) )
	{
		// Число слишком большое, поэтому будет представлено цифрами.
		nlReturnCode nlrc = aResult.resize( 1024 );
		if( nlrcSuccess != nlrc )
			return nlrc;

		if( !aNumber.snprintf( aResult.ToChar(), 1023, 10 ) )
			return nlrcErrorBufferTooSmall;

		return aResult.resize( strlen( aResult.ToConstChar() ) );
	}

	// Установка языка.
	nlReturnCode nlrc = SetLang( aLang );
	if( nlrc != nlrcSuccess )
		return nlrc;

	// Очистка контейнера для результата.
	aResult.clear();

	return int_d2s( aNumber, aResult );
}

nlReturnCode NumberConvertor::s2i( const char * aNumber, long int& aResult ) const
{
	// Проверка аргументов.
	if( !aNumber )
		return nlrcErrorInvArgs;

	// Сброс результата.
	aResult = 0;

	// Очистка вспомогательного буффера.
	bzero( vTmpBuffer, 15 );

	// Разбиение на слова и конвертация.
	const char * p = nullptr;
	int negative = 0;

	aTextString test;
	nlReturnCode nlrc = test.assign( aNumber );
	if( nlrcSuccess != nlrc )
		return nlrc;

	for( p = strtok( test.ToChar(), " " ); p; p = strtok( nullptr, " " ) )
	{
		int res = 0;

		// Сравнение со знаком минус.
		if( Compare( p, 0 ) )
		{
			if( negative != 0 )
			{
				return nlrcUnsuccess;
			}

			negative = -1;
			continue;
		}
		else if( negative == 0 )
			negative = 1;

		// Сравнение с нулем.
		if( Compare( p, 1 ) )
		{
			if( aResult || negative < 0 )
			{
				return nlrcUnsuccess;
			}
		}

		// Сравнение с числами от 1 до 19.
		unsigned int i = 0;
		for( i = 2; i < 21; i++ )
		{
			if( Compare( p, i ) )
			{
				if( i < 11 )
				{
					if( vTmpBuffer[0] )
					{
						return nlrcUnsuccess;
					}
					else
						vTmpBuffer[0] = i - 1;
				}
				else
				{
					if( vTmpBuffer[0] || vTmpBuffer[1] )
					{
						return nlrcUnsuccess;
					}
					else
					{
						vTmpBuffer[1] = 1;
						vTmpBuffer[0] = ( i - 1 ) % 10;
					}
				}
				break;
			}
		}
		if( i != 21 )
			continue;

		if( Compare( p, 90 ) )
		{
			if( vTmpBuffer[0] )
			{
				return nlrcUnsuccess;
			}
			else
				vTmpBuffer[0] = 1;
			continue;
		}

		if( Compare( p, 91 ) )
		{
			if( vTmpBuffer[0] )
			{
				return nlrcUnsuccess;
			}
			else
				vTmpBuffer[0] = 2;
			continue;
		}

		// Сравнение с десятками.
		for( i = 21; i < 29; i++ )
		{
			if( Compare( p, i ) )
			{
				if( vTmpBuffer[1] )
				{
					return nlrcUnsuccess;
				}
				else
					vTmpBuffer[1] = i - 19;
				break;
			}
		}
		if( i != 29 )
			continue;

		// Сравнение с сотнями.
		for( i = 29; i < 38; i++ )
		{
			if( Compare( p, i ) )
			{
				if( vTmpBuffer[2] )
				{
					return nlrcUnsuccess;
				}
				else
					vTmpBuffer[2] = i - 28;
				break;
			}
		}
		if( i != 38 )
			continue;

		// Сравнение с тысячами.
		for( i = 38; i < 41; i++ )
		{
			if( Compare( p, i ) )
			{
				if( vTmpBuffer[3] || vTmpBuffer[4] || vTmpBuffer[5] )
				{
					return nlrcUnsuccess;
				}
				if( vTmpBuffer[0] == 0 && vTmpBuffer[1] == 0 && vTmpBuffer[2] == 0 )
					vTmpBuffer[0] = 1;
				memcpy( vTmpBuffer + 3, vTmpBuffer, 3 );
				bzero( vTmpBuffer, 3 );
				break;
			}
		}
		if( i != 41 )
			continue;
		//        for( i = 51; i < 60; i++ )
		//        {
		//            printf( "%s - %s\n", p, vRU[i] );
		//            if( Compare( p, i ) )
		//            {
		//                if( vTmpBuffer[3] || vTmpBuffer[4] || vTmpBuffer[5] )
		//                {
		//                    printf( "8: %s\n", p );
		//                    return nlrcUnsuccess;
		//                }
		//                vTmpBuffer[0] = i - 50;
		//                memcpy( vTmpBuffer + 3, vTmpBuffer, 3 );
		//                bzero( vTmpBuffer, 3 );
		//                break;
		//            }
		//        }
		//        if( i != 60 )
		//            continue;

		// Сравнение с миллионом.
		for( i = 41; i < 44; i++ )
		{
			if( Compare( p, i ) )
			{
				if( vTmpBuffer[6] || vTmpBuffer[7] || vTmpBuffer[8] )
					return nlrcUnsuccess;
				if( vTmpBuffer[0] == 0 && vTmpBuffer[1] == 0 && vTmpBuffer[2] == 0 )
					vTmpBuffer[0] = 1;
				memcpy( vTmpBuffer + 6, vTmpBuffer, 3 );
				bzero( vTmpBuffer, 3 );
				break;
			}
		}
		if( i != 44 )
			continue;

		// Сравление с миллиардом.
		for( i = 44; i < 47; i++ )
		{
			if( Compare( p, i ) )
			{
				if( vTmpBuffer[9] || vTmpBuffer[10] || vTmpBuffer[11] )
					return nlrcUnsuccess;
				if( vTmpBuffer[0] == 0 && vTmpBuffer[1] == 0 && vTmpBuffer[2] == 0 )
					vTmpBuffer[0] = 1;
				memcpy( vTmpBuffer + 9, vTmpBuffer, 3 );
				bzero( vTmpBuffer, 3 );
				break;
			}
		}
		if( i != 47 )
			continue;

		// Сравление с триллионом.
		for( i = 47; i < 50; i++ )
		{
			if( Compare( p, i ) )
			{
				if( vTmpBuffer[12] || vTmpBuffer[13] || vTmpBuffer[14] )
					return nlrcUnsuccess;
				if( vTmpBuffer[0] == 0 && vTmpBuffer[1] == 0 && vTmpBuffer[2] == 0 )
					vTmpBuffer[0] = 1;
				memcpy( vTmpBuffer + 12, vTmpBuffer, 3 );
				bzero( vTmpBuffer, 3 );
				break;
			}
		}
		if( i != 50 )
			continue;

		return nlrcUnsuccess;
	}

	for( unsigned int i = 0; i < 15; i++ )
	{
		aResult *= 10;
		aResult += vTmpBuffer[14 - i];
	}
	aResult *= negative;

	return nlrcSuccess;
}

nlReturnCode NumberConvertor::s2lf( const char * aNumber, LongFloat& aResult, bool* aStringRep ) const
{
	// Проверка аргументов.
	if( !aNumber )
		return nlrcErrorInvArgs;
	
	if( !aResult.sscanf( aNumber ) )
	{
		if( aStringRep )
			*aStringRep = true;
		long int i;
		nlReturnCode nlrc = s2i( aNumber, i );
		if( nlrc != nlrcSuccess )
			return nlrc;
		aResult.Set( LongFloat( i ) );
	}
	else if( aStringRep )
		*aStringRep = false;
	return nlrcSuccess;
}
