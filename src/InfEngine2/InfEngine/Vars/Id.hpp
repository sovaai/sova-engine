#ifndef INF_ENGINE_VARS_ID_HPP
#define INF_ENGINE_VARS_ID_HPP

#include <iostream>
#include <limits>
#include <cstdlib>
#include <cerrno>

#include <InfEngine2/_Include/Errors.h>

namespace Vars {
	/**
	 *  Идентификатор переменной.
	 */
	class Id {
		public:
			/** Конструктор по умолчанию. */
			Id() noexcept {}

			/** Конструктор копирования. */
			Id( const Id & aVarId ) noexcept {
				vId = aVarId.vId;
			}

			/**
			 *  Конструктор нового идентификатора переменной.
			 * @param aId - числовое представление идентификатора переменной. Может быть любым числом от 0 до 0x7FFE включительно.
			 * @param aTmp - показатель временной переменной, которая не сохраняется в реестре.
			 */
			Id( const unsigned int & aId, bool aTmp ) {
				set( aId, aTmp );
			}

		/**
		 *  Конструктор идентификатора переменной, загруженного из буфера.
		 * @param aBuffer - буфер с сериализованным идентификатором переменной.
		 */
		explicit Id( const char* aBuffer ) noexcept
		{
			auto serialized = std::strtol( aBuffer, nullptr, 10 );
			if( errno != ERANGE && serialized >= 0 && serialized <= std::numeric_limits<unsigned int>::max() )
				load( static_cast<unsigned int>( serialized ) );
		}

		public:
			/**
			 *  Установка идентификатора переменной.
			 * @param aId - числовое представление идентификатора переменной. Может быть любым числом от 0 до 0x7FFE включительно.
			 * @param aTmp - показатель временной переменной, которая не сохраняется в реестре.
			 */
			Id & set( const unsigned int & aId, bool aTmp ) {
				if( aId > vMaxId )
					vId = vMaxId + 1;
				else
					vId = static_cast<std::uint32_t>( aId );

				if( aTmp )
					vId |= vTmpMask;

				return *this;
			}

			/** Установка идентификатора переменной. */
			Id & set( const Id & aVarId ) {
				vId = aVarId.vId; return *this;
			}

			/** Установка идентификатора переменной. */
			Id & operator = ( const Id & aVarId ) noexcept {
				return set( aVarId );
			}

		public:
			/** Сохранение переменной вместе с показателем временности. */
			std::uint32_t serialized() const {
				return vId;
			}

			/** Загрузка сериализованной переменной. */
			void load( std::uint32_t serialized ) {
				vId = serialized;
			}

		public:
			/** Получение числового идентификатора переменной без указания временности. */
			std::uint32_t get() const {
				return vIdMask & vId;
			}

			/** Проверка на временность переменной. */
			bool is_tmp() const {
				return vTmpMask & vId;
			}

			/** Проверка идентификатора. */
			bool valid() const {
				return ( vId & vIdMask ) <= vMaxId;
			}

			/** Проверка идентификатора. */
			explicit operator bool() const {
				return valid();
			}

		public:
			/** Уменьшение значения идентификатора переменной на aVal. */
			Id & operator -= ( const unsigned int & aVal ) {
				if( aVal > get() )
					vId = ( vMaxId + 1 ) | ( vId & vTmpMask );
				else
					vId -= aVal;
				return *this;
			}

			/** Увеличение значения идентификатора переменной на aVal. */
			Id & operator += ( const unsigned int & aVal ) {
				if( aVal > vMaxId - get() )
					vId = ( vMaxId + 1 ) | ( vId & vTmpMask );
				else
					vId += aVal;
				return *this;
			}

			/** Увеличение идентификатора на 1. */
			Id & operator++( int /*i*/ ) {
				return operator += ( 1 );
			}

			/** Уменьшение идентификатора на 1. */
			Id & operator--( int /*i*/ ) {
				return operator -= ( 1 );
			}

		public:
			/** Сравнение идентификатров. */
			bool operator == ( const Id & aVarId ) const {
				return vId == aVarId.vId;
			}

			/** Сравнение идентификатров. */
			bool operator != ( const Id & aVarId ) const {
				return vId != aVarId.vId;
			}

			/** Сравнение идентификатров. */
			bool operator < ( const Id & aVarId ) const {
				return vId < aVarId.vId;
			}

			/** Сравнение идентификатров. */
			bool operator <= ( const Id & aVarId ) const {
				return vId <= aVarId.vId;
			}

			/** Сравнение идентификатров. */
			bool operator > ( const Id & aVarId ) const {
				return vId > aVarId.vId;
			}

			/** Сравнение идентификатров. */
			bool operator >= ( const Id & aVarId ) const {
				return vId >= aVarId.vId;
			}

		public:
			/** Получение макисмально возможного идентификатора. */
			static Id max() noexcept { return Id{ 0x7FFE, true }; }

		private:
			// Маска для флага временности переменной.
			static constexpr std::uint32_t vTmpMask { 0x8000 };
			// Маска для идентификатора переменной.
			static constexpr std::uint32_t vIdMask  { 0x7FFF };
			// Максимальное знаение идентификатора.
			static constexpr std::uint32_t vMaxId	{ 0x7FFE };

			// Идентифкатор переменной.
			std::uint32_t vId { vMaxId + 1 };
	};
}

inline std::ostream & operator << ( std::ostream & aStream, const Vars::Id & aId ) {
	if( aId )
		return aStream << "[" << ( aId.is_tmp() ? "T" : "N" ) << "] " << aId.get();
	else
		return aStream << "[I] NULL";
}

#endif /** INF_ENGINE_VARS_ID_HPP */
