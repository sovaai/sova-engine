#ifndef __TagRSS_hpp__
#define __TagRSS_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Тэг обращения к RSS'у.
	 */
	class TagRSS : public Base
	{
	public:
		/** Конструктор. */
		TagRSS( )
			{ vType = itTagRSS; }

	public:
		/**
		 *  Установка значения аргумента URL.
		 * @param aURL - значение URL.
		 * @param aURLLength - длина значения URL.
		 */
		void SetURL( const char * aURL, unsigned int aURLLength )
		{
			if( (aURL && !aURLLength) || (!aURL && aURLLength) )
			{
				vArgURL = nullptr;
				vArgURLLength = 0;
			}
			else
			{
				vArgURL = aURL;
				vArgURLLength = aURLLength;
			}
		}

		/**
		 *  Установка значения аргумента Alt.
		 * @param aAlt - значение Alt.
		 * @param aAltLength - длина значения Alt.
		 */
		void SetAlt( const char * aAlt, unsigned int aAltLength )
		{
			if( (aAlt && !aAltLength) || (!aAlt && aAltLength) )
			{
				vArgAlt = nullptr;
				vArgAltLength = 0;
			}
			else
			{
				vArgAlt = aAlt;
				vArgAltLength = aAltLength;
			}
		}

		/**
		 *  Установка значения аргумента Offset.
		 * @param aOffset - значение Offset.
		 */
		void SetOffset( unsigned int aOffset )
			{ vArgOffset = aOffset; }

		/**
		 *  Установка значения аргумента ShowTitle.
		 * @param aShowTitle - значение ShowTitle.
		 */
		void SetShowTitle( bool aShowTitle )
			{ vArgShowTitle = aShowTitle; }

		/**
		 *  Утановка значения аргумента ShowLink.
		 * @param aShowLink - значение ShowLink.
		 */
		void SetShowLink( bool aShowLink )
			{ vArgShowLink = aShowLink; }

		/**
		 *  Установка значения аргумента UpdatePeriod.
		 * @param aUpdatePeriod - значение UpdatePeriod.
		 */
		void SetUpdatePeriod( unsigned int aUpdatePeriod )
			{ vArgUpdatePeriod = aUpdatePeriod; }

	public:
		unsigned int GetNeedMemorySize( ) const
			{ return 4 * sizeof(uint32_t) + sizeof(uint32_t) + vArgURLLength + 1 + ((vArgURLLength + 1) % sizeof(uint32_t) ? sizeof(uint32_t) - (vArgURLLength + 1) % sizeof(uint32_t) : 0) + sizeof(uint32_t) + vArgAltLength + 1; }

		unsigned int Save( void * aBuffer ) const;

	private:
		/** Аргумент URL. */
		const char * vArgURL = nullptr;
		/** Длина аргумента URL. */
		unsigned int vArgURLLength = 0;

		/** Аругмент Alt. */
		const char * vArgAlt = nullptr;
		/** Длина аргумента Alt. */
		unsigned int vArgAltLength = 0;

		/** Аргумент Offset. */
		unsigned int vArgOffset = 0;

		/** Аргумент ShowTitle. */
		bool vArgShowTitle = false;

		/** Аргумент ShowLink. */
		bool vArgShowLink = false;

		/** Аргумент UpdatePeriod. */
		unsigned int vArgUpdatePeriod = 0;
	};

	/**
	 *  Манипулятор для работы с сериализованным тэгом запроса к RSS.
	 */
	class TagRSSManipulator
	{
	public:
		/**
		 *  Конструктор.
		 * @param aBuffer - буффер содержащий сериализованный тэг запроса RSS.
		 */
		TagRSSManipulator( const void * aBuffer = nullptr )
			{ vBuffer = static_cast<const char *>(aBuffer); }

	public:
		/**
		 *  Установка буффера с сохраненным тэгом запроса к RSS.
		 * @param aBuffer - буффер содержащий сериализованный тэг запроса RSS.
		 */
		void Init( const void * aBuffer )
			{ vBuffer = static_cast<const char *>(aBuffer); }

	public:
		/**
		 *  Получение значения аргумента URL.
		 * @param aURLLength - длина значения аргумента URL.
		 */
		const char * GetURL( unsigned int & aURLLength ) const
		{
			aURLLength = vBuffer ? reinterpret_cast<const uint32_t*>(vBuffer)[4] : 0;
			return vBuffer ? vBuffer + sizeof(uint32_t)*6 : nullptr;
		}

		/** Получение значения аргумента URL. */
		const char * GetURL( ) const
			{ return vBuffer ? vBuffer + sizeof(uint32_t)*6 : nullptr; }

		/** Получение длины аргумента URL. */
		unsigned int GetURLLength() const
			{ return reinterpret_cast<const uint32_t*>(vBuffer)[4]; }

		/**
		 *  Получение значения аргумента Alt.
		 * @param aAltLength - длина значения аргумента Alt.
		 */
		const char * GetAlt( unsigned int & aAltLength ) const
		{
			aAltLength = vBuffer ? reinterpret_cast<const uint32_t*>(vBuffer)[5] : 0 ;
			return vBuffer ? vBuffer + reinterpret_cast<const uint32_t*>(vBuffer)[3] : nullptr;
		}

		/** Получение значения аргумента Alt. */
		const char * GetAlt( ) const
			{ return vBuffer ? vBuffer + reinterpret_cast<const uint32_t*>(vBuffer)[3] : nullptr; }

		/** Получение длины значения аргумента Alt. */
		unsigned int GetAltLength()
			{ return reinterpret_cast<const uint32_t*>(vBuffer)[5]; }

		/** Получение значения аргумента Offset. */
		unsigned int GetOffset( ) const
			{ return reinterpret_cast<const uint32_t*>(vBuffer)[0]; }

		/** Получение значения аргумента ShowTitle. */
		bool GetShowTitle( ) const
			{ return (reinterpret_cast<const uint32_t*>(vBuffer)[1]&0x1) ? true : false; }

		/** Получение значения аргумента ShowLink. */
		bool GetShowLink( ) const
			{ return (reinterpret_cast<const uint32_t*>(vBuffer)[1]&0x2) ? true : false; }

		/** Получение значения аргумента UpdatePeriod. */
		unsigned int GetUpdatePeriod( ) const
			{ return reinterpret_cast<const uint32_t*>(vBuffer)[2]; }

	private:
		/** Данные элемента. */
		const char * vBuffer = nullptr;
	};
}

#endif  /** __TagRSS_hpp__ */
