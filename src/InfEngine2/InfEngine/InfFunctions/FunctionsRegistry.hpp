#ifndef __FunctionsRegistry_hpp__
#define __FunctionsRegistry_hpp__

#include <InfEngine2/InfEngine/InfFunctions/InfInternalFunction.hpp>
#include <InfEngine2/InfEngine/InfFunctions/InfExternalFunction.hpp>

/**
 *  Реестр функций языка DL.
 */
class FunctionsRegistry
{
public:
	/** Конструктор реестра. */
	FunctionsRegistry() { Create(); }

	/** Открытие реестра. */
	virtual InfEngineErrors Create();

	/** Закрытие открытого ранее реестра. */
	virtual void Close() = 0;

	/** Получение числа зарегистрированных функций. */
	unsigned int GetFunctionsNumber() const { return vFuncNameIndex.GetNamesNumber(); }

	/**
	 *  Поиск зарегистрированной функции.
	 * @param aFuncName - имя переменной.
	 * @param aFuncNameLength - длина имени переменной.
	 * @param aFuncId - идентификатор найденной переменной.
	 */
	InfEngineErrors Search( const char * aFuncName, unsigned int aFuncNameLength, unsigned int & aFuncId ) const;

	/**
	 * Получение имени функции по идентификатору.
	 * @param aFuncId - идентификатор функции.
	 */
	const char * GetFuncNameById( unsigned int aFuncId ) const { return vFuncNameIndex.GetName( aFuncId ); }

	/**
	 * Получение имени функции по идентификатору.
	 * @param aFuncId - идентификатор функции.
	 * @param aFuncNameLength - длина имени функции.
	 */
	const char * GetFuncNameById( unsigned int aFuncId, unsigned int & aFuncNameLength ) const
	{
		return vFuncNameIndex.GetName( aFuncId, aFuncNameLength );
	}

	/**
	 *  Возвращает адрес реализации функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const FDLFucntion GetFunction( unsigned int aFuncId ) const;

	/**
	 *  Возвращает указатель на тип функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const DLFunctionResType * GetFunctionResType( unsigned int aFuncId ) const = 0;

	/**
	 *  Возвращает указатель на количество аргументов у заданной функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const unsigned int * GetFunctionArgCount( unsigned int aFuncId ) const = 0;

	/**
	 *  Возвращает указатель на информацию об аргументе функции.
	 * @param aFuncId - идентификатор функции.
	 * @param aArgNum - номер аргумента.
	 */
	virtual const DLFucntionArgInfo * GetFunctionArgInfo( unsigned int aFuncId, unsigned int aArgNum ) const = 0;

	/**
	 *  Возвращает истинное значение, если у функции переменное число аргументов.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual bool HasVarArgs( unsigned int aFuncId ) const = 0;

	/**
	 *  Получение MD5 чексуммы реестра (без учёта версий).
	 * @param aDigest - чексумма реестра.
	 */
	virtual void GetMD5Digits( unsigned char aDigest[16] ) const = 0;

	/**
	 *  Проверяет, загружена ли группа функций для эмулирования оператора If.
	 */
	virtual bool IfGroupIsLoaded() const = 0;

	/**
	 *  Возвращает указатель на вектор флагов, описвающих свойства функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const DLFunctionOptions * GetOptions( unsigned int aFuncId ) const = 0;

	/**
	 *  Возвращает указатель на информацию о функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const DLFunctionInfo * GetFunctionInfo( unsigned int aFuncId ) const = 0;

protected:
	/** Индекс имен функций. */
	NanoLib::NameIndex vFuncNameIndex;

	/** Индекс альтернативных имен функций. */
	NanoLib::NameIndex vFuncShortNameIndex;
};


/**
 *  Реестр функций языка DL, используемый для компиляции списка функций.
 */
class FunctionsRegistryWR : public FunctionsRegistry
{
public:
	FunctionsRegistryWR() {}

private:
	FunctionsRegistryWR( const FunctionsRegistryWR& );
	FunctionsRegistryWR& operator=( const FunctionsRegistryWR& );

public:
	/** Открытие реестра. */
	virtual InfEngineErrors Create();

	/** Возвращает размер места, необходимого для сохранения объекта. */
	unsigned int GetNeedMemorySize() const;

	/**
	 *  Сохранение реестра в fstorage.
	 * @param aFStorage - открытый fstorage.
	 */
	InfEngineErrors Save( fstorage * aFStorage ) const;

	/** Закрытие открытого ранее реестра. */
	virtual void Close();

	/**
	 *  Регистрация новой внешней функции. Если такая функция уже существует, то будет возвращен
	 * код ошибки #INF_ENGINE_UNSUCCESS.
	 * @param aFilePath - адрес динамической библиотеки. содержащие функцию.
	 * @param aFuncId - зарегистрированный идентификатор функции.
	 */
	InfEngineErrors RegistryNew( const char * aFilePath, unsigned int & aFuncId )
	{
		ReturnWithTraceExt( Registry( aFilePath, aFuncId ), INF_ENGINE_SUCCESS );
	}

	/**
	 *  Регистрация новой внутренней функции. Если такая функция уже существует, то будет возвращен
	 * код ошибки #INF_ENGINE_UNSUCCESS.
	 * @param aDLFunctionInfo - описание функции.
	 * @param aFuncId - зарегистрированный идентификатор функции.
	 */
	InfEngineErrors RegistryNew( const DLFunctionInfo * aDLFunctionInfo, unsigned int & aFuncId )
	{
		ReturnWithTraceExt( Registry( aDLFunctionInfo, aFuncId ), INF_ENGINE_SUCCESS );
	}

	/**
	 *  Регистрация внешней функции. Если такая функция уже существует, то будет возвращен ее идентификатор.
	 * @param aFilePath - адрес динамической библиотеки, содержащие функцию.
	 * @param aFuncId - зарегистрированный идентификатор функции.
	 */
	InfEngineErrors Registry( const char * aFilePath, unsigned int & aFuncId );

	/**
	 *  Регистрация внутренней функции. Если такая функция уже существует, то будет возвращен ее идентификатор.
	 * @param aDLFunctionInfo - описание функции.
	 * @param aFuncId - зарегистрированный идентификатор функции.
	 */
	InfEngineErrors Registry( const DLFunctionInfo * aDLFunctionInfo, unsigned int & aFuncId );

	/**
	 *  Регистрация всех внешних функций, перечисленных в заданном конфигурационном файле.
	 * @param aFilePath - адрес файла со списком адресов динамических библиотек, содержащих DL-функции.
	 * @param aRootDir - адрес каталога, в котором находятся динамические библиотеки с DL-функциями.
	 */
	InfEngineErrors RegistryAll( const char * aFilePath, const char * aRootDir );

	/**
	 *  Получение MD5 чексуммы реестра (без учёта версий).
	 * @param aDigest - чексумма реестра.
	 */
	virtual void GetMD5Digits( unsigned char aDigest[16] ) const;

	/**
	 *  Возвращает указатель на тип реализации функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const DLFunctionType * GetFunctionType( unsigned int aFuncId ) const;

	/**
	 *  Возвращает указатель на тип функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const DLFunctionResType * GetFunctionResType( unsigned int aFuncId ) const;

	/**
	 *  Возвращает казатель на количество аргументов у заданной функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const unsigned int * GetFunctionArgCount( unsigned int aFuncId ) const;

	/**
	 *  Возвращает казатель на информацию об аргументе функции.
	 * @param aFuncIf - идентификатор функции.
	 * @param aArgNum - номер аргумента.
	 */
	virtual const DLFucntionArgInfo * GetFunctionArgInfo( unsigned int aFuncId, unsigned int aArgNum ) const;

	/**
	 *  Возвращает истинное значение, если у функции переменное число аргументов.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual bool HasVarArgs( unsigned int aFuncId ) const;

	/**
	 *  Проверяет, загружена ли группа функций для эмулирования оператора If.
	 */
	virtual bool IfGroupIsLoaded() const;

	/**
	 *  Возвращает указатель на вектор флагов, описвающих свойства функции.
	 * @param aFuncIf - идентификатор функции.
	 */
	virtual const DLFunctionOptions * GetOptions( unsigned int aFuncId ) const;

	/**
	 *  Возвращает указатель на информацию о функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const DLFunctionInfo * GetFunctionInfo( unsigned int aFuncId ) const;

private:
	/**
	 *  Регистрация функции. Если такая функция уже существует, то будет возвращен ее идентификатор.
	 * @param aFunction - функция.
	 * @param aFuncId - зарегистрированный идентификатор функции.
	 */
	InfEngineErrors Registry( InfFunction * aFunction, unsigned int aFuncId );

private:
	/** Список функций. */
	avector<InfFunction*> vFunctions;

	/** Локальный аллокатор */
	nMemoryAllocator vLocalAllocator;

	/** Текущее MD5 состояние. */
	MD5_CTX vMD5State;

	/** MD5 чексумма. */
	unsigned char vMD5Digest[16];

	/** Флаг наличия группы функций для эмулирования оператора If */
	bool vIfGroupIsLoaded { false };
};




/**
 *  Реестр функций языка DL, используемый для загрузки списка функций.
 */
class FunctionsRegistryRO : public FunctionsRegistry
{
public:
	FunctionsRegistryRO() {}

private:
	FunctionsRegistryRO( const FunctionsRegistryRO& );
	FunctionsRegistryRO& operator=( const FunctionsRegistryRO& );

public:
	/** Закрытие открытого ранее реестра. */
	virtual void Close();

	/**
	 *  Открытие реестра без копирования данных в память.
	 * @param aFStorage - открытый fstorage.
	 * @param aRootDir - корневой каталог для функций.
	 * @param aConfigPath - путь к конфигурационному файлу функций.
	 */
	InfEngineErrors Open( fstorage * aFStorage, const char * aRootDir, const char * aConfigPath );

	/** Возвращает исинное значение, если индекс был успешно открыт. */
	bool IsOpened() const;

	/**
	 *  Возвращает адрес реализации функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const FDLFucntion GetFunction( unsigned int aFuncId ) const;

	/**
	 *  Возвращает указатель на тип функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const DLFunctionResType * GetFunctionResType( unsigned int aFuncId ) const;

	/**
	 *  Возвращает казатель на количество аргументов у заданной функции.
	 * @param aFuncIf - идентификатор функции.
	 */
	virtual const unsigned int * GetFunctionArgCount( unsigned int aFuncId ) const;

	/**
	 *  Возвращает казатель на информацию об аргументе функции.
	 * @param aFuncIf - идентификатор функции.
	 * @param aArgNum - номер аргумента.
	 */
	virtual const DLFucntionArgInfo * GetFunctionArgInfo( unsigned int aFuncId, unsigned int aArgNum ) const;

	/**
	 *  Возвращает истинное значение, если у функции переменное число аргументов.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual bool HasVarArgs( unsigned int aFuncId ) const;

	/**
	 *  Получение MD5 чексуммы реестра (без учёта версий).
	 * @param aDigest - чексумма реестра.
	 */
	virtual void GetMD5Digits( unsigned char aDigest[16] ) const;

	/**
	 *  Проверяет, загружена ли группа функций для эмулирования оператора If.
	 */
	virtual bool IfGroupIsLoaded() const;

	/**
	 *  Возвращает указатель на вектор флагов, описвающих свойства функции.
	 * @param aFuncIf - идентификатор функции.
	 */
	virtual const DLFunctionOptions * GetOptions( unsigned int aFuncId ) const;

	/**
	 *  Возвращает указатель на информацию о функции.
	 * @param aFuncId - идентификатор функции.
	 */
	virtual const DLFunctionInfo * GetFunctionInfo( unsigned int aFuncId ) const;

	/**
	 *  Загрузка реализаций DL функций в память.
	 * @param aRootDir - корневой каталог для функций.
	 * @param aConfigPath - путь к конфигурационному файлу функций.
	 */
	InfEngineErrors LoadDynamicLibraries( const char * aRootDir, const char * aConfigPath );

private:
	/** Признак успешного открытия индекса. */
	bool vIsOpened { false };

	/** Список функций только для чтения. */
	InfFunctionManipulator ** vFunctionsRO { nullptr };

	/** Локальный аллокатор */
	nMemoryAllocator vLocalAllocator;

	/** MD5 чексумма. */
	const unsigned char * vMD5Digest { nullptr };

	/** Флаг наличия группы функций для эмулирования оператора If */
	bool vIfGroupIsLoaded { false };
};

#endif // __FunctionsRegistry_hpp__
