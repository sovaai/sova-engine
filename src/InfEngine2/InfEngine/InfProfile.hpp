#ifndef INF_ENGINE_INFPROFILE_HPP
#define INF_ENGINE_INFPROFILE_HPP

#include <NanoLib/Cache.hpp>

#include "CoreWR.hpp"

#define INF_PROFILE_VERSION 3

/**
 *  Класс представляющий основу для профиля инфа.
 */
class InfProfile {
	public:
		virtual ~InfProfile() {}

	protected:
		/**
		 *  Конструктор профиля инфа.
		 * @param aFunctionsRegistry - реестр функций.
		 */
		InfProfile( const FunctionsRegistry & aFunctionsRegistry );


	protected:
		/** Создание основания для инфа. */
		virtual InfEngineErrors CreateFoundation();

		/** Закрытие объекта. */
		virtual void Close();


	protected:
		/**
		 *  Создание ключа для работы с кэшем. Память выделяется с помощью vRuntimeAllocator.
		 * @param aInfId - идентификатор инфа.
		 * @param aKeyLength - длина сгенерированного ключа.
		 */
		const char * CreateKey( unsigned int aInfId, size_t & aKeyLength );


	protected:
		// Состояние объекта.
		enum class State {
			Empty,              // Никакие данные не загружены.
			FoundationIsLoaded, // Загружено основание.
			ExtensionIsLoaded,  // Загружены все данные.
			ExtensionIsCreated  // Все данные созданы.
		} vState { State::Empty };

		// Реестр функций.
		const FunctionsRegistry & vFunctionsRegistry;

		// Основание профиля инфа.
		fstorage * vFoundation { nullptr };

		// Чексумма основания.
		unsigned char vFoundationChecksum[16];
};

/**
 * Структура нужная для сохранения данных в кэш.
 */
struct FStorageSection {
	fstorage_section_id vId;
	void * vData;
	size_t vDataSize;
};

/**
 *  Класс для компиляции профилей инфов.
 */
class InfProfileWR: InfProfile {
	public:
		/**
		 *  Конструктор.
		 * @param aMainBase - ссылка на основную базу.
		 * @param aTmpFilePath2 - путь к файлу для временного словарей во время компиляции.
		 * @param aMemoryLimit - максимальня память, которую можно выделять для размещения компилируемых шаблонов.
		 */
		InfProfileWR( ICoreRO & aMainCore, const char * aTmpFilePath2, unsigned int aMemoryLimit );


	public:
		/** Создание основания для инфа. */
		InfEngineErrors CreateFoundation();

		/** Сброс расширения. */
		InfEngineErrors Reset();

		/** Закрытие объекта. */
		void Close();


	public:
		/**
		 *  Компиляция профиля инфа.
		 * @param aTemplates - ссылка на шаблоны.
		 * @param aTemplatesSize - размер буфера с шаблонами.
		 * @param aSourceVars - переменные для компиляции.
		 * @param aDictsNames - список имён пользовательских словарей.
		 * @param aDicts - список пользовательских словарей.
		 * @param aDictsNumber - количество пользовательских словарей.
		 * @param aIgnoreErrors - флаг, говорящий, продолжать ли компиляцию после нахождения ошибки.
		 */
		InfEngineErrors Create( const char * aTemplates, unsigned int aTemplatesSize, avector<Vars::Raw> & aSourceVars,
								const char ** aDictsNames, const char ** aDicts, unsigned int aDictsNumber, bool aIgnoreErrors );

		/**
		 *  Сохранение профиля в кэш.
		 * @param aInfId - идентификатор инфа.
		 * @param aCache - кэш.
		 */
		InfEngineErrors Save( unsigned int aInfId, NanoLib::Cache & aCache, unsigned int & CacheInsertTm );

		/**
		 *  Зарузка и компиляция алиасов из файла.
		 */
		InfEngineErrors LoadAliasesFromFile( const char * aAliasesPath );


	protected:
		// Ядро профиля инфа.
		ExtICoreWR vExtBase;

		// Вспомогательные объекты.
		avector<FStorageSection> vSections;
};

static const unsigned int CacheKeyLength = 77;

inline const char * CreateKey( unsigned int aInfId, char aKey[CacheKeyLength], size_t & aKeyLength ) {
	memcpy( aKey, "InfProfile-", 11 );

	aKeyLength = 11 + sprintf( aKey + 11, "%u", static_cast<uint32_t>( INF_PROFILE_VERSION ) );
	aKey[aKeyLength] = '-';
	aKeyLength++;
	aKeyLength += sprintf( aKey + aKeyLength, "%u", aInfId );
	aKey[aKeyLength] = '\0';

	return aKey;
}

#endif  /* INF_ENGINE_INFPROFILE_HPP */
