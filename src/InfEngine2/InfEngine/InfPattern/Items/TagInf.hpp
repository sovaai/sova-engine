#ifndef __InfPatternItemsTagInf_hpp__
#define __InfPatternItemsTagInf_hpp__

#include <cstring>

#include <InfEngine2/_Include/BinaryData.hpp>

#include "Array.hpp"

namespace InfPatternItems {
	/**
	 *  Элемент кликабельного ответа.
	 */
	class TagInf: public Base {
		public:
			TagInf() { vType = itInf; }

		public:
			/** Установка данных. */
			void Set( Array * aString, Array * aRequest = nullptr ) {
				vString = aString;
				vRequest = aRequest;
			}

		public:
			unsigned int GetNeedMemorySize() const {
				unsigned int memory = sizeof( uint32_t );
				if( vString )
					memory += vString->GetNeedMemorySize();
				else {
					Array empty;
					memory += empty.GetNeedMemorySize();
				}
				binary_data_alignment( memory );
				if( vRequest )
					memory += vRequest->GetNeedMemorySize();
				else {
					Array empty;
					memory += empty.GetNeedMemorySize();
				}
				binary_data_alignment( memory );
				return memory;
			}

			unsigned int Save( void * aBuffer ) const {
				// Сохранение данных.
				void * ptr = aBuffer;

				// Смещение к сохрененному запросу.
				auto shift = binary_data_skip<uint32_t>( ptr, 1 );

				// Сохранение текста.
				if( vString )
					binary_data_save_object( ptr, *vString );
				else {
					Array empty;
					binary_data_save_object( ptr, empty );
				}

				// Выравнивание смещения.
				*shift = binary_data_alignment( ptr, aBuffer );

				if( vRequest )
					binary_data_save_object( ptr, *vRequest );
				else {
					Array empty;
					binary_data_save_object( ptr, empty );
				}
				binary_data_alignment( ptr, aBuffer );
				return static_cast<char*>( ptr ) - static_cast<char*>( aBuffer );
			}

		private:
			/** Строка, показываемая в ответе. */
			Array * vString { nullptr };

			/** Запрос, отправляемый серверу. */
			Array * vRequest { nullptr };
	};

	/**
	 *  Манипулятор для элемента кликабельного ответа.
	 */
	class TagInfManipulator {
		public:
			TagInfManipulator( const void * aBuffer ): vBuffer{ aBuffer } {}

		public:
			/** Получение запроса к движку. */
			ArrayManipulator GetString() const {
				return ArrayManipulator{ static_cast<const char*>( vBuffer ) + sizeof( uint32_t ) };
			}

			/** Получение запроса к движку. */
			ArrayManipulator GetRequest() const {
				return ArrayManipulator{ binary_data_get<const char*>( vBuffer, *binary_data_get<uint32_t>( vBuffer ) ) };
			}

		private:
			/** Данные элемента. */
			const void * vBuffer { nullptr };
	};
}

#endif /** __InfPatternItemsTagInf_hpp__ */
