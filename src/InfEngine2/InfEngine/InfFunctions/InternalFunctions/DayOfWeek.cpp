#include "DayOfWeek.hpp"

#include <ctime>

#include <NanoLib/aTextString.hpp>

#include "InternalFunction.hpp"
#include "InfEngine2/_Include/Errors.h"

using namespace InternalFunctions;

const DLFucntionArgInfo DayOfWeek::ARGS[] = { { DLFAT_EXTENDED, nullptr, DLFAA_NONE } };

const DLFunctionInfo DayOfWeek::DLF_INFO = {
	"DayOfWeek",
	"dow",
	"Returns current day of week in russian or english according to given parameter. It also can return current day of week as integer.",
	DLFRT_TEXT,
	1,
	ARGS,
	{ 3, 0, 1 },
	{ 3, 0, 1 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5, InfDataProtocolVersion,
	nullptr
};

ExtICoreRO * DayOfWeek::vBase{ nullptr };

InfEngineErrors DayOfWeek::Apply(const char ** aArgs, const char *& aResult, unsigned int & aLength, nMemoryAllocator * aMemoryAllocator,
								 FunctionContext * aFunctionContext ) {

	// Проверяем количество аргументов.
	if( !aArgs[0] || aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;

	// Приводим аргумент к нижнему регистру.
	aTextString tmp;
	if( tmp.assign( aArgs[0] ) != nlrcSuccess )
		return INF_ENGINE_ERROR_INV_ARGS;
	TextFuncs::ToLower( tmp.ToChar( ), tmp.size( ) );

	// Получаем текущее время.
	time_t rawtime;
	time( &rawtime );

	// Изменение времени с учётом текущего часового пояса.
	if( aFunctionContext->vSession ) {
		// Получаем часовой пояс из переменной в текущей сессии.
		auto var_val = aFunctionContext->vSession->GetValue( InfEngineVarUserTimeZone );
		if( var_val ) {
			auto var_value = var_val.AsText();
			if( var_value ) {
				unsigned int var_len{ 0 };
				const char * value = var_value.GetValue( var_len );

				// Разбираем значение часового пояса, записанное в переменной user_timezone.
				if( value ) {
					unsigned int pos{ 0 };
					int time_zone_in_seconds = TextFuncs::GetTimeZone( value, var_len, pos );
					if( pos )
						rawtime += time_zone_in_seconds;
				}
			}
		}
	}

	// Преобразование времени из формата в виде секунд с 1970 года в формат в виде даты.
	struct tm * t = localtime( &rawtime );
	if( !t )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Если локальное время сервера не UTC+4:00 (Москва), то делаем поправку на это.
	if( t->tm_gmtoff != 14400 ) {
		rawtime += 14400 - t->tm_gmtoff;
		// Преобразование времени из формата в виде секунд с 1970 года в формат в виде даты.
		t = localtime( &rawtime );
		if( !t )
			return INF_ENGINE_WARN_UNSUCCESS;
	}

	const char * day_str{ nullptr };
	unsigned int day_len{ 0 };

	// Выбираем язык.
	if( !strcmp( tmp.ToConstChar( ), "ru" ) ) {
		// Русский.
		static const char * RUS[7] = { "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье" };

		day_str = RUS[( t->tm_wday ? t->tm_wday : 7 ) - 1];
		day_len = std::strlen( day_str );
	}
	else if( !strcmp( tmp.ToConstChar( ), "en" ) ) {
		// Английский.
		static const char * ENG[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

		day_str = ENG[( t->tm_wday ? t->tm_wday : 7 ) - 1];
		day_len = std::strlen( day_str );
	}
	else if( !strcmp( tmp.ToConstChar(), "uk" ) ) {
		// Украинский.
		static const char * UKR[7] = { "Понедiлок", "Вiвторок", "Середа", "Четвер", "П'ятниця", "Субота", "Недiля" };

		day_str = UKR[( t->tm_wday ? t->tm_wday : 7 ) - 1];
		day_len = std::strlen( day_str );
	}
	else if( !strcmp( tmp.ToConstChar( ), "n" ) ){
		// Номер.
		char * buffer = static_cast<char *>( aMemoryAllocator->Allocate( 2 ) );
		if( !buffer )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;

		buffer[0] = '0' + ( t->tm_wday ? t->tm_wday : 7 );
		buffer[1] = '\0';

		day_str = buffer;
		day_len = 1;
	}
	else
		return INF_ENGINE_WARN_UNSUCCESS;

	// Записываем результат.
	aResult = day_str;
	aLength = day_len;

	return INF_ENGINE_SUCCESS;
}
