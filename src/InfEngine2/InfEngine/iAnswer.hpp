#ifndef __iAnswer_hpp__
#define __iAnswer_hpp__

#include <NanoLib/aTextStringFunctions.hpp>
#include <NanoLib/nMemoryAllocator.hpp>

#include <InfEngine2/_Include/Errors.h>

#include "Vars/Id.hpp"

/**
 *  Класс представляющий ответ инфа на реплику пользователя.
 */

/**
 * @todo Нужно сделать общий интерфейс для флагов ответа.
 */
class iAnswer
{
public:
	/**
	 *  Констуктор.
	 * @param aFullCopyMode - флаг, показывающий, что нужно копировать все данные, не учитывая флаги aForceCopy.
	 */
	iAnswer( bool aFullCopyMode )
	{ vFullCopyMode = aFullCopyMode; }

	/**
	 *  Конструктор.
	 * @param aAllocator - аллокатор памяти, который будет использоваться для коипрования данных.
	 * @param aFullCopyMode - флаг, показывающий, что нужно копировать все данные, не учитывая флаги aForceCopy.
	 */
	iAnswer( nMemoryAllocator & aAllocator, bool aFullCopyMode )
	{
		vAllocatorPtr = &aAllocator;
		vFullCopyMode = aFullCopyMode;
	}

public:
	/** Элемент ответа. */
	struct Item
	{
		/** Тип элемента. */
		enum class Type
		{
			None		= 0,/** Пустой элемент. */
			Text		= 1,/** Текстовый элемент. */
			Br			= 2,/** Перенос строки. */
			Href		= 3,/** Ссылка. */
			Inf			= 4,/** Кликабельный ответ. */
			Instruct	= 5,/** Инструкция для изменения сессии. */
			OpenWindow	= 6,/** Открытие ссылки в новом окне. */
			RSS			= 7,/** Запрос RSS. */
			Space		= 8,/** Последовательность пробельных символов. */
			StartUList	= 9, /** Начало неупорядоченного форматированного списка. */
			StartOList	= 10, /** Начало упорядоченного форматированного списка. */
			ListItem	= 11, /** Элемент форматированного списка. */
			EndList		= 12, /** Завершение форматрированного списка. */
		} vType;

		/** Данные элемента. */
		union
		{
			/** Текстовая строка. */
			struct
			{
				/** Значение. */
				const char * vValue;
				/** Длина значения. */
				unsigned int vLength;
			} vText;
			/** Ссылка. */
			struct
			{
				/** URL. */
				const char * vURL;
				/** Длина URL. */
				unsigned int vURLLength;
				/** Текст ссылки. */
				const char * vLink;
				/** Длина текста ссылки. */
				unsigned int vLinkLength;
				/** Target ссылки. */
				const char * vTarget;
				/** Длина target'а. */
				unsigned int vTargetLength;
			} vHref;
			/** Кликабельный вопрос инфу. */
			struct
			{
				const char * vValue;
				unsigned int vValueLength;
				const char * vRequest;
				unsigned int vRequestLength;
			} vInf;
			/** Инструкция по изменению сессии. */
			struct
			{
				Vars::Id vVarId = {};
				const char * vValue = nullptr;
				unsigned int vValueLength = 0;
			} vInstruct = {};
			/** Инструкция открытия ссылки в новом окне. */
			struct
			{
				const char * vURL;
				unsigned int vURLLength;
				unsigned int vTarget;
			} vOpenWindow;
			/** Инструкция на запрос к RSS. */
			struct
			{
				/** Значение урла RSS'а. */
				const char * vURL;
				/** Длина значения урла RSS'а. */
				unsigned int vURLLength;
				/** Текст, показываемый при недоступности RSS. */
				const char * vAlt;
				/** Длина текста, показываемого при недоступности RSS. */
				unsigned int vAltLength;
				/** Номер RSS записи. */
				unsigned int vOffset;
				/** Флаг показа заголовка RSS. */
				bool vShowTitle;
				/** Флаг показа ссылки на RSS. */
				bool vShowLink;
				/** Частота обновления RSS. */
				unsigned int vUpdatePeriod;
			} vRSS;
		} vItem;

		/** Флаг, показывающий, что элемент является приоритетным. */
		bool vPriorityFlag = false;
	};

public:
	/**
	 *  Сброс ответа. В случае, если производилось копирование данных, с помощью внутреннего аллокатора, то память
	 * будет сброшена и подготовлена для повторного использования. В случае использования внешнего аллокатора,
	 * память не освобождается.
	 */
	void Reset()
	{
		vInternalAllocator.Reset();
		vItems.clear();
		vPatternId = 0;
		vAnswerId = 0;
		vForceNotEmpty = false;
		vDisableAutoVars = false;
	}

public:
	/**
	 *  Добавление текстового элемента.
	 * @param aText - текст.
	 * @param aTextLength - длина текста.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddText( const char * aText, unsigned int aTextLength, bool aForceCopy );

	/**
	 *  Добавление текстового элемента.
	 * @param aText - текст.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddNullTermText( const char * aText, bool aForceCopy )
	{ return AddText( aText, aText ? strlen( aText ) : 0, aForceCopy ); }

	/**
	 *  Добавление пробельного элемента.
	 * @param aSpaces - строка с пробельными элементами.
	 * @param aSpacesLength - длина строки с пробельными элементами.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddSpace( const char * aSpaces, unsigned int aSpacesLength, bool aForceCopy );

	/**
	 *  Добавление ссылки.
	 * @param aLink - текст ссылки.
	 * @param aLinkLength - длина ссылки.
	 * @param aTarget - текст параметра target.
	 * @param aTargetLength - длина параметра target.
	 * @param aURL - адрес ссылки.
	 * @param aURLLength - длина адреса ссылки.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddHref( const char * aLink, unsigned int aLinkLength,
			const char * aTarget, unsigned int aTargetLength,
			const char * aURL, unsigned int aURLLength, bool aForceCopy );

	/**
	 *  Добавление ссылки.
	 * @param aLink - текст ссылки.
	 * @param aTarget - параметр target.
	 * @param aURL - адрес ссылки.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddNullTermHref( const char * aLink, const char * aTarget, const char * aURL, bool aForceCopy )
	{ return AddHref( aLink, aLink ? strlen( aLink ) : 0, aTarget, aTarget ? strlen( aTarget ) : 0, aURL, aURL ? strlen( aURL ) : 0, aForceCopy ); }

	/**
	 *  Добавление инструкции на изменение сессии.
	 * @param aVarId - идентификатор переменной.
	 * @param aValue - значение.
	 * @param aValueLength - длина значения.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddInstruct( Vars::Id aVarId, const char * aValue, unsigned int aValueLength, bool aForceCopy );

	/**
	 *  Добавление инструкции на изменение сессии.
	 * @param aVarId - идентификатор переменной.
	 * @param aValue - значение.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddNullTermInstruct( Vars::Id aVarId, const char * aValue, bool aForceCopy )
	{ return AddInstruct( aVarId, aValue, aValue ? strlen( aValue ) : 0, aForceCopy ); }

	/** Добавление переноса строки. */
	InfEngineErrors AddBr();

	/** Добавление тэга начала форматированного списка. */
	InfEngineErrors StartList( bool aOrdered );

	/** Добавление тэга начала элемента форматрированного списка. */
	InfEngineErrors AddListItem();

	/** Добавление тэга завершения форматрированного списка. */
	InfEngineErrors FinishList();

	/**
	 *  Добавление кликабельного ответа.
	 * @param aText - текст ответа.
	 * @param aTextLength - длина текста ответа.
	 * @param aRequest - запрос, передаваемый по клику.
	 * @param aRequestLength - длина запроса, передаваемого по клику.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddInf( const char * aText, unsigned int aTextLength, const char * aRequest, unsigned int aRequestLength, bool aForceCopy );

	/**
	 *  Добавление кликабельного ответа.
	 * @param aText - текст ответа.
	 * @param aRequest - запрос, отправляемый по клику.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddNullTermInf( const char * aText, const char * aRequest, bool aForceCopy )
	{ return AddInf( aText, aText ? strlen( aText ) : 0, aRequest, aRequest ? strlen( aRequest ) : 0, aForceCopy ); }

	/**
	 *  Добавление команды на открытие ссылки в новом или текущем окне.
	 * @param aURL - адрес ссылки.
	 * @param aURLLength - длина адреса ссылки.
	 * @param aTarget - имя целевого окна.
	 * @param aForceCopy - флаг принудительного копирования.
	 * @return
	 */
	InfEngineErrors AddOpenWindow( const char * aURL, unsigned int aURLLength, unsigned int aTarget, bool aForceCopy );

	/**
	 *  Добавление команды на открытие ссылки в новом или текущем окне.
	 * @param aURL - адрес ссылки.
	 * @param aTarget - имя целевого окна.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddNullTermOpenWindow( const char * aURL, unsigned int aTarget, bool aForceCopy )
	{ return AddOpenWindow( aURL, aURL ? strlen( aURL ) : 0, aTarget, aForceCopy ); }

	/**
	 *  Добавление команды RSS.
	 * @param aURL - ссылка на RSS ленту.
	 * @param aURLLength - длина ссылки на RSS ленту.
	 * @param aAlt - текст подсказки.
	 * @param aAltLength - длина текста подсказки.
	 * @param aOffset - смещение в ленте.
	 * @param aShowTitle - флаг показа заголовка ленты.
	 * @param aShowLink - флаг показа ссылки на ленту.
	 * @param aUpdatePeriod - указание периода обновления ленты.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddRSS( const char * aURL, unsigned int aURLLength, const char * aAlt, unsigned int aAltLength,
			unsigned int aOffset, bool aShowTitle, bool aShowLink, unsigned int aUpdatePeriod, bool aForceCopy );

	/**
	 *  Добавление команды RSS.
	 * @param aURL - ссылка на RSS ленту.
	 * @param aAlt - текст подсказки.
	 * @param aOffset - смещение в ленте.
	 * @param aShowTitle - флаг показа заголовка ленты.
	 * @param aShowLink - флаг показа ссылки на ленту.
	 * @param aUpdatePeriod - указание периода обновлени ленты.
	 * @param aForceCopy - флаг принудительного копирования.
	 */
	InfEngineErrors AddNullTermRSS( const char * aURL, const char * aAlt, unsigned int aOffset, bool aShowTitle, bool aShowLink, unsigned int aUpdatePeriod, bool aForceCopy )
	{ return AddRSS( aURL, aURL ? strlen( aURL ) : 0, aAlt, aAlt ? strlen( aAlt ) : 0, aOffset, aShowTitle, aShowLink, aUpdatePeriod, aForceCopy ); }


public:
	/**
	 *  Установка флага приоритета для aItemInd-ого элемента.
	 * @todo Нужно совместить с операцией переноса.
	 * @param aItemInd - номер элемента
	 */
	void SetPriority( unsigned int aItemInd );

	/** Обработка приоритетных элементов. */
	InfEngineErrors ProcessPriorityItems();


public:
	/** Получение количества элементов в ответе. */
	unsigned int Size() const
	{ return vItems.size(); }

	/**
	 *  Доступ к aInd-ому элементу ответа.
	 * @param aInd - номер элемента.
	 */
	const Item & At( unsigned int aInd ) const
	{ return vItems[aInd]; }

	/**
	 *  Доступ к aInd-ому элементу ответа.
	 * @param aInd - номер элемента.
	 */
	const Item & operator[]( unsigned int aInd ) const
	{ return vItems[aInd]; }


public:
	/** Проверка на пустоту. */
	bool Empty() const;


public:
	/**
	 *  Получение текста ответа.
	 * @param aResult - контейнер для ответа.
	 * @param aFrom - номер первого элмента, который будет преобразован в текст.
	 * @param aTo - номер первого элемента, который не будет преобразован в текст.
	 */
	InfEngineErrors ToString( aTextString & aResult, unsigned int aFrom, unsigned int aTo ) const;

	/**
	 *  Получение текста ответа.
	 * @param aResult - контейнер для ответа.
	 */
	InfEngineErrors ToString( aTextString & aResult ) const
	{ return ToString( aResult, 0, Size() ); }


public:
	/**
	 *  Копирование ответа.
	 * @param aAnswer - ответ, который будет скопирован.
	 */
	InfEngineErrors Assign( const iAnswer & aAnswer );

	/**
	 *  Копирование ответа.
	 * @param aAnswer - ответ, который будет скопирован.
	 */
	InfEngineErrors operator=( const iAnswer & aAnswer )
	{ return Assign( aAnswer ); }


public:
	/**
	 *  Удаление aItemsNumber элементов с конца.
	 * @param aItemsNumber - количество удаляемых элементов.
	 */
	void PopBack( unsigned int aItemsNumber = 1 )
	{ vItems.resize( vItems.size() >= aItemsNumber ? vItems.size() - aItemsNumber : 0 ); }


public:
	/**
	 *  Изменение флага принудительной непустоты ответа.
	 * @param aFlag - флаг принудительной непустоты ответа.
	 */
	void ForceNotEmpty( bool aFlag = true )
	{ vForceNotEmpty = aFlag; }


public:
	/**
	 *  Изменение флага отключения автоматических переменных.
	 * @param aDisableAutoVars - флаг отключения автоматических переменных.
	 */
	void DisableAutoVars( bool aDisableAutoVars = true )
	{ vDisableAutoVars = aDisableAutoVars; }

	/** Проверка флага отключения автоматических переменных. */
	bool IsAutoVarsEnabled() const
	{ return !vDisableAutoVars; }


public:
	/**
	 *  Установка идентификатора шаблон-ответа.
	 * @param aPatternId - идентификатор шаблона.
	 * @param aAnswerId - идентификатор шаблон-ответа в шаблоне.
	 */
	void SetAnswerId( unsigned int aPatternId, unsigned int aAnswerId )
	{ vPatternId = aPatternId; vAnswerId = aAnswerId; }

	/** Получение идентификатора шаблона. */
	unsigned int GetPatternId() const
	{ return vPatternId; }

	/** Получение идентификатора шаблон-ответа в шаблоне. */
	unsigned int GetAnswerId() const
	{ return vAnswerId; }


private:
	/**
	 *  Выделение памяти для внутренних нужд.
	 * @param aSize - размер выделяемой памяти.
	 */
	void * Allocate( unsigned int aSize )
	{ return vAllocatorPtr ? vAllocatorPtr->Allocate( aSize ) : vInternalAllocator.Allocate( aSize ); }


private:
	/** Внутренний аллокатор памяти. */
	nMemoryAllocator vInternalAllocator = { 10000,10000 };

	/** Ссылка на внешний аллокатор памяти. */
	nMemoryAllocator * vAllocatorPtr = nullptr;

	/** Флаг безусловного копирования данных. */
	bool vFullCopyMode = false;

	/** Массив элементов ответа. */
	avector<Item> vItems;

	/** Идентификатор шаблона. */
	unsigned int vPatternId = 0;

	/** Идентификатор шаблон-ответа. */
	unsigned int vAnswerId = 0;

	/** Флаг, принудительно указывающий, что ответ не считается пустым. */
	bool vForceNotEmpty = false;

	/** Флаг запрета автоматического изменения переменных. */
	bool vDisableAutoVars = false;
};

#endif	/** __iAnswer_hpp__ */
