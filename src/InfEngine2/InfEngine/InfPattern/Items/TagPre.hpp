#ifndef __InfPatternItemsTagPre_hpp__
#define __InfPatternItemsTagPre_hpp__

#include "Base.hpp"
#include "Array.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление элемента ссылки.
	 */
	class TagPre : public Base
	{
	public:
		/** Конструктор. */
		TagPre( )
			{ vType = itPre; }

	public:
		/** Установка данных. */
		InfEngineErrors Set( const char * aText, unsigned int aTextLen )
			{ vText = aText; vTextLen = aTextLen; return INF_ENGINE_SUCCESS; }

	public:
		inline unsigned int GetNeedMemorySize( ) const;

		unsigned int Save( void * aBuffer ) const;

	private:            
		/** Текст. */
		const char * vText = nullptr;

		/** Длина текста. */
		unsigned int vTextLen = 0;
	};

	/**
	 *  Манипулятор элемента ссылки.
	 */
	class TagPreManipulator
	{
	public:
		TagPreManipulator( const void * aBuffer = nullptr ):
			vBuffer( static_cast<const char*>( aBuffer ) ) {}

	public:
		/** Получение текста. */
		inline const char * GetText() const
			{ return vBuffer ? vBuffer + sizeof(uint32_t) : nullptr; }
                
                /** Получение текста и его длины. */
		inline const char * GetText( unsigned int & aLen ) const
		{ 
                    if( vBuffer ) 
                    {
                        aLen = GetTextLen();
                        return vBuffer + sizeof(uint32_t);
                    }
                    return nullptr;
                }

		/** Получение длины текста. */
                inline unsigned int GetTextLen() const
			{ return vBuffer ? *reinterpret_cast<const uint32_t*>(vBuffer) : 0; }

	private:
		/** Данные элемента. */
		const char * vBuffer = nullptr;
	};
}

#endif /** __InfPatternItemsTagPre_hpp__ */
