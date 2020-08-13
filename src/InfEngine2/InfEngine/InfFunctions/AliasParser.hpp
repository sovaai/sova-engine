#ifndef __AliasParser_hpp__
#define __AliasParser_hpp__

#include <NanoLib/LogSystem.hpp>
#include <InfEngine2/InfEngine/DLData.hpp>
#include <InfEngine2/InfEngine/InfPattern/StringParser.hpp>

class AliasParser
{
public:
    /** Описание ошибки разбора алиаса. */
	struct Error: public DLStringParser::Error
    {
        /** Строка, в которой произошла ошибка. */
        const char * vString;
        /** Длина строки, в которой произошла ошибка. */
        unsigned int vStringLength;
        /** Имя алиаса, при разборе которого произошла ошибка. */
        const char * vAliasName;
        /** Длина имени алиаса, при разборе которого произошла ошибка. */
        unsigned int vAliasNameLength;
    };

    /** Массив ошибок разбора алиасов. */
    typedef avector<Error> Errors;

	/**
	 *  Конструктор класса.
	 * @param aDLDataWR - база.
	 * @param aMemoryAllocator - менеджер памяти.
	 */
	AliasParser( DLDataWR & aDLDataWR, nMemoryAllocator & aMemoryAllocator );

    /**
     *  Считывание алиасов из файла.
     * @param aFilePath - путь к файлу.
     * @param aStrictMode - флаг жесткости компиляции.
     * @param aSuccessAliases - количество успешно разобранных алиасов.
     * @param aFaultAliases - количество алиасов с ошибками.
     */
    InfEngineErrors ReadAliasesFromFile( const char * aFilePath, bool aStrictMode,
                                        unsigned int &aSuccessAliases, unsigned int &aFaultAliases,
                                        Errors & aErrors );

    /**
     *  Разбор алиаса.
     * @param aAlias - строка с алиасом.
     * @param aAliasLength - длина строки с алиасом.
     * @param aStrictMode - флаг жесткости компиляции.
     */
    InfEngineErrors Parse( const char * aAlias, unsigned int aAliasLength, bool aStrictMode, Errors & aErrors );

    /**
     *  Выделение текста допустимого значения аргумента типа TEXT.
     * @param aAlias - строка с алиасом.
     * @param aAliasLength - длина строки с алиасом.
     * @param aPos - позиция, с которой следует начать выделение.
     * @param aText - выделенный текст.
     */
    InfEngineErrors ParseText( const char * aAlias, unsigned int aAliasLength, unsigned int & aStartPos, char *& aText );

private:

    /**
     *  Добавить информацию об ошибке разбора алиаса в список ошибок.
     * @param aErrors - список ошибок.
     * @param aErrorId - код ошибки.
     * @param aErrorPos - позиция, в которой обнаружена ошибка.
     * @param aString - строка, в которой произошла ошибка разбора.
     * @param aStringLength - длина строки с ошибкой.
     * @param aAliasName - имя алиаса, при разборе которого произошла ошибка.
     * @param aAliasNameLength - длина имени алиаса.
     */
	InfEngineErrors AddParsingError( Errors & aErrors, DLErrorId aErrorId, unsigned int aErrorPos,
                                    const char * aString, unsigned int aStringLength,
                                    const char * aAliasName = nullptr, unsigned int aAliasNameLength = 0 );

    /**
     *  Выделяет из строки имя алиаса.
     * @param aAlias - строка с алиасом.
     * @param aAliasLength - длина строки с алиасом.
     * @param aStartPos - начальная позиция.
     */
    unsigned int ParseAliasName( const char * aAlias, unsigned int aAliasLength, unsigned int aStartPos ) const;

private:
    // База.
    DLDataWR & vDLDataWR;

    // Менеджер памяти.
    nMemoryAllocator & vMemoryAllocator;
};

#endif // __AliasParser_hpp__
