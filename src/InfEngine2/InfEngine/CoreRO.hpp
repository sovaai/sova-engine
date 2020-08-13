#ifndef __CoreRO_hpp__
#define __CoreRO_hpp__

#include "DLData.hpp"

#include "IndexBase.hpp"

/**
 *  Базовое поисковое ядро. Обрабатывает внешние сервисы и общие реестры.
 */
class BCoreRO
{
public:
	/**
	 *  Открытие поискового ядра. Данные используются напрямую из fstorage без копирования в память.
	 * @param aFStorage - ссылка на открытый fstorage, содержащий поисковое ядро.
	 * @param aFunctionsRootDir - корневой каталог для функций.
	 * @param aFunctionsConfigPath - путь к конфигурационному файлу функций.
	 */
	InfEngineErrors Open( fstorage * aFStorage, const char * aFunctionsRootDir, const char * aFunctionsConfigPath );

	/** Закрытие поискового ядра. */
	void Close( )
		{ vDLData.Close(); vState = State::Closed; }


public:
	/** Проверка состояния поискового ядра. */
	bool IsOpened( ) const
		{ return vState == State::Opened; }


public:
	/** Доступ к реестру InfPerson'ов. */
	const NanoLib::NameIndex & GetInfPersonRegistry( ) const
		{ return vDLData.GetInfPersonRegistry( ); }

	/** Доступ к данным DL. */
	const DLDataRO & GetDLData( ) const
		{ return vDLData; }


protected:
	/** Состояние поискового ядра. */
	enum class State
	{
		/** Ядро не готово. */
		Closed,
		/** Ядро готово для поиска. */
		Opened
	} vState = State::Closed;

	/** Данные DL. */
	DLDataRO vDLData;
};

/**
 *  Поисковое ядро для поиска по шаблонам языка DL. Включает в себя кроме базового ядра
 * индексную базу шаблонов.
 */
class ICoreRO : public BCoreRO
{
public:
	/**
	 *  Открытие поискового ядра. Данные используются напрямую из fstorage без копирования в память.
	 * @param aFStorage - ссылка на открытый fstorage, содержащий поисковое ядро.
	 * @param aFunctionsRootDir - корневой каталог для функций.
	 * @param aFunctionsConfigPath - путь к конфигурационному файлу функций.
	 */
	InfEngineErrors Open( fstorage * aFStorage, const char * aFunctionsRootDir, const char * aFunctionsConfigPath );

	/** Закрытие поискового ядра. */
	void Close( )
		{ vIndexBase.Close(); BCoreRO::Close(); }


public:
	/** Доступ к индексной базе. */
	InfIndexBaseRO & GetIndexBase( )
		{ return vIndexBase; }

	const InfIndexBaseRO & GetIndexBase( ) const
		{ return vIndexBase; }

	/** Освобождение памяти, выделенной ранее при замене синонимов. */
	void ResetSynonymsMaps()
		{ GetIndexBase().GetSynonymBase().Reset(); }


protected:
	/** Индексная база. */
	InfIndexBaseRO vIndexBase;
};

/**
 *  Расширенное поисковое ядро для поиска по шаблонам языка DL, совмещающее в себе основное и дополнительное
 * поисковые ядра. Дополнительное ядро загружается в виде основания ядра и его основной части. Такое представление
 * дополнительного ядра обусловлено тем, что при использовании одного и того же основания и разных основных частей ядра
 * мы получаем существенную экономию памяти, необходимой для его хранения.
 */
class ExtICoreRO : public ICoreRO
{
public:
	/** Конструктор поискового ядра. */
	ExtICoreRO(): vExtDLData( &vDLData ) {}


public:
	/**
	 *  Открытие основного поискового ядра. Данные используются напрямую из fstorage без копирования в память.
	 * @param aFStorage - ссылка на открытый fstorage, содержащий поисковое ядро.
	 * @param aFunctionsRootDir - корневой каталог для функций.
	 * @param aFunctionsConfigPath - путь к конфигурационному файлу функций.
	 */
	InfEngineErrors Open( fstorage * aFStorage, const char * aFunctionsRootDir, const char * aFunctionsConfigPath );

	/** Закрытие основного и дополнительного поисковых ядер. */
	void Close( )
		{ CloseExtBaseFoundation( ); ICoreRO::Close( ); }


public:
	/** Открытие основания дополнительного поискового ядра. */
	InfEngineErrors OpenExtBaseFoundation( );

	/** Закрытие дополнительного поискового ядра вместе с его основанием. */
	InfEngineErrors CloseExtBaseFoundation( );

	/**
	 *  Открытие дополнительного поискового ядра на базе уже загруженного основания. Данные используются напрямую
	 * из буффера без копирования в память.
	 * @param aBuffer - буфер, содержащий поисковое ядро.
	 * @param aBufferSize - размер буффера, содержащего поисковое ядро.
	 * @param aJoiningMethod - метод совмещения одноимённых словарей из пользовательской и основной базы.
	 */
	InfEngineErrors OpenExtBase( const void * aBuffer, unsigned int aBufferSize, InfDictWrapManipulator::JoiningMethod aJoiningMethod );

	/** Закрытие поискового ядра дополнительной базы. Основание остается открытым. */
	InfEngineErrors CloseExtBase( );


public:
	/** Проверка состояния ядра дополнительной базы. */
	bool IsExtBaseOpened( ) const
		{ return vExtState == State::Opened; }

	/** Проверка состояния основания ядра. */
	bool IsExtBasePrepared( ) const
		{ return vExtState == State::Prepared || vExtState == State::Opened; }


public:
	/** Доступ к данным DL дополнтиельного поискового ядра. */
	DLDataRO & GetExtDLData( )
		{ return vExtDLData; }

	/** Доступ к индексной базе дополнительного поискового ядра. */
	InfIndexBaseRO & GetExtIndexBase( )
		{ return vExtIndexBase; }

	/**
	 *  Доступ к значениям переменных поисковых ядер. При этом, считается, что значения взятые из дополнительного
	 * поискового ядра перекрывают значения переменных взятых из основоного ядра.
	 */
	const Vars::Vector & GetVarsValues( ) const
		{ return vExtDLData.GetDefaultVars(); }

	/** Освобождение памяти, выделенной ранее при замене синонимов. */
	void ResetSynonymsMaps()
		{ GetIndexBase().GetSynonymBase().Reset(); }

	/**
	 *  Получение чексуммы пользовательских словарей.
	 * @param aCheckSum - буфер для записи чексуммы (16 байт).
	 */
	void GetUserDictsCheckSum( unsigned char aCheckSum[16] )
		{ vExtDLData.GetDictsCheckSum( aCheckSum ); }


private:
	/** Состояние поискового ядра дополнительной базы. */
	enum class State
	{
		/** Поисковое ядро дополнительной базы и его основание закрыто. */
		Closed,
		/** Поисковое ядро дополнительной базы закрыто, но его основание открыто. */
		Prepared,
		/** Поисковое ядро дополнительной базы открыто. */
		Opened
	} vExtState = State::Closed;

	/** Данные DL дополнительной базы. */
	DLDataRO vExtDLData;

	/** Индексная база дополнительного поискового ядра. */
	InfIndexBaseRO vExtIndexBase;

	/** Основание дополнительной базы. */
	fstorage * vFoundation = nullptr;
};

/**
 *  Поисковое ядро для шаблонов эллипсисов.
 */
class EliCoreRO : public BCoreRO
{
public:
	/**
	 *  Открытие поискового ядра. Данные используются напрямую из fstorage без копирования в память.
	 * @param aFStorage - ссылка на открытый fstorage, содержащий поисковое ядро.
	 */
	InfEngineErrors Open( fstorage * aFStorage );

	/** Закрытие поискового ядра. */
	void Close( )
		{ vIndexBase.Close(); BCoreRO::Close(); }


public:
	/** Доступ к индексной базе поискового ядра. */
	EllipsisIndexBaseRO & GetIndexBase( )
		{ return vIndexBase; }


private:
	/** Индексная база поискового ядра. */
	EllipsisIndexBaseRO vIndexBase;
};

#endif  /** __CoreRO_hpp__ */
