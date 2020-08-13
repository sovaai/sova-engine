#ifndef SYMBOLYM_BASE_HPP
#define SYMBOLYM_BASE_HPP

#include <lib/fstorage/fstorage.h>

#include <NanoLib/LogSystem.hpp>
#include <NanoLib/NameIndex.hpp>

#include "../Synonyms/SynonymParser.hpp"

/**
 * Структура для сериализации и доступа к описанию заменяемого символа.
 */
class SymbolymAttrs {
	public:
		/** Привязывает структуру в режиме RO к буферу с данными. */
		SymbolymAttrs( const char * aBuffer = nullptr ) : vBuffer( aBuffer ) {}

		/** Создаёт буфер, сохраняет в него данные и привязывает к нему структуру. */
		bool Init( nMemoryAllocator & aMemoryAllocator, const SynonymParser::Synonym & aMajor, const SynonymParser::Synonym & aMinor );

		/** Возвращает размер структуры в байтах. */
		unsigned int GetSize() const {
			return vBuffer ? reinterpret_cast<const std::uint16_t*>( vBuffer )[0] : 0;
		}

		/** Возвращает длину главного символа в байтах (без терминирующего нуля). */
		unsigned int GetMajorLength() const {
			return vBuffer ? reinterpret_cast<const std::uint16_t*>( vBuffer )[1] : 0;
		}

		/** Возвращает длину заменяемого символа в байтах (без терминирующего нуля). */
		unsigned int GetMinorLength() const {
			return vBuffer ? reinterpret_cast<const std::uint16_t*>( vBuffer )[2] : 0;
		}

		/** Возвращает главный символ (null-terminated). */
		const char * GetMajor() const {
			return vBuffer ? vBuffer + 3*sizeof( std::uint16_t ) : 0;
		}

		/** Возвращает заменяемый символ (null-terminated). */
		const char * GetMinor() const {
			return vBuffer ? vBuffer + 3*sizeof( std::uint16_t ) + GetMajorLength() + 1 : 0;
		}

		const char * GetBuffer() const {
			return vBuffer;
		}

	private:
		const char * vBuffer = nullptr;
};

/**
 *  Класс для компиляции синонимов и последуещей работы с ними.
 */
class SymbolymBaseWR {
	public:
		SymbolymBaseWR( fstorage_section_id aSectionId );

		/** Создание базы заменяемых символов в режиме WR. */
		InfEngineErrors Create( );

		/** Получение необходимого размера памяти для сохранения объекта. */
		unsigned int GetNeedMemorySize( ) const;

		/**
		 *  Сохранение базы  заменяемых символов в буфер.
		 * @param aBuffer - буфер.
		 * @param aBufferSize - размер буфера в байтах.
		 * @param aUsed - размер использованной части буфера в байтах.
		 */
		InfEngineErrors Save( char * aBuffer, unsigned int aBufferSize, unsigned int & aUsed );

		/**
		 *  Сохранение базы  заменяемых символов в fstorage.
		 * @param aFStorage - открытый fstorage.
		 */
		InfEngineErrors Save( fstorage * aFStorage );

		/** Очистка объекта. */
		void Reset( );

		/** Закрытие открытой ранее базы синонимов. */
		void Close( );

		/**
		 *  Компилирует группу заменяемых символов.
		 * @param aSynGroup - группа заменяемых символов (один главный символ и несколько заменяемых).
		 * @param aSymGroupN - номер группы в словаре.
		 */
		InfEngineErrors AddSymbolymGroup( const SynonymParser::SynonymGroup & aSymGroup, unsigned int aSymGroupN );

		/**
		 *  Возвращает сообщение о последней ошибке.
		 */
		const char * GetErrorMsg() const;

	private:
		/** Служебная переменная. Сообщение об ошибке. */
		aTextString vErrorMsg;

		/** Идентификатор секции заменяемых символов. */
		fstorage_section_id vExtSectionID;

	protected:
		/** Индекс символов. */
		NanoLib::NameIndex vSymbolsIndex;

		/** Список заменяемых символов. */
		avector<SymbolymAttrs> vSymbols;

		/** Менеджер памяти. */
		nMemoryAllocator vMemoryAllocator;
};

/** Класс для работы со скомпилированными заменяемыми символами. */
class SymbolymBaseRO {
	public:
		SymbolymBaseRO( fstorage_section_id aSectionId );

		/**
		 *  Открытие базы заменяемых символов.
		 * @param aFStorage - fstorage, содержащий базу заменяемых символов.
		 */
		InfEngineErrors Open( fstorage* aFStorage );

		/** Освобождение памяти выделенной под замены и карты соответствия. */
		void Close( ) {
			vBuffer = nullptr;
		}

		/**
		 *  Заменяет символы в заданном тексте.
		 * @param aText - текст.
		 * @param aTextLen - длина текста.
		 * @param aNormalizedText - текст, полученный из заданного путём замены символов.
		 * @param aNormalizedTextLen - длина полученного текста.
		 */
		InfEngineErrors ApplySymbolyms( const char * aText, unsigned int aTextLen, const char *& aNormalizedText, unsigned int & aNormalizedTextLen );

		/**
		 *  Заменяет символы в заданном тексте.
		 * @param aText - текст.
		 * @param aTextLen - длина текста.
		 * @param aNormalizedText - текст, полученный из заданного путём замены символов.
		 * @param aNormalizedTextLen - длина полученного текста.
		 * @param aMap - индексы символов в исходной строке, соответствующие символам в полученной сроке.
		 *               Имеет длину aNormalizedTextLen.
		 */
		InfEngineErrors ApplySymbolyms( const char * aText, unsigned int aTextLen,
										const char *& aNormalizedText, unsigned int & aNormalizedTextLen,
										const unsigned int *& aMap );

	protected:
		/**
		 *  Заменяет символы в заданном тексте.
		 * @param aText - текст.
		 * @param aTextLen - длина текста.
		 * @param aNormalizedText - текст, полученный из заданного путём замены символов.
		 * @param aNormalizedTextLen - длина полученного текста.
		 * @param aMap - индексы символов в исходной строке, соответствующие символам в полученной сроке.
		 *               Имеет длину aNormalizedTextLen.
		 * @param aConstructMap - указывает, нужно ли строить карту соответствия.
		 */
		InfEngineErrors ApplySymbolymsAndConstructMap( const char * aText, unsigned int aTextLen,
													   const char *& aNormalizedText, unsigned int & aNormalizedTextLen,
													   const unsigned int *& aMap, bool aConstructMap );

		/** Возвращает количество заменяемых символов. */
		unsigned int GetSymbolsCount() const {
			return ( vBuffer ? reinterpret_cast<const std::uint32_t*>(vBuffer)[1] : 0 );
		}

		/**
		 *	Возвращает описание заданного символа.
		 * @param aSymbolN - номер символа.
		 */
		const SymbolymAttrs GetSymbol( unsigned int aSymbolN ) {
			return vBuffer &&
					aSymbolN < GetSymbolsCount()
						? SymbolymAttrs{ vBuffer + reinterpret_cast<const std::uint32_t*>(vBuffer)[2+aSymbolN] }
						: SymbolymAttrs();
		}

	protected:
		/** Буфер со скомпилированными данными. */
		const char * vBuffer = nullptr;

		/** Менеджер памяти. */
		mutable nMemoryAllocator vMemoryAllocator;

		/** Идентификатор секции индексов главных синонимов и названий словарей синонимов. */
		fstorage_section_id vExtSectionID;

	private:
		// Служебная переменная.
		aTextString vNormalizedText;
		/** Служебный объект для храрнения карты соответствия. */
		avector<unsigned int> vMap;
};

#endif // SYMBOLYM_BASE_HPP

