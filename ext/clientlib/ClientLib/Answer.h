#ifndef __Response_h__
#define __Response_h__

/**
 *  @todo Нужно перейти на использование iAnswer::Item.
 */

#include "ClientLib.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/** Элемент ответа сервера. */
typedef struct
{
	/** Тип элемента ответа сервера. */
	AnswerItemType vType;

	/** Значение элемента. */
	union
	{

		/** Текстовая строка. */
		struct
		{
			/** Текст. */
			char * vText;
			/** Длина текста. */
			unsigned int vTextLength;
		} vString;

		/** Кликабельный ответ инфа. */
		struct
		{
			/** Текст запроса. */
			char * vValue;
			/** Длина текста запроса. */
			unsigned int vValueLength;
			/** Текст запроса, отправляемого движку. */
			char * vRequest;
			/** Длина запроса, отправляемого движку. */
			unsigned int vRequestLength;
		} vInf;

		/** Ссылка. */
		struct
		{
			/** Урл ссылки. */
			char * vURL;
			/** Длина урла ссылки. */
			unsigned int vURLLength;
			/** Текст ссылки. */
			char * vLink;
			/** Длина текста ссылки. */
			unsigned int vLinkLength;
			/** Параметр target для ссылки. */
			char * vTarget;
			/** Длина параметра target для ссылки. */
			unsigned int vTargetLength;
		} vHref;

		/** Инструкция по изменению сессии. */
		struct
		{
			/** Имя переменной. */
			char * vVarName;
			/** Длина имени переменной. */
			unsigned int vVarNameLength;
			/** Присваевоемое значение. */
			char * vValue;
			/** Длина присваевоемого значения. */
			unsigned int vValueLength;
		} vInstruct;

		/** Запрос открытия ссылки. */
		struct
		{
			/** Значение урла ссылки. */
			char * vURL;
			/** Длина значения урла ссылки. */
			unsigned int vURLLength;
			/** Флаг, показывающий нужно ли открывать ссылку в новом окне или же в текущем. */
			unsigned int vParentWindow;
		} vOpenWindow;

		/** Запрос RSS. */
		struct
		{
			/** Значение урла RSS'а. */
			char * vURL;
			/** Длина значения урла RSS'а. */
			unsigned int vURLLength;
			/** Текст, показываемый при недоступности RSS. */
			char * vAlt;
			/** Длина текста, показываемого при недоступности RSS. */
			unsigned int vAltLength;
			/** Номер RSS записи. */
			unsigned int vOffset;
			/** Флаг показа заголовка RSS. */
			unsigned int vShowTitle;
			/** Флаг показа ссылки на RSS. */
			unsigned int vShowLink;
			/** Частота обновления RSS. */
			unsigned int vUpdatePeriod;
		} vRSS;
	} vValue;
} __AnswerItem;

/** Ответ сервера. */
struct __Answer
{
	/** Количество элементов в ответе. **/
	unsigned int vItemsNumber;

	/** Элементы ответа. **/
	__AnswerItem * vItems;
};

#ifdef  __cplusplus
}
#endif

#endif  /** __Response_h__ */
