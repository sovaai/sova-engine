#ifndef __InfCompiler_hpp__
#define __InfCompiler_hpp__

#include <InfEngine2/InfEngine/CoreWR.hpp>

#include <InfEngine2/InfEngine/InfOutput.h>

/**
 *  Компилятор для данных языка DL
 */
class InfCompiler
{
public:
	/**
	 *  Конструктор компилятора.
	 * @param aSwapDictsFile - путь к файлу для временного словарей во время компиляции.
	 * @param aSwapMemoryLimit - максимальня память, которую можно выделять для размещения компилируемых шаблонов.
	 */
	InfCompiler( const char * aSwapDictsFile, unsigned int aSwapMemoryLimit ):
        vCoreWR( nullptr, nullptr, nullptr, aSwapDictsFile, aSwapMemoryLimit ) {}


public:
	/** Создание и инициализация компилятора. */
	InfEngineErrors Create( );

	/**
	 *  Сохранение скомпилированной базы.
	 * @param aFilePath - путь к файлу, в который будет произведено сохранение скомпилированной базы.
	 * @param aSignature - подпись ядра.
	 */
	InfEngineErrors Save( const char * aFilePath, const unsigned char aSignature[16] );

	/** Уничтожение компилятора. */
	void Destroy( )
	{
		vCoreWR.Destroy( );
	}


public:
	/**
	 *  Компиляция шаблонов из файла.
	 * @param aFilePath - путь к файлу с шаблонами.
	 * @param aenc - кодировка данных.
	 */
	InfEngineErrors CompilePatternsFromFile( const std::string & aFilePath, NanoLib::Encoding aenc );

	/**
	 *  Компиляция шаблонов из буффера.
	 * @param aBuffer - буфер с шаблонами.
	 * @param aBufferSize - размер буффера с шаблонами.
	 * @param aIgnoreErrors - флаг, показывающий что нужно игнорировать ошибки компиляции.
	 */
	InfEngineErrors CompilePatternsFromBuffer( const char * aBuffer, unsigned int aBufferSize, bool aIgnoreErrors );


	/**
	 *  Считывание, компиляция и добавление алиасов из файла.
	 * @param aFilePath - путь к файлу с алиасами.
	 */
	InfEngineErrors CompileAliasesFromFile( const char * aFilePath );

	/**
	 *  Регистрация названия словаря. Если такое название уже было зарегистрировано ранее, возвращает INF_ENGINE_WARN_UNSUCCESS.
	 * @param aDictName - имя словаря.
	 * @param aDicNameLength - длина имени словаря.
	 */
	InfEngineErrors RegistryDictName( const char * aDictName, unsigned int aDicNameLength );

	/**
	 *  Считывание, компиляция словаря из файла.
	 * @param aFilePath - путь к файлу со словарём.
	 * @param aDictId - идентификатор, который следует присвоить новому словарю.
	 * @param aDictName - имя нового словаря.
	 * @param aDictNameLength - длина имени нового словаря.
	 * @param aErrorDescription - описание ошибоки компиляции словаря.
	 * @param aErrorString - строка, в которой произошла ошибка компиляции.
	 * @param aEncoding - кодировка данных.
	 */
	InfEngineErrors CompileDictFromFile( const char * aFilePath, const char * aDictName, unsigned int aDictNameLength,
										 aTextString & aErrorDescription, aTextString & aErrorString, NanoLib::Encoding aEncoding );


public:
	/**
	 *  Добавление переменной и значения по умолчанию.
	 * @param aVarName - имя переменной.
	 * @param aVarNameLength - длина значения переменной.
	 * @param aVarValue - значение переменной.
	 * @param aVarValueLength - длина значения переменной.
	 */
	InfEngineErrors AddVariable( const char * aVarName, unsigned int aVarNameLength, const char * aVarValue, unsigned int aVarValueLength );

	/**
	 *  Добавление внешней функции и проверка корректности версий.
	 * @param aFuncPath - путь к файлу.
	 */
	InfEngineErrors AddFunction( const char * aFuncPath );

	/**
	 *  Добавление внутренней функции и проверка корректности версий.
	 * @param aFuncInfo - информация о функции.
	 */
	InfEngineErrors AddFunction( const DLFunctionInfo * aFuncInfo );

	/**
	 *  Добавление синонимов из заданного файла со словарём синонимов.
	 * @param aDictName - название, которое будет ассоциировано со словарём синонимов.
	 * @param aDictNameLen - длина названия словаря синонимов, не считая '\0'.
	 * @param aDictPath - путь к файлу со словарём синонимов.
	 * @param aEncoding - кодировка данных.
	 */
	InfEngineErrors AddSynonymsDict( const char * aDictName, unsigned int aDictNameLen, const char * aDictPath, NanoLib::Encoding aEncoding );
	
	/**
	 *  Добавление синонимов из заданного файла со словарём заменяемых символов.
	 * @param aDictName - название, которое будет ассоциировано со словарём заменяемых символов.
	 * @param aDictNameLen - длина названия словаря заменяемых символов, не считая '\0'.
	 * @param aDictPath - путь к файлу со словарём заменяемых символов.
	 * @param aEncoding - кодировка данных.
	 */
    InfEngineErrors AddSymbolymsDict( const char * aDictName, unsigned int aDictNameLen, const char * aDictPath, NanoLib::Encoding aEncoding );


    /**
     *  Добавление в базу группы заменяемых символов.
     * @param aGroup - группа заменяемых символов.
     */
    InfEngineErrors AddSymbolymGroup( const SynonymParser::SynonymGroup & aGroup );

	/**
	 *  Сохраняет базу синонимов во временный fstorage и загружает
	 *  из него на чтение.
	 */
	InfEngineErrors FinalizeSynonyms();
	
	/**
	 *  Сохраняет базу заменяемых символов во временный fstorage и загружает
	 *  из него на чтение.
	 */
	InfEngineErrors FinalizeSymbolyms();


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
								 unsigned int & aHashTableMaxListLen, double & aHashTableAvgListLen ) const
	{
		vCoreWR.GetTagFuncRegistryStat( aRealSize, aIndexedSize, aTagFuncCount, aUniqueTagFuncCount,
										aMultipleTagFuncCount, aHashTableMaxListLen, aHashTableAvgListLen );
	}


	/**
	 *  Завершает компиляцию шаблонов, содержащих элементы наподобие ExternAnswer.
	 */
	InfEngineErrors MakeResolving();


private:
	/** Ядро компиляции шаблонов. */
	ExtICoreWR vCoreWR;
};

#endif /** __InfCompiler_hpp__ */
