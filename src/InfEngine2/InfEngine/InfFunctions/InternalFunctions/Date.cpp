#include "InternalFunction.hpp"
#include "InfEngine2/_Include/Errors.h"
#include "Date.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo Date::ARGS[] = {
	{ DLFAT_EXTENDED, "%d.%m.%Y" }
};

const DLFunctionInfo Date::DLF_INFO = {
	"Date",
	"dt",
	"Returns current date and time according to given parameter.",
	DLFRT_TEXT,
	1,
	ARGS,
	{ 3, 1, 0 },
	{ 3, 1, 0 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	nullptr
};

ExtICoreRO * Date::vBase = nullptr;

InfEngineErrors Date::Apply( const char ** aArgs, const char *& aResult, unsigned int & aLength,
		nMemoryAllocator * aMemoryAllocator, FunctionContext * aFunctionContext )
{
	// Проверка аргументов.
	const char * DateFormat = nullptr;
	if( !aArgs[0] || aArgs[0][0] == '\0' )
	{
		// Аргумент по умолчанию.
		DateFormat = ARGS[0].default_value;
	}
	else if( aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;
	else
		DateFormat = aArgs[0];

	// Получение текущего времени.
	time_t Timer;
	time( &Timer );

	// Изменение времени с учётом текущего часового пояса.
	if( aFunctionContext->vSession )
	{
		// Получаем часовой пояс из переменной в текущей сессии.
		const Vars::Vector::VarValue var_val = aFunctionContext->vSession->GetValue( InfEngineVarUserTimeZone );
		if( var_val )
		{
			const Vars::Vector::VarText var_value = var_val.AsText();
			if( var_value )
			{
				unsigned int var_len = 0;
				const char * value = var_value.GetValue( var_len );

				// Разбираем значение часового пояса, записанное в переменной user_timezone.
				if( value )
				{
					unsigned int pos = 0;
					int time_zone_in_seconds = TextFuncs::GetTimeZone( value, var_len, pos );
					if( pos )
						Timer += time_zone_in_seconds;
				}
			}
		}
	}

	// Преобразование времени из формата в виде секунд с 1970 года в формат в виде даты.
	struct tm * TimeStruct = localtime( &Timer );
	if( !TimeStruct )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Если локальное время сервера не UTC+3:00 (Москва), то делаем поправку на это.

	if( strcmp( TimeStruct->tm_zone, "MSK" ) )
	{
		Timer += 10800 - TimeStruct->tm_gmtoff;
		// Преобразование времени из формата в виде секунд с 1970 года в формат в виде даты.
		TimeStruct = localtime( &Timer );
		if( !TimeStruct )
			return INF_ENGINE_WARN_UNSUCCESS;
	}

	// Создание требуемого формата для даты.
	char DateBuffer[1024];
	unsigned int DateLength = strftime( DateBuffer, 1024, DateFormat, TimeStruct );

	// Выделяем память под результат.
	char * Buffer = static_cast<char*>( aMemoryAllocator->Allocate( DateLength + 1 ) );
	if( !Buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Копируем результат.
	memcpy( Buffer, DateBuffer, DateLength );
	Buffer[DateLength] = '\0';
	aResult = Buffer;
	aLength = DateLength;

	return INF_ENGINE_SUCCESS;
}
