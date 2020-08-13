#ifndef INF_PATTERN_ITEMS_DICT_INLINE_HPP
#define INF_PATTERN_ITEMS_DICT_INLINE_HPP

#include "Base.hpp"

#include <InfEngine2/InfEngine/Synonyms/SynonymBase.hpp>
#include <InfEngine2/InfEngine/Symbolyms/SymbolymBase.hpp>

/**
 *  Пространство имен элементов шаблонов на языке DL.
 */
namespace InfPatternItems
{
	/**
	 *  Представление inline-словаря.
	 */
	class TagDictInline : public Base
	{
	public:
		TagDictInline() { vType = itDictInline; }

	public:
		/** Установка данных. */
		InfEngineErrors Set( char ** aStrings, unsigned int * aStringsLens, unsigned int aStringsNum );

	public:
		/** Добавление пустого варианта в словарь. */
		void AddEmptyVariant() { vHasEmpty = true; }

		/** Проверка на наличие пустого вараинта в словаре. */
		bool HasEmptyVariant() const { return vHasEmpty; }

	public:
		/** Получение количества элементов в inline словаре. */
		unsigned int GetStringsNumber() const { return vStringsNum; }

		/** Получение aNum'той строки inline словаря. */
		const char * GetString( unsigned int aNum ) const { return vStrings[aNum]; }

		/** Получение длины aNum'той строки inline словаря. */
		unsigned int GetStringLength( unsigned int aNum ) const { return vStringsLens[aNum]; }

		/** Получение количества нормализованных элементов в inline словаре. */
		unsigned int GetNormalizedStringsNumber() const { return vNormalizedStringsNum; }

		/** Получение aNum'той нормализованной строки inline словаря. */
		const char * GetNormalizedString( unsigned int aNum ) const { return vNormalizedStrings[aNum]; }

		/** Получение длины aNum'той нормализованной строки inline словаря. */
		unsigned int GetNormalizedStringLength( unsigned int aNum ) const { return vNormalizedStringsLens[aNum]; }

	public:
		/**
		 *  Замена в тексте инлайн словаря синонимы на канонический вид.
		 * @param aSymbolymsBase - база заменяемых символов.
		 * @param aSynonymsBase - база синонимов.
		 * @param aAllocator - аллокатор памяти.
		 * @todo Нужно избавиться от работы с памятью.
		 */
		InfEngineErrors ApplySynonyms( SymbolymBaseRO & aSymbolymsBase, SynonymBaseRO & aSynonymsBase, nMemoryAllocator & aAllocator );

	public:
		/** Получение размера памяти, необходимого для сохранения данного элемента. */
		unsigned int GetNeedMemorySize() const;

		/**
		 *  Сохранение элемента в буфер.
		 * @param aBuffer - буфер для сохранения элемента.
		 * @return - размер сохраненного элемента.
		 */
		unsigned int Save( void * aBuffer ) const;

	protected:
		/** Количество строк. */
		unsigned int vStringsNum { 0 };

		/** Список строк. */
		char ** vStrings { nullptr };

		/** Список длин строк. */
		unsigned int * vStringsLens { nullptr };

		/** Количество строк, полученных подстановкой синонимов. */
		unsigned int vNormalizedStringsNum { 0 };

		/** Список строк, полученных подстановкой синонимов. */
		char ** vNormalizedStrings { nullptr };

		/** Список длин строк, полученных подстановкой синонимов. */
		unsigned int * vNormalizedStringsLens { nullptr };

		/** Флаг, показывающий есть ли в inline словаре пустой вариант. */
		bool vHasEmpty { false };
	};

	/**
	 *  Манипулятор inline-словаря.
	 */
	class TagDictInlineManipulator
	{
	public:
		/** Конструктор. */
		TagDictInlineManipulator( const void * aBuffer = nullptr ) :
			vBuffer( static_cast<const char*>( aBuffer ) ) {}

	public:
		/** Получение флага, показывающего наличие пустого варианта. */
		bool HasEmpty() const { return *vBuffer ? true : false; }

		/** Получение количества строк. */
		unsigned int GetStringsNumber() const { return *( (uint32_t*)( vBuffer + 1 ) ); }

		/** Получение количества нормализованных строк. */
		unsigned int GetNormalizedStringsNumber() const { return ( (uint32_t*)( vBuffer + 1 ) )[1]; }

		/** Получение ссылки на aNum-тую строку. */
		const char * GetString( unsigned int aNum ) const
		{
			return (const char*)vBuffer + sizeof( uint16_t ) + ( (uint32_t*)( vBuffer + 1 + 2 * sizeof( uint32_t ) ) )[aNum];
		}

		/** Получение ссылки на aNum-тую нормализованную строку. */
		const char * GetNormalizedString( unsigned int aNum ) const
		{
			return (const char*)vBuffer +
				   sizeof( uint16_t ) +
				   ( (uint32_t*)( vBuffer +
								  1 +
								  2 * sizeof( uint32_t ) +
								  GetStringsNumber() * sizeof( uint32_t ) ) )[aNum];
		}

	private:
		/** Данные элемента. */
		const char * vBuffer { nullptr };
	};
}

#endif /** __InfPatternItemTagDictInline_h__ */
