#ifndef INF_PATTERN_ITEMS_TEXT_HPP
#define INF_PATTERN_ITEMS_TEXT_HPP

#include "Base.hpp"

#include <InfEngine2/InfEngine/Synonyms/SynonymBase.hpp>
#include <InfEngine2/InfEngine/Symbolyms/SymbolymBase.hpp>

namespace InfPatternItems
{
	/**
	 *  Представление текстового элемента.
	 */
	class Text : public Base
	{
	public:
		/** Конструктор. */
		Text() { vType = itText; }

		/**
		 *  Конструктор.
		 * @param aBuffer - буфер с сохраненным текстом.
		 */
		Text( const void * aBuffer ) { vType = itText; Set( aBuffer ); }

		/**
		 *  Конструтор.
		 * @param aBuffer - буфер с сохраненным текстом.
		 * @param aBufferSize - размер буфера с сохраненным текстом.
		 */
		Text( const void * aBuffer, unsigned int aBufferSize ) { vType = itText; Set( aBuffer, aBufferSize ); }


	public:
		/**
		 *  Загрузка сохраненного текста.
		 * @param aBuffer - буфер с сохраненным текстом.
		 */
		void Set( const void * aBuffer );

		/**
		 *  Загрузка сохраненного текста.
		 * @param aBuffer - буфер с сохраненным текстом.
		 * @param aBufferSize - размер буфера с сохраненным текстом.
		 */
		void Set( const void * aBuffer, unsigned int aBufferSize );

		/**
		 *  Установка текстового значения.
		 * @param aText - текстовое значение.
		 * @param aTextLength - длина текста.
		 */
		void Set( const char * aText, unsigned int aTextLength );

		/**
		 *  Установка флага, сигнализирующего о том, что текст сотоит только из стоп слов.
		 * @param aStopWordsOnly - истинное значение, если текст сотоит только из стоп слов.
		 */
		void SetStopWordsOnly( bool aStopWordsOnly ) { vStopWordsOnly = aStopWordsOnly; }


	public:
		/**
		 *  Замена в тексте символы и синонимы на каноническую форму.
		 * @param aSymbolymsBase - база заменяемых символов.
		 * @param aSynonymsBase - база синонимов.
		 */
		InfEngineErrors ApplySynonyms( SymbolymBaseRO & aSymbolymsBase, SynonymBaseRO & aSynonymsBase );


	public:
		/** Получение размера памяти, необходимого для сохранения данного элемента. */
		unsigned int GetNeedMemorySize() const;

		/**
		 *  Сохранение элемента в буфер.
		 * @param aBuffer - буфер для сохранения элемента.
		 * @return - размер сохраненного элемента.
		 */
		unsigned int Save( void * aBuffer ) const;


	public:
		/** Получение текста. */
		const char * GetTextString() const { return vBuffer ? vBuffer + 2 * sizeof( uint16_t ) : vText; }

		/** Получение длины текста. */
		unsigned int GetTextLength() const { return vBuffer ? *( (uint16_t*)vBuffer ) : vTextLength; }

		/** Получение текста, полученного заменой слова на главные синонимы. */
		const char * GetNormalizedTextString() const
		{
			return vBuffer ? vBuffer + 2 * sizeof( uint16_t ) + GetTextLength() + 1 : vNormalizedText;
		}

		/** Получение длины текста, полученного заменой слова на главные синонимы. */
		unsigned int GetNormalizedTextLength() const { return vBuffer ? ( (uint16_t*)vBuffer )[1] : vNormalizedTextLength; }

		/** Возвращает истинное значение, если текст состоит только из стоп-слов. */
		bool IfStopWordsOnly() const
		{
			return vBuffer ? *( vBuffer + 2 * sizeof( uint16_t ) + GetTextLength() + 1 + GetNormalizedTextLength() + 1 ) : vStopWordsOnly;
		}


	private:
		/** Текстовая строка. */
		const char * vText { nullptr };

		/** Длина текстовой строки. */
		unsigned int vTextLength { 0 };

		/** Текстовая строка после замены слов на главные синонимы. */
		const char * vNormalizedText { nullptr };

		/** Длина текстовой строки после замены слов на главные синонимы. */
		unsigned int vNormalizedTextLength { 0 };

		/** Буфер с сохраненным текстом. */
		const char * vBuffer { nullptr };

		/** Размер буфера с сохраненным текстом. */
		unsigned int vBufferSize { 0 };

		/** Флаг, устанавливаемый в случае, если текст состоит только из стоп-слов. */
		bool vStopWordsOnly { false };

		/** Размер памяти, необходимой для сохранения аргумента. */
		mutable unsigned int vNeedMemorySize { 0 };
	};
};

#endif /** INF_PATTERN_ITEMS_TEXT_HPP */
