#ifndef __Argument_hpp__
#define	__Argument_hpp__

#include "Array.hpp"

namespace InfPatternItems
{
	/**
	 *  Именованный аргумент.
	 */
	class KeywordArgument: public Base
	{
	public:
		/** Констурктор. */
		KeywordArgument( )
		{
			vType = itKeywordArgument;
			Set( "", 0, nullptr, 0 );
		}

		/**
		 *  Конструктор. При копировании аргумента данные не копируются.
		 * @param aArgument - аргумент, который будет скопирован.
		 */
		KeywordArgument( const KeywordArgument & aArgument )
		{
			vType = itKeywordArgument;
			Set( aArgument );
		}

		/**
		 *  Констурктор.
		 * @param aName - имя аргумента.
		 * @param aNameLength - длина имени аргумента.
		 * @param aValue - значение аргумента.
		 */
		KeywordArgument( const char * aName, unsigned int aNameLength, Array & aValue )
		{
			vType = itKeywordArgument;

			Set( aName, aNameLength, aValue );
		}

		/**
		 *  Конструктор.
		 * @param aName - имя аргумента.
		 * @param aNameLength - длина имени аргумента.
		 * @param aElements - набор элементов, представляющих значение.
		 * @param aElementsNumber - количество элементов, представляющих значение.
		 */
		KeywordArgument( const char * aName, unsigned int aNameLength, Base ** aElements, unsigned int aElementsNumber )
		{
			vType = itKeywordArgument;

			Set( aName, aNameLength, aElements, aElementsNumber );
		}

	public:
		/**
		 *  Установка имени и значения аргумента. Данные НЕ копируются.
		 * @param aName - имя вргумента.
		 * @param aNameLength - длина имени аргумента.
		 * @param aValue - значение аргумента.
		 */
		void Set( const char * aName, unsigned int aNameLength, const Array & aValue )
		{
			vName = aName;
			vNameLength = aNameLength;

			vValue.Set( aValue );

			vBuffer = nullptr;
			vBufferSize = 0;

			vNeedMemorySize = 0;
		}

		/**
		 *  Установка имени и значения аргумента. Данные НЕ копируются.
		 * @param aName - имя аргумента.
		 * @param aNameLength - длина имени аргумента.
		 * @param aElements - набор элементов, представляющих значение.
		 * @param aElementsNumber - количество элементов, представляющих значение.
		 */
		void Set( const char * aName, unsigned int aNameLength, Base ** aElements, unsigned int aElementsNumber )
		{
			vName = aName;
			vNameLength = aNameLength;

			vValue.Set( aElements, aElementsNumber );

			vBuffer = nullptr;
			vBufferSize = 0;

			vNeedMemorySize = 0;
		}

		/**
		 *  Использование ранее сохраненного аргумента. Значение установленное через эту функцию не может быть сохранено.
		 * @param aBuffer - указатель на буфер с сохраненным аргументом.
		 */
		void Set( const void * aBuffer, unsigned int aBufferSize )
		{
			vName = nullptr;
			vNameLength = 0;

			vValue.Set( nullptr, 0 );

			vBuffer = static_cast<const char *>( aBuffer );
			vBufferSize = 0;

			vNeedMemorySize = 0;
		}

		/**
		 *  Установка значения через копирование значения существующего аргумента. При копировании аргумента данные не копируются.
		 * @param aArgument - аргумент, который будет скопирован.
		 */
		void Set( const KeywordArgument & aArgument )
			{ Set( aArgument.vName, aArgument.vNameLength, aArgument.vValue ); }

	public:
		/** Получение значения аргумента. */
		ArrayManipulator GetValue( ) const
			{ return ArrayManipulator( vBuffer ? vBuffer + reinterpret_cast<const uint32_t*>( vBuffer )[0] : nullptr ); }

		/**
		 *  Получение имени аргумента.
		 * @param aNameLength -
		 */
		const char * GetName( unsigned int * aNameLength = nullptr ) const
		{
			if( vBuffer )
			{
				if( aNameLength )
					*aNameLength = reinterpret_cast<const uint32_t*>( vBuffer )[1];
				return vBuffer + 2 * sizeof ( uint32_t );
			}
			else
			{
				if( aNameLength )
					*aNameLength = vNameLength;
				return vName;
			}
		}

	public:
		unsigned int GetNeedMemorySize( ) const;

		unsigned int Save( void * aBuffer ) const;

	private:
		/** Имя аргумента. */
		const char * vName = nullptr;
		/** Длина имени аргумента. */
		unsigned int vNameLength = 0;

		/** Значение аргумента. */
		Array vValue;

		/** Буфер с сохраненным аргументом. */
		const char * vBuffer = nullptr;
		/** Размер буфера с сохраненным аргументом. */
		unsigned int vBufferSize = 0;

		/** Размер памяти, необходимой для сохранения аргумента. */
		mutable unsigned int vNeedMemorySize = 0;
	};
}

#endif	/** __Argument_hpp__ */
