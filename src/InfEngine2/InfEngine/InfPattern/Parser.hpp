#ifndef PARSER_HPP
#define PARSER_HPP

#include <NanoLib/LogSystem.hpp>

#include "../Indexer.hpp"
#include "StringParser.hpp"

/**
 *  Класс для разбора шаблонов
 */
class DLParser
{
public:
	/** Описание ошибки разбора данных на языке DL. */
	class Error : public DLStringParser::Error
	{
	public:
		/** Конструктор по умолчанию. */
		Error() noexcept {}

		/** Copy - конструктор. */
		Error( const Error & aError ) = delete;

		/** Move - конструктор. */
		Error( Error && aError ) noexcept : DLStringParser::Error { std::move( aError ) }
		{
			std::swap( vString, aError.vString );
			std::swap( vObjectId, aError.vObjectId );
		}

	public:
		/** Копирование описания ошибки. */
		bool assign( const Error & aError )
		{
			DLStringParser::Error::assign( aError );
			return vString.assign( aError.vString ) == nlrcSuccess && vObjectId.assign( aError.vObjectId ) == nlrcSuccess;
		}

		/** Перенос описания ошибки. */
		bool assign( Error && aError )
		{
			DLStringParser::Error::assign( std::move( aError ) );
			return vString.assign( std::move( aError.vString ) ) == nlrcSuccess &&
				   vObjectId.assign( std::move( aError.vObjectId ) ) == nlrcSuccess;
		}

	public:
		/** Установка описания ошибки. */
		bool assign( const DLStringParser::Error & error, const aTextString & string = {}, const aTextString & object_id = {} )
		{
			DLStringParser::Error::assign( error );
			return vString.assign( string.c_std_str() ) == nlrcSuccess && vObjectId.assign( object_id.c_std_str() ) == nlrcSuccess;
		}

	public:
		/** Установка строки с ошибокой. */
		bool assign_string( const std::string & aString ) { return vString.assign( aString ) == nlrcSuccess; }
		bool assign_string( const aTextString & aString ) { return assign_string( aString.c_std_str() ); }
		bool assign_string( const char * aString, size_t aLength ) { return vString.assign( aString, aLength ) == nlrcSuccess; }

		/** Установка идентификатора объекта, в котором найдена ошибка. */
		bool assign_object_id( const std::string & aObjectId ) { return vObjectId.assign( aObjectId ) == nlrcSuccess; }
		bool assign_object_id( const aTextString & aObjectId ) { return assign_object_id( aObjectId.c_std_str() ); }
		bool assign_object_id( const char * aObjectId, size_t aLength ) { return vObjectId.assign( aObjectId, aLength ) == nlrcSuccess; }

	public:
		const aTextString & GetObjectId() const noexcept { return vObjectId; }
		const aTextString & GetString() const noexcept { return vString; }

	private:
		/** Строка, в которой произошла ошибка. */
		aTextString vString;

		/** Идентификатор объекта ( шаблон, словаря и т.п. ), в котором произошла ошибка. */
		aTextString vObjectId;
	};

	/** Массив ошибок разбора данных на языке DL. */
	class Errors : public NanoLib::nVector<Error>
	{
	public:
		/** Добавление новой ошибки в список. */
		bool push_back( const DLStringParser::Error & aError, const std::string & aString = {}, const std::string & aObjectId = {} )
		{
			return resize( size() + 1 ) && back().assign( aError ) && back().assign_string( aString ) && back().assign_object_id( aObjectId );
		}
		bool push_back( const DLStringParser::Error & aError, const aTextString & aString, const aTextString & aObjectId = {} )
		{
			return push_back( aError, aString.c_std_str(), aObjectId.c_std_str() );
		}

		/** Добавление списка ошибок в список. */
		bool push_back( const DLStringParser::Errors & aErrors, const aTextString & aString = {}, const aTextString & aObjectId = {} )
		{
			for( const auto & error : aErrors )
			{
				Error err;
				if( !err.assign( error, aString, aObjectId ) || !push_back( std::move( err ) ) )
					return false;
			}
			return true;
		}

		using NanoLib::nVector<Error>::push_back;
	};

	/** Массив ошибок компиляции. */
	using ErrorsExt = std::vector<ErrorReport>;

public:
	/**
	 *  Конструктор.
	 * @param aDLDataWR - База данных DL.
     * @param aIndexer - индексатор.
     * @param aSymbolymsBaseRO - база заменяемых символов.
	 * @param aSynonymBaseRO - база синонимов.
	 * @param aAllocator - аллокатор памяти.
	 */
	DLParser( DLDataWR & aDLDataWR, BaseIndexer & aIndexer,
              SymbolymBaseRO * aSymbolymBaseRO, SynonymBaseRO * aSynonymBaseRO, nMemoryAllocator & aAllocator );

public:
	/**
	 *  Считывание шаблонов из файла.
	 * @param aFilePath - путь к файлу.
	 * @param aEllipsisMode - флаг разбора шаблонов эллипсиса.
	 * @param aStrictMode - флаг жесткости компиляции.
	 * @param aEncoding - кодировка шаблонов.
	 */
	InfEngineErrors ReadPatternsFromFile( const std::string & aFilePath, bool aEllipsisMode, bool aStrictMode, NanoLib::Encoding aEncoding );

	/**
	 *  Считывание шаблонов из потока.
	 * @param aStream - поток с шаблонами.
	 * @param aEllipsisMode - флаг разбора шаблонов эллипсиса.
	 * @param aStrictMode - флаг жесткости компиляции.
	 * @param aStreamName - имя потока с шаблонами.
	 * @param aEncoding - кодировка шаблонов.
	 */
	InfEngineErrors ReadPatternsFromStream( std::istream & aStream, bool aEllipsisMode, bool aStrictMode, const std::string & aStreamName = {}, NanoLib::Encoding aEncoding = NanoLib::Encoding::CP1251 );

	/**
	 *  Разрешение зависимостей в шаблонах, например, таких как тэги ExtendAnswer.
	 * @param aSuccessResolving - количество успешно разрешённых зависимостей.
	 * @param aFaultResolving - количество зависимостей, которые не удалось разрешить.
	 * @param aErrors - сообщения ошибок разрешения зависимостей.
	 */
	InfEngineErrors MakeResolving( unsigned int & aSuccessResolving, unsigned int & aFaultResolving, ErrorsExt & aErrors );

	/**
	 *  Считывание шаблонов из буффера.
	 * @param aBuffer - буфер с шаблонами.
	 * @param aBufferSize - размер буфера.
	 * @param aEllipsisMode - флаг разбора шаблонов эллипсиса.
	 * @param aStrictMode - флаг жесткости компиляции.
	 */
	InfEngineErrors ReadPatternsFromBuffer( const char * aBuffer, unsigned int aBufferSize, bool aEllipsisMode, bool aStrictMode );

	/**
	 *  Считывание, компиляция словаря из файла.
	 * @param aFilePath - путь к файлу со словарём.
	 * @param aStrictMode - флаг жёсткости компиляции.
	 * @param aDictName - имя нового словаря.
	 * @param aDictNameLength - длина имени нового словаря.
	 * @param aErrorDescription - описание ошибки компиляции словаря.
	 * @param aErrorString - строка, в которой произошла ошибка компиляции.
	 * @param aEncoding - кодировка данных.
	 */
	InfEngineErrors ReadDictFromFile( const char * aFilePath, bool aStrictMode, const char * aDictName, unsigned int aDictNameLength,
									  aTextString & aErrorDescription, aTextString & aErrorString, NanoLib::Encoding aEncoding );

	/**
	 *  Считывание, компиляция словаря из буфера.
	 * @param aDictName - имя нового словаря.
	 * @param aDictNameLength - длина имени нового словаря.
	 * @param aDict - текст словаря.
	 * @param aDictNameLength - длина текста словаря.
	 * @param aStrictMode - флаг жёсткости компиляции.
	 * @param aErrorDescription - описание ошибки компиляции словаря.
	 * @param aErrorString - строка, в которой произошла ошибка компиляции.
	 * @param aEncoding - кодировка данных.
	 * @param aUserDict - флаг, показывающий, что производится компиляция пользовательского словаря.
	 */
	InfEngineErrors ReadDictFromBuffer( const char * aDictName, unsigned int aDictNameLength, const char * aDict, unsigned int aDictLen, bool aStrictMode,
										aTextString & aErrorDescription, aTextString & aErrorString, NanoLib::Encoding aEncoding, bool aUserDict );

private:
	/**
	 *  Разбор шаблон-строки.
	 * @param aPattern - шаблон-строка.
	 * @param aResult - контейнер для результата.
	 * @param aEllipsisMode - флаг компиляции эллипсисов.
	 * @param aStrictMode - флаг жесткости компиляции.
	 * @param aFilePath - имя разбираемого файла (для формирования сообщений об ошибках).
	 * @param aFilePathLength - длина имени разбираемого файла (для формирования сообщений об ошибках).
	 */
	InfEngineErrors ParsePattern( const aTextString & aPattern, InfPattern & aResult, bool aEllipsisMode, bool aStrictMode, const std::string & aFilePath = {} );

	/**
	 *  Добавляет идентификатор шаблона или метку --Lable в таблицу меток.
	 * @param aIdLable - идентификатор шаблона или метка --Lable.
	 * @param aId - внутренний идентификатор шаблона.
	 */
	InfEngineErrors AddIdLableToTable( const char * aIdLable, unsigned int aId );

public:
	/**
	 *  Экспорт описаний ошибок разбора. При экспорте все опиания переносятся, а не копируются.
	 * @param aErrors - контейнер для описаний ошибок.
	 */
	InfEngineErrors ExtractErrors( Errors & aErrors );

	/**
	 *  Экспорт описаний предупреждений. При экспорте все описания переносятся, а не копируются.
	 * @param aWarnings - контейнер для описаний предупреждений.
	 */
	InfEngineErrors ExtractWarnings( Errors & aWarnings );

	/**
	 *  Экспорт описаний ошибок и предупреждений. При экспорте все описания переносятся, а не копируются.
	 * @param aErrors - контейнер для описаний ошибок.
	 * @param aWarnings - контейнер для описаний предупреждений.
	 */
	InfEngineErrors ExtractEW( Errors & aErrors, Errors & aWarnings );

	/** Получение количества успешно разобранных объектов. */
	unsigned int GetSuccessCount() const { return vSuccessCounter; }

	/** Получение количества неразобранных объектов. */
	unsigned int GetFailCounter() const { return vFailCounter; }


public:
	/** Очистка памяти. */
	void Reset( bool full = false );

private:
	/** Список ошибок разбора DL данных. */
	Errors vErrors;

	/** Список предупреждений разбора DL данных. */
	Errors vWarnings;

	/** Получение описания ошибок разбора DL строки. */
	InfEngineErrors ExportErrors( const DLStringParser & aParser );

private:
	/** Счетчик успешно разобранных объектов. */
	unsigned int vSuccessCounter { 0 };

	/** Счетчик неразобранных объектов. */
	unsigned int vFailCounter { 0 };

	/** Массив шаблон-условий, используемый при разборе шаблонов. */
	avector<InfPatternItems::ConditionRW*> vConditions;

	/** Массив шаблон-вопросов, используемый при разборе шаблонов. */
	avector<InfPatternItems::QuestionWR*> vQuestions;

	/** Ссылка на шаблон-that, используемая при разборе шаблонов. */
	InfPatternItems::QuestionWR * vThat { nullptr };

	/** Массив шаблон-ответов, используемый при разборе шаблонов. */
	avector<InfPatternItems::AnswerRW> vAnswers;

	/** Массив условных операторов уровня шаблона, используемый при разборе шаблонов. */
	avector<InfPatternItems::TLCS_WR*> vTLCS; // template level condition statment

	/** Массив элементов шаблонов, используемый при разборе шаблонов. */
	avector<InfPatternItems::Base*> vItems;

	/** Массив элементов шаблонов, нуждающихся в обработке поcле парсинга всех шаблонов.
	 * Используется при разборе шаблонов.
	 */
	avector<InfPatternItems::Base*> vWaitingForResolvingTmp;
	avector<InfPatternItems::Array*> vWaitingForResolving;

	/** Массив инструкций словаря, используемый при разборе соварей. */
	avector<InfDictInstruction*> vDictInstructions;

	/** Массив строк словаря, используемый при разборе соварей. */
	avector<InfDictString*> vDictStrings;

	/** Массив референций используемый при разборе шаблонов. */
	avector<InfPatternItems::Segment> vReferences;

	/** Список шаблонов, которые могут быть скомпилированы только после парсинга всех шаблонов. */
	avector<InfPattern*> vResolvingQueue;

	/** Таблица соответствия идентификаторов шаблонов их индексам в базе. */
	NanoLib::NameIndex vTablePatternsId;
	avector<unsigned int> vTableIndexId;

	/** Словарь меток --Lable. */
	NanoLib::NameIndex vInstrLableTable;
	/** Таблица соответствия названий меток названиям шаблонов. */
	avector<const char*> vInstrLablePatternNames;

	/** Сообщения об ошибках обработки тэга ExtendAnswer. */
	ErrorReports vExtendAnswerErrors;

	/** Контейнер для разобранных данных DL. */
	DLDataWR & vDLDataWR;

	/** Указатель на индексатор. */
	BaseIndexer & vIndexer;

    /** База заменяемых символов. */
    SymbolymBaseRO & vSymbolymBaseRO;

	/** База синонимов. */
	SynonymBaseRO & vSynonymBaseRO;

	/** Аллокатор памяти. */
	nMemoryAllocator & vAllocator;

	mutable nMemoryAllocator vTmpAllocator;

	/** Разборщик шаблон-строк. */
	DLStringParser vPatternStringParser;

	/** Вспомогательные переменные, используемые при разборе шаблонов. */
	aTextString vString, vObjectId, vBuffer;
};

#endif  // PARSER_HPP
