#ifndef __EllipsisCompiler_hpp__
#define __EllipsisCompiler_hpp__

#include <InfEngine2/InfEngine/CoreWR.hpp>

/**
 *  Компилятор эллипсисов.
 */
class EllipsisCompiler
{
public:
	/**
	 *  Конструктор компилятора.
	 * @param aTmpFilePath2 -
	 * @param aMemoryLimit -
	 */
	EllipsisCompiler( const char * aTmpFilePath2, unsigned int aMemoryLimit ):
		vCore( aTmpFilePath2, aMemoryLimit ) {}

public:

	typedef enum
	{
		/** Успех. */
		rcSuccess = 0,

		/** Неуспех, при отсутствии ошибок. */
		rcUnsuccess = 1,

		/** Ошибка выделения памяти. */
		rcErrorNoFreeMemory = -1,

		/** Неизвестная ошибка. */
		rcErrorFault = -2,

		/** Ошибка доступа. */
		rcErrorAccess = -3,

		/** Некорректные аргументы функции. */
		rcErrorInvArgs = -4,

		/** Некорректный формат конфигурационного файла. */
		rcErrorInvConfigFormat = -5,

		/** Ошибка работы с FStorage. */
		rcErrorFStorage = -6

	} ReturnCode;

	/** Получение текстового описания кода возврата. */
	const char * ConvertReturnCodeToString( ReturnCode aReturnCode ) const;


private:
	/** Создание и инициализация компилятора. */
	ReturnCode Create( );

	/** Уничтожение компилятора. */
	void Destroy( )
	{
		vCore.Destroy( );
	}


public:
	/**
	 *  Компиляция шаблонов эллипсисов.
	 * @param aConfigFilePath - путь к конфигурационному файлу компиляции.
	 * @param aRootPath - путь к корневому каталогу для компиляции. Если этот параметр указан, то значение пути к
	 *                   корневому каталогу компиляции в конфигурационном файле игнорируется.
	 * @param aEncoding - кодировка данных.
	 * @param aSourcesList - файл со списоком абресов файлов с шаблонами.
	 * @param aTargetPath - путь для сохранения скомпиированной базы.
	 * @param aVerbose - уровень вывода информации на экран.
	 */
	ReturnCode Compile( const char * aConfigFilePath, const char * aRootPath, const char * aEncoding,
						const char * aSourcesList, const char * aTargetPath, int aVerbose );


private:
	/**
	 *  Регистрация переменных и значений по умолчанию.
	 * @param aVarsListPath - список переменных, с указанием значений по умолчанию.
	 *
	 * @todo Поддержать регистрацию переменных.
	 */
	ReturnCode ProcessVarsList( const char * aVarsListPath );
	/**
	 *  Компиляция словарей.
	 * @param aDictsListPath - список словарей.
	 *
	 * @todo Поддержать компиляцию словарей.
	 */
	ReturnCode ProcessDictsList( const char * aDictsListPath );
	/**
	 *  Компиляция эллипсисов.
	 * @param aEllipsisListPath - список файлов, содержащих шаблоны эллипсисов.
	 */
	ReturnCode ProcessEllipsisList( const char * aEllipsisListPath );
	/**
	 *  Компиляция шаблонов эллипсисов из указанного файла.
	 * @param aEllipsisPath - файл с шаблонами эллипсисов.
	 */
	ReturnCode CompilePatternsFile( const char * aEllipsisPath );
	/**
	 *  Сохранение результирующей базы.
	 * @param aResultPath - путь к результирующей базе.
	 */
	ReturnCode Save( const char * aResultPath );

private:
	/** Ядро компиляции шаблонов. */
	EliCoreWR vCore;

	/** Уровень вывода данных на экран. */
	unsigned int vVerbose = 1;

	/** Корневой каталог компиляции. */
	const char * vRootPath = nullptr;

	/** Кодировка данных. */
	mutable NanoLib::Encoding vEncoding {};
} ;

#endif  /** __EllipsisCompiler_hpp__ */
