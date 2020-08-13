#ifndef __InfPatternItemsDictRef_hpp__
#define __InfPatternItemsDictRef_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Ссылка на словарь в шаблон-вопросе по его порядковому номеру.
	 */
	class DictRef: public Base
	{
	public:
		/** Тип ссылки. */
		enum RefType {
			RT_NAME = 0,       // Подставлять вместо тега имя словаря.
			RT_VALUE = 1,      // Подставлять вместо тега сработавшую строку словаря.
			RT_ORIGIN = 2,     // Подставлять вместо тега оригинальный ввод пользователя, сопоставленный со словарём.
			RT_POS = 3,        // Подставлять вместо тега номер сработавшей строки словаря.
			RT_NORM = 4,       // Подставлять вместо тега сработавшую строку словаря с применением замены на главную строку.
		};

		/** Конструктор. */
		DictRef()
			{ vType = itDictRef; }

	public:
		/**
		 *  Установка данных.
		 * @param aRefNum - номер выделенного фрагмента поискового шаблона, на который указывает ссылка.
		 */
		void Set( unsigned int aRefNum, RefType aRefType )
		{
			vRefNum = aRefNum;
			vRefType = aRefType;
		}

	public:
		unsigned int GetNeedMemorySize() const
			{ return sizeof( uint32_t ) + sizeof( uint8_t ); }

		unsigned int Save( void * aBuffer ) const
		{
			// Сохранение номера ссыкли.
			char * ptr = static_cast<char*>( aBuffer );
			*((uint32_t*)ptr) = vRefNum;
			ptr += sizeof( uint32_t );

			// Сохранение типа ссыкли.
			*((uint8_t*)ptr) = vRefType;
			ptr += sizeof( uint8_t );

			return ptr - static_cast<char*>( aBuffer );
		}


	protected:
		/** Номер ссылки. */
		unsigned int vRefNum = 0;

		/** Тип ссылки. */
		RefType vRefType = RT_NAME;
	};

	/**
	 *  Манипулятор для ссылка на словарь в шаблон-вопросе по его порядковому номеру.
	 */
	class DictRefManipulator
	{
	public:
		/** Конструктор. */
		DictRefManipulator():
			vBuffer( nullptr ) {}

		/**
		 *  Конструктор.
		 * @param aBuffer - буффер, содержащий данные элемента.
		 */
		DictRefManipulator( const void * aBuffer ):
			vBuffer( static_cast<const char *>(aBuffer) ) {}

	public:
		/**
		 *  Инициализация.
		 * @param aBuffer - ссылка на буффер, содержащий сохраненный элемент.
		 */
		void Init( const void * aBuffer )
			{ vBuffer = static_cast<const char *>(aBuffer); }

	public:
		/** Получение номера ссылки. */
		unsigned int GetRefNum() const
			{ return *((uint32_t*)vBuffer); }

		/** Получение типа ссылки. */
		DictRef::RefType GetRefType() const
			{ return (DictRef::RefType)*( (uint8_t*)(vBuffer + sizeof(uint32_t)) ); }

	protected:
		/** Данные элемента. */
		const char * vBuffer;
	};
}

#endif  /* __InfPatternItemsDictRef_hpp__ */
