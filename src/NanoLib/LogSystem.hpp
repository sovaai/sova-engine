#ifndef __LogSystem_hpp__
#define __LogSystem_hpp__

#include <syslog.h>
#include <cstdio>
#include <cstdarg>

#include "ReturnCode.hpp"


/**
 *  Уровни логгирования.
 */
typedef enum __LogLevels
{
    LSL_NONE = 0,
    LSL_ERR = LOG_ERR,
    LSL_WARN = LOG_WARNING,
    LSL_INFO = LOG_INFO,
    LSL_DBG = LOG_DEBUG
} LogLevels;



namespace NanoLib
{
	/**
	 *  Базовый класс для логгера.
	 */
	class Logger
	{
	public:
		/**
		 *  Запись информации в лог.
		 * @param aLogLevel - уровень соответствующий записи.
		 * @param aFormat - формат записи.
		 */
		virtual void Write( LogLevels aLogLevel, const char * aFormat, va_list & aArgs ) = 0;
	};

	/**
	 *  Логгер с ипользованием syslog'а.
	 */
	class SysLogLogger : public Logger
	{
	public:
		SysLogLogger( const char * aIdentificator = nullptr, unsigned int aIdentificatorLength = 0 )
			{ Open( aIdentificator, aIdentificatorLength ); }

	public:
		/**
		 *  Открытие системного лога.
		 * @param aIdentification - идентификатор, который будет заноситься в лог вместе с каждой записью.
		 */
		void Open( const char * aIdentificator, unsigned int aIdentificatorLength = 0 );

		/** Закрытие системного лога. */
		void Close( )
			{ closelog(); }

	public:
		void SetIdentificator( const char * aIdentificator, unsigned int aIdentificatorLength = 0 );

	public:
		/**
		 *  Запись информации в лог.
		 * @param aLogLevel - уровень соответствующий записи.
		 * @param aFormat - формат записи.
		 */
		void Write( LogLevels aLogLevel, const char * aFormat, va_list & aArgs );
	};

	class LogSystem
	{
	public:
		static void SetLogger( Logger & aLogger );
		static void SetLogger( Logger & aLogger, LogLevels aLogLevel )
		{
			SetLogger( aLogger );
			SetLogLevel( aLogLevel );
		}

	public:
		static void SetLogLevel( LogLevels aLogLevel );

		static LogLevels GetLogLevel( );

	public:
		static void Write( LogLevels aLogLevel, const char *aFormat, ... );
	};
}

#define LogError( aFormat, ... )\
	do {\
		NanoLib::LogSystem::Write( LSL_ERR, aFormat, ##__VA_ARGS__ );\
		NanoLib::LogSystem::Write( LSL_DBG, "   %s", __PRETTY_FUNCTION__ );\
		NanoLib::LogSystem::Write( LSL_DBG, "     File: %s [ %d ]", __FILE__, __LINE__ );\
	} while( 0 )

#define ReturnWithError( aReturnCode, aFormat,  ... ) \
	do { \
		LogError( aFormat, ##__VA_ARGS__ ); \
		 \
		return aReturnCode; \
	} while( 0 )

#define ReturnWithErrorStrict( aReturnCode, aFormat,  ... ) \
	do { \
		if( vStrictMode ) \
		{ \
			LogError( aFormat, ##__VA_ARGS__ ); \
			\
			return aReturnCode; \
		} \
		return aReturnCode; \
	} while( 0 )


#define LogWarn( aFormat, ... )\
	do \
	{ \
		NanoLib::LogSystem::Write( LSL_WARN, aFormat, ##__VA_ARGS__ );\
		NanoLib::LogSystem::Write( LSL_DBG, "   %s", __PRETTY_FUNCTION__ );\
		NanoLib::LogSystem::Write( LSL_DBG, "     File: %s [ %d ]", __FILE__, __LINE__ );\
	} while( 0 )

#define ReturnWithWarn( aReturnCode, aFormat,  ... ) \
	do \
	{\
		LogWarn( aFormat, ##__VA_ARGS__ );\
		\
		return aReturnCode;\
	} while( 0 )

#define LogInfo( aFormat, ... )\
	do \
	{\
		NanoLib::LogSystem::Write( LSL_INFO, aFormat, ##__VA_ARGS__ );\
		NanoLib::LogSystem::Write( LSL_DBG, "   %s", __PRETTY_FUNCTION__ );\
		NanoLib::LogSystem::Write( LSL_DBG, "     File: %s [ %d ]", __FILE__, __LINE__ );\
	} while( 0 )


#define ReturnWithInfo( aReturnCode, aFormat,  ... ) \
	do \
	{\
		LogInfo( aFormat, ##__VA_ARGS__ );\
		\
		return aReturnCode;\
	} while( 0 )

#define LogTrace() \
	do \
	{\
		NanoLib::LogSystem::Write( LSL_DBG, "<= %s", __PRETTY_FUNCTION__ );\
		NanoLib::LogSystem::Write( LSL_DBG, "     File: %s [ %d ]", __FILE__, __LINE__ );\
	} while( 0 )

#define ReturnWithTrace( aReturnCode ) \
	do {\
		LogTrace();\
		\
		return aReturnCode;\
	} while( 0 )

#define ReturnWithTraceExt( aReturnCode, aExcept ) \
	do {\
		auto rc = aReturnCode; \
		if( rc != aExcept ) \
			LogTrace();\
		\
		return rc;\
	} while( 0 )

#define TryWithTrace( aReturnCode, aExcept ) \
	do {\
		auto rc = aReturnCode; \
		if( rc != aExcept ) \
			ReturnWithTrace( rc ); \
	} while( 0 )


#ifdef NO_DEBUG_LOG

#define LogDebug( aFormat, ... ) \
		do \
		{ \
		} while( 0 )

#define ReturnWithDebug( aReturnCode, aFormat,  ... ) \
		do { \
			return aReturnCode; \
		} while(0)

#else

#define LogDebug( aFormat, ... ) \
		do \
		{ \
			NanoLib::LogSystem::Write( LSL_DBG, aFormat, ##__VA_ARGS__ ); \
		} while( 0 )

#define ReturnWithDebug( aReturnCode, aFormat,  ... ) \
		do { \
			LogDebug( aFormat, ##__VA_ARGS__ ); \
			 \
			return aReturnCode; \
		} while( 0 )
#endif /** NO_DEBUG_LOG */

#endif /** __LogSystem_hpp__ */
