#ifndef __FStorage_h__
#define __FStorage_h__

#include <lib/fstorage/fstorage.h>

/**
 *  Идентификаторы секций для сохранения данных в fstorage.
 */

/** Версия карты секций fstorage. */
#define FSTORAGE_SECTIONS_MAP_VERSION            "3"

/**
 *  Идентификатор проекта для разделения секции fstorage.
 */

    /** Обычный InfEngine. */
    #define FSTORAGE_PID_INF_ENGINE (100U)

    /** Эллипсисы. */
    #define FSTORAGE_PID_ELLIPSIS   (101U)



/** Идентификатор секции для данных шаблонов. */
#define FSTORAGE_SECTION_DLDATA						(0x90000200U)
#define FSTORAGE_SECTION_DLDATA_PATTERNS_STORAGE	(0x90000201U)
#define FSTORAGE_SECTION_DLDATA_EVENTS_BINDINGS		(0x90000202U)

/** Идентификатор секции для реестра переменных. */
#define FSTORAGE_SECTION_VARS_REGISTRY           (0x90000300U)

/** Идентификатор секции для реестра InfPerosn'ов. */
#define FSTORAGE_SECTION_INF_PERSON_REGISTRY     (0x90000400U)

/** Идентификатор секции для хранения основных данных реестра внешних баз данных. */
#define FSTORAGE_SECTION_EXT_SERVICES_DB         (0x90000501U)

/** Идентификатор секции реестра условий. */
#define FSTORAGE_SECTION_CONDITIONS_REGISTRY     (0x90000600U)

/** Идентификатор секции для реестра функций. */
#define FSTORAGE_SECTION_FUNCTIONS_REGISTRY      (0x90000700U)

/** Идентификатор секции для реестра тэгов-функций. */
#define FSTORAGE_SECTION_FUNCTIONS_TAG_REGISTRY  (0x90000800U)

/** Идентификаторы секций для данных шаблонов. */
#define FSTORAGE_SECTION_INDEXER_INF_DICT        (0x90100100U)
#define FSTORAGE_SECTION_INDEXER_INF_QST         (0x90100200U)
#define FSTORAGE_SECTION_INDEXER_INF_THAT        (0x90100300U)
#define FSTORAGE_SECTION_INDEXER_INF_SYNONYMS    (0x90100400U)
#define FSTORAGE_SECTION_INDEXER_INF_SYMBOLYMS   (0x90100500U)
#define FSTORAGE_PID_SYMBOLYMS_TMP FSTORAGE_SECTION_INDEXER_INF_SYMBOLYMS
#define FSTORAGE_PID_SYNONYMS_TMP FSTORAGE_SECTION_INDEXER_INF_SYNONYMS

/** Идентификаторы секций для данных эллипсисов. */
#define FSTORAGE_SECTION_INDEXER_ELLIPSIS_DICT   (0x90200100U)
#define FSTORAGE_SECTION_INDEXER_ELLIPSIS_FIRST  (0x90200200U)
#define FSTORAGE_SECTION_INDEXER_ELLIPSIS_SECOND (0x90200300U)
#define FSTORAGE_SECTION_INDEXER_ELLIPSIS_THIRD  (0x90200400U)

/** Идентификатор секции для сигнатуры лингвистических данных. */
#define FSTORAGE_SECTION_LD_SIGNATURE            (0x90400100U)

#endif /** __FStorage_h__ */
