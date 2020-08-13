#ifndef __NameIndex_h__
#define __NameIndex_h__

#include <_include/_inttype.h>
#include <lib/aptl/avector.h>
#include <lib/md5a/md5a.h>

#include "nMemoryAllocator.hpp"

namespace NanoLib
{
	/**
	 * Система индексирования имен( текстовых строк ).
	 *
	 *  Система индексирования имен предназначена для сопоставления текстовым строкам числовых идентификаторов.
	 * \n\n
	 *  Стоит заметить, что индекс может быть сохранен в буффер, работать на основании ранее сохраненного буфера в
	 * режиме поиска или же быть загруженым из ранее сохраненного буффера с возможностью добавления новых элементов.
	 * \n\n
	 *  Система индексирования гарантирует, что идентификаторы присваиваются именам в возрастающем порядке, начиная с 0,
	 * и без пропусков значений. Соответственно, можно утверждать, что максимальным значением идентификатора в
	 * индексе будет #GetNamesNumber()-1.
	 * \n\n
	 *  В индексе есть ограничение на количество элементов: @f$2^{32}-1@f$
	 */
	class NameIndex
	{
	public:
		/**
		 *  Конструктор. Параметры конструктора определяют размер выделяемой памяти.
		 * @param vAverageNameLength - предполагаемая средняя длина имен.
		 * @param vBaseNamesNumber - число имен, на которое нужно выделить память при инициализации индекса.
		 * @param vStepNamesNumber - число имен, на которое нужно выделять память при необходимости дополнительного
		 *                          выделения памяти.
		 */
		NameIndex( unsigned int vAverageNameLength = 32, unsigned int vBaseNamesNumber = 100, unsigned int vStepNamesNumber = 10 );


	public:
		/** Коды возврата функций-членов класса. */
		typedef enum
		{
			/** Успешное завершение. */
			rcSuccess             =  0,

			/** Элемент уже существует в индексе. */
			rcElementExists       =  1,

			/** Некорректные аргументы функции. */
			rcErrorInvArgs        = -1,
			/** Неполучается выделить память. */
			rcErrorNoFreeMemory   = -2,
			/** В буффере недостаточно места для сохранения идекса. */
			rcErrorBufferTooSmall = -3,
			/** Внутренняя ошибка. */
			rcErrorFault          = -4,
			/** Некорректный статус индекса. */
			rcErrorInvState      = -5,
			/** Достигнут предел количества элементов в индексе. */
			rcErrorLimitIsReached = -6,
			/** Некорректные данные. */
			rcErrorInvData        = -7,
		} ReturnCode;

		/**
			 * Получение текстового описания кода возврата.
			 * @param aReturnCode - код возврата.
			 */
		const char * ReturnCodeToString( NameIndex::ReturnCode aReturnCode ) const;


	public:
		/** Открытие нового индекса в RW режиме. */
		void Create();

		/**
		 *  Открытие, ранее сохраненного индекса, в RO режиме без копирования данных. Данные используются напрямую
		 * из указанного буффера. Соответственно, изменение в буффере может привести к фатальному нарушению работы
		 * индекса.
		 * @param aBuffer - буффер с сохраненным индексом.
		 * @param aBufferSize - размер буфера.
		 */
		NameIndex::ReturnCode Open( const void * aBuffer, unsigned int aBufferSize );

		/**
		 *  Открытие ранее сохраненного индекса в WR режиме с копированием данных в память.В отличии от #Open данная
		 * функция полностью копирует все данные из буффера в память, поэтому дальнейшие изменнения данных в буфере
		 * никаким образом не повлияют на работу индекса.
		 * @param aBuffer - буффер с сохраненным индексом.
		 * @param aBufferSize - размер буфера.
		 */
		NameIndex::ReturnCode Load( const void * aBuffer, unsigned int aBufferSize );

		/**
		 *  Получение MD5 чексуммы для текущего индекса.
		 * @param aDigest - MD5 чексумма.
		 */
		void GetMD5Digest( unsigned char aDigest[16] ) const
			{ memcpy( aDigest, vMD5Digest, 16 ); }


	public:
		/** Очистка индекса. */
		void Reset();

		/** Уничтожение индекса и освобождение всей занятой памяти. */
		void Destroy();


	public:
		/** Получение размера памяти, необходимого для сохранения индекса. */
		unsigned int GetNeedMemorySize() const
			{ return vNeedMemorySize; }

		/**
		 *  Сохранение индекса в буффер.
		 * @param aBuffer - буффер, в который сохранять объект.
		 * @param aBufferSize - размер буффера.
		 */
		unsigned int Save( void * aBuffer ) const;


	public:
		/**
		 *  Добавление имени в индекс. Имена длинее чем 2^24-1 не могут быть добавлены в индекс.
		 * @param aName - имя, которое нужно добавить в индекс.
		 * @param aNameLength - длина имени.
		 * @param aId - идентификатор имени.
		 * @retval Если имя уже присутствует в индексе, то переменной aId присваивается идентификатор имени и
		 *        возвращается код #ElementExists.
		 */
		NameIndex::ReturnCode AddName( const char * aName, unsigned int aNameLength, unsigned int & aId );
		/**
		 *  Добавление имени в индекс. Имена длинее чем 2^24-1 не могут быть добавлены в индекс.
		 * @param aName - имя, которое нужно добавить в индекс.
		 * @param aId - идентификатор имени.
		 * @retval Если имя уже присутствует в индексе, то переменной aId присваивается идентификатор имени и
		 *        возвращается код #ElementExists.
		 */
		NameIndex::ReturnCode AddName( const char * aName, unsigned int& aId )
			{ return AddName( aName, aName ? strlen(aName) : 0, aId ); }


	public:
		/**
		 *  Поиск имени в индексе.
		 * @param aName - имя, которое нужно добавить в индекс.
		 * @param aNameLength - длина имени.
		 */
		const unsigned int * Search( const char * aName, unsigned int aNameLength ) const;
		/**
		 *  Поиск имени в индексе.
		 * @param aName - имя, которое нужно добавить в индекс.
		 */
		const unsigned int * Search( const char * aName ) const
			{ return Search( aName, aName ? strlen(aName) : 0 ); }

		/**
		 *  Получение имени по идентификатору.
		 * @param aId - идентификатор имени.
		 * @param aNameLength - длина имени.
		 */
		const char * GetName( unsigned int aId, unsigned int & aNameLength ) const;

		/**
		 *  Получение имени по идентификатору.
		 * @param aId - идентификатор имени.
		 */
		const char * GetName( unsigned int aId ) const
			{ unsigned int NameLength; return GetName( aId, NameLength ); }

		/** Получение числа заиндексированных имен. */
		unsigned int GetNamesNumber() const
			{ return vNextId; }


	public:
		/** Получение размера выделенной памяти. */
		unsigned int GetAllocatedMemorySize() const
			{ return vAllocator.GetAllocatedMemorySize() + vRWIndex.capacity()*sizeof(niName*) + vRWReverseIndex.capacity()*sizeof(niName*); }


	private:
		/**
		 *  Вычисление хэш функции имени.
		 * @param aName - имя.
		 * @param aNameLength - длина имени.
		 */
		unsigned int Hash( const char * aName, unsigned int aNameLength ) const;


	private:
		/** Структура описывающая имя. */
		typedef struct
		{
			/** Текст имени. */
			const char * vName;
			/** Длина текста имени. */
			unsigned int vNameLength;
			/** Идентификатор имени. */
			unsigned int vId;
			/** Хэш значение имени. */
			unsigned int vHash;
		} niName;


	private:
		/**
		 *  Бинарный поиск имени в RW режиме.
		 * @param aName - имя.
		 * @param aNameLength - длина имени.
		 * @param aExists - флаг, показывающий существует элемент или нет.
		 * @param aHash - сюда помещается хэш код имени, вычисленный в ходе поиска.
		 * @retval Если элемент найден, функция возвращает его позицию, если не найден, то возвращает позицию, на
		 *        которую его нужно добавить.
		 */
		unsigned int SearchRW( const char * aName, unsigned int aNameLength, bool & aExists, unsigned int & aHash ) const;

		/**
		 *  Бинарный поиск имени в RO режиме.
		 * @param aName - имя.
		 * @param aNameLength - длина имени.
		 * @param aExists - флаг, показывающий существует элемент или нет.
		 * @param aHash - сюда помещается хэш код имени, вычисленный в ходе поиска.
		 * @retval Если элемент найден, функция возвращает его позицию, если не найден, то возвращает позицию, на
		 *        которую его нужно добавить.
		 */
		unsigned int SearchRO( const char * aName, unsigned int aNameLength, bool & aExists, unsigned int & aHash ) const;


	private:
		/** Получение числа элементов в RO индексе. */
		unsigned int ROIndexSize() const
			{ return *reinterpret_cast<const uint32_t*>(vBuffer+sizeof(uint32_t)+64+6*sizeof(UINT4)); }
		/** Получение значения следующего доступного индекса. */
		unsigned int ROGetNextID()
			{ return *reinterpret_cast<const uint32_t*>(vBuffer+2*sizeof(uint32_t)+64+6*sizeof(UINT4)); }
		/** Получение указателя на данные прямого индекса. */
		const void * GetFIndex() const
			{ return vBuffer+4*sizeof(uint32_t)+64+6*sizeof(UINT4); }
		/** Получение указателя на данные обратного индекса. */
		const void * GetRIndex() const
			{ return vBuffer+*reinterpret_cast<const uint32_t*>(vBuffer+3*sizeof(uint32_t)+64+6*sizeof(UINT4));}
		/** Получение указателя на данные элемента с позицией aPos в прямом индексе. */
		const void * GetElementByPos( unsigned int aPos ) const
			{ return vBuffer+((uint32_t*)GetFIndex())[2*aPos+1]; }
		/** Получение хэш кода элемента с позицией aPos в прямом индексе. */
		unsigned int ROHashByPos( unsigned int aPos ) const
			{ return ((uint32_t*)GetFIndex())[2*aPos]; }
		/** Получение идентификатора элемента с позицией aPos в прямом индексе. */
		unsigned int * ROIDByPos( unsigned int aPos ) const
			{ return (uint32_t*)GetElementByPos( aPos ); }
		/** Получение имени элемента с позицией aPos в прямом индексе. */
		const char * RONameByPos( unsigned int aPos ) const
			{ return (const char *)GetElementByPos( aPos )+2*sizeof(uint32_t); }
		/** Получение длины имени элемента с позицией aPos в прямом индексе. */
		unsigned int * RONameLengthByPos( unsigned int aPos ) const
			{ return (uint32_t*)GetElementByPos( aPos )+1; }
		/** Получение указателя на данные элемента с идентификатором aId. */
		const void* GetElementByID( unsigned int aId ) const
		{ return vBuffer+((uint32_t*)GetRIndex())[aId]; }
		/** Получение имени элемента с идентификатором aId. */
		const char * RONameByID( unsigned int aId ) const
			{ return (const char *)GetElementByID( aId )+2*sizeof(uint32_t); }
		/** Получение длины имени элемента с идентификатором aId. */
		unsigned int * RONameLengthByID( unsigned int aId ) const
			{ return (uint32_t*)GetElementByID( aId )+1; }


	public:
		/** Структура описывающая статус объекта. */
		typedef enum
		{
			/** Объект не создан или уничтожен. */
			stClosed    = 0,
			/** Объект открыт в режиме readony. */
			stReadOnly  = 1,
			/** Объект открыт в режиме readwrite. */
			stReadWrite = 2
		} niState;

		/** Получение статуса объекта. */
		niState GetState() const
			{ return vState; }


	private:
		/** Статус объекта. */
		niState vState = stClosed;

		/** Аллокатор внутренней памяти. */
		nMemoryAllocator vAllocator;

		/** Хэш индекс в RW режиме. */
		avector<niName*> vRWIndex;

		/** Обратный индекс в RW режиме. */
		avector<niName*> vRWReverseIndex;

		/** Идентификатор для следующего имени. */
		unsigned int vNextId = 0;

		/** Размер памяти необходимой для сохранения индекса. */
		unsigned int vNeedMemorySize = 0;

		/** Ссылка на загруженный буффер в RO режиме. */
		const char * vBuffer = nullptr;

		/** Текущее MD5 состояние. */
		MD5_CTX vMD5State;

		/** MD5 чексумма. */
		unsigned char vMD5Digest[16];
	};
};

#endif  /** __NameIndex_h__ */
