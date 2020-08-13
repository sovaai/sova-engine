#ifndef INF_ENGINE_VARS_REGISTRY_HPP
#define INF_ENGINE_VARS_REGISTRY_HPP

#include <vector>

#include <NanoLib/NameIndex.hpp>
#include <NanoLib/aTextStringFunctions.hpp>

#include "../../_Include/FStorage.hpp"
#include "../../_Include/Errors.h"

#include "Id.hpp"
#include "Default.hpp"

namespace Vars
{
	/**
	 *  Тип переменной.
	 */
	enum class Type {
		/** Текстовая переменная старого образца. */
		Text     = 0,
		/** Структура данных. */
		Struct   = 1,
		/** Тип неивестен (такой тип имеют все переменные в процессе их регистрации в реестре). */
		Unknown  = 2
	};

	/**
	 *  Свойства переменной.
	 */
	enum class Options {
		/** Обозначение отсутствия опций. */
		None        = 0,

		/** Переменная только для чтения. Значение такой переменной не может быть изменено через инструкции. */
		ReadOnly    = 1,

		/** Автоматическая переменная. Значение такой переменной может быть изменено движком, а не только через пользовательские инструкции. */
		Auto        = 2,

		/** Переменные специфического типа для поддержки якорей в анкете. */
		Extended    = 4,

		/** Внутренняя опция для переменных зарегистрированных явно. */
		Explicit    = 8,

		/** Внутренняя опция для переменных из базого реестра, которые были обновлены в текущем реестре. */
		Updated     = 16,
	};

	inline Options operator & ( Options aOption1, Options aOption2 ) {
		return static_cast<Options>( static_cast<unsigned int>( aOption1 ) & static_cast<unsigned int>( aOption2 ) );
	}

	inline Options& operator &= ( Options & aOption1, Options aOption2 ) {
		return aOption1 = aOption1 & aOption2;
	}

	inline Options operator | ( Options aOption1, Options aOption2 ) {
		return static_cast<Options>( static_cast<unsigned int>( aOption1 ) | static_cast<unsigned int>( aOption2 ) );
	}

	inline Options& operator |= ( Options & aOption1, Options aOption2 ) {
		return aOption1 = aOption1 | aOption2;
	}

	/** Оператор проверки включения множества опций aOption2 в множество опций aOption1. */
	inline bool operator > ( Options aOption1, Options aOption2 ) {
		return !( ( ~static_cast<unsigned int>( aOption1 ) ) & static_cast<unsigned int>( aOption2 ) );
	}

	/** Оператор проверки включения множества опций aOption2 в множество опций aOption1. */
	inline bool operator >= ( Options aOption1, Options aOption2 ) {
		return aOption1 > aOption2;
	}

	/** Оператор проверки включения множества опций aOption1 в множество опций aOption2. */
	inline bool operator < ( Options aOption1, Options aOption2 ) {
		return aOption2 > aOption1;
	}

	/** Оператор проверки включения множества опций aOption1 в множество опций aOption2. */
	inline bool operator <= ( Options aOption1, Options aOption2 ) {
		return aOption1 < aOption2;
	}

	/** Переменная DL. Простое представление. */
	struct Raw {
		/** Имя переменной. */
		const char * vName { nullptr };

		/** Значение переменной. */
		const char * vValue { nullptr };
	};

	/**
	 *  Интерфейс реестра переменных.
	 */
	class Registry {
		public:
			virtual ~Registry() {}

		public:
			/** Получение числа зарегистрированных переменных. */
			virtual size_t GetVariablesNumber() const = 0;

			/**
			 *  Поиск зарегистрированной переменной.
			 * @param aVarName - имя переменной.
			 * @param aVarNameLength - длина имени переменной.
			 */
			virtual Id Search( const char * aVarName, size_t aVarNameLength ) const = 0;

			/**
			 * Получение имени переменной по идентификатору.
			 * @param aVarId - идентификатор переменной.
			 */
			virtual const char * GetVarNameById( Id aVarId ) const = 0;

			/**
			 * Получение имени переменной по идентификатору.
			 * @param aVarId - идентификатор переменной.
			 * @param aVarNameLength - длина имени переменной.
			 */
			virtual const char * GetVarNameById( Id aVarId, unsigned int & aVarNameLength ) const = 0;

			/**
			 *  Получение контрольной суммы всего реестра.
			 * @param aCheckSum - буфер для контрольной суммы.
			 */
			virtual void GetFullCheckSum( unsigned char aCheckSum[16] ) const noexcept = 0;

			/**
			 *  Получение контрольной суммы для основной части реестра ( переменных, которые могут быть изменены в сессии ).
			 * @param aCheckSum - буфер для контрольной суммы.
			 */
			virtual void GetMainCheckSum( unsigned char aCheckSum[16] ) const noexcept = 0;

			/**
			 *  Получение контрольной суммы для базового реестра.
			 * @param aCheckSum - буфер для контрольной суммы.
			 */
			virtual void GetBaseCheckSum( unsigned char aCheckSum[16] ) const noexcept = 0;

			/**
			 *  Получение свойств переменной.
			 * @param aVarId - идентификатор переменных.
			 */
			virtual Options GetProperties( Id aVarId ) const = 0;

			/**
			 *  Получение типа переменной по идентификатору.
			 * @param aVarId - идентификатор переменной.
			 */
			virtual Type GetVarType( Id aVarId ) const = 0;

			/**
			 *  Получение количества полей у переменной типа структура.
			 * @param aVarId - идентификатор переменной типа структура.
			 */
			virtual size_t GetStructFieldsCount( Id aVarId ) const = 0;

			/**
			 *  Получение иденитфикатора переменной, являющейся полем заданной структуры.
			 * @param aVarId - идентификатор переменной типа структура.
			 * @param aFieldN - порядковый номер поля структуры (поле default всегда стоит под номером 0).
			 */
			virtual Id GetStructFieldId( Id aVarId, unsigned int aFieldN ) const = 0;

			/**
			 *  Получение иденитфикатора переменной, являющейся значением по умолчанию заданной структуры.
			 * @param aVarId - идентификатор переменной типа структура.
			 */
			virtual Id GetStructDefaultId( Id aVarId ) const = 0;
		
		/**
		 *	Проверка существования переменной в сессии.
		 * @param aVarId - идентификатор переменной.
		 */
		bool VarExists( Id aVarId ) const { return aVarId.get() < GetVariablesNumber(); };
	};

	/**
	 *  Реестр имен переменных в режиме чтения.
	 */
	class RegistryRO : public Registry {
		public:
			/**
			 *  Открытие реестра без копирования данных в память.
			 * @param aFStorage - открытый fstorage.
			 */
			InfEngineErrors Open( fstorage * aFStorage, const RegistryRO * aBaseRegistry = nullptr ) noexcept;

			/** Закрытие открытого ранее реестра. */
			void Close() noexcept;

		public:
			/** Получение числа зарегистрированных переменных. */
			size_t GetVariablesNumber() const noexcept {
				return ( vBaseRegistry ? vBaseRegistry->GetVariablesNumber() : 0 ) + vVarNameIndex.GetNamesNumber();
			}

			/**
			 *  Поиск зарегистрированной переменной.
			 * @param aVarName - имя переменной.
			 * @param aVarNameLength - длина имени переменной.
			 */
			Id Search( const char * aVarName, size_t aVarNameLength ) const noexcept;

			/**
			 * Получение имени переменной по идентификатору.
			 * @param aVarId - идентификатор переменной.
			 */
			const char * GetVarNameById( Id aVarId ) const noexcept {
				unsigned int NameLength;
				return GetVarNameById( aVarId, NameLength );
			}

			/**
			 * Получение имени переменной по идентификатору.
			 * @param aVarId - идентификатор переменной.
			 * @param aVarNameLength - длина имени переменной.
			 */
			const char * GetVarNameById( Id aVarId, unsigned int & aVarNameLength ) const noexcept;

			/**
			 *  Получение типа переменной по идентификатору.
			 * @param aVarId - идентификатор переменной.
			 */
			Type GetVarType( Id aVarId ) const noexcept;

			/**
			 *  Получение количества полей у переменной типа структура.
			 * @param aVarId - идентификатор переменной типа структура.
			 */
			size_t GetStructFieldsCount( Id aVarId ) const noexcept;

			/**
			 *  Получение иденитфикатора переменной, являющейся полем заданной структуры.
			 * @param aVarId - идентификатор переменной типа структура.
			 * @param aFieldN - порядковый номер поля структуры (поле default всегда стоит по номером 0).
			 */
			Id GetStructFieldId( Id aVarId, unsigned int aFieldN ) const noexcept;

			/**
			 *  Получение иденитфикатора переменной, являющейся значением по умолчанию заданной структуры.
			 * @param aVarId - идентификатор переменной типа структура.
			 */
			Id GetStructDefaultId( Id aVarId ) const noexcept {
				return GetStructFieldId( aVarId, 0 );
			}

		public:
			/**
			 *  Получение контрольной суммы всего реестра.
			 * @param aCheckSum - буфер для контрольной суммы.
			 */
			void GetFullCheckSum( unsigned char aCheckSum[16] ) const noexcept;

			/**
			 *  Получение контрольной суммы для основной части реестра ( переменных, которые могут быть изменены в сессии ).
			 * @param aCheckSum - буфер для контрольной суммы.
			 */
			void GetMainCheckSum( unsigned char aCheckSum[16] ) const noexcept;

			/**
			 *  Получение контрольной суммы для базового реестра.
			 * @param aCheckSum - буфер для контрольной суммы.
			 */
			void GetBaseCheckSum( unsigned char aCheckSum[16] ) const noexcept;

		public:
			/**
			 *  Получение свойств переменной.
			 * @param aVarId - идентификатор переменных.
			 */
			Options GetProperties( Id aVarId ) const;

		private:
			bool IsMapped( Id aId ) const noexcept {
				return aId.get() < vMap[0] && GetNodeOptions( vMap, aId ) > Options::Updated;
			}

			Options GetNodeOptions( const uint32_t * aNodes, Id aId ) const noexcept {
				return static_cast<Options>( reinterpret_cast<const uint16_t*>( vBuffer + aNodes[aId.get() + 1] )[1] );
			}

			Type GetNodeType( const uint32_t * aNodes, Id aId ) const noexcept {
				return static_cast<Type>( reinterpret_cast<const uint16_t*>( vBuffer + aNodes[aId.get() + 1] )[0] );
			}

			size_t GetNodeChildsNumber( const uint32_t * aNodes, Id aId ) const noexcept {
				return reinterpret_cast<const uint32_t*>( vBuffer + aNodes[aId.get() + 1] )[1];
			}

			Id GetNodeChildId( const uint32_t * aNodes, Id aId, size_t i ) const noexcept {
				return { reinterpret_cast<const uint32_t*>( vBuffer + aNodes[aId.get() + 1] )[2 + i], false };
			}

			// Проверка идентификатора переменной на корректность.
			bool ValidateId( Id aId ) const {
				return aId && aId.get() < GetVariablesNumber() && vVarNameIndex.GetState() == NanoLib::NameIndex::stReadOnly;
			}

		private:
			// Индекс имен переменных.
			NanoLib::NameIndex vVarNameIndex;

			// Список новых переменных.
			const uint32_t * vNodes = nullptr;

			// Список измененных переменных базового реестра.
			const uint32_t * vMap { nullptr };

			// Буфер с сохраненным реестром.
			const char * vBuffer { nullptr };

			// Указатель на базовый реестр.
			const RegistryRO * vBaseRegistry { nullptr };
	};

	/**
	 *  Реестр имен переменных в режиме чтение-запись.
	 */
	class RegistryRW : public Registry {
		public:
			/** Идентификатор переменной. */
			using Id = Vars::Id;

		public:
			/**
			 *  Создание реестра в режиме RW.
			 * @param aBaseRegistry - базовый реестр переменных.
			 */
			InfEngineErrors Create( const RegistryRO * aBaseRegistry = nullptr ) noexcept;

			/**
			 *  Сохранение реестра в fstorage.
			 * @param aFStorage - открытый fstorage.
			 */
			InfEngineErrors Save( fstorage * aFStorage ) const noexcept;

			/** Закрытие открытого ранее реестра. */
			void Close() noexcept;

		public:
			/** Получение числа зарегистрированных переменных. */
			size_t GetVariablesNumber() const noexcept {
				return ( vBaseRegistry ? vBaseRegistry->GetVariablesNumber() : 0 ) + vVarNameIndex.GetNamesNumber();
			}

			/**
			 *  Регистрация переменной в реестре.
			 * @param aVarName - имя переменной.
			 * @param aVarNameLength - длина имени переменной.
			 * @param aVarId - идентификатор зарегистрированной переменной.
			 * @param aUniq - флаг, показывающий, что должен быть возвращён код INF_ENGINE_WARN_UNSUCCESS, если переменная уже зарегистрирована.
			 */
			InfEngineErrors Registrate( const char * aVarName, unsigned int aVarNameLength, Id & aVarId, bool aUniq = false ) noexcept;

		public:
			/**
			 *  Поиск зарегистрированной переменной.
			 * @param aVarName - имя переменной.
			 * @param aVarNameLength - длина имени переменной.
			 */
			Id Search( const char * aVarName, size_t aVarNameLength ) const noexcept;

			/**
			 * Получение имени переменной по идентификатору.
			 * @param aVarId - идентификатор переменной.
			 */
			const char * GetVarNameById( Id aVarId ) const noexcept {
				unsigned int NameLength;
				return GetVarNameById( aVarId, NameLength );
			}

			/**
			 * Получение имени переменной по идентификатору.
			 * @param aVarId - идентификатор переменной.
			 * @param aVarNameLength - длина имени переменной.
			 */
			const char * GetVarNameById( Id aVarId, unsigned int & aVarNameLength ) const noexcept;

		public:
			/**
			 *  Получение контрольной суммы всего реестра.
			 * @param aCheckSum - буфер для контрольной суммы.
			 */
			void GetFullCheckSum( unsigned char aCheckSum[16] ) const noexcept;

			/**
			 *  Получение контрольной суммы для основной части реестра ( переменных, которые могут быть изменены в сессии ).
			 * @param aCheckSum - буфер для контрольной суммы.
			 */
			void GetMainCheckSum( unsigned char aCheckSum[16] ) const noexcept;

			/**
			 *  Получение контрольной суммы для базового реестра.
			 * @param aCheckSum - буфер для контрольной суммы.
			 */
			void GetBaseCheckSum( unsigned char aCheckSum[16] ) const noexcept;

		public:
			/**
			 *  Получение свойств переменной.
			 * @param aVarId - идентификатор переменных.
			 */
			Options GetProperties( Id aVarId ) const noexcept;

			/**
			 *  Получение типа переменной по её идентификатору.
			 * @param aVarId - идентификатор переменной.
			 */
			Type GetVarType( Id aVarId ) const noexcept;

			/**
			 *  Получение количества полей у переменной типа структура.
			 * @param aVarId - идентификатор переменной типа структура.
			 */
			size_t GetStructFieldsCount( Id aVarId ) const noexcept;

			/**
			 *  Получение идентификатора переменной, являющейся полем заданной структуры.
			 * @param aVarId - идентификатор переменной типа структура.
			 * @param aFieldN - порядковый номер поля структуры (поле default всегда стоит под номером 0).
			 */
			Id GetStructFieldId( Id aVarId, unsigned int aFieldN ) const noexcept;

			/**
			 *  Получение иденитфикатора переменной, являющейся значением по умолчанию заданной структуры.
			 * @param aVarId - идентификатор переменной типа структура.
			 */
			Id GetStructDefaultId( Id aVarId ) const noexcept {
				return GetStructFieldId( aVarId, 0 );
			}

	private:
		// Узел дерева, описывающий переменную.
		class Node
		{
		public:
			// Конструктор по умолчанию.
			Node() noexcept = default;

			// Конструктор с установкой идентификатора.
			Node( Id aId ) noexcept : vId( aId ) {}

		public:
			// Конструктор копирования. ( УДАЛЕН )
			Node( const Node & aNode ) = delete;

			// Конструктор переноса.
			Node( Node && aNode ) noexcept = default;

		public:
			// Сравнение идентификатора узла с другим идентификатором.
			bool operator<( const Vars::Id & aVarId ) const noexcept { return vId < aVarId; }

		public:
			// Оператор копирования. ( УДАЛЕН )
			Node& operator=( const Node & aNode ) = delete;

			// Оператор переноса.
			Node& operator=( Node && aNode ) noexcept = default;

		public:
			// Проверка на корректность идентификатора узла.
			operator bool() const noexcept { return vId.valid(); }

		public:
			// Получение контрольной суммы узла.
			void GetCheckSum( unsigned char aCheckSum[16] ) const noexcept;

		public:
			// Идентификатор переменной.
			Id vId;

			// Идентификатор переменной того же уровня.
			Id vSibling;

			// Идентификатор переменной потомка.
			Id vChild;

			// Тип переменной.
			Type vType { Type::Text };

			// Свойства переменной.
			Options vProperties { Options::None };

			// Число потомков узла.
			size_t vChildsNumber { 0 };
		};

		private:
			// Установка свойств для переменной.
			void SetProperties( Id aVarId, Options aProperties ) noexcept;

		// Добавление свойства для переменной.
		void AddProperties( Id aVarId, Options aProperties ) noexcept;

		// Установка изначальных свойств, основанных на имени переменной.
		void InitProperties( Id aVarId, const char * aVarName, size_t aVarNameLength ) noexcept;

		// Получение узла, соответствующего указанному id.
		const Node * GetNode( Id aVarId ) const noexcept;

		// Получение узла, соответствующего указанному id.
		Node * GetNode( Id aVarId ) noexcept;

		// Поиск переменной с предварительно нормализованным именем.
		Id SearchNormalized( const char * aVarName, size_t aVarNameLength ) const noexcept;

		// Нормализация имени переменной.
		std::string & NormalizeVarName( std::string & aVarName ) const noexcept;

		// Проверка, есть ли еще возможность зарегистрировать новую переменную.
		bool IsFull() const noexcept { return GetVariablesNumber() > Id::max().get(); }

		// Проверка имени переменной.
		bool ValidateName( const char * aVarName, unsigned int aVarNameLength ) const noexcept;

		// Добавление переменной в реестр.
		InfEngineErrors InsertNode( const char * aVarName, unsigned int aVarNameLength, Id & aVarId ) noexcept;

		// Обновление переменной из базового реестра.
		InfEngineErrors MapNode( const Id & aVarId ) noexcept;

	private:
		// Индекс имен переменных.
		NanoLib::NameIndex vVarNameIndex;

		// Узлы дерева, соответствующие переменным.
		std::vector<Node> vNodes;

		// Измененные узлы дерева, соответствующие переменным базового реестра.
		std::vector<Node> vMap;

		// Ссылка на базовый реестр переменных.
		const RegistryRO * vBaseRegistry { nullptr };
	};
}

#endif /** INF_ENGINE_VARS_REGISTRY_HPP */
