#include <ctime>

#include "../FunctionLib.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

static const DLFucntionArgInfo ARGS[] = {
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
	{ DLFAT_EXTENDED, "ru", DLFAA_NONE }
};

static const DLFunctionInfo DLF_INFO = {
	"Extract",
	"dext",
	"Return given date in specified format using specified language.",
	DLFRT_TEXT,
	3,
	ARGS,
	{ 3, 0, 1 },
	{ 3, 0, 1 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

enum class Lang {
	Russian,
	English,
	Ukrainian
};

static const char * DAY_OF_WEEK_ENG[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
static const char * DAY_OF_WEEK_RUS[7] = { "понедельник", "вторник", "среда", "четверг", "пятница", "суббота", "воскресенье" };
static const char * DAY_OF_WEEK_UKR[7] = { "понедiлок", "вiвторок", "середа", "четвер", "п'ятниця", "субота", "недiля" };

static const char * MONTH_ENG[12] =
{ "January", "February ", "March", "April ", "May ", "June", "July ", "August", "September", "October ", "November", "December" };
static const char * MONTH_RUS[12] =
{ "январь", "февраль", "март", "апрель", "май", "июнь", "июль", "август", "сентябрь", "октябрь", "ноябрь", "декабрь" };
static const char * MONTH_UKR[12] =
{ "сiчень", "лютий", "березень", "квiтень", "травень", "червень", "липень", "серпень", "вересень", "жовтень", "листопад", "грудень" };

static const char * MONTH_RUS_GENITIVE[12] =
{ "января", "февраля", "марта", "апреля", "мая", "июня", "июля", "августа", "сентября", "октября", "ноября", "декабря" };
static const char * MONTH_UKR_GENITIVE[12] =
{ "сiчня", "лютого", "березня", "квiтня", "травня", "червня", "липня", "серпня", "вересня", "жовтня", "листопада", "грудня" };

const DLFunctionInfo * GetDLFucntionInfo() { return &DLF_INFO; }

static void AddDayOfMonth( aTextString & str, struct tm * t ) {
	char buf[3];
	sprintf( buf, "%02i", t->tm_mday );
	str.append( buf );
}

static void AddYear2Digits( aTextString & str, struct tm * t ) {
	char buf[3];
	sprintf( buf, "%02i", ( t->tm_year + 1900 ) % 100 );
	str.append( buf );
}

static void AddMonthNum( aTextString & str, struct tm * t ) {
	char buf[3];
	sprintf( buf, "%02i", t->tm_mon + 1 );
	str.append( buf );
}

static void AddMonthName( aTextString & str, Lang lang, struct tm * t ) {
	switch( lang ){
		case Lang::Russian:
			str.append( MONTH_RUS[t->tm_mon] );
			break;

		case Lang::English:
			str.append( MONTH_ENG[t->tm_mon] );
			break;

		case Lang::Ukrainian:
			str.append( MONTH_UKR[t->tm_mon] );
			break;

		default:
			return;
	}
}

void AddMonthNameInGenitive( aTextString & str, Lang lang, struct tm * t ) {
	switch( lang ) {
		case Lang::Russian:
			str.append( MONTH_RUS_GENITIVE[t->tm_mon] );
			break;

		case Lang::English:
			str.append( MONTH_ENG[t->tm_mon] );
			break;

		case Lang::Ukrainian:
			str.append( MONTH_UKR_GENITIVE[t->tm_mon] );
			break;

		default:
			return;
	}
}

static void AddDayOfWeekName( aTextString & str, Lang lang, struct tm * t ) {
	switch( lang ) {
		case Lang::Russian:
			str.append( DAY_OF_WEEK_RUS[( t->tm_wday ? t->tm_wday : 7 ) - 1] );
			break;

		case Lang::English:
			str.append( DAY_OF_WEEK_ENG[( t->tm_wday ? t->tm_wday : 7 ) - 1] );
			break;

		case Lang::Ukrainian:
			str.append( DAY_OF_WEEK_UKR[( t->tm_wday ? t->tm_wday : 7 ) - 1] );
			break;

		default:
			return;
	}
}

static void AddYear4Digits( aTextString & str, struct tm * t ) {
	char buf[5];
	sprintf( buf, "%04i", ( t->tm_year + 1900 ) );
	str.append( buf );
}

static void AddDayOfWeekNum( aTextString & str, struct tm * t ) {
	char buf[2];
	sprintf( buf, "%i", t->tm_wday ? t->tm_wday : 7 );
	str.append( buf );
}

InfEngineErrors Extract( const char ** aArgs, const char *& aResult, unsigned int & aLength,
						 nMemoryAllocator * aMemoryAllocator, FunctionContext * /*aSession*/ ) {

	if( !( aArgs[0] && aArgs[1] && !aArgs[2] ) && !( aArgs[0] && aArgs[1] && aArgs[2] && !aArgs[3] ) )
		return INF_ENGINE_ERROR_ARGC;

	const char * lang_str{ nullptr };
	Lang lang;

	lang_str = aArgs[2] ? aArgs[2] : ARGS[2].default_value;

	if( !strcmp( lang_str, "ru" ) )
		lang = Lang::Russian;
	else if( !strcmp( lang_str, "en" ) )
		lang = Lang::English;
	else if( !strcmp( lang_str, "uk" ) )
		lang = Lang::Ukrainian;
	else
		return INF_ENGINE_WARN_UNSUCCESS;

	unsigned int day;
	unsigned int month;
	unsigned int year;
	bool is_short;

	if( !TextFuncs::ParseDate( aArgs[0], day, month, year, is_short ) || is_short )
		return INF_ENGINE_WARN_UNSUCCESS;

	if( year < 100 )
		year += 2000;

	struct tm t;
	t.tm_sec = 0;
	t.tm_min = 0;
	t.tm_hour = 0;
	t.tm_mday = day;
	t.tm_mon = month - 1;
	t.tm_year = year - 1900;
	t.tm_isdst = 0;

	time_t sec = mktime( &t );
	if( sec == -1 )
		return INF_ENGINE_WARN_UNSUCCESS;

	struct tm * full_t = localtime( &sec );
	if( !full_t )
		return INF_ENGINE_WARN_UNSUCCESS;

	aTextString result;
	const char * begin = aArgs[1];
	const char * end = nullptr;
	while( ( end = strstr( begin, "%" ) ) ) {
		result.append( begin, end - begin );
		begin = end + 1;

		if( *begin ) {
			switch( *begin ) {
				// Символ %.
				case '%':
					result.append( "%" );
					break;

					// День месяца, 01 .. 31.
				case 'd':
					AddDayOfMonth( result, full_t );
					break;

					// ГГ - последние две цифры года.
				case 'g':
					AddYear2Digits( result, full_t );
					break;

					// Номер месяц, 01 .. 12.
				case 'm':
					AddMonthNum( result, full_t );
					break;

					// Название месяца, январь .. декабрь.
				case 'M':
					AddMonthName( result, lang, full_t );
					break;

					// Название месяца в родительном падеже, января .. декабря.
				case 'S':
					AddMonthNameInGenitive( result, lang, full_t );
					break;

					// Название дня недели, понедельник .. воскресенье.
				case 'D':
					AddDayOfWeekName( result, lang, full_t );
					break;

					// ГГГГ - год, четрые цифры.
				case 'y':
					AddYear4Digits( result, full_t );
					break;

					// Номер дня недели, 1 .. 7.
				case 'u':
					AddDayOfWeekNum( result, full_t );
					break;

				default:
					result.append( begin - 1, 2 );
			}
		}
		++begin;
	}
	result.append( begin );

	aLength = result.size();
	char * buffer = static_cast<char *>( aMemoryAllocator->Allocate( aLength + 1 ) );
	if( !buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;
	strcpy( buffer, result.ToConstChar() );

	aResult = buffer;

	return INF_ENGINE_SUCCESS;
}

/*******************************************************************************************************************
 *
 *  Автотесты.
 *
 ******************************************************************************************************************/

#ifdef COMPILE_WITH_AUTOTEST_MODE


CREATE_TEST2( PERCENT_00, "%", 3, "03.05.2010", "%%", "ru" )
CREATE_TEST2( PERCENT_01, "%%%", 3, "03.05.2010", "%%%%%%", "en" )
CREATE_TEST2( PERCENT_02, "% %.%", 4, "03.05.2010", "%% %%.%%" )
CREATE_TEST2( PERCENT_03, "%%%", 3, "03.05.2010", "%%%%%%", "uk" )

CREATE_TEST2( DAY_00, "day = 03 !!!", 3, "03.05.10", "day = %d !!!", "ru" )
CREATE_TEST2( DAY_01, "%03%03%03", 3, "03.05.2010", "%%%d%%%d%%%d", "en" )
CREATE_TEST2( DAY_02, " 31 ", 2, "31.12.10", " %d " )

CREATE_TEST2( DAY_OF_WEEK_00, " 2 ", 3, "03.05.11", " %u ", "ru" )
CREATE_TEST2( DAY_OF_WEEK_01, "5", 3, "06.05.2011", "%u", "en" )
CREATE_TEST2( DAY_OF_WEEK_02, "%7%", 2, "08.05.11", "%%%u%%" )
CREATE_TEST2( DAY_OF_WEEK_03, "%1%", 2, "02.05.2011", "%%%u%%" )

CREATE_TEST2( DAY_NAME_00, "вторник", 3, "03.05.2011", "%D", "ru" )
CREATE_TEST2( DAY_NAME_01, "пятница", 3, "06.05.11", "%D", "ru" )
CREATE_TEST2( DAY_NAME_02, "Saturday", 3, "31.12.2011", "%D", "en" )
CREATE_TEST2( DAY_NAME_03, "Tuesday", 3, "03.05.11", "%D", "en" )
CREATE_TEST2( DAY_NAME_04, "Sunday", 3, "08.05.2011", "%D", "en" )
CREATE_TEST2( DAY_NAME_05, "суббота", 2, "31.12.11", "%D" )

CREATE_TEST2( MONTH_00, "01", 3, "5.1.2011", "%m", "ru" )
CREATE_TEST2( MONTH_01, "07", 3, "05.07.11", "%m", "ru" )
CREATE_TEST2( MONTH_02, "12", 3, "5.12.2011", "%m", "en" )
CREATE_TEST2( MONTH_03, " 03 ", 3, "3.03.11", " %m ", "en" )
CREATE_TEST2( MONTH_04, " 11 ", 4, "1.11.11", " %m " )

CREATE_TEST( MONTH_NAME_00, 12, "январь", 3, "5.1.2011", "%M", "ru" )
CREATE_TEST2( MONTH_NAME_01, "июль", 3, "05.07.11", "%M", "ru" )
CREATE_TEST2( MONTH_NAME_02, "December", 3, "5.12.2011", "%M", "en" )
CREATE_TEST2( MONTH_NAME_03, " March ", 3, "3.03.11", " %M ", "en" )
CREATE_TEST( MONTH_NAME_04, 10, " November ", 3, "1.11.11", " %M ", "en" )
CREATE_TEST2( MONTH_NAME_05, " ноябрь ", 2, "1.11.11", " %M " )

CREATE_TEST2( YEAR_00, "2011", 3, "5.1.2011", "%y", "ru" )
CREATE_TEST2( YEAR_01, "2011", 3, "05.07.11", "%y", "ru" )
CREATE_TEST2( YEAR_02, "2001", 3, "5.12.1", "%y", "en" )
CREATE_TEST2( YEAR_03, " 1998 ", 3, "3.03.1998", " %y ", "en" )
CREATE_TEST2( YEAR_04, " 2121 ", 2, "1.11.2121", " %y " )

CREATE_TEST2( SHORT_YEAR_00, "11", 3, "5.1.2011", "%g", "ru" )
CREATE_TEST2( SHORT_YEAR_01, "11", 3, "05.07.11", "%g", "ru" )
CREATE_TEST2( SHORT_YEAR_02, "01", 3, "5.12.1", "%g", "en" )
CREATE_TEST2( SHORT_YEAR_03, " 98 ", 3, "3.03.1998", " %g ", "en" )
CREATE_TEST2( SHORT_YEAR_04, " 21 ", 2, "1.11.2321", " %g " )

CREATE_TEST2( MONTH_NAME_IN_GENITIVE_00, "января", 3, "5.1.2011", "%S", "ru" )
CREATE_TEST2( MONTH_NAME_IN_GENITIVE_01, "июля", 3, "05.07.11", "%S", "ru" )
CREATE_TEST2( MONTH_NAME_IN_GENITIVE_02, "December", 3, "5.12.2011", "%S", "en" )
CREATE_TEST2( MONTH_NAME_IN_GENITIVE_03, " March ", 3, "3.03.11", " %S ", "en" )
CREATE_TEST2( MONTH_NAME_IN_GENITIVE_04, " November ", 3, "1.11.11", " %S ", "en" )
CREATE_TEST2( MONTH_NAME_IN_GENITIVE_05, " ноября ", 2, "1.11.11", " %S " )

REGISTER_TESTS(
		TEST( PERCENT_00 ),
		TEST( PERCENT_01 ),
		TEST( PERCENT_02 ),
		TEST( DAY_00 ),
		TEST( DAY_01 ),
		TEST( DAY_02 ),
		TEST( DAY_OF_WEEK_00 ),
		TEST( DAY_OF_WEEK_01 ),
		TEST( DAY_OF_WEEK_02 ),
		TEST( DAY_OF_WEEK_03 ),
		TEST( DAY_NAME_00 ),
		TEST( DAY_NAME_01 ),
		TEST( DAY_NAME_02 ),
		TEST( DAY_NAME_03 ),
		TEST( DAY_NAME_04 ),
		TEST( DAY_NAME_05 ),
		TEST( MONTH_00 ),
		TEST( MONTH_01 ),
		TEST( MONTH_02 ),
		TEST( MONTH_03 ),
		TEST( MONTH_04 ),
		TEST( MONTH_NAME_00 ),
		TEST( MONTH_NAME_01 ),
		TEST( MONTH_NAME_02 ),
		TEST( MONTH_NAME_03 ),
		TEST( MONTH_NAME_04 ),
		TEST( MONTH_NAME_05 ),
		TEST( YEAR_00 ),
		TEST( YEAR_01 ),
		TEST( YEAR_02 ),
		TEST( YEAR_03 ),
		TEST( YEAR_04 ),
		TEST( SHORT_YEAR_00 ),
		TEST( SHORT_YEAR_01 ),
		TEST( SHORT_YEAR_02 ),
		TEST( SHORT_YEAR_03 ),
		TEST( SHORT_YEAR_04 ),
		TEST( MONTH_NAME_IN_GENITIVE_00 ),
		TEST( MONTH_NAME_IN_GENITIVE_01 ),
		TEST( MONTH_NAME_IN_GENITIVE_02 ),
		TEST( MONTH_NAME_IN_GENITIVE_03 ),
		TEST( MONTH_NAME_IN_GENITIVE_04 ),
		TEST( MONTH_NAME_IN_GENITIVE_05 ))

DEFAULT_CHECK_DL_FUNCTION( Extract )

#endif	/** COMPILE_WITH_AUTOTEST_MODE */

#ifdef __cplusplus
}
#endif
