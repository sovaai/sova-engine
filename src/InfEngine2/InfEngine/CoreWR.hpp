#ifndef __CoreWR_hpp__
#define __CoreWR_hpp__

#include "CoreRO.hpp"
#include "InfPattern/Parser.hpp"

#include <InfEngine2/InfEngine/Synonyms/SynonymParser.hpp>
#include <InfEngine2/InfEngine/InfPattern/Parser.hpp>
#include <InfEngine2/InfEngine/InfDict/InfDict.hpp>
#include <InfEngine2/Functions/FunctionLib.hpp>
#include <InfEngine2/InfEngine/InfFunctions/AliasParser.hpp>

/**
 *  Базовое ядро для компиляции.
 */
class BCoreWR
{
protected:
	/**
	 *  Конструктор базового ядра.
	 * @param aMainCore - указатель на основное ядро, если создается дополнительное ядро компиляции.
	 * @param aInfIndexer - индексатор.
     * @param aSymbolymsBaseRO - база заменяемых символов.
     * @param aSynonymBaseRO - база синонимов.
	 * @param aTmpFilePath2 - путь к файлу для временного словарей во время компиляции.
	 * @param aMemoryLimit - максимальня память, которую можно выделять для размещения компилируемых шаблонов.
	 */
	BCoreWR( const BCoreRO * aMainCore, BaseIndexer & aInfIndexer,
             SymbolymBaseRO * aSymbolymsBaseRO, SynonymBaseRO * aSynonymBaseRO,
             const char * aTmpFilePath2, unsigned int aMemoryLimit );


public:
	/** Создание ядра компиляции. */
	InfEngineErrors Create();

	/**
	 *  Сохранение ядра в fstorage.
	 * @param aFStorage - ссылка на открытый fstorage.
	 */
	InfEngineErrors Save( fstorage * aFStorage );

	/** Сброс всех данных. */
	virtual void Destroy();

public:

	/**
	 *  Добавление значения для переменной.
	 * @param aVarId - идентификатор переменной.
	 * @param aVarValue - дефолтное значение переменной.
	 * @param aVarValueLength - длина дефолтного значения переменной.
	 */
	InfEngineErrors AddVariable( Vars::Id aVarId, const char * aVarValue, unsigned int aVarValueLength )
	{
		return vDLData.AddVariable( aVarId, aVarValue, aVarValueLength );
	}

	/**  Установить попись базы.
	 * @param aSignature - 16-байтная подпись.
	 */
	void SetSignature( const unsigned char aSignature[16] )
	{
		vDLData.SetSignature( aSignature );
	}

public:

	/** Получение списка предупреждений, возникших при компиляции. */
	const DLParser::Errors & GetWarnings() const
	{
		return vCompilerWarnings;
	}

	/** Получение списка ошибок компиляции. */
	const DLParser::Errors & GetErrors() const
	{
		return vCompilerErrors;
	}

	/** Получение списка ошибок разрешения зависимостей. */
	const DLParser::ErrorsExt & GetResolvigErrors() const
	{
		return vResolvingErrors;
	}

	/**
	 *  Получение статистичекой информации о реестре тэгов-функций.
	 * @param aRealSize - суммарный размер памяти в байтах, занимаемой всеми тэгами функций.
	 * @param aIndexedSize - суммарный размер памяти в байтах, занимаемой всеми тэгами функций после индексации.
	 * @param aTagFuncCount - количество тэгов-функций.
	 * @param aUniqueTagFuncCount - количество уникальных тэгов-функций.
	 * @param aMultipleTagFuncCount - количество тэгов-функций, значение которых не может быть закэшировано.
	 * @param aHashTableMaxListLen - максимальная длина цепочек, использующихся в хэш-таблице для разрешения коллизий.
	 * @param aHashTableAvgListLen - средняя длина цепочек, использующихся в хэш-таблице для разрешения коллизий.
	 */
	void GetTagFuncRegistryStat( unsigned int & aRealSize, unsigned int & aIndexedSize,
								 unsigned int & aTagFuncCount, unsigned int & aUniqueTagFuncCount,
								 unsigned int & aMultipleTagFuncCount,
								 unsigned int & aHashTableMaxListLen, double & aHashTableAvgListLen) const
	{
		return vDLData.GetTagFuncRegistryStat( aRealSize, aIndexedSize, aTagFuncCount, aUniqueTagFuncCount,
											   aMultipleTagFuncCount, aHashTableMaxListLen, aHashTableAvgListLen );
	}

	/**
	 *  Сохраняет базу синонимов во временный fstorage и загружает
	 *  из него на чтение.
	 */
	InfEngineErrors FinalizeSynonyms( );
	
	/**
	 *  Сохраняет базу заменяемых символов во временный fstorage и загружает
	 *  из него на чтение.
	 */
	InfEngineErrors FinalizeSymbolyms( );



private:
	/**
	 *  Индексатор используемый для индексации словарей.
	 * ВАЖНО: индексатор не изменяется при вызове Create(), не очищается при вызове Close() и не сохраняется при
	 *       вызове Save(). Это нужно делать в наследующих классах.
	 */
	BaseIndexer & vIndexer;


protected:

	/** Состояние ядра. */
	enum
	{
		/** Ядро не инициализировано. */
		stClosed  = 0,
		/** Ядро готово компиляции. */
		stReady = 1,
	} vState = stClosed;

	/** Данные шаблонов и словарей. */
	DLDataWR vDLData;

	/** Разборщик шаблонов языка DL. */
	DLParser vDLParser;

	/** Разбощик алиасов DL функций. */
	AliasParser vAliasParser;

	/** Разборщик файлов с синонимами*/
	SynonymParser vSynonymParser;
	
	/** Аллокатор памяти для хранения разобранных данных. */
	nMemoryAllocator vDataAllocator;

	/** Аллокатор памяти для хранения разобранных алиасов. */
	nMemoryAllocator vAliasAllocator;

	/** Аллокатор памяти для хранения разобранных синонимов. */
	nMemoryAllocator vSynonymAllocator;

	/**
	 * @todo Нужно избавиться.
	 *
	 *  Аллокатор для выделения памяти под временные объекты. Может очищатся при каждом
	 * внешнем вызове.
	 */
	nMemoryAllocator vRuntimeAllocator;

	/** массив ошибок компиляции. */
	DLParser::Errors vCompilerErrors;

	/** массив ошибок компиляции. */
	DLParser::ErrorsExt vResolvingErrors;

	/** массив предупреждений компиляции. */
	DLParser::Errors vCompilerWarnings;
};

/**
 *  Ядро компиляции для шаблонов эллипсисов.
 */
class EliCoreWR : public BCoreWR
{
public:
	/**
	 *  Конструктор ядра компиляции.
	 * @param aTmpFilePath2 - путь к файлу для временного словарей во время компиляции.
	 * @param aMemoryLimit - максимальня память, которую можно выделять для размещения компилируемых шаблонов.
	 */
	EliCoreWR( const char * aTmpFilePath2, unsigned int aMemoryLimit );


public:
	/** Создание ядра компиляции. */
	InfEngineErrors Create();

	/**
	 *  Сохранение ядра в fstorage.
	 * @param aFStorage - ссылка на открытый fstorage.
	 */
	InfEngineErrors Save( fstorage * aFStorage );

	/** Уничтожение ядра компиляции. */
	void Destroy()
	{
		vIndexer.Destroy( );
		BCoreWR::Destroy( );
	}


public:
	/**
	 *  Компиляция шаблона из буффера.
	 * @param aBuffer - буффер с шаблоном.
	 * @param aBufferSize - размер буффера.
	 * @param aPatternName - имя шаблона, которое может быть любым, даже не уникальным.
	 * @param aStrictMode - флаг жесткости компиляции.
	 */
	InfEngineErrors CompilePatternFromBuffer( const char * aBuffer, unsigned int aBufferSize, const aTextString & aPatternName, bool aStrictMode );

protected:
	/** Индексатор для шаблонов эллипсисов. */
	EllipsisIndexer vIndexer;

	/** Вспомогательные переменные. */
	aTextString vPattern1, vPattern2, vPattern3;
};

/**
 *  Ядро компиляции.
 */
class ExtICoreWR : public BCoreWR
{
public:
	/**
	 *  Конструктор.
	 * @param aMainCore - указатель на основное ядро, если создается дополнительное ядро компиляции.
     * @param aSymbolymsBaseRO - база заменяемых символов.
     * @param aSynonymBaseRO - база синонимов.
	 * @param aTmpFilePath2 - путь к файлу для временного словарей во время компиляции.
	 * @param aMemoryLimit - максимальня память, которую можно выделять для размещения компилируемых шаблонов.
	 */
    ExtICoreWR( const ICoreRO * aMainCore,
                SymbolymBaseRO * aSymbolymBaseRO, SynonymBaseRO * aSynonymBaseRO,
				const char * aTmpFilePath2, unsigned int aMemoryLimit );


public:

	/**
	 *  Создание и инициализация ядра.
	 * @param aMainCore - ссылка на базовое ядро системы, если предполагается компиляция данных с использованием
	 *                   данных основного ядра.
	 */
	InfEngineErrors Create();

	/**
	 *  Сохранение ядра в fstorage.
	 * @param aFStorage - ссылка на открытый fstorage.
	 */
	InfEngineErrors Save( fstorage * aFStorage );

	/** Уничтожение ядра. */
	void Destroy()
	{
		vIndexer.Destroy( );
		BCoreWR::Destroy( );
	}

	/** Освобождение памяти, выделенной ранее при замене синонимов. */
	void ResetSynonymsMaps()
	{
		if( vMainCore )
			vMainCore->GetIndexBase().GetSynonymBase().Reset();
	}


public:
	/**
	 *  Установка значения по умолчанию для переменной.
	 * @param aVarId - идентификатор переменной.
	 * @param aVarValue - значение переменной.
	 * @param aVarValueLength - длина значения переменной.
	 */
	InfEngineErrors SetDefaultVariableValue( Vars::Id aVarId, const char * aVarValue, unsigned int aVarValueLength );


public:
	/**
	 *  Добавление переменной.
	 * @param aVarName - имя переменной.
	 * @param aVarNameLength - длина значения переменной.
	 * @param aVarId - зарегистрированный идентификатор переменной.
	 */
	InfEngineErrors RegistryNewVariable( const char * aVarName, unsigned int aVarNameLength, Vars::Id & aVarId );

	/**
	 *  Добавление внешней функции.
	 * @param aFuncPath - путь к динамической библиотеки.
	 * @param aFuncId - идентификатор функции.
	 */
	InfEngineErrors RegistryNewFunction( const char * aFuncPath, unsigned int & aFuncId );

	/**
	 *  Добавление внутренней функции.
	 * @param aFuncInfo - информация о функции.
	 * @param aFuncId - идентификатор функции.
	 */
	InfEngineErrors RegistryNewFunction( const DLFunctionInfo * aFuncInfo, unsigned int & aFuncId );

	/**
	 *  Считывание, компиляция и добавление шаблонов из файла.
	 * @param aFilePath - путь к файлу с шаблонами.
	 * @param aSuccessPatterns - количество успешно разобранных шаблонов.
	 * @param aFaultPatterns - количество шаблонов с ошибками.
	 * @param aenc - кодировка данных.
	 */
	InfEngineErrors CompilePatternsFromFile( const std::string & aFilePath, unsigned int & aSuccessPatterns,
											 unsigned int & aFaultPatterns, NanoLib::Encoding aenc );

	/**
	 *  Разрешение зависимостей в шаблонах, например, таких как тэги ExtendAnswer.
	 * @param aSuccessResolving - количество успешно разрешённых зависимостей.
	 * @param aFaultResolving - количество зависимостей, которые не удалось разрешить.
	 */
	InfEngineErrors MakeResolving( unsigned int & aSuccessResolving, unsigned int & aFaultResolving );

	/**
	 *  Считывание, компиляция и добавление шаблонов из буффера.
	 * @param aBuffer - буффер с шаблонами.
	 * @param aBufferLength - блина буффера.
	 * @param aIgnoreErrors - флаг, настраивающий жесткость компиляции.
	 */
	InfEngineErrors CompilePatternsFromBuffer( const char * aBuffer, unsigned int aBufferLength, bool aIgnoreErrors );


	/**
	 *  Считывание, компиляция и добавление алиасов из файла.
	 * @param aFilePath - путь к файлу с алиасами.
	 * @param aSuccessAliases - количество успешно разобранных алиасов.
	 * @param aFaultAliases - количество алиасов с ошибками.
	 */
	InfEngineErrors CompileAliasesFromFile( const char * aFilePath, unsigned int & aSuccessAliases,
											unsigned int & aFaultAliases, AliasParser::Errors & aErrors );

	/**
	 *  Регистрация названия словаря. Если такое название уже было зарегистрировано ранее, возвращает INF_ENGINE_WARN_UNSUCCESS.
	 * @param aDictName - имя словаря.
	 * @param aDictNameLength - длина имени словаря.
	 */
	InfEngineErrors RegistryDictName( const char * aDictName, unsigned int aDictNameLength );

	/**
	 *  Считывание, компиляция словаря из файла.
	 * @param aFilePath - путь к файлу со словарём.
	 * @param aDictName - имя нового словаря.
	 * @param aDictNameLength - длина имени нового словаря.
	 * @param aErrorDescription - описание ошибки компиляции словаря.
	 * @param aErrorString - строка, в которой произошла ошибка компиляции.
	 * @param aEncoding - кодировка данных.
	 */
	InfEngineErrors CompileDictFromFile( const char * aFilePath, const char * aDictName, unsigned int aDictNameLength,
										 aTextString & aErrorDescription, aTextString & aErrorString, NanoLib::Encoding aEncoding );

	/**
	 *  Считывание, компиляция словаря из файла.
	 * @param aDictName - имя нового словаря.
	 * @param aDictNameLength - длина имени нового словаря.
	 * @param aDict - текст словаря.
	 * @param aDictNameLength - длина текста словаря.
	 * @param aErrorDescription - описание ошибки компиляции словаря.
	 * @param aErrorString - строка, в которой произошла ошибка компиляции.
	 * @param aEncoding - кодировка данных.
	 * @param aUserDict - флаг, показывающий, что идёт компиляция пользовательского словаря.
	 */
	InfEngineErrors CompileDictFromBuffer( const char * aDictName, unsigned int aDictNameLength, const char * aDict,
										   unsigned int aDictLen, aTextString & aErrorDescription, aTextString & aErrorString,
										   NanoLib::Encoding aEncoding, bool aUserDict );

	/**
	 *  Считывание, компиляция синонимов из файла.
	 * @param aDictName - название, которое будет ассоциировано со словарём синонимов.
	 * @param aDictNameLen - длина названия словаря синонимов, не считая '\0'.
	 * @param aDictPath - путь к файлу со словарём синонимов.
	 * @param aErrors - сообщения об ошибках.
	 * @param aEncoding - кодировка данных.
	 */
	InfEngineErrors CompileSynonymsFromFile( const char * aDictName, unsigned int aDictNameLen,
											 const char * aDictPath, aTextString & aErrors, NanoLib::Encoding aEncoding );
	
	/**
	 *  Считывание, компиляция заменяемых символов из файла.
	 * @param aDictName - название, которое будет ассоциировано со словарём заменяемых символов.
	 * @param aDictNameLen - длина названия словаря заменяемых символов, не считая '\0'.
	 * @param aDictPath - путь к файлу со словарём заменяемых символов.
	 * @param aErrors - сообщения об ошибках.
	 * @param aEncoding - кодировка данных.
	 */
	InfEngineErrors CompileSymbolymsFromFile( const char * aDictName, unsigned int aDictNameLen,
											  const char * aDictPath, aTextString & aErrors, NanoLib::Encoding aEncoding );

    /**
     *  Добавление в базу группы заменяемых символов.
     * @param aGroup - группа заменяемых символов.
     * @param aErrors - сообщения об ошибках.
     */
    InfEngineErrors AddSymbolymGroup( const SynonymParser::SynonymGroup & aGroup, aTextString & aErrors );


protected:
	/** Индексатор для шаблонов. */
	InfIndexer vIndexer;

	/** Основная база. */
	const ICoreRO* vMainCore;
};

#endif  /** __CoreWR_hpp__ */
