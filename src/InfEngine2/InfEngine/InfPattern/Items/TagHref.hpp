#ifndef __InfPatternItemsTagHref_hpp__
#define __InfPatternItemsTagHref_hpp__

#include "Base.hpp"
#include "Array.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление элемента ссылки.
	 */
	class TagHref : public Base
	{
	public:
		/** Конструктор. */
		TagHref( )
			{ vType = itHref; }

	public:
		/** Установка данных. */
		InfEngineErrors Set( Array * aURL, const char * aTarget, unsigned int aTargetLen, Array * aLink );

	public:
		unsigned int GetNeedMemorySize( ) const;

		unsigned int Save( void * aBuffer ) const;

	private:
		/** URL. */
		Array * vURL = nullptr;

		/** Target. */
		const char * vTarget = nullptr;

		/** Длина Target. */
		unsigned int vTargetLen = 0;

		/** Link. */
		Array * vLink = nullptr;
	};

	/**
	 *  Манипулятор элемента ссылки.
	 */
	class TagHrefManipulator
	{
	public:
		TagHrefManipulator( const void * aBuffer = nullptr ):
			vBuffer( static_cast<const char *>( aBuffer ) ) {}

	public:
		/** Получение ссылки на URL. */
		ArrayManipulator GetURL() const
			{ return ArrayManipulator( vBuffer ? (char*)vBuffer + 2 * sizeof (uint32_t) : nullptr ); }

		/** Получение ссылки на target. */
		const char * GetTarget( ) const
			{ return vBuffer ? (char*)vBuffer + ((uint32_t*)vBuffer)[1] + sizeof (uint16_t) : nullptr; }

		/** Получение ссылки на текст ссылки. */
		ArrayManipulator GetLink() const
			{ return ArrayManipulator( vBuffer ? (char*)vBuffer + *((uint32_t*)vBuffer) :nullptr ); }

	private:
		/** Данные элемента. */
		const char * vBuffer = nullptr;
	};
}

#endif /** __InfPatternItemsTagHref_hpp__ */
