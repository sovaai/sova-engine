#ifndef __SynonymParser_hpp__
#define __SynonymParser_hpp__

#include <NanoLib/LogSystem.hpp>
#include <NanoLib/nMemoryAllocator.hpp>
#include <NanoLib/Encoding.hpp>
#include "InfEngine2/_Include/Errors.h"
#include <NanoLib/aTextStringFunctions.hpp>

class SynonymParser
{
public:

	/** Синоним. */
	struct Synonym
	{
		char * Text;
		unsigned int Length;
	};

	/** Группа синонимов. */
	struct SynonymGroup
	{
		Synonym Major;
		avector<Synonym> Minors;
	};

public:
	SynonymParser( nMemoryAllocator & aMemoryAllocator ) :
		vMemoryAllocator( aMemoryAllocator ) {}

	/**
	 *  Парсинг файла с описанием синонимов.
	 * @param aFilePath - имя текстового файла со списком.
	 * @param aEncoding - кодировка данных.
	 */
	InfEngineErrors ParseFromFile( const char * aFilePath, NanoLib::Encoding aEncoding );

	/**
	 *  Парсинг одной строки.
	 * @param aString - строка.
	 * @param aLength - длина строки в байтах, не считая терминирующий символ.
	 * @param aLineNumber - номер текущей строки.
	 */
	InfEngineErrors ParseString( const char * aString, unsigned int aLength, unsigned int aLineNumber );

	/**
	 *  Очистка объекта.
	 */
	void Reset();

	/**
	 *  Возвращает количество разобранных групп синонимов.
	 */
	inline unsigned int GetGroupsCount() const
	{
		return vSynonyms.size();
	}

	/**
	 *  Возвращает заданную группу синонимов.
	 * @param aGroupN - номер группы синонимов.
	 */
	inline const SynonymGroup & GetSynonymGroup( unsigned int aGroupN ) const
	{
		return vSynonyms[aGroupN];
	}

	/**
	 *  Возвращает список произошедших во время разбора ошибок.
	 */
	const aTextString & GetErrors() const
	{
		return vErrors;
	}

private:
	/**
	 *  Парсит один сисноним и копирует его в выделенную память.
	 * @param aString - строка для разбора.
	 * @param aLength - длина строки для разбора в байтах.
	 * @param aPos - позиция в строке для начала парсинга. Возвращаемое значение - позиция окончания парсинга.
	 * @param aSyononym - синоним.
	 * @param aLineNumber - номер текущей строки.
	 */
	InfEngineErrors ParseSynonym( const char * aString, unsigned int & aPos, unsigned int aLength, Synonym & aSynonym,
			unsigned int aLineNumber );

private:
	/** Группы синонимов. */
	avector<SynonymGroup> vSynonyms;

	/** Описание произошедших ошибок парсинга. */
	aTextString vErrors;

	// Менеджер памяти.
	nMemoryAllocator & vMemoryAllocator;
};

#endif	// __SynonymParser_hpp__
