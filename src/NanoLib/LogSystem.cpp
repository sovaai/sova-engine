#include "LogSystem.hpp"

#include <string>
#include <cstring>

using namespace NanoLib;

static LogLevels gLogLevel =  LSL_NONE;
static Logger * gLogger = nullptr;

/**
 *  Расшифровка типа записи в текстовом виде.
 */
inline const char * LSLRecordType( int aType )
{
	switch( aType )
	{
		case LSL_ERR:
			return "ERROR";
		case LSL_WARN:
			return "WARN";
		case LSL_INFO:
			return "INFO";
		case LSL_DBG:
			return "DEBUG";
		default:
			return "MSG";
	}
}

void SysLogLogger::Open( const char * aIdentificator, unsigned int aIdentificatorLength )
{
	SetIdentificator( aIdentificator, aIdentificatorLength );

	// Закрытие предыдущего лога, если нужно.
	Close();

	// Открытие лога.
	openlog( aIdentificator, LOG_PID, LOG_LOCAL3 );
}

void SysLogLogger::SetIdentificator( const char * aIdentificator, unsigned int aIdentificatorLength )
{
	// Проверка аргументов.
	if( !aIdentificator )
	{
		aIdentificator = "NanoLib Logger";
		aIdentificatorLength = 0;
	}

	if( !aIdentificatorLength )
		aIdentificatorLength = strlen( aIdentificator );
}

void SysLogLogger::Write( LogLevels aLogLevel, const char * aFormat, va_list & aArgs )
{
	static std::string tmpstring;

	try
	{
		tmpstring = LSLRecordType( aLogLevel );
		tmpstring += ": ";
		tmpstring += aFormat;
		tmpstring += "\n";
	}
	catch( ... )
	{
		return;
	}

	vsyslog( aLogLevel, tmpstring.c_str(), aArgs );
}


void LogSystem::SetLogger( Logger & aLogger )
{
	gLogger = &aLogger;
}

void LogSystem::SetLogLevel( LogLevels aLogLevel )
{
	gLogLevel = aLogLevel;
}

LogLevels LogSystem::GetLogLevel( )
{
	return gLogLevel;
}


void LogSystem::Write( LogLevels aLogLevel, const char *aFormat, ... )
{
#ifdef NO_DEBUG_LOG
	if( aLogLevel = LSL_DBG )
		return nlrcSuccess;
#endif // NO_DEBUG_LOG

	// Проверка уровня логгирования.
	if( !gLogger || (int)aLogLevel > (int)gLogLevel )
		return;

	va_list ap;
	va_start( ap, aFormat );
		gLogger->Write( aLogLevel, aFormat, ap );
	va_end( ap );
}
