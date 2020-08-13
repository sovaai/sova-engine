#ifndef __InfEngineServer_hpp__
#define __InfEngineServer_hpp__

#include <NanoLib/CacheMemcached.hpp>

#include <InfEngine2/InfEngine/SearchEngine.hpp>
#include <InfEngine2/InfEngine/InfProfile.hpp>
#include <ClientLib/CSData.hpp>

// Битовые маски, используемые в запросах к серверу и его ответах.
#define INF_MASK_INF      0x01
#define INF_MASK_SESSION  0x02

/** Тип указателя на функцию раскрытия эллипсиса. */
typedef InfEngineErrors ( * EllipsisDiscloseFunction )( const char * aUserPreRequest, unsigned int aUserPreRequestLength,
														const char * aInfPreResponse, unsigned int aInfPreResponseLength,
														const char * aUserRequest, unsigned int aUserRequestLength,
														char* & aResult, unsigned int & aResultLength,
														char* & aEllipsisId, unsigned int & aEllipsisIdLength,
														char* & aEllipsisPartialIDs, unsigned int & aEllipsisPartialIDsLength );

/**
 *  Выделение памяти под объект csdata_data_t с aAttrNum аттрибутами. Выделение
 * производится через заданный аллокатор aMemoryAllocator.
 */
csdata_data_t * csDataAllocate( unsigned int aAttrNum, nMemoryAllocator & aMemoryAllocator );


/**
 *  Класс, реализующий всю серверную функциональность бэкэнда.
 */
class InfEngineServer : protected ExtISearchEngine
{
public:
	/**
	 *  Конструктор.
	 * @param aDictsSwapFilePath - путь к файлу для временного хранения словарей во время компиляции
	 * @param aMemoryLimit - максимальня память, которую можно выделять для размещения компилируемых шаблонов.
	 */
	InfEngineServer( const char * aDictsSwapFilePath, unsigned int aMemoryLimit );

	~InfEngineServer()
	{
		if( vUserBaseBuffer )
			free( vUserBaseBuffer );
	}

public:
	/**
	 *  Добавление сервера для кэширования.
	 * @param aHost - хост сервера memcached.
	 * @param aPort - порт сервера memcached.
	 * @todo Нужна проверка сервера.
	 */
	InfEngineErrors AddCacheServer( const char * aHost, unsigned int aPort );

	/**
	 *  Установка максимального времени жизни записей в кэше.
	 * @param aTTL - время жизни записи в кэше ( в секундах ).
	 */
	void SetCacheTTL( time_t aTTL )
	{ vCache.SetTTL( aTTL ); }


public:
	/** Тип алгоритма раскрытия эллипсиса. */
	enum class EllipsisMode
	{
		Off,    /** Раскрытие эллипсиса выключено. */
		Force,  /** Раскрытие эллипсиса производится в приоритетном режиме. */
		Best    /** Раскрытие эллипсиса производится в конкурентном режиме. */
	};

	/** Значение по умолчанию для настройки раскрытия эллипсиса конкретного инфа. */
	enum class EllipsisDefault
	{
		None,   /** Индивидуальная настройка системы раскрытия эллипсиса для инфов отключена. */
		On,     /** По умолчанию эллипсис раскрывается. */
		Off     /** По умолчанию эллипсис не раскрывается. */
	};

	/**
	 *  Включение системы раскрытия эллипсиса.
	 * @param aEllipsisDiscloseFunction - функция раскрытия эллипсиса.
	 * @param aOnDemandMode - флаг, говорящий использовать ли раскрытие эллипсиса только для выделенных инфов.
	 * @param aConcurentMode - флаг, говорящий, что нужно проводить поиск и по запросу пользователя и по раскрытому
	 *                        эллипсису и выбирать лучшй, а не безоговорочно использовать поиск по эллипсису.
	 */
	InfEngineErrors SetUpEllipsisEngine( EllipsisDiscloseFunction aEllipsisDiscloseFunction, EllipsisMode aEllipsisMode = EllipsisMode::Off, EllipsisDefault aEllipsisDefault = EllipsisDefault::None );


public:
	/**
	 *  Открытие основной базы и основания для кастомизирующей баз.
	 * @param aBasePath - путь к основной базе.
	 * @param aFunctionsRootDir - корневой каталог для функций.
	 * @param aFunctionsConfigPath - путь к конфигурационному файлу функций.
	 * @param aAliasesConfigPath - путь к конфигурационному файлу алиасов.
	 */
	InfEngineErrors Open( const char * aBasePath,
						  const char * aFunctionsRootDir, const char * aFunctionsConfigPath, const char * aAliasesConfigPath );

	/** Закрытие основной и кастомизирующей баз. */
	void Close();


public:
	/**
	 *  Удаление сессии из кэша.
	 * @param aSessionId - идентификатор сессии.
	 */
	InfEngineErrors CmdPurgeSession( unsigned int aSessionId );

	/**
	 *  Добавление сессии в кэш.
	 * @param aSessionId - идентификатор сессии.
	 * @param aInfId - идентификатор инфа.
	 * @param aSession - данные сессии.
	 */
	InfEngineErrors CmdSetSession( unsigned int aSessionId, unsigned int aInfId, avector<Vars::Raw> & aSession );

	/**
	 *  Обновление заданной сессии.
	 * @param aSessionId - идентификатор сессии.
	 * @param aVarList - список имён и значений переменных.
	 */
	InfEngineErrors CmdUpdateSession( unsigned int aSessionId, const avector<Vars::Raw> & aVarList );

public:
	/**
	 *  Запрос ответа инфа на реплику пользователя.
	 * @param aSessionId - идентификатор сессии.
	 * @param aInfId - идентификатор инфа.
	 * @param aRequest - реплика пользователя.
	 * @param aData - результат запроса подготовленный для оправки через процесс сервер.
	 * @param aVarsList - список переменных.
	 * @param aCSDataAllocator - менеджер памяти для размещения результата запроса aData.
	 */
	InfEngineErrors CmdRequest( unsigned int aSessionId, unsigned int aInfId,
								const char * aRequest, unsigned int aRequestLength,
								const avector<Vars::Raw> & aVarsList,
								csdata_data_t ** aData, nMemoryAllocator & aCSDataAllocator );


public:
	/**
	 *  Удаление инфа из кэша.
	 * @param aInfId - идентификатор инфа.
	 */
	InfEngineErrors CmdPurgeInf( unsigned int aInfId );

	/**
	 *  Добавление инфа в кэш. При этом происходит компиляция шаблонов инфа.
	 * @param aInfId - идентификатор инфа.
	 * @param aTemplates - шаблоны инфа.
	 * @param aInfProfile - профиль инфа.
	 * @param aDictsNames - список имён пользовательских словарей.
	 * @param aDicts - список пользовательских словарей.
	 * @param aDictsNumber - количество пользовательских словарей.
	 */
	InfEngineErrors CmdSetInf( unsigned int aInfId, const char * aTemplates, avector<Vars::Raw> & aInfProfile,
							   const char ** aDictsNames, const char ** aDicts, unsigned int aDictsNumber );

	/**
	 *  Установка максимального количества слов во фразах для раскрытия эллипсиса.
	 * @param aMaxEllRequestLength - максимальное количество слов во фразах для раскрытия эллипсиса.
	 */
	void SetMaxEllRequestLength( unsigned int aMaxEllRequestLength )
	{ vMaxEllRequestLength = aMaxEllRequestLength; }

	/**
	 *  Установка логики выбора ответов с флагом UNKNOWN.
	 * @param aSkipUnknownLogic - тип логики выбора ответа.
	 */
	void SetSkipUnknownLogic( InfBFinderExt::SkipUnknownLogic aSkipUnknownLogic )
	{ vFinder.SetSkipUnknownLogic( aSkipUnknownLogic ); }

	/**
	 *  Проверка подписи.
	 * @param aSignature - подпись.
	 */
	bool CheckSignature( const unsigned char aSignature[16] ) const;


private:
	InfEngineErrors LoadUserBase( unsigned int aInfId, RequestStat & aRequestStat, bool & aSuccess );

	/**
	 *  Обработка шаблон-ответов, связанных с событием aEvent.
	 * @param aEvent - событие.
	 */
	InfEngineErrors ProcessEvent( InfPattern::Events aEvent );

	InfEngineErrors GetInfAnswer( const char * aUserRequest, unsigned int aUserRequestLength, RequestStat & aRequestStat );

	InfEngineErrors AddInstruct( Vars::Id aVarId, const char * aValue, unsigned int aValueLength );

	/**
	 *  Определение, является ли запрос пользователя событием.
	 * @param aString - строка содержащая запрос пользователя.
	 * @param aStringLength - длина запроса пользователя.
	 */
	bool DetectEvent( const char * aString, unsigned int aStringLength ) const;

	/**
	 *  Подсчет длины запроса пользователя в словах и добавление ее в качестве значения автоматической переменной.
	 * @param aUserRequest - запрос пользователя.
	 * @param aUserRequestLength - длина запроса пользователя.
	 * @param aSession - сессия.
	 */
	InfEngineErrors CountRequestLength( const char * aUserRequest, unsigned int aUserRequestLength, Session & aSession );

	/**
	 *  Подсчет длины запроса пользователя в словах.
	 * @param aUserRequest - запрос пользователя.
	 * @param aUserRequestLength - длина запроса пользователя.
	 * @param aWordsCnt - результат.
	 */
	InfEngineErrors CountRequestLength( const char * aUserRequest, unsigned int aUserRequestLength, unsigned int & aWordsCnt );


public:
	/**
	 *  Установка метода слияния словарей из основной и пользовательской базы.
	 * @param aJoiningMethod - метод слияния словарей.
	 */
	void SetDictMergeMethod( InfDictWrapManipulator::JoiningMethod aJoiningMethod )
	{
		vJoiningMethod = aJoiningMethod;
		vFinder.SetDictMergeMethod( aJoiningMethod );
	}

	/**
	 *  Флаг, показывающий нужно ли использовать пользовательские словари для поиска по основной базе.
	 * @param aUseUserDictsInMainBaseSearch - true, если нужно использовать пользовательские словари для поиска по основной базе. Иначе - false.
	 */
	void SetDictMergeEffect( bool aUseUserDictsInMainBaseSearch )
	{ vFinder.SetDictMergeEffect( aUseUserDictsInMainBaseSearch ); }

private:
	// Состояние сервера.
	enum class State
	{
		Closed, // База закрыта. сервер не готов к работе.
		Opened, // База открыта.
	} vState { State::Closed };

	// Алгоритм раскрытия эллипсиса
	EllipsisMode vEllipsisMode { EllipsisMode::Off };

	// Настройка системы индивидуальной конфигурации раскрытия эллипсиса для инфов.
	EllipsisDefault vEllipsisDefault = EllipsisDefault::None;

	// Кэш для кастомизированных инфов и сессий.
	NanoLib::CacheMemcached vCache;

	// Сессия.
	Session vUserSession;

	// Данные инфа, загруженные из кэша.
	mutable char * vUserBaseBuffer { nullptr };

	// Компилятор для профиля инфа.
	InfProfileWR vInfProfileWR;

	// Указатель на функцию раскрытия эллипсиса.
	EllipsisDiscloseFunction vEllipsisDiscloseFunction { nullptr };

	// Таймаут на раскрытие эллипсиса.
	unsigned int vEllipsisTimeOut { 10 };

	// Максимальное количество слов во фразах для раскрытия эллипсиса.
	unsigned int vMaxEllRequestLength;

	// Метод слияния словарей из основной и пользовательской базы.
	InfDictWrapManipulator::JoiningMethod vJoiningMethod { InfDictWrapManipulator::jmAddiction };

	// Контейнер для результата поиска.
	iAnswer vAnswer;
	// Контейнер для инструкций, полученных через шаблоны, связанные с событиями.
	iAnswer vEventInstructions;
	// Контейнер для ответа при поиске по раскрытому эллипсису.
	iAnswer vEllipsisAnswer;
};

#endif /** __InfEngineServer_hpp__ */
