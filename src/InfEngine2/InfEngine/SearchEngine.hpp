#ifndef __SearchEngine_hpp__
#define __SearchEngine_hpp__

#include "ExtFinder.hpp"
#include "CoreRO.hpp"
#include "ExtFinder.hpp"

/**
 *  Базовый класс поискового движка для шаблонов языка DL.
 */
class BSearchEngine
{
protected:
	/** Конструктор поискового движка. */
	BSearchEngine();

	/** Деструктор поискового движка. */
	~BSearchEngine( );


protected:
	/**
	 *  Открытие поисковой базы.
	 * @param aBasePath - путь к файлу с базой.
	 */
	InfEngineErrors OpenBase( const char * aBasePath );

	/** Закрытие поисковой базы. */
	void CloseBase( );


public:
	/**
	 * Получение сигнататуры лингвистических данных из открытой базы.
	 * @param aSignature - буфер с сигнатурой.
	 * @param aSignatureSize - размер буфера в байтах.
	 * @param aSectionID - идентификаторо секции fstorage, в которой сохранена сигнатура.
	 */
	InfEngineErrors GetSignature( const char *& aSignature, unsigned int & aSignatureSize,
								  fstorage_section_id aSectionID = FSTORAGE_SECTION_LD_SIGNATURE ) const;


	/**
	 *  Открытие поисковой базы, получения сигнататуры лингвистических данных, закрытие базы.
	 * @param aBasePath - путь к файлу с базой.
	 * @param aSignature - буфер с сигнатурой.
	 * @param aSignatureSize - размер буфера в байтах.
	 * @param aEllipsis - флаг, показывающий, что используется база эллипсисов.
	 * @param aSectionID - идентификаторо секции fstorage, в которой сохранена сигнатура.
	 */
	InfEngineErrors GetSignature( const char * aBasePath, const char *& aSignature, unsigned int & aSignatureSize,
								  bool aEllipsis, fstorage_section_id aSectionID = FSTORAGE_SECTION_LD_SIGNATURE);

protected:
	InfEngineErrors DetectLangs( const char * aUserRequest, unsigned int aUserRequestLength, Session &aSession, iAnswer &aAnswer ) const;

protected:
	/** Ссылка на fstorage, из которого загружаются данные поисковой базы. */
	fstorage * vFStorage = nullptr;

	/** Менеджер памяти, обслуживающий объекты, существующие на протяжении всей жизни поискового движка. */
	nMemoryAllocator vMemoryAllocator;

    /** Информация, полученная в результате неудачной проверки на совместимость сигнатур движка и лингвистических данных.*/
    aTextString vSignatureCompareInfo;
};

/**
 *  Класс поискового движка для шаблонов языка DL.
 */
class ISearchEngine : public BSearchEngine
{
public:
	/** Конструктор поискового движка. */
	ISearchEngine() = default;

public:
	/**
	 *  Открытие поисковой базы.
	 * @param aFilePath - путь к файлу с базой.
	 */
	InfEngineErrors OpenBase( const char * aBasePath );

	/** Закрытие поисковой базы. */
	void CloseBase( );


public:
	/**
	 *  Генерация ответа на запрос.
	 * @param aSession - данные сессии.
	 * @param aRequest - строка запроса.
	 * @param aRequestLength - длина строки запроса.
	 * @param aAnswer - контейнер для результата.
	 * @param aWeight - вес подобранного шаблона.
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 */
	InfEngineErrors Answer( Session &aSession, const char * aRequest, unsigned int aRequestLength, iAnswer &aAnswer, unsigned int &aWeight, RequestStat & aRequestStat );

	/** Создание сессии. Освобождение памяти должен сделать вызывающий функцию. */
	Session * CreateSession( ) const;

	/** Возвращает ссылку на поисковую базу. */
	const ICoreRO & GetBase( ) const;

protected:
	/** Поисковые данные. */
	ICoreRO vBase;

	/** Поисковик по шаблонам. */
	InfBFinder vFinder;

	/** Поисковик по шаблонам. */
	BFinder vBFinder;
};

/**
 *  Класс движка для поиска ответа по базе шаблонов языка DL, использующий дополнительную базу
 * пользовательских шаблонов при поиске.
 */
class ExtISearchEngine : protected BSearchEngine
{
public:
	/** Конструктор. */
	ExtISearchEngine() = default;
	
public:
	/**
	 *  Открытие поисковой базы.
	 * @param aMainBasePath - путь к файлу с основной базой.
	 * @param aFunctionsRootDir - корневой каталог для функций.
	 * @param aFunctionsConfigPath - путь к конфигурационному файлу функций.
	 */
	InfEngineErrors OpenBase( const char * aBasePath, const char * aFunctionsRootDir, const char * aFunctionsConfigPath );

	/** Закрытие основной и дополнительной поисковых баз. */
	void CloseBase( );
    
public:
	/**
	 *  Генерация ответа на запрос.
	 * @param aSession - данные сессии.
	 * @param aRequest - строка запроса.
	 * @param aRequestLength - длина строки запроса.
	 * @param aAnswer - контейнер для результата.
	 * @param aWeigth - вес подобранного ответа.
	 * @param aAnswerFromUserBase - флаг, показывающий является ли ответ порожденным по шаблону из кастомизирующей базы или же из основной.
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 */
	InfEngineErrors Answer( Session &aSession, const char * aRequest, unsigned int aRequestLength, iAnswer &aAnswer, unsigned int &aWeigth, bool &aAnswerFromUserBase , RequestStat & aRequestStat);

public:
	Session * CreateSession( ) const
	{
		Session * session = new Session( vBase.GetDLData( ).GetVarsRegistry( ) );
		if( session )
			session->SetDefaultValues( vBase.GetDLData( ).GetDefaultVars( ) );

		return session;
	}

    ExtICoreRO & GetBase()
    {
        return vBase;
    }

protected:
	/** Ядро поисковых данных. */
	ExtICoreRO vBase;

	/** Поисковик шаблонов, учитывающий дополнительную базу при поиске. */
	InfBFinderExt vFinder;
};

/**
 *  Класс движка для поиска шаблонов эллипсисов.
 */
class EliSearchEngine
{
public:
	/** Конструктор. */
	EliSearchEngine();

	/** Деструктор. */
	~EliSearchEngine( )
	{
		CloseBase( );
	}


public:

	/** Код возврата. */
	typedef enum
	{
		/** Успех. */
		rcSuccess = 0,

		/** Неуспешное завершение выполнения, не являющееся ошибкой. К примеру, эллипсис не был раскрыт. */
		rcUnsuccess = 1,

		/** Некорректные аргументы. */
		rcErrorInvArgs = -1,

		/** Ошибка fstorage. */
		rcErrorFStorage = -2,

		/** Ошибка выделения памяти. */
		rcErrorNoFreeMemory = -3,

		/** Некорректные данные. */
		rcErrorInvData = -4,

		/** Неизвестная ошибка. */
		rcErrorFault = -5,

		/** База не открыта. */
		rcErrorBaseIsNotOpen = -6,

		/** База уже открыта. */
		rcErrorBaseIsOpened = -7,

		/** Ошибка при работе с функцией. */
		rcErrorDLF = -8
	} ReturnCode;

	/**
	 *  Получение текстового описания кода возврата.
	 * @param aReturnCode - код возврата.
	 */
	const char * ConvertReturnCodeToString( ReturnCode aReturnCode ) const;


public:
	/**
	 *  Загрузка базы эллипсисов.
	 * @param aEllipsisBasePath - путь к базе.
	 */
	ReturnCode LoadEllisisBase( const char * aEllipsisBasePath );

	/** Закрытие базы эллипсисов. */
	void CloseBase( );


public:
	/**
	 *  Раскрытие эллипсиса.
	 * @param aUserRequest - текущий запрос пользователя.
	 * @param aUserRequestLength - длина текущего запроса пользователя.
	 * @param aPreInfResponse - предыдущий ответ инфа.
	 * @param aPreInfResponseLength - длина предыдущего ответа инфа.
	 * @param aPreUserRequest - предыдущая реплика пользователя.
	 * @param aPreUserRequestLength - длина предыдущей реплики пользователя.
	 * @param aResult - контейнер для результата.
	 * @param aEllipsisId - идентификатор шаблона, раскрывшего эллипсис.
	 * @param aEllipsisPartialIDs - список идентификаторов шаблонов с частично раскрывшим эллипсис.
	 * @retval В случае, если раскрыть эллипсис не удалось, возвращается #rcUnsuccess;
	 */
	ReturnCode EllipsisDisclose( const char * aUserRequest, unsigned int aUserRequestLength,
								 const char * aPreInfResponse, unsigned int aPreInfResponseLength,
								 const char * aPreUserRequest, unsigned int aPreUserRequestLength,
								 aTextString &aResult, aTextString &aEllipsisId,
								 aTextString &aEllipsisPartialIDs );

private:
	/** Ссылка на открытый fstorage базы эллипсисов. */
	fstorage * vFStorageMain = nullptr;

	/** Ядро поиска. */
	EliCoreRO vCore;

	/** Поисковик по шаблонам эллипсисов первого типа. */
	InfBFinder vFinderF {};
	/** Поисковик по шаблонам эллипсисов второго типа. */
	InfBFinder vFinderS {};
	/** Поисковик по шаблонам эллипсисов третьего типа. */
	InfBFinder vFinderT {};

	/** Текущая сессия. Для эллипсисов она всегда пуста. */
	Session vSession;

	/** Вспомогательный объект для генерации ответа. */
	iAnswer vAnswer;

	/** Объект для работы с подобранным шаблоном. */
	EliMatchedPattern vMatchedPattern {};

	/** Аллокатор памяти. */
	nMemoryAllocator vRuntimeAllocator;

	/** Аллокатор памяти, который очищается после каждого запроса. */
	nMemoryAllocator vTmpAllocator;
};

#endif  /** __SearchEngine_hpp__ */
